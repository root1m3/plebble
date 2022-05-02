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
#include <functional>
#include <mutex>
#include <string>

#include <us/gov/ko.h>
#include <us/gov/bgtask.h>
#include <us/gov/crypto/types.h>
#include <us/gov/socket/types.h>
#include <us/gov/relay/daemon_t.h>

#include "types.h"

namespace us { namespace gov { namespace relay {

    struct peer_t;

    struct pushman: bgtask {
        using t = bgtask;
        using filter_t = daemon_t::filter_t;
        using push_item = daemon_t::push_item;

        enum push_code_t { //communications
            push_begin = 0,
            push_ko = push_begin, //0
            push_ok, //1

            push_end
        };

        pushman(daemon_t&);
        ~pushman();

        void run();
        void onwakeup();
        void push_to_devices();
        void wait();
        void flush_push();
        void schedule_push(socket::datagram*, const filter_t&);
        ko push_KO(ko msg, const filter_t&);
        ko push_KO(const hash_t& tid, ko msg, const filter_t&);
        ko push_OK(const string& msg, const filter_t&);
        ko push_OK(const hash_t& tid, const string& msg, const filter_t&);

    public:
        vector<push_item>* vpush;
        mutex mx;
        condition_variable cv;
        daemon_t& daemon;
        #if CFG_LOGS == 1
            string logdir;
            string logfile;
        #endif
    };

}}}

