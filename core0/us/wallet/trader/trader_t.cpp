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
#include "trader_t.h"
#include <sstream>
#include <chrono>

#include <us/gov/stacktrace.h>
#include <us/gov/io/seriable.h>
#include <us/gov/config.h>
#include <us/gov/io/cfg.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/socket/client.h>
#include <us/gov/cash/tx_t.h>
#include <us/gov/cash/ttx.h>
#include <us/gov/cash/map_tx.h>
#include <us/gov/cash/file_tx.h>

#include <us/wallet/engine/peer_t.h>
#include <us/wallet/engine/daemon_t.h>
#include <us/wallet/wallet/local_api.h>

#include "traders_t.h"
#include "chat_t.h"
#include "business.h"
#include "types.h"

#define loglevel "wallet/trader"
#define logclass "trader_t"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::trader_t;

using namespace std::chrono;

#include "olog_t.inc"
template<typename... Args>
void c::olog(const Args&... args) const {
    ::ologx(*this, args...);
}

#if CFG_LOGS == 1
    #undef log
    #define log(...) log_("tid", this->id, __FILE__, __LINE__, __VA_ARGS__)
#endif

c::trader_t(traders_t& parent, engine::daemon_t& demon, const hash_t& parent_tid, const string& datasubdir): b(demon), parent(parent), bootstrapper(nullptr), datasubdir(datasubdir), parent_tid(parent_tid) {
    log("constructor", "TRACE 8c");
    ts_creation = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
    ts_activity.store(ts_creation);
    my_personality = parent.personality;
    my_challenge = personality_t::gen_challenge();
    log("my_challenge", my_challenge);
}

c::~trader_t() {
    log("destructor", this);
    join();
    delete p;
    delete bootstrapper;
}

ko c::permission_bootstrap(const peerid_t& peerid) const {
    assert(peerid.is_not_zero());
    if (bootstrapped_by != peerid) {
        if (bootstrapped_by.is_zero()) {
	        log("permission for bootstraping is granted to any peer in the group");
	        return ok;
        }
        auto r = "KO_60978 Peer doesn't own the bootstrapper." ;
        log(r, "re-bootstrap permission is denied.");
        return r;
    }
    log("re-bootstrap permission is granted");
    return ok;
}

ko c::boot(size_t utid, wallet::local_api& wallet) {
    log("boot 0", "TRACE 8c");
    delete bootstrapper;
    bootstrapper = nullptr;
    load_state(utid);
    assert(id.is_not_zero());
    if (utid != traders_t::get_utid(id, wallet)) {
        auto r = "KO 87997 utid doesn't match.";
        log(r);
        return r;
    }
    init(id, remote_endpoint, wallet);
    olog("boot from state read from disk.");
    return ok;
}

pair<ko, hash_t> c::boot(const peerid_t& peerid, bootstrapper_t* bs) {
    log("boot 1", "TRACE 8c", bs);
    ts_activity.store(duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count());
    auto r = permission_bootstrap(peerid);
    if (is_ko(r)) {
        return make_pair(r, hash_t(0));
    }
    delete bootstrapper;
    bootstrapper = bs;
    bootstrapped_by = peerid;
    assert(bootstrapped_by.is_not_zero());
    log("bootstraper", bs, "start");
    return bootstrapper->start(*this);
}

void c::init(const hash_t& tid, const endpoint_t& rep, wallet::local_api& wallet) {
    log("init", "TRACE 8c", tid, "ep", rep.to_string(), "subhome", wallet.subhome, "w", &wallet);
    remote_endpoint = rep;
    log("init", "remote_endpoint", remote_endpoint.to_string());
    w = &wallet;
    if (parent_tid.is_zero()) { //users can input zero meaning root
        parent_tid = tid; // zero is root (me root)
    } //parent trade shares datadir with its children, unless overriden by datasubdir
    log("datasubdir", datasubdir);
    if (datasubdir.empty()) {
        datasubdir = parent_tid.encode();
        log("datasubdir was empty.", "Now is", datasubdir);
    }
    id = tid;
    if (need_init()) { //olog decides
        #if CFG_LOGS == 1
            ostringstream os;
            os << "trade_" << id;
            logdir = parent.trades_logdir;
            logfile = os.str();
        #endif
        log("init_olog"); //trade log
        {
            ostringstream os;
            os << "/trade_" << ts_creation << '_' << id;
            init_olog(parent.home, os.str());
        }
        olog("Instantiated trader");
        start();
        schedule_push(push_trade, "en");
    }
    else {
        olog("New bootstrapper called init");
    }
    olog("Remote: ", remote_endpoint.to_string());
    olog("Side", initiator() ? "Initiator" : "Follower");
}

void c::on_stop() {
    save_state();
    b::on_stop();
    if (p != nullptr) p->on_finish();
}

const endpoint_t& c::local_endpoint() const {
    assert(w != nullptr);
    return w->local_endpoint;
}

pair<ko, hostport_t> c::resolve_ip_address(const hash_t& pkh) const {
    pair<ko, hostport_t> ret;
    ret.first = parent.daemon.lookup_wallet(pkh, ret.second);
    return move(ret);
}

void c::online(peer_t& peer) {
    log("online", "TRACE 8c");
    b::online(peer);
    if (peer.wallet_local_api == nullptr) {
        log("storing wallet_local_api in peer");
        peer.wallet_local_api = w;
    }
    assert(peer.wallet_local_api == w);
    if (bootstrapper == nullptr) {
        return;
    }
    log("calling bootstrapper->online", "TRACE 8c");
    bootstrapper->online(peer);
}

void c::offline() {
    log("offline", "TRACE 8c");
    if (bootstrapper != nullptr) {
        bootstrapper->offline();
    }
    b::offline();
}

int c::set_personality(const string& sk, const string& moniker) {
    log("set_personality", moniker);
    return my_personality.reset_if_distinct(sk, moniker);
}

ko c::set_protocol(protocol* p_, ch_t& ch) {
    lock_guard<mutex> lock(mx);
    return set_protocol_(p_, ch);
}

