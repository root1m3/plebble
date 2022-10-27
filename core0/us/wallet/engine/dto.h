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
#include <us/gov/engine/track_status_t.h>

#include <us/wallet/trader/bookmarks_t.h>
#include <us/wallet/trader/endpoint_t.h>

#include "types.h"
#include "protocol.h"

namespace us::wallet::engine {

    struct peer_t;

    using blob_writer_t = gov::io::blob_writer_t;
    using blob_reader_t = gov::io::blob_reader_t;

    struct dto {
        using bookmarks_t = us::wallet::trader::bookmarks_t;
        using bookmark_t = us::wallet::trader::bookmark_t;
        using endpoint_t = us::wallet::trader::endpoint_t;
        using track_status_t = us::gov::engine::track_status_t;

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

