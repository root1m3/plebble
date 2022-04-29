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
#include <utility>
#include <us/gov/ko.h>
#include <us/gov/bgtask.h>

#include "daemon0_t.h"
#include "datagram.h"
#include "types.h"
#include "single/handler_daemon_t.h"

namespace us::gov::socket {

    struct client;
    struct peer_t;

    struct rpc_daemon_t: daemon0_t, single::handler_daemon_t {
        using b = daemon0_t;
        using caller = b;
        using handler = single::handler_daemon_t;
        using dispatcher_t = datagram::dispatcher_t;

        rpc_daemon_t(channel_t channel, dispatcher_t*);
        ~rpc_daemon_t() override;

        void set_busy_handlers(busyled_t::handler_t* hsend, busyled_t::handler_t* hrecv);
        virtual client* create_client();
        bool process_unhandled(client&, datagram*) final override;
        inline busyled_t& recv_led() final override { return handler::busyled; }
        inline void wakeup_handler() { handler::task_wakeup(); }
        inline void wakeup_caller() { caller::task_wakeup(); }
        virtual void on_destroy_(client&) {}
        virtual ko connect() = 0;
        void attach(client*) final override;
        void detach(client&) final override;
        ko start();
        ko wait_ready(const time_point& deadline) const;
        ko wait_ready(int secs) const;
        void stop();
        void join();
        bool sendq_active() const;
        void on_stop() override;
        string rewrite(ko) const override;
        inline bool isup() const { return handler::isup() && caller::isup(); }
        inline bool isdown() const { return handler::isdown() || caller::isdown(); }
        inline bool is_active() const { return handler::is_active() && caller::is_active(); }
        ko send1(client&, datagram*);

    public:
        void dump(const string& prefix, ostream&) const;
        virtual void dump_all(const string& prefix, ostream&) const;
        ko wait_recv_connection(bool initiate);

    public:
        peer_t* peer{nullptr};

        #if CFG_LOGS == 1
            string logdir;
        #endif
    };

}

