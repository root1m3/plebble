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
#include <us/gov/config.h>
#include <us/gov/types.h>
#include <us/gov/engine/app.h>

#include "sensor_t.h"

namespace us::gov::sys {

    struct local_delta: engine::app::local_delta {
        using b = engine::app::local_delta;

        local_delta& operator = (const local_delta& other) { id = other.id; sensor = other.sensor; return *this; }
        local_delta& operator = (int zero) = delete;
        local_delta();
        local_delta(const local_delta& other): sensor(other.sensor), id(other.id) {}

        bool operator == (const local_delta&) const = delete;
        ~local_delta() override;

        appid_t app_id() const override;
        void hash_data_to_sign(sigmsg_hasher_t&) const override;
        void hash_data(hasher_t&) const override;
        const hash_t& get_hash() const;
        hash_t compute_hash() const;

    public:
        void dump(const string& prefix, ostream&) const override;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        hash_t id;
        sensor_t sensor;
        mutable hash_t hash{0};
    };

}

namespace std {
    template <> struct hash<us::gov::sys::local_delta> {
        size_t operator()(const us::gov::sys::local_delta&) const;
    };
}

