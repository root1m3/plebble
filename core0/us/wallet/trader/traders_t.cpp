//===-                           P L E B B L E
//===-                         https://plebble.us
//===-
//===-              Copyright (C) 2017-2022 root1m3@plebble.us
//===-
//===-                      GNU GENERAL PUBLIC LICENSE
//===-                       Version 3, 29 June 2007
//===-
//===-    This program is free software: you can redistribute it and/or modify
//===-    it under the terms of the GPLv3 License as published by the Free
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
#include "traders_t.h"
#include <dlfcn.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <array>
#include <filesystem>

#include <us/gov/config.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/cash/tx_t.h>
#include <us/gov/cash/ttx.h>
#include <us/gov/cash/map_tx.h>
#include <us/gov/cash/file_tx.h>
#include <us/gov/io/cfg0.h>

#include <us/wallet/engine/daemon_t.h>
#include <us/wallet/engine/peer_t.h>
#include <us/wallet/wallet/local_api.h>
#include <us/wallet/trader/r2r/w2w/business.h>

#include "trader_protocol.h"
#include "trader_t.h"
#include "business.h"
#include "businesses_t.h"
#include "bootstrap/initiator_t.h"
#include "bootstrap/follower_t.h"
#include "bootstrap/a_t.h"
#include "bootstrap/b_t.h"
#include "types.h"

#define loglevel "wallet/trader"
#define logclass "traders_t"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::traders_t;

const char* c::KO_39010 = "KO 39010 Command not processed here.";
const char* c::cert_authority_t::KO_77965 = "KO 77965 Personality didn't change.";

//home: .us/chan/wallet/trader"
c::traders_t(wallet::local_api& parent): parent(parent), home(parent.home + "/trader") {
    log("traders_t constructor", home);
    //us::gov::io::cfg0::ensure_dir(home);

    ca.init(home + "/ca");

//    log("loading lf");
//    load_lf();
    load_state();
    log(size(), "active trades");
}

c::~traders_t() {
    join();
    log("destroying traders");
    for (auto& i: *this) {
        assert(i.second->busyref.load() == 0);
        delete i.second;
    }
    clear();
}

c::cert_authority_t::cert_authority_t() {
}

ko c::cert_authority_t::init(const string& home0) {
    log("cert_authority_t::init");
    home = home0;
    us::gov::io::cfg0::ensure_dir(home);
    {
        string oldf = home + "/personality_sk";
        log("todo: remove >= alpha-56 ", oldf); //TODO
        if (us::gov::io::cfg0::file_exists(oldf)) {
            log("delete old file ", oldf);
            ::remove(oldf.c_str());
        }
    }
    auto personality_file = home + "/personality";
    if (!us::gov::io::cfg0::file_exists(personality_file)) {
        log("generating new personality");
        auto p = personality_t::generate("Anonymous CA");
        auto r = p.save(personality_file);
        if (is_ko(r)) {
            return r;
        }
    }
    if (!us::gov::io::cfg0::file_exists(personality_file)) {
        auto r = "KO 40118 personality file not found.";
        log(r, personality_file);
        return r;
    }
    log("load_personality");
    return personality.load(personality_file);
}

ko c::cert_authority_t::reset_personality(const string& sk, const string& moniker) {
    auto r = personality.reset_if_distinct(sk, moniker);
    if (r == 0) {
        log(KO_77965);
        return KO_77965; 
    }
    auto personality_file = home + "/personality";
    {
        auto r = personality.save(personality_file);
        if (is_ko(r)) {
            return r;
        }
    }
    init(home);
    use_in_new_trades = true;
    return ok;
}

ko c::cert_authority_t::store_(const cert_t& o) {
    auto nft = o.nft();
    auto f = nft.filename();
    ostringstream certfile;
    certfile << home << "/cert/" << f.first;
    us::gov::io::cfg0::ensure_dir(certfile.str());
    certfile << '/' << f.second;
    auto r = o.save(certfile.str());
    if (is_ko(r)) {
        log("failed to save cert in", certfile.str());
        return r;
    }
    log("saved cert in", certfile.str());
    return ok;
}

