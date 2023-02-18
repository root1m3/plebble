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
#include "networking.h"

#include <us/gov/config.h>
#include <us/gov/crypto/ec.h>
#include <us/gov/crypto/types.h>
#include <us/gov/engine/evidence.h>

#include "okev.h"
#include "evc_t.h"

#define loglevel "test/net"
#define logclass "main"
#include <us/gov/logs.inc>
#include <us/gov/socket/dto.inc>

#include "../assert.inc"

namespace us::test {

using namespace us;
using namespace chrono_literals;
using namespace std::chrono;
using evidence = us::gov::engine::evidence;
using c = us::test::networking;

namespace {
    string logdir0;
    string homedir;
}

void test_fuzzy() {
    tee("test::daemon_t::test_fuzzy");
    auto k = us::gov::crypto::ec::keys::generate();
    networking peerd(k, us::test::networking::govport, cout);
    #if CFG_LOGS == 1
    peerd.logdir = logdir0 + "/test_fuzzy";
    #endif
    assert(peerd.start() == ok);
    tee("starting peerd");
    this_thread::sleep_for(1ms);
    tee("self test");
    peerd.self_test();
    tee("stopping peerd...");
    peerd.stop();
    peerd.join();
    tee("stopped peerd");
}

struct n {  //a node

    n() : keys(us::gov::crypto::ec::keys::generate()) {
    }

    ~n() {
        delete o;

    }

