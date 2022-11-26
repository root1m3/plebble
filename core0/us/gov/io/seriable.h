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

#include "writable.h"
#include "readable.h"
//#include "blob_reader_t.h"
//#include "blob_writer_t.h"
#include "types.h"

namespace us::gov::io {

    struct blob_writer_t;
    struct blob_reader_t;

    struct seriable: readable, writable {
        using blob_writer_t = us::gov::io::blob_writer_t;
        using blob_reader_t = us::gov::io::blob_reader_t;
        using serid_t = readable::serid_t;

        using writable::serial_id;
    };

    static_assert(std::is_convertible<seriable*, writable*>::value, "KO 74822");
    static_assert(std::is_convertible<seriable*, readable*>::value, "KO 74823");


}

