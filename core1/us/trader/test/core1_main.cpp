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
#include "main.h"

#include <us/gov/engine/auth/collusion_control_t.h>
#include <us/gov/config.h>
#include <us/gov/cli/hmi.h>

#include <us/wallet/cli/hmi.h>

#include <us/test/w2w_t.h>

#include "network.h"
#include "bid2ask_t.h"

#define loglevel "test"
#define logclass "main"
#include <us/gov/logs.inc>
#include <us/test/assert.inc>

#if CFG_COUNTERS == 0
    Error. configure with counters.
    bin/configure ... --with-counters
#endif

using namespace std;
using namespace us::trader::test;
using us::ko;
using us::ok;

bool batch = false;

void test_r2r(const string& homedir, const string& logdir, const string& vardir) {
    us::gov::engine::auth::collusion_control_t::max_nodes_per_ip = 255;
    ostream& os = cout;

    network n(homedir, logdir, vardir, "test-c1_r2r_stage1", os);

    cout << "stage1" << endl;
    n.stage1();

/*
    {
      w2w_t o(n);
      o.run();
    }
*/

    us::test::r2r_t::wait_from_seq = 0; //120

    {
        bid2ask_t o(n);
        o.run();
    }

    auto b = n.bookmarks();
    b.dump("bookmarks>", cout);

    if (!batch) {
        n.menu();
    }

    /*
    cout << "=software updates=====================================================================" << endl;
    //software updates account
    {
        auto a=bid.wallet_cli->api().new_address();
        assert(a.first==ok);
        cout << "software updates account: " << a.second << ". @ bid wallet" << endl;

        {
        cout << "list addresses:" << endl;
        auto l=bid.wallet_cli->api().list(1);
        assert(l.first==ok);
        cout << l.second << endl;
        }

        gov::engine::evidence* ev;
        {
            auto am=1000000;
            auto t=bid.wallet_cli->api().transfer(a.second, am, hash_t(0));
            if (t.first!=ok) cout << t.first << endl;
            assert(t.first==ok);
            cout << "transfer " << am << " to address " << a.second << endl;
            cout << t.second << endl;
            ev=gov::engine::evidence::from_b58(t.second);
        }
        uint64_t ts=ev->ts;

        wait_settlement(*bid.gov_cli, ts);
        delete ev;

        cout << "balance account " << a.second << ":" << endl;
        auto t=bid.wallet_cli->api().balance(1);
        if (t.first!=ok) {
            cout << t.first << endl;
        }
        else {
            cout << t.second << endl;
        }

    }
*/
    cout << "ended" << endl;
}

#if CFG_LOGS == 0
#undef log_dir
namespace {
    string log_dir() {
        ostringstream os;
        os << "logs/us-trader_" << getpid() << '/' << "test_r2r";
        return os.str();
    }
}
#endif

void test_nodes_main() {
    string logdir0 = "test_r2r";
    log_start(logdir0, "main");
    tee("=================test_nodes========================");

    string homedir = log_dir() + "/home";
    string vardir = log_dir() + "/var";

    network::test();

    test_r2r(homedir, logdir0, vardir);
    tee("=========== end testing nodes ==============");
}

void test_nodes() {
    tee("=================test_nodes========================");
    log("starting thread");
    thread th(&test_nodes_main); //start new log file
    th.join();
    tee("=========== end testing nodes ==============");
}

void help() {
    cout << "--batch        Unattended/not interactive" << endl;
}

int core1_main(int argc, char** argv) {
    us::gov::io::shell_args args(argc, argv);

    while(true) {
        string command = args.next<string>();
        if (command == "--batch") {
            batch = true;
        }
        else if (command.empty()) {
            break;
        }
        else {
            help();
            cout << "Invalid command/option." << endl;
            return 1;
        }
    }


    if (argc > 1) {
        string arg = argv[1];
        if (arg == "--batch") {
            batch = true;
        }
    }

    test_nodes();
    tee("TEST SUCCEED");
    us::gov::cli::hmi::process_cleanup();
    us::wallet::cli::hmi::process_cleanup();
    return 0;
}

