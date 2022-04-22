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
#include <us/gov/io/seriable_map.h>

namespace us { namespace gov { namespace cash {

    struct m_t: io::seriable_map<string, string> {
        m_t();
        m_t(const m_t&);
        m_t& operator = (const m_t&) = delete;
        void dump(const string& prefix, ostream&) const;
        void hash_data_to_sign(sigmsg_hasher_t&) const;
        void hash_data(crypto::ripemd160&) const;
        void merge(const m_t&);

    };

}}}

