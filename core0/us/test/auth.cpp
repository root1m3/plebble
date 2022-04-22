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
#include <iostream>
#include <fstream>
#include <random>

#include <us/gov/config.h>
#include <us/gov/engine/auth/app.h>
#include <us/gov/engine/auth/local_delta.h>
#include <us/gov/engine/auth/delta.h>
#include <us/gov/engine/auth/collusion_control_t.h>
#include <us/gov/engine/peer_t.h>
#include <us/gov/engine/daemon_t.h>
#include "test_platform.h"

#define loglevel "test"
#define logclass "auth"
#include "logs.inc"

#include "assert.inc"

namespace us { namespace test {

using namespace std;
using namespace chrono;
using namespace chrono_literals;

struct auth_app: us::gov::engine::auth::app, us::test::test_platform {
    using b = us::gov::engine::auth::app;
    using t = test_platform;
    using keys = us::gov::crypto::ec::keys;
    using pubkey_t = keys::pub_t;
    using peer_t = us::gov::engine::peer_t;

    mt19937_64 rng;

    auth_app(ostream& os, us::gov::engine::daemon_t& e): t(os), k(us::gov::crypto::ec::keys::generate()), b(e) {
        connections.push_back(create_connection("8.8.8.8", 16672));
        connections.push_back(create_connection("8.8.8.9", 16672));
        connections.push_back(create_connection("8.8.8.10", 16672));
    }
    ~auth_app() {
        for (auto& i: connections) delete i;
    }

    us::gov::engine::peer_t* create_connection(const string& ip, uint16_t port) {
        keys peerk(us::gov::crypto::ec::keys::generate());
        us::gov::engine::peer_t* p = new us::gov::engine::peer_t(demon.peerd, -1);
        static_cast<us::gov::auth::peer_t*>(p)->stage = us::gov::auth::peer_t::authorized;
        p->hostport = make_pair(us::gov::socket::client::ip4_encode(ip), port);
        p->pubkey = peerk.pub;
        return p;
    }

    void check_hall_to_node(int n) {

    }

    void check_out_to_hall(int n) {

        auto prevpol_ng = growth;
        auto prevpol_mg = min_growth;
        growth = 0;
        min_growth = 1.0;

        os << "db empty" << endl;
        check(db.nodes.size(), (size_t)0);
        check(db.hall.size(), (size_t)0);

        auto p = connections[n];
        auto orig_ip = p->hostport.first;
        auto orig_port = p->hostport.second;
        auto orig_pool_sz = pool->online.size();
        auto orig_db_n_sz = db.nodes.size();
        auto orig_db_h_sz = db.hall.size();

        os << "a" << endl;
        check(my_stage(), peer_t::out);

        os << "girl connects 1st time, ip=" << p->hostport.first << endl;
        basic_auth_completed(p, 3382);
        os << "1";
        check(p->stage, peer_t::out);
        os << "3";
        check(pool->online.size(), orig_pool_sz+1);
        os << "4";
        check(pool->online.begin()->second.net_address, orig_ip);
        check(pool->online.begin()->second.port, (uint16_t)3382);
        os << endl;

        os << "connects again 1" << endl;
        basic_auth_completed(p, orig_port);
        os << "1";
        check(p->stage,peer_t::out);
        os << "3";
        check(pool->online.size(),orig_pool_sz+1);
        os << "4";
        check(pool->online.begin()->second.net_address, orig_ip);
        check(pool->online.begin()->second.port, orig_port);
        os << endl;

        os << "local delta" << endl;
        us::gov::engine::app::local_delta* ld = create_local_delta();
        os << "2";
        check(pool->online.size(), orig_pool_sz);
        os << endl;

        os << "connects again 2" << endl;
        basic_auth_completed(p, 2010);
        os << "2";
        check(pool->online.size(), orig_pool_sz+1);
        os << endl;

        os << "connects again 3, with different ip" << endl;
        auto newip = orig_ip - 1;
        auto newport = orig_port - 1;
        p->hostport.first = newip;
        p->hostport.second = newport;
        basic_auth_completed(p, 16672);
        os << "2";
        check(pool->online.size(), orig_pool_sz+1);
        check(pool->online.begin()->second.net_address, newip);
        check(pool->online.begin()->second.port, (uint16_t)16672); //--------------------
        os << endl;

        os << "db import poolic" << endl;
        us::gov::engine::auth::app::delta* d = new us::gov::engine::auth::delta();
        d->merge(ld);
        d->end_merge();
        us::gov::engine::pow_t pow;
        import(*d, pow);
        delete d;

        os << "2";
        check(pool->online.size(), orig_pool_sz + 1);
        os << "3";
        check(pool->online.begin()->second.net_address, newip);
        check(pool->online.begin()->second.port, (uint16_t)16672);
        os << "6";
        check(db.hall.size(), orig_db_h_sz + 1);
        os << "7";
        check(db.nodes.size(), orig_db_n_sz);
        os << endl;
        growth = prevpol_ng;
        min_growth = prevpol_mg;

    }

