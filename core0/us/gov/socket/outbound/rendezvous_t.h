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
#include <condition_variable>
#include <mutex>
#include <unordered_map>

#include <us/gov/ko.h>
#include <us/gov/socket/types.h>

namespace us { namespace gov { namespace socket {

    using namespace us;
    struct datagram;
    struct client;
    struct caller_daemon_t;

    struct rendezvous_t final: unordered_map<seq_t, datagram*> {

        static const char* KO_3029, *KO_20190;

        rendezvous_t(caller_daemon_t&);
        ~rendezvous_t();

        bool arrived(datagram *);
        void flush();
        pair<ko, datagram*> sendrecv(client&, datagram*, string& remote_error);

    public:
        caller_daemon_t& caller_daemon;
        seq_t next_seq{0};
        mutex mx;
        condition_variable cv;
        bool finished{false};
    };

}}}

