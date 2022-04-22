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
#include <us/gov/cli/hmi.h>
#include <us/gov/vcs.h>
#include <us/gov/io/screen.h>

#define loglevel "govx"
#define logclass "main"
#include <us/gov/logs.inc>

using namespace us::gov;
using namespace std;
using us::ko;
using us::ok;

struct hmi_t: us::gov::cli::hmi {
    using b = us::gov::cli::hmi;
    hmi_t(int argc, char** argv, ostream& os): b(argc, argv, os) {}
    void setup_signals(bool) override;
};

hmi_t* hmi{nullptr};

using us::gov::io::screen;

void sig_handler(int s) {
    {
        screen::lock_t lock(hmi->scr, true);
        lock.os << "main: received signal " << s << '\n';
        lock.os << "stopping ...\n";
    }
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
        signal(SIGINT,SIG_DFL);
        signal(SIGTERM,SIG_DFL);
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

