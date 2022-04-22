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
#include <tuple>
#include <vector>
#include <us/gov/types.h>
#include <us/gov/io/seriable_map.h>
#include "account_t.h"

namespace us { namespace gov { namespace peer {

    struct nodes_t final: io::seriable_map<hash_t, account_t> {
        using b = io::seriable_map<hash_t, account_t>;

        using account_type = account_t;
        using index_type = hash_t;

        static const char* KO_82978, *KO_92276;

        nodes_t() {}
        nodes_t(const nodes_t&);

        void dump(const string& prefix, ostream&) const;
        void hash_data_to_sign(sigmsg_hasher_t&) const;
        void hash_data(hasher_t&) const;
        void dump_as_seeds(ostream&) const;
        void collect(vector<tuple<hash_t, host_t, port_t>>&) const;
        void collect(vector<hash_t>&) const;
        static ko check(const hash_t& id, const account_t&, channel_t);
        pair<ko, account_t> lookup(const hash_t& p) const;
        pair<ko, tuple<hash_t, host_t, port_t>> random_entry() const;

    };

}}}


