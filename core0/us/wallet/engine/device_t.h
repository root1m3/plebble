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
#include "types.h"

namespace us { namespace wallet { namespace engine {

    struct device_t {

        static string default_name;

        device_t() {}
        device_t(const pub_t& pub, const string& subhome, const string& name): pub(pub), name(name), subhome(subhome) {}
        device_t(const string& subhome, const string& name): name(name), subhome(subhome) {}
        device_t(const device_t& other): pub(other.pub), name(other.name), subhome(other.subhome) {}

        void to_stream(ostream&) const;
        static pair<bool, device_t> from_stream(istream&);
        static pair<bool, device_t> from_stream_prev(istream&);
        static pair<bool, device_t> from_stream_v1(istream&);
        void dump(ostream&) const;
        uint16_t decode_pin() const;
        void encode_pin(pin_t);

    public:
        string name;
        pub_t pub;
        string subhome;
    };

}}}

