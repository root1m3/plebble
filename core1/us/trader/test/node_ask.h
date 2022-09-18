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

    struct node_ask: us::test::node {
        using b = us::test::node;

        node_ask(const string& id, const string& homedir, const string& logdir, const string& vardir, uint16_t gport, uint16_t wport);
        string desc() const override;
        virtual bool load_data(const string& r2rhome);
        vector<string> r2r_libs(bool filter_not_active) override;
        virtual void vol_file(ostream& os);
        void create_shop(const string& r2rhome);
        void banner(ostream& os) const override;

        hash_t addr;
        hash_t recv_coin;
        hash_t reward_coin;

    };

}

