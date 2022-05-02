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
#include "node_bid.h"
#include <us/wallet/trader/workflow/doctype_processors_t.h>

#define loglevel "trader/test"
#define logclass "node_bid"
#include <us/gov/logs.inc>
#include <us/test/assert.inc>

using c = us::trader::test::node_bid;
using namespace std;

c::node_bid(const string& id, const string& homedir, const string& logdir, const string& vardir, uint16_t gport, uint16_t wport): b(id, homedir, logdir, vardir, gport, wport) {
}

bool c::load_data(const string& r2rhome) {
    created = true;
    return true;
}

string c::desc() const {
    return "Bidrick";
}

vector<string> c::r2r_libs(bool filter_not_active) {
    return {"bid2ask-bid"};
}

void c::create_bid(const string& r2rhome, vector<node*> processors, bookmarks_t&& bookmarks) {
    b::create_node(r2rhome);
    using doctype_processors_t = us::wallet::trader::workflow::doctype_processors_t;
    assert(!created);
    cout << "creating bid" << endl;
    auto& hmi = *wallet_cli;

    ///doc processors for bid node
    {
        doctype_processors_t doctype_processors;
        doctype_processors.clear();
        for (auto& i: processors) {
            string desc = i->desc();
            string ep = i->ep();
            pair<vector<uint16_t>, vector<uint16_t>> dc = i->doctypes(); ///consume, produce
            for (auto& j: dc.first) {
                doctype_processors.add(j, desc, ep); //nodes consuming doctype
            }
        }
        ostringstream f;
        f << homedir << "/123/wallet/trader/" << r2rhome << "/doctype_processors";
        cout << "writing file " << f.str() << endl;
        doctype_processors.save(f.str());
    }
    created = true;
    restart_daemons();
}

void c::banner(ostream& os) const {
     os << "######                                       \n";
     os << "#     # # ####   #####  #  ####  #    # \n";
     os << "#     # #  #  #  #    # # #    # #   #  \n";
     os << "######  #  #   # #    # # #      ####   \n";
     os << "#     # #  #   # #####  # #      #  #   \n";
     os << "#     # #  #  #  #   #  # #    # #   #  \n";
     os << "######  # ####   #    # #  ####  #    # \n";
     os << "A bidder called " << desc() << '\n';
}

