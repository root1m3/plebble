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
#include <string_view>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

#include "model.h"
#include "apifun.h"

namespace us::apitool {

    using namespace std;

    struct apifun;
    struct model;

/*
function name encoded in datagram svc.
caller side (src) *-------------------wire---------------------------* (dst) handler side
f(in_t&& o_in, out_dst_t& o_out)                                       function(in_dst_t&&, out_t& o_out)
                            [src] ---- datagram payload: o_in -------> [dst]
                            [dst] <-- response dgram payload: o_out -- [src]
*/

    struct coder {
        using mne_t = string;
        using targ = pair<string, string>;
        using targs = vector<targ>;
        using svcfish_entry_t = apifun::svcfish_entry_t;


        coder(model& m): m(m) {}
        virtual ~coder() {}
        static void feedback();
        void write_file_prefix(const api_t&, ostream&) const;
        void write_file_prefix_all_langs(ostream&) const;
        void write_file_prefix(ostream&) const;
        virtual string lang() const = 0;
        virtual string line_comment() const = 0;

        string line_comment__bash() const { return "# "; }

        pair<string, string> names_in(const apifun&, bool side_caller) const;
        pair<string, string> names_out(const apifun&, bool side_caller) const;
        static void sides_prefix(bool side_caller, ostream&);
        static string fn_name(const apifun& f, bool side_caller);
        virtual string get_type(const string& nne) const = 0;

        void generate() const;
        void gen_gov_daemon_api() const;
        void do_gov_daemon_api(const api_t&, ostream& counters_include) const;
        virtual void gen_service_handler_headers(const api_t&, const string& scope) const {}
        void gen_service_handlers(const api_t&, const string& scope) const;
        void gen_service_handlers(const api_t&, const string& scope, bool side_caller, ostream&) const;
        virtual bool gen_service_handlers(const apifun&, const string& scope, bool side_caller, ostream&) const = 0;
        virtual bool gen_service_handlers_response(const apifun&, const string& scope, bool side_caller, ostream&) const = 0;
        virtual void gen_gov_protocol_counters_init(const api_t&, ostream& include) const {}
        void gen_protocol(const api_t&) const;
        virtual void gen_protocol(const api_t&, ostream&) const = 0;
        void do_common_api(const api_t&) const;
        void gen_purevir(const api_t&) const;
        void gen_purevir(const api_t&, bool side_caller) const;
        void gen_purevir(const api_t&, bool side_caller, ostream&) const;
        virtual void gen_purevir(const apifun&, bool side_caller, ostream&) const = 0;

        void gen_override(const api_t&) const;
        void gen_override(const api_t&, bool side_caller) const;
        void gen_override(const api_t&, bool side_caller, ostream&) const;
        virtual void gen_override(const apifun&, bool side_caller, ostream&) const {};

        void gen_rpc_impl(const api_t&) const;
        void gen_rpc_impl(const api_t&, bool side_caller, ostream&) const;
        virtual bool gen_rpc_impl(const apifun&, bool side_caller, ostream&) const = 0;

        void gen_local_impl(const api_t&) const;
        void gen_local_impl(const api_t&, ostream&) const;
        virtual bool gen_local_impl(const apifun&, ostream&) const = 0;

        void gen_service_router(const api_t&) const;
        void gen_service_router(const api_t&, bool side_caller, ostream&) const;
        virtual void gen_service_router(const apifun&, bool side_caller, ostream&) const = 0;

        void gen_dto(const api_t&) const;
        void gen_dto(const api_t&, bool side_caller) const;
        void gen_dto_hdr(const api_t&, bool side_caller, ostream&) const;
        void gen_dto_impl(const api_t&, bool side_caller, ostream&) const;

        virtual void gen_dto_out_hdr(const apifun&, bool side_caller, ostream&) const = 0;
        virtual void gen_dto_out_impl(const apifun&, bool side_caller, ostream&) const = 0;
        virtual void gen_dto_in_hdr(const apifun&, bool side_caller, ostream&) const = 0;
        virtual void gen_dto_in_impl(const apifun&, bool side_caller, ostream&) const = 0;

        void gen_svc_v() const;
        void gen_svc_v0() const;
        void gen_svc_list() const;
        void gen_svcfish() const;
        void gen_svcfish_inv() const;
        void gen_svcfish(ostream&) const;
        void gen_svcfish_inv(ostream&) const;
        virtual void write_svcfish_entry(const svcfish_entry_t&, ostream&) const = 0;
        void write_svcfish_entry_comment(const svcfish_entry_t&, ostream&) const;

    public:
        static bool feedback_enabled;
        model& m;
    };

}


