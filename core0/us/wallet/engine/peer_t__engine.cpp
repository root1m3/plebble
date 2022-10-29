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
#include "peer_t.h"
#include <sstream>
#include "types.h"

#include <us/gov/config.h>
#include <us/gov/io/cfg0.h>
#include <us/gov/socket/datagram.h>

#include <us/wallet/engine/daemon_t.h>

#include "wallet_connection_t.h"

#define loglevel "wallet/engine"
#define logclass "peer_t__engine"
#include <us/gov/logs.inc>
#include <us/gov/vcs.h>

using namespace us::wallet::engine;
using c = us::wallet::engine::peer_t;

#include <us/api/generated/wallet/c++/engine/cllr_rpc-impl>

bool c::process_work__engine(datagram* d) {
    using namespace protocol;
    switch (d->service) {
        #include <us/api/generated/wallet/c++/engine/hdlr_svc-router>
    }
    return false;
}

#include <us/api/generated/wallet/c++/engine/hdlr_svc_handler-impl>

//------------------apitool - API Spec defined @ us/api/generated/wallet/c++/engine/hdlr_local-impl

ko c::handle_print_grid(string& data) {
    log("print_grid");
    auto& demon = static_cast<daemon_t&>(daemon);
    ostringstream ans;
    demon.watch(ans);
    data = ans.str();
    return ok;
}

ko c::handle_print_socket_connections(string& data) {
    log("print_socket_connections");
    auto& demon = static_cast<daemon_t&>(daemon);
    ostringstream ans;
    string pfx;
    dynamic_cast<us::gov::socket::multipeer::server&>(daemon).dump(pfx, ans);
    data = ans.str();
    return ok;
}

ko c::handle_allw(string& data) {
    log("allw");
    auto& demon = static_cast<daemon_t&>(daemon);
    ostringstream os;
    demon.users.dump(os);
    data = os.str();
    return ok;
}

ko c::handle_sync(string& data) {
    log("sync");
    auto& demon = static_cast<daemon_t&>(daemon);
    ostringstream os;
    demon.users.sync(os);
    data = os.str();
    return ok;
}

ko c::handle_reload_file(string&& filename, string& ans) {
    log("reload_file");
    auto& demon = static_cast<daemon_t&>(daemon);
    demon.traders.reload_file(filename);
    ans = "ok.";
    return ok;
}

ko c::handle_get_component_update(get_component_update_in_dst_t&& o_in, get_component_update_out_t& o_out) {
    log("get_component_update. brand_code", o_in.brandcode, "component", o_in.component, "vcsname", o_in.vcsname);
    /// in:
    ///     string brandcode;
    ///     string component;
    ///     string vcsname;

    /// out:
    ///    string vcsname;
    ///    vector<uint8_t> bin_pkg;

/*
Returns:
{ok, {filename, {}}}  ==> remote component version is the latest.
{ok, {filename, {non-empty}}}  ==> returning payload and latest filename.
{KO 66953, {{}, {}}}  ==> invalid component.
{KO 70973, {filename, {}}}  ==> internal problem.
{KO 70974, {filename, {}}}  ==> internal problem.
{KO 22122, {downloads dir, {}}} ==> internal problem. Missing dir.
{KO 22123, {downloads dir, {}}} ==> internal problem. No files found in dir.
*/

    if (o_in.component != "android") {
        auto r = "KO 66953 Invalid component.";
        log(r, o_in.component);
        return r;
    }
    string apkfilename = us::vcs::apkfilename();
    if (apkfilename == o_in.vcsname) {
        log("apkfilename == o_in.vcsname", apkfilename);
        o_out.vcsname = o_in.vcsname;
        o_out.bin_pkg.clear();
        return ok;
    }
    log("apkfilename != o_in.vcsname", apkfilename);


    string branddir;
//    string blobhashfile;
    string blobfile;
    auto brandcode = o_in.brandcode;
    if (brandcode == "XaOywxt") { //any
        brandcode = CFG_ANDROID_BLOB_ID;
        log("any brandcode XaOywxt resolved to ", brandcode);
    }
    log("brandcode ", brandcode);
    {
        auto& demon = static_cast<daemon_t&>(daemon);
        ostringstream os;
        os << demon.downloads_dir << '/' << o_in.component << '/' << brandcode;
        branddir = os.str();
//        os << "/blob_name";
//        blobhashfile = os.str();
        blobfile = branddir + "/blob.apk";
    }
/*
    string sblobhash;
    {
        log("read_file", blobhashfile);
        auto r = us::gov::io::read_text_file_(blobhashfile, sblobhash);
        if (is_ko(r)) {
            o_out.file = o_in.curver;
            o_out.bin_pkg.clear();
            auto r = "KO 70973 blob_name file not found.";
            log(r);
            return r;
        }
    }
    us::gov::io::cfg0::trim(sblobhash);
    log("sblobhash, o_in.curver", sblobhash, o_in.curver);
*/
//    if (sblobhash == o_in.curver) {
    {
        log("read_file_", blobfile);
        auto r = us::gov::io::read_file_(blobfile, o_out.bin_pkg);
        if (is_ko(r)) {
            o_out.vcsname = o_in.vcsname;
            o_out.bin_pkg.clear();
            auto r = "KO 70974 blob file not found.";
            log(r);
            return r;
        }
    }
    o_out.vcsname = apkfilename;
    log("ok vcsname=", o_out.vcsname, "blob", o_out.bin_pkg.size(), "bytes");
    return ok;
}

