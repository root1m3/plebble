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
#include <us/gov/engine/protocol.h>
#include <us/gov/engine/local_deltas_t.h>
#include <us/gov/io/blob_writer_t.h>
#include <us/gov/io/blob_reader_t.h>
#include <us/gov/cash/accounts_t.h>
#include <us/gov/cash/addresses_t.h>
#include <us/gov/types.h>

namespace us::gov::engine {

    struct peer_t;

    using blob_writer_t = io::blob_writer_t;
    using blob_reader_t = io::blob_reader_t;

    struct dto {
        using blob_t = io::blob_t;
        using accounts_t = cash::accounts_t;
        using addresses_t = cash::addresses_t;

        #include <us/api/generated/c++/gov/engine/hdlr_dto-hdr>
        #include <us/api/generated/c++/gov/engine/cllr_dto-hdr>
        #include <us/api/generated/c++/gov/engine_auth/hdlr_dto-hdr>
        #include <us/api/generated/c++/gov/engine_auth/cllr_dto-hdr>
        #include <us/api/generated/c++/gov/cash/hdlr_dto-hdr>
        #include <us/api/generated/c++/gov/cash/cllr_dto-hdr>
        #include <us/api/generated/c++/gov/traders/hdlr_dto-hdr>
        #include <us/api/generated/c++/gov/traders/cllr_dto-hdr>
        #include <us/api/generated/c++/gov/sys/hdlr_dto-hdr>
        #include <us/api/generated/c++/gov/sys/cllr_dto-hdr>
    };

    struct caller_api: dto {
        #include <us/api/generated/c++/gov/engine/cllr_purevir>
        #include <us/api/generated/c++/gov/engine_auth/cllr_purevir>
        #include <us/api/generated/c++/gov/cash/cllr_purevir>
        #include <us/api/generated/c++/gov/traders/cllr_purevir>
        #include <us/api/generated/c++/gov/sys/cllr_purevir>
    };

    struct api: caller_api {
        #include <us/api/generated/c++/gov/engine/hdlr_purevir>
        #include <us/api/generated/c++/gov/engine_auth/hdlr_purevir>
        #include <us/api/generated/c++/gov/cash/hdlr_purevir>
        #include <us/api/generated/c++/gov/traders/hdlr_purevir>
        #include <us/api/generated/c++/gov/sys/hdlr_purevir>
    };

}

