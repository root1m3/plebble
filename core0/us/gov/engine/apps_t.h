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
#include "app.h"
#include <map>
#include <iostream>
#include <us/gov/io/seriable.h>

namespace us::gov::engine {

    struct apps_t final: map<appid_t, app*>, virtual io::seriable {
        ~apps_t();
        void dump(const string& prefix, ostream&) const;
        void full_dump(const string& prefix, int detail, ostream&) const;
        ko shell_command(appid_t, istream&, ostream&);
        void process(const evidence&);

    public:
        ko from_blob(blob_reader_t&) override;
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
    };

}

