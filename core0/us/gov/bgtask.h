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
#include <thread>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <us/gov/config.h>
#include "types.h"

namespace us::gov {

    using namespace std;

    struct start_stop_daemon {

        virtual ~start_stop_daemon() {};

        virtual ko start() = 0;
        virtual ko wait_ready(const time_point& deadline) const = 0;
        virtual ko wait_ready(int seconds_deadline) const = 0;
        virtual void stop() = 0;
        virtual void join() = 0;
        virtual void task_wakeup() = 0;
        virtual bool isup() const = 0;
        virtual bool isdown() const = 0;
        virtual bool resumed() const = 0;
        virtual void reset() = 0;
        virtual bool reset_wait() = 0;
        virtual bool reset_resume() = 0;
        virtual bool is_active() const = 0;
    };

    struct bgtask {

        bgtask();
        bgtask(function<void()> run, function<void()> onwakeup);
        virtual ~bgtask();

        static const char* KO_81021;

        enum state_t: short {
            state_idle = 0,
            state_pending_work = 1,
            state_exit = 2,
        };

        void set_run_function(function<void()> r);
        ko start();
        ko wait_ready(const time_point& deadline) const;
        ko wait_ready(int seconds_deadline) const;

        void stop();
        virtual void on_stop() {}
        void join();
        void task_wakeup();
        bool isup() const { return resume.load() != state_exit; }
        bool isdown() const { return resume.load() == state_exit; }
        bool resumed() const { return resume.load() > state_idle; }
        void run_();
        inline void reset() { resume.store(state_idle); }
        bool reset_wait();
        bool reset_resume(); //true: changed behavior of the next wait -> it won't wait; false: didn't change hehaviour of next wait, it could either wait or not
        inline bool is_active() const { return active; }

    public:
        atomic<state_t> resume{state_idle};
        thread* th{nullptr};
        bool active{false};

        function<void ()> run;
        function<void ()> onwakeup;

        mutable mutex mx_ready;
        bool ready{false};
        mutable condition_variable cv_ready;
    };

    struct wbgtask: bgtask {
        using b = bgtask;
        wbgtask(function<void ()> onwakeup);
        virtual ~wbgtask();

        virtual void collect() = 0;
        virtual void task_init() {};
        virtual void task() = 0;
        virtual void task_cleanup() {};

        private:
        void collect_();
        void run();
    };

}