ko c::cert_authority_t::cert_create(string&& msg, cert_t::options&& o, hash_t& nft) {
    log("new cert");
    lock_guard<mutex> lock(mx);
    o.sk = personality.k_sign_docs();
    bool is_root_ca{true};
    o.kv.set("version", "1");
    o.kv.set("CA", "SOA Start of Authority");
    o.kv.set("lang", "en");
    //o.kv.set("CA_types", "SOA [Start of Authority]; PA [Parent Authority] );
    o.text = msg;
    cert_t* doc;
    {
        auto r = cert_t::create(o);
        if (is_ko(r.first)) {
            log(r.first);
            assert(r.second == nullptr);
            return r.first;
        }
        doc = r.second;
        assert(doc != nullptr);
    }
    #ifdef DEBUG
    {
        log("verifying cert");
        ostringstream os;
        auto v = doc->verify(os);
        log("verify cert", v, os.str());
        if (!v) {
            doc->write_pretty(cerr);
            cerr << os.str() << '\n';
            delete doc;
            auto r = "KO 40399 cert does not verify";
            log(r);
            assert(false);
            return r;
        }
    }
    #endif
    auto r = store_(*doc);
    if (is_ko(r)) {
        delete doc;
        return r;
    }

    nft = doc->nft();
    { //registry
        ostringstream regfile;
        regfile << home << '/' << personality.nft();
        us::gov::io::cfg0::ensure_dir(regfile.str());
        regfile << "/certs";
        ofstream os(regfile.str(), ios::app);
        os << nft << '\n';
    }
    return ok;
}

ko c::cert_authority_t::cert_import(cert_t&& cert, hash_t& nft) {
    log("import cert");
    lock_guard<mutex> lock(mx);
    ostringstream os;
    auto v = cert.verify(os);
    log("verify cert", v, os.str());
    assert(v);
    if (!v) {
        auto r = "KO 42399 cert didn't verify";
        log(r);
        //todo: if cert expired -> remove file if exists
        return r;
    }
    log("cert signature is valid.");
    auto r = store_(cert);
    if (is_ko(r)) {
        return r;
    }
    nft = cert.nft();
    return ok;
}

ko c::cert_authority_t::cert_list(uint8_t&& id, cert_index_t& index) {
    if (id == 0) {
        us::gov::io::cfg0::ensure_dir(home + "/cert");
        ostringstream cmd;
        cmd << "find " << home << "/cert -type f -exec basename {} \\;";
        string ans;
        auto r = us::gov::io::system_command(cmd.str(), ans);
        if (is_ko(r)) {
            return r;
        }
        istringstream is(ans);
        index.clear();
        while (is.good()) {
            string line;
            getline(is, line);
            hash_t nft(line);
            if (nft.is_zero()) {
                break;
            }
            index.emplace(move(nft), "cert");
        }
        return ok;
    }
    if (id == 1) {
        log("listing certs issued by me");
        ostringstream regfile;
        regfile << home << '/' << personality.nft() << "/certs";
        ifstream is(regfile.str());
        while (is.good()) {
            string line;
            getline(is, line);
            hash_t nft(line);
            if (nft.is_zero()) {
                break;
            }
            index.emplace(move(nft), "cert");
        }
        return ok;
    }
    auto r = "KO 66739 invalid list id.";
    log(r);
    return r;
}

ko c::cert_authority_t::cert_get(const hash_t& nft, cert_t& cert) {
    lock_guard<mutex> lock(mx);
    auto f = nft.filename();
    ostringstream certfile;
    certfile << home << "/cert/" << f.first << '/' << f.second;
    auto r = cert.load(certfile.str());
    if (is_ko(r)) {
        log(r, "unable to load cert ", certfile.str());
        return r;
    }
    return ok;
}

ko c::cert_authority_t::cert_show(const hash_t& nft, string& s) {
    lock_guard<mutex> lock(mx);
    auto f = nft.filename();
    ostringstream certfile;
    certfile << home << "/cert/" << f.first << '/' << f.second;
    cert_t cert;
    auto r = cert.load(certfile.str());
    if (is_ko(r)) {
        log(r, "unable to load cert ", certfile.str());
        return r;
    }
    ostringstream os;
    cert.write_pretty(os);
    s = os.str();
    return ok;
}

