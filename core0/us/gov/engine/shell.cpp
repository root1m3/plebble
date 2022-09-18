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
#include "shell.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>
#include <cassert>
#include <sstream>
#include <vector>

#include <us/gov/stacktrace.h>
#include <us/gov/config.h>
#include <us/gov/vcs.h>
#include <us/gov/types.h>
#include <us/gov/io/cfg.h>
#include <us/gov/crypto/base58.h>
#include <us/gov/socket/multipeer/server.h>
#include <us/gov/dfs/daemon_t.h>
#include <us/gov/engine/db_t.h>
#include <us/gov/engine/auth/app.h>
#include <us/gov/sys/app.h>
#include <us/gov/cash/app.h>

#include "daemon_t.h"
#include "db_analyst.h"
#include "types.h"

#define loglevel "gov/engine"
#define logclass "shell"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::shell;

c::shell(daemon_t& d): d(d) {
}

void c::help(ostream& os) const {
    os << PLATFORM << "-gov ; Introspective Shell. Copyright (C) " << CFG_COPYRIGHT_LINE << '\n';
    os << CFG_COPYRIGHT_LINE2 << '\n';
    os << CFG_COPYRIGHT_LINE3 << '\n';
    os << "  Channel " << d.peerd.channel << '\n';
    os << "  Public key " << d.id.pub << '\n';
    os << "  Address " << d.id.pub.hash() << '\n';
    os << "  SW Version ";
    us::vcs::version(os);
    os << '\n';
        #ifdef DEBUG
            os << "  Warning: this is a debug build.\n";
        #else
            os << "  This is an optimized build.\n";
        #endif
        #if CFG_LOGS == 1
            os << "  Warning: writing log files in " << LOGDIR << '\n';
        #else
            os << "  Logs: disabled.\n";
        #endif

    os << "Commands:\n";
    os << "  h|-h|help|-help|--help This help.\n";
    os << "  home                  Print current home directory.\n";
    os << "  s|server              Print networking info.\n";
    os << "  y|syncd               Print data sync info.\n";
    os << "  n|chain               Print current chain.\n";
    os << "  v|votes               Print votes\n";
    os << "  c|cycle               Print cycle.\n";
    os << "  b|blocks              Print blocks info.\n";
    os << "  g|grid                Print neighbours grid.\n";
    os << "  gw                    Print grid (2).\n";
    os << "  layoffdays [days]     Print/Set days a node can be off duty.\n";
    os << "  clean_blocks          Deletes orphaned old blocks.\n";
    os << "  clean_files           Deletes non-listed files.\n";
    os << "  tip <hash>            sync with this tip.\n";
    os << "  ns|net_status         Print raw network status.\n";
    os << "  ns2                   Print formatted network status.\n";
    os << "  tracked_tx            Print info about tx being tracked.\n";
    os << "  watch                 Print monitoring info.\n";
    os << "  logline               Print a line for the log.\n";
    os << "  uptime                process age\n";
    #if CFG_COUNTERS == 1
        os << "  p|performances        Print performances\n";
    #endif
    os << "  apps                  List apps.\n";
    os << "  app <id>              Enter app shell.\n";
    os << "  seeds                 Print list of seed nodes.\n";
    os << "  a|add_node <address>  Add a seed node.\n";
    os << "  bl          Show blacklist.\n";
    os << "  cbl         Clear blacklist.\n";
    os << "  gc          Show garbage collector.\n";
    os << "  test_node <host:port>\n";
    os << "  data <addr>           Prints data stored in the address specified.\n";
    os << "  print_kv_b64 <addr>          Prints same data for automatic processing.\n";
    os << "  update_dfs_index      fetch missing files.\n";
    os << "  consensus             print consensus info.\n";
    os << "  list_files <address> <path>      list files in address matching path.\n";
}

ko c::command(const string& cmdline, ostream& os) {
    istringstream is(cmdline);
    return command(is, os);
}

struct test_client: peer_t {
    using b = peer_t;

    test_client(net_daemon_t& d, ostream& fos): b(d, -1) {
        char name[32];
        strcpy(name, "/tmp/test_connection-XXXXXX");
        close(mkstemp(name));
        fos << name << '\n';
        os = new ofstream(name);
        *os << "client created\n";
    }

    ~test_client() override {
        *os << "client destroyed\n";
        delete os;
    }