    networking* o{nullptr};
    us::gov::crypto::ec::keys keys;
};


void watch(int frame, const vector<n>& svrs) {
    cout << "\n\n\n";
    cout << "--Network watch.--Frame " << frame << "---------------------------" << endl;
    for (int i = 0; i < svrs.size(); ++i) {
        cout << "####### node#" << i << " listening port " << svrs[i].o->myport << " id " << svrs[i].o->id.pub.hash()  << /*"recv_evs " <<  svrs[i].o->recv_evs <<*/ " uniqevs " << svrs[i].o->uniqevs << "; ";
        svrs[i].o->pool.watch(cout);
        cout << '\n';
        svrs[i].o->watch(cout);
        cout << "node peerd sendq:\n";
        #if CFG_COUNTERS == 1
            cout << "recv: bytes_received " << us::gov::socket::datagram::counters.bytes_received << '\n';
        #endif
        cout << "daemon::local_api endpoint sendq:\n";
        assert(svrs[i].o != nullptr);
    }
    cout << "evidences sent fail ";
    for (int i = 0; i < svrs.size(); ++i) {
        cout << svrs[i].o->evsent_fail << ' ';
    }
    cout << endl;
    cout << "evidences sent succ ";
    for (int i = 0; i < svrs.size(); ++i) {
        cout << svrs[i].o->evsent_succ << ' ';
    }
    cout << endl;
    cout << "evidences recv succ ";
    for (int i = 0; i < svrs.size(); ++i) {
        cout << svrs[i].o->uniqevs << ' ';
    }
    cout << endl;
    cout << "evidences recv dupl ";
    for (int i = 0; i < svrs.size(); ++i) {
        cout << svrs[i].o->dupevs << ' ';
    }
    cout << '\n';
    log("same reporting point");
    for (int i = 1; i < svrs.size(); ++i) {
        assert(svrs[i].o->evc == svrs[0].o->evc);
    }
    svrs[0].o->evc->dump(cout);
}

void load_test(const string& logdir, vector<n>& svrs, int evidences, int separation_ms, int nl) {
    //assert(nl==1);
    for (int i = 0; i < svrs.size(); ++i) {
        assert(svrs[i].o->isup());
        svrs[i].o->evsent_succ = 0;
        svrs[i].o->evsent_fail = 0;
        svrs[i].o->uniqevs = 0;
        if (i < nl) {
            ostringstream l;
            l << logdir << "/node-" << i;
            string ld = l.str();
            svrs[i].o->set_load_function(bind(&us::test::networking::test_load, svrs[i].o, evidences, milliseconds(separation_ms)));
            svrs[i].o->start_load(ld);
        }
    }
    for (int i = 0; i < svrs.size(); ++i) {
        if (i < nl) {
            tee("waiting for readyness", i);
            svrs[i].o->wait_load_ready();
        }
    }
    cout << "Display" << endl;
    us::test::test_platform t(cout);
    int counter = 0;
    if (nl > 0) {
	
        while(true) {
            bool isl = false;
            for (int i = 0; i < nl; ++i) {
                if (svrs[i].o->is_loading()) {
                    isl = true;
                    break;
                }
            }
            if (!isl) break;
            cout << "\033[2J"; //clear screen
            watch(++counter, svrs);
            this_thread::sleep_for(1s);
        }
        cout << "waiting for load threads" << endl;
        for (int i = 0; i < nl; ++i) {
            svrs[i].o->join_load();
        }
    }

    //summary
    cout << "summary:" << endl;
    cout << "input: " << evidences << " evidences separated by " << separation_ms << " ms." << endl;
    for (int i=0; i<svrs.size(); ++i) {
        cout << "  node " << svrs[i].o->nodendx << ":" << endl;
        cout << "      load: evsent_succ " << svrs[i].o->evsent_succ << endl;
        cout << "      load: evsent_fail " << svrs[i].o->evsent_fail << endl;
        cout << "      uniqevs recv: " << svrs[i].o->uniqevs << endl;
    }

    //checks here
    cout << "evidences sent fail? " << endl;
    for (int i = 0; i < svrs.size(); ++i) {
        assert(svrs[i].o->evsent_fail == 0);
    }
    for(int i = 0; i < nl; ++i) {
        cout << "node " << i << " evidences sent succ " << svrs[i].o->evsent_succ << '/' << evidences << endl;
        assert(svrs[i].o->evsent_succ == evidences);
    }
    for (int i = nl; i < svrs.size(); ++i) { //noone else sent any evidence
        assert(svrs[i].o->evsent_succ == 0);
    }
    for (int i = 0; i < svrs.size(); ++i) {
        cout << "node " << i << " evidences recv succ " << svrs[i].o->uniqevs << '/' << (nl * evidences) << endl;
        assert(svrs[i].o->uniqevs == nl * evidences);
    }

    {
        int totevssent = 0;
        for (int i = 0; i < svrs.size(); ++i) {
            totevssent += svrs[i].o->evsent_succ;
        }
        cout << "Total evidences sent by all nodes: " << totevssent << endl;
        for (int i = 0; i < svrs.size(); ++i) {
            cout << "Total evidences recv by node " << i << ":" << svrs[i].o->uniqevs<< endl;
        }
        for (int i = 0; i < svrs.size(); ++i) {
            assert(svrs[i].o->uniqevs == totevssent);
        }
    }
    cout << "checks OK " << endl;
}

void test_network_load(const string& logdir, vector<n>& svrs, int num_loaders) {
    cout << "Testing " << num_loaders << " simultaneous loading nodes" << endl;
    int num_datagrams;
    int separation_ms;

    num_datagrams = 10;
    separation_ms = 1000;
    cout << "num_loaders " << num_loaders << " num_datagrams " << num_datagrams << " separation_ms " << separation_ms << endl;
    load_test(logdir + "/10_dgram-1000_ms", svrs, num_datagrams, separation_ms, num_loaders);

    num_datagrams = 100;
    separation_ms = 100;
    cout << "num_loaders " << num_loaders << " num_datagrams " << num_datagrams << " separation_ms " << separation_ms << endl;
    load_test(logdir + "/100_dgram-100_ms", svrs, num_datagrams, separation_ms, num_loaders);

//    num_datagrams = 1000;
//    separation_ms = 10;
//    cout << "num_loaders " << num_loaders << " num_datagrams " << num_datagrams << " separation_ms " << separation_ms << endl;
//    load_test(logdir + "/1000_dgram-10_ms", svrs, num_datagrams, separation_ms, num_loaders);

 //   num_datagrams=10000;
 //   separation_ms=1;
 //   cout << "num_loaders " << num_loaders << " num_datagrams " << num_datagrams << " separation_ms " << separation_ms << endl;
 //   load_test(logdir+"/10000_dgram-1_ms",svrs,num_datagrams,separation_ms,num_loaders);

 //   num_datagrams=10000;
 //   separation_ms=0;
 //   cout << "num_loaders " << num_loaders << " num_datagrams " << num_datagrams << " separation_ms " << separation_ms << endl;
 //   load_test(logdir+"/10000_dgram-0_ms",svrs,num_datagrams,separation_ms,num_loaders);

    cout << "End testing " << num_loaders << " simultaneous loading nodes" << endl;
}

void dump(const vector<hostport_t>& seeds, ostream& os) {
    os << seeds.size() << " seeds:" << endl;
    for (auto& i: seeds) {
        os << "    " << i.first << " " << us::gov::socket::client::ip4_decode(i.first) << ":" << i.second << endl;
    }
}

void test_network(const string& logdir) {
    int numnodes = 4;
    log("test mesh network", "num nodes", numnodes);
    vector<n> svrs;
    svrs.resize(numnodes);
    vector<hostport_t> seeds;
    for (int i = 0; i < svrs.size(); ++i) {
        seeds.push_back(make_pair(us::gov::socket::client::ip4_encode(networking::addr_from_ndx(i)), networking::govport + i));
    }
    dump(seeds, cout);
    int workers = 2;
    evc_t evc(svrs.size());
    for (int i = 0; i < svrs.size(); ++i) {
        svrs[i].o = new networking(svrs[i].keys, networking::govport + i, seeds, workers, cout);
        svrs[i].o->nodendx = i;
        svrs[i].o->evc = &evc;
        {
            ostringstream os;
            os << logdir << "/node-" << i;
            #if CFG_LOGS == 1
                svrs[i].o->logdir = os.str();
            #endif
        }
        assert(svrs[i].o->start() == ok);
    }
    cout << "bringing up threads" << endl;
    this_thread::sleep_for(1s);
    log("wait until all nodes are connected");
    while(true) {
        bool ok = true;
        watch(0, svrs);
        for (int i = 0; i < svrs.size(); ++i) {
            int n = svrs[i].o->clique[0]->num_edges_minage(10);
            cout << "  node " << i << " has " << n << " stable edges" << endl;
            if (n == 0) {
                ok = false;
            }
        }
        if (ok) break;
        cout << "wait until all nodes are interconnected" << endl;
        this_thread::sleep_for(10s);
    }
    cout << "all nodes are interconnected, starting load..." << endl;

    if (numnodes >= 4) {
        test_network_load(logdir + "/load1", svrs, 1);
        test_network_load(logdir + "/load2", svrs, 2);
        test_network_load(logdir + "/load3", svrs, 3);
        test_network_load(logdir + "/load4", svrs, numnodes);
    }
    else {
        test_network_load(logdir + "/load", svrs, numnodes);
    }
    cout << "shutting down threads" << endl;
    for (int i = 0; i < svrs.size(); ++i) {
        cout << "stopping node " << i << endl;
        svrs[i].o->stop();
    }
    for (int i = 0; i < svrs.size(); ++i) {
        cout << "waiting for node " << i << endl;
        svrs[i].o->join();
    }
}

struct tqsend: us::test::test_platform {
    typedef us::test::test_platform b;
    typedef us::gov::socket::send_queue_t queue_t;
    typedef us::gov::socket::client client;
    typedef us::gov::socket::datagram dgram;

