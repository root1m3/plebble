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
#include "basket.h"
#include <fstream>

#include <us/wallet/protocol.h>
#include <us/wallet/wallet/local_api.h>
#include <us/wallet/trader/params_t.h>
#include <us/wallet/engine/daemon_t.h>

#include <us/trader/workflow/types.h>

#define loglevel "trader/workflow/consumer"
#define logclass "basket_item_t"
#include <us/gov/logs.inc>

using namespace us::trader::workflow;
using c = us::trader::workflow::consumer::basket_item_t;

bool c::operator == (const c& other) const {
       return sale_price_per_unit() == other.sale_price_per_unit() && reward_price_per_unit() == reward_price_per_unit();
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(first) + blob_writer_t::blob_size(second);
 }

void c::to_blob(blob_writer_t& writer) const {
    writer.write(first);
    writer.write(second);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(first);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(second);
        if (is_ko(r)) return r;
    }
    return ok;
}

