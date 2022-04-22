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

#include <us/gov/io/seriable_map.h>
#include <us/gov/crypto/types.h>

#include "basket_item.h"

namespace us { namespace trader { namespace workflow { namespace consumer {

    using hash_t = us::gov::crypto::ripemd160::value_type;

    struct cat_t: us::gov::io::seriable_unordered_map<hash_t, basket_item_t> {
        using ch_t = us::wallet::trader::ch_t;

        cat_t();
        ~cat_t() override;

        void dump(ostream&) const;

        static string fields();

    };

}}}}

