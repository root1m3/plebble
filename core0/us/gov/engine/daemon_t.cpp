//===-                           P L E B B L E
//===-                         https://plebble.us
//===-
//===-              Copyright (C) 2017-2022 root1m3@plebble.us
//===-
//===-                      GNU GENERAL PUBLIC LICENSE
//===-                       Version 3, 29 June 2007
//===-
//===-    This program is free software: you can redistribute it and/or modify
//===-    it under the terms of the AGPLv3 License as published by the Free
//===-    Software Foundation.
//===-
//===-    This program is distributed in the hope that it will be useful,
//===-    but WITHOUT ANY WARRANTY; without even the implied warranty of
//===-    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//===-
//===-    You should have received a copy of the General Public License
//===-    along with this program, see LICENCE file.
//===-    see https://www.gnu.org/licenses
//===-
//===----------------------------------------------------------------------------
//===-
#include "daemon_t.h"
#include <sys/stat.h>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <tuple>
#include <filesystem>

#include <us/gov/likely.h>
#include <us/gov/io/cfg0.h>
#include <us/gov/io/cfg.h>
#include <us/gov/crypto/base58.h>
#include <us/gov/crypto/ec.h>
#include <us/gov/socket/client.h>
#include <us/gov/engine/auth/app.h>
#include <us/gov/engine/auth/node_address_tx.h>
#include <us/gov/auth/peer_t.h>
#include <us/gov/cash/app.h>
#include <us/gov/traders/app.h>
#include <us/gov/sys/app.h>

#include "protocol.h"
#include "db_analyst.h"
#include "local_deltas_t.h"
#include "counters_t.h"
#include "diff.h"
#include "db_t.h"
#include "db_analyst.h"

#define loglevel "gov/engine"
#define logclass "daemon_t"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::daemon_t;
namespace fs = std::filesystem;

#if CFG_COUNTERS == 1
    counters_t c::counters;
#endif

us::gov::engine::mezzanine::mezzanine(daemon_t* d): d(d), b(bind(&c::run, d), bind(&c::onwakeup, d)) {
}

void c::constructor() {
    running_since = chrono::system_clock::now();
    blocksdir = get_blocksdir(home);
    peerd.fsroot = get_fsrootdir(home);
    io::cfg0::ensure_dir(blocksdir);
    io::cfg0::ensure_dir(peerd.fsroot);
    db = new db_t(*this);
}

c::daemon_t(): //minimum non-functional constructor, used for S_file function
        peerd(*this, 2), //localnet
        sysops(*this),
        syncd(this),
        neuralyzer(this),
        t(this),
        pm(peerd) {
    db = new db_t(*this);
}

c::daemon_t(channel_t channel, const keys_t& k):
        peerd(*this, channel),
        sysops(*this),
        syncd(this),
        id(k),
        neuralyzer(this),
        t(this),
        pm(peerd),
        device_filter([&](const socket::client& peer)->bool { return true; }) {
    constructor();
}

c::daemon_t(channel_t channel, const keys_t& k, const string& home, port_t port, pport_t pport, uint8_t edges, uint8_t devices, int workers, const vector<hostport_t>& seed_nodes, const string& status_file):
        peerd(*this, channel, port, pport, edges, devices, workers, seed_nodes),
        home(home),
        syncd(this),
        sysops(*this),
        id(k),
        neuralyzer(this),
        t(this),
        pm(peerd),
        device_filter([&](const socket::client& peer)->bool { return true; }),
        status_file(status_file) {
    constructor();
}

c::~daemon_t() {
    join();
    delete _local_deltas;
    delete db;
}

us::ko c::start() {
    ko r = ok;
    load_head();
    #if CFG_LOGS == 1
        syncd.logdir = logdir;
        evidence_processor::logdir = logdir;
        pm.logdir = logdir;
        pm.logfile = "pushman-devices";
        peerd.logdir = logdir + "/peerd";
    #endif
    log("protocol goes", (encrypt_protocol ? "encrypted" : "cleartext"));
    peerd.encrypt_traffic = encrypt_protocol;
    log("start net_daemon");
    r = peerd.start();
    if (unlikely(is_ko(r))) {
        return r;
    }
    log("start pushman");
    r = pm.start();
    if (unlikely(is_ko(r))) {
        peerd.stop();
        return r;
    }
    log("starting thread: syncd");
    r = syncd.start();
    if (unlikely(is_ko(r))) {
        peerd.stop();
        pm.stop();
        return r;
    }
    log("starting thread: evidence_processor");
    r = evidence_processor::start();
    if (unlikely(is_ko(r))) {
        peerd.stop();
        pm.stop();
        syncd.stop();
        return r;
    }
    r = t::start();
    if (unlikely(is_ko(r))) {
        peerd.stop();
        pm.stop();
        syncd.stop();
        evidence_processor::stop();
        return r;
    }
    r = wait_ready(chrono::system_clock::now() + seconds(2));
    if (unlikely(is_ko(r))) {
        peerd.stop();
        pm.stop();
        syncd.stop();
        evidence_processor::stop();
        t::stop();
        return r;
    }
    return ok;
}

