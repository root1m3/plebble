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
#pragma once
#include <string>
#include <us/gov/cash/tx_t.h>
#include <us/gov/io/seriable.h>
#include <memory>

namespace us { namespace wallet { namespace trader { namespace r2r { namespace w2w {

    using tx_t = us::gov::cash::tx_t;
    using track_t = ts_t;

    struct txlog_item_t {
        txlog_item_t(const string& io_summary, uint16_t state_id, const string& state, tx_t* inv, tx_t* pay);

        bool req_pay() const { return !pay; }
        string title() const;

    public:
        unique_ptr<tx_t> inv;
        unique_ptr<tx_t> pay;
        string io_summary;
        uint16_t state_id{0};
        string state;
    };


}}}}}

