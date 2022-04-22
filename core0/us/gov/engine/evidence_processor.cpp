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
#include "evidence_processor.h"
#include <us/gov/peer/types.h>

#define loglevel "engine"
#define logclass "evidence_processor"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::evidence_processor;

c::evidence_processor(): bgtask(bind(&c::run, this), bind(&c::onwakeup, this)) {
}

c::~evidence_processor() {
}

void c::onwakeup() {
    cv.notify_all();
}

void c::process_evidences(uint64_t maxts) {
    log("process_evidences maxts", maxts);
    evidence* e;
    while(true) {
        {
            lock_guard<mutex> lock(calendar.mx);
            auto i = calendar.begin();
            if (i == calendar.end()) break;
            if (i->e->ts >= maxts) break;
            e = i->e;
            calendar.erase(i);
            calendar.last_processed = e->ts;
        }
        exec_evidence(*e);
        delete e;
    }
}

void c::wait() {
    if (t::reset_wait()) {
        return;
    }
    {
        using namespace chrono;
        unique_lock<mutex> lock(calendar.mx);
        cv.wait_for(lock, 1s, [&] {
                ts_t front = duration_cast<nanoseconds>((system_clock::now() - calendar.relay_interval).time_since_epoch()).count();
                auto i = calendar.begin();
                bool b = i != calendar.end() && i->e->ts <= front;
                return front >= calendar.block_closure || b || isdown();
            });
    }
    t::reset_wait();
}

void c::wait_new_cycle() {
    auto prev = calendar.block_closure;
    mutex mxw;
    unique_lock<mutex> lock(mxw);
    cv.wait(lock, [&] {
            return prev != calendar.block_closure || t::isdown();
        });
}

void c::run() {
    #if CFG_LOGS == 1
        log_start(logdir, "evidence_processor");
    #endif
    using namespace chrono;
    while(t::isup()) {
        ts_t front = duration_cast<nanoseconds>((system_clock::now() - calendar_t::relay_interval).time_since_epoch()).count();
        log("loop", front, calendar.block_closure);
        if (front >= calendar.block_closure) {
            log("time hit or crossed the block closure mark", front, ">=", calendar.block_closure);
            process_evidences(calendar.block_closure);
            on_block_closure(calendar.block_closure);
            calendar.last_processed = calendar.block_closure - 1;
            auto d = duration_cast<nanoseconds>(calendar_t::cycle_period).count();
            calendar.block_opening += d;
            calendar.block_closure += d;
            log("trigger consensus", "next calendar.block_closure", calendar.block_closure);
            cv.notify_all();
        }
        else {
            if (ready_for_block_opening(calendar.block_opening)) {
                calendar.clear_until(calendar.block_opening);
                process_evidences(front);
            }
            else {
                log("keeping evidences on hold");
                this_thread::sleep_for(1s);
            }
        }
        wait();
    }
}

