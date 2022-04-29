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
#include <mutex>
#include <set>
#include <us/gov/types.h>
#include <us/gov/crypto/types.h>
#include "types.h"

namespace us::gov::engine {

    struct daemon_t;

    struct neuralyzer_t final {

        neuralyzer_t(daemon_t*);
        neuralyzer_t(const neuralyzer_t&) = delete;
        neuralyzer_t& operator = (const neuralyzer_t&) = delete;
        ~neuralyzer_t();

        void old_chain(const hash_t&);
        void get_blocks(set<hash_t>&, const hash_t&);
        int cleanup();

    private:
        void delete_chain(const hash_t&);

    public:
        daemon_t *d;
        hash_t old;
        mutex mx;
    };

}

