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
#include <iostream>
#include <us/trader/test/main.h>

#define loglevel "trader/test"
#define logclass "main"
#include <us/gov/logs.inc>

using namespace std;

int main(int argc, char** argv) {
    cout << "START" << endl;
    us::dbg::thread_logger::set_root_logdir("logs");
    log_pstart("us-trader_test-c1");
    log_start("", "main");

    log_info(cout);
    cout << endl;

    return core1_main(argc, argv);
}

