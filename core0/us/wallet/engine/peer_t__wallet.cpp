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
#include "peer_t.h"
#include <sstream>
#include "types.h"

#include <us/gov/socket/datagram.h>
#include <us/gov/cash/ttx.h>
#include <us/gov/cash/map_tx.h>
#include <us/gov/cash/file_tx.h>
#include <us/wallet/engine/daemon_t.h>
#include <us/wallet/wallet/algorithm.h>
#include <us/wallet/trader/bookmarks_t.h>
#include <us/wallet/wallet/local_api.h>
#include <us/wallet/engine/bookmark_index_t.h>

#define loglevel "wallet/engine"
#define logclass "peer_t__wallet"
#include <us/gov/logs.inc>
#include <us/gov/socket/dto.inc>

using namespace us::wallet::engine;
using c = us::wallet::engine::peer_t;

#include <us/api/generated/wallet/c++/wallet/cllr_rpc-impl>

bool c::process_work__wallet(datagram* d) {
    log("process_work__wallet");
    using namespace protocol;
    assert(is_role_device());
    assert(local_w != nullptr);
    switch(d->service) {
        #include <us/api/generated/wallet/c++/wallet/hdlr_svc-router>
    }
    return false;
}

//using delegate = *local_w;
#define delegate (*local_w)
#include <us/api/generated/wallet/c++/wallet/hdlr_svc_handler-impl>