us::ko c::wait_ready(const time_point& deadline) const {
    ko r = peerd.wait_ready(deadline);
    if (r != ok) {
        return r;
    }
    r = syncd.wait_ready(deadline);
    if (r != ok) {
        return r;
    }
    r = pm.wait_ready(deadline);
    if (r != ok) {
        return r;
    }
    r = evidence_processor::wait_ready(deadline);
    if (r != ok) {
        return r;
    }
    r = t::wait_ready(deadline);
    if (r != ok) {
        return r;
    }
    return ok;
}

ko c::wait_ready(int seconds_deadline) const {
    return wait_ready(chrono::system_clock::now() + chrono::seconds(seconds_deadline));
}

void c::onwakeup() {
    cv_cyc.notify_all();
}

void c::stop() {
    t::stop();
    log("ending threads");
    evidence_processor::stop();
    syncd.stop();
    pm.stop();
    peerd.stop();
}

void c::join() {
    t::join();
    log("waiting for thread: evidence_processor");
    evidence_processor::join();
    log("waiting for thread: syncd");
    syncd.join();
    log("waiting for thread: pushman");
    pm.join();
    log("waiting for thread: net_daemon");
    peerd.join();
}

void c::logline(ostream& os) const {
    votes.logline(os);
}

string c::get_blocksdir(const string& govhome) {
    return govhome + "/blocks";
}

string c::get_fsrootdir(const string& govhome) {
    return govhome + "/fsroot";
}

bool c::patch_db(vector<hash_t>& patches) {
    #if CFG_COUNTERS == 1
        ++counters.patch_db;
    #endif
    lock_guard<mutex> lock(mx_import);
    for (auto i = patches.rbegin(); i != patches.rend(); ++i) {
        const hash_t& hash = *i;
        ostringstream filename;
        filename << blocksdir << '/' << hash;
        pair<ko, blob_reader_t::readable*> r = io::seriable::load(filename.str(), [&](const blob_reader_t::serid_t& serid) -> blob_reader_t::readable* {
                switch (serid) {
                    case diff::serid: return new diff();
                    case db_t::serid: return new db_t(*this);
                }
                return nullptr;
            });
        if (is_ko(r.first)) {
            remove(filename.str().c_str());
            return false;
        }
        {
            diff* d = dynamic_cast<diff*>(r.second);
            if (d != nullptr) {
                if (!import_(*d, hash)) {
                    #if CFG_COUNTERS == 1
                        ++counters.problem_importing_diff_file;
                    #endif
                    delete d;
                    return false;
                }
                delete d;
                continue;
            }
        }
        {
            db_t* d = dynamic_cast<db_t*>(r.second);
            if (d != nullptr) {
                replace_db(hash, d);
                return true;
            }
        }
        assert(false); // this shall be unreachable code
        log("KO 40399 Not a diff nor Snapshot file");
        delete r.second;
        return false;
    }
    #if CFG_COUNTERS == 1
        ++counters.patch_db_ok;
    #endif
    hash_t h;
    if (snapshot(h)) {
        patches.clear();
        patches.emplace_back(h);
    }
    return true;
}

hash_t c::get_last_delta_imported() const {
    lock_guard<mutex> lock(mx_import);
    return last_delta_imported;
}

void c::set_tx_error(ts_t from, ts_t to, const string& err) {
    string progress = evt.set_errorx(from, to, err);
    if (progress.empty()) {
        return;
    }
    pm.schedule_push(blob_writer_t::get_datagram(peerd.channel, protocol::engine_track_response, 0, progress), device_filter);
}

void c::set_tx_status(ts_t from, ts_t to, evt_status_t st) {
    string progress = evt.set_statusx(from, to, st);
    if (progress.empty()) {
        return;
    }
    pm.schedule_push(blob_writer_t::get_datagram(peerd.channel, protocol::engine_track_response, 0, progress), device_filter);
}

void c::set_tx_error(ts_t ts, const string& err) {
    string progress = evt.set_errorx(ts, err);
    if (progress.empty()) {
        return;
    }
    pm.schedule_push(blob_writer_t::get_datagram(peerd.channel, protocol::engine_track_response, 0, progress), device_filter);
}

void c::set_tx_status(ts_t ts, evt_status_t st) {
    string progress = evt.set_statusx(ts, st);
    if (progress.empty()) {
        return;
    }
    pm.schedule_push(blob_writer_t::get_datagram(peerd.channel, protocol::engine_track_response, 0, progress), device_filter);
}

void c::eat_diff(const hash_t& voted_tip, diff* my_tip) {
    log("eat_diff", voted_tip, my_tip);
    using namespace chrono;
    auto tip = voted_tip;
    if (likely(my_tip != nullptr)) {
        auto from = my_tip->id-duration_cast<nanoseconds>(calendar_t::cycle_period).count();
        if (likely(tip == my_tip->hash())) {
            #if CFG_COUNTERS == 1
                ++counters.computed_right_delta;
            #endif
            save(*my_tip);
            unique_lock<mutex>(mx_import);
            if (likely(import_(*my_tip, tip))) {
                snapshot(tip);
                update_dfs_index_delta();
                set_tx_status(from, my_tip->id, evt_settled);
            }
            else {
                clear_();
                log("DB CLEARED");
                set_tx_error(from, my_tip->id, "error 2012");
            }
        }
        else {
            set_tx_error(from, my_tip->id, "fork");
            #ifdef DEBUG
                log("saved unsuccesfull block", my_tip->hash(), tip);
                save(*my_tip);
            #endif
        }
        delete my_tip;
    }
    syncd.update(tip, get_last_delta_imported());
}

