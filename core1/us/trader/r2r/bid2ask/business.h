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
#pragma once
#include <string>
#include <map>
#include <sstream>
#include <mutex>
#include <chrono>
#include <fstream>
#include <unordered_map>

#include <us/gov/cash/tx_t.h>
#include <us/gov/io/factory.h>

#include <us/wallet/trader/workflow/business.h>
#include <us/wallet/trader/ch_t.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/wallet/local_api.h>
#include <us/wallet/trader/workflow/workflow_t.h>

#include <us/trader/r2r/bid2ask/types.h>
#include <us/trader/workflow/consumer/docs.h>
#include <us/trader/workflow/consumer/basket.h>

#include "protocol.h"

namespace us::trader::r2r::bid2ask {

    struct business_t: us::wallet::trader::workflow::business_t {
        using b = us::wallet::trader::workflow::business_t;
        using ch_t = us::wallet::trader::ch_t;
        using catalogue_t = us::trader::workflow::consumer::catalogue_t;
        using basket_t = us::trader::workflow::consumer::basket_t;
        using tx_t = us::gov::cash::tx_t;
        using workflow_t = us::wallet::trader::workflow::workflow_t;

    public:
        business_t();
        ~business_t() override;

    public:

        bool invert(protocol_selection_t&) const override;

    };

}

