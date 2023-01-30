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

#include <us/gov/io/seriable.h>

#include <us/wallet/trader/bookmarks_t.h>
#include <us/wallet/trader/bootstrap/protocols_t.h>
#include <us/wallet/engine/bookmark_index_t.h>
#include <us/wallet/trader/cert/cert_index_t.h>
#include <us/wallet/trader/cert/cert_t.h>

#include "types.h"
#include "protocol.h"

namespace us::gov::engine {
    struct track_status_t;
}

namespace us::wallet::trader {
    struct protocol_selection_t;
    struct endpoint_t;
    struct bookmarks_t;
    struct bookmark_t;
}

namespace us::wallet::engine {

    struct peer_t;
    struct track_status_t;
    struct bookmark_index_t;

    using blob_writer_t = gov::io::blob_writer_t;
    using blob_reader_t = gov::io::blob_reader_t;

    struct dto {
        using writable = us::gov::io::writable;
        using readable = us::gov::io::readable;
        using bookmarks_t = us::wallet::trader::bookmarks_t;
        using bookmark_t = us::wallet::trader::bookmark_t;
        using endpoint_t = us::wallet::trader::endpoint_t;
        using track_status_t = us::gov::engine::track_status_t;
        using protocol_selection_t = us::wallet::trader::protocol_selection_t;
        using protocols_t = us::wallet::trader::bootstrap::protocols_t;
        using bookmark_index_t = us::wallet::engine::bookmark_index_t;
        using cert_t = us::wallet::trader::cert::cert_t;
        using cert_index_t = us::wallet::trader::cert::cert_index_t;

        #include <us/api/generated/wallet/c++/engine/cllr_dto-hdr>
        #include <us/api/generated/wallet/c++/engine/hdlr_dto-hdr>
        #include <us/api/generated/wallet/c++/pairing/cllr_dto-hdr>
        #include <us/api/generated/wallet/c++/pairing/hdlr_dto-hdr>
        #include <us/api/generated/wallet/c++/r2r/cllr_dto-hdr>
        #include <us/api/generated/wallet/c++/r2r/hdlr_dto-hdr>
        #include <us/api/generated/wallet/c++/wallet/cllr_dto-hdr>
        #include <us/api/generated/wallet/c++/wallet/hdlr_dto-hdr>

    };

}