void c::load_head() {
    string filename = blocksdir + "/head";
    ifstream is(filename);
    hash_t head(0);
    if (is.good()) is >> head;
    syncd.update(head, get_last_delta_imported());
}

bool c::ready_for_consensus() const {
    if (unlikely(!db->auth_app->is_node())) return false;
    if (unlikely(!syncd.in_sync())) return false;
    return true;
}

bool c::wait_for_secs_to(const seconds& s, ts_t block_deadline) {
    using namespace chrono;
    if (t::reset_wait()) {
        return t::isup();
    }
    mutex mxw;
    unique_lock<mutex> lock(mxw);
    ts_t deadline = block_deadline - duration_cast<nanoseconds>(s).count();
    ts_t now = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
    if (unlikely(now >= deadline)) return true;
    cv_cyc.wait_for(lock, nanoseconds(deadline - now), [&] {
        ts_t now = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
        if (likely(now >= deadline)) {
            return true;
        }
        if (unlikely(t::isdown())) {
            return true;
        }
        return false;
    });
    return t::isup();
}

void c::print_map(ostream& os) const {
    using namespace chrono;
    auto now = system_clock::now();
    auto tp = now.time_since_epoch();
    minutes m = duration_cast<minutes>(tp);
    tp -= m;
    seconds s = duration_cast<seconds>(tp);
    char x[61];
    for (int i = 0; i < s.count(); ++i) {
        x[i] = '>';
    }
    for (int i = s.count(); i < 60; ++i) {
        x[i] = '-';
    }
    x[0] = x[60 - 20] = x[60 - 10] = '*';
    x[60] = '\0';
    os << x;
}

#include <us/gov/socket/multipeer/server.h>
void c::watch(ostream& os) const {
    print_map(os);
    os << "uptime " << uptime() << '\n';
    syncd.dump(os);
    os << '\n';
    #if CFG_COUNTERS == 1
        os << "socket::server::select_cycles " << static_cast<const socket::multipeer::server&>(peerd).counters.select_cycles << '\n';
        os << "  lost_evidences " << counters.lost_evidences << '\n';
        os << "  processed_evidences " << counters.processed_evidences << " last_block " << counters.processed_evidences_last_block << " cur_block " << counters.processed_evidences_cur_block << '\n';
    #endif
    os << "  last_vote " << last_vote << " last_consensus " << last_consensus << '\n';
    votes.dump("", os);
    os << '\n';
    #if CFG_COUNTERS == 1
        silent.dump(os);
        os << '\n';
        db_analyst::print_chain(blocksdir, syncd.tip(), os);
    #endif
    os << '\n';
}

void c::write_status(const string& line) const {
    if (status_file.empty()) return;
    log("writing status", status_file);
    ofstream os(status_file);
    os << line << '\n';
    watch(os);
}

void c::run() {
    using namespace chrono;
    #if CFG_LOGS == 1
        assert(!logdir.empty());
        log_start(logdir, "consensus_engine");
    #endif
    log("consensus cycle period", 60, "secs");
    assert(calendar_t::cycle_period == seconds(60));
    static constexpr chrono::seconds limit_local_deltas{20};
    static constexpr chrono::seconds limit_votes{10};
    while(t::isup()) {
        diff* mydiff{nullptr};
        ts_t from{0};
        ts_t id{0};
        while(true) {
            if (unlikely(!ready_for_consensus())) {
                if (_local_deltas != nullptr) {
                    auto id = _local_deltas->id;
                    auto from = id - duration_cast<nanoseconds>(calendar_t::cycle_period).count();
                    set_tx_error(from, id, "node out of sync.");
                    delete _local_deltas;
                    _local_deltas = nullptr;
                }
                log("not ready for consensus");
                break;
            }
            if (unlikely(_local_deltas == nullptr)) {
                log("local_deltas is null.");
                break;
            }
            log("broadcast local_deltas", _local_deltas->id);
            peerd.send(*_local_deltas);
            id = _local_deltas->id;
            from = id-duration_cast<nanoseconds>(calendar_t::cycle_period).count();
            if (likely(pools.add(_local_deltas, true))) {
                set_tx_status(from, id, evt_mempool);
                write_status("collecting deltas");
                if (unlikely(!wait_for_secs_to(limit_local_deltas, calendar.block_closure))) {
                    log("KO 90594");
                    break;
                }
                log("local_deltas accepted in pool.");
                _local_deltas = nullptr;

                mydiff = pools.close(id); //mydiff.id is the
                assert(mydiff != nullptr);
                mydiff->prev = get_last_delta_imported();
                set_tx_status(from, id, evt_consensus);
                vote_tip(*mydiff);
            }
            else {
                set_tx_error(from, id, "rejected by mempool");
                log("local_deltas not accepted in pool");
                _local_deltas = nullptr;
                delete pools.close(id);
            }
            break;
        }
        log("collecting votes");
        write_status("collecting votes");
        if (unlikely(!wait_for_secs_to(limit_votes, calendar.block_closure))) {
            log("");
            break;
        }
        #if CFG_COUNTERS == 1
        {
            unique_lock<mutex> lock1(db->auth_app->db.mx_nodes, defer_lock), lock2(silent.mx, defer_lock);
            lock(lock1, lock2);
            silent.clear();
            for (auto& i: db->auth_app->db.nodes) {
                if (votes.voters.find(i.first) == votes.voters.end()) {
                    silent.emplace(i.first);
                }
            }
            lock1.unlock();
            lock2.unlock();
        }
        #endif
        last_consensus = votes.select();
        log("most voted tip", last_consensus);
        if (likely(!last_consensus.is_zero())) {
            log("eat diff");
            eat_diff(last_consensus, mydiff);
        }
        else {
            set_tx_error(from, id, "lost. 0 votes");
            log("tip 0");
            delete mydiff;
        }
        mydiff = nullptr;
        #if CFG_COUNTERS == 1
            ++counters.cycles;
            if (counters.cycles % 10 == 0) {
                save_dashboard();
                log("saved dashboard");
            }
        #endif
        log("cycle completed");
        write_status("cycle completed");
        wait_new_cycle();
        log("new cycle");
    }
    log("end");
}

