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
#include <us/gov/engine/daemon_t.h>
#include <us/gov/config.h>
#include <us/gov/engine/db_t.h>

#include <us/wallet/engine/daemon_t.h>
#include <us/wallet/engine/rpc_daemon_t.h>
#include <us/wallet/wallet/local_api.h>

#include "node.h"
#include "dispatcher_t.h"

#define loglevel "test"
#define logclass "node"
#include <us/gov/logs.inc>
#include "assert.inc"

using c = us::test::node;
using namespace std;
using hash_t = us::gov::crypto::ripemd160::value_type;

//morello
string c::localip("192.168.42.75"); //dont use 127.0.0.1 , unreachable from mobile when using advertised IPs (IP lookup)

//desktop
//string c::localip("192.168.0.96"); //dont use 127.0.0.1 , unreachable from mobile when using advertised IPs (IP lookup)

//laptop
//string c::localip("192.168.0.106"); //dont use 127.0.0.1 , unreachable from mobile when using advertised IPs (IP lookup)

//mob hotspot
//string c::localip("192.168.43.170"); //dont use 127.0.0.1 , unreachable from mobile when using advertised IPs (IP lookup)

c::node(const string& id, const string& root_homedir, const string& root_logdir, const string& root_vardir, uint16_t gport, uint16_t wport): id(id), gport(gport), wport(wport) {
    ostringstream h;
    h << root_homedir << "/node_" << id;
    ostringstream l;
    l << root_logdir << "/node_" << id;
    ostringstream v;
    v << root_vardir << "/node_" << id;
    homedir = h.str();
    logdir = l.str();
    vardir = v.str();
    tee("constructor node", id, "home:", homedir, "log:", logdir, "var:", vardir);
}

bool c::load_data(const string& r2rhome) {
    return true;
}

string c::ep() const {
    return "hijk";
}

string c::thome() const {
    return homedir + "/123/wallet/trader";
}

void c::create_node(const string& r2rhome) {
    if (!us::gov::io::cfg0::dir_exists("nodes/" + id)) {
        cerr << "KO 68403 missing conf dir for node " << id << endl;
        assert(false);
    }

    ostringstream f;
    f << thome() << "/" << r2rhome;
    us::gov::io::cfg0::ensure_dir(f.str());
    {
        ostringstream cmd;
        cmd << "cp nodes/" << id << "/logo.png  " << f.str() << "/";
        cout << "logotype:" << endl;
        cout << cmd.str() << endl;
        system(cmd.str().c_str());
    }
    {
        ostringstream cmd;
        cmd << "cp nodes/" << id << "/ico.png  " << f.str() << "/";
        cout << "ico:" << endl;
        cout << cmd.str() << endl;
        system(cmd.str().c_str());
    }
    {
        ostringstream cmd;
        cmd << "cp nodes/" << id << "/name  " << f.str() << "/";
        cout << "name:" << endl;
        cout << cmd.str() << endl;
        system(cmd.str().c_str());
    }
    cout << "files created at " << f.str() << '\n';
    system((string("find ") + f.str()).c_str());
}

c::bookmarks_t c::bookmarks() const {
    bookmarks_t bm;
    auto r = wallet_cli->rpc_daemon->get_peer().call_qr(bm);
    assert(r == ok);
    return move(bm);
}

void c::start_govd(bool init_chain) {
    log("starting gov daemon");
    cout << "starting gov daemon " << gport << endl;
    govx_t::params p;
    p.daemon = true;
    p.port = gport;
    p.pport = gport;
    p.edges = 10;
    p.workers = 2;
    p.devices = 2; //1 for walletd + one for wallet rpccli
    p.shell = true;
    p.channel = 123;
    p.homedir = homedir;
    if (init_chain) {
        cout << "starting new chain" << endl;
        ostringstream ep;
        ep << localip << ":" << p.pport;
        pair<ko, us::gov::io::cfg*> r = us::gov::engine::daemon_t::init_chain(p.channel, p.get_home_gov(), ep.str());
        if (is_ko(r.first)) {
            cerr << r.first << endl;
            assert(false);
        }
        cout << "init chain: 1" << endl;
        delete r.second;
        cout << "init chain: 2" << endl;
        cout << "init chain: " << ep.str() << " " <<  p.homedir << endl;
    }
    assert(gov == nullptr);
    gov = new govx_t(p, cout);
    #if CFG_LOGS == 1
        gov->logdir = logdir + "/govd";
    #endif
    assert(gov->start() == us::ok);
    log("started gov daemon");
    gov->daemon->db->auth_app->growth = 10.0;
    assert(is_ok(gov->daemon->wait_ready(3)));
}

