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
#include <queue>
#include <vector>
#include <unordered_map>
#include <map>

#include <us/gov/bgtask.h>
#include <us/gov/config.h>
#include <us/gov/socket/busyled_t.h>

namespace us::gov::socket {

    struct datagram;
    struct client;

    struct qi final: pair<datagram*, client*> {
        using b = pair<datagram*, client*>;
        qi(datagram*, client*);
        qi(const qi&) = delete;
        ~qi();
        short errcount{0};
    };

    struct priority_service {
        bool operator () (qi*& lhs, qi*& rhs) const;
    };

    using pri_q_t = priority_queue<qi*, vector<qi*>, priority_service>;

    struct attic_t final: map<uint64_t, client*> {
        static constexpr uint64_t min_age{200000000}; //200ms

        ~attic_t();
        bool add_(qi*);
        void add_(vector<qi*>&&);
        int64_t rem_time() const;
        unordered_map<client*, vector<qi*>> clindex;
    };

    struct send_queue_t: pri_q_t, bgtask {  //datagrams with lower service code have more priority
        using b = pri_q_t;
        using t = bgtask;

        struct ongoing_t: unordered_map<client*, qi*>  {
            qi* reset(qi*);
            void set(qi*);
        };

        static constexpr size_t wmh{CFG_SENDQ_WMH}; //watermark high
        static constexpr size_t schunk{CFG_SENDQ_SCHUNK};

        send_queue_t();
        ~send_queue_t() override;

        ko send(datagram*, client*, uint16_t pri);

        qi* next();
        qi* next_();
        qi* replace(qi*);
        size_t get_size() const;
        void wait();
        void dump(const string& prefix, ostream&) const;
        void clear();
        void attic_store(client*);
        void attic_retrieve();
        void run_send();
        void onwakeup();
        void set_busy_handler(busyled_t::handler_t* hsend);
        void resume_clients_();
        void hold(client*);
        bool wait_empty(); //3 seconds timeout

        condition_variable cv;
        mutable mutex mx;
        #if CFG_COUNTERS == 1
            uint64_t sent{0};
            uint64_t dropped{0};
            uint64_t bytes_sent{0};
        #endif
        attic_t attic;
        ongoing_t ongoing;
        busyled_t busyled;
        condition_variable cv_empty;
        #if CFG_LOGS == 1
            string logdir;
        #endif
    };

}

