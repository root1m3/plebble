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
#include "network.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <set>

#include <us/gov/engine/daemon_t.h>
#include <us/gov/engine/db_t.h>
#include <us/gov/engine/track_status_t.h>
#include <us/gov/cli/rpc_peer_t.h>
#include <us/wallet/engine/daemon_t.h>

#include "node_bank.h"
#include "dispatcher_t.h"
#include "main.h"

#define loglevel "us/test"
#define logclass "network"
#include <us/gov/logs.inc>
#include "assert.inc"

using namespace std;
using namespace us;
using us::ko;
using c = us::test::network;
using hash_t = us::gov::crypto::ripemd160::value_type;

string c::OFAddress = "4NwEEwnQbnwB7p8yCBNkx9uj71ru";
string c::local_blob = "blobs/maihealth_alpha-29_9f204a31e38f75cb7e3632aac3542d26fa0d549bafb70be86bb10d7f8962115d.tzst";

namespace {
    set<c*> instances;
    mutex mx;

    void sig_handler(int s) {
        cout << "main: received signal " << s << endl;
        cout << "stopping ...\n";
        {
        lock_guard<mutex> lock(mx);
        for (auto* i: instances) {
            log("stopping instance", i);
            cout << "stopping instance" << i << endl;
            i->stop();
        }
        }
        signal(SIGINT, SIG_DFL);
        signal(SIGTERM, SIG_DFL);
        signal(SIGPIPE, SIG_DFL);
    }

    void setup_signals(c* inst, bool on) {
        log("setup signals", inst, "network #", instances.size());
        cout << "setup signals " << inst << " network #" << instances.size() << '\n';
        lock_guard<mutex> lock(mx);
        if (on) {
            if (instances.empty()) {
                signal(SIGINT, sig_handler);
                signal(SIGTERM, sig_handler);
                signal(SIGPIPE, SIG_IGN);
            }
            instances.emplace(inst);
        }
        else {
            auto i = instances.find(inst);
            assert(i != instances.end());
            instances.erase(i);
            if (instances.empty()) {
                signal(SIGINT, SIG_DFL);
                signal(SIGTERM, SIG_DFL);
                signal(SIGPIPE, SIG_DFL);
            }
        }
    }
}

c::network(const string& homedir, const string& logdir, const string& vardir, const string& stage1dir, ostream& os): homedir(homedir), stage1dir(stage1dir), out(os) {
}

c::~network() {
    join();
    for (auto& i: *this) {
        delete i.second;
    }
}

void c::add_node(const string& name, node* n) {
    if (empty()) {
        genesis_node = n;
    }
    emplace(name, n);
}

void c::start() {
    tee("start");
    if (!us::gov::io::cfg0::dir_exists(stage1dir)) {
        tee("building", stage1dir);
        setup_signals(this, true);
        cout << "start_govs" << endl;
        start_govs(true);
        //this_thread::sleep_for(1s);
        cout << "start_wallets" << endl;
        start_wallets();
        //this_thread::sleep_for(1s);
        stage1_create();
        stop();
        join();
        {
            ostringstream os;
            os << "mkdir " << stage1dir;
            system(os.str().c_str());
        }
        //backup home dir from at point. it will resume from here on next executions.
        {
            ostringstream os;
            os << "mv " << homedir << "/" << "* " << stage1dir << "/"; // -R";
            system(os.str().c_str());
        }
        {
            ostringstream os;
            os << "find " << stage1dir << "/ -name \"*.so\" -delete";
            system(os.str().c_str());
        }

    }
    tee("Resuming state from", stage1dir);
    stage1_ff();
    setup_signals(this, true);
    start_govs(false);
    start_wallets();
    test_list_protocols();
//    this_thread::sleep_for(3s);
}

void c::stop() {
    for (auto& i: *this) {
        //if(i.second->gov != nullptr) i.second->stop();
        i.second->stop();
    }
    setup_signals(this, false);
}

void c::join() {
    for (auto& i: *this) {
        //if(i.second->gov != nullptr) i.second->stop();
        i.second->join();
    }
}

void c::mute_all() {
    for (auto& i: *this) {
        if(i.second->gov != nullptr) i.second->gov->set_mute(true);
        if(i.second->gov_cli != nullptr) i.second->gov_cli->set_mute(true);
        if(i.second->wallet != nullptr) i.second->wallet->set_mute(true);
        if(i.second->wallet_cli != nullptr) i.second->wallet_cli->set_mute(true);
        if(i.second->wallet_cli_dis != nullptr) i.second->wallet_cli_dis->muted = true;
    }
}