void c::create_walletd() {
    install_r2r_libs();
    log("creating wallet daemon");
    us::wallet::cli::params p;
    p.daemon = true;
    p.homedir = homedir;
    p.channel = 123;
    p.listening_port = p.published_port = wport;
    p.backend_host = "localhost";
    p.backend_port = gport;
    //p.logd = logdir + "/walletd";
    assert(!vardir.empty());
    p.downloads_dir = vardir + "/www/us_html/downloads";
    assert(wallet == nullptr);
    wallet = new walletx_t(p, cout);
}


vector<string> c::r2r_libs(bool filter_not_active) {
    vector<string> r;
    return r;
}


void c::install_r2r_libs() {
    tee("install_r2r_libs");
/*
    vector<string> libs = r2r_libs(false);
    ostringstream so;
    for (auto& l: libs) {
        if (l == "w2w-w") continue;
        ostringstream file;
        file << "../libtrader-" << l << ".so";
        ostringstream dir;
        dir << homedir << "/" << 123 << "/wallet/trader/lib";
        system((string("mkdir -p ") + dir.str()).c_str());
        cout << "installing lib " << file.str() << " at " << dir.str() << '\n';
        ostringstream cmd;
        cmd << "cp " << file.str() << " " << dir.str() << "/";
        system(cmd.str().c_str());
    }
*/
}

void c::gov_cli_start() {
    govx_t::params p;
    p.daemon = false;
    p.port = gov->p.pport;
    p.sysophost = localip;
    p.channel = 123;
    p.homedir = homedir;
    assert(gov_cli == nullptr);
    gov_cli = new govx_t(p, cout);
    #if CFG_LOGS == 1
        gov_cli->logdir = logdir + "/gov_cli";
    #endif
    auto r = gov_cli->start();
    if (is_ko(r)) cout << r << endl;
    assert(r == ok);
    assert(is_ok(gov_cli->rpc_daemon->wait_ready(3)));
}

void c::create_wallet_cli() {
    us::wallet::cli::params p;
    p.daemon = false;
    p.channel = 123;
    p.homedir = homedir;
    p.walletd_host = localip;
    p.walletd_port = wallet->p.published_port;
    p.subhome = wallet_cli_subhome; //empty=NC Wallet; non-empty=custodial wallet (guest)
    assert(wallet_cli == nullptr);
    wallet_cli = new walletx_t(p, cout);
}

void c::start_gov(bool init_chain) {
    cout << "---------------------" << endl;
    cout << "starting node " << id << " gov port " << gport << " wallet port " << wport << endl;
    start_govd(init_chain);
    //this_thread::sleep_for(1s);
    gov_cli_start();
    //this_thread::sleep_for(1s);
}

pair<vector<uint16_t>, vector<uint16_t>> c::doctypes() const {
    pair<vector<uint16_t>, vector<uint16_t>> r;
    return r;
}

void c::start_walletd_cli() {
    assert(wallet == nullptr);
    assert(wallet_cli == nullptr);
    create_walletd();
    create_wallet_cli();
    #if CFG_LOGS == 1
        wallet->logdir = logdir + "/walletd";
        wallet_cli->logdir = logdir + "/wallet_cli";
    #endif

    assert(wallet->start() == ok);
    log("started wallet daemon");
    assert(is_ok(wallet->daemon->wait_ready(3)));
//    this_thread::sleep_for(1s);
    assert(wallet_cli_dis == nullptr);
    wallet_cli_dis = new dispatcher_t(id, *wallet_cli, cout);

    assert(wallet_cli->start(wallet_cli_dis) == ok);
    assert(is_ok(wallet_cli->rpc_daemon->wait_ready(3)));

    log("started wallet hmi");
    //this_thread::sleep_for(1s);

    auto bm = bookmarks();
    assert(!bm.empty());
    wallet_pkh = bm.begin()->second.qr.endpoint.pkh;
    cout << "wallet pkh = " << wallet_pkh << endl;
}

