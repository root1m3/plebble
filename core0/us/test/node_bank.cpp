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
#include "node_bank.h"
#include <us/wallet/trader/workflow/doctype_processors_t.h>

#define loglevel "test"
#define logclass "node_bank"
#include <us/gov/logs.inc>
#include "assert.inc"

using c = us::test::node_bank;
using namespace std;

c::node_bank(const string& id, const string& homedir, const string& logdir, const string& vardir, port_t gport, port_t wport): node(id, homedir, logdir, vardir, gport, wport) {
}

void c::update_plugins(const string& destdir) {
    tee("update_plugins");
    b::update_plugins(destdir);
}

bool c::load_data(const string& r2rhome) {
    tee("load_data");
    return b::load_data(r2rhome);
}

string c::desc() const {
    return "Banksy";
}

vector<string> c::r2r_libs(bool filter_not_active) {
    return {"w2w-w"};
}

void c::create_bank(const string& r2rhome) {
    b::create_node(r2rhome);
    cout << "creating bank" << endl;
}

void c::banner(ostream& os) const {
    os << "######                             \n";
    os << "#     # #### #   # #    #     #   #\n";
    os << "#     # #  # ##  # #   #   ##  # # \n";
    os << "######  #### # # # ####   #     #  \n";
    os << "#     # #  # # # # #  #    #    #  \n";
    os << "#     # #  # #  ## #   #    #   #  \n";
    os << "######  #  # #  ## #    # ##    #  \n";
    os << "A bank called " << desc() << '\n';
    b::banner(os);
}