/*
void c::load_lf() {
    namespace fs = std::filesystem;
    string cgidir = home + "/bin";
    us::gov::io::cfg0::ensure_dir(cgidir);
    for (auto& p: fs::directory_iterator(cgidir)) {
        if (!is_regular_file(p.path())) continue;
        lf.emplace(p.path().filename().string());
    }
}
*/

ko c::start() {
    #if CFG_LOGS == 1
        trades_logdir = logdir + "/trades";
    #endif
    lock_guard<mutex> lock(_mx);
    for (auto& i: *this) {
        auto r = i.second->start();
        if (is_ko(r)) {
            stop_();
            return r;
        }
    }
    return ok;
}

ko c::wait_ready(const time_point& deadline) const {
    lock_guard<mutex> lock(_mx);
    for (auto& i: *this) {
        auto r = i.second->wait_ready(deadline);
        if (is_ko(r)) {
            return r;
        }
    }
    return ok;
}

void c::stop_() {
    save_state_();
    log("stopping traders");
    for (auto& i: *this) {
        log("stopping trade ", i.first);
        i.second->stop();
    }
}

void c::stop() {
    lock_guard<mutex> lock(_mx);
    stop_();
}

void c::join() {
    log("join");
    lock_guard<mutex> lock(_mx);
    for (auto& i: *this) {
        log("waiting for trade ", i.first);
        i.second->join();
    }
}

/*
pair<string, string> c::load_personality(const string& file) const {
    log("loading personality from", file);
    ifstream is(file);
    string sk;
    is >> sk;
    string moniker;
    getline(is, moniker);
    if (is.fail()) {
        auto r = "KO 55000 Nothing loaded from file";
        log(r, file);
        return make_pair("11111111111111111111", "anonymous");
    }
    return make_pair(sk, moniker);
}
*/
/*
ko c::load_set_personality(const string& file) {
    auto x = load_personality(file);
    set_personality(x.first, x.second);
    return ok;
}
*/

/*
int c::set_personality(const string& sk, const string& moniker) {
    log("set_personality", moniker);
    return personality.reset_if_distinct(sk, moniker);
}
*/

//pair<ko, hash_t> c::initiate(const hash_t parent_tid, const string& datadir, inverted_qr_t&& inverted_qr, wallet::local_api& w) {
pair<ko, hash_t> c::initiate(const hash_t parent_tid, const string& datadir, inverted_qr_t&& inverted_qr) {
    log("initiate trader", inverted_qr.to_string());
    if (unlikely(inverted_qr.endpoint.chan != parent.daemon.channel)) {
        auto r = "KO 20100 Invalid channel";
        log(r);
        return make_pair(r, hash_t(0));
    }
    if (unlikely(inverted_qr.endpoint.pkh.is_zero())) {
        auto r = "KO 83376 Invalid endpoint address";
        log(r);
        return make_pair(r, hash_t(0));
    }
    if (unlikely(inverted_qr.endpoint == parent.local_endpoint)) {
        auto r = "KO 83316 Trading with myself.";
        log(r);
        return make_pair(r, hash_t(0));
    }
    auto tder = new trader_t(*this, parent.daemon, parent_tid, datadir);
    log("boot with initiator bootstrapper", tder);
    auto r = tder->boot(parent.daemon.id.pub.hash(), new bootstrap::initiator_t(move(inverted_qr), parent));
    if (is_ko(r.first)) {
        return r;
    }
    log("new trade id", r.second);
    lock_guard<mutex> lock(_mx);
    auto e = emplace(r.second, tder);
    assert(e.second);
    return r;
}

void c::erase_trader_(const hash_t& tid) {
    log("Erase trade.", tid);
    trader_t* tder;
    auto i = find(tid);
    if (i == end()) return;
    assert(i->second->busyref.load() == 0);
    log("delete trade.", tid);
    delete i->second;
    erase(i);
}

trader_t* c::lock_trader_(const hash_t& tid) {
    log("lock_trader_");
    auto tder = unlocked_trader_(tid);
    log("++busyref", tid);
    ++tder->busyref;
    return tder;
}

trader_t* c::unlocked_trader_(const hash_t& tid) {
    log("unlocked_trader_");
    trader_t* tder;
    auto i = find(tid);
    if (i == end()) {
        log("new trader", tid);
        auto t = new trader_t(*this, parent.daemon, hash_t(0), "");
        i = emplace(tid, t).first;
    }
    return i->second;
}

