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
#include <unordered_set>
#include <vector>
#include <deque>
#include <mutex>
#include <us/gov/types.h>
#include <us/gov/crypto/types.h>
#include <us/gov/socket/types.h>

namespace us::gov::peer {

    struct peer_t;

    struct grid_t: vector<peer_t*> {
        grid_t(): bi(end()) {}
        bool add(peer_t&, bool check_unique);
        bool ended(peer_t*);
        void dump(ostream&) const;
        void watch(ostream&) const;
        peer_t* prepare_worker_send(unordered_set<const peer_t*>& visited);
        bool find(const hash_t&) const;
        int num_edges_minage(int secs_old) const;
        peer_t* pick_one();

        iterator bi;
        mutable mutex mx_;

        struct faillog_t: deque<string> {
            mutable mutex mx;
            void add(const hostport_t&);
            void dump(ostream&) const;
        };

        faillog_t faillog;
    };

}

