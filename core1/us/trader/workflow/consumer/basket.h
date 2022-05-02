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

#include <us/gov/io/seriable_map.h>
#include <us/gov/crypto/ripemd160.h>
#include <us/gov/cash/tx_t.h>
#include <us/gov/types.h>

#include <us/wallet/trader/business.h>
#include <us/wallet/trader/ch_t.h>

#include "basket_item.h"
#include "cat.h"

namespace us { namespace trader { namespace workflow { namespace consumer {

    using hash_t = us::gov::crypto::ripemd160::value_type;
    using vol_t = int32_t;
    using serial_t = uint64_t;

    struct basket_t: us::gov::io::seriable_unordered_map<hash_t, pair<vol_t, basket_item_t>> {
        using b = us::gov::io::seriable_unordered_map<hash_t, pair<vol_t, basket_item_t>>;
        using hash_value_t = tuple<hash_t, cash_t, cash_t>;
        using ch_t = us::wallet::trader::ch_t;
        using prod_t = hash_t;

        basket_t();
        ~basket_t() override;
        using item_t = basket_item_t;

        void addx(const prod_t&, vol_t, const cash_t& price, const cash_t& reward, const string& itemdesc);
        void dump(ostream&) const;
        void dump(const string& pfx, ostream&) const;
        void dump(const string& pfx, const string& pfxitems, ostream&) const;
        void incr_serial(const basket_t& basket_ch, ch_t& ch);
        pair<ko, vol_t> substract_(const prod_t&, vol_t, const basket_item_t&, ch_t&);
        void add_(const prod_t&, vol_t, const basket_item_t&, ch_t&);
        void set(ch_t&);
        pair<cash_t, cash_t> value() const;
        hash_value_t hash_value() const;
        cat_t cat() const;
        static pair<vol_t, basket_item_t> transfer(basket_t& src, basket_t& dst, const hash_t& prod, vol_t, const basket_t& basket_ch, ch_t&); //returns the number and a copy of the item
        static string fields();

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        string name;
        serial_t serial{0};

    private:
        mutable mutex mx;
    };

}}}}