ko c::set_protocol_(protocol* p_, ch_t& ch) { //sh params changed (local, remote)
    log("set_protocol", p_, ch.to_string());
    if (unlikely(p_ == p)) {
        return ok;
    }
    delete p;
    if (p_ != nullptr) {
        if (p != nullptr) {
            olog("Protocol changed.");
        }
        else {
            olog("Protocol set.");
        }
        log("call on_attach");
        auto r = p_->attach(*this, ch);
        if (is_ko(r)) {
            return r;
        }
    }
    else {
        my_personality = parent.personality;
        ch.shared_params_changed = true;
    }
    p = p_;
    log("shared_params changed?", ch.to_string());
    return ok;
}

void c::on_file_updated(const string& path, const string& name) {
    if (p == nullptr) return;
    p->on_file_updated(path, name);
}

void c::update_activity() {
    log("update activity timestamp");
    ts_activity.store(duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count());
}

void c::update_peer(ch_t&& ch) {
    log("update_peer");
    ch.close();
    ostringstream os;
    os << "procch ";
    ch.to_stream(os);
    log(os.str());
    schedule_exec(os.str());
}

c::params_t c::shared_params() const {
    if (p == nullptr) {
        return params_t();
    }
    return p->shared_params();
}

ko c::update_peer(peer_t& peer, ch_t&& ch) const {
    ch.close();
    log("update_peer", ch.to_string());
    if (ch.personality_changed) {
        log("personality_changed");
        blob_t blob;
        svc_t svc;
        auto proof = my_personality.gen_proof(peer_challenge);
        if (ch.shared_params_changed) {
            log("shared_params_changed", "personality_changed");
            lock_guard<mutex> lock(p->_local_params_mx);
            blob_writer_t w(blob, blob_writer_t::blob_size(proof) + blob_writer_t::blob_size(p->_local_params.shared));
            w.write(p->_local_params.shared);
            w.write(proof);
            svc = svc_personality_and_params;
        }
        else {
            blob_writer_t w(blob, blob_writer_t::blob_size(proof));
            w.write(proof);
            svc = svc_personality;
        }
        log("sending over svc", svc);
        auto r = peer.call_trading_msg(peer_t::trading_msg_in_t(id, svc, blob));
        if (is_ko(r)) {
            log(r);
        }
    }
    else if (ch.shared_params_changed) {
        log("shared_params_changed");
        if (p != nullptr) {
            auto r = p->update_peer_(peer, move(ch));
            if (is_ko(r)) {
                return r;
            }
        }
    }
    if (ch.need_update_devices()) {
        log("push_data", "trace 5i0S", "trace c983Z");
        schedule_push(push_data, peer.get_lang());
    }
    for (auto& i: ch.updated_files) {
        log("Notifying other trades about a file update:", i.first, i.second);
        parent.on_file_updated2(i.first, i.second, this);
    }
    return ok;
}

ko c::on_remote(remote_params_t *rp, ch_t& ch) {
    log("on_remote params");
    if (!p) {
        auto r = "KO 40300 No protocol selected";
        log(r);
        return r;
    }
    return p->on_remote(rp, ch);
}

ko c::on_remote(personality_proof_t::raw_t&& id, ch_t& ch) {
    log("on_remote personality::proof::raw", id.to_string());
    unique_lock<mutex> lock(mx);
    if (peer_personality.reset(move(id))) {
        olog("Peer's personality changed", peer_personality.id, peer_personality.moniker);
        if (p != nullptr) {
            lock.unlock();
            auto r = p->on_remote(peer_personality, ch);
            if (is_ko(r)) {
                return r;
            }
        }
    }
    return ok;
}

ko c::on_remote(personality_proof_t&& proof, ch_t& ch) {
    log("on_remote personality::proof");
    if (!proof.verify(my_challenge)) {
        auto r = "KO 65095 Personality didnt verify";
        log(r);
        return r;
    }
    return on_remote(move(proof.raw()), ch);
}

ko c::on_remote(personality_proof_t&& proof, remote_params_t *rp, ch_t& ch) {
    log("on_remote personality::proof+params");
    auto r = on_remote(move(proof), ch);
    if (is_ko(r)) {
        return r;
    }
    auto r2 = p->on_remote(rp, ch);
    if (is_ko(r2)) {
        return r2;
    }
    return ok;
}

ko c::deliver(personality_proof_t&& personality_proof, ch_t& ch) {
    log("deliver personality_proof", personality_proof.raw().to_string());
    auto r = on_remote(move(personality_proof), ch);
    if (is_ko(r)) {
        log(r, "KO 44933 Failed setting remote personality");
        return r;
    }
    return ok;
}

ko c::deliver(protocols_t&& protocols, challenge_t&& challenge, ch_t& ch) {
    log("deliver protocols, challenge");
    lock_guard<mutex> lock(mx);
    peer_challenge = move(challenge);
    log("saved peer_challenge", peer_challenge);
    if (peer_protocols != protocols) {
        peer_protocols = move(protocols);
        ch.always_update_devices_();
        log("saved peer protocls", peer_protocols.size());
    }
    return ok;
}

ko c::deliver(endpoint_t&& endpoint, protocols_t&& protocols, challenge_t&& challenge, personality_proof_t&& personality_proof, ch_t& ch) {
    auto r = deliver(move(personality_proof), ch);
    if (is_ko(r)) {
        return r;
    }
    r = deliver(move(protocols), move(challenge), ch);
    if (is_ko(r)) {
        return r;
    }
    log("deliver endpoint");
    lock_guard<mutex> lock(mx);
    if (remote_endpoint != endpoint) {
        log("remote_endpoint updated 1.", remote_endpoint.to_string());
        remote_endpoint = move(endpoint);
        ch.always_update_devices_();
    }
    return ok;
}

ko c::deliver(protocol_selection_t&& protocol_selection, params_t&& params, ch_t& ch) {
    log("deliver protocol_selection, params;", "prot/role:", protocol_selection.first, protocol_selection.second, "; params sz", params.size());
    auto p = parent.create_protocol(move(protocol_selection), move(params));
    if (p.first != ok) {
        log(p.first);
        assert(p.second == nullptr);
        reset();
        return p.first;
    }
    auto r = set_protocol(p.second, ch);
    if (is_ko(r)) {
        return r;
    }
    return ok;
}

ko c::deliver(challenge_t&& challenge, protocol_selection_t&& protocol_selection, params_t&& params, ch_t& ch) {
    log("deliver endpoint, challenge, protocol_selection, params");
    auto r = deliver(move(protocol_selection), move(params), ch);
    if (is_ko(r)) {
        return r;
    }
    peer_challenge = move(challenge);
    return ok;
}

