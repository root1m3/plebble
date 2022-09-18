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
using namespace us::test;
using us::ko;
using us::ok;

bool batch = false;
bool only_interactive_shell = false;

void test_r2r(const string& homedir, const string& logdir, const string& vardir) {
    us::gov::engine::auth::collusion_control_t::max_nodes_per_ip = 255;
    ostream& os = cout;

    network_c1 n(homedir, logdir, vardir, "test-c1_r2r_stage1", os);
    n.start();

    //us::test::r2r_t::wait_from_seq = 0; //120

    if (!only_interactive_shell) {
        bid2ask_t o(n);
        o.run();
    }

    auto b = n.bookmarks();
    b.dump("bookmarks>", cout);

    cout << "CORE1-L2 TESTS PASS" << endl;

    if (!batch) {
        n.menu();
    }

    n.stop();
    n.join();
}

#if CFG_LOGS == 0
#undef log_dir
namespace {
    string log_dir() {
        ostringstream os;
        os << "logs/us-trader_" << getpid();
        return os.str();
    }
}
#endif

void test_l2_main(string logdir0) {
    log_start(logdir0, "main");
    tee("=================test_l2========================");

    string homedir = log_dir() + "/home";
    string vardir = log_dir() + "/var";

    network::test();

    test_r2r(homedir, logdir0, vardir);
    tee("=========== end testing l2 ==============");
}

void test_l2() {
    log("starting thread");
    string logdir0 = log_dir() + "/test_l2";
    thread th(&test_l2_main, logdir0); //start new log file
    th.join();
}

void help() {
    cout << "--batch        Unattended/not interactive" << endl;
    cout << "--shell        Bootstrap network and start interactive shell." << endl;
    cout << "--wait-between-steps     capture unexpected push_data datagrams before entering next step.\n";
}

int core1_main(int argc, char** argv) {
    us::gov::io::shell_args args(argc, argv);

    while(true) {
        string command = args.next<string>();
        if (command == "--batch") {
            batch = true;
        }
        else if (command == "--shell") {
            only_interactive_shell = true;
            batch = false;
        }
        else if (command.empty()) {
            break;
        }
        else if (command == "--wait-between-steps") {
            cout << "waiting between steps\n";
            us::test::r2r_t::enable_wait = true;
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

    test_l2();

    tee("TEST SUCCEED");
    us::gov::cli::hmi::process_cleanup();
    us::wallet::cli::hmi::process_cleanup();
    return 0;
}