void c::stop() {
    tee("stopping node", id);
    if (gov_cli) {
        tee(" gov_cli", id);
        gov_cli->_assert_on_stop = false;
        gov_cli->stop();
    }
    if (wallet_cli) {
        tee(" wallet_cli", id);
        wallet_cli->_assert_on_stop = false;
        wallet_cli->stop();
    }
    if (wallet) {
        tee(" wallet", id);
        wallet->_assert_on_stop = false;
        wallet->stop();
    }
    if (gov) {
        tee(" gov", id);
        gov->_assert_on_stop = false;
        gov->stop();
    }
}

void c::abort_tests() {
    cerr << "node::abort tests" << endl;
    if (wallet_cli_dis != nullptr) wallet_cli_dis->abort_tests();
    stop();
    lock_guard<mutex> lock(mx_abort);
    abortsignaled = true;
    cv_abort.notify_all();
}

void c::join() {
    if (wallet_cli) {
        tee(" waiting for wallet_cli", id);
        wallet_cli->join();
        delete wallet_cli;
        wallet_cli = nullptr;
        //delete wallet_cli_dis;
        wallet_cli_dis = nullptr;
        tee(" stopped. wallet cli", id);
    }
    if (gov_cli) {
        tee(" waiting for gov_cli", id);
        gov_cli->join();
        delete gov_cli;
        gov_cli = nullptr;
        tee(" stopped. gov cli", id);
    }
    if (wallet) {
        tee(" waiting for wallet", id);
        wallet->join();
        delete wallet;
        wallet = nullptr;
        tee(" stopped. wallet daemon", id);
    }
    if (gov) {
        tee(" waiting for gov", id);
        gov->join();
        delete gov;
        gov = nullptr;
        tee(" stopped. gov daemon", id);
    }
}

void c::test_list_protocols() {
    string s;
    auto r = wallet_cli->rpc_daemon->get_peer().call_list_protocols(s);
    if (is_ko(r)) cout << r << endl;
    assert(r == ok);
    set<string> p;
    istringstream is(s);
    while (is.good()) {
        string line;
        getline(is, line);
        if (line.empty()) continue;
        istringstream is2(line);
        string pr;
        string role;
        is2 >> pr;
        is2 >> role;
        cout << "insert " << pr << "-" << role << endl;
        p.insert(pr + "-" + role);
    }
    cout << "call_list_protocols returned:" << endl;
    for (auto& i: p) {
        cout << "  " << i << endl;
    }
    assert(p.find("w2w-w") != p.end());
    vector<string> libs = r2r_libs(true);
    for (auto& j: libs) {
        cout << j << "..."; cout.flush();
        assert(p.find(j) != p.end());
        cout << "ok\n";
    }
}

void c::wait(uint64_t secs) const {
    unique_lock<mutex> lock(mx_abort);
    if (abortsignaled) {
        return;
    }
    cv_abort.wait_for(lock, chrono::seconds(secs), [&]{ return abortsignaled; });
    abortsignaled = false;
}

void c::sleep_for(uint64_t secs) const {
    //this_thread::sleep_for(chrono::seconds(secs));
    wait(secs);
}

void c::update_plugins() {
    ostringstream os;
    os << homedir << "/123/wallet/trader/lib/";
    us::gov::io::cfg0::ensure_dir(os.str());
    update_plugins(os.str());
}

