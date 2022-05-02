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
#include "hmi.h"
#include <us/gov/engine/daemon_t.h>
#include <us/gov/vcs.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/config.h>
#include <us/gov/engine/protocol.h>
#include <us/gov/engine/db_analyst.h>
#include <us/gov/engine/db_t.h>
#include <us/gov/engine/grid_analyst.h>
#include <us/gov/io/seriable.h>
#include <us/gov/cash/accounts_t.h>
#include <us/gov/cash/app.h>

#define loglevel "gov/cli"
#define logclass "hmi"
#include <us/gov/logs.inc>

using namespace us::gov::cli;
using us::ko;
using c = us::gov::cli::hmi;

const char* c::KO_91809 = "KO 91809 Not an offline command.";
const char* c::KO_10100 = "KO 10100 Could not connect.";
const char* c::KO_11000 = "KO 11000 Invalid command.";

c::hmi(ostream& os): scr(os) {
    log("arguments:");
    constructor();
}

c::hmi(int argc, char** argv, ostream& os): scr(os), p(shell_args(argc, argv)) {
    log("arguments:");
    constructor();
}

c::hmi(const shell_args& a, ostream& os): scr(os), p(a) {
    log("arguments:");
    constructor();
}

c::hmi(const params& p, ostream& os): scr(os), p(p) {
    log("arguments:");
    constructor();
}

void c::constructor() {
    logdump(p.args);
    home = p.get_home_gov();
}

c::~hmi() {
    join();
    delete cfgcli;
    delete cfg;
    assert(rpc_daemon == nullptr);
    assert(daemon == nullptr);
}

ko c::exec_online1(const string& cmdline) {
    log("exec_online1", cmdline);
    if (unlikely(rpc_daemon == nullptr)) {
        auto r = "KO 60594 Not started.";
        log(r);
        return r;
    }
    if (unlikely(rpc_peer == nullptr)) {
        auto r = "KO 60593 peer lost.";
        log(r);
        return r;
    }
    istringstream is(cmdline);
    string cmd;
    is >> cmd;
    string rew;
    if (cmd == "accounts" || cmd == "a") {
        rew = "app 30 db";
    }
    else if (cmd == "files" || cmd == "f") {
        rew = "app 30 files";
    }
    else if (cmd == "syncd") {
        rew = "y";
    }
    else if (cmd == "data") {
        ostringstream cmd0;
        string data;
        is >> data;
        cmd0 << "data " << data;
        rew = cmd0.str();
    }
    else if (cmd == "list_files") {
        ostringstream cmd0;
        string data1;
        is >> data1;
        string data2;
        is >> data2;
        cmd0 << "list_files " << data1 << ' ' << data2;
        rew = cmd0.str();
    }
    else if (cmd == "sysop" || cmd == "-cmd" || cmd == "cmd") {
        getline(is, rew);
    }
    string remote;
    if (!rew.empty()) {
        cmd = "sysop";
        remote = rew;
    }
    log("cmd", cmd, "remote:", remote);
    if (cmd == "sysop") {
        string ans;
        auto r = rpc_peer->call_sysop(remote, ans);
        if (is_ko(r)) {
            return r;
        }
        io::cfg0::trim(ans);
        scr << ans << '\n';
        return ok;
    }
    {
        shell_args args(cmdline);
        string cmd = args.next<string>();
        if (cmd == "track") {
            ts_t ts = args.next<ts_t>(0);
            if (ts == 0) {
                return "KO 58831";
            }
            string ans;
            auto r = rpc_peer->call_track(ts, ans);
            if (is_ko(r)) {
                return r;
            }
            screen::lock_t lock(scr, interactive);
            lock.os << ans << '\n';
            return ok;
        }
        if (cmd == "nodes" || cmd == "n") {
            struct seeds_t: vector<tuple<hash_t, uint32_t, uint16_t>> {
                void dump(ostream& os) const {
                    for (auto &i: *this) {
                        os << get<0>(i) << ' ' << socket::client::endpoint(get<1>(i), get<2>(i)) << '\n';
                    }
                }
            };
            seeds_t seeds;
            auto r = rpc_peer->call_nodes(seeds);
            if (is_ko(r)) {
                return r;
            }
            screen::lock_t lock(scr, interactive);
            seeds.dump(lock.os);
            return ok;
        }
        if (cmd == "lookup_node") {
            hash_t h = args.next<hash_t>(hash_t(0));

            rpc_peer_t::lookup_node_out_dst_t o;
            auto r = rpc_peer->call_lookup_node(h, o);
            if (is_ko(r)) {
                return r;
            }
            screen::lock_t lock(scr, interactive);
            lock.os << socket::client::endpoint(o.net_addr, o.port) << '\n';
            return ok;
        }
        if (cmd == "wallets" || cmd == "w") {
            struct wallets_t: vector<hash_t> {
                void dump(ostream& os) const {
                    for (auto &i: *this) {
                        os << i << '\n';
                    }
                }
            };
            wallets_t wallets;
            auto r = rpc_peer->call_wallets(wallets);
            if (is_ko(r)) {
                return r;
            }
            screen::lock_t lock(scr, interactive);
            wallets.dump(lock.os);
            return ok;
        }
        if (cmd == "lookup_wallet") {
            hash_t h = args.next<hash_t>(hash_t(0));
            rpc_peer_t::lookup_wallet_out_dst_t o;
            auto r = rpc_peer->call_lookup_wallet(h, o);
            if (is_ko(r)) {
                return r;
            }
            screen::lock_t lock(scr, interactive);
            lock.os << socket::client::endpoint(o.net_addr, o.port) << '\n';
            return ok;
        }
        if (cmd == "fetch_accounts") {
            int detailed = args.next<int>(0);
            cash::addresses_t v;
            while (true) {
                hash_t h = args.next<hash_t>(hash_t(0));
                if (h.is_zero()) break;
                v.emplace_back(h);
            }
            cash::accounts_t o;
            {
                auto r = rpc_peer->call_fetch_accounts(v, o);
                if (is_ko(r)) {
                    return r;
                }
            }
            screen::lock_t lock(scr, interactive);
            o.dump("", detailed, lock.os);
            return ok;
        }
        if (cmd == "show") {
            string cmd;
            is >> cmd;
            if (cmd == "c" || cmd == "w") {
                string l;
                ko r = us::gov::io::read_text_file_(CFG_LICENSE_FILE, l);
                if (is_ko(r)) {
                    return r;
                }
                screen::lock_t lock(scr, interactive);
                lock.os << l << '\n';
                return ok;
            }
            auto r = "KO 70693 Invalid command show";
            log(r);
            return r;
        }
    }
    auto r = KO_11000;
    log(r); 
    return r;
}

