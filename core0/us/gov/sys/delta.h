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
#include <us/gov/types.h>
#include "sensors_t.h"
#include "local_delta.h"

namespace us { namespace gov { namespace sys {

    struct delta: engine::app::delta {
        using b = engine::app::delta;

        struct merger final: sensors_t {
            void merge(const sensor_t&);
        };

        delta();
        delta(const delta&) = delete;
        delta& operator = (const delta&) = delete;
        ~delta() override;

        uint64_t merge(engine::app::local_delta*) override;
        void end_merge() override;

    public:
        void dump(const string& prefix, ostream&) const override;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:

        local_delta g;
        merger m;
    };

}}}

