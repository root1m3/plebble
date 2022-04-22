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
#include "neuralyzer_t.h"
#include "daemon_t.h"

#define loglevel "gov/engine"
#define logclass "neuralyzer_t"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::neuralyzer_t;

c::neuralyzer_t(daemon_t* d): old(0), d(d) {
}

c::~neuralyzer_t() {
    lock_guard<mutex> lock(mx);
    delete_chain(old);
}

void c::old_chain(const hash_t& h) {
    lock_guard<mutex> lock(mx);
    if (unlikely(h == old)) return;
    delete_chain(old);
    old = h;
}

void c::get_blocks(set<hash_t>& s, const hash_t& h) {
    hash_t cur = h;
    hash_t prev(0);
    lock_guard<mutex> lock(mx);
    while(!cur.is_zero()) {
        s.emplace(cur);
        if (d->get_prev_block(cur, prev)) {
            cur = prev;
            continue;
        }
        if (!prev.is_zero()) {
            s.emplace(prev);
        }
        break;
    }
}

void c::delete_chain(const hash_t& h) {
    hash_t cur = h;
    hash_t prev(0);
    while(!cur.is_zero()) {
        if (d->get_prev_block(cur, prev)) {
            string filename = d->blocksdir + '/' + cur.to_b58();
            ::remove(filename.c_str());
            cur = prev;
            continue;
        }
        if (!prev.is_zero()) {
            string filename = d->blocksdir + '/' + prev.to_b58();
            ::remove(filename.c_str());
        }
        break;
    }
}

