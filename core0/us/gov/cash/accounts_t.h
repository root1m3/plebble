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
#include "account_t.h"
#include <us/gov/io/seriable_map.h>
#include <unordered_map>

namespace us { namespace gov { namespace cash {

    struct accounts_t final: io::seriable_map<hash_t, account_t> {
        using b = io::seriable_map<hash_t, account_t>;

        static const char* KO_69213;

    public:
        accounts_t();
        ~accounts_t();

    public:
        using batch_item = account_t;
        struct batch_t final: unordered_map<hash_t, batch_item> {
            void add(const hash_t& address, const batch_item&);
        };

    public:
        void add(const batch_t& batch);
        account_t merge() const;

    public:
        cash_t get_balance() const;
        bool pay(const hash_t& k, const cash_t& amount);
        bool withdraw(const hash_t& k, const cash_t& amount);
        cash_t sum() const;

    public:
        ko print_data(const hash_t& addr, ostream&) const;
        void print_kv_b64(const hash_t& addr, ostream&) const;
        void dump(const string& prefix, int detail, ostream&) const;

    public:
        void list_files(const hash_t& addr, const string& path, ostream&) const;
        void get_files(vector<pair<hash_t, pair<hash_t, fileattr_t>>>&) const;
        void get_files(vector<pair<hash_t, fileattr_t>>&) const;
        void get_files(vector<pair<hash_t, uint32_t>>&) const;
        void get_files(set<hash_t>&) const;
        hash_t file_hash(const hash_t& addr, const string& path) const;

    public:
        void hash_data_to_sign(sigmsg_hasher_t&) const;
        void hash_data(hasher_t&) const;

    };

}}}

