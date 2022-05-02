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
#include <unordered_map>
#include <unordered_set>
#include <mutex>

#include <us/gov/types.h>
#include <us/gov/crypto/types.h>
#include <us/gov/config.h>

#include "types.h"

namespace us::gov::engine {

    struct votes_t final {
        #if CFG_COUNTERS == 1
            unordered_map<hash_t, const hash_t> voters;
        #else
            unordered_set<hash_t> voters;
        #endif
        unordered_map<hash_t, uint64_t> votes;
        void clear();
        void dump(const string& prefix, ostream&) const;
        bool add(const pubkey_t::hash_t&, const hash_t&);
        hash_t select();
        void logline(ostream&) const;

        #ifdef CFG_PERMISSIONED_NETWORK
            hash_t master_vote{0};
        #endif

        bool closed{false};
        mutable mutex mx;
    };

}

