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
#include <us/gov/types.h>
#include <us/gov/config.h>
#include <us/gov/engine/evidence.h>

#include "sigcode_t.h"
#include "tx/sections_t.h"

namespace us::gov::cash {

    using namespace std;

    struct tx_t final: engine::evidence {
        using b = engine::evidence;

        static constexpr eid_t eid{0};

        using inputs_t = tx::inputs_t;
        using outputs_t = tx::outputs_t;
        using section_t = tx::section_t;
        using sections_t = tx::sections_t;

        tx_t();
        tx_t(const tx_t&);
        ~tx_t() override {}

        sigcode_t get_sigcode_all() const;
        sigcode_t make_sigcode_all() const;
        string name() const override { return "cash::tx"; }
        bool check_amounts() const override;
        sigmsg_hash_t get_hash(const sigcode_t&) const;
        void write_sigmsg(sigmsg_hasher_t&, const sigcode_t&) const;
        void write_pretty_en(ostream&) const override;
        void write_pretty_es(ostream&) const override;
        void dump(const string& prefix, ostream&) const;
        bool verify(ostream&) const override;
        section_t& add_section(const hash_t& token);
        string pay_amounts_line() const;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        sections_t sections;
    };

    static ostream& operator << (ostream& os, const cash::tx_t& t) {
        os << t.encode();
        return os;
    }

}