void c::save(const diff& bl) const {
    blob_t blob;
    bl.write(blob);
    ostringstream fn;
    fn << blocksdir << '/' << bl.hash(blob);
    auto finalname = fn.str();
    fn << ".tmp";
    auto tmpname = fn.str();
    auto r = io::write_file_(blob, tmpname);
    if (is_ko(r)) {
        return;
    }
    ::rename(tmpname.c_str(), finalname.c_str());
}

bool c::get_prev_block(const hash_t& h, hash_t& prev) const {
    ostringstream filename;
    filename << blocksdir << '/' << h;
    if (!io::cfg0::file_exists(filename.str())) return false;
    blob_reader_t::blob_header_t header;
    ifstream is(filename.str(), ios::binary);
    is.get(reinterpret_cast<char&>(header.version));
    is.get(reinterpret_cast<char&>(header.serid));
    if (!is.good()) {
        return false;
    }
    if (unlikely(header.version != blob_reader_t::current_version)) {
        log("Blob is not current version.", +header.version, +blob_reader_t::current_version);
        if (header.version == blob_reader_t::current_version - 1) {
            log("Blob is prev version.", +header.version, +blob_reader_t::current_version);
        }
        else {
            log(blob_reader_t::KO_60499, +header.version, +blob_reader_t::current_version);
            return false;
        }
    }
    if (header.serid == diff::serid) {
        is.read((char*)prev.data(), hasher_t::output_size);
        if (!is.good()) {
            return false;
        }
    }
    else {
        prev.zero();
    }
    return true;
}

void c::update_peers_state() {
    peerd.visit([&](socket::client& c)->void {
           auto& p = static_cast<peer_t&>(c);
           if (unlikely(p.stage == peer_t::sysop)) return;
           if (unlikely(p.stage == peer_t::device)) return;
           p.stage = db->auth_app->db.get_stage(p.pubkey.hash());
        });
}

local_deltas_t* c::create_local_deltas(ts_t cycle_id) { //from evidences collected until time cycle_id (not included)
    log("create_local_deltas", cycle_id);
    auto* ld = new local_deltas_t();
    for (auto& i: db->apps) {
        log("create_local_deltas app", i.first);
        app::local_delta* app_ld = i.second->create_local_delta();
        assert(app_ld != nullptr);
        ld->emplace(i.first, app_ld);
    }
    if (ld->empty()) {
        delete ld;
        #if CFG_COUNTERS == 1
            ++counters.created_empty_local_deltas;
        #endif
        return nullptr;
    }
    ld->id = cycle_id;
    ld->sign(id);
    #if CFG_COUNTERS == 1
        ++counters.created_nonempty_local_deltas;
    #endif
    return ld;
}

void c::vote_tip(const diff& b) {
    const hash_t& h = b.hash();
    log("vote for ", h);
    last_vote = h;
    last_consensus = 0;
    auto r = votes.add(id.pub.hash(), h);
    crypto::ec::sig_t sig;
    crypto::ec::instance.sign(id.priv, h, sig);
    log("voting", h);
    peer_t::vote_tip_in_t o_in(h, id.pub, sig);
    peerd.send_vote(o_in.get_datagram(peerd.channel, 0));
}

void c::on_sync() {
    log("on_sync");
    update_dfs_index();
    #if CFG_COUNTERS == 1
        save_dashboard();
    #endif
}

