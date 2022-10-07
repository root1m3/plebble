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
extern "C" {

    c* uswallet_business_create(int if_version) {
        if (if_version != us::wallet::trader::business_t::interface_version) {
            auto r = "KO 43731 Interfaces mismatch.";
            log(r, "Requested interface", if_version, "but I expose version", us::wallet::trader::business_t::interface_version);
            return nullptr;
        }
        return new c();
    }

    void uswallet_business_destroy(c* p) {
        log("destroy business");
        delete p;
    }

}

