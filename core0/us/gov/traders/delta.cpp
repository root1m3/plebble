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
#include "delta.h"

#include <functional>
#include <random>

#include <us/gov/config.h>
#include <us/gov/auth/peer_t.h>
#include <us/gov/engine/daemon_t.h>

#include "wallet_address.h"
#include "local_delta.h"
#include "types.h"

#define loglevel "gov/traders"
#define logclass "delta"
#include <us/gov/logs.inc>

using namespace us::gov::traders;
using c = us::gov::traders::delta;

void c::delta::merger::merge(const wallets_t& c) {
    for (auto& i: c) {
        if (unlikely(i.first.is_zero())) {
            auto r = "KO 10231 Invalid address.";
            log(r);
            continue;
        }
        auto n = find(i.first);
        if (n == end()) {
            n = emplace(i.first, us::gov::engine::majority_merger<account_t>()).first;
        }
        n->second.merge(i.second);
    }
}

uint64_t c::delta::merge(engine::app::local_delta* other0) {
    traders::local_delta* other = static_cast<traders::local_delta*>(other0);
    online_merger.merge(other->online);
    b::merge(other0);
    return 0;
}

void c::delta::end_merge() {
    online_merger.end_merge(online);
}

void c::delta::merger::end_merge(wallets_t& c) {
    for (auto& i: *this) {
        if (unlikely(i.first.is_zero())) {
            log("KO 44033 Invalid address.");
            continue;
        }
        account_t consensus_value;
        i.second.end_merge(consensus_value);
        c.emplace(i.first, consensus_value);
    }
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "wallets online:\n";
    online.dump(prefix + "    ", os);
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(online);
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(online);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(online);
        if (is_ko(r)) return r;
    }
    return ok;
}