//#define RECORD_EVIDENCES
void c::exec_evidence(const evidence& e) {
    bool x;
    if (e.app == cash::app::id()) {
        log("routing to cash::app");
        x = db->cash_app->process(e);
    }
    else if (e.app == sys::app::id()) {
        log("routing to sys::app");
        x = db->sys_app->process(e);
    }
    else if (e.app == auth::app::id()) {
        log("routing to auth::app");
        x = db->auth_app->process(e);
    }
    else if (e.app == traders::app::id()) {
        log("routing to traders::app");
        x = db->traders_app->process(e);
    }
    else {
        set_tx_error(e.ts, "Unknown processor");
        log("unknown app in evidence", +e.app);
        x = false;
    }
    #if CFG_COUNTERS == 1
        log("processed_evidence", counters.processed_evidences_cur_block, e.ts);
        ++counters.processed_evidences;
        ++counters.processed_evidences_cur_block;
        log("counters.processed_evidences_cur_block", counters.processed_evidences_cur_block);
    #else
        log("processed_evidence", e.ts);
    #endif
    if (likely(x)) {
        set_tx_status(e.ts, evt_mempool);
    }
}

void c::new_evidence(evidence* ev) {
    assert(ev != nullptr);
    auto r = process_evidence2(ev);
    if (is_ko(r)) {
        return;
    }
    auto dgram = ev->get_datagram(peerd.channel, 0);
    assert(dgram != nullptr);
    peerd.relay_evidence(dgram, nullptr);
}

ko c::process_evidence2(evidence* ev) {
    log("process_evidence2");
    assert(ev != nullptr);
    auto ts = ev->ts;
    ko r = calendar.scheduleX(ev); ////0-accepted; 1-invalid-expired; 2-invalid-late; 3-dup;
    #ifdef RECORD_EVIDENCES
        log("WARNING", "RECORDING_EVIDENCES @", LOGDIR, "/evidences");
        ts_t now = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
        ostringstream filename;
        filename << LOGDIR << "/evidences";
        if (is_ko(r)) {
            log("process_evidence", "discarded evidence");
            filename << "_discarded";
        }
        static mutex mx;
        lock_guard<mutex> lock(mx);
        ofstream os(filename.str(),ios::app);
        auto xh = ev->compute_hash();
        auto x = ev->ts;
        os << now << ' ' << x << ' ' << xh << '\n';
    #endif
    if (is_ko(r)) {
        if (r == calendar_t::WP_50451) { //duplicate
        }
        else if (r == calendar_t::KO_50450) {
            set_tx_error(ts, "arrived too late");
        }
        delete ev;
        return r;
    }
    set_tx_status(ts, evt_calendar);
    return ok;
}

int c::call_query_block(const hash_t& hash) {
    log("query_block", hash);
    return peerd.send(2, 0, blob_writer_t::get_datagram(peerd.channel, protocol::engine_query_block, 0, hash));
}

namespace {
    struct genesis_daemon: daemon_t {
        using b = daemon_t;

        genesis_daemon(channel_t channel, const keys_t& keys, const string& home): b(channel, keys, home, 0, 0, 0, 0, 0, vector<hostport_t>(), "") {
        }

        ko init_chain(const string& addrport) {
            log("init_chain", addrport);
            assert(!addrport.empty());
            auto pool = new diff();
            hostport_t a = io::cfg::parse_host(addrport);
            if (!socket::client::is_valid_ip(a.first, peerd.channel)) {
                auto r = "KO 73400 Invalid ip4 address.";
                log(r);
                return r;
            }
            engine::auth::node_address_tx ev(id.pub.hash(), a.first, a.second);
            db->auth_app->process(ev);
            auto* lds = create_local_deltas(calendar.block_closure);
            assert(lds != nullptr);
            auto id = lds->id;
            if (!pools.add(lds, true)) {
                auto r = "KO 64155 error.";
                log(r);
                return r;
            }
            auto genblock = pools.close(id);
            genblock->prev = 0;
            save(*genblock);
            if (!import(*genblock, genblock->hash())) {
                auto r = "KO 52044 Error creating database.";
                log(r);
                return r;
            }
            delete genblock;
            return ok;
        }
    };
}

pair<ko, io::cfg*> c::init_chain(channel_t channel, const string& govhome, const string& addrport) {
    using io::cfg;
    if (addrport.empty()) {
        return make_pair("KO 20548 I need the address of the genesis node.", nullptr);
    }
    if (cfg::dir_exists(govhome + "/blocks")) {
        auto r = "KO 12002 Cannot start a new blockchain if blocks dir exists.";
        log(r);
        return make_pair(r, nullptr);
    }
    if (!cfg::ensure_dir(govhome)) {
        auto r = "KO 42033 Cannot create dir.";
        log(r);
        return make_pair(r, nullptr);
    }
    auto conf = cfg::load(channel, govhome, true);
    if (conf.first != ok) {
        return conf;
    }
    if (!conf.second->keys.pub.valid) {
        delete conf.second;
        auto r = "KO 9403 Invalid node pubkey.";
        log(r);
        return make_pair(r, nullptr);
    }
    genesis_daemon d(channel, conf.second->keys, conf.second->home);
    d.init_chain(addrport);
    return conf;
}

