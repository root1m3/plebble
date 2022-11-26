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
#include <map>
#include <utility>
#include <mutex>
#include <unordered_map>
#include <fstream>

#include <us/gov/crypto/ripemd160.h>
#include <us/gov/config.h>
#include <us/gov/io/seriable.h>

#include <us/wallet/trader/bootstrap/protocols_t.h>
#include <us/wallet/trader/bootstrap/initiator_t.h>

#include "libs_t.h"

namespace us::wallet::wallet {
    struct local_api;
}

namespace us::wallet::engine {
    struct daemon_t;
}

namespace us::wallet::trader {

    struct business_t;

    //route trading_msg src -> wallet in charge
    struct route_w final: map<route_t, wallet::local_api*> {
        route_w(route_t route, wallet::local_api* w) {
            emplace(route, w);
        }

        void dump(const string& pfx, ostream&) const;
    };

    struct trades_t final: unordered_map<hash_t, route_w> { //, us::gov::io::seriable {
        using protocols_t = bootstrap::protocols_t;
        using peer_t = engine::peer_t;
        using inverted_qr_t = us::wallet::trader::bootstrap::initiator_t::inverted_qr_t;


    public:
        trades_t(engine::daemon_t&);
        trades_t(const trades_t&) = delete;
        ~trades_t();

        void dump(const string& pfx, ostream&) const;
/*        
    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;
        string sername() const;
        void save_state() const;
        void load_state();
*/

    public:
        void reload_file(const string& fqn);
        ko trading_msg(peer_t&, route_t, const hash_t& tid, svc_t svc, blob_t&& msg);
        pair<ko, hash_t> initiate(const hash_t parent_tid, const string& datadir, inverted_qr_t&&, wallet::local_api&);

    private:
        ko resume_trade(peer_t& peer, route_t, const hash_t& trade_id, svc_t svc, const blob_t& blob);

    public:
        protocols_t default_protocols(bool guest) const;
        void save_default_protocols() const;

    public:
        #if CFG_LOGS == 1
            string logdir;
        #endif

        libs_t libs;
        
        engine::daemon_t& daemon;

    private:
        mutable mutex _mx;

    };

}

