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
#include <string>
#include <us/gov/engine/evidence.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/engine/signed_data.h>
#include <us/gov/config.h>

namespace us { namespace gov { namespace sys {

    struct install_script final: engine::evidence, engine::signed_data {
        using b = engine::evidence;
        using s = engine::signed_data;

        static constexpr eid_t eid{0};

        install_script();
        ~install_script() override;

        string name() const override { return "sys::install_script"; }
        void hash_data_to_sign(sigmsg_hasher_t&) const override;
        void hash_data(hasher_t&) const override;
        bool verify(ostream&) const override;
        void write_pretty_en(ostream&) const override;
        void write_pretty_es(ostream&) const override;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        string script;
    };

}}}