ko c::blobfeed_upload() {
//pat.gov_cli.exec("
//        {
//        ostringstream os;
//        os << "store " << OFAddress << " -f " << local_blob << " $blobid;
//        pat.wallet_cli.exec(os.str());
//        }
    return ok;
}

ko c::blobfeed_install() {
    return ok;
}

ko c::android_app_test_automatic_updates() {
    out << "tap settings|version label|check for updates" << endl;
    out << "  Expected: Toast KO 22122 - dir not found" << endl;
    string input;
    out << "Type something to continue" << endl;
    cin >> input;

    auto& pat = *find("bank0")->second;
    string compdir = pat.wallet->p.downloads_dir + "/android";
    out << "Component dir: " << compdir << endl;
    us::gov::io::cfg0::ensure_dir(compdir);


    out << "tap settings|version label|check for updates" << endl;
    out << "  Expected: Toast KO 22123 - No updates found." << endl;
    out << "Type something to continue" << endl;
    cin >> input;

    ostringstream cmd;
    cmd << ". ../../../../core0/us/vcs_git_env; rm -f " << compdir << "/" << "*.apk; cp ../../../../core0/us/android/app/build/outputs/apk/debug/output-debug.apk " << compdir << "/${core0_brand}-wallet_android_${core0_branch}_${core0_hashname}.apk";
    out << cmd.str() << endl;
    system(cmd.str().c_str());
    out << "comp dir:" << endl;
    system((string("ls -la ") + compdir).c_str());

    out << "tap settings|version label|check for updates" << endl;
    out << "  Expected: Toast OK, we run the latest version." << endl;
    out << "Type something to continue" << endl;
    cin >> input;
    return ok;
}

ko c::android_app_test__prepair(string subhome, node& n) {
    cout << "PIN (or auto): "; cout.flush();
    string inputPIN;
    getline(cin, inputPIN);
    us::gov::io::cfg0::trim(inputPIN);
    pin_t pin;
    if (inputPIN == "auto") {
        pin = numeric_limits<pin_t>::max();
    }
    else {
        pin = atoi(inputPIN.c_str());
    }
    if (pin == 0) {
        return "KO 87096";
    }
    cout << "type 'auto' to generate subhomes tied to devices. (Input anything else to use subhome '" << subhome << "'): " ; cout.flush();
    string subhome_auto;
    getline(cin, subhome_auto);
    us::gov::io::cfg0::trim(subhome_auto);
    if (subhome_auto == "auto") {
        subhome = "auto";
    }
    cout << "using subhome " << subhome << endl;
    {
        cout << "pre-authorizing " << pin << " subhome " << subhome << endl;
        us::wallet::cli::rpc_peer_t::prepair_device_out_dst_t ans;
        auto r = n.wallet_cli->rpc_daemon->get_peer().call_prepair_device(us::wallet::cli::rpc_peer_t::prepair_device_in_t(pin, subhome, "Tests"), ans);
        if (r != ok) {
            cout << n.wallet_cli->rpc_daemon->rewrite(r) << endl;
            assert(false);
        }
        cout << "prepair_device response: PIN: " << ans.pin << " subhome: " << ans.subhome << endl;
    }
    return ok;
}

ko c::android_app_test__pair(string subhome, node& n) {
    {
        auto r = n.wallet_cli->exec("list_devices");
        if (r != ok) {
            cout << r << endl;
            return r;
        }
    }
    {
        vector<string> attempts;
        auto r = n.wallet_cli->rpc_daemon->get_peer().call_attempts(attempts);
        if (r != ok) {
            cout << r << endl;
            return r;
        }
        if (attempts.empty()) {
            ko e = "KO 33020 No unauthorized attempts registered";
            cout << e << ", try again" << endl;
            return e;
        }
        cout << "attempts:" << endl;
        for (auto& i: attempts) {
            cout << i << endl;
        }
        us::gov::crypto::ec::keys::pub_t pubk;
        istringstream is(*attempts.begin());
        is >> pubk;
        assert(!is.fail());
        {
            cout << "authorizing " << pubk << " subhome " << subhome << endl;
            string ans;
            auto r = n.wallet_cli->rpc_daemon->get_peer().call_pair_device(us::wallet::cli::rpc_peer_t::pair_device_in_t(pubk, subhome, "Tests"), ans);
            if (r != ok) {
                cout << r << endl;
                assert(false);
            }
            cout << "pair_device response: " << ans << endl;
        }
    }
    return ok;
}

