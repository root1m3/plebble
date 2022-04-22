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
#include "sysops_t.h"
#include "protocol.h"
#include "peer_t.h"

#define loglevel "gov/engine"
#define logclass "sysops_t"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::sysops_t;

ko c::exec(peer_t* peer, const string& cmd, ostream& os) {
    lock_guard<mutex> lock(mx);
    auto i = find(peer);
    if (i == end()) {
        i = emplace(peer, shell(this->d)).first;
    }
    log("sysop cmd. size", cmd.size(), ">", cmd, "<");
    return i->second.command(cmd, os);
}

