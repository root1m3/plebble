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
#include <us/gov/engine/app.h>
#include <us/gov/peer/nodes_t.h>
#include <unordered_map>
#include <us/gov/types.h>

namespace us { namespace gov { namespace engine { namespace auth {

    using nodes_t = peer::nodes_t;

    struct collusion_control_t final: unordered_map<host_t, uint8_t> {
        static uint8_t max_nodes_per_ip;
        bool allow(host_t ip_addr);
        void rm(host_t ip_addr);
        void update_(const nodes_t&); //nodes locked by caller
        void dump(ostream&) const;
    };

}}}}

