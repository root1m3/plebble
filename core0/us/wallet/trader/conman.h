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
#include <atomic>
#include <mutex>
#include <atomic>
#include <queue>
#include <condition_variable>

#include <us/gov/bgtask.h>
#include <us/gov/socket/client.h>
#include <us/gov/socket/datagram.h>
#include <us/wallet/engine/peer_t.h>

#include "endpoint_t.h"
#include "olog_t.h"
#include "ch_t.h"

namespace us::wallet::engine {
    struct daemon_t;
}

namespace us::wallet::trader {

    struct conman: us::gov::bgtask, olog_t {
        using b = olog_t;
        using t = us::gov::bgtask;
        using peer_t = engine::peer_t;
        using client = us::gov::socket::client;
        using datagram = us::gov::socket::datagram;
        using ch_t = trader::ch_t;

        enum state_t {
            state_off,
            state_offline,
            state_req_online,
            state_connect_failed,
            state_connected,
            state_online,
            state_req_offline,
            state_gone_offline,
            state_dying,
            num_states
        };

        static constexpr const char* statestr[num_states] = {"off", "offline", "req_online", "connect_failed", "connected", "online", "req_offline", "gone_offline", "dying"};

    public:
        conman(engine::daemon_t& demon);
        virtual ~conman();

    public:
        virtual ko exec_online(peer_t&, const string&, ch_t&) = 0;
        virtual ko exec_offline(const string&, ch_t&) = 0;
        virtual bool requires_online(const string& cmd) const = 0;
        virtual void on_KO(ko, const string&) = 0;
        virtual const endpoint_t& local_endpoint() const = 0;
        virtual pair<ko, pair<uint32_t, uint16_t>> resolve_ip_address(const hash_t& address) const = 0;
        virtual ko update_peer(peer_t&, ch_t&&) const = 0;
        virtual void dispose() = 0;
        virtual void saybye(peer_t&) = 0;

    public:
        virtual void die(const string& reason);
        virtual void offline();
        virtual void online(peer_t&);
//        virtual void on_start();

    public:
        void onwakeup();
        void run();
        ko start();
        void join();
        pair<bool, state_t> connection_supervisor();
        bool set_state(state_t);
        bool set_state_(state_t);
        void wait();
        void wait_busyref();
        void run_connection_bootstrap_supervisor();
        void show_data(const string& lang, ostream&) const;
        void show_data_(const string& lang, ostream&) const;
        void update_ip();
        state_t initiate_connection();
        void schedule_exec(string cmd);
        virtual void dump(const string& pfx, ostream&) const;
        state_t get_stateX() const;

        template<typename... Args> void olog(const Args&... args) const;

    private:
        state_t state{state_off};
        condition_variable cv;
        atomic<bool> state_changed{false};
        static constexpr uint8_t tries_req_online_def{10};
        int tries_req_online{tries_req_online_def};
        ko ko_ip_resolution{ok};

    public:
        hostport_t remote_ip{0, 0};
        endpoint_t remote_endpoint;
        string reason; //for dying
        atomic<int> busyref{0};

    private:
        peer_t* cli{nullptr};
        mutable mutex mx;
        engine::daemon_t& daemon;
        
        queue<string> cmdq;
        mutable mutex mx_cmdq;

        mutable unique_lock<mutex> data_lock{mx, defer_lock};

    protected:
        #if CFG_LOGS == 1
            string logdir;
            string logfile;
        #endif
    };

}

