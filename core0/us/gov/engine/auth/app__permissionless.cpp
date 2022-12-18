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
#include <us/gov/config.h>
#ifdef CFG_PERMISSIONLESS_NETWORK

#include <us/gov/engine/daemon_t.h>
#include <us/gov/engine/db_t.h>

#include "app.h"

#define loglevel "gov/engine/auth"
#define logclass "app__permissionless"
#include <us/gov/engine/logs.inc>

using namespace us::gov::engine::auth;
using c = us::gov::engine::auth::app;

void c::layoff(nodes_t& n, uint16_t cut) {
    log("layoff", cut);
    for (auto i = n.begin(); i != n.end(); ) {
        if (i->second.seen <= cut) {
            log("laid off ", i->first, i->second.seen, "<=", cut);
            i = n.erase(i);
        }
        else {
            ++i;
        }
    }
}

void c::layoff() {
    using namespace chrono;
    auto block_ts = demon.db->last_delta_imported_id;
    if (block_ts == 0) {
        block_ts = duration_cast<nanoseconds>(duration_cast<minutes>(system_clock::now().time_since_epoch())).count();
    }
    uint16_t cut = duration_cast<hours>(chrono::nanoseconds(block_ts)).count() / 24 - layoff_days;
    log("cut", cut);
    {
        log("lay off hall");
        lock_guard<mutex> lock(db.mx_hall);
        layoff(db.hall, cut);
    }
    {
        log("lay off nodes");
        lock_guard<mutex> lock(db.mx_nodes);
        layoff(db.nodes, cut);
    }
}

void c::add_growth_transactions(unsigned int seed) {
    log("add growth tx. seed", seed, "growth", growth, "min_growth", min_growth);
    if (abs(growth) < 1e-8) {
        log("growth is nearly 0", growth);
        return;
    }
    default_random_engine generator(seed);
    nodes_t* src;
    nodes_t* dst;
    size_t maxr;
    int s;
    if (growth >= 0) {
        size_t nh;
        {
            lock_guard<mutex> lock(db.mx_hall);
            nh = db.hall.size();
            if (nh == 0) {
                log("empty hall");
                return;
            }
        }
        s = floor((double) nh * growth);
        src = &db.hall;
        dst = &db.nodes;
        if (s < min_growth) s = min_growth;
        if (s > nh) s = nh;
        maxr = nh - 1;
        log("src=hall", "dst=nodes", "nh=", nh, "s=", s, "maxr=", maxr);
        lock_guard<mutex> lock(db.mx_nodes);
        if (db.nodes.size() >= CFG_MAX_NODES) {
            log("Reached limit of", db.nodes.size(), "/", CFG_MAX_NODES, "nodes. Keeping", nh, "nodes in hall.");
            return;
        }
    }
    else {
        size_t nn;
        {
            lock_guard<mutex> lock(db.mx_nodes);
            nn = db.nodes.size();
            if (nn == 0) {
                return;
            }
        }
        s = -floor((double) nn * growth);
        src = &db.nodes;
        dst = &db.hall;
        maxr = nn - 1;
        log("src=nodes", "dst=hall", "nn=", nn, "s=", s, "maxr=", maxr);
    }
    uniform_int_distribution<size_t> distribution(0, maxr);
    lock(lock_nodes, lock_hall);
    logdump("GT before src>", *src);
    logdump("GT before dst>", *dst);
    for (size_t i = 0; i < s; ++i) {
        log("i", i, "src sz", src->size());
        auto p = src->begin();
        size_t r;
        while (true) {
            r = distribution(generator);
            if (r >= src->size()) continue;
            break;
        }
        advance(p, r);
        log("moving ", p->first);
        dst->emplace(*p);
        src->erase(p);
    }
    logdump("GT after src>", *src);
    logdump("GT after dst>", *dst);
    lock_nodes.unlock();
    lock_hall.unlock();        
}

void c::shard() {
    if (db.nodes.size() < cfg_shard_size) return;

    int highestbit = 0;
    {
        auto v = maskcoord.mask;
        while (true) {
            if (v == 0) break;
            v >>= 1;
            ++highestbit;
        }
    }
    using mask_t = maskcoord_t::mask_t;
    mask_t bit = (1 << highestbit);
    auto newm = maskcoord.mask | bit;
    maskcoord.mask = newm;
    if ((demon.id.pub.hash().lsdw() & bit) != 0) {
        maskcoord.coord |= bit;
    }
    db.filter(maskcoord);
}

#endif

