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
#include <us/gov/io/seriable.h>
#include <us/wallet/wallet/handler_api.h>

#include "dto.h"
#include "types.h"
#include "protocol.h"

namespace us::wallet::engine {

    struct peer_t;

    using blob_writer_t = gov::io::blob_writer_t;
    using blob_reader_t = gov::io::blob_reader_t;

    struct caller_api: virtual dto, us::wallet::wallet::caller_api {

        #include <us/api/generated/c++/wallet/engine/cllr_purevir>
        #include <us/api/generated/c++/wallet/pairing/cllr_purevir>
        #include <us/api/generated/c++/wallet/r2r/cllr_purevir>

    };

    struct api: caller_api {

        #include <us/api/generated/c++/wallet/engine/hdlr_purevir>
        #include <us/api/generated/c++/wallet/pairing/hdlr_purevir>
        #include <us/api/generated/c++/wallet/r2r/hdlr_purevir>

    };

}

