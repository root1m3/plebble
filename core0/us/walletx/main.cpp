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
#include <us/gov/config.h>
#include <us/gov/ko.h>
#include <us/gov/io/shell_args.h>
#include <us/gov/io/screen.h>
#include <us/wallet/cli/params.h>
#include <us/wallet/cli/hmi.h>

#define loglevel "walletx"
#define logclass "main"
#include <us/gov/logs.inc>

using namespace std;
using namespace us;
using us::gov::io::shell_args;
using us::wallet::cli::params;
using us::gov::io::screen;

struct hmi_t: us::wallet::cli::hmi {
    using b = us::wallet::cli::hmi;
    hmi_t(int argc, char** argv, ostream& os): b(argc, argv, os) {}
    void setup_signals(bool) override;
};

hmi_t* hmi{nullptr};

void sig_handler(int s) {
    cout << "main: received signal " << s << endl;
    cout << "stopping ..." << endl;
    hmi->stop();
    hmi->setup_signals(false);
}

void hmi_t::setup_signals(bool on) {
    b::setup_signals(on);
    if (on) {
        signal(SIGINT, sig_handler);
        signal(SIGTERM, sig_handler);
    }
    else {
        signal(SIGINT, SIG_DFL);
        signal(SIGTERM, SIG_DFL);
    }
}

int main(int argc, char** argv) {
    log_pstart(argv[0]);
    log_start("", "main");
    hmi = new hmi_t(argc, argv, cout);
    string r = hmi->run();
    log("end");
    if (hmi->p.daemon) {
        screen::lock_t lock(hmi->scr, false);
        lock.os << "main: exited " << (r.empty() ? r : "normally") << '\n';
    }
    delete hmi;
    hmi = nullptr;
    hmi_t::process_cleanup();
    return r.empty() ? 0 : 1;
}


