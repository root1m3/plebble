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
#include "trader_protocol.h"

#include <set>

#include "business.h"
#include "protocol_selection_t.h"
#include "bootstrap/protocols_t.h"

namespace us::wallet::trader {

    struct lib0_t: set<business_t*> {

        lib0_t();
        virtual ~lib0_t();

        virtual void cleanup();

        virtual pair<ko, business_t*> create_business() = 0;  //returns weak ptr
        virtual ko delete_business(business_t*);
        virtual ko good() const = 0;

    };

    struct trades_t;

    struct libs_t final: map<protocol_selection_t, lib0_t*> {
        using protocols_t = us::wallet::trader::bootstrap::protocols_t;

        libs_t(trades_t&);
        libs_t(const libs_t&) = delete;
        ~libs_t();

        pair<ko, business_t*> create_business(const protocol_selection_t&);
        ko delete_business(business_t*);
        protocols_t available_protocol_selections() const;
        void load();
        static protocol_selection_t extract_protocol_selection(const string& filename);

    private:
        void load_bank_factory();
        void load_plugins();

        trades_t& parent;
    };

}

