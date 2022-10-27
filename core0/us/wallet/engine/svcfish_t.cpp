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
#include "svcfish_t.h"

#include <us/gov/config.h>

#define loglevel "wallet/engine"
#define logclass "svcfish_t"
#include <us/gov/logs.inc>

using namespace us::wallet::engine;
using namespace us;

using c = us::wallet::engine::svcfish_t;

svc_t c::from_prev(svc_t svc) const {
    auto i = db01.find(svc);
    if (i == db01.end()) {
        return svc;
    }
    return i->second;
}

us::svc_t c::to_prev(svc_t svc) const {
    auto i = db10.find(svc);
    if (i == db10.end()) {
        return svc;
    }
    return i->second;
}

