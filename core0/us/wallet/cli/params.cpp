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
#include "params.h"

#include <thread>

#include <us/gov/config.h>
#include <us/wallet/engine/wallet_connection_t.h>

//#define loglevel "wallet/cli"
//#define logclass "params"
//#include <us/gov/logs.inc>

// don't log here, it's used before initializing the logging subsystem

using namespace us::wallet::cli;
using c = us::wallet::cli::params;

void c::constructor() {
    workers = 2 * thread::hardware_concurrency();

    ostringstream os;
    os << "/home/" << GOVUSER << "/downloads";
    downloads_dir = os.str();
}

c::params(): args("") {
    constructor();
}

c::params(const shell_args& a): args(a) {
    constructor();
    while(true) {
        string command = args.next<string>();
        if (command == "-wp") {
            walletd_port = args.next<int>();
        }
        else if (command == "-lp") {
            listening_port = args.next<uint16_t>();
        }
        else if (command == "-pp") {
            published_port = args.next<uint16_t>();
        }
        else if (command == "-pin") {
            pin = args.next<uint16_t>();
        }
        else if (command == "-whost") {
            walletd_host = args.next<string>();
        }
        else if (command == "-bp") {
            backend_port = args.next<int>();
        }
        else if (command == "-bhost") {
            backend_host = args.next<string>();
        }
        else if (command == "-home") {
            homedir = args.next<string>();
        }
        else if (command == "-d") {
            daemon = true;
        }
        else if (command == "-b") {
            advanced = false;
        }
        else if (command == "-n") {
            names = false;
        }
        else if (command == "-local") {
            local = true;
        }
//        else if (cmd=="-nc") {
//            p.ncurses=true;
//        }
        else if (command == "-log") {
            verbose = true;
        }
        else if (command == "-nolog") {
            nolog = true;
        }
        else if (command == "-logd") {
            logd = args.next<string>();
        }
        else if (command == "-c") {
            channel = args.next<channel_t>();
        }
        else if (command == "-v") {
            max_devices = args.next<int>();
        }
        else if (command == "-e") {
            max_trade_edges = args.next<int>();
        }
        else if (command == "-w") {
            workers = args.next<int>();
        }
        else if (command == "-om") {
            uint16_t m = args.next<uint16_t>();
            if (m < num_modes) om = (output_mode)m;
        }
        else if (command == "-dd") {
            downloads_dir = args.next<string>();
        }
        else if (command == "-nb") {
            banner = false;
        }
        else if (command == "-B") {
            using wallet_connection_t = us::wallet::engine::wallet_connection_t;
            auto blobb58 = args.next<string>();
            us::wallet::engine::wallet_connection_t wc;
            ko r = wc.read(blobb58);
            if (is_ko(r)) {
                cerr << r << '\n';
            }
            else {
                channel = wc.ip4_endpoint.channel;
                walletd_port = wc.ip4_endpoint.port;
                walletd_host = wc.ip4_endpoint.shost;
            }
        }
        else if (command == "--custodial_wallet_id") {
            subhome = args.next<string>();
        }
        #if CFG_FCGI == 1
        else if (command == "-fcgi") {
           fcgi = true;
        }
        #endif
        else if (command == "-t") {
            rpc_timeout_secs = args.next<uint16_t>(CFG_DGRAM_ROUNDTRIP_TIMEOUT_SECS);
        }
        else if (!command.empty()) {
            cmd = command;
            break;
        }
        else {
            break;
        }
    }
}

string c::get_home_wallet() const {
    return get_home() + "/wallet";
}

string c::get_home_wallet_rpc_client() const {
    return get_home_wallet() + "/rpc_client";
}

string c::get_home_gov() const {
    return get_home() + "/gov";
}

string c::get_home() const {
    ostringstream os;
    if (homedir.empty()) {
        const char* env_p = std::getenv("HOME");
        if (!env_p) {
            cerr << "No $HOME env var defined" << endl;
            exit(1);
        }
        os << env_p << "/" << DATADIR;
    }
    else {
        os << homedir;
    }
    if (channel != 0) {
        os << '/' << channel;
    }
    return os.str();
}

void c::dump(ostream& os) const {
    dump("", os);
}

void c::dump(const string& pfx, ostream& os) const {
    os << pfx << "channel: " << channel << '\n';
    os << pfx << "home: " << get_home() << '\n';
    #ifdef DEBUG
        os << pfx << "warning: this is a debug build.\n";
    #else
        os << pfx << "this is an optimized build.\n";
    #endif
    #if CFG_LOGS == 1
        if (verbose) {
            os << pfx << "writing log to cout\n";
        }
        else {
            os << pfx << "warning: logs are ON. Writing files in " << logd << '\n';
        }
    #else
        os << pfx << "logs: disabled.\n";
    #endif

    if (daemon) {
        os << pfx << "run wallet daemon\n";
        os << pfx << "  gov daemon at: " << backend_host << ":" << backend_port << '\n';
        os << pfx << "  listening port: " << listening_port << '\n';
        os << pfx << "  published listening port: " << published_port << '\n';
        os << pfx << "  max device edges: " << (int)max_devices << '\n';
        os << pfx << "  max trader edges: " << (int)max_trade_edges << '\n';
        os << pfx << "  hardware concurrency: " << thread::hardware_concurrency() << '\n';
        os << pfx << "  workers: " << (int)workers << '\n';
        #if CFG_FCGI == 1
            os << pfx << "  fcgi: " << (int)fcgi << '\n';
            if (fcgi) {
                os << pfx << "       listening at: 127.0.0.1:9000\n";
            }
        #endif
    }
    else {
        os << pfx << "run rpc shell\n";
        os << pfx << "  wallet daemon at: " << walletd_host << ":" << walletd_port << '\n';
        os << pfx << "  connect_for_recv " << rpc__connect_for_recv << '\n';
        os << pfx << "  stop_on_disconnection " << rpc__stop_on_disconnection << '\n';
    }
}

void c::connecting_to(ostream& os) const {
    os << PLATFORM;
    os << "-wallet daemon at " << walletd_host << ':' << walletd_port;
}