ko c::deliver(params_t&& params, ch_t& ch) {
    log("deliver params");
    return on_remote(new params_t(move(params)), ch);
}

ko c::deliver(personality_proof_t&& personality_proof, params_t&& params, ch_t& ch) {
    auto r = deliver(move(personality_proof), ch);
    if (is_ko(r)) return r;
    if (params.empty()) return ok;
    return deliver(move(params), ch);
}

ko c::deliver(endpoint_t&& endpoint, challenge_t&& challenge, personality_proof_t&& personality_proof, params_t&& params, ch_t& ch) {
    log("deliver endpoint, challenge, personality, params");
    auto r = deliver(move(personality_proof), move(params), ch);
    if (is_ko(r)) {
        return r;
    }
    log("deliver endpoint, challenge");
    lock_guard<mutex> lock(mx);
    if (remote_endpoint != endpoint) {
        log("remote_endpoint updated 2.", remote_endpoint.to_string());
        remote_endpoint = move(endpoint);
        ch.always_update_devices_();
    }
    peer_challenge = move(challenge);
    return ok;
}

ko c::on_svc_personality_and_params(blob_reader_t& reader, ch_t& ch) {
    if (p == nullptr) {
        auto r = "KO 77069 no protocol.";
        log(r);
        return r;
    }
    {
        auto r = p->on_svc_params(reader, ch);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = on_svc_personality(reader, ch);
        if (is_ko(r)) {
            return r;
        }
    }
    return ok;
}

ko c::on_svc_personality(blob_reader_t& reader, ch_t& ch) {
    personality::proof_t proof;
    auto r = reader.read(proof);
    if (is_ko(r)) {
        return r;
    }
    return on_remote(move(proof), ch);
}

ko c::trading_msg(peer_t& peer, uint16_t svc, blob_t&& blob) {
    log("trading_msg svc", svc);
    update_activity();
    static_assert((uint16_t)svc_end < (uint16_t)trader_protocol::svc_begin);
    if (svc < svc_end) {
        return trading_msg_trader(peer, svc, move(blob));
    }
    {
        if (p != nullptr) {
            log("given to protocol");
            assert(svc >= trader_protocol::svc_begin);
            return p->trading_msg(peer, svc, move(blob));
        }
        //peer sent params too fast, before we init protocol.
        log("unlikely but possible in slow computers. received protocol msg before initialising protocol", "");
        if (svc == trader_protocol::svc_params) {
            return trader_protocol::hold_remote_params(move(blob));
        }
    }
    log("Ignored protocol svc", svc);
    return trader_protocol::KO_29100;
}

ko c::trading_msg_trader(peer_t& peer, svc_t svc, blob_t&& blob) {
    log("trading_msg_trader", svc);
    static_assert((uint16_t)svc_end < (uint16_t)trader_protocol::svc_begin);
    if (svc >= svc_handshake_begin && svc < svc_handshake_end) {
        log("bootstrapper->trading_msg_trader", svc);
        if (bootstrapper == nullptr) {
            auto r = "KO 60186 Invalid state.";
            log(r);
            return r;
        }
        return bootstrapper->trading_msg(peer, svc, move(blob));
    }
    switch(svc) {
        case svc_roles_request: {
            log("Received protocols request.");
            protocols_t p;
            parent.published_protocols(p, false);
            log("Sending over requested protocols");
            blob_t payload;
            p.write(payload);
            return peer.call_trading_msg(peer_t::trading_msg_in_t(id, svc_roles, payload));
        }
        case svc_roles: {
            log("Received peer protocols.");
            {
                blob_reader_t reader(blob);
                lock_guard<mutex> lock(mx);
                auto r = reader.read(peer_protocols);
                if (is_ko(r)) {
                    return r;
                }
                if (peer_protocols.empty()) {
                    olog("empty set!");
                }
//                else {
//                    peer_protocols.dump(*logos);
//                }
            }
            log("push roles");
            schedule_push(push_roles, peer.get_lang()); //something changed for the user
            return ok;
        }
        case svc_qr_request: {
            log("Received qr request.");
            bookmarks_t bm;
            parent.published_bookmarks(*w, bm);
            log("Sending over requested qr");
            blob_t payload;
            bm.write(payload);
            return peer.call_trading_msg(peer_t::trading_msg_in_t(id, svc_qr, payload));
        }
        case svc_qr: {
            log("Received qr.");
            {
                lock_guard<mutex> lock(mx);
                auto r = peer_qrs.read(blob); //.from_stream(is);
                if (is_ko(r)) {
                    return r;
                }
                if (peer_qrs.empty()) {
                    olog("empty set!");
                }
                else {
                    peer_qrs.dump("peer_qr> ", *logos);
                }
            }
            log("push roles");
            schedule_push(push_qr_peer, peer.get_lang()); //something changed for the user
            return ok;
        }
        case svc_end_protocol: {
            ch_t ch(0);
            auto r = set_protocol(nullptr, ch);
            if (is_ko(r)) {
                return r;
            }
            olog("Protocol ended by peer request.");
            return update_peer(peer, move(ch));
        }
        case svc_personality_and_params: {
            log("received svc_personality_and_params");
            if (++peer_mutations > 10) { //cut potential snowball
                auto r = "KO 56954 Peer mutated too many times.";
                olog(r);
                return r;
            }
            ch_t ch(0);
            blob_reader_t reader(blob);
            auto r = on_svc_personality_and_params(reader, ch);
            if (is_ko(r)) {
                return r;
            }
            return update_peer(peer, move(ch));
        }
        case svc_personality: { //reception of peer personality
            log("received svc_personality");
            if (++peer_mutations > 10) { //cut potentially snowball
                auto r = "KO 56954 Peer mutated too many times.";
                olog(r);
                return r;
            }
            ch_t ch(0);
            blob_reader_t reader(blob);
            auto r = on_svc_personality(reader, ch);
            if (is_ko(r)) {
                return r;
            }
            return update_peer(peer, move(ch));
        }
        case svc_chat_msg: {
            chat_t::entry msg;
            {
                blob_reader_t reader(blob);
                auto r = reader.read(msg);
                if (is_ko(r)) {
                    return r;
                }
            }
            msg.me = false;
            log("recv msg from peer", msg.size());
            if (msg.empty()) {
                auto r = "KO 58686 Empty chat msg.";
                log(r);
                return r;
            }
            {
                ts_t m = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
                log("msg local time", m);
                chat.emplacexx(m, msg);
            }
            if (!resume_chat(peer)) {
                schedule_push(push_chat, peer.get_lang());
            }
            return ok;
        }
        case svc_functions_request: {
            log("Received functions request. - DISABLED");
            return ok;
        }
        case svc_functions: {
            log("Received remote functions. - DISABLED");
            return ok;
        }
        case svc_ping: {
            log("Received svc_ping", "Sending svc_pong.");
            return peer.call_trading_msg(peer_t::trading_msg_in_t(id, svc_pong, blob_t()));
        }
        case svc_pong: {
            ts_t ts_recv_pong = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
            log("Received svc_pong. ts", ts_recv_pong, "ping sent ts", ts_sent_ping, "calling pong_hander with roundtrip of", ts_recv_pong - ts_sent_ping, "ns");
            pong_handler(ts_recv_pong - ts_sent_ping);
            return ok;
        }
    }
    return trader_protocol::KO_29100;
}

