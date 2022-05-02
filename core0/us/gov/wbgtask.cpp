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
#include "bgtask.h"
#include <functional>

#define loglevel "gov"
#define logclass "wbgtask"
#include <us/gov/logs.inc>

#if CFG_LOGS__GOV_CONCUR == 0
    #if CFG_LOGS == 1
        #undef log
        #define log (void)sizeof
    #endif
#endif

using namespace us::gov;
using c = us::gov::wbgtask;
using namespace chrono;

c::wbgtask(function<void ()> onwakeup): b(bind(&c::run, this), onwakeup) {
}

c::~wbgtask() {
}

void c::run() {
    task_init();
    while (isup()) {
        task();
        collect_();
    }
    task_cleanup();
}

void c::collect_() {
    if (reset_wait()) {
        return;
    }
    collect();
    reset_wait();
    return;
}