c::evt_data_t c::ev_track_t::track(calendar_t& cal, ts_t ts) {
    lock_guard<mutex> lock(mx);
    if (ts < mints) {
        return evt_data_t(c::evt_untracked, string(evt_status_str[evt_untracked]) + ". Too old."); //make_pair(c::evt_untracked, string(evt_status_str[evt_untracked])+". Too old.");
    }
    auto i = find(ts);
    if (unlikely(i != end())) {
        log("tracking info found", ts);
        return i->second;
    }
    if (cal.has(ts)) {
        log("tracking info created. calendar. ", ts);
        return emplace(ts, evt_data_t(evt_calendar)).first->second;
    }
    else {
        log("tracking info created. waiting arrival. ", ts);
        return emplace(ts, evt_data_t(evt_wait_arrival)).first->second;
    }
}

string c::ev_track_t::set_errorx(ts_t ts, const string& err) {
    lock_guard<mutex> lock(mx);
    auto i = find(ts);
    if (unlikely(i == end())) {
        return "";
    }
    if (i->second.st != evt_error) {
        return "";
    }
    i->second.st = evt_error;
    i->second.info = err;
    ostringstream os;
    os << ts << " 0 " << evt_error << ' ' << evt_status_str[evt_error] << ' ' << err;
    return os.str();
}

string c::ev_track_t::set_statusx(ts_t ts, evt_status_t st) {
    lock_guard<mutex> lock(mx);
    auto i = find(ts);
    if (unlikely(i == end())) {
        return "";
    }
    if (i->second.st == evt_error) {
        return "";
    }
    i->second.st = st;
    i->second.info = c::evt_status_str[st];
    ostringstream os;
    os << ts << " 0 " << st << ' ' << evt_status_str[st];
    return os.str();
}

void c::ev_track_t::dump(ostream& os) const {
    for (auto& i: *this) {
        os << i.first << ' ' << i.second.st << ' ' << i.second.info << '\n';
    }
    os << size() << " tracked evidences.\n";
}

string c::ev_track_t::set_statusx(ts_t from, ts_t to, evt_status_t st) {
    lock_guard<mutex> lock(mx);
    mints = from;
    auto i = begin();
    bool b{false};
    while (i != end()) {
        if (from <= i->first && i->first < to) {
            if (i->second.st != evt_error) {
                i->second.st = st;
                i->second.info = c::evt_status_str[st];
                b = true;
            }
            ++i;
        }
        else {
            if (i->first < from) {
                i = erase(i);
            }
            else {
                ++i;
            }
        }
    }
    if (!b) return "";
    ostringstream os;
    os << from << ' ' << to << ' ' << st << ' ' << evt_status_str[st];
    return os.str();
}

string c::ev_track_t::set_errorx(ts_t from, ts_t to, const string& err) {
    lock_guard<mutex> lock(mx);
    mints = from;
    auto i = begin();
    bool b{false};
    while (i != end()) {
        if (i->first >= from && i->first < to) {
            if (i->second.st != evt_error) {
                i->second.st = evt_error;
                i->second.info = string(evt_status_str[evt_error]) + " " + err;
                b = true;
            }
            ++i;
        }
        else {
            if (i->first<from) {
                i = erase(i);
            }
            else {
                ++i;
            }
        }
    }
    if (!b) return "";
    ostringstream os;
    os << from << ' ' << to << ' ' << evt_error << ' ' << evt_status_str[evt_error] << ' ' << err;
    return os.str();
}

void c::files_to_keep(vector<pair<hash_t, uint32_t>>& v) const {
    if (!syncd.in_sync()) {
        log("not in sync");
        return;
    }
    lock_guard<mutex> lock(mx_import);
    db->cash_app->db.accounts->get_files(v);
}

void c::update_dfs_index_delta() {
    lock_guard<mutex> lock(mx_import);
    for (auto& i: db->cash_app->old_files) {
        peerd.index_rm(i.first);
    }
    peerd.index_set(db->cash_app->new_files);
}

void c::update_dfs_index() {
    log("update_dfs_index");
    vector<pair<hash_t, uint32_t>> v;
    files_to_keep(v);
    log("peerd.index_set", v.size());
    peerd.index_set(v);
}

void c::report_random_node() {
    using namespace chrono;
    peer::peer_t* p = peerd.grid.pick_one();
    if (p == nullptr) return;
    if (!p->is_role_peer()) {
        log("not role peer", p->rolestr[p->role]);
        return;
    }
    if (!db->auth_app->report_node(p->pubkey.hash())) {
        auto d = duration_cast<seconds>(system_clock::now() - p->since);
        if (d > 300s) {
            log("disconection out-for-long node", p->pubkey.hash());
            p->disconnect(0, "Try again by rejoining the network.");
        }
    }
    --p->sendref;
}

void c::on_block_closure(ts_t block_closure) {
    report_random_node();
    _local_deltas = create_local_deltas(block_closure);
    #if CFG_COUNTERS == 1
        counters.processed_evidences_last_block = counters.processed_evidences_cur_block;
        counters.processed_evidences_cur_block = 0;
    #endif
}

bool c::ready_for_block_opening(ts_t block_opening) const {
    log("last_delta_imported_id", db->last_delta_imported_id, "block_opening", block_opening);
    return db->last_delta_imported_id == block_opening;
}

