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
#include "coder.h"
#include "apifun.h"

namespace us::apitool {
    struct apifun;
    struct model;
}

namespace us::apitool::tex {

    using namespace std;

    struct coder final: apitool::coder {
        using b = apitool::coder;

        coder(model& m): b(m) {}

        string lang() const override { return "tex"; }
        string line_comment() const override { return "%"; }

        void generate() const;
        void generate(const api_t&) const;
        void do_fn(const apifun&, ostream&) const;

        string get_type(const string&) const override { return ""; }
        bool gen_service_handlers(const apifun&, const string& scope, bool side_caller, ostream&) const override { return false; }
        bool gen_service_handlers_response(const apifun&, const string& scope, bool side_caller, ostream&) const override { return false; }
        void gen_protocol(const api_t&, ostream&) const override {}
        void gen_purevir(const apifun&, bool side_caller, ostream&) const override {}
        bool gen_rpc_impl(const apifun&, bool side_caller, ostream&) const override { return false; }
        bool gen_local_impl(const apifun&, ostream&) const override { return false; }
        void gen_service_router(const apifun&, bool side_caller, ostream&) const override {}
        void gen_dto_in_hdr(const apifun&, bool side_caller, ostream&) const override {}
        void gen_dto_in_impl(const apifun&, bool side_caller, ostream&) const override {}
        void gen_dto_out_hdr(const apifun&, bool side_caller, ostream&) const override {}
        void gen_dto_out_impl(const apifun&, bool side_caller, ostream&) const override {}

        void write_svcfish_entry(const svcfish_entry_t&, ostream&) const override;
    };

}

