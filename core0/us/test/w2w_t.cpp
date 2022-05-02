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
#include "w2w_t.h"
#include "node.h"
#include "dispatcher_t.h"
#include "network.h"

#define loglevel "test"
#define logclass "w2w_t"
#include <us/gov/logs.inc>
#include "assert.inc"

using c = us::test::w2w_t;

void c::test(node& w1, node& w2) {
    using hash_t = us::gov::crypto::ripemd160::value_type;
    cout << "w1 node: ";
    w1.gov_cli->exec("id");
    cout << "w2 node: ";
    w2.gov_cli->exec("id");
    cout << "w1 node wallet port: " << w1.wport << endl;
    cout << "w2 node wallet port: " << w2.wport << endl;
}

void c::run() {
    auto& n1 = *n.find("pat")->second;
    auto& n2 = *n.find("gp")->second;
    test_r2r_cfg(n1, n2, [&](node& n1, node& n2) { test(n1, n2); }, trade_id);
}