pair<ko, string> c::exec_cmd(const string& cmd) {
    pair<ko, string> ans;
    ans.first = rpc_peer->call_sysop(cmd, ans.second);
    return move(ans);
}

ko c::exec_online(const string& cmdline) {
    log("exec_online", cmdline);
    if (cmdline.empty()) {
        return "KO 11000 Empty command.";
    }
    return exec_online1(cmdline);
}

ko c::exec_offline(const string& cmdline) {
    shell_args args(cmdline);
    string cmd = args.next<string>();
    if (cmd == "id") {
        print_id();
        return ok;
    }
    if (cmd == "version") {
        scr << us::vcs::version() << '\n';
        return ok;
    }
    if (cmd == "print_chain") {
        string bd = engine::daemon_t::get_blocksdir(home);
        hash_t tip = args.next<hash_t>();
        screen::lock_t lock(scr, interactive);
        engine::db_analyst::print_chain(bd, tip, lock.os);
        return ok;
    }
    if (cmd == "print_matrix") {
        string file = args.next<string>();
        int detail = args.next<int>(0);
        screen::lock_t lock(scr, interactive);
        engine::db_analyst::print_app30db(file, detail, lock.os);
        return ok;
    }
    if (cmd == "print_nodes") {
        string file = args.next<string>();
        screen::lock_t lock(scr, interactive);
        engine::db_analyst::print_app20db(file, lock.os);
        return ok;
    }
    if (cmd == "print_home") {
        scr << p.get_home() << '\n';
        return ok;
    }
    if (cmd == "file_header") {
        string file = args.next<string>();
        us::gov::io::blob_reader_t::blob_header_t hdr;
        ko r = us::gov::io::blob_reader_t::read_header(file, hdr);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << "version " << hdr.version << '\n';
        lock.os << "type '" << hdr.serid << "' 0x" << hex << +hdr.serid << '\n';
        return ok;
    }
    if (cmd == "S_file") {
        string file = args.next<string>();
        int detail = p.args.next<int>(0);
        dump_db(file, detail);
        return ok;
    }
    if (cmd == "D_file") {
        string file = args.next<string>();
        dump_diff(file);
        return ok;
    }
    if (cmd == "init_chain") {
        string addr = args.next<string>();
        return init_chain(addr);
    }
    if (cmd == "ip4_decode") {
        uint32_t n = p.args.next<uint32_t>();
        scr << us::gov::socket::client::ip4_decode(n) << '\n';
        return ok;
    }
    if (cmd == "ba") {
        screen::lock_t lock(scr, interactive);
        engine::daemon_t::fsinfo(engine::daemon_t::get_blocksdir(home), p.dot, lock.os);
        return ok;
    }
    if (cmd == "h" || cmd == "-h" || cmd == "help" || cmd == "-help" || cmd == "--help") {
        screen::lock_t lock(scr, interactive);
        help(p, lock.os);
        return ok;
    }
    if (cmd.empty()) {
        if (interactive) {
            screen::lock_t lock(scr, interactive);
            help(p, lock.os);
            return ok;
        }
    }
    log("not an offline command");
    return KO_91809;
}