    void self_test() {
        check_out_to_hall(0);
        check_hall_to_node(0);
    }

    vector<us::gov::engine::peer_t*> connections;
    keys k;
};

void test_auth_app() {
    us::gov::engine::daemon_t d(0, us::gov::crypto::ec::keys::generate());

    test::auth_app o(cout, d);
    o.self_test();
}

struct auth_apps: vector<test::auth_app*>, us::test::test_platform {
    using b = vector<test::auth_app*>;
    using t = test_platform;

    vector<us::gov::engine::daemon_t*> d;

    auth_apps(ostream& os): t(os) {
        for (int i = 0; i < 20; ++i) {
            d.push_back(new us::gov::engine::daemon_t(0, us::gov::crypto::ec::keys::generate()));
            push_back(new test::auth_app(os, *d[i]));
        }
    }
    ~auth_apps() {
        for (auto& i: *this) delete i;
        for (auto& i: d) delete i;
    }

    void import0() {
        log("import0");
        int N = size();

        us::gov::engine::app::local_delta* ld[N];
        for (int i = 0; i < N; ++i) {
            ld[i] = (*this)[i]->create_local_delta();
        }
        us::gov::engine::auth::delta* d = new us::gov::engine::auth::delta();
        for (int i = 0; i < N; ++i) {
            d->merge(ld[i]);
        }
        d->end_merge();
        us::gov::engine::pow_t pow;

        for (int i = 0; i < N; ++i) {
            log("import node ", i, "auth app instance=", (*this)[i], "nodes instance=", &(*this)[i]->db.nodes);
            (*this)[i]->import(*d, pow);
        }
        delete d;
    }

    void import() {
        int N = size();
        for (int i = 0; i < N; ++i)
            (*this)[i]->basic_auth_completed((*this)[i]->connections[0], 2222);

        import0();
    }

    void tohall() {
        int N = size();
        auto prevpol_ng = (*this)[0]->growth;
        auto prevpol_mg = (*this)[0]->min_growth;
        for (int i = 0; i < N; ++i) {
            (*this)[i]->growth = 0;
            (*this)[i]->min_growth = 1.0;
        }

        os << "import\n";
        import();

        os << N << " different nodes from same ip connect to my test nodes" << endl;
        for (int i = 0; i < N; ++i) {
            os << i;
            check((*this)[i]->db.nodes.size(), (size_t)0);
        }
        os << endl;
        for (int i = 0; i < N; ++i) {
            auto NN = N;
            if (NN > ::us::gov::engine::auth::collusion_control_t::max_nodes_per_ip) NN = ::us::gov::engine::auth::collusion_control_t::max_nodes_per_ip;
            os << "i=" << i << " hall sz " << (*this)[i]->db.hall.size() << " max " << NN << " max_nodes_per_ip " << (int) ::us::gov::engine::auth::collusion_control_t::max_nodes_per_ip << endl;
            check((*this)[i]->db.hall.size(), (size_t)NN);
        }
        os << endl;

        for (int i = 0; i < N; ++i) {
            (*this)[i]->growth = prevpol_ng;
            (*this)[i]->min_growth = prevpol_mg;
        }
    }

