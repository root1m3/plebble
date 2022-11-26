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

#include <mutex>
#include <string>

#include <us/gov/config.h>
#include <us/gov/ko.h>
#include <us/wallet/trader/bookmarks_t.h>

#include "types.h"

namespace us::wallet::wallet {

    struct bookmarksman_t final: us::wallet::trader::bookmarks_t {
        using b = us::wallet::trader::bookmarks_t;
        using bookmark_t = us::wallet::trader::bookmark_t;

        bookmarksman_t();
        ko init(const string& home);
        ko load();
        ko save();
        ko load_();
        ko save_();
        blob_t blob() const;
        ko add(const string& name, bookmark_t&&);
        ko remove(const string& key);

        string home;
        mutable mutex mx;
    };

}