ko c::exec(const string& command_line) {
    log("exec line", command_line);
    auto r = exec_offline(command_line);
    if (r == KO_91809) {
        r = exec_online(command_line);
    }
    return r;
}

namespace {
    using us::gov::io::screen;

    struct blhandler_t :us::gov::socket::busyled_t::handler_t {
        blhandler_t(hmi& h, int n): h(h), n(n) {}
        void on_busy() override {
        }
        void on_idle() override {
        }
        int n;
        hmi& h;
    };
}

void c::banner(const params& p, ostream& os) {
    using namespace std::chrono;
    string ind = "    ";
    if (p.daemon) {
        os << PLATFORM << "-gov daemon. Channel " << p.channel << '.';
    }
    else {
        os << PLATFORM << "-gov Introspective Shell. Channel " << p.channel << '.';
    }
    os << " Copyright (C) " << CFG_COPYRIGHT_LINE << '\n';
    os << ind << CFG_COPYRIGHT_LINE2 << '\n';
    os << ind << CFG_COPYRIGHT_LINE3 << '\n';
    os << ind << "version: " << us::vcs::version() << '\n';
    os << ind << "local time: " << duration_cast<nanoseconds>((system_clock::now() - us::gov::engine::calendar_t::relay_interval).time_since_epoch()).count() << " ns since 1/1/1970\n";
    os << ind << "tx time shift: " << duration_cast<seconds>(us::gov::engine::calendar_t::relay_interval).count() << " seconds.\n";
    os << ind << "Build configuration: ";
    #if CFG_LOGS == 1
        os << "[with logs] ";
    #else
        os << "[without logs] ";
    #endif
    #ifdef DEBUG
        os << "[debug build] ";
    #else
        os << "[optimized build] ";
    #endif
    os << '\n';
    os << ind << "Parameters:\n";
    p.dump(ind, os);
}

void c::on_connect(ko r) {
    log("on_connect.");
    if (is_ko(r)) {
        scr << rewrite(r) << '\n';
        return;
    }
}

void c::interactive_shell() {
    interactive = true;
    assert(rpc_daemon != nullptr);
    assert(p.cmd.empty());
    bool was_mute = scr.mute;
    scr.set_mute(false);
    {
        screen::lock_t lock(scr, interactive);
        if (p.banner) {
            banner(p, lock.os);
            lock.os << "\ndevice public key is " << cfgcli->keys.pub << " address " << cfgcli->keys.pub.hash() << '\n';
        }
        lock.os << "Connecting to ";
        p.connecting_to(lock.os);
        lock.os << '\n';
        lock.os << "Type h or help.\n";
    }
    while (rpc_daemon->is_active()) {
        {
            screen::lock_t lock(scr, false);
            lock.os << "> "; lock.os.flush();
        }
        string line;
        getline(cin, line);
        us::gov::io::cfg0::trim(line);
        if (line == "q" || !rpc_daemon->is_active()) {
            scr << "quitting...\n";
            break;
        }
        auto r = exec(line);
        if (is_ko(r)) {
            screen::lock_t lock(scr, false);
            lock.os << r << '\n';
        }
    }
    interactive = false;
    scr.set_mute(was_mute);
}

