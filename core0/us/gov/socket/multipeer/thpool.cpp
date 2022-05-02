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
#include "thpool.h"
#include <chrono>
#include <us/gov/likely.h>
#include <us/gov/socket/peer_t.h>

#define loglevel "gov/socket/multipeer"
#define logclass "thpool"
#include <us/gov/socket/logs.inc>

using namespace us::gov::socket;
using c = us::gov::socket::multipeer::thpool;

c::thpool() {
    log("thread pool constructor");
}

c::~thpool() {
    log("thread pool destructor");
    join();
}

us::ko c::start(size_t sz) {
    ko r = ok;
    lock_guard<mutex> lock(mx);
    resume.store(0);
    assert(empty());
    log("creating workers", sz);
    resize(sz);
    for (auto& i: *this) {
        i = new thread(&thpool::run, this);
    }
    return ok;
}

us::ko c::wait_ready(const time_point& deadline) const {
    return ok;
}

void c::stop() {
    if (empty()) return;
    resume.store(2);
    cv.notify_all();
}

void c::join() {
    lock_guard<mutex> lock(mx);
    for (auto& i: *this) {
        i->join();
        delete i;
    }
    clear();
}

void c::run() {
    #if CFG_LOGS == 1
        log_start(logdir, "worker");
    #endif
    while (resume.load() != 2) {
        peer_t* peer;
        {
            unique_lock<mutex> lock(mx_q);
            if (q.empty()) {
                log("empty queue");
                cv.wait(lock,[&](){ return !q.empty() || resume.load() > 0; }); //WP 4033
                if (resume.load() == 1) resume.store(0);
                if (unlikely(resume.load() == 2)) {
                    log("exit worker dispatch loop");
                    break;
                }
            }
            peer = *q.begin();
            q.erase(q.begin());
            log("attending", peer->refdata(), "remaining queue sz=", q.size());
        }
        assert(peer != nullptr);
        log("-begin job ---------------", peer->refdata());
        assert(peer->busyrecv.load() > 0);
        peer->process_work();
        log("-end job------------------", peer->refdata());
        #if CFG_COUNTERS == 1
            ++counters.jobs_dispatched;
        #endif
    }
    log("worker ended");
}

void c::process(peer_t* peer) {
    log("add client to work processing queue");
    {
        lock_guard<mutex> lock(mx_q);
        if (q.find(peer) != q.end()) {
            log("client already in processing queue");
            cv.notify_one();
            return;
        }
        assert(peer->busyrecv.load() == 0);
        peer->prepare_worker_recv();
        log("worker locked. recv work queued");
        assert(peer->busyrecv.load() == 1);
        log("pool", peer->refdata());
        q.emplace(peer);
        log("work queue sz=", q.size());
    }

    #if CFG_COUNTERS == 1
        ++counters.jobs_queued;
    #endif
    log("signal one thread to handle the job");
    cv.notify_one(); //Flow continues at WP 4033
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "thread pool size: " << size() << '\n';
    os << prefix;
    watch(os);
    os << '\n';
}

void c::watch(ostream& os) const {
    #if CFG_COUNTERS == 1
        os << "jobs queued/wait/dispatched " << counters.jobs_queued << "/";
        {
            lock_guard<mutex> lock(mx_q);
            os << q.size();
        }
        os << "/" << counters.jobs_dispatched;
    #else
        os << "jobs wait ";
        {
            lock_guard<mutex> lock(mx_q);
            os << q.size();
        }
    #endif
}

