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
#include <us/gov/engine/protocol.h>
#include <us/gov/cash/app.h>
#include <us/gov/cash/file_tx.h>
#include <us/gov/cash/addresses_t.h>
#include "daemon_t.h"
#include "db_t.h"
#include "types.h"

#define loglevel "gov/engine"
#define logclass "peer_t__cash"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::peer_t;

#include <us/api/generated/gov/c++/cash/cllr_rpc-impl>

bool c::process_work__cash(datagram* d) {
    using namespace protocol;
    switch(d->service) {
        #include <us/api/generated/gov/c++/cash/hdlr_svc-router>
    }
    return false;
}

#include <us/api/generated/gov/c++/cash/hdlr_svc_handler-impl>

//------------------apitool - API Spec defined @ us/api/generated/gov/c++/cash/hdlr_local-impl

ko c::handle_fetch_accounts(addresses_t&& addresses, accounts_t& accounts) {
    log("fetch_accounts");
    auto& db = engine_daemon().db->cash_app->db;
    lock_guard<mutex> lock(db.mx);
    log("db sz", db.accounts->size());
    for (auto& i: addresses) {
        log("account", i);
        auto a = db.accounts->find(i);
        if (likely(a != db.accounts->end())) {
            accounts.emplace(*a);
        }
    }
    return ok;
}

ko c::handle_share_file(share_file_in_dst_t&& o_in, string& error) {
    log("share_file");
    /// in:
    ///     blob_t file_tx_blob;
    ///     vector<uint8_t> content;

    if (!us::gov::socket::datagram::validate_payload(o_in.content)) {
        auto r = "KO 12146 File size exceed datagram limit.";
        log(r);
        error = r;
        return r;
    }
    cash::file_tx* tx = new cash::file_tx();
    {
        auto r = tx->read(o_in.file_tx_blob);
        if (is_ko(r)) {
            error = r;
            delete tx;
            return r;
        }
    }
    if (unlikely(tx->size != o_in.content.size())) {
        auto r = "KO 70693 content size doesn't match tx.";
        error = r;
        log(r);
        return r;
    }
    if (tx->size > 0) {
        auto hash = us::gov::crypto::ripemd160::digest(o_in.content);

        if (unlikely(tx->digest != hash)) {
            auto r = "KO 70692 content doesn't match digest in tx.";
            error = r;
            log(r);
            return r;
        }

        ///save file in DFS directory
        ostringstream filename;
        filename << static_cast<dfs::daemon_t&>(daemon).fsroot << '/' << hash;
        ofstream os(filename.str(), ios::binary);
        os.write((const char*)&o_in.content[0], o_in.content.size());
    }
    engine_daemon().new_evidence(tx);
    return ok;
}

ko c::handle_search_kv(search_kv_in_dst_t&& o_in, string& data) {
    log("search_kv");
    /// in:
    ///     hash_t addr;
    ///     string key;

    auto& db = engine_daemon().db->cash_app->db;
    ostringstream os;
    db.print_kv_b64(o_in.addr, os); //TODO arg_key
    data = os.str();
    return ok;
}

ko c::handle_file_hash(file_hash_in_dst_t&& o_in, hash_t& content_fingerprint) {
    log("file_hash");
    /// in:
    ///     hash_t addr;
    ///     string id;
    auto& db = engine_daemon().db->cash_app->db;
    content_fingerprint = db.file_hash(o_in.addr, o_in.id);
    if (content_fingerprint.is_zero()) {
        auto r = "KO 70291 File not found.";
        log(r);
        return r;
    }
    return ok;
}

ko c::handle_file_content(file_content_in_dst_t&& o_in, vector<uint8_t>& content) {
    log("file_content");
    /// in:
    ///     hash_t addr;
    ///     hash_t local_hash;
    ///     string fileid;

    auto& db = engine_daemon().db->cash_app->db;
    auto hash = db.file_hash(o_in.addr, o_in.fileid);
    if (hash.is_zero()) {
        content.clear();
        return "KO 87170 File not found in ledger.";
    }
    if (hash == o_in.local_hash) {
        content.clear();
        return ok;
    }
    vector<uint8_t> r;
    log("file has updated content");
    ostringstream filename;
    filename << engine_daemon().peerd.fsroot << '/' << hash;
    log("reading content for file", filename.str());
    auto e = io::read_file_(filename.str(), r);
    if (is_ko(e)) {
        content.clear();
        return e;
    }
    return ok;
}

//-/----------------apitool - End of API implementation.