void c::print_id() const {
    auto r = cfg_daemon::load(p.channel, home, false);
    if (r.first!=ok) {
        scr << r.first << '\n';
        assert(r.second == nullptr);
        return;
    }
    assert(r.second != nullptr);
    scr << "Node public key is " << r.second->keys.pub << " address " << r.second->keys.pub.hash() << '\n';
    delete r.second;
}

ko c::init_chain(const string& addr) {
    pair<ko, io::cfg*> r = engine::daemon_t::init_chain(p.channel, p.get_home_gov(), addr);
    if (is_ko(r.first)) {
        return r.first;
    }
    screen::lock_t lock(scr, interactive);
    lock.os << "Node public key is " << r.second->keys.pub << " address " << r.second->keys.pub.hash() << '\n';
    lock.os << "New blockchain successfully created at " << p.get_home() << '\n';
    return ok;
}

void c::setup_signals(bool on) {
    if (on) {
        signal(SIGPIPE, SIG_IGN);
    }
    else {
        signal(SIGPIPE,SIG_DFL);
    }
}

pair<ko, gov::io::cfg1*> c::load_cfg(const string& home, bool gen) {
    return gov::io::cfg1::load(home, gen);
}

ko c::start_daemon(busyled_t::handler_t* busyled_handler_send, busyled_t::handler_t* busyled_handler_recv, datagram::dispatcher_t* dis) {
    log("start daemon mode");
    assert(dis == nullptr);
    {
        screen::lock_t lock(scr, interactive);
        p.dump(lock.os);
    }
    {
        auto r = cfg_daemon::load(p.channel, home, true);
        if (is_ko(r.first)) {
            return r.first;
        }
        delete cfg;
        cfg = r.second;
    }
    if (p.banner) {
        screen::lock_t lock(scr, interactive);
        banner(p, lock.os);
        lock.os << "\nNode public key is " << cfg->keys.pub << " address " << cfg->keys.pub.hash() << '\n';
    }
    log("node public key", cfg->keys.pub, "address", cfg->keys.pub.hash());
    assert(cfg->keys.pub == gov::crypto::ec::keys::get_pubkey(cfg->keys.priv));
    string stfile = p.get_status_file();
    log("status file", stfile);
    daemon = new engine::daemon_t(p.channel, cfg->keys, cfg->home, p.port, p.pport, p.edges, p.devices, p.workers, cfg->seed_nodes, stfile);
    daemon->sysop_allowed = p.shell;
    log("sysop_allowed", daemon->sysop_allowed);
    if (daemon->sysop_allowed) {
        write_rpc_client_key();
    }
    daemon->encrypt_protocol = false;
    if (daemon->encrypt_protocol && p.channel == 0) {
        log("OVERRIDING CONFIG: clear-text public protocol on channel 0.");
        daemon->encrypt_protocol = false;
    }
    daemon->peerd.force_seeds = p.force_seeds;
    #if CFG_LOGS == 1
        daemon->logdir = logdir + "/daemon";
    #endif
    auto r = daemon->start();
    if (unlikely(is_ko(r))) {
        delete daemon;
        daemon = nullptr;
        return r;
    }
    r = daemon->wait_ready(chrono::system_clock::now() + chrono::seconds(CFG_HMI_READY_TIMEOUT_SECS));
    if (unlikely(is_ko(r))) {
        delete daemon;
        daemon = nullptr;
        return r;
    }
    setup_signals(true);
    return ok;
}

