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
#include <random>
#include <functional>

#include <us/gov/config.h>
#include <us/gov/engine/daemon_t.h>
#include <us/gov/auth/peer_t.h>

#include "acl_tx.h"
#include "node_address_tx.h"

#define loglevel "gov/engine/auth"
#define logclass "node_address_tx"
#include <us/gov/engine/logs.inc>

using namespace us::gov::engine;
using c = us::gov::engine::auth::node_address_tx;

c::node_address_tx(): b(engine::auth::app::id(), eid) {
}

c::node_address_tx(const hash_t& pkh, uint32_t netaddr, uint16_t pport): pkh(pkh), net_addr(netaddr), pport(pport), b(engine::auth::app::id(), eid) {
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "pkh " << pkh << '\n';
    os << prefix << "net_addr " << net_addr << '\n';
    os << prefix << "pport " << pport << '\n';
}

void c::write_pretty_en(ostream& os) const {
    os << tx_begin_en << '\n';
    os << "  engine::auth transaction type " << eid << " - Node Address\n";
    os << "  timestamp: " << ts << '\n';
    os << "  pkh " << pkh << '\n';
    os << "  net_addr " << net_addr << '\n';
    os << "  pport " << pport << '\n';
    os << '\n';
    os << tx_end_en << '\n';
}

void c::write_pretty_es(ostream& os) const {
    os << tx_begin_en << '\n';
    os << "  engine::auth tipo transaccion " << eid << " - Direccion de nodo\n";
    os << "  timestamp: " << ts << '\n';
    os << "  pkh " << pkh << '\n';
    os << "  net_addr " << net_addr << '\n';
    os << "  pport " << pport << '\n';
    os << '\n';
    os << tx_end_en << '\n';
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(pkh) +
    blob_writer_t::blob_size(net_addr) +
    blob_writer_t::blob_size(pport);
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(pkh);
    writer.write(net_addr);
    writer.write(pport);
}

ko c::from_blob(blob_reader_t& reader) {
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
    return ok;
}

