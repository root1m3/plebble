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
#include "pools_t.h"
#include "diff.h"
#include "local_deltas_t.h"
#include "types.h"

#define loglevel "gov/engine"
#define logclass "pools_t"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::pools_t;

c::pools_t::~pools_t() {
    for (auto& i: *this) delete i.second;
}

bool c::pools_t::add(local_deltas_t* ld, bool create_diff) { //local_deltas is managed regardles return value
    assert(ld != nullptr);
    log("looking for diff id", ld->id);
    lock_guard<mutex> lock(mx);
    auto i = find(ld->id);
    if (i == end()) {
        log("diff not found", ld->id);
        if (!create_diff) {
            log("diff not created for", ld->id, "ditching local_deltas");
            delete ld;
            return false;
        }
        i = emplace(ld->id, new diff()).first;
        log("new diff created for", ld->id);
    }
    return i->second->add(ld);
}

diff* c::pools_t::close(ts_t id) {
    assert(id != 0);
    auto i = find(id);
    if (i == end()) {
        return nullptr;
    }
    auto r = i->second;
    erase(i);
    r->close();
    return r;
}

