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
#include <type_traits>

#include "seriable.h"
#include "blob_reader_t.h"
#include "blob_writer_t.h"

namespace us::gov::io {

    using namespace std;

    template<typename t>
    struct seriable_vector: vector<t>, virtual seriable {
        using b = vector<t>;
        using b::b;
        inline size_t blob_size() const override { return blob_writer_t::blob_size(static_cast<const vector<t>&>(*this)); }
        inline void to_blob(blob_writer_t& writer) const override { writer.write(static_cast<const vector<t>&>(*this)); }
        inline ko from_blob(blob_reader_t& reader) override { return reader.read(static_cast<vector<t>&>(*this)); }
    };

    static_assert(std::is_convertible<seriable_vector<string>*, io::writable*>::value, "KO 43822");

}