ko c::trading_msg(peer_t& peer, svc_t svc, const hash_t& trade_id, blob_t&& blob) {
    log("trading_msg [bin] svc", svc, blob.size(), " bytes");
    if (unlikely(svc < svc_end)) {
        switch(svc) {
            case svc_kill_trade: { //kill must be sent after completing a trading handshake or will fail
                log("svc_kill_trade");
                kill(trade_id, "peer sent message svc_kill_trade");
                return ok;
            }
            break;
            case svc_ko: { // KO Code  TODO: is this dead code?
                log("Received remote KO code:", "tid", trade_id);
                io::blob_reader_t reader(blob);
                string msg;
                auto r = reader.read(msg);
                if (is_ko(r)) {
                    return r;
                }
                log("Received remote KO code msg:", msg, "tid", trade_id);
                //process_svc_ko(trade_id, msg);
                return ok;
            }
            break;
        }
        return trader_protocol::KO_29100;
    }

    {
        switch(svc) {
            case trader_t::svc_handshake_b1:
            case trader_t::svc_handshake_a1:
            case trader_t::svc_handshake_c1: {
                log("resume trade as follower");
                trader_t* tder;
                {
                    lock_guard<mutex> lock(_mx);
                    tder = lock_trader_(trade_id);
                }
                assert(tder != nullptr);
                {
                    log("boot with follower bootstrapper", trade_id, tder);
                    auto r = tder->boot(peer.pubkey.hash(), new bootstrap::follower_t(trade_id, peer));
                    if (is_ko(r.first)) {
                        log("--busyref", trade_id);
                        --tder->busyref;
                        lock_guard<mutex> lock(_mx);
                        erase_trader_(trade_id);
                        return r.first;
                    }
                }
                auto r = tder->trading_msg(peer, svc, move(blob));
                log("--busyref", trade_id);
                --tder->busyref;
                if (is_ko(r)) {
                    log("Oo2", r);
                    lock_guard<mutex> lock(_mx);
                    erase_trader_(trade_id);
                }
                return r;
            }
            break;
        }
    }
    log("looking up trader");
    trader_t* tder;
    {
        lock_guard<mutex> lock(_mx);
        tder = lock_trader_(trade_id);
    }
    if (unlikely(tder == nullptr)) {
        ko r = "KO 40391 Trader not connected.";
        log(r);
        return r;
    }
    auto r = tder->trading_msg(peer, svc, move(blob));
    --tder->busyref;
    return r;
}

void c::dump(ostream& os) const {
    lock_guard<mutex> lock(_mx);
    for (auto& i: *this) {
        os << "trade # " << i.second->id << '\n';
        i.second->dump("  ", os);
    }
}

void c::dump(const string& prefix, ostream& os) const {
    lock_guard<mutex> lock(_mx);
    string pfx = prefix + "  ";
    for (auto& i: *this) {
        os << prefix << "trade # " << i.second->id << '\n';
        i.second->dump(pfx, os);
    }
    os << prefix << size() << " active trades\n";
}

void c::list_trades(ostream& os) const {
    lock_guard<mutex> lock(_mx);
    for (auto& i: *this) {
        assert(i.second->w != nullptr);
        os << i.second->id << ' ';
        i.second->list_trades(os);
        os << '\n';
    }
}

void c::local_functions(ostream& os) const {
}

void c::dispose(trader_t* tder) {
    log("dispose", tder->id);
    thread dispose_task([&, tder]() {
        auto i = tder->id;
        tder->join();
        delete tder;
        });
    dispose_task.detach();
}

void c::on_file_updated2(const string& path, const string& name, const trader_t* source_trader) { //shall be locked on caller
    log("on_file_updated2");
    lock_guard<mutex> lock(_mx);
    for (auto i = begin(); i != end(); ++i) {
        if (i->second == source_trader) continue;
        i->second->on_file_updated(path, name);
    }
    log("end of on_file_updated");
}

void c::on_destroy(peer_t& cli) {
}

ko c::exec_shell(const string& cmd, ostream& os) const {
    std::array<char, 128> buffer;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        os << "KO 9476 - popen() failed!";
        return "KO 94760 Invalid command.";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        os << buffer.data();
    }
    return ok;
}

