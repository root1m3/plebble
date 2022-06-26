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
#include "shard.h"
#include <us/gov/config.h>
#include <us/test/network.h>

#define loglevel "us/test/shard"
#define logclass "main"
#include <us/test/logs.inc>

#include <us/test/assert.inc>

using namespace std;
using namespace us::test;
using c = us::test::shard;
/*
namespace {

struct network_c0: network {
    using b = network;

    network_c0(const string& homedir, const string& logdir, const string& vardir, ostream& os): b(homedir, logdir, vardir, "test-c0_r2r_stage1", os) {
        add_node("bank1", new node_bank("bank1", homedir, logdir, vardir, 22072, 22073));
        add_node("bank2", new node_bank("bank2", homedir, logdir, vardir, 22172, 22173));
    }

    ~network_c0() {
    }

    void stage1_configure() override {
        cout << "configuring bank nodes" << endl;
        dynamic_cast<node_bank*>(find("bank1")->second)->create_bank("w2w/bank1");
        dynamic_cast<node_bank*>(find("bank2")->second)->create_bank("w2w/bank2");
        find("bank1")->second->register_wallet();
        find("bank2")->second->register_wallet();
    }

    void stage1_ff_configure() override {
        cout << "configuring ask node" << endl;
        assert(dynamic_cast<node_bank*>(find("bank1")->second)->load_data("w2w/bank1"));

        cout << "configuring pat node" << endl;
        assert(dynamic_cast<node_bank*>(find("bank2")->second)->load_data("w2w/bank2"));
    }

};

}
*/
void c::main() {
}
