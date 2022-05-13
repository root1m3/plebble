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
#include <set>
#include <mutex>
#include <chrono>
#include <atomic>
#include <condition_variable>

#include <us/gov/ko.h>

#include "evidence.h"

namespace us::gov::engine {

    struct app;

    struct calndx final {
        inline calndx(evidence* e): e(e) {}
        inline bool operator < (const calndx& other) const { 
            if (likely(e->ts != other.e->ts)) {
                return e->ts < other.e->ts;
            }        
            return e->uniq() < other.e->uniq();
        }
        evidence* e;
    };

    struct calendar_t: set<calndx> {

        static constexpr chrono::seconds cycle_period{60};
        static constexpr chrono::seconds relay_interval{15}; //Tx execution is delayed this many seconds to account for propagation.

        static const char* KO_50450, *WP_50451;

        calendar_t();
        ~calendar_t();
        calendar_t(const calendar_t&) = delete;
        calendar_t& operator = (const calendar_t&) = delete;

    public:
        void clear_until(uint64_t);
        ko scheduleX(evidence*); //0-accepted; 1-invalid-expired; 2-invalid-late; 3-dup
        bool has(uint64_t ts) const;
        mutable mutex mx;
        uint64_t block_opening;
        uint64_t block_closure;
        uint64_t last_processed;
    };

}