chat_t::entry c::AI_chat(const chat_t&, peer_t& peer) {
    log("AI_chat");
    if (p != nullptr) {
        return p->AI_chat(chat, peer);
    }
    return chat_t::entry();
}

bool c::resume_chat(peer_t& peer) {
    log("resume_chat on socket");
    chat_t::entry response;
    uint64_t nr;
    response = AI_chat(chat, peer);
    if (response.empty()) {
        log("AI_chat decided to mute");
        return false; // hasn't push
    }
    log("sending msg");
    send_msg(peer, response);
    return true; // has already pushed
}

void c::schedule_push(uint16_t code, const string& lang) const {
    assert(w != nullptr);
    parent.schedule_push(get_push_datagram(code, lang), *w);
}

void c::schedule_push(datagram* d) const {
    assert(w != nullptr);
    parent.schedule_push(d, *w);
}

void c::funs_t::exec_help(const string& prefix, ostream& os) const {
    for (auto& i: *this) {
       os << prefix << i;
    }
}

datagram* c::get_push_datagram(uint16_t pc, const string& lang) const {
    auto blob = push_payload(pc, lang);
    auto d = peer_t::push_in_t(id, pc, blob).get_datagram(parent.daemon.channel, 0);
    log("get_push_datagram", pc, "blob sz", blob.size(), "dgram sz", d->size());
    return d;
}

blob_t c::push_payload(uint16_t pc, const string& lang) const {
    log("push_payload. push code", pc);
    assert(pc < push_end);
    blob_t blob;
    switch(pc) {
        case push_log: {
            log("pushing log");
            ostringstream os;
            show_log(id, os);
            blob = blob_writer_t::make_blob(os.str());
        }
        break;
        case push_data: {
            log("pushing data");
            //ostringstream doc;
            write_data(lang, blob);
        }
        break;
        case push_chat: {
            log("pushing chat");
            chat.write(blob);
        }
        break;
        case push_bookmarks: {
            log("pushing bookmarks");
            parent.bookmarks.write(blob);
        }
        break;
        case push_roles_mine: {
            log("pushing roles mine");
            protocols_t my_protocols = parent.published_protocols(false);
            my_protocols.write(blob);
        }
        break;
        case push_roles_peer: {
            log("pushing roles peer");
            lock_guard<mutex> lock(mx);
            peer_protocols.write(blob);
        }
        break;
        case push_roles: {
            log("pushing roles");
            protocols_t my_protocols = parent.published_protocols(true);
            //parent.invert(my_protocols);
            protocols_t p;
            {
                lock_guard<mutex> lock(mx);
                p = protocols_t::filter_common(my_protocols, peer_protocols);
            }
            p.write(blob);
        }
        break;
        case push_qr_mine: {
            log("pushing qr mine");
            bookmarks_t bm;
            parent.published_bookmarks(*w, bm);
            bm.write(blob);
        }
        break;
        case push_qr_peer: {
            log("pushing qr peer");
            lock_guard<mutex> lock(mx);
            peer_qrs.write(blob);
        }
        break;
        case push_trade: { //initiate new-trade
            log("pushing init-new-trade", id);
        }
        break;
        case push_killed: {
            log("pushing killed trade", id);
            blob = blob_writer_t::make_blob(reason);
        }
        break;
        case push_help: {
            log("pushing help");
            ostringstream os;
            help("", os);
            blob = blob_writer_t::make_blob(os.str());
        }
        break;
        case push_local_functions: {
            log("pushing local functions");
            ostringstream os;
            parent.lf.dump(os);
            blob = blob_writer_t::make_blob(os.str());
        }
        break;
        case push_remote_functions: {
            log("pushing remote functions");
            ostringstream os;
            rf.dump(os);
            blob = blob_writer_t::make_blob(os.str());
        }
        break;
    }
    return move(blob);
}

void c::dump(const string& pfx, ostream& os) const {
    os << "id " << id << '\n';
    os << pfx << "protocol ";
    if (p != nullptr) {
        p->dump(os);
    }
    else {
        os << "null";
    }
    os << '\n';
    b::dump(pfx, os);
}

void c::list_trades(ostream& os) const {
    os << ts_creation << ' ' << ts_activity.load() << ' ';
    assert(w != nullptr);
    os << '/' << w->subhome << ' ';
    if (remote_endpoint.pkh.is_zero()) {
        os << "- - ";
    }
    else {
        remote_endpoint.to_streamX(os);
        os << ' ';
    }
    if (p != nullptr) {
        p->list_trades_bit(os);
    }
    else {
        os << "- - ";
    }
}

void c::show_data(const string& lang, ostream& os) const {
    lock_guard<mutex> lock(mx);
    show_data_(lang, os);
}

