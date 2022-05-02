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

#include <us/gov/likely.h>

#include "blob_writer_t.h"
#include "blob_reader_t.h"
#include "types.h"

namespace us { namespace gov { namespace io {

    struct seriable: blob_reader_t::readable, blob_writer_t::writable {
        using blob_writer_t = us::gov::io::blob_writer_t;
        using blob_reader_t = us::gov::io::blob_reader_t;

        using blob_writer_t::writable::serial_id;
    };

}}}

