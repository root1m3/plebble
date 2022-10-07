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
#include <us/wallet/trader/workflow/doc_t.h>
#include <us/wallet/trader/workflow/signed_doc_t.h>
#include <us/wallet/trader/workflow/expiry_doc_t.h>

namespace us::trader::workflow::organization {

    using magic_t = us::wallet::trader::workflow::doc0_t::magic_t;

    struct appointment_traits {
        static constexpr magic_t magic{400};
        static constexpr auto name{"app"};
        static constexpr auto long_name{"Appointment"};
        static constexpr auto long_name_es{"Cita"};
    };
    using appointment_t = us::wallet::trader::workflow::doc_t<us::wallet::trader::workflow::signed_expiry_doc_t, appointment_traits>;

    struct reference_traits {
        static constexpr magic_t magic{401};
        static constexpr auto name{"ref"};
        static constexpr auto long_name{"Reference"};
        static constexpr auto long_name_es{"Referencia"};
    };
    using reference_t = us::wallet::trader::workflow::doc_t<us::wallet::trader::workflow::doc0_t, reference_traits>;

}