void c::show_data_(const string& lang, ostream& os) const {
    using namespace std::chrono;
    assert(w != nullptr);
    {
        auto gts = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
        log("data_generated", gts);
        os << "data_generated " << gts << '\n';
    }
    os << "lang " << lang << '\n';
    os << "created " << ts_creation << '\n';
    os << "parent_tid " << parent_tid << '\n';
    os << "datasubdir " << datasubdir << '\n';
    os << "activity " << ts_activity.load() << '\n';
    os << "bootstrapped_by " << bootstrapped_by << '\n';
    b::show_data(lang, os);
    os << "local_endpoint "; w->local_endpoint.to_streamX(os); os << '\n';
    os << "my_personality " << my_personality.id << '\n';
    os << "my_moniker " << my_personality.moniker << '\n';
    os << "peer_personality " << peer_personality.id << '\n';
    os << "peer_moniker " << peer_personality.moniker << '\n';
    os << "peer_protocols " << peer_protocols.size() << '\n';
    os << "initiator " << (initiator() ? 'Y' : 'N') << '\n';
    os << "roles " << (peer_protocols.empty() ? 'N' : 'Y') << '\n';
    os << "qrs " << (peer_qrs.empty() ? 'N' : 'Y') << '\n';
    if (p == nullptr) {
        os << "protocol not set\n";
    }
    else {
        os << "protocol " << p->get_name() << ' ' << p->rolestr() << '\n';
        p->data(lang, os);
    }
    os << "rf " << (rf.empty() ? 'N' : 'Y') << '\n';
}

void c::write_data(const string& lang, blob_t& blob) const {
    ostringstream os;
    show_data(lang, os);
    string sd = os.str();

    #if CFG_LOGS == 1
    {
        data_t data;
        data.from(sd);
        log("dataInfo", "called from:");
        log_stacktrace();
        auto diff = data.get_diff(prev_data);
        logdump("datadiff> ", diff);
        {
            data_t data2(prev_data);
            if (is_ko(data2.apply(diff))) {
                log("KO 44498 diff doesnt work");
            }
            if (data != data2) {
                log("KO 44499 diff doesnt work");
            }
        }
        log("storing prev_data");
        prev_data = data;
    }
    #endif
    blob_writer_t w(blob, blob_writer_t::blob_size(sd));
    w.write(sd);
    log("push_data sz str", sd.size(), "blob", blob.size());
}

bool c::requires_online(const string& cmd) const {
    if (p != nullptr) {
        if (p->requires_online(cmd)) return true;
    }
    if (cmd == "protocols") return true;
    if (cmd == "connect") return true;
    if (cmd == "start") return true;
    if (cmd == "msg") return true;
    if (cmd == "request") return true;
    if (cmd == "end") return true;
    if (cmd == "send") return true;
    if (cmd == "ping") return true;
    if (cmd == "change") return true;
    if (cmd == "kill") return true;
    return false;
}

bool c::sig_reload(ostream& os) {
    os << "signal RELOAD\n";
    bool ch = false;
    return ch;
}

bool c::sig_hard_reset(ostream&) {
    return false;
}

bool c::sig_reset(ostream& os) {
    os << "signal RESET\n";
    bool ch = false;
    ostringstream x;
    x <<"forgotten ";
    {
        if (chat.clearxx()) {
            schedule_push(trader_t::push_chat, "en");
            x << "chat ";
            ch = true;
        }
    }
    if (ch) {
        os << x.str() << '\n';
    }
    return ch;
}

void c::on_signal(int sig) {
    olog("Received Signal from User ", sig);
    bool ch = false;
    ostringstream os;
    switch(sig) {
        case 0:
            ch |= sig_reset(os);
            break;
        case 1:
            ch |= sig_hard_reset(os);
            break;
        case 2:
            ch |= sig_reload(os);
            break;
    }
    if (p != nullptr) {
        log("transmit signal to protocol");
        ch |= p->on_signal(sig, os);
    }
    if (ch) {
        log(os.str());
        if (p != nullptr) {
            ch_t ch(0);
            ch.shared_params_changed = true;
            update_peer(move(ch));
        }
        olog(os.str());
        return;
    }
    log("nothing changed in reaction to signal", sig);
    if (sig == 0) {
        olog("Raise HARD RESET");
        on_signal(1);
    }
    else if (sig == 1) {
        olog("No further actions for sig", sig);
    }
}

ko c::send_msg(peer_t& peer, const chat_t::entry& msg) {
    ts_t n = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
    chat.emplacexx(n, msg);
    blob_t blob;
    msg.write(blob);
    log("sending chat msg");
    auto r = peer.call_trading_msg(peer_t::trading_msg_in_t(id, svc_chat_msg, blob));
    schedule_push(push_chat, peer.get_lang());
    return r;
}

ko c::send_msg(peer_t& peer, const string& hmsg) {
    chat_t::entry msg(hmsg);
    msg.me = true;
    return send_msg(peer, msg);
}

void c::send_msg(const string& hmsg) {
    log("send_msg", hmsg);
    ostringstream cmd;
    cmd << "msg " << hmsg;
    schedule_exec(cmd.str());
}

