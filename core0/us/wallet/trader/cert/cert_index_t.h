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
#include <map>
#include <string>
#include <mutex>
#include <functional>

#include <us/gov/config.h>
#include <us/gov/io/seriable_map.h>
#include <us/gov/io/blob_reader_t.h>

#include "cert_t.h"

namespace us::wallet::trader::cert {

    struct cert_index_t: us::gov::io::seriable_map<hash_t, string> {
        using serid_t = us::gov::io::blob_reader_t::serid_t;

        void dump(ostream&) const;

        serid_t serial_id() const override { return 'C'; }
    };

}

