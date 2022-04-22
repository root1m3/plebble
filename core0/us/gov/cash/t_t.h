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
#include <us/gov/io/seriable_map.h>

namespace us { namespace gov { namespace cash {

    struct accounts_t;

    struct t_t final: io::seriable_map<hash_t, cash_t> {
        t_t();
        t_t(const t_t&);
        t_t& operator = (const t_t&) = delete;
        void dump(const string& prefix, const hash_t& addr, ostream&) const;
        void hash_data_to_sign(sigmsg_hasher_t&) const;
        void hash_data(hasher_t&) const;
        bool burn(const hash_t& token, const cash_t&);
        cash_t get_value(const hash_t& token) const;
        void set_supply(const hash_t& token, const cash_t& supply);
        void merge(const t_t&);
        void on_destroy(accounts_t& ledger);

    };

}}}

