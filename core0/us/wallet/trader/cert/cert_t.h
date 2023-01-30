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
#include "doc_t.h"
#include "expiry_doc_t.h"
#include "signed_doc_t.h"

namespace us::wallet::trader::cert {

    using magic_t = doc0_t::magic_t;

    struct cert_traits {
        static constexpr magic_t magic{10};
        static constexpr auto name{"cert"};
        static constexpr auto long_name{"Certificate"};
        static constexpr auto long_name_es{"Certificado"};
    };

    using cert_t = doc_t<signed_expiry_doc_t, cert_traits>;

}

