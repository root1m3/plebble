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
#pragma once

#include <map>
#include <set>
#include <vector>
#include "sim.h"

namespace us::sim {

    using namespace std;

    struct node_t;

    struct account {
        int balance;
    };

    struct db_t: map<address_t, account> {
        using b = map<address_t, account>;

        vector<node_t*> nodes;

        int version{0};

        db_t();
        db_t(const db_t&);
        db_t* split(int bit);
        void dump(const string& pfx, ostream& os) const;

        set<node_t*> random_nodes(int);
        node_t* random_node();
    };

}

