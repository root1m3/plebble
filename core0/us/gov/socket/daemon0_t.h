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
#include "peer_t.h"
#include "datagram.h"
#include "types.h"
#include "outbound/caller_daemon_t.h"

namespace us::gov::socket {

    struct daemon0_t: caller_daemon_t {
        using b = caller_daemon_t;

        using dispatcher_t = datagram::dispatcher_t;

        daemon0_t(channel_t);
        ~daemon0_t() override;

        virtual bool process_unhandled(client&, datagram*);
        void process_ko_work(peer_t&, channel_t, seq_t, ko);
        void process_ko_work(peer_t&, seq_t, ko);
        void process_ok_work(peer_t&, datagram*);
        inline bool outbound_traffic__goes_encrypted() const { return b::encrypt_traffic; }
        virtual void on_destroy_(client&) {}
        virtual void attach(client*) = 0;
        virtual void detach(client&) = 0;
        virtual busyled_t& recv_led() = 0;
        virtual string rewrite(ko) const;
    public:
        virtual void on_connect(client&, ko err) {}
        inline pair<ko, datagram*> sendrecv(client& peer, datagram* d, string& remote_error) { return rendezvous.sendrecv(peer, d, remote_error); }

    public:
        void dump(const string& prefix, ostream&) const;
        virtual void dump_all(const string& prefix, ostream&) const;

        channel_t channel;
    };

}