ko c::exec_offline(const string& cmd0, ch_t& ch) {
    log("exec_offline", cmd0);
    string lang = "en";
    update_activity();
    if (cmd0.empty()) {
        schedule_push(push_help, lang);
        return ok;
    }
    istringstream is(cmd0);
    string cmd;
    is >> cmd;
    assert(!cmd.empty());
    if (cmd == "show") {
        string cmd;
        is >> cmd;
        if (cmd == "roles") {
            string cmd;
            is >> cmd;
            if (cmd == "mine") {
                schedule_push(push_roles_mine, lang);
                return ok;
            }
            if (cmd == "peer") {
                schedule_push(push_roles_peer, lang);
                return ok;
            }
            if (cmd.empty()) {
                schedule_push(push_roles, lang);
                return ok;
            }
            auto r = "KO 40932 Invalid command roles.";
            log(r);
            return move(r);
        }
        if (cmd == "qrs") {
            string cmd;
            is >> cmd;
            if (cmd == "mine") {
                schedule_push(push_qr_mine, lang);
                return ok;
            }
            if (cmd == "peer") {
                schedule_push(push_qr_peer, lang);
                return ok;
            }
            auto r = "KO 40932 Invalid command qr <mine|peer>.";
            log(r);
            return move(r);
        }
        if (cmd == "chat") {
            schedule_push(push_chat, lang);
            return ok;
        }
        if (cmd == "bookmarks") {
            schedule_push(push_bookmarks, lang);
            return ok;
        }
        if (cmd == "log") {
            schedule_push(push_log, lang);
            return ok;
        }
        if (cmd == "data") {
            ch.always_update_devices_();
            return ok;
        }
        if (cmd == "lf") {
            schedule_push(push_local_functions, lang);
            return ok;
        }
        if (cmd == "rf") {
            schedule_push(push_remote_functions, lang);
            return ok;
        }
    }
    if (cmd == "copybm") {
        int i = -1;
        is >> i;
        log("copybm", i);
        --i;
        lock_guard<mutex> lock(mx);
        auto it = peer_qrs.begin();
        if (i < 0) {
            auto r = "KO 76904 Invalid Ordinal. [check 'show qrs peer']";
            log(r, i);
            return r;
        }
        advance(it, i);
        if (it == peer_qrs.end()) {
            auto r = "KO 76905 Invalid Ordinal. [check 'show qrs peer']";
            log(r, i);
            return r;
        }
        auto r = create_bookmark(it->first, it->second);
        if (is_ko(r)) return r;
        schedule_push(push_bookmarks, lang);
        return push_OK("Bookmark added.");
    }
    if (cmd == "reset") {
        on_signal(0);
        ch.all_changed();
        return push_OK("Sent signal 0 (Reset memory)");
    }
    if (cmd == "reload") {
        on_signal(2);
        return push_OK("Sent signal 2 (Reload)");
    }
    if (cmd == "lf") {
        auto r = "KO 30291 Not implemented.";
        log(r);
        return move(r);
    }
    if (cmd == "disconnect") {
        if (set_state(state_req_offline)) {
            return push_OK("Going offline.");
        }
        return push_OK("Already offline.");
    }
    if (cmd == "procch") {
        ch_t x(0);
        auto r = x.from_stream(is);
        log("ch", x.to_string());
        if (is_ko(r)) return r;
        ch |= x;
        return ok;
    }
    if (p != nullptr) {
        log("protocol exec offline", cmd0);
        ch.open(p->_local_params, p->_local_params_mx);
        auto r = p->exec_offline(cmd0, ch);
        ch.close();
        return r;
    }
    auto r = trader_protocol::WP_29101;
    log(r);
    return move(r);
}

void c::dispose() {
    log("dispose");
    schedule_push(trader_t::push_killed, "en");
    parent.dispose(this);
}

ko c::push_KO(ko msg) const {
    assert(w != nullptr);
    return parent.push_KO(id, msg, *w);
}

ko c::push_OK(const string& msg) const {
    assert(w != nullptr);
    return parent.push_OK(id, msg, *w);
}

void c::saybye(peer_t& peer) {
    log("saybye");
    blob_t blob;
    {
        string msg = "Bye";
        blob_writer_t w(blob, blob_writer_t::blob_size(msg));
        w.write(msg);
    }
    peer.call_trading_msg(peer_t::trading_msg_in_t(id, traders_t::svc_kill_trade, blob));
}

void c::die(const string& rson) {
    log("die", rson);
    if (reason.empty()) {
        reason = rson;
    }
    else {
        reason = reason + '\n' + rson;
    }
    b::die(reason);
}

void c::on_KO(ko r, const string& cmd) {
    assert(is_ko(r));
    olog(r, cmd);
    push_KO(r);
}

qr_t c::remote_qr() const {
    if (p == nullptr) {
        return qr_t(remote_endpoint);
    }
    return qr_t(remote_endpoint, p->protocol_selection());
}

ko c::create_bookmark(const string& name, string&& icofile, string&& label) {
    blob_t blob;
    us::gov::io::read_file_(icofile, blob);
    bookmark_t b(move(remote_qr()), bookmark_info_t(move(label), move(blob)));
    lock_guard<mutex>(parent.bookmarks_mx);
    return parent.bookmarks.add(name, move(b));
}

ko c::create_bookmark(const string& name, const bookmark_t& bm) {
    bookmark_t b(bm);
    lock_guard<mutex>(parent.bookmarks_mx);
    return parent.bookmarks.add(name, move(b));
}

protocol_selection_t c::opposite_protocol_selection() const {
    if (p == nullptr) {
        return protocol_selection_t();
    }
    return p->opposite_protocol_selection();
}