    void disconnectx(channel_t channel, seq_t seq, const reason_t& reason) final override {
        *os << "disconnect\n";
        print_stacktrace_release(*os);
        b::disconnectx(channel, seq, reason);
    }

    void verification_completed(pport_t rpport, pin_t pin) override {
        *os << "verification_completed\n";
        b::verification_completed(rpport, pin);
        if (!verification_is_fine()) {
            *os << "verification_not_fine\n";
        }
        *os << "added to grid\n";
    }

    ko authorizeX(const pub_t& p, pin_t pin) override {
        auto r = b::authorizeX(p, pin);
        *os << "authorize? " << (r == ok ? "ok" : r) << '\n';
        return r;
    }

    ofstream* os;
};

bool c::test_node(const string& n, ostream& os) {
    auto h = us::gov::io::cfg::parse_host(n);
    if (h.first == 0) {
        os << "incorrect addr " << n << '\n';
        return false;
    }
    auto p = new test_client(d.peerd, os);
    bool b = d.peerd.grid_connect_test(p, h, os);
    if (!b) {
        os << "could not connect\n";
        return false;
    }
    return true;
}

ko c::command(istream& is, const string& cmd, ostream& os) {
    log("command", cmd);
    if (cur_app != -1) {
        bool b = d.shell_command(cur_app, is, os);
        if (!b) {
            cur_app = -1;
        }
        return ok;
    }
    if (cmd == "h" || cmd == "-h" || cmd == "help" || cmd == "-help" || cmd == "--help") {
        help(os);
        return ok;
    }
    if (cmd == "server" || cmd == "s") {
        string pfx;
        d.peerd.dump(pfx, os);
        return ok;
    }
    if (cmd == "bl") {
        dynamic_cast<const socket::multipeer::server&>(d.peerd).ban.dump(os);
        return ok;
    }
    if (cmd == "cbl") {
        dynamic_cast<us::gov::socket::multipeer::server&>(d.peerd).ban.clear();
        os << "emptied ban list.\n";
        return ok;
    }
    if (cmd == "gc") {
        dynamic_cast<us::gov::socket::daemon_t&>(d.peerd).peers.gc.dump(os);
        return ok;
    }
    if (cmd == "syncd" || cmd == "y") {
        d.syncd.dump(os);
        return ok;
    }
    if (cmd == "chain" || cmd == "n") {
        db_analyst::print_chain(d.blocksdir, d.syncd.tip(), os);
        return ok;
    }
    if (cmd == "tip") {
        hash_t tip;
        is >> tip;
        if (!tip.is_zero()) {
            d.clear();
            d.syncd.update(tip, d.get_last_delta_imported());
            os << "updated syncd to sync on " << tip << '\n';
        }
        else {
            os << "unreadable hash " << tip << '\n';
        }
        return ok;
    }
    if (cmd == "layoffdays") {
        uint16_t lod{0};
        is >> lod;
        if (lod >= 2) {
            d.db->auth_app->layoff_days = lod;
        }
        else {
            os << "value should be >=2\n";
        }
        os << "Nodes unseen for " << d.db->auth_app->layoff_days << " are laid off.\n";
        return ok;
    }
    if (cmd == "uptime") {
        os << "uptime " << d.uptime() << '\n';
        return ok;
    }
    if (cmd == "test_node") {
        string hp;
        is >> hp;
        test_node(hp, os);
        return ok;
    }
    if (cmd == "clean_blocks") {
        int n = d.clean_orphaned_blocks();
        os << n << " files deleted.\n";
        return ok;
    }
    if (cmd == "clean_files") {
        int n = d.clean_unlisted_files();
        os << n << " files deleted.\n";
        return ok;
    }
    if (cmd == "consensus") {
        os << "last_vote " << d.last_vote << " last_consensus " << d.last_consensus << '\n';
        return ok;
    }
    if (cmd == "list_files") {
        hash_t addr;
        string path;
        is >> addr;
        is >> path;
        if (is.fail()) {
            os << "KO 62008 Invalid input\n";
        }
        else {
            lock_guard<mutex> lock(d.mx_import);
            d.db->cash_app->db.list_files(addr, path, os);
        }
        return ok;
    }
    if (cmd == "logline") {
        d.logline(os);
        d.syncd.logline(os);
        #if CFG_COUNTERS == 1
            os << "{LE " << d.counters.lost_evidences << ' ';
            os << "PE " << d.counters.processed_evidences << ' ';
            os << "LB " << d.counters.processed_evidences_last_block << ' ';
            os << "CB " << d.counters.processed_evidences_cur_block << ' ';
            os << "LV " << d.last_vote << ' ';
            os << "LC " << d.last_consensus << ' ';
        #endif
        os << "} ";
        return ok;
    }
    if (cmd == "watch") {
        d.watch(os);
        return ok;
    }
    if (cmd == "b" || cmd == "blocks") {
        d.fsinfo(false, os);
        os << "blockchain depth " << d.depth << '\n';
        return ok;
    }
    if (cmd == "g" || cmd == "grid") {
//        d.peerd.grid.dump(os);
        d.peerd.clique.dump(os);
        return ok;
    }
    if (cmd == "gw") {
        os << "neighbours:\n";
//        d.peerd.grid.watch(os);
        d.peerd.watch(os);
        os << "devices:\n";
        d.peerd.grid_dev.watch(os);
        return ok;
    }
    if (cmd == "apps") {
        d.list_apps("", os);
        return ok;
    }
    if (cmd == "app") {
        int id = -1;
        is >> id;
        if (id < 0) {
            os << "(app) Missing argument app id\n";
        }
        else {
            cur_app = id;
        }
        return d.shell_command(cur_app, is, os);
    }
    if (cmd == "a" || cmd == "add_node") {
        string ip;
        is >> ip;
        if (!ip.empty()) {
            auto host = io::cfg::parse_host(ip);
            if (!us::gov::socket::client::is_valid_ip(host.first, d.peerd.channel)) {
                os << "KO 20199 Invalid IP4 address for current channel.\n";
            }
            else {
                d.peerd.add_seed_node(host);
                os << "Added seed node " << us::gov::socket::client::ip4_decode(host.first) << ":" << host.second << '\n';
            }
        }
        else {
            os << "KO 86996 Syntax is ip4address[:tcpport]\n";
        }
        return ok;
    }
    if (cmd == "seeds") {
        os << "Seed nodes\n";
        for (auto& i: d.peerd.seed_nodes) {
            os << us::gov::socket::client::ip4_decode(i.second.net_address) << ":" << i.second.port << '\n';
        }
        return ok;
    }
    #if CFG_COUNTERS == 1
    if (cmd == "p" || cmd == "performances") {
        d.print_performances(os);
        return ok;
    }
    #endif
    if (cmd == "home") {
        os << d.home << '\n';
        return ok;
    }
    if (cmd == "v" || cmd == "votes") {
        d.votes.dump("",os);
        #if CFG_COUNTERS == 1
            d.silent.dump(os);
        #endif
        return ok;
    }
    if (cmd == "c" || cmd == "cycle") {
        d.print_map(os);
        os << '\n';
        return ok;
    }
    if (cmd == "ns" || cmd == "net_status") {
        //os << streams_version << ' ';
        lock_guard<mutex> lock(d.mx_import);
        blob_t blob;
        d.db->sys_app->db.write(blob);
        os << crypto::b58::encode(blob) << '\n';
        return ok;
    }
    if (cmd == "ns2") {
        lock_guard<mutex> lock(d.mx_import);
        d.db->sys_app->db.dump("", os);
        os << '\n';
        return ok;
    }
    if (cmd == "tracked_tx") {
        d.evt.dump(os);
        os << '\n';
        return ok;
    }
    if (cmd == "data") {
        hash_t addr;
        is >> addr;
        lock_guard<mutex> lock(d.mx_import);
        return d.db->cash_app->db.print_data(addr, os);
    }
    if (cmd == "print_kv_b64") {
        hash_t addr;
        is >> addr;
        lock_guard<mutex> lock(d.mx_import);
        d.db->cash_app->db.print_kv_b64(addr, os);
        return ok;
    }
    if (cmd == "update_dfs_index") {
        d.update_dfs_index();
        dynamic_cast<const us::gov::dfs::daemon_t&>(d.peerd).dump(" dfs> ",os);
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
            os << l << '\n';
            return ok;
        }
        return "KO 74693 Invalid command show";
    }

    auto r = "KO 91000 Invalid command.";
    log(r);
    return r;
}

ko c::command(istream& is, ostream& os) {
    if (is.tellg() == 0) {
        cur_app = -1;
    }
    while (is.good()) {
        string cmd;
        is >> cmd;
        if (cmd.empty()) break;
        auto r = command(is, cmd, os);
        if (is_ko(r)) {
            return r;
        }
    }
    return ok;
}

