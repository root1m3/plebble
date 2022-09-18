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

#include <us/test/node.h>

namespace us::test {

    using namespace std;

    struct node_bid: node {
        using b = node;

        node_bid(const string& id, const string& homedir, const string& logdir, const string& vardir, uint16_t gport, uint16_t wport);
        //bool load_data(const string& r2rhome);
        string desc() const override;
        vector<string> r2r_libs(bool filter_not_active) override;
        void create_bid(const string& r2rhome, vector<node*> processors, bookmarks_t&&);
        void banner(ostream& os) const override;

    };

}