void c::set_last_delta_imported_(const hash_t& h, ts_t id) {
    last_delta_imported = h;
    db->last_delta_imported_id = id;
    string filename = blocksdir + "/head";
    ofstream os(filename);
    os << last_delta_imported << '\n';
}

void c::set_last_delta_imported(const hash_t& h, ts_t id) {
    lock_guard<mutex> lock(mx_import);
    set_last_delta_imported_(h, id);
}

void c::clear() {
    {
        lock_guard<mutex> lock(mx_import);
        clear_();
    }
    peerd.index_clear();
}

void c::clear_() {
    #if CFG_COUNTERS == 1
        ++counters.clear_db;
    #endif
    for (auto& i: db->apps) {
        i.second->clear();
    }
    set_last_delta_imported_(hash_t(0), 0);
    depth = 0;
}

unsigned int c::rng_seed() const {
    if (last_delta_imported.empty()) return 0;
    return *reinterpret_cast<const unsigned int*>(&last_delta_imported);
}

bool c::import(const diff& b, const hash_t&h) {
    lock_guard<mutex> lock(mx_import);
    return import_(b, h);
}

bool c::import_(const diff& b, const hash_t& h) {
    log("import diff", h, "id", b.id);
    #if CFG_COUNTERS == 1
        ++counters.diff_applied;
    #endif
    if (b.prev != last_delta_imported) {
        if (unlikely(b.prev.is_zero())) {
            #if CFG_COUNTERS == 1
                ++counters.genesis_diff_applied;
            #endif
            log("cleared db", h);
            clear_();
        }
        else {
            #if CFG_COUNTERS == 1
                ++counters.diff_discarded_not_in_sequence;
            #endif
            log("KO 91201 Not in sequence.", h);
            return false;
        }
    }
    log("apps");
    for (auto& i: b) {
        auto a = db->apps.find(i.first);
        assert(a != db->apps.end());
        assert(a->second != nullptr);
        log("importing app ", i.first);
        a->second->import(*i.second, b.proof_of_work);
        if (a->second == db->auth_app) {
            log("update_peers_state");
            update_peers_state();
        }
    }
    set_last_delta_imported_(h, b.id);
    ++depth;
    log("updated depth to", depth);
    return true;
}

#if CFG_COUNTERS == 1
    void c::silent_t::dump(ostream& os) const {
        if (empty()) {
            os << "All nodes voted.\n";
        }
        else {
            os << size() << " silent nodes:\n";
            for (auto& i: *this) {
                os << "  " << i << '\n';
            }
        }
    }
#endif

ko c::shell_command(int app_id, istream& is, ostream& os) {
    return db->apps.shell_command(app_id, is, os);
}

void c::list_apps(const string& prefix, ostream& os) const {
    db->apps.dump(prefix, os);
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "Hello from engine::daemon\n";
    os << prefix << "networking:\n";
    peerd.dump(prefix + "  ", os);
}

namespace {
    template <typename Container, typename Fun>
    void tuple_for_each(const Container& c, Fun fun) {
        for (auto& e : c)
            fun(std::get<0>(e), std::get<1>(e), std::get<2>(e));
    }
    string duration_str(chrono::milliseconds time) {
        using namespace std::chrono;
        using T = std::tuple<milliseconds, int, const char *>;
        constexpr T formats[] = {
            T{hours(1), 2, ""},
            T{minutes(1), 2, ":"},
            T{seconds(1), 2, ":"},
        };
        std::ostringstream o;
        tuple_for_each(formats, [&time, &o](auto denominator, auto width, auto separator) {
            o << separator << std::setw(width) << std::setfill('0') << (time / denominator);
            time = time % denominator;
        });
        return o.str();
    }
    string age(const chrono::system_clock::time_point& time) {
            using namespace std::chrono;
            return duration_str(duration_cast<milliseconds>(system_clock::now() - time));
    }
}

string c::uptime() const {
    return age(running_since);
}

bool c::snapshot(hash_t& h) {
    if (depth != 10) {
        return false;
    }
    #if CFG_COUNTERS == 1
        ++counters.snapshot_create;
    #endif
    h = save_db();
    neuralyzer.old_chain(last_delta_imported);
    set_last_delta_imported_(h, db->last_delta_imported_id);
    depth = 0;
    syncd.last_snapshot = h;
    return true;
}

int c::clean_orphaned_blocks() {
    int n = 0;
    set<hash_t> exclude;
    neuralyzer.get_blocks(exclude, last_delta_imported);
    neuralyzer.get_blocks(exclude, neuralyzer.old);
    for (auto& p: fs::directory_iterator(blocksdir)) {
        if (!is_regular_file(p.path())) continue;
        auto f = p.path().filename();
        if (f == "head") continue;
        hash_t hf = hash_t::from_b58(f);
        if (exclude.find(hf) != exclude.end()) continue;
        ::remove(p.path().c_str());
        ++n;
    }
    return n;
}

void c::list_files(ostream& os) const {
    vector<pair<hash_t, uint32_t>> f;
    db->cash_app->get_files(f);
    for (auto& i: f) {
        os << i.first << ' ' << i.second << " bytes\n";
    }
}

