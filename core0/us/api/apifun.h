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
#include <string_view>
#include <vector>
#include <iostream>
#include <map>
#include <set>

namespace us::apitool {

    using namespace std;

    struct api_t;

    struct apifun  {
        using mne = string;
        using argname = string;

        struct svcfish_entry_t {
            char op;
            string from_service;
            int from_svc;
            string to_service;
            int to_svc;
            string comment;
        };

        using svcfish_db_t = vector<svcfish_entry_t>;

        struct io_types_t: vector<pair<mne, argname>> { //mnemonic
            bool from_stream(istream&);
            string id() const;
            string identifierX() const;
            static string identifierX(const vector<pair<string, string>>&);
            static string identifierX(const string& constructor);
            static constexpr string_view arg_prefix{"_arg_"};
            bool peerinfo{false};
            bool pass_dgram{false};
            bool async_handler{false}; //default responses are handled with rendezvous
            bool heap{false}; //handler. false: by ref; true: by ptr
        };

        void collect_in_specs(map<string, vector<pair<string, string>>>&) const;
        void collect_out_specs(map<string, vector<pair<string, string>>>&) const;
        static apifun from_stream(istream&);
        void compute_get_protocol_vector(const string& prefix, vector<pair<string, bool>>& r, int& nextsvc);

        bool is_sync() const { return sync_type == "sync"; }
        static apifun example1();

        struct netsvc_t {
            map<int, string> netsvc_net;
            map<string, int> netsvc_net2;
            map<string, int> netsvc_hit;

            map<int, string> netsvc;
            //map<string, int> netsvc2;
        };
        void compute_netsvc(netsvc_t&, bool& ch, svcfish_db_t& db, svcfish_db_t& dbinv);
        void hitfn(netsvc_t& netsvc, const string& serv) const;

    public:
        io_types_t in;
        io_types_t out;

        string name;
        string fcgi;
        string service;
        int svc{-1};
        int svc_ret{-1};
        string custom_rpc_impl;
        string sync_type;
        string _api_stream_write_fn_sfx;
        api_t* api{nullptr};
    };

}

