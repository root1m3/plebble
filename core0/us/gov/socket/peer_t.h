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
#include <chrono>
#include "client.h"
#include "api.h"

namespace us { namespace gov { namespace socket {

    struct datagram;
    struct daemon_t;
    struct rpc_daemon_t;

    struct peer_t: client, api {
        using b = client;

        using time_point = chrono::system_clock::time_point;
        static const char* KO_50143, *KO_20179;

        static const char* finished_reason_1;
        static const char* finished_reason_2;
        constexpr static chrono::seconds stall{5s};

        peer_t(rpc_daemon_t&);
        peer_t(daemon_t&, sock_t sock);
        ~peer_t() override;

        void ping();
        pair<ko, datagram*> sendrecv(datagram*, string& remote_error);
        virtual bool process_work(datagram*);
        void process_ko_work(channel_t, seq_t, ko);
        void process_ok_work(datagram*);
        ko send1(datagram*);

    public:
        void dump(const string& prefix, ostream&) const;
        void dump_all(const string& prefix, ostream&) const override;

    public: //multipeer
        void set_finished() final override;

        pair<security_level_t, string> security_level() const override;
        void prepare_worker_recv();
        void worker_ended_recv();
        bool stalled_datagram() const;

        virtual void process_work(); ///Entry point of a worker thread
        virtual void on_peer_disconnected(const reason_t&);

    public: //API
        thread_local static string lasterror;
        #ifdef has_us_gov_socket_api
            #include <us/api/generated/c++/gov/socket/hdlr_svc_handler-hdr>
            #include <us/api/generated/c++/gov/socket/cllr_override>
            #include <us/api/generated/c++/gov/socket/hdlr_override>
        #endif
    };

}}}

