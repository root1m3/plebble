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
#pragma once
#include "coder.h"
#include "apifun.h"
#include <set>
#include <functional>

namespace us { namespace apitool {
    struct apifun;
    struct model;
}}

namespace us { namespace apitool { namespace tex {

    using namespace std;

    struct coder final: apitool::coder {
        using b = apitool::coder;

        static string protocol_prefix;
        static unordered_map<string, string> types;

        coder(model& m): b(m) {}

        string lang() const override { return "tex"; }
        void gen_fcgi_index(const api_t&) const;
        void gen_fcgi_index(const api_t&, ostream&) const;
        void gen_fcgi_index(const apifun&, ostream&) const;
        string get_type(const string& nne) const override;
        void gen_service_handler_headers(const api_t&, const string& scope, bool side_caller, ostream&) const;
        void gen_service_handler_headers(const api_t&, const string& scope) const override;
        bool gen_service_handlers(const apifun&, const string& scope, bool side_caller, ostream&) const override;
        bool gen_service_handlers_response(const apifun&, const string& scope, bool side_caller, ostream&) const override;
        void gen_gov_protocol_counters_init(const api_t&, int base, ostream& include) const override;
        void gen_protocol(const api_t&, int nbase, ostream&) const override;
        void gen_purevir(const apifun&, bool side_caller, ostream&) const override;
        void gen_override(const apifun&, bool side_caller, ostream&) const override;
        bool gen_rpc_impl(const apifun&, bool side_caller, ostream&) const override;
        void gen_impl_response(const apifun&, bool side_caller, function<void(const apifun& f, const string& pfx, bool side_caller, ostream&)> implementation, ostream&) const;
        void gen_impl(const apifun&, bool side_caller, function<void(const apifun& f, const string& pfx, bool side_caller, ostream&)> implementation, ostream&) const;
        bool gen_local_impl(const apifun&, ostream&) const override;
        void gen_service_router(const apifun&, bool side_caller, ostream&) const override;
        void gen_dto_out_hdr(const apifun&, bool side_caller, ostream&) const override;
        void gen_dto_in_hdr(const apifun&, bool side_caller, ostream&) const override;
        void gen_dto_in_dst_constructor(const apifun&, ostream&) const;
        void gen_dto_in_constructor(const apifun&, ostream&) const;
        void gen_dto_in_constructor_args(const apifun&, bool names, ostream&) const;
        void gen_dto_out_constructor(const apifun&, ostream&) const;
        void gen_dto_out_dst_constructor(const apifun&, ostream&) const;
        void gen_dto_blob_size(const apifun&, const apifun::io_types_t&, ostream&) const;
        void gen_dto_to_blob(const apifun&, const apifun::io_types_t&, ostream&) const;
        void gen_dto_from_blob(const apifun&, const apifun::io_types_t&, ostream&) const;
        void gen_dto_get_datagram(const apifun&, const apifun::io_types_t&, const string& prot_sfx, ostream&) const;
    };

}}}

