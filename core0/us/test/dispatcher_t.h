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
#pragma once
#include <condition_variable>
#include <mutex>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <atomic>
#include <us/wallet/cli/hmi.h>
#include "main.h"

namespace us { namespace test {
    using namespace std;

    struct dispatcher_t: us::wallet::cli::hmi::dispatcher_t {
        using b = us::wallet::cli::hmi::dispatcher_t;

        dispatcher_t(const string& id, walletx_t& i, ostream&);
        static string codename(uint16_t code);
        static void print(const string& id, const hash_t& tid, uint16_t code, const vector<uint8_t>&, ostream&);
        bool dispatch(us::gov::socket::datagram*);

        struct expected_code_t: map<uint16_t, int> { //num ocurrences, mode 0:exact, 1:minimum
            expected_code_t(const string& id, ostream&);

            void arrived(const hash_t& h, uint16_t code, const vector<uint8_t>& s);
            void dump(ostream& os) const;
            void arrived(uint16_t code);
            void check_payload(const hash_t& tid, uint16_t code, const vector<uint8_t>&, ostream&);
            void wait();
            void wait_no_clear();
            void decrement(uint16_t code);
            void increase(uint16_t code);
            void increase_or_set_1_if_nonpos(uint16_t code);
            void add_exact_occurrences(uint16_t code, int);
            void add_minimum_occurrences(uint16_t code, int);
            bool all_empty() const;
            void clear_all();
            void check_not_expecting() const;
            int num_expected() const;
            void default_step_wait() { step_wait_ms = 20000; }

            struct check_t: map<uint16_t, function<void(const hash_t&, uint16_t code, const vector<uint8_t>&)>> {
            };

            ostream& out;
            string id;
            check_t check;
            bool enabled{false};
            mutex mx;
            condition_variable cv;
            function<void(ostream&)> reftest{[](ostream&){}}; //prints reference to the current test
            set<uint16_t> marked_minimum;
            uint64_t step_wait_ms;
        };

        ostream& out;
        string id;
        bool muted{false};
        static atomic<int> dgram_count;
        static atomic<int> dgram_count_test;
        expected_code_t expected_code;

        using data_t = us::wallet::trader::data_t;
        struct data_seq_t: vector<data_t> {
            data_seq_t();
            bool add(const string&);
            void dump(ostream&) const;
        };

        data_seq_t data_seq;

    };

}}

