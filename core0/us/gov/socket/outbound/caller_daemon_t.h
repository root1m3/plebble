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
#include "send_queue.h"
#include "rendezvous_t.h"
#include <chrono>

namespace us { namespace gov { namespace socket {

    struct datagram;
    struct client;

    struct caller_daemon_t: send_queue_t {
        using b = send_queue_t;

        caller_daemon_t();
        ko start();
        void stop();
        ko send1(client&, datagram*);
        void dump(const string& prefix, ostream&) const;

    public:
        rendezvous_t rendezvous;
        bool encrypt_traffic{true};
    };

}}}

