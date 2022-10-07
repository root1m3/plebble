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
#include <sstream>

#include <us/gov/crypto/ec.h>
#include <us/gov/crypto/ripemd160.h>
#include <us/gov/crypto/base58.h>
#include <us/gov/io/cfg0.h>
#include <us/gov/io/seriable_map.h>
#include <us/gov/cash/tx_t.h>

#include "types.h"

namespace us::wallet::trader {

    struct kv final: gov::io::seriable_map<string, string> {
        using b = gov::io::seriable_map<string, string>;
        using b::clear;
        using b::empty;
        using b::blob_size;
        using b::to_blob;
        using b::from_blob;

        kv() {}
        kv(const kv& other): b(other) {}
        ~kv() {}

        using b::size;

        bool override_with(const kv&);
        inline kv& operator = (const kv& other) { static_cast<b&>(*this) = static_cast<const b&>(other); return *this; }
        bool operator != (const kv&) const;
        void hash(sigmsg_hasher_t&) const;
        void hash(hasher_t&) const;
        void dump(const string& prefix, ostream&) const;
        void write_pretty(const string& prefix, ostream&) const;
        int get(const string& key, int def) const;
        uint64_t get(const string& key, const uint64_t& def) const;
        hash_t get(const string& key, const hash_t& def) const;
        cash_t get(const string& key, const cash_t& def) const;
        static char yn(bool b) { return b ? 'Y' : 'N'; }

        template<typename t>
        bool set(const string& key, const t& value) {
            ostringstream os;
            os << value;
            return sets(key, os.str());
        }
        bool sets(const string& key, const string& value);
        bool unset(const string& key);

        template<typename t>
        bool underride(const string& key, const t& value) {
            ostringstream os;
            os << value;
            return underrides(key, os.str());
        }
        bool underrides(const string& key, const string& value); //only writes if key doesnt exist, leaving untouched the existing content otherwise. Returns true if entry was created.

        string get(const string&, const string& def) const;

        static string hide_skvalue(const string& key,const string& value);
        static bool dump_b64(const string& prefix, const string& keyname, const string& value, ostream&);
        string to_b64() const;

    public:
        void to_streamX(ostream&) const;
        ko from_streamX(istream&);
        bool loadX(istream&); //return true if something changed
        bool loadX(const string& file); //return true if something changed
        bool load_b64X(const string& b64); //return true if something changed
        void saveX(const string& file) const;

    };

}

