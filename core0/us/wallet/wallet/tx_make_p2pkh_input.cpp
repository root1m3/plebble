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
#include <us/gov/io/blob_writer_t.h>
#include <us/gov/io/blob_reader_t.h>

#include "tx_make_p2pkh_input.h"
#include "types.h"

#define loglevel "wallet/wallet"
#define logclass "tx_make_p2pkh_input"
#include <us/gov/logs.inc>

using namespace us::wallet::wallet;
using c = us::wallet::wallet::tx_make_p2pkh_input;

size_t c::blob_size() const {
    return blob_writer_t::blob_size(src_addr) +
        blob_writer_t::blob_size(rcpt_addr) +
        blob_writer_t::blob_size(amount) +
        blob_writer_t::blob_size(token);
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(src_addr);
    writer.write(rcpt_addr);
    writer.write(amount);
    writer.write(token);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(src_addr);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(rcpt_addr);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(amount);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(token);
        if (is_ko(r)) return r;
    }
    return ok;
}