int c::clean_unlisted_files() {
    set<hash_t> f;
    db->cash_app->get_files(f);
    return peerd.rm_files(f);
}

void c::replace_db(const hash_t& h, db_t* db_) {
    delete db;
    db = db_;
    set_last_delta_imported_(h, db->last_delta_imported_id);
    depth = 0;
}

ko c::load_db(const string& filename) {
    auto o = new db_t(*this);
    auto r = o->load2(filename);
    if (is_ko(r.first)) {
        return r.first;
    }
    replace_db(r.second.second, o);
    return ok;
}

ko c::load_db(const blob_t& blob) {
    auto o = new db_t(*this);
    auto r = o->read2(blob);
    if (is_ko(r.first)) {
        return r.first;
    }
    //assert(r.second.first.version == io::blob_reader_t::current_version);
    assert(r.second.first.serid == db_t::serid);
    replace_db(r.second.second, o);
    return ok;
}

ko c::load_dbX(const hash_t& h) {
    #if CFG_COUNTERS == 1
        ++counters.load_db;
    #endif
    ostringstream filename;
    filename << blocksdir << '/' << h;
    auto r = load_db(filename.str());
    if (is_ko(r)) {
        return r;
    }
    return ok;
}

hash_t c::save_db() {
    log("save_db");
    blob_t blob;
    db->write(blob);
    string file = blocksdir + "/db";
    auto r = io::write_file_(blob, file);
    if (is_ko(r)) {
        return hash_t(0);
    }
    auto h = hasher_t::digest(blob);
    ostringstream osf;
    osf << blocksdir << '/' << h;
    if (unlikely(::rename(file.c_str(), osf.str().c_str()) != 0)) {
        log("KO 44093 Failed renaming file.");
        #if CFG_COUNTERS == 1
            ++counters.error_renaming_db_file;
        #endif
        h.zero();
        return h;
    }
    return h;
}

void c::fsinfo(bool dot, ostream& os) const {
    fsinfo(blocksdir, dot, os);
}

void c::fsinfo(const string& home, bool dot, ostream& os) {
    db_analyst a(home);
    if (dot)
        a.dot(os);
    else
        a.dump(os);
}

#if CFG_COUNTERS == 1
    #include <us/gov/vcs.h>
    void c::write_status(ostream& os) const {
        if (syncd.in_sync()) {
            int nodes = 0;
            int nodes_waiting = 0;
            {
                lock_guard<mutex> lock(db->auth_app->db.mx_nodes);
                nodes = db->auth_app->db.nodes.size();
            }
            {
                lock_guard<mutex> lock(db->auth_app->db.mx_hall);
                nodes_waiting = db->auth_app->db.hall.size();
            }
            os << "status up\n";
            os << "strength " << nodes << " nodes\n";
            os << "nodes_waiting " << nodes_waiting << '\n';
            int nodes_total = nodes + nodes_waiting;
            os << "nodes_total " << nodes_total << '\n';
            os << "consensus_cycle_period_min 1" << '\n';
            os << "load_tx_cycle 0" << '\n';
        }
        else {
            os << "status syncing\n";
        }
        os << "software_version ";
        us::vcs::version(os);
        os << '\n';
    }

    void c::save_dashboard() const {
        lock_guard<mutex> lock(mx_dashboard);
        ofstream os("status.off");
        write_status(os);
        ::remove("status");
        ::rename("status.off", "status");
    }

    void c::print_performances(ostream& os) const {
        os << "US node " << id.pub << ' ' << id.pub.hash() << '\n';
        os << "uptime " << uptime() << '\n';
        os << "hardware concurrency " << thread::hardware_concurrency() << '\n';
        os << "Counters\n";
        os << "-socket::server:\n";
        gov::socket::multipeer::server::counters.dump(os);
        os << '\n';
        os << "-socket::daemon:\n";
        gov::socket::daemon_t::counters.dump(os);
        os << '\n';
        os << "-socket::datagram:\n";
        gov::socket::datagram::counters.dump(os);
        gov::socket::datagram::counters.dump_bw(running_since,os);
        os << '\n';
        os << "-socket::client:\n";
        gov::socket::client::counters.dump(os);
        os << '\n';
        os << "-auth::peer:\n";
        gov::auth::peer_t::counters.dump(os);
        os << '\n';
        os << "-engine::daemon:\n";
        counters.dump(os);
        os << '\n';
        os << "-engine::auth_app:\n";
        gov::engine::auth::app::counters.dump(os);
        os << '\n';
        os << "-engine::syncd:\n";
        syncd_t::counters.dump(os);
        os << "-crypto::ec\n";
        gov::crypto::ec::counters.dump(os);
        os << '\n';
        os << "-cash::app\n";
        gov::cash::app::counters.dump(os);
        os << '\n';
    }
#endif

pair<ko, evidence*> c::parse_evidence(const blob_t& blob) const { //returns accepted or rejected
    log("parse_evidence");
    return evidence::from_blob(blob);
}

void c::reg::register_arrival(uint64_t r) {
    max = r;
    sum += r;
    ++num;
}

void c::reg::reset() {
    sum = num = max = 0;
}

