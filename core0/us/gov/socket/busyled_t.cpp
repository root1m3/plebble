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
#include "busyled_t.h"
#include <us/gov/likely.h>

#define loglevel "gov/socket"
#define logclass "busyled_t"
#include "logs.inc"

using namespace us::gov::socket;
using c = us::gov::socket::busyled_t;

c::busyled_t() {
}

c::~busyled_t() {
    delete handler;
}

void c::set() {
    if (unlikely(handler == nullptr)) return;
    log("busyled set");
    if (ref.fetch_add(1) != 0) return;
    log("trigger on_busy");
    handler->on_busy();
}

void c::reset() {
    if (unlikely(handler == nullptr)) return;
    log("busyled reset");
    if (ref.fetch_sub(1) != 1) return;
    log("trigger on_idle");
    handler->on_idle();
}

void c::set_handler(handler_t* h) {
    delete handler;
    handler = h;
}