void c::help(const string& ind, ostream& os) {
    using fmt = us::wallet::trader::trader_protocol;
    auto ind____=ind+"    ";
    os << ind << "Trading help.\n";
    os << ind << "Commands:\n";
    fmt::twocol(ind____, "[R2R Trades]", "----------", os);
    fmt::twocol(ind____, "qr", "Display my QRs", os);
    fmt::twocol(ind____, "save_qr_bookmark <name> <file>", "Save to file (or stdout if no file) a bookmark of me by name", os);
    fmt::twocol(ind____, "list_protocols", "Lists available trading protocols", os);
    fmt::twocol(ind____, "start <node_address> <protocol> <role>", "Initiates a new P2P private trade using endpoint", os);
    fmt::twocol(ind____, "list", "Lists all active trades", os);
    fmt::twocol(ind____, "kill <trade#>", "Terminates the specified trade", os);
    fmt::twocol(ind____, "<trade#> [<subcommand>]", "Enter a shell for the specified trade", os);
    fmt::twocol(ind____, "reload <full-path-doc>", "Tell all active trades the specified file changed on disk", os);
    fmt::twocol(ind____, "wallet", "Exit trading shell going back to wallet shell", os);
    os << '\n';
    fmt::twocol(ind____, "[Service API]", "----------", os);
    fmt::twocol(ind____, "exec [<cmd>]", "Exec command. Empty cmd for help", os);
    os << '\n';
    fmt::twocol(ind____, "[World]", "----------", os);
    fmt::twocol(ind____, "world", "Lists remote wallets", os);
    fmt::twocol(ind____, "bookmarks [file]", "Print my bookmarks (or those in input file).", os);
    fmt::twocol(ind____, "bookmarks_append <dstfile> <srcfile>", "Append bookmarks in srcfile into bookmarks file dstfile.", os);
    fmt::twocol(ind____, "bookmarks_rename <file> <old-name> <new-name>", "Rename entry in bookmarks file.", os);


    fmt::twocol(ind____, "save_bookmark <name> <file>", "Append to file (or stdout) a bookmark by name ", os);
    fmt::twocol(ind____, "load_bookmark <file> ", "Load ", os);
    os << '\n';
}

void c::list_files_dir(const string& prefix, const string& dir, ostream& os) {
    namespace fs = std::filesystem;
    for (auto& p: fs::directory_iterator(dir)) {
        if (!is_regular_file(p.path())) continue;
        os << prefix << p.path().filename().string() << '\n';
    }
}

ko c::exec(const hash_t& tid, const string& cmd) {
    log("exec", tid, cmd);
    trader_t* tder;
    {
        lock_guard<mutex> lock(_mx);
        log("finding trade", tid, "among", size(), "traders");
        auto i = find(tid);
        if (i == end()) {
            auto r = "KO 15322 Trade not found.";
            parent.push_KO(tid, r);
            log(r, tid);
            return r;
        }
        tder = i->second;
    }
    log("found trader", tder);
    if (tder->w != &parent) {
        auto r = "KO 15323 Trader is operating an unexpected wallet.";
        parent.push_KO(tid, r);
        log(r, tid, tder->w, &parent);
        return r;
    }
    tder->schedule_exec(cmd);
    return ok;
}

void c::exec_help(const string& prefix, ostream& os) const {
    log("exec_help");
    os << prefix << " personality set <secret_key> <moniker>     Set initial personality for starting new trades.";
    os << "  * Current personality used: ";
    ca.personality.one_liner(os);
    os << '\n';
}

ko c::exec(istream& is) {
    log("exec is");
    auto g = is.tellg();
    string cmd;
    is >> cmd;
    if (cmd.empty()) {
        ostringstream os;
        exec_help("", os);
        parent.push_OK(hash_t(0), os.str());
        return ok;
    }
    ostringstream os;
    if (cmd == "personality") {
        string cmd;
        is >> cmd;
        if (cmd == "set") {
            string sk;
            string moniker;
            is >> sk;
            getline(is, moniker);
            us::gov::io::cfg0::trim(moniker);
            if (is.fail()) {
                auto r = "KO 89979";
                log(r);
                return r;
            }
            auto r = ca.reset_personality(sk, moniker);
            if (is_ko(r)) {
                if (r == cert_authority_t::KO_77965) {
                    return parent.push_OK(hash_t(0), "Personality didn't change.");
                }
                return r;
            }
            ostringstream os;
            os << "New trades will be initiated using personality ";
            ca.personality.one_liner(os);
            os << '.';
            return parent.push_OK(hash_t(0), os.str());
        }
        auto r = "KO 50399 Invalid personality command.";
        log(r);
        return r;
    }
    is.seekg(g);
    return KO_39010;
}