void c::set_consume_pin(node& n) {
    cout << "Consume PIN? (1|0): "; cout.flush();
    string i;
    getline(cin, i);
    us::gov::io::cfg0::trim(i);
    if (i == "0") {
        n.wallet->daemon->devices.set_consume_pin(false);
    }
    else if (i == "1") {
        n.wallet->daemon->devices.set_consume_pin(true);
    }
    else {
        cerr << "Nothing done." << endl;
    }
}

namespace {
    string inputsubhome;
}

ko c::android_app_test() {
    assert(!empty());
    auto& n = *begin()->second;
    cout << "In settings (Android app), connect to wallet at : " << node::localip << ":" << n.wallet->p.listening_port << " channel " << 123 << endl;
//    cout << "First connect without PIN to receive an error not-authorized." << endl;

    while (true) {
        cout << "Node " << n.id << endl;
        cout << "Pairing Menu\n";
        cout << "0- Skip Pairing Menu\n";
        cout << "1- Subhome: " << inputsubhome << "\n";
        cout << "2- list_devices\n";
        cout << "3- pair from unauthorized attempts & continue (Do a connection attempt with the app before choosing 3)\n";
        cout << "4- set_consume_pin [consume_pin " << (n.wallet->daemon->devices.get_consume_pin() ? "1" : "0") << "]\n";
        cout << "5- Prepair & continue\n";
        string input;
        getline(cin, input);
        us::gov::io::cfg0::trim(input);
        if (input == "1") {
            cout << "Enter subhome (e.g. mycusw1) for custodial mode, or empty for non-custodial mode." << endl;
            cout << "Subhome: "; cout.flush();
            getline(cin, inputsubhome);
            us::gov::io::cfg0::trim(inputsubhome);
        }
        else if (input == "2") {
            auto r = n.wallet_cli->exec("list_devices");
            if (r != ok) {
                cout << r << endl;
                return r;
            }
        }
        else if (input == "3") {
            auto r = android_app_test__pair(inputsubhome, n);
            if (is_ko(r)) {
                return r;
            }
            break;
        }
        else if (input == "4") {
            set_consume_pin(n);
        }
        else if (input == "5") {
            auto r = android_app_test__prepair(inputsubhome, n);
            if (is_ko(r)) {
                return r;
            }
            break;
        }
        else if (input == "0") {
            break;
        }
    }

    cout << "Check you have green light on screen." << endl;

    while(true) {
        cout << "Menu - Android" << endl;
        cout << "0 Back to main menu" << endl;
        cout << "1 Automatic updates" << endl;
        cout << "blob-feed - A new blob is produced by cbs." << endl;
        cout << "  blobs/maihealth_alpha-29_9f204a31e38f75cb7e3632aac3542d26fa0d549bafb70be86bb10d7f8962115d.tzst  " << endl;
        cout << "  2.1 upload to blockchain.   " << endl;
        cout << "  2.2 install on node " << n.id << "." << endl;
        string input;
        getline(cin, input);
        us::gov::io::cfg0::trim(input);

        if (input == "0") {
            break;
        }
        else if (input == "1") {
            assert(android_app_test_automatic_updates() == ok);
        }
        else if (input == "2.1") {
            assert(blobfeed_upload() == ok);
        }
        else if (input == "2.2") {
            assert(blobfeed_install() == ok);
        }
    }
    return ok;
}

void c::consoles_shell() {
    mute_all();
    while(true) {
        cout << "console for node? :";
        for (auto& i: *this) {
            cout << i.first << ' ';
        }
        cout << "\nselect node> "; cout .flush();
        string cmd;
        cin >> cmd;
        auto u = find(cmd);
        if (u == end()) {
            break;
        }
        cout << cmd << ". console type? : g w";
        cout << "\nselect console> "; cout .flush();
        string type;
        cin >> type;
        if (type == "g") {
            u->second->gov_cli->interactive_shell();
        }
        else if (type == "w") {
            u->second->wallet_cli->interactive_shell();
        }
        else {
            break;
        }
    }
}

