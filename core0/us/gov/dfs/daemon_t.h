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
#include <iostream>
#include <vector>
#include <set>
#include <cassert>

#include <us/gov/bgtask.h>
#include <us/gov/types.h>
#include <us/gov/relay/daemon_t.h>

#include "api.h"
#include "fileattr_t.h"
#include "types.h"

namespace us::gov::dfs {

    struct daemon_t;
    struct peer_t;

    struct mezzanine: bgtask {
        using b = bgtask;
        mezzanine(daemon_t*);
        daemon_t* d;
    };

    struct daemon_t: base_ns::daemon_t, mezzanine {
        using b = base_ns::daemon_t;
        using t = dfs::mezzanine;

        daemon_t(channel_t channel);
        daemon_t(channel_t channel, port_t port, pport_t pport, uint8_t edges, uint8_t devices, int workers, const vector<hostport_t>& seeds);
        daemon_t(const daemon_t&) = delete;
        ~daemon_t() override;

        daemon_t& operator = (const daemon_t&) = delete;

    public:
        static string resolve_filename(const string& filename);
        void dump(const string& prefix, ostream&) const;
        void save(const string& hash, const vector<uint8_t>& data, int propagate);
        string load(const string& hash);
        virtual bool check_dfs_permission(const hash_t& addr) const;
        void wait();
        void run();
        void on_dfs_wakeup();
        inline void wakeup_dfs() { t::task_wakeup(); }
        void index_set(const vector<pair<hash_t, uint32_t>>&);
        void index_add(const hash_t&, uint32_t sz);
        void index_rm(const hash_t&);
        void index_clear();
        void rm_file(const hash_t&);
        size_t rm_files(const set<hash_t>& exclude);
        socket::client* create_client(sock_t sock) override { assert(false); return nullptr; }

    public:
        ko start();
        ko wait_ready(const time_point& deadline) const;
        ko wait_ready(int seconds_deadline) const;
        void stop();
        void join();
        inline bool is_active() const { return b::is_active() && t::is_active(); }
        inline bool isup() const { return b::isup() && t::isup(); }

    public:
        condition_variable cv;
        mutable mutex mx;
        unordered_map<hash_t, uint32_t> index;
        string fsroot;
    };

}

