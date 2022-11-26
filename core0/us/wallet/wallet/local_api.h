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

#include <us/gov/config.h>
#include <us/gov/relay/pushman.h>
#include <us/gov/engine/track_status_t.h>

#include <us/wallet/engine/bookmark_index_t.h>
#include <us/wallet/trader/bootstrap/protocols_t.h>
#include <us/wallet/trader/businesses_t.h>
#include <us/wallet/trader/traders_t.h>

#include "handler_api.h"
#include "algorithm.h"
#include "txlog_t.h"
#include "bookmarksman_t.h"
#include "types.h"

namespace us::wallet::engine {
        struct daemon_t;
}

namespace us::wallet::trader {
        struct traders_t;
//        struct businesses_t;
        struct bookmark_info_t;
}

namespace us::wallet::wallet {

    struct local_api final: algorithm, handler_api {
        using w = algorithm;
        using daemon_t = engine::daemon_t;
        using track_status_t = us::gov::engine::track_status_t;
        using protocols_t = us::wallet::trader::bootstrap::protocols_t;
        using bookmark_index_t = us::wallet::engine::bookmark_index_t;
        using traders_t = us::wallet::trader::traders_t;
        using protocol_selection_t = us::wallet::trader::protocol_selection_t;
        using bookmark_info_t = us::wallet::trader::bookmark_info_t;

        static const char* KO_20183;

        local_api(engine::daemon_t&, const string& home, const string& subhome, const hash_t& subhomeh, trader::endpoint_t&&);
        ~local_api() override;

        ko refresh_data();
        void dump(const string& pfx, ostream& os) const;
        pair<ko, affected_t> track_pay(const asa_t& asa_pay, const asa_t& asa_charge, tx_t&, const hash_t& tid, track_status_t&);
        pair<ko, affected_t> analyze_pay(const asa_t& asa_pay, const asa_t& asa_charge, const tx_t&);

    public: //wallet
        #include <us/api/generated/wallet/c++/wallet/hdlr_override>

    public:
        enum push_code_t: uint16_t { //communications node-HMI
            push_begin = 50,
            push_txlog = push_begin,

            push_end
        };

    public:
        blob_t push_payload(uint16_t pc) const;
        datagram* get_push_datagram(const hash_t& trade_id, uint16_t pc) const;
        vector<datagram*> get_push_datagrams(const set<hash_t>& trade_ids, uint16_t pc) const;

        void on_tx_tracking_status(const track_status_t&);

    public:
        void published_bookmarks(bookmarks_t&) const;
        //void guest_published_protocols(protocols_t&) const;
        void published_bookmark_infos(vector<pair<protocol_selection_t, bookmark_info_t>>&) const;

        bookmarksman_t bookmarks;

    public:
        void schedule_push(socket::datagram*);
        void schedule_push(vector<socket::datagram*>&&);

        ko push_KO(ko) const;
        ko push_OK(const string& msg) const;
        ko push_KO(const hash_t& tid, ko msg) const;
        ko push_OK(const hash_t& tid, const string& msg) const;

    public:
        ko start();
        ko wait_ready(const time_point& deadline) const;
        void stop();
        void join();

    public:
        ko exec(const string& cmd); ///API
        ko exec(istream&);
        void exec_help(const string& prefix, ostream&) const;
        ko exec(const hash_t& tid, const string& cmd); /// API command_trade

    public:
        daemon_t& daemon;
        int refcount{0};
        relay::pushman::filter_t device_filter;
        string subhome;
        hash_t subhomeh;
        string lang{"en"};
        string home;
        trader::endpoint_t local_endpoint;
        txlog_t txlog;

        #if CFG_LOGS == 1
            string logdir;
        #endif

        traders_t traders;

    public: //components depending on (this)
        us::wallet::trader::businesses_t businesses;
    };

}

