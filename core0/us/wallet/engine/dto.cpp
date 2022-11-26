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
#include <us/gov/io/blob_reader_t.h>
#include <us/gov/io/blob_writer_t.h>
#include <us/gov/engine/track_status_t.h>

#include <us/wallet/trader/endpoint_t.h>
#include "bookmark_index_t.h"
#include "dto.h"

#define loglevel "gov/engine"
#define logclass "dto"
#include "logs.inc"
#include <us/gov/socket/dto.inc>

using c = us::wallet::engine::dto;

#include <us/api/generated/wallet/c++/engine/cllr_dto-impl>
#include <us/api/generated/wallet/c++/engine/hdlr_dto-impl>
#include <us/api/generated/wallet/c++/pairing/cllr_dto-impl>
#include <us/api/generated/wallet/c++/pairing/hdlr_dto-impl>
#include <us/api/generated/wallet/c++/r2r/cllr_dto-impl>
#include <us/api/generated/wallet/c++/r2r/hdlr_dto-impl>
#include <us/api/generated/wallet/c++/wallet/cllr_dto-impl>
#include <us/api/generated/wallet/c++/wallet/hdlr_dto-impl>
