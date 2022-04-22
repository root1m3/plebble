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
#include "wallet_address.h"

#include <thread>
#include <chrono>
#include <iostream>
#include <sys/time.h>

#include <us/gov/likely.h>
#include <us/gov/crypto/base58.h>
#include <us/gov/peer/daemon_t.h>

#include "app.h"
#include "types.h"

#define loglevel "gov/traders"
#define logclass "wallet_address"
#include <us/gov/logs.inc>

using namespace us::gov::traders;
using c = us::gov::traders::wallet_address;

c::wallet_address(const hash_t& pkh, host_t netaddr, port_t pport): pkh(pkh), net_addr(netaddr), pport(pport), b(us::gov::traders::app::id(), 0) {
    log("wallet_address constructor", pkh, netaddr, pport);
}

c::wallet_address(): b(us::gov::traders::app::id(), eid) {
}

c::~wallet_address() {
    log("destroying wallet_address");
}

void c::hash_data_to_sign(sigmsg_hasher_t& h) const {
    b::hash_data_to_sign(h);
    h.write(pkh);
    h.write(net_addr);
    h.write(pport);
}

void c::hash_data(hasher_t& h) const {
    b::hash_data(h);
    h.write(pkh);
    h.write(net_addr);
    h.write(pport);
}

size_t c::blob_size() const {
    return b::blob_size() + blob_writer_t::blob_size(pkh) + blob_writer_t::blob_size(net_addr) + blob_writer_t::blob_size(pport) + s::blob_size();
}

void c::to_blob(blob_writer_t& writer) const {
    b::to_blob(writer);
    writer.write(pkh);
    writer.write(net_addr);
    writer.write(pport);
    s::to_blob(writer);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = b::from_blob(reader);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(pkh);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(net_addr);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(pport);
        if (is_ko(r)) return r;
    }
    {
        auto r = s::from_blob(reader);
        if (is_ko(r)) return r;
    }
    return ok;
}

void c::write_pretty_en(ostream& os) const {
    os << tx_begin_en << '\n';
    os << "  trader transaction type " << eid << " - Wallet Address\n";
    os << "  timestamp: " << ts << '\n';
    os << "  pkh " << pkh << '\n';
    os << "  net_addr " << net_addr << '\n';
    os << "  pport " << pport << '\n';
    os << '\n';
    os << tx_end_en << '\n';
    s::write_pretty_en(os);
}

void c::write_pretty_es(ostream& os) const {
    os << tx_begin_es << '\n';
    os << "  tipo transaccion negociador " << eid << " - Direccion Monedero\n";
    os << "  fecha: " << ts << '\n';
    os << "  pkh " << pkh << '\n';
    os << "  net_addr " << net_addr << '\n';
    os << "  pport " << pport << '\n';
    os << '\n';
    os << tx_end_es << '\n';
    s::write_pretty_es(os);
}

bool c::verify(ostream& os) const {
    bool ans;
    ans = b::verify(os);
    ans &= s::verify(os);
    return ans;
}

void c::wallet_address::dump(const string& prefix, ostream& os) const {
    os << prefix << "pkh " << pkh << '\n';
    os << prefix << "net_addr " << net_addr << '\n';
    os << prefix << "pport " << pport << '\n';
}

