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
#include <us/gov/io/cfg.h>
#include <us/gov/cli/hmi.h>
#include <us/gov/crypto/base58.h>
#include <us/gov/io/cfg1.h>
#include <us/gov/io/seriable.h>
#include <us/gov/cash/sigcode_t.h>
#include <us/gov/vcs.h>
#include <us/gov/config.h>

#include <us/wallet/engine/daemon_t.h>
#include <us/wallet/wallet/algorithm.h>
#include <us/wallet/wallet/local_api.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/traders_t.h>
#include <us/wallet/trader/trader_protocol.h>
#include <us/wallet/trader/workflow/trader_protocol.h>
#include <us/wallet/trader/qr_t.h>
#include <us/wallet/trader/r2r/w2w/protocol.h>

#include <us/wallet/protocol.h>
#include "types.h"

#define loglevel "wallet/cli"
#define logclass "hmi"
#include <us/gov/logs.inc>

using namespace us::wallet::cli;
using c = us::wallet::cli::hmi;

const char* c::KO_11000 = "KO 11000 Invalid command.";
const char* c::KO_10020 = "KO 10020 Feature not available in RPC mode.";
const char* c::KO_10021 = "KO 10021 Feature only available in RPC mode.";
const char* c::KO_91810 = "KO 91810 Not an offline command.";
const char* c::KO_40322 = "KO 40322 Not implemented.";

c::hmi(ostream& os): scr(os) {
    constructor();
}

c::hmi(int argc, char** argv, ostream&os): scr(os), p(shell_args(argc, argv)) {
    log("arguments:");
    constructor();
}

c::hmi(const params& p, ostream& os): scr(os), p(p) {
    log("arguments:");
    constructor();
}

c::hmi(const shell_args& a, ostream& os): scr(os), p(a) {
    log("arguments:");
    constructor();
}

void c::constructor() {
    logdump(p.args);
    home = p.get_home_wallet();
}

c::~hmi() {
    join();
    delete daemon;
    delete rpc_daemon;
    delete cfg;
    #if CFG_FCGI == 1
        delete fcgi;
    #endif
}

void c::print_id() const {
    auto r = load_cfg(home, true);
    if (is_ko(r.first)) {
        log(r.first);
        return;
    }
    auto cfg = r.second;
    scr << "Wallet public key is " << cfg->keys.pub << " address " << cfg->keys.pub.hash() << '\n';
    delete cfg;
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

void c::setup_signals(bool on) {
    if (on) {
        signal(SIGPIPE, SIG_IGN);
    }
    else {
        signal(SIGPIPE, SIG_DFL);
    }
}

pair<ko, gov::io::cfg1*> c::load_cfg(const string& home, bool gen) const {
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
        auto r = load_cfg(home, true);
        if (is_ko(r.first)) {
            log(r.first);
            return r.first;
        }
        delete cfg;
        cfg = r.second;
    }
    if (p.banner) {
        screen::lock_t lock(scr, interactive);
        banner(p, lock.os);
        lock.os << "\nWallet public key is " << cfg->keys.pub << " address " << cfg->keys.pub.hash() << '\n';
    }
    log("wallet public key", cfg->keys.pub, "address", cfg->keys.pub.hash());
    assert(cfg->keys.pub == gov::crypto::ec::keys::get_pubkey(cfg->keys.priv));
    pport_t pport = 0;
    assert(daemon == nullptr);
    assert(w == nullptr);
    shostport_t gov_backend = make_pair(p.backend_host, p.backend_port);
    daemon = new engine::daemon_t(p.channel, cfg->keys, p.listening_port, p.published_port, home, gov_backend, p.max_trade_edges, p.max_devices, p.workers, p.downloads_dir);
    daemon->encrypt_traffic = true;
    #if CFG_LOGS == 1
        assert(!logdir.empty());
        daemon->logdir = logdir + "/daemon";
    #endif
    log("instantiate root wallet");
    log("set_busy_handlers send:", (busyled_handler_send == nullptr ? "null" : "not null"), "recv:", (busyled_handler_recv == nullptr ? "null" : "not null"));
    daemon->set_busy_handlers(busyled_handler_send, busyled_handler_recv);
    log("starting daemon");
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
    string subhome;
    log("selecting wallet. subhome =", subhome);
    w = daemon->users.get_wallet(subhome);
    { /// Import gov key
        auto k = gov::io::cfg1::load_sk(p.get_home_gov());
        if (is_ko(k.first)) {
            log("gov key is not available at", p.get_home_gov());
        }
        else {
            log("adding gov key");
            w->add_address(k.second);
        }
    }
    cerr << "DBG STARTED-WALLET_DAEMON." << endl;
    #if CFG_FCGI == 1
        if (p.fcgi) {
            cerr << "DBG run_fcgi" << endl;
            fcgi_t::api = daemon;
            run_fcgi(p);
        }
        else {
            cerr << "DBG not run_fcgi" << endl;
        }
    #else
        cerr << "DBG def CFG_FCGI " << CFG_FCGI << endl;
    #endif
    return ok;
}

ko c::start_rpc_daemon(busyled_t::handler_t* busyled_handler_send, busyled_t::handler_t* busyled_handler_recv, datagram::dispatcher_t* dis) {
    log("stating rpc_daemon");
    if (rpc_daemon != nullptr) {
        auto r = "KO 39201 already started";
        log(r);
        return r;
    }
    log("rpc mode", "home", p.get_home_wallet_rpc_client());
    {
        auto r = load_cfg(p.get_home_wallet_rpc_client(), true);
        if (is_ko(r.first)) {
            return r.first;
        }
        delete cfg;
        cfg = r.second;
    }
    shostport_t shostport = make_pair(p.walletd_host, p.walletd_port);
    rpc_daemon = new rpc_daemon_t(*this, cfg->keys, shostport, rpc_peer_t::role_device, dis);
    rpc_daemon->connect_for_recv = p.rpc__connect_for_recv;
    rpc_daemon->stop_on_disconnection = p.rpc__stop_on_disconnection;
    #if CFG_LOGS == 1
        assert(!logdir.empty());
        rpc_daemon->logdir = logdir + "/rpc_daemon";
    #endif
    log("set_busy_handlers send:", (busyled_handler_send == nullptr ? "null" : "not null"), "recv:", (busyled_handler_recv == nullptr ? "null" : "not null"));
    rpc_daemon->set_busy_handlers(busyled_handler_send, busyled_handler_recv);
    {
        auto r = rpc_daemon->start();
        if (unlikely(is_ko(r))) {
            delete rpc_daemon;
            rpc_daemon = nullptr;
            return r;
        }
    }
    {
        auto r = rpc_daemon->wait_ready(CFG_HMI_READY_TIMEOUT_SECS);
        if (unlikely(is_ko(r))) {
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
        }
    }
    else {
        log("rpc_daemon->connect_for_recv is 0. Not waiting for auth");
    }
    return ok;
}