    tqsend(): b(cout) {
    }

    struct qt: queue_t {
        qt() {}
        using queue_t::send;
    };

    struct cli: client {
        using client::client;
        svc_t translate_svc(svc_t svc, bool inbound) const override { return svc; }

    };

    struct socket_daemon_t: us::gov::socket::daemon_t {
        using b = us::gov::socket::daemon_t;
        socket_daemon_t(): b(0, 0, 0) {
        }

        client* create_client(sock_t sock) override {
            return new cli(*this, sock); // Receiver
        }


    };

    void test2() {
        using namespace us::gov::io;
        cout << "============================test2" << endl;
        qt qsend;
        #if CFG_LOGS == 1
            qsend.logdir = logdir + "/test2";
        #endif
        assert(qsend.start() == ok);
        socket_daemon_t socket_daemon;
        cli cl(socket_daemon); // Sender
        cl.sock = 123;
        check(cl.sendref.load(),0);
        uint16_t pri = 0;
        for (int i = 0; i < 50; ++i) {
            if (i > 20) check(cl.sendref.load() > 0, true);
            cout << "cl.sendref.load()=" << cl.sendref.load() << endl;
            qsend.send(write_datagram(0, 1, 0, string("0123456789 123456789 123456789")), &cl, pri);
        }
        this_thread::sleep_for(1ms);
        for (int i = 50; i < 100; ++i) {
            if (i > 70) check(cl.sendref.load() > 0, true);
            cout << "cl.sendref.load()=" << cl.sendref.load() << endl;
            qsend.send(write_datagram(0, 1, 0, string("0123456789 123456789 123456789")), &cl, pri);
        }
        check(cl.sendref.load() > 0, true);
        this_thread::sleep_for(1s);
        check(cl.sendref.load(), 0);

        qsend.stop();
        qsend.join();
        cout << "test2 finished" << endl;
    }

