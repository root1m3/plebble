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
#include <mutex>

#include <us/gov/types.h>
#include <us/gov/io/seriable.h>
#include <us/gov/peer/account_t.h>
#include <us/gov/peer/nodes_t.h>
#include <us/gov/types.h>

namespace us { namespace gov { namespace traders {

    struct db_t final: io::seriable {
        using account_t = peer::account_t;
        using wallets_t = peer::nodes_t;

        db_t();
        void hash(hasher_t&) const;
        void dump(const string& prefix, ostream&) const;
        void clear();
        void collect(vector<tuple<hash_t, host_t, port_t>>&) const;
        void collect(vector<hash_t>&) const;
        pair<ko, account_t> lookup(const hash_t&) const;
        pair<ko, tuple<hash_t, host_t, port_t>> random_wallet() const;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        wallets_t wallets;
        mutable mutex mx_wallets;
    };

}}}

