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
#pragma once
#include <vector>
#include <unordered_set>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include <us/gov/config.h>

namespace us { namespace gov { namespace socket {

    struct peer_t;

}}}

namespace us { namespace gov { namespace socket {  namespace multipeer {

    struct thpool final: vector<thread*> {

        using time_point = chrono::system_clock::time_point;

        thpool();
        ~thpool();

        ko start(size_t sz);
        ko wait_ready(const time_point& deadline) const;
        void stop();
        void join();
        void process(peer_t*);
        void run();
        void watch(ostream&) const;
        void dump(const string& prefix, ostream&) const;

    public:
        unordered_set<peer_t*> q;
        condition_variable cv;
        mutable mutex mx_q;
        atomic<short> resume{0};
        mutex mx;
        #if CFG_COUNTERS == 1
            struct counters_t {
                uint32_t jobs_queued{0};
                atomic<uint32_t> jobs_dispatched{0};
                void dump(ostream&) const;
            };
            counters_t counters;
        #endif
        #if CFG_LOGS == 1
            string logdir;
        #endif
    };

}}}}

