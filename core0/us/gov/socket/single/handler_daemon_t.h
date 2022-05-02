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
#include <us/gov/bgtask.h>
#include <us/gov/socket/types.h>
#include <us/gov/socket/busyled_t.h>
#include <us/gov/socket/datagram.h>

namespace us::gov::socket {

    struct rpc_daemon_t;
    struct peer_t;
    struct client;

}

namespace us::gov::socket::single {

    struct handler_daemon_t: bgtask {
        using b = bgtask;
        using dispatcher_t = datagram::dispatcher_t;

        static const char* KO_40032;

        handler_daemon_t(rpc_daemon_t&, dispatcher_t*);
        inline virtual ~handler_daemon_t() { delete dispatcher; }

        void stop();

        peer_t& get_peer();
        const peer_t& get_peer() const;
        bool has_peer() const;

        bool is_connected() const;

        ko wait_connection(bool initiate);
        ko connect();

        void onwakeup();
        void wait(const time_point& deadline);
        void run_recv();
        ko wait_connected();

        void set_busy_handler(busyled_t::handler_t*);
        virtual bool process_unhandled(client&, datagram*);

    public:
        condition_variable cv_recv;
        bool connect_for_recv{true};
        bool stop_on_disconnection{true};

        time_point last_try_connect;
        int ms{10000}; //wait after failed connection

        condition_variable cv_connected;
        mutable mutex mx_connect;

        busyled_t busyled;
        dispatcher_t* dispatcher;
        rpc_daemon_t& daemon;

        #if CFG_COUNTERS == 1
            struct counters_t {
                uint32_t disconnection_normal{0};
                uint32_t disconnection_unknown_service{0};
                void dump(ostream&) const;
            };
            static counters_t counters;
        #endif

        #if CFG_LOGS == 1
            string logdir;
        #endif
    };

}