hash_t c::create_coin(int64_t supply) {
    tee("trace BXmAJ");
    hash_t coin;
    {
        auto ans = wallet_cli->rpc_daemon->get_peer().call_new_address(coin);
        assert(ans == ok);
    }
    cout << "new address for coin: " << coin << endl;
    {
        blob_t blob;
        auto t = wallet_cli->rpc_daemon->get_peer().call_transfer(us::wallet::cli::rpc_peer_t::transfer_in_t(coin, 1000, hash_t(0), 1), blob);
        if (t != ok) cout << t << endl;
        assert(t == ok);
        cout << "transfer 1000" << endl;
        cout << blob.size() << endl;
        {
        string s;
        assert(is_ok(wallet_cli->rpc_daemon->get_peer().call_tx_decode(blob, s)));
        cout << s << endl;
        }
    }
    {
        ostringstream cmd;
        cmd << "data " << coin;
        int n = 0;
        while (true) {
            cout << "exec " << cmd.str() << endl;
            auto r = gov_cli->exec_cmd(cmd.str());
            if (r.first == us::gov::socket::rendezvous_t::KO_20190) {
                cout << r.first << endl;
                cout << "lasterror: " << gov_cli->rpc_daemon->get_peer().lasterror << endl;
                if (gov_cli->rpc_daemon->get_peer().lasterror == "KO 69213 Address not found.") {
                    cout << ++n << " waiting until address settles... " << r.second << endl;
                    //this_thread::sleep_for(5s);
                    sleep_for(5);
                    continue;
                }

            }
            if (is_ko(r.first)) {
                cout << "Error: " << r.first << endl;
                assert(false);
            }
            break;
        }
    }
    while (true) {
        blob_t blob;
        auto t = wallet_cli->rpc_daemon->get_peer().call_set_supply(us::wallet::cli::rpc_peer_t::set_supply_in_t(coin, supply, 1), blob);
        if (is_ko(t)) {
            cout << t << endl;
            cout << "lasterror: " << gov_cli->rpc_daemon->get_peer().lasterror << endl;
            if (string(gov_cli->rpc_daemon->get_peer().lasterror) == "KO 32001 A funded account must exist") {
                cout << "waiting a bit more for tx settlement...\n";
                this_thread::sleep_for(5s);
                continue;
            }
            assert(false);
        }
        cout << "set supply coin " << supply << endl;
        cout << blob.size() << endl;
        {
            string s;
            assert(is_ok(wallet_cli->rpc_daemon->get_peer().call_tx_decode(blob, s)));
            cout << s << endl;
        }
        break;
    }

    {
        ostringstream cmd;
        cmd << "data " << coin;
        while(true) {
            auto r = gov_cli->exec_cmd(cmd.str());
            assert(r.first == ok);
            cout << r.second << endl;
            if (r.second.find("mint. Supply left ") != string::npos) {
                break;
            }
            this_thread::sleep_for(1s);
        }
    }
    return coin;
}

/*
void c::restart_wallet_daemon() {
    tee("restart_wallet_daemon");
    cout << id << "> restarting wallet daemon\n";
    cout << id << "  > stopping walletd" << endl;
    wallet->_assert_on_stop = false;
    wallet_cli->_assert_on_stop = false;
    wallet->stop();
    wallet_cli->stop();
    cout << id << "  > waiting for walletd" << endl;
    wallet->join();
    wallet_cli->join();
//    cout << id << "  > sleep 3s" << endl;
    //this_thread::sleep_for(3s);
    wallet->_assert_on_stop = true;
    assert(wallet->daemon == nullptr);

    cout << id << "  > starting walletd" << endl;
    assert(wallet->start() == ok);
    assert(wallet_cli->start() == ok);//    cout << id << "  > sleep 1s" << endl;
//    this_thread::sleep_for(1s);

    assert(is_ok(wallet->daemon->wait_ready(3)));
    assert(wallet->daemon->isup());

    assert(is_ok(wallet_cli->rpc_daemon->wait_ready(3)));
    assert(wallet_cli->rpc_daemon->isup());

    cout << id << "\n> test_list_protocols\n";
    test_list_protocols();
    wallet->_assert_on_stop = true;
    wallet_cli->_assert_on_stop = true;
    cout << "\ndaemons restarted" << endl;
}
*/
void c::restart_daemons() {
    log("restart_daemons");
    cout << "\nrestart_daemons" << endl;
    tee(id, "=========================stopping wallet-cli");
    auto dis = wallet_cli->rpc_daemon->dispatcher;
    assert(wallet_cli_dis == dis);
    wallet_cli->rpc_daemon->dispatcher = nullptr;

    wallet_cli->_assert_on_stop = false;
    wallet_cli->stop();
    tee(id, "=========================stopping walletd");
    wallet->_assert_on_stop = false;
    wallet->stop();
    tee(id, "=========================waiting for wallet-cli");
//    cout << id << "  > waiting for wallet-cli" << endl;
    wallet_cli->join();
    tee(id, "=========================waiting for walletd");
    wallet->join();

//    wallet_cli_dis = nullptr;
//    cout << id << "  > sleep 3s" << endl;
    //this_thread::sleep_for(3s);
    wallet->_assert_on_stop = true;
    wallet_cli->_assert_on_stop = true;
    assert(wallet->daemon == nullptr);
    assert(wallet_cli->rpc_daemon  == nullptr);

    tee(id, "=========================starting walletd");
//    cout << id << "  > starting walletd" << endl;
    assert(wallet->start() == ok);
    tee(id, "=========================starting wallet-cli");
//    assert(wallet_cli_dis == nullptr);
//    wallet_cli_dis = new dispatcher_t(id, *wallet_cli, cout);
    assert(wallet_cli->start(wallet_cli_dis) == ok);

    assert(is_ok(wallet->daemon->wait_ready(3)));
    assert(is_ok(wallet_cli->rpc_daemon->wait_ready(3)));
    //cout << id << "  > sleep 1s" << endl;
//    this_thread::sleep_for(1s);
    assert(wallet->daemon->isup());
    assert(wallet_cli->rpc_daemon->isup());
    tee(id, "=========================daemons restared");

    cout << id << "\n> test_list_protocols\n";
    test_list_protocols();
    assert(wallet_cli_dis != nullptr);
}

