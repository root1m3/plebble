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
#include "calendar.h"
#include "evidence.h"
#include "app.h"

#define loglevel "engine"
#define logclass "calendar"
#include "logs.inc"

using namespace std;
using namespace chrono;
using namespace chrono_literals;
using namespace us::gov::engine;
using c = us::gov::engine::calendar_t;
using us::ko;

constexpr seconds c::calendar_t::relay_interval;
constexpr seconds c::calendar_t::cycle_period;

const char* c::KO_50450 = "KO 50450 Invalid evidence. Late.";
const char* c::WP_50451 = "WP 50450 Duplicate evidence.";

c::calendar_t() {
     auto d = duration_cast<nanoseconds>(duration_cast<minutes>(system_clock::now().time_since_epoch()));
     last_processed = d.count() - 1;
     block_opening = d.count();
     block_closure = (d + duration_cast<nanoseconds>(cycle_period)).count();
}

c::~calendar_t() {
    for (auto& i: *this) delete i.e;
}

bool c::has(ts_t ts) const {
    lock_guard<mutex> lock(mx);
    if (empty()) return false;
    const_iterator it;
    typename iterator_traits<const_iterator>::difference_type count, step;
    count = size();
    const_iterator first = begin();
    while (count > 0) {
        it = first;
        step = count / 2;
        advance(it, step);
        if (it->e->ts < ts) {
            first = ++it;
            count -= step + 1;
        }
        else
            count = step;
    }
    return first->e->ts == ts;
}

ko c::scheduleX(evidence* e) {
    log("schedule", e->ts);
    assert(e != nullptr);
    ts_t front = duration_cast<nanoseconds>((system_clock::now() - relay_interval).time_since_epoch()).count();
    log("arrived evidence. margin", e->ts-front);
    lock_guard<mutex> lock(mx);
    if (e->ts <= last_processed) {
        log(KO_50450, "ts", e->ts, "<=last_processed", last_processed);
        return KO_50450;
    }
    if (!emplace(calndx(e)).second) { //duplicate
        log(WP_50451, e->ts);
        return WP_50451;
    }
    log("ev successfully entered the calendar", e->ts);
    return ok;
}

void c::clear_until(ts_t t) {
    log("clear_until", t);
    lock_guard<mutex> lock(mx);
    while(true) {
        auto i = begin();
        if (i == end()) break;
        if (i->e->ts >= t) {
            break;
        }
        log("ditched evidence with ts", i->e->ts);
        delete i->e;
        erase(i);
    }
}

calndx::calndx(evidence*e): e(e) {
}

bool calndx::operator < (const calndx& other) const {
    return e->ts < other.e->ts;
}

