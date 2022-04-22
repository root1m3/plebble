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
#include <us/gov/engine/app.h>
#include "accounts_t.h"

namespace us { namespace gov { namespace cash {

    struct local_delta final: engine::app::local_delta {
        using b = engine::app::local_delta;

    public:
        local_delta();
        local_delta(const local_delta&);
        ~local_delta() override;
        bool operator == (const local_delta&) const;
        bool operator != (const local_delta&) const;
        local_delta& operator = (int zero);

    public:
        const hash_t& get_hash() const;
        hash_t compute_hash() const;

    public: /// engine
        appid_t app_id() const override;
        void hash_data_to_sign(sigmsg_hasher_t&) const override;
        void hash_data(hasher_t&) const override;

    public:
        void dump(const string& prefix, ostream&) const override;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        accounts_t accounts;
        cash_t fees{0};
        mutable hash_t hash{0};
    };

}}}

namespace std {
    template <>
    struct hash<us::gov::cash::local_delta> {
        size_t operator()(const us::gov::cash::local_delta&) const;
    };
}