ko c::exec_online(peer_t& peer, const string& cmd0, ch_t& ch) {
    log("exec_online", cmd0);
    update_activity();
    istringstream is(cmd0);
    string cmd;
    is >> cmd;
    assert(!cmd.empty());
    if (cmd == "request") {
        string cmd;
        is >> cmd;
        if (cmd == "roles") {
            auto r = peer.call_trading_msg(peer_t::trading_msg_in_t(id, svc_roles_request, blob_t()));
            if (is_ko(r)) {
                return move(r);
            }
            return push_OK("roles requested.");
        }
        if (cmd == "qrs") {
            auto r = peer.call_trading_msg(peer_t::trading_msg_in_t(id, svc_qr_request, blob_t()));
            if (is_ko(r)) {
                return move(r);
            }
            return push_OK("qrs requested.");
        }
        if (cmd == "rf") {
            auto r = peer.call_trading_msg(peer_t::trading_msg_in_t(id, svc_functions_request, blob_t()));
            if (is_ko(r)) {
                return move(r);
            }
            return push_OK("Requested remote functions.");
        }
    }
    if (cmd == "start") {
        string prot;
        string role;
        is >> prot;
        is >> role;
        log("start protocol", prot, role);
        if (is.fail()) {
            auto r = "KO 40935 Invalid protocol or role.";
            olog(r);
            return move(r);
        }
        auto r = bootstrapper->dialogue_b.initiate(peer, protocol_selection_t(move(prot), move(role)), ch);
        if (is_ko(r)) {
            return move(r);
        }
        return push_OK("Waiting for peer...");
    }
    if (cmd == "end") {
        olog("Sent end command to remote trader.");
        auto r = set_protocol(nullptr, ch);
        if (is_ko(r)) {
            return r;
        }
        return peer.call_trading_msg(peer_t::trading_msg_in_t(id, svc_end_protocol, blob_t()));
    }
    if (cmd == "send") {
        string cmd;
        is >> cmd;
        if (cmd == "personality") {
            ch.personality_changed = true;
            ch.no_need_to_update_devices_();
            string mute;
            is >> mute;
            if (mute == "mute") {
                return ok;
            }
            return push_OK("Sent personality");
        }
    }
    if (cmd == "change") {
        string cmd;
        is >> cmd;
        if (cmd == "personality") {
            log("change personality");
            string ssk;
            string mon;
            is >> ssk;
            is >> mon;
            if (is.fail()) {
                auto r = "KO 65009 Bad inputs.";
                log(r);
                return move(r);
            }
            if (ssk == "0") {
                log("change to anonymous personality with 0");
                ssk = "11111111111111111111";
            }
            priv_t sk(ssk);
            auto r = my_personality.reset_if_distinct(sk, mon) != 0;
            ch.personality_changed |= r;
            if (r) {
                if (p != nullptr) {
                    ch.open(p->_local_params, p->_local_params_mx);
                    p->rehome_apply(ch);
                    ch.close();
                }
                return push_OK("Personality changed");
            }
            return push_OK("Personality didn't change.");
        }
        if (cmd == "moniker") {
            string mon;
            is >> mon;
            if (is.fail()) {
                auto r = "KO 65009";
                log(r);
                return move(r);
            }
            if (my_personality.moniker == mon) {
                return push_OK("Moniker didn't change.");
            }
            ch.personality_changed |= true;
            return push_OK("Moniker changed");
        }
    }
    if (cmd == "msg") {
        string hmsg;
        getline(is, hmsg);
        us::gov::io::cfg0::trim(hmsg);
        if (hmsg.empty()) {
            auto r = "KO 30292 Empty message.";
            log(r);
            return move(r);
        }
        send_msg(peer, hmsg);
        return ok;
    }
    if (cmd == "connect") {
        return push_OK("Connecting..."); /// requires_online does the attempt. push_OK("Online.");
    }
    if (cmd == "ping") {
        log("ping cmd");
        string x;
        is >> x;
        if (x == "X") { //silent, current handler
            log("ping using current handler");
            ping(static_cast<peer_t&>(peer));
            return ok;
        }
        log("ping using pushOk handler");
        ping(static_cast<peer_t&>(peer), [&](uint64_t ns_roundtrip) {
                ostringstream os;
                os << "Received Pong!. Roundtrip took " << ns_roundtrip << " ns.";
                log(os.str());
                push_OK(os.str());
            });
        return push_OK("Sent ping.");
    }
    if (cmd == "kill") {
        log("saybye");
        saybye(peer);
        assert(peer.wallet_local_api != nullptr);
        parent.kill(traders_t::get_utid(id, *peer.wallet_local_api), "API function kill_trade");
        return ok;
    }
    if (cmd == "make_bookmark" || cmd == "makebm") {
        string name;
        is >> name;
        string file;
        is >> file;
        string label;
        getline(is, label);
        auto r = create_bookmark(name, move(file), move(label));
        if (is_ko(r)) {
            return r;
        }
        return push_OK("Bookmark created.");
    }
    if (p != nullptr) {
        log("protocol exec online", cmd0);
        ch.open(p->_local_params, p->_local_params_mx);
        auto r = p->exec_online(peer, cmd0, ch);
        ch.close();
        return r;
    }
    auto r = trader_protocol::WP_29101;
    log(r, cmd);
    return move(r);
}

void c::ping(function<void(uint64_t)> handler) {
    log("ping + handler");
    log("set pong handler");
    pong_handler = [this, handler](uint64_t ns_roundtrip) {
            log("calling 2nd pong handler");
            handler(ns_roundtrip);
            log("reset pong handler");
            pong_handler = [](uint64_t){}; //reset;
        };
    schedule_exec("ping X");
}

void c::ping(peer_t& peer, function<void(uint64_t)> handler) {
    log("ping + peer + handler");
    log("set pong handler");
    pong_handler = [this, handler](uint64_t ns_roundtrip) {
            log("calling 2nd pong handler");
            handler(ns_roundtrip);
            log("reset pong handler");
            pong_handler = [](uint64_t){}; //reset;
        };
    ping(peer);
}

void c::ping(peer_t& peer) {
    ts_sent_ping = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
    log("ping peer ts", ts_sent_ping);
    peer.call_trading_msg(peer_t::trading_msg_in_t(id, svc_ping, blob_t()));
}

void c::reset_ping() {
    pong_handler = [](uint64_t){};
}

void c::funs_t::dump(ostream& os) const {
    for (auto& i: *this) {
        os << i << '\n';
    }
}

void c::help(const string& ind, ostream& os) const {
    using fmt = trader_protocol;
    string ind2 = ind + "    ";
    os << ind << "Trade id: " << id << '\n';
    os << ind << "Personality: " << my_personality.id << ' ' << my_personality.moniker << '\n';
    os << ind << "Trader commands:\n";
    if (get_stateX() == state_offline) {
        fmt::twocol(ind2, "connect", "Go online", os);
    }
    else {
        fmt::twocol(ind2, "disconnect", "Go offline", os);
    }
    fmt::twocol(ind2, "ping", "Ping remote trader.", os);
    fmt::twocol(ind2, "request", "Request remote object", os);
    {
        auto ind3 = ind2 + " \"  ";
        fmt::twocol(ind3, "roles", "Request roles they want you to play", os);
        fmt::twocol(ind3, "qrs", "Request remote shared r2r QRs.", os);
        fmt::twocol(ind3, "rf", "Request remote functions list", os);
    }
    fmt::twocol(ind2, "show", "Display object", os);
    {
        auto ind3 = ind2 + " \"  ";
        fmt::twocol(ind3, "roles", "Roles I can initiate", os);
        fmt::twocol(ind3, "roles mine", "Roles I can play, by protocol", os);
        fmt::twocol(ind3, "roles peer", "Roles peer can play", os);
        fmt::twocol(ind3, "qrs mine", "My shared r2r endpoints. QRs.", os);
        fmt::twocol(ind3, "qrs peer", "Their shared r2r endpoints. QRs.", os);
        fmt::twocol(ind3, "data", "Available data", os);
        fmt::twocol(ind3, "log", "Log content", os);
        fmt::twocol(ind3, "chat", "Dialogue history", os);
        fmt::twocol(ind3, "bookmarks", "Endpoints worth to remember.", os);
        fmt::twocol(ind3, "lf", "List local functions", os);
        fmt::twocol(ind3, "rf", "List remote functions", os);
    }
    fmt::twocol(ind2, "makebm <local_name> <ico_file|-> <label> ", "Create a bookmark with this endpoint.", os);
    fmt::twocol(ind2, "copybm <#ordinal>", "Copy bookmark from peer. (see 'qrs peer' command)", os);
    fmt::twocol(ind2, "change personality <key> <moniker>", "Change my personality.", my_personality.moniker, os);
    fmt::twocol(ind2, "change moniker <moniker>", "Change moniker of current personality.", my_personality.moniker, os);
    fmt::twocol(ind2, "send personality [mute]", "Send over personality proof [without feedback].", my_personality.moniker, os);
    fmt::twocol(ind2, "start <protocol> <role>", "Start protocol playing the specified role", os);
    fmt::twocol(ind2, "reset", "Send RESET signal", os);
    fmt::twocol(ind2, "reload", "Send RELOAD signal", os);
    fmt::twocol(ind2, "kill", "Archive this trade", os);
    fmt::twocol(ind2, "lf <name> <args>|help", "Execute local function", os);
    fmt::twocol(ind2, "rf <name> <args>|help", "Execute remote function", os);
    fmt::twocol(ind2, "msg <text>", "Send message to peer [chat]", os);
    fmt::twocol(ind2, "trade", "Go back to trading shell", os);
    fmt::twocol(ind2, "wallet", "Go back to wallet shell", os);
    if (p != nullptr) {
        os << ind << "R2R Protocol: " << p->get_name() << ". Role: " << p->rolestr() << "\n";
        os << ind << "    Home: " << p->pphome << "\n";
        os << ind << "Protocol offline commands:\n";
        {
            fmt::twocol(ind2, "show", "Display object", os);
            p->help_show(ind2 + " \"  ", os);
        }
        p->help_onoffline(ind2, os);
        os << ind << "Protocol Online commands:\n";
        p->help_online(ind2, os);
    }
    else {
        os << ind << "R2R Protocol: None.\n";
    }
    os << '\n';
}