    void test3() {
        using namespace us::gov::io;
        socket_daemon_t socket_daemon;
        cli cl(socket_daemon); // Sender
        check(cl.sendref.load(),0);
        {
            queue_t qsend;
            for (int i = 0; i < 10; ++i) {
                qsend.send(write_datagram(0, 1, 0, string("")), &cl, 0);
            }
            check(cl.sendref.load(), 10);
        }
        check(cl.sendref.load(), 0);
        cout << "test3 finished" << endl;
    }

    void test1() {
        using namespace us::gov::io;
        queue_t qsend;
        socket_daemon_t socket_daemon;
        cli cl(socket_daemon); // Sender
        qsend.send(write_datagram(0, 1, 0, string("")), &cl, 0);
        check(qsend.size() == 1, true);
        auto i = qsend.top();
        assert (i != 0);
        assert(i->first != 0);
        assert(i->second == &cl);
        check(i->first->service == 0,true); //sendq overwrittes dgram::service with the priority, as the field is no longer used for any other purpose.
        check(cl.sendref.load(), 1);
        qsend.pop();
        check(qsend.size() == 0, true);
        check(cl.sendref.load() == 1, true);
        delete i;
        check(cl.sendref.load() == 0, true);
        cout << "send 100" << endl;
        for (int i = 0; i < 100; ++i) {
            qsend.send(write_datagram(0, i, 0, string("")), &cl, 0);
        }
        check(qsend.size() == 100, true);
        check(cl.sendref.load() == 100, true);
        for (int i = 0; i < 100; ++i) {
            auto z = qsend.top();
            check(z->first->service == 0,true);
            qsend.pop();
            delete z;
        }
        check(qsend.size() == 0, true);
        check(cl.sendref.load() == 0, true);
        cout << "send 100 inverted" << endl;
        for (int i = 99; i >= 0; --i) {
            qsend.send(write_datagram(0, i, 0, string("")), &cl, 6);
        }
        check(qsend.size() == 100,true);
        check(cl.sendref.load() == 100,true);
        for (int i = 0; i < 100; ++i) {
            auto z = qsend.top();
            check(z->first->service == 6, true);
            qsend.pop();
            delete z;
        }
        check(qsend.size() == 0, true);
        check(cl.sendref.load() == 0, true);
        cout << "test1 finished" << endl;
    }

    void test() {
        cout << "========sendq tests============" << endl;
        test1();
        test3();
        test2();
        cout << "======/=sendq tests============" << endl;
    }

    #if CFG_LOGS == 1
    string logdir;
    #endif
};

void test_qsend() {
    tqsend o;
    #if CFG_LOGS == 1
    o.logdir = logdir0 + "/test_qsend";
    #endif
    o.test();
}

#if CFG_LOGS == 0
    #undef log_dir
    namespace {
        string log_dir() {
            ostringstream os;
            os << "logs/us-trader_" << getpid() << '/' << "net";
            return os.str();
        }
    }
#endif

void test_peerd_main() {
    log_start(logdir0, "main");
    tee("=================test_peerd========================");
    homedir = log_dir() + "/home";
    log("fuzzy traffic");
    //test_fuzzy();
    log("send queue");
    test_qsend();
    log("mesh network traffic");
    test_network(logdir0 + "/test_network");
    tee("=========== end testing peerd ==============");
}

void test_peerd() {
    tee("=================test_peerd========================");
    logdir0 = "networking.cpp__test_peerd";
    tee("starting thread", "logdir", logdir0);
    thread th(&test_peerd_main);
    th.join();
    tee("=========== end testing peerd ==============");
}

}

