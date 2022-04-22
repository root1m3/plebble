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
#include <us/gov/types.h>
#include <us/gov/config.h>
#include <us/gov/engine/evidence.h>

#include "locking_program_input_t.h"

namespace us { namespace gov { namespace cash {

    struct map_tx final: engine::evidence {
        using b = engine::evidence;

        static constexpr eid_t eid{1};

        map_tx();
        ~map_tx() override {}

        string name() const override { return "cash::map_tx"; }
        sigmsg_hash_t get_hash() const;
        void write_sigmsg(sigmsg_hasher_t&) const;
        void write_pretty_en(ostream& os) const override;
        void write_pretty_es(ostream& os) const override;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        hash_t address;
        locking_program_input_t locking_program_input;
        string key;
        string value;
    };

    static ostream& operator << (ostream& os, const cash::map_tx& t) {
        os << t.encode();
        return os;
    }

}}}

