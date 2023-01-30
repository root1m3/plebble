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
#include <chrono>

#include <us/gov/crypto/base58.h>
#include <us/wallet/trader/params_t.h>
#include <us/wallet/wallet/local_api.h>

#include "doc_t.h"

#define loglevel "wallet/trader/cert"
#define logclass "doc_t"
#include <us/gov/logs.inc>

using namespace us::wallet::trader::cert;
//using c = us::wallet::trader::cert::doc_t;

ko us::wallet::trader::cert::wallet_push_OK__(wallet::local_api& w, const string& s) {
    return w.push_OK(hash_t(0), s);
}

