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

#include "types.h"

#define loglevel "io"
#define logclass "params"
#include "logs.inc"

using namespace us::gov::io;
using c = us::gov::io::params;

c::params(): args("") {
    log("hardware concurrency", thread::hardware_concurrency());
    workers = 2 * thread::hardware_concurrency();
}

c::params(const shell_args& a): args(a) {
    log("hardware concurrency", thread::hardware_concurrency());
    workers = 2 * thread::hardware_concurrency();

    while(true) {
        string command = args.next<string>();
        if (command == "-p") {
            port = args.next<port_t>();
        }
        else if (command == "-pp") {
            pport = args.next<pport_t>();
        }
        else if (command == "-d") {
            daemon = true;
        }
        else if (command == "-ds") {
            daemon = true;
            shell = true;
        }
        else if (command == "-fs") {
            force_seeds = true;
        }
        else if (command == "-dot") {
            dot = true;
        }
        else if (command == "-e") {
            edges = args.next<int>();
        }
        else if (command == "-v") {
            devices = args.next<int>();
        }
        else if (command == "-w") {
            workers = args.next<int>();
        }
        else if (command == "-c") {
            channel = args.next<channel_t>();
        }
        else if (command == "-home") {
            homedir = args.next<string>();
        }
        else if (command == "-host") {
            sysophost = args.next<string>();
        }
        else if (command == "-xp") {
            //daemon_encrypt_protocol = args.next<bool>(false); //Ignored. deprecated
        }
        else if (command == "-status") {
            status_file = args.next<bool>(false);
        }
        #if CFG_LOGS == 1
        else if (command == "-log") {
            verbose = true;
        }
        #endif
        else if (command == "-om") {
            uint16_t m = args.next<uint16_t>();
            if (m < num_modes) om = (output_mode)m;
        }
        else if (command == "-n") {
            names = true;
        }
        else if (command == "-nb") {
            banner = false;
        }
        else if (!command.empty()) {
            cmd = command + " " + args.next_line();
            break;
        }
        else {
            break;
        }
    }
}

string c::get_home_gov() const {
    return get_home() + "/gov";
}

string c::get_home_gov_rpc_client() const {
    return get_home_gov() + "/rpc_client";
}

string c::get_status_file() const {
    if (!status_file) return "";
    return get_home_gov() + "/status";
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
        if (verbose)
            os << pfx << "writing log to cout\n";
        else
            os << pfx << "warning: writing log files in " << LOGDIR << '\n';
    #else
        os << pfx << "logs: disabled.\n";
    #endif
    if (daemon) {
        os << pfx << "run daemon\n";
        os << pfx << "  listening port: " << port << '\n';
        os << pfx << "  published listening port: " << pport << '\n';
        os << pfx << "  network topology: " << TOPOLOGY << '\n';
        os << pfx << "  max node edges: " << (int)edges << '\n';
        os << pfx << "  max wallet edges: " << (int)devices << '\n';
//        os << pfx << "  protocol is " << (daemon_encrypt_protocol ? "" : "not ") << "encrypted\n";
        os << pfx << "  workers: " << (int)workers << '\n';
        os << pfx << "  sysop shell: " << boolalpha << shell << '\n';
    }
    else {
        os << pfx << "run rpc shell\n";
        os << pfx << "  host: " << sysophost << ":" << port << '\n';
        os << pfx << "  connect_for_recv " << rpc__connect_for_recv << '\n';
        os << pfx << "  stop_on_disconnection " << rpc__stop_on_disconnection << '\n';
    }
}

void c::connecting_to(ostream& os) const {
    os << PLATFORM << "-gov daemon at " << sysophost << ':' << port;
}