ko c::start_rpc_daemon(busyled_t::handler_t* busyled_handler_send, busyled_t::handler_t* busyled_handler_recv, datagram::dispatcher_t* dis) {
    if (rpc_daemon != nullptr) {
        auto r = "KO 39201 already started";
        log(r);
        return r;
    }
    log("rpc mode", "home", p.get_home_gov_rpc_client());
    using us::gov::io::cfg_id;
    {
        auto r = load_cfg(p.get_home_gov_rpc_client(), false);
        if (is_ko(r.first)) {
            return r.first;
        }
        delete cfgcli;
        cfgcli = r.second;
    }
    log("device public key", cfgcli->keys.pub, "address", cfgcli->keys.pub.hash());
    shostport_t shostport = make_pair(p.sysophost, p.port);
    rpc_daemon = new rpc_daemon_t(*this, cfgcli->keys, shostport, rpc_peer_t::role_sysop, dis);
    rpc_daemon->connect_for_recv = p.rpc__connect_for_recv;
    rpc_daemon->stop_on_disconnection = p.rpc__stop_on_disconnection;
    #if CFG_LOGS == 1
        rpc_daemon->logdir = logdir + "/rpc_daemon";
    #endif
    log("set_busy_handlers send:", (busyled_handler_send == nullptr ? "null" : "not null"), "recv:", (busyled_handler_recv == nullptr ? "null" : "not null"));
    rpc_daemon->set_busy_handlers(busyled_handler_send, busyled_handler_recv);
    {
        auto r = rpc_daemon->start();
        if (unlikely(is_ko(r))) {
            rpc_peer = nullptr;
            delete rpc_daemon;
            rpc_daemon = nullptr;
            return r;
        }
    }
    {
        log("wait_ready");
        auto r = rpc_daemon->wait_ready(chrono::system_clock::now() + chrono::seconds(CFG_HMI_READY_TIMEOUT_SECS));
        if (unlikely(is_ko(r))) {
            rpc_daemon->stop();
            rpc_peer = nullptr;
            delete rpc_daemon;
            rpc_daemon = nullptr;
            return r;
        }
    }
    rpc_peer = static_cast<rpc_peer_t*>(rpc_daemon->peer);
    assert(rpc_peer != nullptr);
    if (rpc_daemon->connect_for_recv) {
        {
            log("wait_connected");
            auto r = rpc_daemon->wait_connected();
            if (is_ko(r)) {
                rpc_daemon->stop();
                rpc_peer = nullptr;
                delete rpc_daemon;
                rpc_daemon = nullptr;
                return r;
            }
        }
        {
            log("waiting for auth handshake to complete");
            auto r = rpc_peer->wait_auth();
            if (unlikely(is_ko(r))) {
                log(r);
                rpc_daemon->stop();
                rpc_peer = nullptr;
                delete rpc_daemon;
                rpc_daemon = nullptr;
                return r;
            }
            log("ready");
        }
    }
    setup_signals(true);
    return ok;
}

us::gov::socket::datagram::dispatcher_t* c::create_dispatcher() {
    return new dispatcher_t(*this);
}

ko c::start() {
    if (p.daemon) {
        return start(nullptr, nullptr, nullptr);
    }
    else {
        return start(create_dispatcher());
    }
}

ko c::start(datagram::dispatcher_t* dispatcher) {
    if (p.daemon) {
        return start(nullptr, nullptr, dispatcher);
    }
    else {
        return start(new blhandler_t(*this, 0), new blhandler_t(*this, 1), dispatcher);
    }
}

ko c::start(busyled_t::handler_t* busyled_handler_send, busyled_t::handler_t* busyled_handler_recv, datagram::dispatcher_t* dis) {
    #if CFG_LOGS == 1
        if (p.verbose) {
            us::dbg::task::verbose = true;
        }
    #endif
    assert(rpc_daemon == nullptr);
    assert(rpc_peer == nullptr);
    assert(daemon == nullptr);
    ko r;
    if (p.daemon) {
        r = start_daemon(busyled_handler_send, busyled_handler_recv, dis);
    }
    else {
        r = start_rpc_daemon(busyled_handler_send, busyled_handler_recv, dis);
    }
    if (is_ko(r)) {
        return r;
    }
    setup_signals(true);
    return ok;
}

void c::stop() {
    setup_signals(false);
    if (daemon != nullptr) {
        daemon->stop();
    }
    if (rpc_daemon != nullptr) {
        rpc_daemon->stop();
    }
}

