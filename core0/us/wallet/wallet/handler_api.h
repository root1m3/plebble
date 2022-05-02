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
#include <us/wallet/engine/protocol.h>
#include <us/wallet/engine/dto.h>
#include <us/wallet/trader/bookmarks_t.h>
#include <us/wallet/trader/endpoint_t.h>
#include "types.h"

namespace us { namespace wallet { namespace engine {
    struct peer_t;
}}}

namespace us { namespace wallet { namespace wallet {

    using blob_writer_t = gov::io::blob_writer_t;
    using blob_reader_t = gov::io::blob_reader_t;

    struct handler_api: virtual us::wallet::engine::dto {
        #include <us/api/generated/c++/wallet/wallet/hdlr_purevir>
    };

    struct caller_api: virtual us::wallet::engine::dto {
        #include <us/api/generated/c++/wallet/wallet/cllr_purevir>
    };

}}}