void c::start_govs(bool make_stage) {
    cout << "starting govs" << endl;
    genesis_node->start_gov(make_stage);
    ostringstream os;
    os << "cmd a " << node::localip << ":" << genesis_node->gport;
    for (auto& i: *this) {
        if (i.second == genesis_node) continue;
        i.second->start_gov(false);
        cout << i.first << " <-- " << os.str() << endl;
        if (make_stage) {
            i.second->gov_cli->exec(os.str());
        }
    }

    int n = 0;
    const int to = 240;
    while(!check()) {
        genesis_node->gov_cli->exec("sysop gw y");
        ++n;
        cout << "Waiting for all nodes registration in the blockchain. In " << (to - n) << " secs I'd enter an interactive shell." << endl;
        if (n > to) {
            n = 0;
            setup_signals(this, false);
            consoles_shell();
            setup_signals(this, true);
            cout << "resuming test" << endl;
        }
        this_thread::sleep_for(1s);
    }
}

void c::start_wallets() {
    for (auto& i: *this) {
        i.second->start_walletd_cli();
    }
}

void c::stage1_create() {
    stage1_configure();
    for (auto& i: *this) {
        i.second->register_wallet();
    }
    while (true) {
        vector<hash_t> w;
        auto r = genesis_node->wallet_cli->rpc_daemon->get_peer().call_world(w);
        assert(is_ok(r));
        assert(w.size() > 0);
        cout << "wait reg all wallets. " << w.size() << "/" << size() << endl;
        if (w.size() >= size()) break;
        this_thread::sleep_for(1s);
    }
}

void c::stage1_ff() {
    cout << "Fast forwarding stage1. homedir=" << homedir << endl;
    {
        ostringstream os;
        os << "mkdir -p " << homedir;
        system(os.str().c_str());
    }
    {
        ostringstream os;
        os << "cp " << stage1dir << "/* " << homedir << "/ -R";
        cout << "xec: " << os.str() << endl;
        system(os.str().c_str());
    }
    stage1_ff_configure();
}

c::bookmarks_t c::filtered_bookmarks(const string& nodename, const string& exclude_label) {
    node* n = find(nodename)->second;
    bookmarks_t filtered;
    for (auto& i: n->bookmarks()) {
        if (i.second.label != exclude_label) {
            assert(filtered.add(i.first, i.second) == ok);
        }
    }
    return filtered;
}

void c::test_list_protocols() {
    for (auto& i: *this) {
        cout << "----------------------------------------------" << i.first << endl;
        i.second->test_list_protocols();
    }
}

void c::watch() {
    cout << "========================= watch " << frame << " ====================" << endl;
    for (auto& i: *this) {
        cout << "----------------------------------------------" << i.first << endl;
        i.second->gov_cli->exec("app 20 db");
        i.second->gov_cli->exec("gw");
        i.second->gov_cli->exec("watch");
    }
    ++frame;
}

size_t c::nh(const us::gov::engine::auth::db_t& o) const {
    size_t x = 0;
    {
        lock_guard<mutex> lock(o.mx_hall);
        x += o.hall.size();
    }
    return x;
}

size_t c::nn(const us::gov::engine::auth::db_t& o) const {
    size_t x = 0;
    {
        lock_guard<mutex> lock(o.mx_nodes);
        x += o.nodes.size();
    }
    return x;
}

bool c::check_nh() { //hall+nodes must not contain more entries than the network size
    cout << "========================= nodes-hall " << frame << " ====================" << endl;
    vector<pair<string, pair<size_t, size_t>>> v;
    for (auto& i: *this) {
        cout << "----------------------------------------------" << i.first << endl;
        assert(i.second->gov != nullptr);
        assert(i.second->gov->daemon != nullptr);
        assert(i.second->gov->daemon->db->auth_app != nullptr);
        i.second->gov->daemon->db->auth_app->db.dump("", cout);
        size_t numh = nh(i.second->gov->daemon->db->auth_app->db);
        size_t numn = nn(i.second->gov->daemon->db->auth_app->db);
        v.push_back(make_pair(i.first, make_pair(numh, numn)));
    }

    for (auto& i: v) {
        cout << "nodes " << i.first << ' ' << i.second.first << '+' << i.second.second << '/' << size() << endl;
    }
    int x = size();
    for (auto& i: v) {
        assert((i.second.first + i.second.second) <= size());
        if (i.second.first == 0 && i.second.second == size()) --x;
    }
    ++frame;
    return x == 0;
}

