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
#include <unordered_map>
#include <map>
#include <sstream>
#include <mutex>
#include <tuple>
#include <chrono>
#include <fstream>

#include <us/gov/io/seriable.h>
#include <us/gov/crypto/ripemd160.h>
#include <us/gov/cash/tx_t.h>
#include <us/gov/types.h>

#include <us/wallet/trader/business.h>
#include <us/wallet/trader/ch_t.h>

namespace us::trader::workflow::consumer {

    struct basket_item_t: pair<cash_t, cash_t>, virtual us::gov::io::seriable { //price, reward
        inline cash_t sale_price_per_unit() const { return first; }
        inline cash_t reward_price_per_unit() const { return second; }

        bool operator == (const basket_item_t& other) const;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    };

}

