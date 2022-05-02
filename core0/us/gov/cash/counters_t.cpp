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
#include "counters_t.h"
#if CFG_COUNTERS == 1

#define loglevel "gov/cash"
#define logclass "counters_t"
#include <us/gov/logs.inc>

using namespace std;
using namespace us::gov;
using namespace us::gov::cash;
using c = us::gov::cash::counters_t;
using us::ko;

void c::dump(ostream& os) const {
    os << "tx_rejected_bad_amounts " << tx_rejected_bad_amounts << '\n';
    os << "tx_rejected_state " << tx_rejected_state << '\n';
    os << "tx_rejected_could_not_unlock_input " << tx_rejected_could_not_unlock_input << '\n';
    os << "tx_rejected_locking_program_mismatch " << tx_rejected_locking_program_mismatch << '\n';
    os << "tx_processed " << tx_processed << '\n';
}

#endif