bool c::check_num_addresses() {
    cout << "========================= addresses " << endl;
    vector<pair<string, size_t>> v;
    for (auto& i: *this) {
        cout << "----------------------------------------------" << i.first << endl;
        assert(i.second->gov != nullptr);
        assert(i.second->gov->daemon != nullptr);
        assert(i.second->gov->daemon->db->cash_app != nullptr);
        assert(i.second->gov->daemon->db->cash_app->db.accounts != nullptr);
        i.second->gov->daemon->db->cash_app->db.accounts->dump("", 1, cout);
        size_t numa = i.second->gov->daemon->db->cash_app->db.accounts->size();
        v.push_back(make_pair(i.first, numa));
    }
    for (auto& i: v) {
        cout << "addr " << i.first << ' ' << i.second << "/min " << size() << endl;
    }
    int x = size();
    for (auto& i: v) {
        if (i.second >= size()) --x;
    }
    return x == 0;
}

void c::check_sync() { //once in sync never lose sync
    int n = 0;
    for (auto& i: *this) {
        if(i.second->gov->daemon->syncd.in_sync()) ++n;
    }
    cout << "num_in_sync prev=" << num_in_sync << " cur=" << n << '\n';
    assert(n >= num_in_sync);
    num_in_sync = n;
}

bool c::check() {
    bool b = check_nh();
    bool b2 = check_num_addresses();
    check_sync();
    cout << "check_nh: " << b << " check_num_addresses: " << b2 << " check_sync: " << num_in_sync << '/' << size() << endl;
    return b && b2 && num_in_sync == size();
}

void c::wait_settlement(govx_t& gov_cli, uint64_t ts) {
    using track_status_t = us::gov::engine::track_status_t;
    us::gov::engine::evt_status_t st = us::gov::engine::evt_unknown;
    auto t0 = chrono::system_clock::now();

    cout << "waiting for settlement tx.ts=" << ts << endl;
    while (true) {
        cout << "."; cout.flush();
        track_status_t track_status;
        auto r = gov_cli.rpc_daemon->get_peer().call_track(ts, track_status);
        assert(r == ok);
        if (track_status.st != st) {
            st = track_status.st;
            cout << "\ntx status changed to: " << us::gov::engine::evt_status_str[st] << '\n';
            if (st == us::gov::engine::evt_settled) break;
        }
        this_thread::sleep_for(1s);
    }
    auto t1 = chrono::system_clock::now();
    cout << "tx settled in " << chrono::duration_cast<chrono::seconds>(t1 - t0).count() << " secs\n";
}

pair<blob_t, hash_t> c::transfer(walletx_t& from, walletx_t& to, const hash_t& coin, const cash_t& amount) {
    pair<blob_t, hash_t> ret;
    auto r = to.rpc_daemon->get_peer().call_recv_address(ret.second);
    assert(r == ok);
    cout << "Transfer: " << endl;
    cout << "  recv addr: " << ret.second << endl;
    cout << "  amount: " << amount << endl;
    cout << "  coin: " << coin << endl;


    auto t = from.rpc_daemon->get_peer().call_transfer(us::wallet::cli::rpc_peer_t::transfer_in_t(ret.second, amount, coin, 1), ret.first);
    if (t != ok) cout << t << endl;
    assert(t == ok);
    return move(ret);
}

void c::transfer_wait(node& from, node& to, const hash_t& coin, const cash_t& amount) {
    log("trace B756ghh2");
    {
        string bal;
        cout << "source balance account" << endl;
        auto t = to.wallet_cli->rpc_daemon->get_peer().call_balance(2, bal);
        if (is_ko(t)) {
            cout << t << endl;
        }
        assert(t == ok);
        cout << bal << endl;
    }

    auto tx = transfer(*from.wallet_cli, *to.wallet_cli, coin, amount);
    {
        auto ev = gov::engine::evidence::from_blob(tx.first);
        assert(ev.first == ok);
        uint64_t ts = ev.second->ts;
        cout << "exec ts: " << ts << endl;
        wait_settlement(*to.gov_cli, ts);
        delete ev.second;
    }
    {
        cout << "balance account " << tx.second << ":" << endl;
        string bal;
        auto t = to.wallet_cli->rpc_daemon->get_peer().call_balance(2, bal);
        if (t != ok) {
            cout << t << endl;
            assert(false);
        }
        else {
            cout << bal << endl;
            ostringstream os;
            os << coin << ' ' << amount;
            cout << "check: " << os.str() << endl;
            assert(bal.find(os.str()) != string::npos);
        }
    }
}