void c::join() {
    if (daemon != nullptr) {
        log("waiting for daemon");
        daemon->join();
        delete daemon;
        daemon = nullptr;
        assert(cfgcli == nullptr);
        delete cfg;
        cfg = nullptr;
    }
    if (rpc_daemon != nullptr) {
        log("waiting for rpc_daemon");
        rpc_daemon->join();
        delete rpc_daemon;
        rpc_daemon = nullptr;
        assert(cfg == nullptr);
        delete cfgcli;
        cfgcli = nullptr;
    }
}

void c::on_stop() {
    log("on_stop");
}

void c::verification_completed(bool verif_ok) {
    log("verification_completed ", verif_ok);
}

void c::on_I_disconnected(const string& reason) {
    log("I disconnected. Reason: ", reason);
}

void c::on_peer_disconnected(const string& reason) {
    log("peer disconnected. Reason: ", reason);
    screen::lock_t lock(scr, true);
    lock.os << "Peer disconnected with reason: " << reason << '\n';
}

void c::write_rpc_client_key() {
    using us::gov::io::cfg_id;
    string homedir = p.get_home_gov();
    string rpchomedir = p.get_home_gov_rpc_client();
    auto k = cfg_id::load_sk(homedir);
    auto rpck = cfg_id::load_sk(rpchomedir);
    if (k.first == ok && rpck.first != ok) {
         log("writing private key to rpc_client", rpchomedir);
         cfg_id::write_k(rpchomedir, k.second);
    }
}

void c::set_mute(bool m) {
    scr.mute = m;
}

string c::rewrite(ko r) const {
    if (r == KO_11000) {
        return string(r) + "; Type -h for help.";
    }
    if (rpc_daemon) return rpc_daemon->rewrite(r);
    return "";
}

string c::run_() {
    log("run", p.cmd);
    {
        if (!p.cmd.empty()) {
            ko r = exec_offline(p.cmd);
            if (r != KO_91809) {
                return rewrite(r);
            }
        }
    }
    {
        ko r = start();
        if (unlikely(is_ko(r))) {
            return rewrite(r);
        }
    }
    string ret;
    if (!p.daemon) {
        if (p.cmd.empty()) {
            interactive_shell();
        }
        else {
            ko r = exec_online(p.cmd);
            if (is_ko(r)) {
                ret = rewrite(r);
            }
        }
        stop();
    }
    join();
    return ret;
}

string c::run() {
    auto r = run_();
    if (!r.empty()) {
        screen::lock_t lock(scr, false);
        lock.os << r << '\n';
    }
    return r;
}

