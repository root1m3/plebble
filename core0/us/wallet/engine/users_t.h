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
#include <unordered_map>
#include <mutex>
#include <string>

#include <us/gov/config.h>
#include <us/gov/engine/track_status_t.h>

#include "types.h"

namespace us::wallet::wallet {
    struct local_api;
}

namespace us::wallet::engine {

    struct daemon_t;
    struct peer_t;

    struct users_t final: unordered_map<hash_t, wallet::local_api*> {
        using datagram = us::gov::socket::datagram;
        using track_status_t = us::gov::engine::track_status_t;

    public:
        users_t(daemon_t&);
        ~users_t();

        void init();

    public:
        wallet::local_api* get_wallet(const string& subhome); //returns soft pointer
        void release_wallet(wallet::local_api*);

    public:
        void dump(ostream&) const;
        void sync(ostream&);
        void on_tx_tracking_status(const track_status_t&);

    public:
        daemon_t& daemon; //daemon mode, trading enabled

        #if CFG_LOGS == 1
            string logdir;
        #endif

        wallet::local_api* root_wallet{nullptr};

    private:
        mutable mutex mx;
    };

}

