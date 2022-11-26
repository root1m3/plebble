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

#include <us/gov/config.h>
#include <us/wallet/trader/bootstrap/protocols_t.h>
#include <us/wallet/trader/bootstrap/initiator_t.h>
#include <us/wallet/engine/bookmark_index_t.h>


#include "protocol_selection_t.h"
#include "trader_protocol.h"
#include "qr_t.h"
#include "params_t.h"
#include "types.h"

namespace us::wallet::wallet {
    struct local_api;
}

namespace us::wallet::trader {

    struct business_t;
    struct trader_protocol;

    struct businesses_t final: map<protocol_selection_t, business_t*> {
        using wallet_local_api = us::wallet::wallet::local_api;
        using protocol_factories_t = trader_protocol::factories_t;
        using protocols_t = us::wallet::trader::bootstrap::protocols_t;
        using inverted_qr_t = us::wallet::trader::bootstrap::initiator_t::inverted_qr_t;
        using bookmark_index_t = us::wallet::engine::bookmark_index_t;

        businesses_t(wallet_local_api& parent);
        ~businesses_t();

        void init(const protocols_t& default_r2r);
        void cleanup();

    public:
        void api_list_protocols(ostream&) const;

    public:
        void select_all();
        business_t* select(const protocol_selection_t&);
        void recycle(business_t*);
        ko invert(qr_t&);

    private:
        ko select(iterator&);

/*
    public:
        struct r2r_t {
            trader_protocol* p{nullptr};
            business_t* bz{nullptr};
        };
*/
    public:

        [[nodiscard]] pair<ko, trader_protocol*> create_protocol(protocol_selection_t&&);
        //[[nodiscard]] pair<ko, trader_protocol*> create_opposite_protocolX(protocol_selection_t&&);
        [[nodiscard]] pair<ko, trader_protocol*> create_protocol(protocol_selection_t&&, params_t&& remote_params);

        void bookmark_info(vector<pair<protocol_selection_t, bookmark_info_t>>& o) const;
        pair<ko, hash_t> initiate(const hash_t parent_tid, const string& datadir, qr_t&&);

        void exec_help(const string& prefix, ostream&) const;
        ko exec(istream&);

        //void published_bookmarks(bookmarks_t&) const;

        void published_protocols(protocols_t&, bool invert) const;
//        void invert(protocols_t&) const;
//        void invert(protocol_t&) const;
        ko invert(protocol_selection_t&) const;
        protocols_t published_protocols(bool invert) const;
        void dump(ostream& os) const;

    public:
        ko handle_r2r_index_hdr(protocols_t& protocols);
        ko handle_r2r_bookmarks(protocol_selection_t&& protocol_selection, bookmarks_t& bookmarks);
        ko handle_r2r_index(bookmark_index_t& bookmark_index);

    public:
        wallet_local_api& parent;
        protocol_factories_t protocol_factories;

    };

}

