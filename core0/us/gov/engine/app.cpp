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
#include "app.h"
#include <cassert>

#include <us/gov/stacktrace.h>
#include <us/gov/cash/app.h>
#include <us/gov/cash/local_delta.h>
#include <us/gov/cash/delta.h>
#include <us/gov/sys/app.h>
#include <us/gov/sys/local_delta.h>
#include <us/gov/sys/delta.h>
#include <us/gov/traders/app.h>
#include <us/gov/traders/local_delta.h>
#include <us/gov/traders/delta.h>

#include "daemon_t.h"
#include "diff.h"
#include "types.h"
#include "policies.h"
#include "auth/local_delta.h"
#include "auth/delta.h"
#include "auth/app.h"

#define loglevel "gov/engine"
#define logclass "app"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::app;

uint64_t c::delta::merge(local_delta* other) {
    ++multiplicity;
    delete other;
    return 0;
}

c::local_delta* c::local_delta::create(appid_t id) {
    if (id == auth::app::id()) return new auth::local_delta();
    if (id == cash::app::id()) return new cash::local_delta();
    if (id == sys::app::id()) return new sys::local_delta();
    if (id == traders::app::id()) return new traders::local_delta();
    log("KO 11094 Invalid app id", +id);
    //assert(false);
    return nullptr;
}

c::delta* c::delta::create(appid_t id) {
    if (id == auth::app::id()) return new auth::delta();
    if (id == cash::app::id()) return new cash::delta();
    if (id == sys::app::id()) return new sys::delta();
    if (id == traders::app::id()) return new traders::delta();
    log("KO 11095 Invalid app id", +id);
    //assert(false);
    return nullptr;
}

c::delta* c::delta::create(appid_t id, const blob_t& blob) {
    if (id == auth::app::id()) {
        auto o = new auth::delta();
        auto r = o->read(blob);
        if (is_ko(r)) {
            delete o;
            return nullptr;
        }
        return o;
    }
    if (id == cash::app::id()) {
        auto o = new cash::delta();
        auto r = o->read(blob);
        if (is_ko(r)) {
            delete o;
            return nullptr;
        }
        return o;
    }
    if (id == sys::app::id()) {
        auto o = new sys::delta();
        auto r = o->read(blob);
        if (is_ko(r)) {
            delete o;
            return nullptr;
        }
        return o;
    }
    if (id == traders::app::id()) {
        auto o = new traders::delta();
        auto r = o->read(blob);
        if (is_ko(r)) {
            delete o;
            return nullptr;
        }
        return o;
    }
    log("KO 11096 Invalid app id", id);
    return nullptr;
}

hash_t c::local_delta::compute_hash() const {
    blob_t blob;
    write(blob);
    hasher_t hasher;
    hasher.write(blob);
    hash_t v;
    hasher.finalize(v);
    return move(v);
}

ko c::shell_command(istream&, ostream& os) {
    return "KO 80795 No shell available for app.";
}

