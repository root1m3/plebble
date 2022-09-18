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
#include "node_ask.h"

#define loglevel "trader/test"
#define logclass "node_ask"
#include <us/gov/logs.inc>
#include <us/test/assert.inc>

using namespace std;

using c = us::test::node_ask;

c::node_ask(const string& id, const string& homedir, const string& logdir, const string& vardir, uint16_t gport, uint16_t wport): b(id, homedir, logdir, vardir, gport, wport) {
}

string c::desc() const {
    return "Impact Shopping";
}

bool c::load_data(const string& r2rhome) {
    ostringstream fname;
    fname << homedir << "/123/wallet/trader/" << r2rhome << "/data";
    if (!us::gov::io::cfg0::file_exists(fname.str())) {
        cout << "file " << fname.str() << " doesn't exist" << endl;
        return false;
    }
    cout << "loading shop data from file " << fname.str() << "doesn't exist" << endl;
    ifstream is(fname.str());
    is >> addr;
    is >> recv_coin;
    is >> reward_coin;
    assert(!is.fail());
    //created = true;
    return true;
}

vector<string> c::r2r_libs(bool filter_not_active) {
/*
    if (filter_not_active) {
        if (!created) return {};
    }
*/
    return {"bid2ask-ask"};
}

void c::vol_file(ostream& os) {
    os << "gv9rkTQJ7Cyzep2dXfeVFWvSJac 1000 10000 150 Product 1\n";
    os << "3qRd49QNBqpH6XzPyug67fQBuufe 1000 7510 230 Product 2\n";
    os << "2rbPjmoN3TpZ3KeVQnA6EJsKEkhi 1000 5050 17 Product 3\n";
}

void c::create_shop(const string& r2rhome) {
//    assert(!created);
    cout << "creating shop" << endl;
    b::create_node(r2rhome);
    auto& hmi = *wallet_cli;
    hmi.exec("balance 1");
    {
        string ans;
        auto r = hmi.rpc_daemon->get_peer().call_balance(1, ans);
        if (is_ko(r)) cout << r << endl;
        assert(r == ok);
        cout << "balance: " << ans << endl;
        assert(!ans.empty());
    }
    {
        auto r = hmi.rpc_daemon->get_peer().call_new_address(addr);
        if (r != ok) cout << r << endl;
        assert(r == ok);
    }
    cout << "new address: " << addr << endl;
    {
        blob_t blob;
        auto t = hmi.rpc_daemon->get_peer().call_transfer(us::wallet::cli::rpc_peer_t::transfer_in_t(addr, 10000000, hash_t(0), 1), blob);
        if (t != ok) cout << t << endl;
        assert(t == ok);
        cout << "transfer 10000000" << endl;
        cout << blob.size() << endl;
        {
            ostringstream cmd;
            cmd << "data " << addr;
            int n = 0;
            while(true) {
                us::gov::cash::accounts_t accounts;
                auto r = gov_cli->rpc_daemon->get_peer().call_fetch_accounts(us::gov::cash::addresses_t{addr}, accounts);
                if (r == ok) {
                    break;
                }
                cout << ++n << " waiting until address settles... " << accounts.size() << endl;
                this_thread::sleep_for(5s);
            }
        }
    }
    recv_coin = create_coin(100000000);
    reward_coin = create_coin(150000000);

    { //transfer coins to the shop address
        blob_t blob;
        auto t = wallet_cli->rpc_daemon->get_peer().call_transfer(us::wallet::cli::rpc_peer_t::transfer_in_t(addr, 10000000, reward_coin, 1), blob);
        if (t != ok) cout << t << endl;
        assert(t == ok);
        cout << "transfer 10000000 reward_coin " << reward_coin << " to " << addr << endl;
        cout << blob.size() << endl;
    }
    {
        int n = 0;
        while(true) {
            us::gov::cash::accounts_t accounts;
            auto r = gov_cli->rpc_daemon->get_peer().call_fetch_accounts(us::gov::cash::addresses_t{addr}, accounts);
            assert(r == ok);
            accounts.dump("", 2, cout);
            if (accounts.begin()->second.box.t != nullptr) break;
            cout << ++n << " waiting until address settles... " << accounts.size() << endl;
            this_thread::sleep_for(5s);
        }
    }
    {
        ostringstream cmd;
        cmd << "mkdir -p " << homedir << "/123/wallet/trader/" << r2rhome;
        system(cmd.str().c_str());
    }
    {
        ostringstream f;
        f << homedir << "/123/wallet/trader/" << r2rhome << "/data";
        cout << "writing file " << f.str() << endl;
        {
            ofstream os(f.str());
            os << addr << ' ' << recv_coin << ' ' << reward_coin << '\n';
        }
    }
    {
        ostringstream f;
        f << homedir << "/123/wallet/trader/" << r2rhome << "/vol";
        cout << "writing file " << f.str() << endl;
        {
            ofstream os(f.str());
            vol_file(os);
        }
    }
    {
        ostringstream f;
        f << homedir << "/123/wallet/trader/" << r2rhome << "/params_private";
        cout << "writing file " << f.str() << endl;
        {
            ofstream os(f.str());
            os << "personality_sk 3RHpTGvmFTvoDaVcymMMgXMuapTTT8v5rtprEGjZMdNG\n";
            os << "personality_moniker Impact Shopping\n";
        }
        //pub key 21DCFrG7K1NSJKTeiGNhTvnH4y5DZr4MDmSVHf8sLBcto
        //addr zzqCyAiEUz9mrw6EX1Rdehmovpv
    }
    //created = true;
    //restart_daemons();
}

void c::banner(ostream& os) const {
    os << "_ _|                            |     ___|  |                      _)             \n";
    os << "  |  __ `__ \\  __ \\   _` |  __| __| \\___ \\  __ \\   _ \\  __ \\  __ \\  | __ \\   _` | \n";
    os << "  |  |   |   | |   | (   | (    |         | | | | (   | |   | |   | | |   | (   | \n";
    os << "___|_|  _|  _| .__/ \\__,_|\\___|\\__| _____/ _| |_|\\___/  .__/  .__/ _|_|  _|\\__, | \n";
    os << "              _|                                       _|    _|            |___/  \n";
    os << "Do shopping at " << desc() << '\n';
}

