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
#include <array>
#include <us/gov/io/seriable.h>
#include <us/gov/engine/evidence.h>
#include <us/gov/crypto/ripemd160.h>
#include <us/gov/engine/signed_data.h>
#include <us/wallet/trader/ch_t.h>
#include <us/wallet/trader/kv.h>

namespace us::wallet::trader::cert {

    using namespace us::gov;

    struct doc0_t: virtual gov::io::seriable {
        using params_t = us::wallet::trader::kv;
        using ch_t = trader::ch_t;
        using magic_t = uint16_t;

        static constexpr auto begin_of_document = "-- BEGIN OF DOCUMENT --------------------------------------------------------------";
        static constexpr auto end_of_document = "-- END OF DOCUMENT ----------------------------------------------------------------";
        static constexpr auto begin_of_document_es = "-- DOCUMENTO ----------------------------------------------------------------------";
        static constexpr auto end_of_document_es = "-- FIN DEL DOCUMENTO --------------------------------------------------------------";

        doc0_t();
        virtual ~doc0_t();

        enum type_t: uint8_t{
            type_string,
            type_image,
            type_bookmarks,

            num_types
        };

        constexpr static array<const char*, num_types> typestr = {"string", "image", "bookmarks"};

        struct options {
            static constexpr auto KO_31998 = "KO_31998 Invalid option.";
            ko parse_cmdline(istream&);
            virtual ko parse(const string&, istream&);

            static void exec_help(const string& prefix, ostream&);
            void dump(const string& pfx, ostream& os) const;

            string text;
            params_t kv;
        };
        ko init(const options&);

        template<typename H>
        void hashmsg(H& h) const {
            h.write(ts);
            params.hash(h);
            h.write(buf);
        }

        inline uint64_t get_ts() const { return ts; }
        virtual magic_t get_magic() const = 0;
        virtual const char* get_name() const = 0;
        virtual void set(const string& key_prefix, ch_t&) const;
        virtual void unset(const string& key_prefix, ch_t&) const;
        virtual bool verify(ostream&) const;
        bool verify() const;
        virtual void hash_data_to_sign(sigmsg_hasher_t&) const;
        virtual void hash_data(hasher_t&) const;

        public:
        virtual string title() const = 0;
        virtual string title_es() const = 0;
        string lang() const;
        bool tamper(const string& search, const string& replace);
        void write_pretty(ostream&) const;
        virtual void write_pretty_en(ostream&) const;
        virtual void write_pretty_es(ostream&) const;
        string as_string() const;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        ts_t ts;
        params_t params;
        vector<uint8_t> buf;
        type_t type{type_string};
    };

}