    void growth() {
        log("growth");
        size_t N = size();
        cout << "size=" << N << endl;
        auto prevpol_ng = (*this)[0]->growth;
        auto prevpol_mg = (*this)[0]->min_growth;

        for (int i = 0; i < N; ++i) {
            (*this)[i]->growth = 0.1;
            (*this)[i]->min_growth = 1.0;
        }

        size_t nh = (*this)[0]->db.hall.size();
        double growth = (*this)[0]->growth;
        for (int i = 0; i < N; ++i) {
            os << "i=" << i << " db.hall sz=" << (*this)[i]->db.hall.size() << "; db.nodes. sz=" << (*this)[i]->db.nodes.size() << endl;
            check((*this)[i]->db.hall.size(), N);
            check((*this)[i]->db.nodes.size(), (size_t)0);
            check((*this)[i]->growth, growth);
        }
        os << "import" << endl;
        log("import");
        import();
        log("/ import");
        os << "/import" << endl;

        size_t s = floor((double)nh * growth);
        log("s=", s);
        for (int i = 0; i < N; ++i) {
            os << "i=" << i << " db.hall sz=" << (*this)[i]->db.hall.size() << "; db.nodes sz=" << (*this)[i]->db.nodes.size() << "; s=" << s << " N-s=" << (N-s) << endl;
            check((*this)[i]->db.nodes.size(), s);
            check((*this)[i]->db.hall.size(), N - s);
        }
        os << "ok" << endl;

        for (int i = 0; i < N; ++i) {
            (*this)[i]->growth = prevpol_ng;
            (*this)[i]->min_growth = prevpol_mg;
        }
    }

    void dismissed_nodes() {
        size_t N=size();
        auto prevpol_ng = (*this)[0]->growth;
        auto prevpol_mg = (*this)[0]->min_growth;
        for (int i = 0; i < N; ++i) {
            (*this)[i]->growth = 0;
            (*this)[i]->min_growth = 0;
        }

        auto sz = (*this)[0]->db.nodes.size();
        os << "A " << sz << endl;
        assert(sz != 0);
        for (int i = 0; i < N; ++i) {
            os << i; os.flush();
            check((*this)[i]->db.nodes.size(), sz);
            os << "-" << endl;
        }
        os << endl;
        for (int i = 0; i < N; ++i) {
            assert(!(*this)[i]->db.nodes.empty());
            (*this)[i]->db.nodes.begin()->second.seen-=20;
        }
        import0();
        os << "B" << endl;
        for (int i = 0; i < N; ++i) {
            os << i;
            check((*this)[i]->db.nodes.size(), sz - 1);
        }
        os << endl;
        for (int i = 0; i < N; ++i) {
            (*this)[i]->growth = prevpol_ng;
            (*this)[i]->min_growth = prevpol_mg;
        }
    }

    void self_test() {
        int N = size();
        auto save_mnpi = ::us::gov::engine::auth::collusion_control_t::max_nodes_per_ip;
        ::us::gov::engine::auth::collusion_control_t::max_nodes_per_ip = N;
        os << "empty db" << endl;
        for (int i = 0; i < N; ++i) {
            os << i;
            check((*this)[i]->db.nodes.size(), (size_t)0);
            check((*this)[i]->db.hall.size(), (size_t)0);
        }
        os << endl;
        os << "tohall" << endl;
        tohall();
        for (int i = 0; i < N; ++i) {
            os << i << " check nodes sz=0, hall sz=" << N << endl;
            check((*this)[i]->db.nodes.size(),(size_t)0);
            check((*this)[i]->db.hall.size(),(size_t)N);
        }
        os << "growth" << endl;
        growth();
        os << "dismissed nodes" << endl;
        dismissed_nodes();
        ::us::gov::engine::auth::collusion_control_t::max_nodes_per_ip = save_mnpi;
    }
};

void test_auth_apps() {
    auth_apps o(cout);
    o.self_test();
}

void test_auth() {
    #ifndef CFG_PERMISSIONLESS_NETWORK
        cout << "Tests require CFG_PERMISSIONLESS_NETWORK" << endl;
        assert(false);
    #endif
    log("auth", "max_nodes_per_ip", (int)::us::gov::engine::auth::collusion_control_t::max_nodes_per_ip);
    auto mnpi = ::us::gov::engine::auth::collusion_control_t::max_nodes_per_ip;
    cout << "auth app" << endl;
    test_auth_app();
    assert(::us::gov::engine::auth::collusion_control_t::max_nodes_per_ip == mnpi);
    cout << "auth apps" << endl;
    test_auth_apps();
    assert(::us::gov::engine::auth::collusion_control_t::max_nodes_per_ip == mnpi);
}

}}

