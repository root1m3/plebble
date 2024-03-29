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
#include <us/wallet/trader/cert/doc0_t.h>
#include <us/wallet/trader/cert/doc_t.h>
#include <us/wallet/trader/cert/signed_doc_t.h>

namespace us::trader::workflow::logistics {

    using magic_t = us::wallet::trader::cert::doc0_t::magic_t;

    struct parcel_traits {
        static constexpr magic_t magic{500};
        static constexpr auto name{"cat"};
        static constexpr auto long_name{"Catalogue"};
        static constexpr auto long_name_es{"Catalogo"};
    };
    using parcel_t = us::wallet::trader::cert::doc_t<us::wallet::trader::cert::signed_doc_t, parcel_traits>;

    struct shipping_receipt_traits {
        static constexpr magic_t magic{501};
        static constexpr auto name{"cat"};
        static constexpr auto long_name{"Catalogue"};
        static constexpr auto long_name_es{"Catalogo"};
    };
    using shipping_receipt_t = us::wallet::trader::cert::doc_t<us::wallet::trader::cert::signed_doc_t, shipping_receipt_traits>;

}

