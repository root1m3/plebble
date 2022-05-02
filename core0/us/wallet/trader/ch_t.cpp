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
#include "ch_t.h"
#include "params_t.h"

#define loglevel "trader"
#define logclass "ch_t"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::ch_t;

c::ch_t(int n): local_params(nullptr), lock(nullptr) {
    log("new ch_t");

}

c::ch_t(local_params_t& lp, mutex& mx): local_params(&lp), lock(new unique_lock(mx)) {
    log("new ch_t with ref to locked local_params");
}

c::~ch_t() {
    log("destroyed ch_t");
    delete lock;
}

void c::open(local_params_t& lp, mutex& mx) {
    assert(lock == nullptr);
    local_params = &lp;
    lock = new unique_lock(mx);
}

void c::close() {
    delete lock;
    lock = nullptr;
    local_params = nullptr;
}

string c::to_string() const {
    ostringstream os;
    os << "priv: " << priv_params_changed << "; shared_params: " << shared_params_changed << "; personality: " << personality_changed << "; update devices: " << update_devices;
    return os.str();
}

c& c::operator |= (const ch_t& other) {
    shared_params_changed |= other.shared_params_changed;
    personality_changed |= other.personality_changed;
    priv_params_changed |= other.priv_params_changed;
    if (other.update_devices > update_devices) {
        update_devices = other.update_devices;
    }
    return *this;
}

void c::always_update_devices_() {
    if (update_devices == 2) return;
    log("force push_data. update_devices:", update_devices, " -> 2");
    update_devices = 2;
}

void c::to_stream(ostream& os) const {
    assert(lock == nullptr);
    os << priv_params_changed << ' ' << shared_params_changed << ' ' << personality_changed << ' ' << update_devices << ' ';
}

void c::file_updated(const string& path, const string& name) {
    updated_files.emplace_back(path, name);
}

bool c::need_update_devices() const {
    bool r = true;
    switch (update_devices) {
        case 0: {
            r = false;
        }
        break;
        case 1: {
            r = any();
        }
        break;
    }
    log("need_update_devices", r);
    return r;
}

ko c::from_stream(istream& is) {
    assert(lock == nullptr);
    is >> priv_params_changed;
    is >> shared_params_changed;
    is >> personality_changed;
    is >> update_devices;
    if (is.fail()) {
        auto r = "KO 68878";
        log(r);
        return r;
    }
    return ok;
}