void c::register_wallet() {
    string ans;
    auto r = wallet_cli->rpc_daemon->get_peer().call_register_w(us::gov::socket::client::ip4_encode(localip), ans);
    assert(is_ok(r));
    while(true) {
        gov_cli->exec("cmd app 40 db");
        vector<hash_t> w;
        auto r = wallet_cli->rpc_daemon->get_peer().call_world(w);
        assert(is_ok(r));
        cout << w.size() << endl;
        if (w.size() > 0) break;
        this_thread::sleep_for(1s);
    }
}

void c::banner(ostream& os) const {
    os << "     homedir = " << homedir << '\n';
    os << "     logdir = " << logdir << '\n';
    os << "     vardir = " << vardir << '\n';
}

/*
void test_traders1() {

    traders_t t = new traders_t();
    t->add_trader();
    t->add_trader();
    auto b1 = t->to_blob(b1)

    traders_t t2 = new traders_t();
    auto b2 = taders_t::from_blob(b1)
    assert(b1 == b2);
    assert(*t1 == *t2);

    delete t1;
    delete t2;
}
*/

void c::copy_state(const string& dest) const {
    tee("copy_state", dest);
    ostringstream cmd;
    cmd << "cp " << wallet->daemon->users.root_wallet->traders.sername() << ' ' << dest << "__traders";
    tee(cmd.str());
    assert(system(cmd.str().c_str()) == 0);
    for (auto& i: wallet->daemon->users.root_wallet->traders) {
        if (i.second->id.is_zero()) continue;
        hash_t tid = i.second->id; //traders_t::get_utid_rootwallet();
        ostringstream cmd;
        cmd << "cp " << i.second->sername().first << "/" << i.second->sername().second << ' ' << dest << "__tid_" << tid;
        tee(cmd.str());
        assert(system(cmd.str().c_str()) == 0);
    }
}

void c::diff_state(const string& st0, const string& st1) const {
    ostringstream cmd;
    cmd << "diff " << st0 << "__traders " << st1 << "__traders";
    tee(cmd.str());
    assert(system(cmd.str().c_str()) == 0);
    for (auto& i: wallet->daemon->users.root_wallet->traders) {
        if (i.second->id.is_zero()) continue;
//        size_t utid = traders_t::get_utid_rootwallet(i.second->id);
        hash_t tid = i.second->id;
        ostringstream cmd;
        cmd << "diff " << st0 << "__tid_" << tid << " " << st1 << "__tid_" << tid;
        tee(cmd.str());
        assert(system(cmd.str().c_str()) == 0);
    }
}

void c::traders_serialization_save(const traders_t& o) {
    tee("traders_serialization");
    tee("traders size", o.size());
    tee("saving to", o.sername());
    o.save_state();
    for (auto& i: o) {
        tee("trader serialization:");
        i.second->save_state();
    }
}

void c::traders_serialization_save() {
    traders_serialization_save(wallet->daemon->users.root_wallet->traders);
}

void c::print_pointers(ostream& os) const {
    os << "gov " << gov << '\n';
    os << "gov->daemon " << gov->daemon << '\n';
    os << "gov_cli " << gov_cli << '\n';
    os << "gov_cli->rpc_daemon " << gov_cli->rpc_daemon << '\n';
    os << "wallet " << wallet << '\n';
    os << "wallet->daemon " << wallet->daemon << '\n';
    os << "wallet_cli " << wallet_cli << '\n';
    os << "wallet_cli->rpc_daemon " << wallet_cli->rpc_daemon << '\n';
    os << "wallet_cli_dis " << wallet_cli_dis << '\n';
}