void c::reload_file(const string& fqn) {
    log("reload_file", fqn);
    auto r = us::gov::io::cfg0::split_fqn(fqn);
    if (is_ko(r.first)) {
        return;
    }
    if (r.second.second.empty()) {
        auto r = "KO 74477 Path to a directory given.";
        log(r);
        return;
    }
    on_file_updated2(r.second.first, r.second.second, nullptr);
}

size_t c::blob_size() const {
    auto sz = blob_writer_t::sizet_size(size()) + size() * us::gov::crypto::ripemd160::output_size;
    //for (auto& i: *this) {
    //    sz += blob_writer_t::blob_size(i.second->w->subhome);
    //}
    log("blob_size", sz);
    return sz;
}

void c::to_blob(blob_writer_t& writer) const {
    auto n = size();
    log("to_blob", "cur", (uint64_t)(writer.cur - writer.blob.data()), "list size", n);
    writer.write_sizet(n);
    for (auto& i: *this) {
        writer.write(i.first);
        //writer.write(parent.subhome);
    }
}

ko c::from_blob(blob_reader_t& reader) {
    log("from_blob", "cur", (uint64_t)(reader.cur - reader.blob.data()));
    uint64_t sz;
    auto r = reader.read_sizet(sz);
    if (is_ko(r)) return r;
    if (unlikely(sz > blob_reader_t::max_sizet_containers)) return blob_reader_t::KO_75643;
    for (int i = 0; i < sz; ++i) {
        log("loading trade #", i);
        hash_t tid;
        {
            auto r = reader.read(tid);
            if (is_ko(r)) return r;
            log("read tid", tid);
        }
/*
        string subhome;
        {
            auto r = reader.read(subhome);
            if (is_ko(r)) return r;
            log("read subhome", subhome);
        }
*/
        {
            log("boot with no bootstrapper", tid);
/*
            auto w = daemon.users.get_wallet(subhome);
            if (w == nullptr) {
                auto r = "KO 87868 wallet could not be instantiated.";
                log(r, "subhome", subhome);
                continue;
            }
  */
            trader_t* tder = lock_trader_(tid);
            assert(tder != nullptr);
            auto r = tder->boot(tid, parent);
            log("--busyref", tid);
            --tder->busyref;
            if (is_ko(r.first)) {
                log("tid", tid);
                erase_trader_(tid);
                continue;
            }
        }
    }
    return ok;
}

string c::sername() const {
    return home + "/active";
}

void c::load_state_() {
    log("loading active trades");
    auto r = load(sername());
    if (is_ko(r)) {
        log("active trades could not be loaded", sername());
    }
}

void c::load_state() {
    lock_guard<mutex> lock(_mx);
    load_state_();
}

void c::save_state_() const { //assume mx is taken
    log("saving active trades - SKIPPED");
    //auto r = save(sername() + "_off"); // TODO: re-enable trades persistence
    //if (is_ko(r)) {
    //    log("active trades could not be saved", sername());
    //}
}

void c::save_state() const { //assume mx is taken
    lock_guard<mutex> lock(_mx);
    save_state_();
}

void c::kill(const hash_t& tid, const string& source) {
    log("spawned assassin thread");
    thread killwork([&, tid, source]() {
        #if CFG_LOGS == 1
        {
            ostringstream os;
            os << "assassin_trade_" << tid;
            log_start("assassin", os.str());
        }
        #endif
        log("source", source);
        trader_t* o;
        {
            lock_guard<mutex> lock(_mx);
            auto i = find(tid);
            if (i == end()) {
                auto r = "KO 40398 Trade not found";
                log(r);
                return;
            }
            o = i->second;
            erase(i); //take out so no new jobs are executed
        }
        o->die(string("killed by: ") + source);
    });
    killwork.detach();
}

