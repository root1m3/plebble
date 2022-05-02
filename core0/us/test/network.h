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

#include <map>
#include <string>
#include <us/gov/engine/auth/db_t.h>
#include <us/wallet/trader/workflow/doctype_processors_t.h>
#include <us/wallet/trader/bookmarks_t.h>
#include "main.h"

namespace us { namespace test {
    using namespace std;

    struct node;

    struct network: map<string, node*> {
        using bookmarks_t = us::wallet::trader::bookmarks_t;
        using doctype_processors_t = us::wallet::trader::workflow::doctype_processors_t;

        network(const string& homedir, const string& logdir, const string& vardir, const string& stage1dir, ostream&);
        virtual ~network();

        void add_node(const string& id, node*);

        void stop();
        void mute_all();
        ko blobfeed_upload();
        ko blobfeed_install();
        ko android_app_test_automatic_updates();
        ko android_app_test();
        void consoles_shell();
        void start_govs(bool make_stage);
        void start_wallets();
        virtual void stage1_configure() = 0;
        virtual void stage1_ff_configure() = 0;
        void stage1_create();
        void stage1_ff();
        void stage1();
        void test_list_protocols();
        void watch();
        size_t nh(const us::gov::engine::auth::db_t&) const;
        size_t nn(const us::gov::engine::auth::db_t&) const;
        bool check_nh(); //hall+nodes must not contain mopre entries than the network size
        bool check_num_addresses();
        void check_sync();
        bool check();
        void wait_settlement(govx_t& gov_cli, uint64_t ts);
        pair<blob_t, hash_t> transfer(walletx_t& from, walletx_t& to, const hash_t& coin, const cash_t& amount);
        void transfer_wait(node& from, node& to, const hash_t& coin, const cash_t& amount);
        void menu();
        bookmarks_t filtered_bookmarks(const string& nodename, const string& exclude_label); //"bank"
        struct static_tests {
            void test_endpoint();
            void test_endpoint(const string&, const string&);
            void test_arch();

            void test();
        };
        static void test();
        bookmarks_t bookmarks() const;

    public:
        static string OFAddress;
        static string local_blob;
        int num_in_sync{0};
        int frame{0};
        node* genesis_node{nullptr};
        string homedir;
        doctype_processors_t doctype_processors;
        string stage1dir;
        ostream& out;
    };

}}

