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
#include <vector>
#include <iostream>

#include <us/gov/ko.h>
#include <us/gov/io/seriable_vector.h>

namespace us { namespace wallet { namespace trader { namespace bootstrap {

    struct protocols_t: us::gov::io::seriable_vector<pair<string, string>> {
        using item = pair<string, string>;
        using b = vector<item>;

        static protocols_t filter_common(protocols_t, protocols_t);

        void dump(ostream&) const;

    };

}}}}


