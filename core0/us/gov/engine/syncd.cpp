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

#include "daemon_t.h"
#include "syncd.h"

#define loglevel "gov/engine"
#define logclass "syncd"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::syncd_t;

#if CFG_COUNTERS == 1
    c::counters_t c::counters;
#endif

c::syncd_t(actuator* d): d(d), head(0), cur(0), tail(0), t(bind(&c::run, this), bind(&c::onwakeup, this)) {
}

c::~syncd_t() {
    join();
}

const hash_t& c::tip() const {
    lock_guard<mutex> lock(mx);
    return head;
}

void c::onwakeup() {
   cv.notify_all();
}

void c::run() {
    #if CFG_LOGS == 1
        log_start(logdir, "syncd");
    #endif
    static constexpr int reset_countdown = 100;
    int query_reps{reset_countdown};
    hash_t last_queryed;
    while(isup()) {
        hash_t he(0), ta(0), cu(0);
        vector<hash_t> patches;
        while(true) {
            {
                lock_guard<mutex> lock(mx);
                if (cur == tail) break;
                if (head.is_zero()) break;
                if (cur.is_zero()) break;
                he = head;
                ta = tail;
                cu = cur;
            }
            if (cu == he) {
                patches.clear();
            }
            hash_t prev;
            if (d->get_prev_block(cu, prev)) {
                patches.push_back(cu);
                lock_guard<mutex> lock(mx);
                cur = prev;
            }
            else {
                log("query block", cu);
                if (likely(d->call_query_block(cu) > 0)) {
                    if (last_queryed != cu) {
                        last_queryed = cu;
                        query_reps = 20;
                    }
                    else {
                        #if CFG_COUNTERS == 1
                            ++counters.db_reset_countdowns;
                        #endif
                        if (--query_reps == 0) {
                            {
                                lock_guard<mutex> lock(mx);
                                head = cur = tail = 0;
                            }
                            #if CFG_COUNTERS == 1
                                ++counters.db_reset;
                            #endif
                            ostringstream lfn;
                            lfn << LOGDIR << "/reset_snapshot";
                            ofstream os(lfn.str());
                            os << "using Snapshot " << last_snapshot << '\n';
                            d->clear();
                            update(last_snapshot, hash_t(0));
                        }
                    }
                    log("wait4file");
                    if (!file_arrived.load()) {
                        mutex mxw;
                        unique_lock<mutex> lock(mxw);
                        cv.wait_for(lock, 3s, [&]{ return file_arrived.load() || resumed(); });
                        reset_wait();
                        file_arrived.store(false);
                    }
                }
                else {
                    log("sleep3s");
                    mutex mxw;
                    unique_lock<mutex> lock(mxw);
                    cv.wait_for(lock, 3s, [&]{ return resumed(); });
                    reset_wait();
                }
            }
            if (unlikely(isdown())) return;
        }
        if (!patches.empty()) {
            log("patch_db", patches.size());
            if (d->patch_db(patches)) {
                {
                    lock_guard<mutex> lock(mx);
                    cur = head = tail = *patches.begin();
                }
                #if CFG_COUNTERS == 1
                    ++counters.trigger_sync_event;
                #endif
                log("on_sync");
                d->on_sync();
            }
            else {
                log("clear");
                ostringstream lfn;
                lfn << LOGDIR << "/reset_snapshot";
                ofstream os(lfn.str());
                os << "using snapshot " << last_snapshot << endl;
                d->clear();
                update(last_snapshot, hash_t(0));
                reset_resume();
                //resume.store(1);
            }
        }
        bool need_fix;
        {
            lock_guard<mutex> lock(mx);
            need_fix = ((head != tail) && (tail.is_not_zero()));
        }
        if (need_fix) {
            log("wait3");
            wait(3s);
        }
        else {
            log("wait");
            wait();
        }
    }
    log("end");
}

void c::signal_file_arrived() {
   file_arrived.store(true);
   task_wakeup();
}

void c::update(const hash_t& h, const hash_t& t) {
    {
        lock_guard<mutex> lock(mx);
        cur = head = h;
        tail = t;
    }
    reset_resume();
    cv.notify_all();
}

void c::update(const hash_t& t) {
    {
        lock_guard<mutex> lock(mx);
        tail = t;
        cur = head;
    }
    reset_resume();
    cv.notify_all();
}

void c::wait() {
    if (reset_wait()) {
        return;
    }
    mutex mxw;
    unique_lock<mutex> lock(mxw);
    cv.wait(lock, [&]{ return resumed(); });
    reset_wait();
}

void c::wait(const chrono::steady_clock::duration& d) {
    if (reset_wait()) {
        return;
    }
    mutex mxw;
    unique_lock<mutex> lock(mxw);
    cv.wait_for(lock, d, [&]{ return resumed(); });
    reset_wait();
}

bool c::in_sync() const {
    lock_guard<mutex> lock(mx);
    return in_sync_();
}

bool c::in_sync_() const {
    return head == tail && head.is_not_zero();
}

void c::logline(ostream& os) const {
    lock_guard<mutex> lock(mx);
    os << "{S " << (in_sync_() ? '1' : '0') << ' ';
    os << "H " << head << ' ';
    if (cur != head) os << "C " << cur << ' ';
    if (tail != cur) os << "T " << tail << ' ';
    os << "LS " << last_snapshot << "} ";
}

void c::dump(ostream& os) const {
    os << "Greetings from syncd\n";
    lock_guard<mutex> lock(mx);
    os << "in sync: " << boolalpha << in_sync_() << '\n';
    os << "head: " << head << '\n';
    os << "cur: " << cur << '\n';
    os << "tail: " << tail << '\n';
    os << "last snapshot  " << last_snapshot << '\n';
}

#if CFG_COUNTERS == 1
    void c::counters_t::dump(ostream& os) const {
        os << "trigger_sync_event " << trigger_sync_event << '\n';
        os << "db_reset_countdowns " << db_reset_countdowns << '\n';
        os << "db_reset " << db_reset << '\n';
    }
#endif

