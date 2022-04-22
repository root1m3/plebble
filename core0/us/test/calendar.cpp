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
#include <fstream>

#include <us/gov/config.h>
#include <us/gov/engine/calendar.h>

#include "test_platform.h"

#define loglevel "test"
#define logclass "calendar"
#include "logs.inc"

#include "assert.inc"

namespace us { namespace test {

using namespace std;
using namespace chrono;
using namespace chrono_literals;


struct calendar: us::gov::engine::calendar_t, us::test::test_platform {
    typedef us::test::test_platform t;

    calendar(ostream&os):t(os) {
    }

    void self_test() {
        os << "from " << last_processed << " to " << block_closure << endl;
        uint64_t expected=duration_cast<nanoseconds>(cycle_period).count()+1;
        check(block_closure-last_processed, expected);

        check(has(0), false);

    }

};

int main_calendar() {

    calendar c(cout);
    c.self_test();
    return 0;
}

}}