void c::static_tests::test_arch() {
        cout << "char " << sizeof(char) << endl;
        cout << "short " << sizeof(short) << endl;
        cout << "bool " << sizeof(bool) << endl;
        cout << "int " << sizeof(int) << endl;
        cout << "long " << sizeof(long) << endl;
        cout << "long long " << sizeof(long long) << endl;
        cout << "float " << sizeof(float) << endl;
        cout << "double " << sizeof(double) << endl;
        cout << "size_t " << sizeof(size_t) << endl;
    /*
         armv7l X86_64
    char      1 1
    short     2 2
    bool      1 1
    int       4 4
    long      4 8  !
    long long 8 8
    float     4 4
    double    8 8
    size_t    4 8  !
    */
}

#include <us/wallet/trader/endpoint_t.h>

void c::static_tests::test_endpoint(const string& s, const string& exp) {
    using endpoint_t = us::wallet::trader::endpoint_t;
    cout << '\n';
    endpoint_t ep(s);
    ostringstream os;
    ep.to_streamX(os);
    string x = os.str();
    endpoint_t ep2(x);
    ostringstream os2;
    ep2.to_streamX(os2);
    string x2 = os2.str();
    cout << "input    '" << s << "'\n";
    cout << "output   '" << x << "'\n";
    cout << "output2 '" << x2 << "'\n";
    cout << "expected '" << exp << "'\n";
    assert(x == exp);
    assert(x == x2);
}

void c::static_tests::test_endpoint() {
    test_endpoint("", "11111111111111111111 ");
    test_endpoint("192.168.0.1:1234", "111111111111112oapb1fT6 ");
    test_endpoint("123 192.168.0.1:1234", "123 111111111111112oapb1fT6 ");
    test_endpoint("192.168.0.1:1234.wloc", "111111111111112oapb1fT6.wloc ");
    test_endpoint("192.168.0.1.drmay", "11111111111111115vd41e.drmay ");
    test_endpoint("2PF9JdAPmqKhnw51fvw1zTbiWsuM", "2PF9JdAPmqKhnw51fvw1zTbiWsuM ");
    test_endpoint("2PF9JdAPmqKhnw51fvw1zTbiWsuM.bar", "2PF9JdAPmqKhnw51fvw1zTbiWsuM.bar ");
    test_endpoint("16", "16 11111111111111111111 ");
    test_endpoint("55 2PF9JdAPmqKhnw51fvw1zTbiWsuM", "55 2PF9JdAPmqKhnw51fvw1zTbiWsuM ");
    test_endpoint("55 2PF9JdAPmqKhnw51fvw1zTbiWsuM.boo", "55 2PF9JdAPmqKhnw51fvw1zTbiWsuM.boo ");
}

void c::static_tests::test() {
    test_arch();
    test_endpoint();
}

void c::test() {
    static_tests o;
    o.test();
}

void c::menu() {
    setup_signals(this, false);
    while(true) {
        out << "RPC Devices Menu" << endl;
        out << "====" << endl;
        out << "0 Exit" << endl;
        out << "1 gov/wallet terminal. (doc/sequences~seq.id.2)" << endl;
        out << "2 android app. (doc/sequences~seq.id.1)" << endl;
        out << "  " << endl;
        string input;
        getline(cin, input);
        if (cin.fail()) {
            cerr << "cin failed" << endl;
            break;
        }
        us::gov::io::cfg0::trim(input);
        if (input == "1") {
            consoles_shell();
        }
        else if (input == "2") {
            if (is_ko(android_app_test())) {
                cout << "Start the app first, so I can get its public key." << endl;
                continue;
            }
        }
        else if (input == "0") {
            break;
        }
    }
    setup_signals(this, true);
}

c::bookmarks_t c::bookmarks() const {
    bookmarks_t bm;
    for (auto& i: *this) {
        auto nbm = i.second->bookmarks();
        cout << i.first << ":" << nbm.size() << " bookmarks" << endl;
        bm += nbm;
    }
    return bm;
}

