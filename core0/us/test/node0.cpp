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
#include <string>

#include <us/gov/config.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/engine/daemon_t.h>
#include <us/gov/cli/hmi.h>
#include <us/gov/io/cfg.h>
#include <us/gov/io/cfg1.h>
#include <us/gov/cash/tx_t.h>
#include <us/gov/cash/ttx.h>
#include <us/gov/config.h>

#include <us/wallet/cli/params.h>
#include <us/wallet/wallet/algorithm.h>
#include <us/wallet/cli/hmi.h>
#include <us/wallet/engine/devices_t.h>
#include <us/wallet/engine/daemon_t.h>

#define loglevel "test"
#define logclass "node"
#include "logs.inc"

#include "assert.inc"

namespace us::test {

using namespace std;
using us::gov::io::cfg_daemon;
using us::gov::io::shell_args;
using us::gov::socket::datagram;
using us::ko;

struct govx_t: gov::cli::hmi {
    using b = gov::cli::hmi;
    using b::hmi;
};

using walletx_t = wallet::cli::hmi;

namespace {
    string logdir;
}

void test_disabled_sysop(const string& homedir) {
    tee("================test_disabled_sysop", homedir);
    govx_t::params gp;
    gp.daemon = true;
    gp.port = gp.pport = 22222;
    gp.edges = 5;
    gp.workers = 2;
    gp.shell = false;
    gp.homedir = homedir;
    govx_t govdaemon(gp, cout);
    #if CFG_LOGS == 1
        govdaemon.logdir = logdir + "/gov_daemon";
    #endif
    assert(govdaemon.start() == ok);
    this_thread::sleep_for(100ms);
    {
        tee("new rpc client");
        govx_t::params p;
        p.port = 22222;
        p.sysophost = "127.0.0.1";
        p.homedir = gp.homedir;
        {
            govx_t hmi(p, cout);
            #if CFG_LOGS == 1
                hmi.logdir = logdir + "/no_key/gov_cli";
            #endif
            assert(hmi.start() == us::gov::io::cfg1::KO_97832);
        }
        govdaemon.write_rpc_client_key();
        {
            govx_t hmi(p, cout);
            #if CFG_LOGS == 1
                hmi.logdir = logdir + "/gov_cli";
            #endif
            auto r = hmi.start();
            if (is_ko(r)) {
                cout << hmi.rewrite(r) << endl;
            }
            assert(r == ok);

            hmi.join();
            //assert(hmi.ret == us::gov::cli::rpc_daemon_t::KO_20195);
            //assert(hmi.ret==govx_t::KO_20195);
        }
    }
    govdaemon.stop();
    govdaemon.join();
}

void test_gov_command(govx_t& hmi, const string& cmd, ko expected) {
    tee("== test_gov_command", cmd);
    auto r = hmi.exec(cmd);
    cout << "exec: " << cmd << " expected " << (expected == ok ? "ok" : expected) << " got " << (r == ok ? "ok" : r) << '\n';
    if (r != expected) {
        assert(false);
    }
}

void test_sysop(const string& homedir) {
    tee("====test_sysop", homedir);
    govx_t::params gp;
    gp.daemon = true;
    gp.port = gp.pport = 22222;
    gp.edges = 5;
    gp.workers = 2;
    gp.shell = true;
    gp.channel = 123;
    gp.homedir = homedir;
    govx_t govxd(gp, cout);
    #if CFG_LOGS == 1
        govxd.logdir = logdir + "/gov_daemon";
    #endif
    assert(govxd.start() == ok);
    this_thread::sleep_for(100ms);
    {
        govx_t::params p;
        p.daemon = false;
        p.port = 22222;
        p.sysophost = "127.0.0.1";
        p.channel = 123;
        p.homedir = gp.homedir;

        govx_t hmi(p, cout);
        #if CFG_LOGS == 1
            hmi.logdir = logdir + "/gov_rpc_cli";
        #endif
        assert(hmi.start() == ok);
//        assert(hmi.rpc_daemon->wait_connected() == ok);
        assert(hmi.rpc_peer != nullptr);

        test_gov_command(hmi, "sysop h", ok);
        test_gov_command(hmi, "sysop uptime", ok);
        test_gov_command(hmi, "sysop app 20 db", ok);
        test_gov_command(hmi, "sysop app 30 db", ok);
        test_gov_command(hmi, "sysop app 40 db", ok);

        hmi.stop();
        hmi.join();
    }
    govxd.stop();
    govxd.join();
}


void test_gov_rpc_keys(const string& homedir) {
    using us::gov::io::cfg_id;
    auto k1 = cfg_id::load_sk(homedir);
    auto k2 = cfg_id::load_sk(homedir + "/rpc_client");
    assert(k1.first == ok);
    assert(!k1.second.is_zero());
    assert(k2.first == ok);
    assert(!k2.second.is_zero());
    assert(k1.second.to_b58() == k2.second.to_b58());
}

void test_wallet_rpc_keys(const string& homedir) {
    cout << "test_wallet_rpc_keys home=" << homedir << '\n';
    us::wallet::engine::devices_t d(homedir);
    d.dump(cout);
    cout << "Random device not authorized\n";
    {
        using request_data_t = us::gov::id::request_data_t;
        request_data_t request_data;
        auto r = d.authorize(us::gov::crypto::ec::keys::generate().pub, 0, request_data);
        assert(r == us::wallet::engine::devices_t::KO_30291);
    }
    cout << "Console entry.\n";
    string privk;
    assert(us::gov::io::read_text_file_(homedir + "/rpc_client/k", privk) == ok);
    us::gov::crypto::ec::keys::priv_t sk(privk);
    cout << "  privk '" << privk << "'\n";
    cout << "  privk '" << sk << "'\n";
    cout << "  privk '" << sk.to_b58() << "'\n";
    us::gov::io::cfg0::trim(privk);
    assert(privk == sk.to_b58());
    auto pubk = us::gov::crypto::ec::keys::get_pubkey(sk);
    cout << "  pubkey '" << pubk << "'\n";
    cout << "  pubkeyh '" << pubk.hash() << "'\n";
    assert(d.size() == 1);
    using request_data_t = us::gov::id::request_data_t;
    request_data_t request_data;
    auto r = d.authorize(pubk, 0, request_data);
    assert(r == ok); //authorized
    assert(request_data.empty()); //given subhome
}

govx_t* start_gov_daemon(const string& homedir) {
    log("starting gov daemon");
    govx_t::params p;
    p.daemon = true;
    p.port = 22222;
    p.pport = 22222;
    p.edges = 5;
    p.workers = 2;
    p.devices = 2; //1 for walletd + one for wallet rpccli
    p.shell = true;
    p.channel = 123;
    p.homedir = homedir;
    govx_t* govx = new govx_t(p, cout);
    #if CFG_LOGS == 1
        govx->logdir = logdir + "/test_hmi_local/gov_daemon";
    #endif
    assert(govx->start() == us::ok);
    log("started gov daemon");
    this_thread::sleep_for(100ms);
    return govx;
}

walletx_t* start_wallet_daemon(const string& homedir) {
    log("starting wallet daemon", homedir);
    wallet::cli::params p;
    p.daemon = true;
    p.homedir = homedir;
    p.channel = 123;
    p.listening_port = p.published_port = 22223;
    p.backend_host = "localhost";
    p.backend_port = 22222;
    p.downloads_dir = homedir + "/downloads";
    p.logd = logdir + "/test_wallet_rpc/wallet_daemon";
    walletx_t* walletx = new walletx_t(p, cout);
    assert(walletx->start() == ok);
    log("started wallet daemon");
    this_thread::sleep_for(100ms);
    return walletx;
}

void test_wallet_hmi(wallet::cli::hmi& hmi) {
    tee("test_wallet_hmi");
    tee("hmi home", hmi.home);
    system(string("find " + hmi.home).c_str());
    //assert(hmi.daemon != nullptr);
    //tee("hmi devices home", hmi.daemon->devices.get_home());
    tee("exec: list_devices");
    assert(hmi.exec("list_devices") == ok);

    log("T0");
    assert(is_ko(hmi.exec("this command shall fail")));

    log("T1");
    cout << "ping: ";
    assert(is_ok(hmi.exec("ping")));

    log("T2");
    {
        cout << "balance: ";
        auto r = hmi.exec("balance");
        if (is_ko(r)) {
            cout << r << endl;
        }
        assert(is_ok(r));
        //assert(r == us::gov::socket::rendezvous_t::KO_20190);
        //cout << hmi.rpc_peer->lasterror << endl;
        assert(hmi.rpc_peer->lasterror == "");
    }

    log("T3");
    cout << "balance 0: ";
    assert(is_ok(hmi.exec("balance 0")));

    log("T4");
    cout << "balance 1: ";
    assert(is_ok(hmi.exec("balance 1")));

    log("T5");
    cout << "balance 2: ";
    assert(is_ok(hmi.exec("balance 2")));

    cout << '\n';
}

/*
void test_wallet_hmi_local(const string& homedir) {
    cout << "\n\nwallet_hmi_local\n";
    log("starting wallet cli");
    wallet::cli::params p;
    p.daemon = false;
    //p.local = true;
    p.backend_port = 22222;
    p.homedir = homedir;
    p.channel = 123;
    walletx_t x(p,cout);
    x.logdir = logdir + "/test_hmi_local/walletx";
    assert(x.start() == ok);
    log("started wallet cli");
    test_wallet_hmi(x);
    log("stopping wallet cli");
    x.stop();
    x.join();
}
*/

void test_wallet_hmi_rpc(const string& homedir) {
    tee("test_wallet_hmi_rpc", homedir);
    wallet::cli::params p;
    p.daemon = false;
    p.channel = 123;
    p.homedir = homedir;
    p.walletd_host = "localhost";
    p.walletd_port = 22223;
    p.logd = logdir + "/test_hmi_rpc/walletx";
    tee("logs at", p.logd);
    walletx_t x(p, cout);
    assert(x.start() == ok);
    assert(x.daemon == nullptr);
    test_wallet_hmi(x);
    tee("stopping wallet cli");
    x.stop();
    x.join();
    tee("stopped wallet cli");
}

void test_gov_cli_files(const string& h) {
    using us::gov::io::cfg0;
    assert(cfg0::dir_exists(h));
}

void test_wallet_cli_files(const string& h) {
    using us::gov::io::cfg0;
    assert(cfg0::dir_exists(h));
    assert(cfg0::dir_exists(h + "/rpc_client"));
    assert(cfg0::file_exists(h + "/rpc_client/k"));
}

void test_wallet_daemon_files(const string& h) {
    tee("test_wallet_daemon_files", h);
    using us::gov::io::cfg0;
    string d = h;
    assert(cfg0::dir_exists(d));
    d += "/123";
    assert(cfg0::dir_exists(d));
    d += "/wallet";
    assert(cfg0::dir_exists(d));
    assert(cfg0::file_exists(d + "/keys"));
    assert(cfg0::file_exists(d + "/k"));
    assert(cfg0::file_exists(d + "/d"));
    assert(cfg0::dir_exists(d + "/trader"));
    assert(cfg0::dir_exists(d + "/trader/lib"));
    assert(cfg0::dir_exists(d + "/trader/bin"));
    assert(cfg0::dir_exists(d + "/trader/w2w"));
    test_wallet_rpc_keys(d);
}

void test_gov_daemon_files(const string& h) {
    tee("test_gov_daemon_files", h);
    using us::gov::io::cfg0;
    string d = h;
    assert(cfg0::dir_exists(d));
    d += "/123";
    assert(cfg0::dir_exists(d));
    d += "/gov";
    assert(cfg0::dir_exists(d));
    assert(cfg0::file_exists(d + "/nodes.auto"));
    assert(cfg0::dir_exists(d + "/blocks"));
    assert(cfg0::file_exists(d + "/k"));
    assert(cfg0::file_exists(d + "/nodes.distr"));
    assert(cfg0::file_exists(d + "/nodes.manual"));
    assert(cfg0::dir_exists(d + "/locking"));
    assert(cfg0::dir_exists(d + "/fsroot"));
    test_gov_rpc_keys(d);
}

void test_cli_files(const string& h) {
    using us::gov::io::cfg0;
    string d = h;
    assert(cfg0::dir_exists(d));
    d += "/123";
    assert(cfg0::dir_exists(d));
    test_gov_cli_files(d + "/gov");
    test_wallet_cli_files(d + "/wallet");
}

void test_api(const string& homedir) {
    tee("test_api", homedir);
    auto govxd = start_gov_daemon(homedir);
    auto walletxd = start_wallet_daemon(homedir);
cout << "XXXXXX0" << endl;
walletxd->daemon->devices.dump(cout);
    tee("==test_gov_daemon_files");
    test_gov_daemon_files(homedir);
    tee("==test_wallet_daemon_files", homedir);
    test_wallet_daemon_files(homedir);
cout << "XXXXXX" << endl;
walletxd->daemon->devices.dump(cout);

//    log("test_wallet_hmi_local", homedir);
//    test_wallet_hmi_local(homedir);
    tee("==test_wallet_hmi_rpc", homedir);
    test_wallet_hmi_rpc(homedir);
    log("stopping daemons");
    walletxd->stop();
    govxd->stop();
    walletxd->join();
    delete walletxd;
    log("stopped wallet daemon");
    govxd->join();
    delete govxd;
    log("stopped gov daemon");
}

void test_config(const string& homedir) {
    using us::gov::io::cfg_id;
    {
        string dirname = homedir + "/KO_92182_this_dir_shall_not_be_created";
        assert(!cfg_id::dir_exists(dirname));
        auto conf = cfg_id::load(dirname,false);
        assert(conf.first == cfg_id::KO_97832);
        assert(!cfg_id::dir_exists(dirname));
        assert(conf.second == nullptr);
    }
    {
        string dirname = homedir + "/test_config";
        assert(!cfg_id::dir_exists(dirname));
        auto conf = cfg_id::load(dirname,true);
        assert(conf.first == ok);
        assert(cfg_id::dir_exists(dirname));
        assert(conf.second != nullptr);
        delete conf.second;
    }
}

void test_connect_failed(const string& homedir) {
    tee("================test_connect_failed", homedir);
    govx_t::params p;
    p.daemon = false;
    p.port = 12345; //no daemon here
    p.sysophost = "127.0.0.1";
    p.homedir = homedir;
    p.rpc__stop_on_disconnection = true;

    using us::gov::io::cfg_id;
    auto k = us::gov::crypto::ec::keys::generate();
    cfg_id::write_k(p.get_home_gov_rpc_client(), k.priv);

    govx_t hmi(p, cout);
    #if CFG_LOGS == 1
        hmi.logdir = logdir + "/test_connect_failed";
    #endif
    tee("start");
    auto r = hmi.start();
    if (is_ok(r)) {
        assert(false);
    }
    assert(r != ok);
    tee("expected rpc_daemon to stop by itself after failing (rpc__stop_on_disconnection)");
    hmi.join();
    tee("ok");
}

#if CFG_LOGS == 0
#undef log_dir
namespace {
    string log_dir() {
        ostringstream os;
        os << "logs/us-test_" << getpid() << "/node";
        return os.str();
    }
}
#endif


void test_node0_main() {
    log_start(logdir, "main");
    tee("=================test_node==main==================");
    string homedir = log_dir() + "/home";
    test_config(homedir);
    test_connect_failed(homedir + "/test_connect_failed");
    test_disabled_sysop(homedir + "/test_disabled_sysop");
    test_sysop(homedir + "/test_sysop");
    cout << "test_api" << endl;
    test_api(homedir + "/test_api");
    tee("=========== end testing node ==main========");
}

void test_node0() {
    tee("=================test_node========================");
    logdir = "node0.cpp__test_node";
    log("starting thread", "logdir", logdir);
    thread th(&test_node0_main); //start new log file
    th.join();
    tee("=========== end testing node ==============");
}

}