ko c::handle_get_component_hash(get_component_hash_in_dst_t&& o_in, string& curver) {
    log("get_component_hash");
    /// in:
    ///     string component;
    ///     string filename;

    if (o_in.component != "android") {
        auto r = "KO 66953 Invalid component.";
        log(r, o_in.component);
        return r;
    }
    string apkfilename = us::vcs::apkfilename();
    if (apkfilename != o_in.filename) {
        auto r = "KO 66954 Not available.";
        log(r, apkfilename, o_in.filename);
        return r;
    }
    string branddir;
    string blobhashfile;
    log("brandcode ", CFG_ANDROID_BLOB_ID);
    {
        auto& demon = static_cast<daemon_t&>(daemon);
        ostringstream os;
        os << demon.downloads_dir << '/' << o_in.component << '/' << CFG_ANDROID_BLOB_ID;
        branddir = os.str();
        os << "/blob_name";
        blobhashfile = os.str();
    }
    {
        log("read_file", blobhashfile);
        auto r = us::gov::io::read_text_file_(blobhashfile, curver);
        if (is_ko(r)) {
            auto r = "KO 70978 blob_name file not found.";
            log(r);
            return r;
        }
    }
    us::gov::io::cfg0::trim(curver);
    if (curver.empty()) {
        auto r = "KO 70928 curver is empty.";
        log(r);
        return r;
    }
    log("curver", curver);
    return ok;
}

ko c::handle_harvest(harvest_in_dst_t&& o_in, string& ans) {
    log("harvest");
    /// in:
    ///     uint16_t id;
    ///     uint16_t timeout;
    ///     uint16_t decay;
    ///     priv_t priv;
    return ok;
}

ko c::handle_world(vector<hash_t>& wallets) {
    log("world");
    auto& demon = static_cast<daemon_t&>(daemon);
    return demon.gov_rpc_daemon.get_peer().call_wallets(wallets);
}

ko c::handle_lookup_wallet(hash_t&& addr, lookup_wallet_out_t& o_out) {
    log("lookup_wallet");
    /// out:
    ///    uint32_t net_addr;
    ///    uint16_t port;

    auto& demon = static_cast<daemon_t&>(daemon);
    hostport_t hostport;
    auto r = demon.lookup_wallet(addr, hostport);
    if (is_ko(r)) {
        return r;
    }
    o_out.net_addr = hostport.first;
    o_out.port = hostport.second;
    return ok;
}

ko c::handle_random_wallet(random_wallet_out_t& o_out) {
    log("random_wallet");
    /// out:
    ///    hash_t addr;
    ///    uint32_t net_addr;
    ///    uint16_t port;

    auto& demon = static_cast<daemon_t&>(daemon);
    daemon_t::gov_rpc_daemon_t::peer_type::random_wallet_out_dst_t fc_out;
    auto r = demon.gov_rpc_daemon.get_peer().call_random_wallet(fc_out);
    if (is_ko(r)) {
        return r;
    }
    o_out.addr = fc_out.addr;
    o_out.net_addr = fc_out.net_addr;
    o_out.port = fc_out.port;
    return ok;
}

ko c::handle_register_w(uint32_t&& net_addr, string& ans) {
    log("register_w");
    auto& demon = static_cast<daemon_t&>(daemon);
    ans = "ok.";
    auto r = demon.register_w_(net_addr);
    if (is_ko(r)) {
        ans = r;
    }
    return r;
}

ko c::handle_conf(conf_in_dst_t&& o_in, string& ans) {
    log("conf");
    /// in:
    ///     string key;
    ///     string value;

    auto& demon = static_cast<daemon_t&>(daemon);
    return demon.handle_conf(o_in.key, o_in.value, ans);
}

//-/----------------apitool - End of API implementation.

