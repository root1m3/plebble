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
#include <vector>
#include "main.h"

namespace us { namespace test {
    using namespace std;

    struct dispatcher_t;

    struct node {
        using bookmarks_t = us::wallet::trader::bookmarks_t;

        node(const string& id, const string& root_homedir, const string& root_logdir, const string& root_vardir, uint16_t gport, uint16_t wport);
        virtual string desc() const = 0;
        virtual void banner(ostream&) const = 0;

        string ep() const;
        void start_govd(bool init_chain = false);
        void create_walletd();
        void create_node(const string&);

        virtual vector<string> r2r_libs(bool filter_not_active);

        void install_r2r_libs();

        void gov_cli_start();
        void create_wallet_cli();
        void start_gov(bool init_chain);
        pair<vector<uint16_t>, vector<uint16_t>> doctypes() const;
        void start_walletd_cli();
        void stop();
        void join();
        void test_list_protocols();
        hash_t create_coin(int64_t supply);
        void restart_daemons();
        bookmarks_t bookmarks() const;
        void register_wallet();

        static string localip;
        string homedir;
        string logdir;
        string vardir;
        uint16_t gport;
        uint16_t wport;
        string id;
        govx_t* gov{nullptr};
        govx_t* gov_cli{nullptr};
        walletx_t* wallet{nullptr};
        walletx_t* wallet_cli{nullptr};
        dispatcher_t* wallet_cli_dis{nullptr};
        bool created{false};
        hash_t wallet_pkh;
    };

}}

