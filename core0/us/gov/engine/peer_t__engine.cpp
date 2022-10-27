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

#include <us/gov/config.h>
#include <us/gov/engine/protocol.h>

#include "daemon_t.h"
#include "local_deltas_t.h"
#include "db_t.h"
#include "types.h"

#define loglevel "gov/engine"
#define logclass "peer_t__engine"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::peer_t;

#include <us/api/generated/gov/c++/engine/cllr_rpc-impl>

bool c::process_work__engine(datagram* d) {
    using namespace protocol;
    switch(d->service) {
        #include <us/api/generated/gov/c++/engine/hdlr_svc-router>
    }
    return false;
}

#include <us/api/generated/gov/c++/engine/hdlr_svc_handler-impl>

//------------------apitool - API Spec defined @ us/api/generated/gov/c++/engine/hdlr_local-impl

ko c::handle_local_deltas(datagram* dgram, local_deltas_t* local_deltas) {
    log("local_deltas", "svc", dgram->service, "sz", dgram->size());
    ostream nullos(0);
    if (unlikely(!local_deltas->verify(nullos))) {
        delete local_deltas;
        #if CFG_COUNTERS == 1
            ++engine_daemon().counters.recv_local_deltas_wrong_signature;
        #endif
        ko r = "KO 20790 Invalid local_deltas";
        log(r);
        return r;
    }
    if (!engine_daemon().db->auth_app->db.is_node(local_deltas->pubkey.hash())) {
        auto r = "KO 26192 Signature signed by unregistered node.";
        log(r, local_deltas->pubkey.hash(), "nodes sz", engine_daemon().db->auth_app->db.nodes.size());
        delete local_deltas;
        #if CFG_COUNTERS == 1
            ++engine_daemon().counters.recv_local_deltas_wrong_signature;
        #endif
        return r;
    }
    if (!engine_daemon().pools.add(local_deltas, true)) {
        #if CFG_COUNTERS == 1
            ++engine_daemon().counters.recv_local_deltas_duplicate;
        #endif
        log("Duplicate local deltas.");
        delete dgram;
        return ok;
    }
    log("added to pool");
    #if CFG_COUNTERS == 1
        ++engine_daemon().counters.recv_local_deltas_accepted;
    #endif
    log("relaying local deltas");
    engine_daemon().peerd.relay_local_deltas(dgram, this);
    return ok;
}

ko c::handle_vote_tip(datagram* dgram, vote_tip_in_dst_t&& o_in) {
    log("vote_tip", "svc", dgram->service, "sz", dgram->size());
    /// in:
    ///     hash_t tip;
    ///     pub_t pubkey;
    ///     sig_t signature;

    #if CFG_COUNTERS == 1
        ++engine_daemon().counters.recv_vote;
    #endif
    log("received vote", o_in.tip, "from", o_in.pubkey.hash());
    if (!crypto::ec::instance.verify(o_in.pubkey, o_in.tip, o_in.signature)) {
        #if CFG_COUNTERS == 1
            ++engine_daemon().counters.recv_vote_wrong_signature;
        #endif
        auto r = "KO 90392 Wrong signature in vote.";
        log(r);
        return r;
    }
    if (!engine_daemon().votes.add(o_in.pubkey.hash(), o_in.tip)) {
        log("duplicate vote");
        #if CFG_COUNTERS == 1
            ++engine_daemon().counters.recv_dup_vote;
        #endif
        delete dgram;
        return ok;
    }
    log("voted. relaying vote.");
    if (unlikely(engine_daemon().db->auth_app->my_stage() != peer_t::node)) {
        log("I am not a node.");
        delete dgram;
        return ok;
    }
    int n = engine_daemon().peerd.relay_vote(dgram, this);
    if (likely(n > 0)) {
        log("relay vote", n);
        #if CFG_COUNTERS == 1
            ++engine_daemon().counters.relay_vote_success;
        #endif
    }
    else {
        log("relay vote failed");
        #if CFG_COUNTERS == 1
            ++engine_daemon().counters.relay_vote_fail;
        #endif
    }
    return ok;
}

ko c::handle_query_block(hash_t&& block_hash, vector<uint8_t>& block_file) {
    log("query_block", block_hash);
    ostringstream file;
    file << engine_daemon().blocksdir << '/' << block_hash;
    auto r = io::read_file_(file.str(), block_file);
    if (is_ko(r)) {
        return r;
    }
    if (block_file.empty()) {
       auto r = "KO 10007 Block unavailable.";
       log(r, "could not read file", file.str());
       return r;
    }
    return ok;
}

