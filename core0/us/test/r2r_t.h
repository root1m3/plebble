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
#include <string>
#include <functional>

#include <us/gov/io/seriable.h>

#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/data_t.h>

#include "main.h"
#include "network.h"

#define Fail() fail_(__FILE__, __LINE__)
#define Failb(x) fail_(x, __FILE__, __LINE__)
#define Fail_KO(x) fail_(is_ko(x), __FILE__, __LINE__)

#define Check_s_contains(x, y) fail_(x.find(y)==string::npos, __FILE__, __LINE__)
#define Check_s_not_contains(x, y) fail_(x.find(y)!=string::npos, __FILE__, __LINE__)
#define Check_s_eq(x, y) fail_(x!=y, __FILE__, __LINE__)
#define Check_s_not_eq(x, y) fail_(x==y, __FILE__, __LINE__)

#define Check_data(node, key, expected) check_data_(node, key, expected, __FILE__, __LINE__)
#define Check_entry_exists(x, key) check_entry_exists_(x, key, __FILE__, __LINE__)


namespace us::test {
    using namespace std;

    struct node;
    struct dispatcher_t;

    struct r2r_t {

        using data_t = us::wallet::trader::data_t;
        using roles_t = us::wallet::trader::roles_t;
        using blob_reader_t = us::gov::io::blob_reader_t;

        r2r_t(network&);

        void wait(node&, node&);
        void wait(node&, node&, uint64_t timeout_ms);
        void wait_no_clear(node&, node&);
        void wait_no_clear1(node&, node&);
        void wait_no_clear2(node&, node&);
        void curtest(node&, node&, const string& title, const char*file, int line);
        void curtest_cont(node&, node&, const string& title, const char*file, int line);
        void test_r2r_cfg(node&, node&, function<void(node&, node&)> foo, hash_t& tid);
        void banner(node&, node&, ostream& os);
        void check_entry_exists_(node&, const string& key, const char* file, int line);
        void check_data_(node&, const string& key, const string& expected, const char* file, int line);
        string get_data(node&, const string& key);
        void check_data_null(node&, const string& key);
        void exec_cur_trade(node&, const string& command_line);
        void fail_(const char* file, int line);
        void fail_(bool, const char* file, int line);
        void test_trade_start_dialog_a(node&, node&, hash_t& trade_id, int n1_num_protocols_seq0, int n1_num_protocols_seq1, int n2_num_protocols_seq0);
        void test_trade_start_dialog_b(node&, node&, hash_t& trade_id, int num_protocols0, int num_protocols1);
        static string to_string(const hash_t&);
        static int testseq;
        ostream& out;
        network& n;
        static int wait_from_seq;
    };

}

