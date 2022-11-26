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
#include <string>
#include <mutex>
#include <functional>

#include <us/gov/config.h>
#include <us/gov/io/seriable_map.h>
#include <us/gov/io/blob_reader_t.h>

#include <us/wallet/trader/protocol_selection_t.h>
#include <us/wallet/trader/bookmarks_t.h>
#include <us/wallet/trader/bootstrap/protocols_t.h>

namespace us::wallet::wallet {
    struct local_api;
}

namespace us::wallet::engine {

    struct daemon_t;

    struct bookmark_index_t: us::gov::io::seriable_map<us::wallet::trader::protocol_selection_t, us::wallet::trader::bookmarks_t> {
        using protocol_selection_t = us::wallet::trader::protocol_selection_t;
        using bookmarks_t = us::wallet::trader::bookmarks_t;
        using bookmark_t = us::wallet::trader::bookmark_t;
        using protocols_t = us::wallet::trader::bootstrap::protocols_t;
        using qr_t = us::wallet::trader::qr_t;
        using serid_t = blob_reader_t::serid_t;

        void dump(ostream&) const;
        //bookmark_index_t& operator = (const bookmark_index_t& other);
        void protocols(protocols_t&) const;
        void protocols(function<bool (protocol_selection_t&)>, protocols_t&) const;
        //void inverted_protocols(function<bool (protocol_selection_t&)> invert, protocols_t& o) const {

        void fill_bookmarks(const protocol_selection_t&, bookmarks_t&);
        bookmark_index_t filter(function<bool (const protocol_selection_t&)>);

        serid_t serial_id() const override { return 'N'; }
    };

    struct bookmark_index2_t: bookmark_index_t {
        using b = bookmark_index_t;

        bookmark_index2_t(daemon_t&);

        void protocols(protocols_t&) const;
        void protocols(function<bool (protocol_selection_t&)>, protocols_t&) const;
        //void inverted_protocols(function<bool (protocol_selection_t&)> invert, protocols_t& o) const {
        void fill_bookmarks(const protocol_selection_t&, bookmarks_t&);
        bookmark_index_t filter(function<bool (const protocol_selection_t&)>);

        void add(wallet::local_api&);
        string filename() const;

    public:
        mutable mutex mx;
        daemon_t& parent;
    };
}

