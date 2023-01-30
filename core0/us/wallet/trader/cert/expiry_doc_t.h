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
#include "doc0_t.h"
#include <string>
#include <us/gov/socket/types.h>

namespace us::wallet::trader::cert {

    using namespace us::gov;

    struct expiry_doc_t: doc0_t {
        using b = doc0_t;

        struct options: b::options {
            ko parse(const string&, istream&) override;

            static void exec_help(const string& prefix, ostream&);
            void dump(const string& pfx, ostream&) const;

            int xhours{0};
        };

       expiry_doc_t();
        ~expiry_doc_t() override;

        void init(const string& text, const params_t& kv, int expires_hours);
        ko init(const options&);
        void write_pretty_en(ostream&) const override;
        void write_pretty_es(ostream&) const override;
        bool verify(ostream&) const override;
        using b::verify;
        void hash_data_to_sign(sigmsg_hasher_t&) const override;
        void hash_data(hasher_t&) const override;
        void set(const string& key_prefix, ch_t&) const override;
        void unset(const string& key_prefix, ch_t&) const override;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        ts_t expiry;
    };

}

