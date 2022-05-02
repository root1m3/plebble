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
#include <us/wallet/trader/workflow/expiry_doc_t.h>
#include <us/wallet/trader/workflow/signed_doc_t.h>

namespace us { namespace trader { namespace workflow { namespace consumer {
    using magic_t = us::wallet::trader::workflow::doc0_t::magic_t;

    struct catalogue_traits {
        static constexpr magic_t magic{288};
        static constexpr auto name{"cat"};
        static constexpr auto long_name{"Catalogue"};
        static constexpr auto long_name_es{"Catalogo"};
    };
    using catalogue_t = us::wallet::trader::workflow::doc_t<us::wallet::trader::workflow::expiry_doc_t, catalogue_traits>;

    struct invoice_traits {
        static constexpr magic_t magic{290};
        static constexpr auto name{"inv"};
        static constexpr auto long_name{"Invoice"};
        static constexpr auto long_name_es{"Factura"};
    };
    using invoice_t = us::wallet::trader::workflow::doc_t<us::wallet::trader::workflow::signed_doc_t, invoice_traits>;

    struct payment_traits {
        static constexpr magic_t magic{291};
        static constexpr auto name{"pay"};
        static constexpr auto long_name{"Payment"};
        static constexpr auto long_name_es{"Pago"};
    };
    using payment_t = us::wallet::trader::workflow::doc_t<us::wallet::trader::workflow::signed_doc_t, payment_traits>;

    struct receipt_traits {
        static constexpr magic_t magic{292};
        static constexpr auto name{"rcpt"};
        static constexpr auto long_name{"Receipt"};
        static constexpr auto long_name_es{"Recibo"};
    };
    using receipt_t = us::wallet::trader::workflow::doc_t<us::wallet::trader::workflow::signed_doc_t, receipt_traits>;

}}}}