ko c::start(busyled_t::handler_t* busyled_handler_send, busyled_t::handler_t* busyled_handler_recv, datagram::dispatcher_t* dis) {
    log("start");
    #if CFG_LOGS == 1
        if (p.verbose) {
            us::dbg::task::verbose = true;
        }
        if (p.nolog) {
            us::dbg::thread_logger::instance.disabled = true;
        }
        logdir = p.logd;
    #endif
    #if CFG_FCGI == 1
        if (!p.daemon && p.fcgi) {
            auto r = "KO 78667 -fcgi requires -d";
            log(r);
            return r;
        }
    #endif
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

ko c::start(datagram::dispatcher_t* dispatcher) {
    log("start");
    if (p.daemon) {
        return start(nullptr, nullptr, dispatcher);
    }
    else {
        return start(new blhandler_t(*this, 0), new blhandler_t(*this, 1), dispatcher);
    }
}

ko c::start() {
    log("start");
    if (p.daemon) {
        return start(nullptr, nullptr, nullptr);
    }
    else {
        return start(create_dispatcher());
    }
}

void c::on_connect(ko r) {
    log("on_connect.");
    if (is_ko(r)) {
        scr << rewrite(r) << '\n';
        return;
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

void c::stop() {
    log("stop");
    setup_signals(false);
    #if CFG_FCGI == 1
        if (fcgi) {
            log("bringing down fcgi");
            fcgi->terminate();
            this_thread::sleep_for(1s);
            if (fcgi) {
                tee("KO 70695 after terminating, fcgi is expected to leave a null pointer here.");
                assert(false);
            }
        }
    #endif
    if (daemon != nullptr) {
        daemon->stop();
    }
    if (rpc_daemon != nullptr) {
        rpc_daemon->stop();
    }
}

void c::join() {
    if (daemon != nullptr) {
        daemon->join();
        w = nullptr;
        delete daemon;
        daemon = nullptr;
    }
    if (rpc_daemon != nullptr) {
        rpc_daemon->join();
        delete rpc_daemon;
        rpc_daemon = nullptr;
    }
    delete cfg;
    cfg = nullptr;
}

void c::new_trade(const hash_t& tid) {
    if (cur.is_not_zero()) {
        return;
    }
    enter_trade(tid);
}

void c::enter_trade(const hash_t& tid) {
    if (interactive) {
        screen::lock_t lock(scr, true);
        lock.os << "Entering trade " << tid << '\n';
    }
    cur = tid;
    curpro = "";
    {
        lock_guard<mutex> lock(data.mx);
        data.clear();
    }
    {
        lock_guard<mutex> lock(roles.mx);
        roles.clear();
    }
}

void c::leave_trade() {
    if (interactive) {
        screen::lock_t lock(scr, true);
        lock.os << "Leaving trade " << cur << '\n';
    }
    cur = hash_t(0);
    curpro = "";
    {
        lock_guard<mutex> lock(data.mx);
        data.clear();
    }
    {
        lock_guard<mutex> lock(roles.mx);
        roles.clear();
    }
}

void c::leave_trade(const hash_t& tid) {
    if (cur != tid) return;
    leave_trade();
}

bool c::trade_interactive(shell_args& args, const hash_t& tid) { //return: exit parent loop
    log("trade. interactive");
    scr << "Trading shell. Trade " << tid << '\n';
    string line = args.next_line();
    enter_trade(tid);
    //cur = tid;
    //curpro = "";
    //data = data_t();
    //roles = roles_t();
    bool ret;
    while (rpc_daemon->is_active()) {
        log("wapi.exec_trade", tid, line);
        if (line == "trade" || line == "exit" || line == "q") {
            ret = true;
            break;
        }
        if (line == "wallet") {
            ret = false;
            break;
        }
        auto r = rpc_daemon->get_peer().call_exec_trade(rpc_peer_t::exec_trade_in_t(tid, line));
        if (is_ko(r)) {
            scr << rewrite(r) << '\n';
        }
        scr.flush();
        { /// Set prompt
            ostringstream os;
            if (cur.is_zero()) {
                os << "trade";
            }
            else {
                os << cur;
            }
            lock_guard<mutex> lock(data.mx);
            auto i = data.find("protocol");
            if (i != data.end()) {
                curpro = i->second;
                if (curpro == "not set") {
                    curpro = "";
                }
                us::gov::io::cfg0::trim(curpro);
                if (!curpro.empty()) {
                    os << '-' << curpro;
                }
            }
            scr.prompt = os.str();
        }
        line = scr.capture_input(*rpc_daemon);
        if (!rpc_daemon->is_active()) {
            ret = false;
            break;
        }
    }
    leave_trade();
    return ret;
}

bool c::exec_trade_global(const string& command, shell_args& args) {
    log("trade. non-interactive");
    log("trade command", command);
    if (command == "qr") {
        bookmarks_t b;
        auto r = rpc_daemon->get_peer().call_qr(b);
        if (is_ko(r)) {
            scr << r << '\n';
            return true;
        }
        screen::lock_t lock(scr, interactive);
        b.dump("", lock.os);
        return true;
    }
    if (command == "list_protocols") {
        string data;
        rpc_daemon->get_peer().call_list_protocols(data);
        scr << data << '\n';
        return true;
    }
    if (command == "start") {
        using qr_t = us::wallet::trader::qr_t;

        auto line = args.next_line();
        qr_t qr;
        auto e = qr.from(line);
        if (is_ko(e)) {
            scr << e << '\n';
            return true;
        }
        log("start trade", qr.to_string(), "using datasubdir", datasubdir);
        blob_t blob;
        qr.write(blob);
        hash_t tid;
        auto r = rpc_daemon->get_peer().call_trade(rpc_peer_t::trade_in_t(hash_t(0), datasubdir, blob), tid);
        if (is_ko(r)) {
            auto s = rpc_daemon->get_peer().lasterror;
            if (s.empty()) {
                scr << r << '\n';
            }
            else {
                scr << s << '\n';
            }
        }
        scr << tid << '\n';
        return true;
    }
    if (command == "save_qr_bookmark") {
        string name = args.next<string>();
        if (name.empty()) {
            auto r = "KO 93772 Bookmark name.";
            log(r);
            scr << r << '\n';
            return true;
        }
        bookmarks_t bm;
        auto r = rpc_daemon->get_peer().call_qr(bm);
        if (is_ko(r)) {
            scr << r << '\n';
            return true;
        }
        auto i = bm.find(name);
        if (i == bm.end()) {
            auto r = "KO 93722 not found.";
            log(r);
            scr << r << '\n';
            return true;
        }
        string file = args.next<string>();
        if (file.empty()) {
            screen::lock_t lock(scr, interactive);
            lock.os << i->second.encode() << '\n'; 
            return true;
        }
        bookmarks_t bm2;
        bm2.load(file);
        bm2.add("bookmark", i->second);
        {
            auto r = bm2.save(file);
            if (is_ko(r)) {
                scr << r << '\n';
                return true;
            }
        }
        return true;
    }
    if (command == "bookmarks") {
        bookmarks_t bm;
        string file = args.next<string>();
        if (file.empty()) {
            auto r = rpc_daemon->get_peer().call_bookmark_list(bm);
            if (is_ko(r)) {
                scr << r << '\n';
                return true;
            }
        }
        else {
            auto r = bm.load(file);
            if (is_ko(r)) {
                scr << r << '\n';
                return true;
            }
        }
        screen::lock_t lock(scr, interactive);
        bm.dump("", lock.os);
        return true;
    }
    if (command == "save_bookmark") {
        string name = args.next<string>();
        if (name.empty()) {
            auto r = "KO 93772 Bookmark name.";
            log(r);
            scr << r << '\n';
            return true;
        }
        bookmarks_t bm;
        {
            auto r = rpc_daemon->get_peer().call_bookmark_list(bm);
            if (is_ko(r)) {
                scr << r << '\n';
                return true;
            }
        }
        auto i = bm.find(name);
        if (i == bm.end()) {
            auto r = "KO 93721 not found.";
            log(r);
            screen::lock_t lock(scr, interactive);
            bm.dump("> ", lock.os); 
            lock.os << r << " >" << name << "<\n";
            return true;
        }
        string file = args.next<string>();
        if (file.empty()) {
            screen::lock_t lock(scr, interactive);
            lock.os << i->second.encode() << '\n'; 
            return true;
        }
        bookmarks_t bm2;
        {
            auto r = bm2.load(file);
        }
        bm2.add("bookmark", i->second);
        {
            auto r = bm2.save(file);
            if (is_ko(r)) {
                scr << r << '\n';
                return true;
            }
        }
        return true;
    }
    if (command == "bookmarks_append") {
        string dstfile = args.next<string>();
        string srcfile = args.next<string>();
        bookmarks_t dst;
        {
            auto r = dst.load(dstfile);
            if (is_ko(r)) {
                scr << r << '\n';
                return true;
            }
        }
        bookmarks_t src;
        {
            auto r = src.load(srcfile);
            if (is_ko(r)) {
                scr << r << '\n';
                return true;
            }
        }
        dst.add(src);
        {
            auto r = dst.save(dstfile);
            if (is_ko(r)) {
                scr << r << '\n';
                return true;
            }
        }
        return true;
    }
    if (command == "bookmarks_rename") {
        string dstfile = args.next<string>();
        string oldname = args.next<string>();
        string newname = args.next<string>();
        bookmarks_t dst;
        {
            auto r = dst.load(dstfile);
            if (is_ko(r)) {
                scr << r << '\n';
                return true;
            }
        }
        auto i = dst.find(oldname);
        if (i == dst.end()) {
            auto r = "KO 87998 not found";
            scr << r << '\n';
            return true;
        }
        auto b = i->second;
        dst.erase(i);
        dst.add(newname, b);
        {
            auto r = dst.save(dstfile);
            if (is_ko(r)) {
                scr << r << '\n';
                return true;
            }
        }
        return true;
    }
    if (command == "list") {
        string data;
        auto r = rpc_daemon->get_peer().call_list_trades(data);
        if (is_ko(r)) {
            scr << r << '\n';
            return true;
        }
        scr << data << '\n';
        return true;
    }
    if (command == "world") {
        vector<hash_t> wallets;
        auto r = rpc_daemon->get_peer().call_world(wallets);
        if (is_ko(r)) {
            scr << r << '\n';
            return true;
        }
        hash_t me(0);
        if (cfg != nullptr) {
            me = cfg->keys.pub.hash();
            scr << "me: " << me << '\n';
        }
        screen::lock_t lock(scr, interactive);
        for (auto& w: wallets) {
            lock.os << rpc_daemon->channel << ' ' << w;
            if (me == w) lock.os << " (me)";
            lock.os << '\n';
        }
        return true;
    }
    if (command == "kill") {
        auto t = args.next<hash_t>();
        string data;
        auto r = rpc_daemon->get_peer().call_kill_trade(t, data);
        if (is_ko(r)) {
            scr << r << '\n';
            return true;
        }
        scr << data << '\n';
        return true;
    }
    if (command == "exec") { //api_exec
        string cmd = args.next_line();
        auto r = rpc_daemon->get_peer().call_exec(cmd);
        if (is_ko(r)) {
            scr << r << '\n';
            return true;
        }
        return true;
    }
    if (command.empty()) { //api_exec
        log("missing tid, show help.");
        screen::lock_t lock(scr, interactive);
        us::wallet::trader::traders_t::help("", lock.os);
        return true;
    }
    return false;
}

void c::trade_non_interactive(shell_args& args) {
    log("trade. non-interactive");
    string command = args.next<string>();
    if (exec_trade_global(command, args)) {
        return;
    }
    if (command == "wallet" || command == "exit" || command == "q") {
        return;
    }
    hash_t tid{0};
    istringstream is(command);
    is >> tid;
    if (is.fail()) {
        log(KO_11000);
        screen::lock_t lock(scr, interactive);
        lock.os << KO_11000 << '\n';
        return;
    }
    {
        string command = args.test_next<string>();
        if (command == "trade" || command == "wallet" || command == "exit" || command == "q") {
            cur = hash_t(0);
            curpro = "";
            return;
        }
    }
    log("command trade", tid);
    auto r = rpc_daemon->get_peer().call_exec_trade(rpc_peer_t::exec_trade_in_t(tid, args.next_line()));
    if (is_ko(r)) {
        scr << r << '\n';
    }
}

void c::trade_interactive(shell_args& args) {
    log("trade. interactive");
    scr << "Trading shell.\n";
    while (rpc_daemon->is_active()) {
        string command = args.next<string>();
        if (!exec_trade_global(command, args)) {
            istringstream is(command);
            hash_t tid{0};
            is >> tid;
            if (is.fail()) {
                log(KO_11000);
                scr << rewrite(KO_11000) << '\n';
            }
            else {
                if (!trade_interactive(args, tid)) {
                    break;
                }
            }
        }
        scr.flush();
        scr.prompt = "trade";
        string line = scr.capture_input(*rpc_daemon);
        if (!rpc_daemon->is_active()) {
            break;
        }
        if (line == "wallet" || line == "exit" || line == "q") {
            break;
        }
        if (line == "trade") {
            scr << "Already at the trading menu.\n";
            cur = hash_t(0);
            curpro = "";
        }
        if (cur.is_not_zero()) {
            ostringstream os;
            os << cur;
            line = os.str();
        }
        args = shell_args(line);
    }
}

void c::trade(bool interactive, shell_args& args) {
    interactive ? trade_interactive(args) : trade_non_interactive(args);
}

void c::tx_help(const string& ind, const params& p, ostream& os) {
    using fmt = us::wallet::trader::trader_protocol;
    fmt::twocol(ind, "tx new", "Creates an empty transaction", os);
    fmt::twocol(ind, "tx add_section <tx> <token>", "Creates a coin section in the given transaction", os);
    fmt::twocol(ind, "tx add_input <tx> <section> <address> <amount>", "Creates an input in the specified section of the given transaction", os);
    fmt::twocol(ind, "tx add_output <tx> <section> <address> <amount>", "Creates an output in the specified section of the given transaction", os);
    fmt::twocol(ind, "tx make_sigcode exec_time <open|close> [s <section> [i <input>]* [o <output>]* ] [-]", "", os);
    fmt::twocol(ind, "tx decode_sigcode <sigcode_b58>", "", os);
    fmt::twocol(ind, "tx ts <tx> <secs in future>", "Sets a new exec-time for the given transaction", os);
    fmt::twocol(ind, "tx sign_input <tx> <section> <input> <sigcode> <secret key>", "Signs the given tx input", os);
    fmt::twocol(ind, "tx check <tx>", "", os);
    fmt::twocol(ind, "tx send <tx>", "", os);
    fmt::twocol(ind, "tx sign <tx> <sigcodeb58>", "", os);
    fmt::twocol(ind, "tx decode <evidenceB58>", "", os);
}

ko c::tx(shell_args& args) {
    string command = args.next<string>();
    if (command == "new") {
        blob_t blob;
        auto r = rpc_daemon->get_peer().call_tx_new(blob);
        if (is_ko(r)) {
            return r;
        }
        scr << us::gov::crypto::b58::encode(blob) << '\n';
        return ok;
    }
    if (command=="add_section") {
        auto txb58 = args.next<string>();
        if (txb58.empty()) {
            auto r = "KO 54032 Missing encoded transaction.";
            log(r);
            return r;
        }
        hash_t token = args.next<hash_t>();
        if (token.is_zero()) {
            auto r = "KO 55043 Invalid coin.";
            log(r);
            return r;
        }
        auto blob_in = us::gov::crypto::b58::decode(txb58);
        blob_t blob_out;
        auto r = rpc_daemon->get_peer().call_tx_add_section(rpc_peer_t::tx_add_section_in_t(blob_in, token), blob_out);
        if (is_ko(r)) {
            return r;
        }
        scr << us::gov::crypto::b58::encode(blob_out) << '\n';
        return ok;
    }
    if (command == "add_output") {
        auto txb58=args.next<string>();
        if (txb58.empty()) {
            auto r = "KO 54032 Missing encoded transaction.";
            log(r);
            return r;
        }
        int n = args.next<int>(-1);
        if (n < 0) {
            auto r = "KO 55044 Invalid section.";
            log(r);
            return r;
        }
        auto addr=args.next<hash_t>();
        if (addr.is_zero()) {
            auto r = "KO 40302 Invalid address.";
            log(r);
            return r;
        }
        auto amount = args.next<cash_t>(0);
        if (amount < 1) {
            auto r = "KO 40312 Invalid amount.";
            log(r);
            return r;
        }
        auto blob_in = us::gov::crypto::b58::decode(txb58);
        blob_t blob_out;
        auto r = rpc_daemon->get_peer().call_tx_add_output(rpc_peer_t::tx_add_output_in_t(blob_in, n, addr, amount), blob_out);
        if (is_ko(r)) {
            return r;
        }
        scr << us::gov::crypto::b58::encode(blob_out) << '\n';
        return ok;
    }
    if (command == "add_input") {
        auto txb58 = args.next<string>();
        if (txb58.empty()) {
            auto r = "KO 54032 Missing encoded transaction.";
            log(r);
            return r;
        }
        int n = args.next<int>(-1);
        if (n < 0) {
            auto r = "KO 55044 Invalid section.";
            log(r);
            return r;
        }
        auto addr = args.next<hash_t>();
        if (addr.is_zero()) {
            auto r = "KO 40302 Invalid address.";
            log(r);
            return r;
        }
        auto amount = args.next<cash_t>(0);
        if (amount < 1) {
            auto r = "KO 40312 Invalid amount.";
            log(r);
            return r;
        }
        auto blob_in = us::gov::crypto::b58::decode(txb58);
        blob_t blob_out;
        auto r = rpc_daemon->get_peer().call_tx_add_input(rpc_peer_t::tx_add_input_in_t(blob_in, n, addr, amount), blob_out);
        if (is_ko(r)) {
            return r;
        }
        scr << us::gov::crypto::b58::encode(blob_out) << '\n';
        return ok;
    }
    if (command == "make_sigcode") {
        gov::cash::sigcode_t o;
        while (true) {
            string cmd = args.next<string>();
            if (cmd == "exec_time") {
                string ts = args.next<string>();
                if (ts == "open") {
                    o.ts = false;
                }
                else if (ts == "close") {
                    o.ts = true;
                }
                else {
                    auto r = "KO 20110 exec_time should be either 'open' or 'close'";
                    log(r);
                    return r;
                }
            }
            else if (cmd == "s") {
                auto s = args.next<int>(-1);
                if (s < 0) {
                    auto r = "KO 55044 Invalid section.";
                    log(r);
                    return r;
                }
                if (s >= o.size()) {
                    o.resize(s + 1);
                }
                auto it = o.begin();
                advance(it, s);
                while(true) {
                    string cmd = args.next<string>();
                    if (cmd == "i") {
                        auto i = args.next<int>(-1);
                        if (i < 0) {
                            auto r = "KO 30012 Invalid input.";
                            log(r);
                            return r;
                        }
                        it->inputs.emplace_back(i);
                    }
                    else if (cmd == "o") {
                        auto i = args.next<int>(-1);
                        if (i < 0) {
                            auto r = "KO 30013 Invalid output.";
                            log(r);
                            return r;
                        }
                        it->outputs.emplace_back(i);
                    }
                    else if (cmd.empty()) {
                        break;
                    }
                    else if (cmd == "-") {
                        break;
                    }
                    else {
                        return KO_11000;
                    }
                }
            }
            else if (cmd.empty()) {
                break;
            }
            else {
                return KO_11000;
            }
        }
        {
            ostringstream os;
            os << o.encode() << '\n';
            ::operator << (scr, os.str());
        }
        return ok;
    }
    if (command == "decode_sigcode") {
        auto b58 = args.next<string>();
        gov::cash::sigcode_t o;
        auto r = o.read(b58);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        o.dump(lock.os);
        return ok;
    }
    if (command == "ts") {
        auto txb58 = args.next<string>();
        gov::cash::tx_t tx;
        auto r = tx.read(txb58);
        if (r != ok) {
            return r;
        }
        ts_t n = args.next<ts_t>(0);
        tx.update_ts(n * 1e9);
        using ::operator <<;
        scr << tx.encode() << '\n';
        return ok;
    }
    if (command == "sign_input") {
        auto txb58 = args.next<string>();
        gov::cash::tx_t tx;
        {
            auto r = tx.read(txb58);
            if (r != ok) {
                return r;
            }
        }
        int n = args.next<int>(-1);
        if (n < 0 || n >= tx.sections.size()) {
            auto r = "KO 55044 Invalid section.";
            log(r);
            return r;
        }
        auto it = tx.sections.begin();
        advance(it, n);
        int m = args.next<int>(-1);
        if (m < 0 || m >= it->inputs.size()) {
            auto r = "KO 30012 Invalid input.";
            log(r);
            return r;
        }
        auto iti = it->inputs.begin();
        advance(iti, m);
        auto ssigcode = args.next<string>();
        if (ssigcode.empty()) {
            auto r = "KO 30291 Invalid sigcode.";
            log(r);
            return r;
        }
        gov::cash::sigcode_t sigcode;
        {
            auto r = sigcode.read(ssigcode);
            if (r != ok) {
                return r;
            }
        }
        auto privk = args.next<gov::crypto::ec::keys::priv_t>();
        if (privk.is_zero()) {
            auto r = "KO 88127 Invalid private key.";
            log(r);
            return r;
        }
        iti->locking_program_input = us::wallet::wallet::algorithm::create_input(tx.get_hash(sigcode), privk);
        if (!us::gov::cash::app::check_input(iti->address, tx, iti->locking_program_input)) {
            auto r = "KO 10923 Could not create signature.";
            log(r);
            return r;
        }
        using ::operator <<;
        scr << tx.encode() << '\n';
        return ok;
    }
    if (command == "set_supply") {
        auto addr = args.next<hash_t>();
        if (addr.is_zero()) {
            auto r = "KO 40302 Invalid address.";
            log(r);
            return r;
        }
        cash_t amount = args.next<cash_t>(-1);
        if (amount < 0) {
            auto r = "KO 40312 Invalid amount.";
            log(r);
            return r;
        }
        blob_t blob_out;
        auto r = rpc_daemon->get_peer().call_set_supply(rpc_peer_t::set_supply_in_t(addr, amount, 1), blob_out);
        if (is_ko(r)) {
            return r;
        }
        scr << us::gov::crypto::b58::encode(blob_out) << '\n';
        return ok;
    }
    if (command == "transfer") {
        auto a = args.next<hash_t>();
        auto b = args.next<cash_t>(0);
        auto c = args.next_token();
        blob_t blob_out;
        auto r = rpc_daemon->get_peer().call_transfer(rpc_peer_t::transfer_in_t(a, b, c, 1), blob_out);
        if (is_ko(r)) {
            return r;
        }
        scr << us::gov::crypto::b58::encode(blob_out) << '\n';
        return ok;
    }
    if (command == "transfer_from") {
        auto s = args.next<hash_t>();
        auto a = args.next<hash_t>();
        auto b = args.next<cash_t>(0);
        auto c = args.next_token();
        blob_t blob_out;
        auto r = rpc_daemon->get_peer().call_transfer_from(rpc_peer_t::transfer_from_in_t(s, a, b, c, 1), blob_out);
        if (is_ko(r)) {
            return r;
        }
        scr << us::gov::crypto::b58::encode(blob_out) << '\n';
        return ok;
    }
    if (command == "invoice") {
        if (args.args_left() == 0) {
            screen::lock_t lock(scr, interactive);
            lock.os << KO_11000 << '\n';
            help_2steptx("", lock.os);
            return KO_11000;
        }
        hash_t recv_addr=args.next<hash_t>();
        cash_t recv_amount=args.next<cash_t>();
        hash_t recv_token=args.next_token();
        string option=args.next<string>("");
        cash_t send_amount{0};
        hash_t send_token{0};
        if (option == "-reward") {
            send_amount = args.next<cash_t>(0);
            send_token = args.next_token();
            if (send_amount == 0) {
                auto r = "KO 29161 Reward amount.";
                log(r);
                return r;
            }
        }
        else {
            if (!option.empty()) {
                auto r = "KO 29162 Invalid option.";
                log(r);
                return r;
            }
        }
        blob_t blob_out;
        auto r = rpc_daemon->get_peer().call_tx_charge_pay(rpc_peer_t::tx_charge_pay_in_t(recv_addr, recv_amount, recv_token, send_amount, send_token), blob_out);
        if (is_ko(r)) {
            return r;
        }
        scr << us::gov::crypto::b58::encode(blob_out) << '\n';
        return ok;
    }
    if (command == "pay") {
        if (args.args_left() == 0) {
            screen::lock_t lock(scr, interactive);
            lock.os << KO_11000 << '\n';
            help_2steptx("", lock.os);
            return KO_11000;
        }
        auto blob_in = us::gov::crypto::b58::decode(args.next<string>());
        blob_t blob_out;
        auto r = rpc_daemon->get_peer().call_tx_pay(rpc_peer_t::tx_pay_in_t(blob_in, 1), blob_out);
        if (is_ko(r)) {
            return r;
        }
        scr << us::gov::crypto::b58::encode(blob_out) << '\n';
        return ok; //wapi.tx_pay(tx);
    }
    if (command == "sign") {
        auto blob_in = us::gov::crypto::b58::decode(args.next<string>());
        auto blob_sigcode = us::gov::crypto::b58::decode(args.next<string>());
        blob_t blob_out;
        auto r = rpc_daemon->get_peer().call_tx_sign(rpc_peer_t::tx_sign_in_t(blob_in, blob_sigcode), blob_out);
        if (is_ko(r)) {
            return r;
        }
        scr << us::gov::crypto::b58::encode(blob_out) << '\n';
        return ok;
    }
    if (command == "decode") {
        auto blob_in = us::gov::crypto::b58::decode(args.next<string>());
        string tx_pretty;
        auto r = rpc_daemon->get_peer().call_tx_decode(blob_in, tx_pretty);
        if (is_ko(r)) {
            return r;
        }
        scr << tx_pretty << '\n';
        return ok;
    }
    if (command == "check") {
        auto blob_in = us::gov::crypto::b58::decode(args.next<string>());
        string data;
        auto r = rpc_daemon->get_peer().call_tx_check(blob_in, data);
        if (is_ko(r)) {
            return r;
        }
        scr << data << '\n';
        return ok;
    }
    if (command == "send") {
        auto blob_in = us::gov::crypto::b58::decode(args.next<string>());
        string info;
        auto r = rpc_daemon->get_peer().call_tx_send(blob_in, info);
        if (is_ko(r)) {
            return r;
        }
        scr << info << '\n';
        return ok;
    }
    screen::lock_t lock(scr, interactive);
    tx_help("", p, lock.os);
    return KO_11000;
}

void c::banner(const params& p, ostream& os) {
    using namespace std::chrono;
    string ind = "    ";
    os << PLATFORM << "-wallet (rpc client). Channel " << p.channel << ". Copyright (C) " << CFG_COPYRIGHT_LINE << '\n';
    os << ind << CFG_COPYRIGHT_LINE2 << '\n';
    os << ind << CFG_COPYRIGHT_LINE3 << '\n';
    os << ind << "version: " << us::vcs::version() << '\n';
    os << ind << "local time: " << duration_cast<nanoseconds>((system_clock::now() - us::gov::engine::calendar_t::relay_interval).time_since_epoch()).count() << " ns since 1/1/1970\n";
    os << ind << "tx time shift: " << duration_cast<seconds>(us::gov::engine::calendar_t::relay_interval).count() << " seconds.\n";
    os << ind << "Build configuration: ";
    #if CFG_LOGS == 1
        os << "[with logs] ";
        if (p.nolog) {
            os << "[disabled] ";
        }
        if (p.verbose) {
            os << "[logs to screen] ";
        }
        os << "[logdir " << p.logd << "] ";
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
    p.dump(ind + "    ", os);
}

void c::help_2steptx(const string& ind, ostream&os) {
    using fmt = us::wallet::trader::trader_protocol;
    fmt::twocol(ind, "invoice <recv account> <amount> <gas|coin> [-reward <award amount> <gas|coin>]", "Creates an invoice (incomplete transaction)", os);
    fmt::twocol(ind, "pay <tx>", "Pay invoice. Complete the transaction and send it over", os);
}

namespace {

    template<typename t>
    string tostr(const t& o) {
        ostringstream os;
        os << " [" << o << ']';
        return os.str();
    }

    template<>
    string tostr(const bool& o) {
        ostringstream os;
        os << " [" << boolalpha << o << ']';
        return os.str();
    }

}

void c::help(const params& p, ostream& os) { //moved
    using fmt = us::wallet::trader::trader_protocol;
    if (p.banner) {
        banner(p, os);
    }
    os << "Usage: " << PLATFORM << "-wallet [options] [command]\n";
    os << "Options:\n";
    string ind = "    ";
    string ind____ = ind + "    ";
    fmt::twocol(ind, "General parameters:", "", os);
    if (p.advanced) {
        fmt::twocol(ind____, "-b", "Basic mode", os);
    }
    if (p.advanced) {
        fmt::twocol(ind____, "-local", "Instantiate a wallet instead of connecting to a daemon.", tostr(p.local), os);
        fmt::twocol(ind____, "-home <homedir>", "homedir", tostr(p.get_home()), os);
        fmt::twocol(ind____, "-d", "Runs daemon (backend) instead of an rpc-client.", tostr(p.listening_port), os);
        fmt::twocol(ind____, "-e <edges>", "Max num walletd neightbours", tostr((int)p.max_trade_edges), os);
        fmt::twocol(ind____, "-v <edges>", "Max num device neightbours", tostr((int)p.max_devices), os);
        fmt::twocol(ind____, "-w <workers>", "Threadpool size", tostr((int)p.workers), os);
        fmt::twocol(ind____, "-lp <port>", "Listening Port. (-d)", tostr(p.listening_port), os);
        fmt::twocol(ind____, "-pp <port>", "Published listening port.", tostr(p.published_port), os);
        fmt::twocol(ind____, "-pin <PIN number>", "Connect using PIN.", tostr(p.pin), os);
        fmt::twocol(ind____, "-c <channel>", "System channel", tostr(p.channel), os);
        #if CFG_FCGI == 1
            fmt::twocol(ind____, "-fcgi", "Behave as a fast-cgi program. requires -d", tostr(p.fcgi ? "yes" : "no"), os);
        #endif
        #if CFG_LOGS == 1
            fmt::twocol(ind____, "-log", "Logs in screen.", os);
            fmt::twocol(ind____, "-logd <dir>", "Directory for writting log files.", os);
            fmt::twocol(ind____, "-nolog ", "Disable logs.", os);
        #endif
        //fmt::twocol(ind____, "-om <output_mode>", "0:human, 1:text, 2:xml, 3:json", tostr(p.get_output_mode()), os);
        fmt::twocol(ind____, "-dd <downloads_dir>", "Directory containing software updates blobs", tostr(p.downloads_dir), os);
        fmt::twocol(ind____, "-n", "Omit field names in output", tostr(p.names), os);
        fmt::twocol(ind____, "-nb", "Don't show the banner.", os);
        {
            ostringstream os;
            os << "RPC to backend(" << PLATFORM << "-gov) parameters:";
            fmt::twocol(ind, os.str(), "", os);
        }
        fmt::twocol(ind____, "-bhost <address>", "us-gov IP address" ,tostr(p.backend_host), os);
        fmt::twocol(ind____, "-bp <port>", "TCP port", tostr(p.backend_port), os);
        {
            ostringstream os;
            os << "RPC to wallet-daemon (" << PLATFORM << "-wallet) parameters:";
            fmt::twocol(ind, os.str(), "", os);
        }
        fmt::twocol(ind____, "-whost <address>", "walletd address", tostr(p.walletd_host), os);
        fmt::twocol(ind____, "-wp <port>", "walletd port", tostr(p.walletd_port), os);
    }
    os << '\n';
    os << "Commands:\n";
    fmt::twocol(ind, "misc:", "----------", os);
    fmt::twocol(ind____, "id", "Print wallet id and exit.", os);
    fmt::twocol(ind____, "version", "Print software version.", os);
    os << '\n';
    fmt::twocol(ind, "cash: Query", "----------", os);
    fmt::twocol(ind____, "balance <0|1|2>", "If detail is 0 it displays a collapsed view of accounts. 1 or 2 shows the breakdown by account with the given level of detail", os);
    fmt::twocol(ind____, "recv_address", "Shows a suitable address for receiving transfers", os);
    os << '\n';
    fmt::twocol(ind, "cash: Functions for transfering to <dest account>", "----------", os);
    fmt::twocol(ind____, "transfer <dest account> <amount> <gas|coin>", "Source account/s will be choosen automatically", os);
    fmt::twocol(ind____, "transfer_from <source account> <dest account> <amount> <gas|coin>", "Selectable source account", os);
    os << '\n';
    fmt::twocol(ind, "cash: Two-step transactions", "----------", os);
    help_2steptx(ind____, os);
    if (p.advanced) {
        os << '\n';
        fmt::twocol(ind, "cash: Coin/token creation", "----------", os);
        fmt::twocol(ind____, "set_supply <address> <amount>", "Set or reset the coin mint balance", os);
        os << '\n';
        fmt::twocol(ind, "Transaction making", "----------", os);
        tx_help(ind____, p, os);
    }
    os << '\n';
    fmt::twocol(ind, "keys:", "----------", os);
    fmt::twocol(ind____, "address new", "Generates a new key-pair, adds the private key to the wallet and prints its asociated address", os);
    if (p.advanced) {
        fmt::twocol(ind____, "address add <privkey>", "Imports a given private key in the wallet", os);
    }
    fmt::twocol(ind____, "list [show_priv_keys=0|1]", "Lists all keys. Default value for show_priv_keys is 0", os);
    if (p.advanced) {
        fmt::twocol(ind____, "gen_keys", "Generates a key pair without adding them to the wallet.", os);
        fmt::twocol(ind____, "mine_public_key <string>", "Creates a vanity address containing the specified string", os);
        fmt::twocol(ind____, "priv_key <private key>", "Gives information about the given private key", os);
        fmt::twocol(ind____, "pub_key <public key>", "Gives information about the given public key", os);
        fmt::twocol(ind____, "digest <filename>", "Computes RIPEMD160+base58 to the content of the file.", os);
        fmt::twocol(ind____, "sign -f <filename> <private key>", "Sign file.", os);
        fmt::twocol(ind____, "sign \"<message>\" <private key>", "Sign message", os);
        fmt::twocol(ind____, "verify -f <filename> <pubkey> <signature>", "Verify signed file.", os);
        fmt::twocol(ind____, "verify \"<message>\" <pubkey> <signature>", "Verify signed message", os);
        fmt::twocol(ind____, "encrypt -f <filename> <sender_private_key> <recipient_public_key>", "Encrypts file.", os);
        fmt::twocol(ind____, "encrypt \"<message>\" <sender_private_key> <recipient_public_key>", "Encrypts message", os);
        fmt::twocol(ind____, "decrypt \"<b58>\" <sender_public_key> <recipient_private_key>", "Decrypts message", os);
        fmt::twocol(ind____, "hash add <hash1> <hash2>", "Results in RIPEMD160(hash1, hash2).", os);
    }
    os << '\n';
    if (p.advanced) {
        fmt::twocol(ind, "Device pairing:", "----------", os);
        fmt::twocol(ind____, "device_id", "Show this device Id.", os);
        fmt::twocol(ind____, "pair <pubkey> [<subhome>|-] [<name>]", "Authorize a device identified by its public key", os);
        fmt::twocol(ind____, "unpair <pubkey>", "Revoke authorization to the specified device", os);
        fmt::twocol(ind____, "prepair <pin|auto> [<subhome>|-] [<name>]", "Pre-Authorize an unknown device identified by a pin number [1-65535]", os);
        fmt::twocol(ind____, "unprepair <pin>", "Revoke Pre-authorization to the specified pin", os);
        fmt::twocol(ind____, "list_devices", "Prints the list of recognized devices, together with the list of recently unathorized attempts log", os);
        fmt::twocol(ind____, "net_info", "Prints contextual information and wallet type (root/guest)", os);
        os << '\n';
        fmt::twocol(ind, "Public storage (on-chain):", "----------", os);
        os << ind << "  key-value:\n";
        fmt::twocol(ind____, "store <address> -kv <key> <value>", "Store key-value pair in a funded account. cost: 1 per cycle", os);
        fmt::twocol(ind____, "rm <address> -kv <key>", "Removes the key-value pair", os);
        fmt::twocol(ind____, "search <address> <word1|word2...>", "Search key-value table", os);
        os << ind << "  file:\n";
        fmt::twocol(ind____, "store <address> -f <file> [path]", "Store file in a funded account. cost: 1 per kibibyte per cycle", os);
        fmt::twocol(ind____, "rm <address> -f <hash> <path>", "Removes a file. <path> example: /", os);
        fmt::twocol(ind____, "file <hash>", "Retrieves file content", os);
        fmt::twocol(ind____, "list <address> <path>", "List files starting with path", os);
        os << '\n';
        fmt::twocol(ind, "Private storage: (in-wallet)", "----------", os);
        fmt::twocol(ind____, "timeseries list", "List accounts holding timeseries.", os);
        fmt::twocol(ind____, "timeseries <address> new", "Creates a new time series in the given address.", os);
        fmt::twocol(ind____, "timeseries <address> add [-f <filename>|-b58 <b58>|\"text line\"]", "Appends a new entry.", os);
        fmt::twocol(ind____, "timeseries <address> show [<timemark>]", "List all events registered. Or obtain its content if a timemark is given.", os);
        os << '\n';
        #ifdef CFG_PERMISSIONED_NETWORK
            fmt::twocol(ind, "Permissions:", "----------", os);
            os << ind << "  Nodes:\n";
            fmt::twocol(ind____, "nodes allow <address> <node-master key>", "Accept node", os);
            fmt::twocol(ind____, "nodes deny <address> <node-master key>", "Reject node", os);
            os << '\n';
        #endif
        fmt::twocol(ind, "Law/Compilance:", "----------", os);
        fmt::twocol(ind____, "compilance_report <jurisdiction> <date-from> <date-to>", "Produces a private report including personal, financial and ownership data that voluntarily could be submitted to regulators", os);
        fmt::twocol(ind, "Trader:", "----------", os);
        fmt::twocol(ind, "trade <command>", "Access to trading functions", os);
        us::wallet::trader::traders_t::help(ind____, os);
        os << '\n';
        fmt::twocol(ind, "Daemon control/monitoring:", "----------", os);
        fmt::twocol(ind____, "s", "Show socket connections", os);
        fmt::twocol(ind____, "gw", "Show neighbours", os);
        fmt::twocol(ind____, "allw", "Show all wallets", os);
        fmt::twocol(ind____, "regw <ip address>", "Register this wallet reachable at the specified address.", os);
        fmt::twocol(ind____, "sync", "Reload files", os);
        fmt::twocol(ind____, "reload_doc <full-path-doc>", "Invoke reload on all active trades in all wallets. Only via root wallet", os);
        os << '\n';
        fmt::twocol(ind, "Net-dev control:", "----------", os);
        fmt::twocol(ind____, "patch_os <script file> <skey>", "System upgrade/maintenance. Requires governance key", os);
        //fmt::twocol(ind____, "harvest <id> <timeout> <decay> <skey>", "Discover topology. id is any uint16_t or 0 to reset", os);
        os << ind____ << "(Account for updates: " << AUTOMATIC_UPDATES_ACCOUNT << ")\n";
        os << '\n';
        fmt::twocol(ind, "Connectivity:", "----------", os);
        fmt::twocol(ind____, "ping", "Endpoint check", os);
        os << '\n';
        fmt::twocol(ind, "Software:", "----------", os);
        fmt::twocol(ind____, "version", "Print version.", os);
        fmt::twocol(ind____, "h|-h|help|-help|--help", "This help screen", os);
        os << '\n';
    }
    os << '\n';
}

ko c::exec_online1(const string& cmd, shell_args& args) {
    log("exec", cmd);
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
    string command = cmd;
    if (command == "transfer" || command == "invoice" || command == "pay" || command == "transfer_from" || command == "set_supply") {
        command = "tx";
        --args.n;
    }
    if (command == "tx") {
        return tx(args);
    }
    if (command == "priv_key") {
        auto privkey = args.next<gov::crypto::ec::keys::priv_t>();
        rpc_peer_t::priv_key_out_dst_t o_out;
        auto r = rpc_daemon->get_peer().call_priv_key(move(privkey), o_out);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << "pub " << o_out.pub << '\n';
        lock.os << "addr " << o_out.addr << '\n';
        return ok;
    }
    if (command == "pub_key") {
        auto pubkey = args.next<gov::crypto::ec::keys::pub_t>();
        rpc_peer_t::pub_key_out_dst_t o_out;
        auto r = rpc_daemon->get_peer().call_pub_key(move(pubkey), o_out);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << "pub " << o_out.pub << '\n';
        lock.os << "addr " << o_out.addr << '\n';
        return ok; //wapi.tx_pay(tx);
    }
    if (command == "address") {
        command = args.next<string>();
        if (command == "new") {
            hash_t addr;
            auto r = rpc_daemon->get_peer().call_new_address(addr);
            if (is_ko(r)) {
                return r;
            }
            screen::lock_t lock(scr, interactive);
            lock.os << "addr " << addr << '\n';
            return ok;
        }
        if (command == "add") {
            auto sk = args.next<gov::crypto::ec::keys::priv_t>();
            hash_t addr;
            auto r = rpc_daemon->get_peer().call_add_address(sk, addr);
            if (is_ko(r)) {
                return r;
            }
            screen::lock_t lock(scr, interactive);
            lock.os << "addr " << addr << '\n';
            return ok;
        }
        return KO_11000;
    }
    if (command == "list") {
        auto detailed = args.next<uint16_t>(0);
        string data;
        auto r = rpc_daemon->get_peer().call_list(detailed, data);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << "addr " << data << '\n';
        return ok;
    }
    if (command == "balance") {
        auto detailed = args.next<uint16_t>(0);
        string data;
        auto r = rpc_daemon->get_peer().call_balance(detailed, data);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << data << '\n';
        return ok;
    }
    if (command == "recv_address") {
        hash_t addr;
        auto r = rpc_daemon->get_peer().call_recv_address(addr);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << "addr " << addr << '\n';
        return ok;
    }
    if (command == "stress") {
        int i = 0;
        while(rpc_daemon->is_active()) {
            {
                screen::lock_t lock(scr, interactive);
                lock.os << "call #" << i++ << '\n';
            }
            string data;
            auto r = rpc_daemon->get_peer().call_balance(0, data);
            if (is_ko(r)) {
                return r;
            }
            screen::lock_t lock(scr, interactive);
            lock.os << data << '\n';
            return ok;
        }
        return ok;
    }
    if (command == "gen_keys") {
        rpc_peer_t::gen_keys_out_dst_t o_out;
        auto r = rpc_daemon->get_peer().call_gen_keys(o_out);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << "priv " << o_out.priv << '\n';
        lock.os << "pub " << o_out.pub << '\n';
        lock.os << "addr " << o_out.addr << '\n';
        return ok;
    }
    if (command == "sign") {
        string msg = args.next<string>();
        vector<uint8_t> v;
        if (msg == "-f") {
            string filename = args.next<string>();
            auto r = us::gov::io::read_file_(filename, v);
            if (is_ko(r)) {
                return r;
            }
        }
        else {
            v = vector<uint8_t>(msg.begin(), msg.end());
        }
        auto sk = args.next<gov::crypto::ec::keys::priv_t>();
        rpc_peer_t::sign_out_dst_t o_out;
        auto r = rpc_daemon->get_peer().call_sign(rpc_peer_t::sign_in_t(v, sk), o_out);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << "pubkey " << o_out.pub << '\n';
        lock.os << "sig " << o_out.sig.to_b58() << '\n';
        return ok;
    }
    if (command == "verify") {
        string msg = args.next<string>();
        vector<uint8_t> v;
        if (msg == "-f") {
            string filename = args.next<string>();
            auto r = us::gov::io::read_file_(filename, v);
            if (is_ko(r)) {
                return r;
            }
        }
        else {
            v = vector<uint8_t>(msg.begin(), msg.end());
        }
        auto pubkey = args.next<gov::crypto::ec::keys::pub_t>();
        auto sig = us::gov::crypto::ec::instance.sig_from_b58(args.next<string>());
        uint8_t result = 0;
        auto r = rpc_daemon->get_peer().call_verify(rpc_peer_t::verify_in_t(v, pubkey, sig), result);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << "result " << (result == 1 ? "Yes" : "No") << '\n';
        return ok;
    }
    if (command == "encrypt") {
        string msg = args.next<string>();
        vector<uint8_t> v;
        if (msg == "-f") {
            string filename = args.next<string>();
            auto r = us::gov::io::read_file_(filename, v);
            if (is_ko(r)) {
                return r;
            }
        }
        else {
            v = vector<uint8_t>(msg.begin(), msg.end());
        }
        auto sender_privkey = args.next<gov::crypto::ec::keys::priv_t>();
        auto recipient_pubkey = args.next<gov::crypto::ec::keys::pub_t>();
        vector<uint8_t> msg_scrambled;
        auto r = rpc_daemon->get_peer().call_encrypt(rpc_peer_t::encrypt_in_t(v, sender_privkey, recipient_pubkey), msg_scrambled);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << us::gov::crypto::b58::encode(msg_scrambled) << '\n';
        return ok;
    }
    if (command == "decrypt") {
        string msg = args.next<string>();
        vector<uint8_t> v;
        string outf;
        if (msg == "-f") {
            string filename = args.next<string>();
            auto r = us::gov::io::read_file_(filename, v);
            if (is_ko(r)) {
                return r;
            }
            outf = filename + ".decrypted";
        }
        else {
            v = vector<uint8_t>(msg.begin(), msg.end());
        }
        auto sender_pubkey = args.next<gov::crypto::ec::keys::pub_t>();
        auto recipient_privkey = args.next<gov::crypto::ec::keys::priv_t>();
        vector<uint8_t> cleartext;
        auto r = rpc_daemon->get_peer().call_decrypt(rpc_peer_t::decrypt_in_t(v, sender_pubkey, recipient_privkey), cleartext);
        if (is_ko(r)) {
            return r;
        }
        if (outf.empty()) {
            screen::lock_t lock(scr, interactive);
            lock.os.write((const char*)cleartext.data(), cleartext.size());
        }
        else {
            auto r = us::gov::io::write_file_(cleartext, outf);
            if (is_ko(r)) {
                return r;
            }
            screen::lock_t lock(scr, interactive);
            lock.os << "written file " << outf << '\n';

        }
        return ok;
    }
    if (command == "patch_os") {
        string filename = args.next<string>();
        string script;
        {
            auto r = us::gov::io::read_text_file_(filename, script);
            if (is_ko(r)) {
                return r;
            }
        }
        auto grootsk = args.next<gov::crypto::ec::keys::priv_t>();
        if (grootsk.is_zero()) {
            return "KO 87956 Invalid secret key.";
        }
        rpc_peer_t::patch_os_out_dst_t o_out;
        {
            auto r = rpc_daemon->get_peer().call_patch_os(rpc_peer_t::patch_os_in_t(script, grootsk, 1), o_out);
            if (is_ko(r)) {
                return r;
            }
        }
        screen::lock_t lock(scr, interactive);
        lock.os << us::gov::crypto::b58::encode(o_out.blob_ev) << '\n';
        lock.os << "job " << o_out.job << '\n';
        return ok;
    }
    if (command == "component") {
        return KO_40322;
    }
    if (command == "harvest") {
        return KO_40322;
    }
    if (command == "store") {
        log("store");
        hash_t addr = args.next<hash_t>();
        string opt = args.next<string>();
        if (opt == "-kv") {
            string key = args.next<string>();
            string value = args.next<string>();
            blob_t o_out;
            auto r = rpc_daemon->get_peer().call_store_kv(rpc_peer_t::store_kv_in_t(addr, key, value, 1), o_out);
            if (is_ko(r)) {
                return r;
            }
            screen::lock_t lock(scr, interactive);
            lock.os << us::gov::crypto::b58::encode(o_out) << '\n';
            return ok;
        }
        if (opt == "-f") {
            string file = args.next<string>();
            string path = args.next<string>();
            path = us::gov::io::cfg0::rewrite_path(path);
            vector<uint8_t> content;
            {
                auto r = gov::io::read_file_(file, content);
                if (is_ko(r)) {
                    return r;
                }
            }
            rpc_peer_t::store_file_out_dst_t o_out;
            auto r = rpc_daemon->get_peer().call_store_file(rpc_peer_t::store_file_in_t(addr, path, content, 1), o_out);
            if (is_ko(r)) {
                return r;
            }
            screen::lock_t lock(scr, interactive);
            lock.os << us::gov::crypto::b58::encode(o_out.blob_ev) << '\n';
            lock.os << "path " << o_out.path << '\n';
            lock.os << "digest " << o_out.digest << '\n';
            return ok;
        }
        return KO_11000;
    }
    if (command == "rm") {
        hash_t addr = args.next<hash_t>();
        string opt = args.next<string>();
        if (opt == "-kv") {
            string key = args.next<string>();
            blob_t o_out;
            auto r = rpc_daemon->get_peer().call_rm_kv(rpc_peer_t::rm_kv_in_t(addr, key, 1), o_out);
            if (is_ko(r)) {
                return r;
            }
            screen::lock_t lock(scr, interactive);
            lock.os << us::gov::crypto::b58::encode(o_out) << '\n';
            return ok;
        }
        if (opt == "-f") {
            hash_t digest = args.next<hash_t>();
            string path = args.next<string>();
            blob_t o_out;
            auto r = rpc_daemon->get_peer().call_rm_file(rpc_peer_t::rm_file_in_t(addr, path, digest, 1), o_out);
            if (is_ko(r)) {
                return r;
            }
            screen::lock_t lock(scr, interactive);
            lock.os << us::gov::crypto::b58::encode(o_out) << '\n';
            return ok;
        }
        return KO_11000;
    }
    if (command == "file") {
        hash_t digest = args.next<hash_t>();
        vector<uint8_t> content;
        auto r = rpc_daemon->get_peer().call_file(digest, content);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os.write((const char*)content.data(), content.size());
        return ok;
    }
    if (command == "search") {
        return KO_40322;
    }
    if (command == "timeseries") {
        auto cmd = args.next<string>();
        if (cmd == "list") {
            string o_out;
            auto r = rpc_daemon->get_peer().call_timeseries_list(o_out);
            if (is_ko(r)) {
                return r;
            }
            screen::lock_t lock(scr, interactive);
            lock.os << o_out << '\n';
            return ok;
        }
        hash_t addr(cmd);
        if (addr.is_zero()) {
            auto r = "KO 69302 Invalid command or address. Valid commands are: list.";
            log(r);
            return r;
        }
        cmd = args.next<string>();
        if (cmd == "show") {
            string o_out;
            ts_t ts = args.next<ts_t>();
            auto r = rpc_daemon->get_peer().call_timeseries_show(rpc_peer_t::timeseries_show_in_t(addr, ts), o_out);
            if (is_ko(r)) {
                return r;
            }
            screen::lock_t lock(scr, interactive);
            lock.os << o_out << '\n';
            return ok;
        }
        if (cmd == "new") {
            string o_out;
            auto r = rpc_daemon->get_peer().call_timeseries_new(rpc_peer_t::timeseries_new_in_t(addr, 1), o_out);
            if (is_ko(r)) {
                return r;
            }
            screen::lock_t lock(scr, interactive);
            lock.os << o_out << '\n';
            return ok;
        }
        if (cmd == "add") {
            string s = args.next<string>();
            vector<uint8_t> v;
            if (s == "-f") {
                string fname = args.next<string>();
                auto r = us::gov::io::read_file_(fname, v);
                if (is_ko(r)) {
                    return r;
                }
            }
            else if (s == "-b58") {
                string b58 = args.next<string>();
                v = us::gov::crypto::b58::decode(b58);
            }
            else {
                v = std::vector<uint8_t>(s.length());
                std::copy(s.begin(), s.end(), v.begin());
            }            
            if (v.empty()) {
                auto r = "KO 76037 Empty content.";
                log(r);
                return r;
            }
            string o_out;
            {
                auto r = rpc_daemon->get_peer().call_timeseries_add(rpc_peer_t::timeseries_add_in_t(addr, v), o_out);
                if (is_ko(r)) {
                    return r;
                }
            }
            screen::lock_t lock(scr, interactive);
            lock.os << o_out << '\n';
            return ok;
        }
        auto r = "KO 52109 Valid commands are: show|new|add.";
        log(r);
        return r;
    }
    #ifdef CFG_PERMISSIONED_NETWORK
        if (command == "nodes") {
            string verb = args.next<string>();
            hash_t addr = args.next<hash_t>();
            auto sk = args.next<gov::crypto::ec::keys::priv_t>();
            if (verb == "allow") {
                blob_t o_out;
                auto r = rpc_daemon->get_peer().call_nodes_allow(rpc_peer_t::nodes_allow_in_t(addr, sk, 1), o_out);
                if (is_ko(r)) {
                    return r;
                }
                screen::lock_t lock(scr, interactive);
                lock.os << us::gov::crypto::b58::encode(o_out) << '\n';
                return ok;
            }
            if (verb == "deny") {
                blob_t o_out;
                auto r = rpc_daemon->get_peer().call_nodes_deny(rpc_peer_t::nodes_deny_in_t(addr, sk, 1), o_out);
                if (is_ko(r)) {
                    return r;
                }
                screen::lock_t lock(scr, interactive);
                lock.os << us::gov::crypto::b58::encode(o_out) << '\n';
                return ok;
            }
            return KO_11000;
        }
    #endif
    if (command == "trade") {
        log("trade", "caller interactive", interactive, "args left", args.args_left());
        bool interactive_ = interactive && args.args_left() == 0;
        trade(interactive_, args);
        if (rpc_daemon->is_active()) {
            if (interactive_) {
                scr << "Wallet shell.\n";
            }
        }
        return ok;
    }
    if (command == "gw") {
        string data;
        auto r = rpc_daemon->get_peer().call_print_grid(data);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << data << '\n';
        return ok;
    }
    if (command == "s") {
        string data;
        auto r = rpc_daemon->get_peer().call_print_socket_connections(data);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << data << '\n';
        return ok;
    }
    if (command == "allw") {
        string data;
        auto r = rpc_daemon->get_peer().call_allw(data);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << data << '\n';
        return ok;
    }
    if (command == "regw") {
        string ip = args.next<string>();
        string ans;
        auto r = rpc_daemon->get_peer().call_register_w(us::gov::socket::client::ip4_encode(ip), ans);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << ans << '\n';
        return ok;
    }
    if (command == "sync") {
        string data;
        auto r = rpc_daemon->get_peer().call_sync(data);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << data << '\n';
        return ok;
    }
    if (command == "reload_doc") {
        auto t = args.next<string>();
        string ans;
        auto r = rpc_daemon->get_peer().call_reload_file(t, ans);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << ans << '\n';
        return ok;
    }
    if (command == "pair") {
        pub_t pub = args.next<pub_t>();
        if (!pub.valid) {
            return "KO 24019 Invalid public key.";
        }
        auto subhome = args.next<string>();
        auto name = args.next<string>();
        string ans;
        auto r = rpc_daemon->get_peer().call_pair_device(rpc_peer_t::pair_device_in_t(pub, subhome, name), ans);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << ans << '\n';
        return ok;
    }
    if (command == "unpair") {
        auto pub = args.next<pub_t>();
        string ans;
        auto r = rpc_daemon->get_peer().call_unpair_device(pub, ans);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << ans << '\n';
        return ok;
    }
    if (command == "prepair") {
        string spin = args.next<string>();
        pin_t pin=0;
        if (spin == "auto") {
            pin = numeric_limits<pin_t>::max();
        }
        else {
            istringstream is(spin);
            is >> pin;
            if (is.fail()) {
                auto r = "KO 10241 Invalid PIN.";
                log(r);
                return r;
            }
        }
        if (pin == 0) {
            auto r = "KO 10242 Invalid PIN.";
            log(r);
            return r;
        }
        auto subhome = args.next<string>();
        auto name = args.next<string>();
        rpc_peer_t::prepair_device_out_dst_t o_out;
        auto r = rpc_daemon->get_peer().call_prepair_device(rpc_peer_t::prepair_device_in_t(pin, subhome, name), o_out);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << "subhome " << o_out.subhome << '\n';
        lock.os << "pin " << o_out.pin << '\n';
        return ok;
    }
    if (command == "unprepair") {
        pin_t pin = args.next<pin_t>(0);
        string ans;
        auto r = rpc_daemon->get_peer().call_unprepair_device(pin, ans);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << ans << '\n';
        return ok;
    }
    if (command == "net_info") {
        rpc_peer_t::net_info_out_dst_t o_out;
        auto r = rpc_daemon->get_peer().call_net_info(o_out);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << "seeds:\n";
        for (auto& i: o_out.seeds) {
            lock.os << get<0>(i) << ' ' << rpc_peer_t::endpoint(get<1>(i), get<2>(i)) << '\n';
        }
        lock.os << "wallet address " << o_out.wallet_address << '\n';
        lock.os << "subhome " << o_out.subhome << '\n';
        return ok;
    }
    if (command == "list_devices") {
        string data;
        auto r = rpc_daemon->get_peer().call_list_devices(data);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << "seeds:\n";
        lock.os << data << '\n';
        return ok;
    }
    if (command == "ping") {
        string ans;
        auto r = rpc_daemon->get_peer().call_ping("", ans);
        if (is_ko(r)) {
            return r;
        }
        screen::lock_t lock(scr, interactive);
        lock.os << ans << '\n';
        return ok;
    }
    if (command == "compilance_report") {
        return KO_40322;
    }
    if (command == "h" || command == "-h" || command == "help" || command == "-help" || command == "--help") {
        screen::lock_t lock(scr, interactive);
        help(p, lock.os);
        return ok;
    }
    return KO_11000;
}

ko c::exec_online(const string& cmd, shell_args& args) {
    if (cmd.empty()) {
        auto r = "KO 11000 Empty command.";
        log(r);
        return r;
    }
    auto r = exec_online1(cmd, args);
    return r;
}

ko c::exec_offline(const string& cmd, shell_args& args) {
    log("exec_offline", cmd);
    if (cmd == "id") {
        print_id();
        return ok;
    }
    if (cmd == "version") {
        scr << us::vcs::version() << '\n';
        return ok;
    }
    if (cmd == "device_id") {
        auto r = us::gov::io::cfg1::load(p.get_home_wallet_rpc_client(), true);
        if (is_ko(r.first)) {
            return r.first;
        }
        scr << r.second->keys.pub << '\n';
        delete r.second;
        return ok;
    }
    if (cmd == "mine_public_key") {
        string pattern = args.next<string>();
        screen::lock_t lock(scr, interactive);
        return mine_public_key(pattern, lock.os);
    }
    if (cmd == "digest") {
        string file = args.next<string>();
        screen::lock_t lock(scr, interactive);
        return digest_file(file, lock.os);
    }
    if (cmd == "hash") {
        string cmd = args.next<string>();
        if (cmd == "add") {
            using hasher_t = us::gov::crypto::ripemd160;
            using hash_t = hasher_t::value_type;
            hash_t h1 = args.next<hash_t>();
            hash_t h2 = args.next<hash_t>();
            if (h1.is_zero() || h2.is_zero()) {
                auto r = "KO 54088 add requires two valid hashes.";
                log(r);
                return r;
            }
            hasher_t h;
            h.write(h1);
            h.write(h2);
            hash_t result;
            h.finalize(result);
            screen::lock_t lock(scr, interactive);
            lock.os << result << '\n';
            return ok;
        }
        auto r = "KO 54098 Invalid hash command. Valid are: add.";
        log(r);
        return r;
    }
    if (cmd == "h" || cmd == "-h" || cmd == "help" || cmd == "-help" || cmd == "--help") {
        screen::lock_t lock(scr, interactive);
        help(p, lock.os);
        return ok;
    }
    if (cmd == "show") {
        string cmd = args.next<string>();
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
        return "KO 70693 Invalid command show";
    }
    if (cmd.empty()) {
        if (interactive) {
            screen::lock_t lock(scr, interactive);
            help(p, lock.os);
            return ok;
        }
    }
    log("not an hmi offline command");
    return KO_91810;
}

ko c::exec(const string& command_line) {
    log("exec line", command_line);
    auto args = shell_args(command_line);
    auto cmd = args.next<string>();
    auto r = exec_offline(cmd, args);
    if (r == KO_91810) {
        r = exec_online(cmd, args);
    }
    return r;
}

void c::interactive_shell() {
    assert(p.cmd.empty());
    interactive = true;
    bool was_mute = scr.mute;
    scr.set_mute(false);
    {
        screen::lock_t lock(scr, interactive);
        if (p.banner) {
            banner(p, lock.os);
            lock.os << "\ndevice public key is " << cfg->keys.pub << " address " << cfg->keys.pub.hash() << '\n';
        }
        lock.os << "Connecting to ";
        p.connecting_to(lock.os);
        lock.os << '\n';
        lock.os << "Type h or help.\n";
    }
    while (rpc_daemon->is_active()) {
        {
            screen::lock_t lock(scr, false);
            lock.os << "wallet> "; lock.os.flush();
        }
        string line;
        getline(cin, line);
        us::gov::io::cfg0::trim(line);
        if (line == "q" || line == "exit" || !rpc_daemon->is_active()) {
            scr << "quitting...\n";
            break;
        }
        auto r = exec(line);
        if (is_ko(r)) {
            screen::lock_t lock(scr, false);
            lock.os << rpc_daemon->rewrite(r) << '\n';
        }
    }
    interactive = false;
    scr.set_mute(was_mute);
}

string c::rewrite(ko r) const {
    if (r == KO_11000) {
        return string(r) + "; Type -h for help.";
    }
    if (rpc_daemon) return rpc_daemon->rewrite(r);
    return "";
}

string c::run_() {
    log("run cmd=", p.cmd);
    {
        if (!p.cmd.empty()) {
            ko r = exec_offline(p.cmd, p.args);
            if (r != KO_91810) {
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
            ko r = exec_online(p.cmd, p.args);
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
    if (p.local) {
        string keyfile = p.get_home() + "/wallet/keys";
        scr << "keyfile " << keyfile << '\n';
        us::wallet::wallet::algorithm algo(keyfile);
        ostringstream os;
        algo.list(1, os);
        scr << os.str() << '\n';
        return "";
    }

    auto r = run_();
    if (!r.empty()) {
        screen::lock_t lock(scr, false);
        lock.os << r << '\n';
    }
    return r;
}

c::dispatcher_t::dispatcher_t(c& i): m(i) {
}

bool c::dispatcher_t::dispatch(us::gov::socket::datagram* d) {
    log("dispatcher_t::dispatch", d->service);
    using us::wallet::trader::trader_t;
    using us::wallet::trader::trader_protocol;
    using trader_protocol_wf = us::wallet::trader::workflow::trader_protocol;
    using pushman = us::gov::relay::pushman;
    using us::gov::io::screen;
    auto& scr = m.scr;
    static constexpr const char* pfx = "   ";
    if (scr.is_capturing()) {
        scr << '\n';
    }
    switch (d->service) {
        case us::gov::protocol::relay_push: {
            us::wallet::cli::rpc_peer_t::push_in_dst_t o_in;
            {
                us::gov::io::blob_reader_t reader(*d);
                auto r = reader.read(o_in);
                if (is_ko(r)) {
                    scr << "HMI " << this << ": [D " << d->service << "] " << r << '\n';
                    break;
                }
            }
            log("dispatcher_t::dispatch", "relay_push", o_in.tid, o_in.code, o_in.blob.size());
            switch(o_in.code) {
                case trader_t::push_trade: {
                    log("trader_t::push_trade ", o_in.tid);
                    {
                        screen::lock_t lock(scr, m.interactive);
                        lock.os << "New trade. " << o_in.tid << '\n';
                    }
                    m.new_trade(o_in.tid);
                }
                break;
                case trader_t::push_killed: {
                    log("trader_t::kill_trade ", o_in.tid);
                    {
                        screen::lock_t lock(scr, m.interactive);
                        lock.os << "Trade " << o_in.tid << " has been killed.\n";
                    }
                    m.leave_trade(o_in.tid);
                }
                break;
            }
            if (m.cur == o_in.tid) {
                log("dispatch push blob sz", o_in.tid, "blob_sz", o_in.blob.size(), "code", o_in.code);
                switch (o_in.code) {
                    case trader_t::push_help:
                    case us::gov::relay::pushman::push_ko: {
                        log("recv push_ko");
                        string payload;
                        auto r = us::gov::io::blob_reader_t::parse(o_in.blob, payload);
                        if (is_ko(r)) {
                            screen::lock_t lock(scr, m.interactive);
                            lock.os << "HMI " << this << " " << r << '\n';
                            break;
                        }
                        screen::lock_t lock(scr, m.interactive);
                        lock.os << "## " << payload << '\n';
                    }
                    case us::gov::relay::pushman::push_ok: {
                        log("recv push_ok");
                        string payload;
                        auto r = us::gov::io::blob_reader_t::parse(o_in.blob, payload);
                        if (is_ko(r)) {
                            screen::lock_t lock(scr, m.interactive);
                            lock.os << "HMI " << this << " " << r << '\n';
                            break;
                        }
                        screen::lock_t lock(scr, m.interactive);
                        lock.os << payload << '\n';
                    }
                    break;
                    case trader_t::push_data: {
                        log("trader_t::push_data ", o_in.tid);
                        string payload;
                        auto r = us::gov::io::blob_reader_t::parse(o_in.blob, payload);
                        if (is_ko(r)) {
                            screen::lock_t lock(scr, m.interactive);
                            lock.os << r << '\n';
                            break;
                        }
                        {
                            lock_guard<mutex> lock(m.assets_mx);
                            m.data.from(payload);
                        }
                        screen::lock_t lock(scr, m.interactive);
                        lock.os << "Data:\n";
                        m.data.dump(pfx, lock.os);
                    }
                    break;
                    case trader_t::push_chat: {
                        log("trader_t::push_chat ", o_in.tid);
                        chat_t chat;
                        auto r = us::gov::io::blob_reader_t::parse(o_in.blob, chat);
                        if (is_ko(r)) {
                            screen::lock_t lock(scr, m.interactive);
                            lock.os << r << '\n';
                            break;
                        }
                        screen::lock_t lock(scr, m.interactive);
                        lock.os << "Chat:\n";
                        chat.dump("   ", lock.os);
                    }
                    break;
                    case trader_t::push_roles: {
                        log("trader_t::push_roles ", o_in.tid);
                        lock_guard<mutex> locka(m.assets_mx);
                        auto r = us::gov::io::blob_reader_t::parse(o_in.blob, m.roles);
                        if (is_ko(r)) {
                            screen::lock_t lock(scr, m.interactive);
                            lock.os << r << '\n';
                            break;
                        }
                        screen::lock_t lock(scr, m.interactive);
                        lock.os << "Matching roles:\n";
                        m.roles.dump(pfx, lock.os);
                    }
                    break;
                    case trader_t::push_roles_mine: {
                        roles_t roles;
                        auto r = us::gov::io::blob_reader_t::parse(o_in.blob, roles);
                        if (is_ko(r)) {
                            screen::lock_t lock(scr, m.interactive);
                            lock.os << r << '\n';
                            break;
                        }
                        screen::lock_t lock(scr, m.interactive);
                        log("trader_t::push_roles_mine ", o_in.tid);
                        roles.dump("My roles: ", lock.os);
                    }
                    break;
                    case trader_t::push_roles_peer: {
                        roles_t roles;
                        auto r = us::gov::io::blob_reader_t::parse(o_in.blob, roles);
                        if (is_ko(r)) {
                            screen::lock_t lock(scr, m.interactive);
                            lock.os << r << '\n';
                            break;
                        }
                        screen::lock_t lock(scr, m.interactive);
                        log("trader_t::push_roles_peer ", o_in.tid);
                        roles.dump("Peer roles: ", lock.os);
                    }
                    break;
                    case trader_t::push_qr_peer: {
                        bookmarks_t bookmarks;
                        auto r = bookmarks.read(o_in.blob);
                        if (is_ko(r)) {
                            screen::lock_t lock(scr, m.interactive);
                            lock.os << "HMI " << this << " " << r << '\n';
                            break;
                        }
                        screen::lock_t lock(scr, m.interactive);
                        lock.os << "Peer's shared bookmarks:\n";
                        bookmarks.dump("   ", lock.os);
                    }
                    break;
                    case trader_t::push_qr_mine: {
                        bookmarks_t bookmarks;
                        auto r = bookmarks.read(o_in.blob);
                        if (is_ko(r)) {
                            screen::lock_t lock(scr, m.interactive);
                            lock.os << "HMI " << this << " " << r << '\n';
                            break;
                        }
                        screen::lock_t lock(scr, m.interactive);
                        lock.os << "Bookmarks I share:\n";
                        bookmarks.dump("   ", lock.os);
                    }
                    break;
                    case trader_t::push_log: {
                        string payload;
                        auto r = us::gov::io::blob_reader_t::parse(o_in.blob, payload);
                        if (is_ko(r)) {
                            screen::lock_t lock(scr, m.interactive);
                            lock.os << "HMI " << this << " " << r << '\n';
                            break;
                        }
                        screen::lock_t lock(scr, m.interactive);
                        log("trader_t::push_log ", o_in.tid);
                        lock.os << "Trade Log:\n" << payload << "\n";
                    }
                    break;
                    case trader_protocol_wf::push_workflow_item: {
                        screen::lock_t lock(scr, m.interactive);
                        log("trader_t::push_workflow_item ", o_in.tid);
                        lock.os << "Workflow item.\n";
                    }
                    break;
                    case trader_protocol_wf::push_doc: {
                        string payload;
                        auto r = us::gov::io::blob_reader_t::parse(o_in.blob, payload);
                        if (is_ko(r)) {
                            screen::lock_t lock(scr, m.interactive);
                            lock.os << "HMI " << this << " " << r << '\n';
                            break;
                        }
                        screen::lock_t lock(scr, m.interactive);
                        log("trader_t::push_doc ", o_in.tid);
                        lock.os << "Workflow doc:\n" << payload << "\n";
                    }
                    break;
                    case us::wallet::trader::trader_protocol::push_logo: {
                        log("trader_protocol::push_logo ", o_in.tid);
                        screen::lock_t lock(scr, m.interactive);
                        lock.os << "Received logo file of size " << o_in.blob.size() << " bytes.\n";
                    }
                    break;
                    case us::wallet::trader::trader_protocol::push_ico: {
                        log("trader_protocol::push_ico ", o_in.tid);
                        screen::lock_t lock(scr, m.interactive);
                        lock.os << "Received ico file of size " << o_in.blob.size() << " bytes.\n";
                    }
                    break;
                    case us::wallet::trader::workflow::trader_protocol::push_redirects: {
                        log("trader_protocol::push_redirects ", o_in.tid);
                        bookmarks_t bookmarks;
                        auto r = bookmarks.read(o_in.blob);
                        if (is_ko(r)) {
                            screen::lock_t lock(scr, m.interactive);
                            lock.os << "HMI " << this << " " << r << '\n';
                            break;
                        }
                        screen::lock_t lock(scr, m.interactive);
                        lock.os << "Remote redirects:\n";
                        bookmarks.dump("   ", lock.os);
                    }
                    break;
                    case trader_t::push_bookmarks: {
                        log("trader_t::push_bookmarks ");
                        bookmarks_t bookmarks;
                        auto r = bookmarks.read(o_in.blob);
                        if (is_ko(r)) {
                            screen::lock_t lock(scr, m.interactive);
                            lock.os << "HMI " << this << " " << r << '\n';
                            break;
                        }
                        screen::lock_t lock(scr, m.interactive);
                        lock.os << "Bookmarks:\n";
                        bookmarks.dump("   ", lock.os);
                    }
                    break;
                    case us::wallet::wallet::local_api::push_txlog: {
                        log("push_txlog");
                        us::wallet::wallet::index_t index;
                        auto r = index.read(o_in.blob);
                        if (is_ko(r)) {
                            screen::lock_t lock(scr, m.interactive);
                            lock.os << "HMI " << this << " " << r << '\n';
                            break;
                        }
                        screen::lock_t lock(scr, m.interactive);
                        lock.os << "transfer log:\n";
                        index.dump("   ", lock.os);
                    }
                    break;
                }
            }
        }
        break;
        default:
            scr << "HMI " << this << ": [D " << d->service << "] KO 79968 Not handled.\n";
    }
    if (scr.is_capturing()) scr.print_prompt();
    delete d;
    return true;
}

void c::set_mute(bool m) {
    scr.mute = m;
}

us::gov::socket::datagram::dispatcher_t* c::create_dispatcher() {
    return new dispatcher_t(*this);
}

#if CFG_FCGI == 1
void c::run_fcgi(const params& p) {
    log("run_fcgi");
cerr << "DBG 1" << endl;
    assert(fcgi == nullptr);
    try {
cerr << "DBG 2" << endl;
        fcgi = new Fastcgipp::Manager<fcgi_t>();
//cerr << "DBG 3" << endl;
        //fcgi->setupSignals();
cerr << "DBG 4" << endl;
        if (!fcgi->listen("127.0.0.1", "9000")) {
cerr << "DBG 5" << endl;
            cerr << "Cannot listen on FCGI port.\n";
cerr << "DBG 6" << endl;
            exit(1);
        }
cerr << "DBG 7" << endl;
        thread th([&] {
cerr << "DBG 8" << endl;
            fcgi->start();
cerr << "DBG 9" << endl;
            fcgi->join();
cerr << "DBG 10" << endl;
            delete fcgi;
            fcgi = nullptr;
cerr << "DBG 11" << endl;
            cerr << "fcgi interface is down.\n";
        });
cerr << "DBG 12" << endl;
        th.detach();
cerr << "DBG 13" << endl;
    }
    catch (std::exception& ex) {
cerr << "DBG 14" << endl;
        delete fcgi;
        fcgi = nullptr;
cerr << "DBG 15" << endl;
        cerr << ex.what() << '\n';
cerr << "DBG 16" << endl;
    }
cerr << "DBG 17" << endl;
}
#endif

/*
#include <ncurses.h>

int c::run_ncurses(shell_args args, params& p ) {
  //  signal(SIGWINCH, resizeHandler);
    initscr();
    printw("other.wallet");
    refresh();
    getch();
    endwin();
    return 0;
}
*/

ko c::digest_file(const string& file, ostream& os) {
    log("digest_file");
    auto r = us::gov::crypto::ripemd160::digest_file(file);
    if (is_ko(r.first)) return r.first;
    os << r.second << '\n';
    return ok;
}

ko c::mine_public_key(const string& pattern, ostream& os) {
    log("mine_public_key");
    while(true) {
        gov::crypto::ec::keys k = gov::crypto::ec::keys::generate();
        string pubkey = k.pub.to_b58();
        if (pubkey.find(pattern) != string::npos) {
            os << "Private key: " << k.priv.to_b58() << '\n';
            os << "Public key: " << k.pub.to_b58() << '\n';
            return ok;
        }
    }
    return ok;
}

void c::process_cleanup() {
    us::gov::cli::hmi::process_cleanup();
}

