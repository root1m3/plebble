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
#include "seriable.h"
#include <map>
#include <unordered_map>

namespace us { namespace gov { namespace io {

    template<typename k, typename v>
    struct seriable_map: map<k, v>, virtual seriable {
        using b = map<k, v>;
        using b::map;
        inline size_t blob_size() const override { return blob_writer_t::blob_size(static_cast<const map<k, v>&>(*this)); }
        inline void to_blob(blob_writer_t& writer) const override { writer.write(static_cast<const map<k, v>&>(*this)); }
        inline ko from_blob(blob_reader_t& reader) override { return reader.read(static_cast<map<k, v>&>(*this)); }
    };

    template<typename k, typename v>
    struct seriable_unordered_map: unordered_map<k, v>, virtual seriable {
        using b = unordered_map<k, v>;
        using b::unordered_map;
        inline size_t blob_size() const override { return blob_writer_t::blob_size(static_cast<const unordered_map<k, v>&>(*this)); }
        inline void to_blob(blob_writer_t& writer) const override { writer.write(static_cast<const unordered_map<k, v>&>(*this)); }
        inline ko from_blob(blob_reader_t& reader) override { return reader.read(static_cast<unordered_map<k, v>&>(*this)); }
    };

}}}

