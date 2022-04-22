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
#include <mutex>
#include <us/wallet/trader/olog_t.h>
#include <us/gov/likely.h>
#include <us/gov/config.h>

namespace {
    using namespace std;
    using olog_t = us::wallet::trader::olog_t;

    template<typename t>
    void dologx(const olog_t& i, const t& arg) {
        assert(i.logos != nullptr);
        *i.logos << arg << '\n';
    }

    template<typename type1, typename... types>
    void dologx(const olog_t& i, const type1& arg1, const types& ... args) {
        assert(i.logos != nullptr);
        *i.logos << arg1 << ' ';
        dologx(i, args...);
    }

    template<typename... Args>
    void ologx(const olog_t& i, const Args&... args) {
        #if CFG_LOGS == 1
            log("olog", args...);
        #endif
        lock_guard<mutex> lock(i.mx_logos);
        if (unlikely(i.logos == nullptr)) {
            log("KO 50349 olog not ready");
            return;
        }
        *i.logos << i.ts() << ' ';
        dologx(i, args...);
        assert(!i.logos->fail());
        i.logos->flush();
    }
}