ko c::handle_query_block_response(vector<uint8_t>&& block_file) {
    log("query_block_response", block_file.size());

    ostringstream fn;
    hash_t h = crypto::ripemd160::digest(block_file);
    log("received block", h, block_file.size(), "bytes");
    fn << engine_daemon().blocksdir << '/' << h;
    string final_filename = fn.str();
    static mutex mx;
    {
        lock_guard<mutex> lock(mx);
        if (!io::cfg0::file_exists(final_filename)) {
            fn << ".tmp";
            string tmp_filename = fn.str();
            {
                ofstream os(tmp_filename, ios::out | ios::binary);
                os.write((char*)block_file.data(), block_file.size());
            }
            ::rename(tmp_filename.c_str(), final_filename.c_str());
            engine_daemon().syncd.signal_file_arrived();
            log("saved file", final_filename);
        }
        else {
            log("file already exists");
        }
    }
    return ok;
}

ko c::handle_sysop(string&& cmd, string& ans) {
    log("sysop");
    if (stage != sysop) {
        auto r = "KO 66021 Only sysop allowed.";
        log(r, "disconnect. not a sysop. stage", stagestr[stage], endpoint(), "id/sysop=", is_role_sysop());
        disconnect(0, r);
        return r;
    }
    if (!engine_daemon().sysop_allowed) {
        auto r = "KO 55101 Sysop shell is disabled.";
        log(r, "disconnect-sysop not allowed", endpoint());
        disconnect(0, r);
        return r;
    }
    ostringstream os;
    auto r = engine_daemon().sysops.exec(this, cmd, os);
    if (is_ko(r)) {
        os << r << '\n';
    }
    ans = os.str();
    return ok;
}

ko c::handle_track(uint64_t&& ts, track_status_t& track_status) {
    log("track");
    engine_daemon().evt.track(engine_daemon().calendar, ts, track_status);
    return ok;
}

ko c::handle_track_response(track_status_t&& track_status) {
    log("track_response");
    return ok; //Ignore
}

ko c::handle_ev(datagram* dgram, blob_t&& blob_evidence) {
    log("ev", "svc", dgram->service, "sz", dgram->size());
    {
        evidence* ev;
        {
            auto r = engine_daemon().parse_evidence(blob_evidence);
            if (unlikely(is_ko(r.first))) {
                assert(r.second == nullptr);
                return r.first;
            }
            ev = r.second;
        }
        {
            auto r = engine_daemon().process_evidence2(ev);
            if (is_ko(r)) {
                return r;
            }
        }
    }
    log("relaying evidence");
    static_cast<net_daemon_t&>(daemon).relay_evidence(dgram, this);
    return ok;
}

ko c::handle_ev_track(datagram* dgram, blob_t&& blob_evidence, track_status_t& track_status) {
    log("ev_track", "svc", dgram->service, "sz", dgram->size());
    ts_t ts;
    {
        evidence* ev;
        {
            auto r = engine_daemon().parse_evidence(blob_evidence);
            if (unlikely(is_ko(r.first))) {
                assert(r.second == nullptr);
                return r.first;
            }
            ev = r.second;
            ts = ev->ts;
        }
        {
            auto r = engine_daemon().process_evidence2(ev);
            if (is_ko(r)) {
                if (r != calendar_t::WP_50451) {
                    return r;
                }
                log("evidence already exist in calendar");
            }
            else {
                log("new evidence added to calendar");
            }
        }
    }
    log("track evidence", ts);
    engine_daemon().evt.track(engine_daemon().calendar, ts, track_status);
    log("relaying evidence");
    static_cast<net_daemon_t&>(daemon).relay_evidence(dgram, this);
    return ok;
}

ko c::handle_harvest(seq_t seq, harvest_in_dst_t&& o_in) {
    log("harvest", seq);
    /// in:
    ///     uint16_t key;
    ///     uint16_t timeout;
    ///     uint16_t decay;
    ///     string payload;
    return "KO 55049 deprecated function";
}

ko c::handle_harvest_rollback(seq_t seq, string&& response) {
    log("harvest_rollback", seq);
    /// in:
    ///     string response;

    return "KO 55049 deprecated function";
}

//-/----------------apitool - End of API implementation.

//------------------apitool - API Spec defined @ us/api/generated/gov/c++/engine/hdlr_local-impl


