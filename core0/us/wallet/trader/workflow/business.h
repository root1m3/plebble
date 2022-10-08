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

#include <us/gov/io/factory.h>

#include <us/wallet/trader/business.h>
#include <us/wallet/trader/ch_t.h>
#include <us/wallet/trader/trader_t.h>

#include "workflow_t.h"
#include "item_t.h"

namespace us::wallet::trader::workflow {

    struct business_t: us::wallet::trader::business_t {
        using b = us::wallet::trader::business_t;
        using ch_t = us::wallet::trader::ch_t;

    public:
        business_t();
        ~business_t() override;

    };

}
