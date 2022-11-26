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
#include "bookmarksman_t.h"

#include <us/gov/io/cfg0.h>

#include "types.h"

#define loglevel "wallet/wallet"
#define logclass "bookmarksman_t"
#include <us/gov/logs.inc>

using namespace us::wallet::wallet;
using c = us::wallet::wallet::bookmarksman_t;

c::bookmarksman_t() {
}

ko c::init(const string& h) {
    home = h;
    if (!us::gov::io::cfg0::ensure_dir(home)) {
        auto r = "KO 10882";
        log(r);
        return r;
    }
    return ok;
}

ko c::load() {
    lock_guard<mutex> lock(mx);
    return load_();
}

ko c::save() {
    lock_guard<mutex> lock(mx);
    return save_();
}

ko c::load_() {
    log("loading bookmarks from", home + "/bookmarks");
    blob_t blob;
    auto r = us::gov::io::read_file_(home + "/bookmarks", blob);
    if (is_ko(r)) {
        return r;
    }
    clear();
    return read(blob);
}

ko c::save_() {
    log("saving bookmarks to", home + "/bookmarks");
    blob_t blob;
    write(blob);
    ofstream os(home + "/bookmarks");
    os.write((const char*)blob.data(), blob.size());
    if (os.fail()) {
        auto r = "KO 60127 Could not write bookmarks.";
        log(r);
        return r;
    }
    return ok;
}

blob_t c::blob() const {
    log("bookmarksman_t::blob");
    lock_guard<mutex> lock(mx);
    blob_t blob;
    write(blob);
    log("bookmarksman_t::blob returned", blob.size(), "bytes");
    return blob;
}

ko c::add(const string& name, bookmark_t&& bm) {
    lock_guard<mutex> lock(mx);
    auto r = b::add(name, move(bm));
    if (is_ko(r)) {
        return r;
    }
    return save_();
}

ko c::remove(const string& key) {
    lock_guard<mutex> lock(mx);
    auto r = b::remove(key);
    if (is_ko(r)) {
        return r;
    }
    return save_();
}


