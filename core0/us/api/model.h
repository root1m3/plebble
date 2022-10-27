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
#include <vector>
#include <string>
#include <string_view>
#include <map>
#include <tuple>

#include "api_t.h"

namespace us::apitool {

    using namespace std;

    struct api_t;
    struct coder;

    struct model final: vector<api_t*> {
        using netsvc_t = api_t::netsvc_t;
        using svcfish_entry_t = api_t::svcfish_entry_t;
        using svcfish_db_t = apifun::svcfish_db_t;

        model(const string& process);
        ~model();

        int add(const string& name, int svc0);
        int add_delegate(const string& name, int svc0);
        void write_netsvc(ostream&);
        void load_netsvc_run();
        void end_adding();

    public:
        string process;


        netsvc_t netsvc;

        uint8_t netsvc_serial{0};
        uint8_t prevserial{0};

        //op svc_src svc_dest comment 
        svcfish_db_t svcfish;
        svcfish_db_t svcfish_inv;
    };

}