void c::file_reloadx(const string& fqn) {
   log("raising signal 2 - reload");
   on_signal(2);
}

size_t c::blob_size() const {
    size_t sz = blob_writer_t::blob_size(id) +
        blob_writer_t::blob_size(parent_tid) +
        blob_writer_t::blob_size(my_personality) +
        blob_writer_t::blob_size(peer_personality) +
        blob_writer_t::blob_size(peer_protocols) +
        blob_writer_t::blob_size(peer_qrs) +
        blob_writer_t::blob_size(ts_creation) +
        blob_writer_t::blob_size(my_challenge) +
        blob_writer_t::blob_size(peer_challenge) +
        blob_writer_t::blob_size(bootstrapped_by) +
        blob_writer_t::blob_size(chat) +
        blob_writer_t::blob_size(datasubdir) +
        blob_writer_t::blob_size(endpoint_pkh) +
        blob_writer_t::blob_size(remote_endpoint) +
        blob_writer_t::blob_size(rf) +
        blob_writer_t::blob_size(p, parent.protocol_factories) +
        blob_writer_t::blob_size(ts_activity.load());
    log("blob_size", sz);
    return sz;
}

void c::to_blob(blob_writer_t& writer) const {
    log("to_blob", "cur", (uint64_t)(writer.cur - writer.blob.data()));
    log("id", id);
    writer.write(id);
    writer.write(parent_tid);
    writer.write(my_personality);
    writer.write(peer_personality);
    writer.write(peer_protocols);
    writer.write(peer_qrs);
    writer.write(ts_creation);
    writer.write(my_challenge);
    writer.write(peer_challenge);
    writer.write(bootstrapped_by);
    log("chat");
    writer.write(chat);
    writer.write(datasubdir);
    log("endpoint_pkh");
    writer.write(endpoint_pkh);
    writer.write(remote_endpoint);
    log("rf");
    writer.write(rf);
    log("protocol");
    writer.write(p, parent.protocol_factories);
    log("ts_activity");
    writer.write(ts_activity.load());
}

ko c::from_blob(blob_reader_t& reader) {
    log("from_blob", "cur", (uint64_t)(reader.cur - reader.blob.data()));
    {
        auto r = reader.read(id);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = reader.read(parent_tid);
        if (is_ko(r)) {
            return r;
        }
    }    
    {
        auto r = reader.read(my_personality);
        if (is_ko(r)) {
            return r;
        }
    }    
    {
        auto r = reader.read(peer_personality);
        if (is_ko(r)) {
            return r;
        }
    }    
    {
        auto r = reader.read(peer_protocols);
        if (is_ko(r)) {
            return r;
        }
    }    
    {
        auto r = reader.read(peer_qrs);
        if (is_ko(r)) {
            return r;
        }
    }    
    {
        auto r = reader.read(ts_creation);
        if (is_ko(r)) {
            return r;
        }
    }    
    {
        auto r = reader.read(my_challenge);
        if (is_ko(r)) {
            return r;
        }
    }    
    {
        auto r = reader.read(peer_challenge);
        if (is_ko(r)) {
            return r;
        }
    }    
    {
        auto r = reader.read(bootstrapped_by);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = reader.read(chat);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = reader.read(datasubdir);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = reader.read(endpoint_pkh);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = reader.read(remote_endpoint);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = reader.read(rf);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = reader.read(p, parent.protocol_factories);
        if (is_ko(r)) {
            return r;
        }
        if (p != nullptr) {
            p->tder = this;
        }
    }
    {
        uint64_t ts;
        auto r = reader.read(ts);
        if (is_ko(r)) {
            return r;
        }
        ts_activity.store(ts);
    }
    return ok;
}

pair<string, string> c::sername(size_t utid) const {
    ostringstream os;
    os << parent.home << "/state/" << utid;
    return make_pair(os.str(), "state");
}

void c::load_state(size_t utid) {
    log("load_state", sername(utid).first);
    auto n = sername(utid);
    lock_guard<mutex> lock(mx);
    auto r = load(n.first + '/' + n.second);
    if (is_ko(r)) {
        log("trade could not be loaded", n.first , n.second);
        if (r != us::gov::io::cfg0::KO_84012) { //file does not exist
            log(r);
            cerr << r << endl;
            assert(false);
        }
    }
}

void c::save_state() const {
    if (id.is_zero()) {
        return;
    }
    log("save_state", sername(traders_t::get_utid(id, *w)).first);
    auto n = sername(traders_t::get_utid(id, *w));
    us::gov::io::cfg0::ensure_dir(n.first);
    lock_guard<mutex> lock(mx);
    auto r = save(n.first + '/' + n.second + "_off");
    if (is_ko(r)) {
        log("active trades could not be saved", n.first , n.second);
    }
}

