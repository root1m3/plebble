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
#include "peer_t.h"
#include <sstream>
#include "types.h"

#include <us/gov/socket/datagram.h>
#include <us/wallet/engine/daemon_t.h>

#define loglevel "wallet/engine"
#define logclass "peer_t__pairing"
#include <us/gov/logs.inc>

using namespace us::wallet::engine;
using c = us::wallet::engine::peer_t;

#include <us/api/generated/c++/wallet/pairing/cllr_rpc-impl>

bool c::process_work__pairing(datagram* d) {
    using namespace protocol;
    switch(d->service) {
        #include <us/api/generated/c++/wallet/pairing/hdlr_svc-router>
    }
    return false;
}

#include <us/api/generated/c++/wallet/pairing/hdlr_svc_handler-impl>

//------------------apitool - API Spec defined @ us/api/generated/c++/wallet/pairing/hdlr_local-impl

ko c::handle_pair_device(pair_device_in_dst_t&& o_in, string& ans) {
    log("pair_device");
    /// in:
    ///     pub_t pub;
    ///     string subhome;
    ///     string name;

    if (!o_in.pub.valid) {
        auto r = "KO 10862 Invalid public key.";
        log(r);
        return r;
    }
    auto& demon = static_cast<daemon_t&>(daemon);
    {
        auto r = demon.devices.device_pair(o_in.pub, o_in.subhome, o_in.name);
        if (is_ko(r)) {
            return r;
        }
    }
    ans = "done.";
    return ok;
}

ko c::handle_unpair_device(pub_t&& pub, string& ans) {
    log("unpair_device");
    if (!pub.valid) {
        auto r = "KO 10862 Invalid public key.";
        log(r);
        return r;
    }
    auto& demon = static_cast<daemon_t&>(daemon);
    {
        auto r = demon.devices.device_unpair(pub);
        if (is_ko(r)) {
            return r;
        }
    }
    demon.disconnect(pub, 0, "Revoked authorization.");
    ans = "done.";
    return ok;
}

ko c::handle_list_devices(string& data) {
    log("list_devices");
    auto& demon = static_cast<daemon_t&>(daemon);
    ostringstream os;
    demon.devices.dump(os);
    data = os.str();
    return ok;
}

ko c::handle_attempts(vector<string>& data) {
    log("attempts");
    auto& demon = static_cast<daemon_t&>(daemon);
    demon.devices.get_attempts().dump(data);
    return ok;
}

ko c::handle_prepair_device(prepair_device_in_dst_t&& o_in, prepair_device_out_t& o_out) {
    log("prepair_device");
    /// in:
    ///     uint16_t pin;
    ///     string subhome;
    ///     string name;

    /// out:
    ///    uint16_t pin;
    ///    string subhome;

    auto& demon = static_cast<daemon_t&>(daemon);
    {
        auto r = demon.devices.device_prepair(o_in.pin, o_in.subhome, o_in.name);
        if (is_ko(r.first)) {
            return r.first;
        }
        o_out.pin = r.second;
    }
    o_out.subhome = o_in.subhome;
    return ok;
}

ko c::handle_unprepair_device(pin_t&& pin, string& ans) {
    log("unprepair_device");
    auto& demon = static_cast<daemon_t&>(daemon);
    {
        auto r = demon.devices.device_unprepair(pin);
        if (is_ko(r)) {
            ans = r;
            return r;
        }
    }
    ans = "done.";
    return ok;
}

//-/----------------apitool - End of API implementation.