void c::help(const params& p, ostream& os) {
    if (p.banner) {
        banner(p, os);
    }
    os << "Usage: " << PLATFORM << "-gov [options] [command]\n";
    os << "Options are:\n";
    os << "  -d                 Run daemon \n";
    os << "  -ds                Run daemon with sysop shell. " << boolalpha << p.shell << '\n';
    os << "  -cmd <command>     Forward sysop command to gov process.\n";
    os << "  -fs                Force using seeds. " << boolalpha << p.force_seeds << '\n';
    os << "  -dot               Block analyzer. Output in dot format. " << boolalpha << p.dot << '\n';
    os << "  -lookup            <address>  Resolve IP4 address from pubkeyh.\n";
    os << "  -p <port>          Listening port. " << p.port << '\n';
    os << "  -pp <port>         Published listening port. " << p.pport << '\n';
    os << "  -e <edges>         Max num node neightbours " << (int)p.edges << '\n';
    os << "  -v <edges>         Max num wallet neightbours " << (int)p.devices << '\n';
    os << "  -w <workers>       threadpool size " << (int)p.workers << '\n';
    os << "  -c <channel>       Channel number. " << p.channel << '\n';
//    os << "  -xp <0|1>          Encrypt protocol. " << p.daemon_encrypt_protocol << '\n';
    os << "  -home <homedir>    Set home directory. " << p.get_home() << '\n';
    os << "  -host <address>    daemon host. " << p.sysophost << '\n';
    os << "  -status <0|1>      Write status file. (only with -d or -ds). " << (p.status_file ? p.get_status_file() : "No") << '\n';
    os << "  -om <output_mode>  0:human, 1:text, 2:xml, 3:json. [" << p.get_output_mode() << "]\n";
    os << "  -n                 Prepend field names in output. [" << p.names << "]\n";
    os << "  -nb                Don't show the banner.\n";
    #if CFG_LOGS == 1
    os << "  -log               Logs in console.\n";
    #endif
    os << "Commands are:\n";
    os << '\n';
    os << "offline:\n";
    os << "  id                                  Print node id and exit.\n";
    os << "  version                             Print software version and exit.\n";
    os << "  ip4_decode <uint32>\n";
    os << "  print_chain <tip>                   Print block backtrace.\n";
    os << "  print_matrix <file> <0|1|2>         Print ledger with given level of detail.\n";
    os << "  print_nodes <file>    \n";
    os << "  print_home                          Prints the current working home directory\n";
    os << "  init_chain <IP address>             Creates genesis block.\n";
    os << "  ba                                  Run block analyzer.\n";

    os << "  file_header <file>                  Decodes two first bytes of a file (version and type).\n";
    os << "  D_file <diff_file>                  Print contents of diff file 'D'.\n";
    os << "  S_file <snapshot_file> [<0|1|2>]    Print contents of snapshot file. 'S' [detail level],\n";
    os << "  h|-h|help|-help|--help              This help screen.\n";
    os << '\n';
    os << "online:\n";
    rpc_daemon_t::apihelp("  ", os);
    os << "  sysop <cmd|h>                       Send sysop commands to gov process shell.\n";
    os << "  track <ts>\n";
    os << "  n|nodes\n";
    os << "  lookup_node <hash>\n";
    os << "  wallets\n";
    os << "  lookup_wallet <hash>\n";
    os << "  fetch_accounts <int detail>\n";
    os << "  a|accounts                          List accounts.\n";
    os << "  f|files                             List files.\n";
    os << "  net_status                          Print nodes raw data.\n";
    os << "  syncd                               Print sync daemon info.\n";
    os << "  data <address>                      Print map table stored in the specified address.\n";
    os << "  list_files <address> <path>         Print files in address filtered by path.\n";
    os << "  cmd [<command>]                     Forward sysop command to gov process.\n";
    os << "                                      (def) Enter rpc sysop console to " << PLATFORM << "-gov daemon.\n";
    os << '\n';
}

void c::dump_diff(const string& diff_file) const {
    using diff = us::gov::engine::diff;
    diff o;
    auto r = o.load(diff_file);
    if (is_ko(r)) {
        scr << r << '\n';
        return;
    }
    screen::lock_t lock(scr, interactive);
    o.dump("", lock.os);
}

void c::dump_db(const string& snapshot_block, int detail) const {
    engine::daemon_t d;
    auto r = d.load_db(snapshot_block);
    if (is_ko(r)) {
       scr << r << '\n';
       return;
    }
    screen::lock_t lock(scr, interactive);
    d.db->dump(detail, lock.os);
}

/*
void c::import_v5(const string& snapshot_block) const {
    auto conf = cfg_daemon::load(home, false);
    if (is_ko(conf.first)) {
        scr << conf.first << '\n';
        return;
    }
    {
        engine::daemon_t d(conf.second->keys);
        auto r = d.load_db(snapshot_block);
        if (is_ko(r)) {
           scr << "KO 70694 \n";
           delete conf.second;
           return;
        }
        auto hash = d.save_db();
        screen::lock_t lock(scr, interactive);
        lock.os << hash << '\n';
    }
    delete conf.second;
}
*/
bool c::on_datagram(datagram* d) {
    log("on_datagram", d->service);
    scr << "HMI" << this << ": [D " << d->service << "] KO 79968 Not handled.\n";
    delete d;
    if (scr.is_capturing()) scr.print_prompt();
    return true;
}

c::dispatcher_t::dispatcher_t(c& i): m(i) {
}

bool c::dispatcher_t::dispatch(us::gov::socket::datagram* d) {
    return m.on_datagram(d);
}

void c::process_cleanup() {
    #if CFG_COUNTERS == 1
        us::gov::socket::datagram::counters.clean();
    #endif
}

