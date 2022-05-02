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
#include <unistd.h>
#include <iostream>
#include <fstream>

#include <us/gov/config.h>
#include <us/gov/crypto/types.h>
#include <us/gov/engine/syncd.h>

#include "test_platform.h"

#define loglevel "test"
#define logclass "syncd"
#include "logs.inc"

#include "assert.inc"

namespace us { namespace test {

using namespace std;
using namespace us;

struct test_actuator: us::gov::engine::syncd_t::actuator, us::test::test_platform {
    using t = test_platform;

    test_actuator(ostream& os):t(os) {
    }

    bool get_prev_block(const hash_t& h, hash_t& prev) const {
        cout << "actuator call: get_prev " << h << endl;
        ++get_prev_counter;
        if (h.is_zero()) {
            return false;
        }
        prev=hash_t(h.uint32()-1);
        return true;
    }

    int neighbours_queryed{2};

    int call_query_block(const hash_t& hash) {
        ++query_block_counter;
        return neighbours_queryed;
    }

    void clear() {
        ++clear_counter;
    }

    bool patch_db(vector<hash_t>& patches) {
        ++patch_db_counter;
        return true;
    }

    void on_sync() {
        ++on_sync_counter;
    }
    int on_sync_counter{0};
    int patch_db_counter{0};
    int clear_counter{0};
    int query_block_counter{0};
    mutable int get_prev_counter{0};

    void self_test() {
        hash_t p(7);
        check(p.uint32(), (uint32_t)7);
        {
            bool r = get_prev_block(hash_t(3), p);
            check(r,true);
            check(p, hash_t(2));
        }
        {
            bool r = get_prev_block(hash_t(2), p);
            check(r,true);
            check(p, hash_t(1));
        }
        {
            bool r = get_prev_block(hash_t(1), p);
            check(r, true);
            check(p, hash_t(0));
        }
        {
            bool r = get_prev_block(hash_t(0), p);
            check(r, false);
        }
    }

};

// core0 must be configured with either --testdebug or --testrelease flags ([test] removes final trait from classes.)
struct syncd_t: us::gov::engine::syncd_t, us::test::test_platform {
    using b = us::gov::engine::syncd_t;
    using t = test_platform;

    test_actuator ac;

    syncd_t(ostream&os): b(&ac), t(os), ac(os) {
    }

    void self_test() {
        ac.self_test();
    }

};

void test_syncd() {
    syncd_t syncd(cout);
    syncd.start();
    this_thread::sleep_for(1s);

    syncd.self_test();

    syncd.stop();
    syncd.join();
}

}}

