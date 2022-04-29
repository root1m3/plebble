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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <csignal>
#include <cassert>
#include <memory>
#include <tuple>
#include <atomic>
#include <iostream>
#include <condition_variable>
#include <us/gov/config.h>
#include <us/gov/crypto/symmetric_encryption.h>
#include "types.h"

namespace us::gov::socket {

    struct daemon0_t;
    struct datagram;
    struct busyled_t;

    struct client {

        using time_point = chrono::system_clock::time_point;
        static const char *KO_7190, *KO_27190, *KO_27191;
        static const char *KO_58961, *KO_60541, *KO_10100;
        static const char *KO_10210;
        static struct timeval timeout;

        enum security_level_t {
            security_level_ok,
            security_level_disconnect,
            security_level_ban,
        };

    public:
        client(daemon0_t&);
        client(daemon0_t&, int sock);
        virtual ~client();

    public:
        void disconnect(seq_t, const reason_t&);
        virtual void disconnectx(channel_t, seq_t, const reason_t&);
        void register_reason(channel_t, seq_t, const reason_t&);
        virtual void on_I_disconnected(const reason_t&) {}

    public:
        virtual pair<ko, datagram*> encrypt0(datagram*) const;
        virtual pair<ko, datagram*> decrypt0(datagram*) const;

    public:
        void set_finish();
        virtual void set_finished();
        inline bool is_finished() const { return finished.load(); }
        void set_finishing_reason(channel_t peer_channel, seq_t, const reason_t&);

    public:
        static bool is_valid_ip(host_t, channel_t);
        static bool is_wan_ip(host_t);
        static shost_t ip4_decode(host_t);
        static host_t ip4_encode(const shost_t&);
        static hostport_t ip4_encode(const shostport_t&);
        static string endpoint(host_t, port_t);
        static string endpoint(const hostport_t&);
        static pair<ko, shostport_t> parse_endpoint(const string& ipport);
        string endpoint() const; //IP:PORT
        hostport_t raddress() const;

    public:
        static string age(ts_ms_t);
        static string age(const time_point&);

    public:
        string refdata() const;
        void dump(const string& prefix, ostream&) const;
        virtual void dump_all(const string& prefix, ostream&) const;

    public:
        virtual pair<security_level_t, string> security_level() const { return make_pair(security_level_ok, ""); }

    public:
        void disable_recv_timeout();
        ko init_sock2(const hostport_t&, bool block);
        ko connect0(const hostport_t&, bool block);
        ko connect0(const shostport_t&, bool block);
        void on_connect(ko);
        inline bool connected() const { return sock != -1 && !finished.load(); }

    public:
        pair<ko, datagram*> recv4x(datagram*, busyled_t&); //used by manager clients
        pair<ko, datagram*> recv6(busyled_t&); //used by auto responder

    public:
        int sock;
        hostport_t hostport;

        time_point since;
        atomic<ts_t> activity;

        daemon0_t& daemon;

    public:
        atomic<bool> finished{false};
        tuple<channel_t, seq_t, reason_t> finished_reason;

    public:
        atomic<int> sendref{0};
        atomic<int> busyrecv{0};

    public:
        time_point activity_recv;
        datagram* curd{nullptr};
        mutable mutex mx_curd;

    #if CFG_COUNTERS == 1
    public:
        struct counters_t {
            uint32_t connection_refused{0};
            uint32_t busy_outbound_pipe{0};
            uint32_t attempt_read_fragment{0};
            uint32_t disconnection_wrong_read{0};
            uint32_t unknown_hostname{0};
            uint32_t zero_read{0};
            void dump(ostream&) const;
        };

        static counters_t counters;
    #endif

    #if CFG_LOGS == 1
    public:
        string logdir;
    #endif
    };

}

