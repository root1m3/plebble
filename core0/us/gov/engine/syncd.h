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
#pragma once
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <atomic>

#include <us/gov/config.h>
#include <us/gov/bgtask.h>

#include "diff.h"

namespace us::gov::engine {

    using namespace std;

    struct syncd_t final_: bgtask {
        using t = bgtask;

        struct actuator {
            virtual ~actuator() {}
            virtual bool get_prev_block(const hash_t& h, hash_t& prev) const = 0;
            virtual int call_query_block(const hash_t& hash) = 0;
            virtual void clear() = 0;
            virtual bool patch_db(vector<hash_t>& patches) = 0;
            virtual void on_sync() = 0;
        };

        syncd_t(actuator* d);
        ~syncd_t() override;
        syncd_t(const syncd_t&)=delete;
        syncd_t& operator = (const syncd_t&)=delete;

        void dump(ostream&) const;
        void run();
        void update(const hash_t& head, const hash_t& tail);
        void update(const hash_t& tail);
        void wait();
        void onwakeup();
        void wait(const chrono::steady_clock::duration&);
        const hash_t& tip() const;
        bool in_sync() const;
        bool in_sync_() const;
        void signal_file_arrived();
        void logline(ostream&os) const;
        actuator* d;
        condition_variable cv;
        mutable mutex mx;

        hash_t head;
        hash_t cur;
        hash_t tail;
        condition_variable cv_wait4file;
        mutex mx_wait4file;
        atomic<bool> file_arrived{false};
        hash_t last_snapshot{0};

        #if CFG_COUNTERS == 1
            struct counters_t {
                uint32_t trigger_sync_event{0};
                uint32_t db_reset_countdowns{0};
                uint32_t db_reset{0};
                void dump(ostream&os) const;
            };
            static counters_t counters;
        #endif

        #if CFG_LOGS == 1
            string logdir;
        #endif
    };

}

