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
#include "workflow_t.h"
#include "docs.h"

#define loglevel "trader/workflow/organization"
#define logclass "workflow_t"
#include <us/gov/logs.inc>

using namespace us::trader::workflow::organization;
using c = us::trader::workflow::organization::workflow_t;

c::workflow_t() {
}

c::~workflow_t() {
}

c::bitem* c::enable_appointment(bool b, ch_t& ch) {
    return b ? add<appointment_t>(ch) : remove<appointment_t>(ch);
}


c::bitem* c::enable_reference(bool b, ch_t& ch) {
    return b ? add<reference_t>(ch) : remove<reference_t>(ch);
}

