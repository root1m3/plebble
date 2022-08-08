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
#include <fstream>
#include <us/gov/config.h>
#include <us/gov/likely.h>
#include <us/gov/vcs.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/crypto/base58.h>
#include "protocol.h"
#include "daemon_t.h"

#ifndef HAVE_CFG
    #error need config.h
#endif

#define loglevel "gov/id"
#define logclass "peer_t"
#include "logs.inc"

using namespace us::gov::id;
using c = us::gov::id::peer_t;

const char* c::KO_6017 = "KO 6017 authentication failed.";

thread_local bool c::inbound_traffic__was_encrypted{false};

constexpr array<const char*, c::num_stages> c::stagestr;

c::peer_t(daemon_t& daemon, int sock): b(daemon, sock) {
    log("constructor");
}

c::~peer_t() {
    log("destructor");
    delete se;
    delete handshakes;
}

ko c::connect(const hostport_t& hostport, pport_t pport, pin_t pin, role_t asrole, bool block) {
    log("connect_as", client::endpoint(hostport), "role", rolestr[asrole], "pport", pport, "pin", pin, "block", block);
    auto r = b::connect0(hostport, block);
    if (likely(r == ok)) {
        log("initiating dialogue as role '", asrole, "' pport", pport, "pin", pin);
        initiate_dialogue(asrole, pport, pin);
    }
    log("result", r == ok ? "ok" : r);
    return r;
}

ko c::connect(const shostport_t& shostport, pport_t pport, pin_t pin, role_t role, bool block) {
    return connect(ip4_encode(shostport), pport, pin, role, block);
}

void c::disconnectx(channel_t channel, seq_t seq, const reason_t& reason) {
    log("disconnect", channel, seq, reason);
    b::disconnectx(channel, seq, reason);
    cv_auth.notify_all();
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "    id: stage " << stagestr[stage_peer] << " pubk " << pubkey << " pubkh " << pubkey.hash() << " role " << rolestr[role] << " swver " << short_version() << '\n';
}

void c::dump_all(const string& prefix, ostream& os) const {
    dump(prefix, os);
    b::dump_all(prefix, os);
}

c::handshakes_t::handshakes_t(role_t role, pport_t pport, pin_t pin) {
    me = new handshake_t(role, pport, pin);
}

c::handshakes_t::handshakes_t() {
    peer = new handshake_t();
}

c::handshakes_t::~handshakes_t() {
    delete me;
    delete peer;
}

c::handshake_t::handshake_t(role_t role, pport_t pport, pin_t pin) {
    log("handshake_t constructor", role, pport, pin);
    *reinterpret_cast<uint16_t*>(&msg[0]) = *reinterpret_cast<uint16_t*>(&us::vcs::codehash[0]); //LE
    *reinterpret_cast<uint16_t*>(&msg[2]) = pport; //LE
    *reinterpret_cast<uint16_t*>(&msg[4]) = pin; //LE
    msg[6] = role;
    ifstream f("/dev/urandom");
    f.read(reinterpret_cast<char*>(&msg[7]), sigmsg_hasher_t::output_size - 7);
    log("msg", msg, "version_fingerprint", parse_version_fingerprint(), "pport", parse_pport(), "pin", parse_pin(), "role", parse_role());
}

c::handshake_t::handshake_t() {
    log("handshake_t default constructor");
}

uint16_t c::handshake_t::parse_version_fingerprint() const {
    return *reinterpret_cast<const uint16_t*>(&msg[0]);
}

uint16_t c::handshake_t::parse_pport() const {
    return *reinterpret_cast<const uint16_t*>(&msg[2]);
}

uint16_t c::handshake_t::parse_pin() const {
    return *reinterpret_cast<const uint16_t*>(&msg[4]);
}

c::role_t c::handshake_t::parse_role() const {
    return (role_t)msg[6];
}

void c::handshake_t::dump(const string& pfx, ostream& os) const {
    os << pfx << "msg " << msg << '\n';
    role_t r = parse_role();
    os << pfx << "role " << rolestr[r] << '\n';
    os << pfx << "pport " << parse_pport() << '\n';
    os << pfx << "pin " << parse_pin() << '\n';
    os << pfx << "version fingerprint " << parse_version_fingerprint() << '\n';
}

string c::short_version() const {
    vector<unsigned char> v;
    const char* p = reinterpret_cast<const char*>(&version_fingerprint);
    v.push_back(*p++);
    v.push_back(*p);
    return us::gov::crypto::b58::encode(v);
}

const keys_t& c::get_keys() const {
    log("get_keys");
    return dynamic_cast<const daemon0_t&>(daemon).get_keys();
}

ko c::turn_on_encryption() {
    log("turn_on_encryption?");
    if (se != nullptr) {
        log("WARNING 37290 Encryption is already ON.");
        return ok;
    }
    if (unlikely(stage_peer != verified)) {
        auto r = "KO 37190 Encryption cannot be turned on.";
        log(r);
        return r;
    }
    se = new crypto::symmetric_encryption();
    ko r = se->init(get_keys().priv, pubkey);
    if (unlikely(is_ko(r))) {
        log(r);
        delete se;
        se = nullptr;
        return r;
    }
    #if CFG_LOGS == 1
        if (daemon.outbound_traffic__goes_encrypted()) {
            log("================== ENCRYPTION TURNED ON ==================");
        }
        else {
            log("================== ENCRYPTION TURNED send:OFF recv:ON  ==================");
        }
    #endif
    return ok;
}

pair<ko, datagram*> c::encrypt0(datagram* d) const {
    log("need? encrypt0. svc", d->service);
    assert(d != nullptr);
    svc_t svc = d->decode_service(); //pre-decoded field (service) is 0 when sending
    if (unlikely(svc == 0)) {
        log("Datagram is already encrypted.", d->size());
        return make_pair(ok, d);
    }
    if (svc < protocol::id_end) {
        bool clear_text = true;
        if (svc < protocol::id_begin) {
            if (svc != protocol::socket_ping && svc != protocol::socket_ping_response) {
                clear_text = false;
            } 
        }
        if (clear_text) {
            log("Not encrypting. Service", svc, " forced to go cleartext. (auth handshake or ping)");
            return make_pair(ok, d);
        }
    }
    if (se == nullptr) {
        log("Not encrypting. Encryption not turned on.");
        return make_pair(ok, d);
    }
    log("outbound_traffic__goes_encrypted", daemon.outbound_traffic__goes_encrypted(), "inbound_traffic__was_encrypted", inbound_traffic__was_encrypted);
    pair<ko, datagram*> r;
    if (daemon.outbound_traffic__goes_encrypted() || inbound_traffic__was_encrypted) {
        r = d->encrypt(*se);
        if (unlikely(is_ko(r.first))) {
            assert(r.second == nullptr);
        }
        if (r.second != d) {
            log("datagram encrypted. svc", svc, "length", d->size(), "-> svc", r.second->decode_service(), "length", r.second->size());
            log("deleting original clear-text datagram.");
            delete d;
        }
    }
    else {
        r = make_pair(ok, d);
    }
    return r;
}

pair<ko, datagram*> c::decrypt0(datagram* e) const {
    log("decrypt datagram");
    assert(e != nullptr);
    if (e->service != 0) {
        log ("dgram arrived without encryption. svc", e->service);
        inbound_traffic__was_encrypted = false;
        return make_pair(ok, e);
    }
    inbound_traffic__was_encrypted = true;
    if (se == nullptr) {
        auto r = "KO 30029 Encryption is not turned on.";
        log(r);
        delete e;
        return make_pair(r, nullptr);
    }
    pair<ko,datagram*> d = e->decrypt(daemon.channel, *se);
    if (unlikely(is_ko(d.first))) {
        assert(d.second == nullptr);
    }
    if (d.second != e) {
        log("deleting original encrypted datagram.");
        delete e;
    }
    return d;
}

void c::set_stage_peer(stage_t s) {
    lock_guard<mutex> lock(mx_auth);
    stage_peer = s;
}

c::stage_t c::get_stage_peer() const {
    lock_guard<mutex> lock(mx_auth);
    return stage_peer;
}

ko c::wait_auth() const {
    log("wait_auth", CFG_AUTH_TIMEOUT_SECS," secs");
    chrono::system_clock::time_point deadline = since + chrono::seconds(CFG_AUTH_TIMEOUT_SECS);
    unique_lock<mutex> lock(mx_auth);
    if (stage_peer == verified || stage_peer == verified_fail) {
        log(stage_peer == verified ? "verified ok" : KO_6017);
        return ok;
    }
    log("wait for signal or timeout");
    cv_auth.wait_until(lock, deadline, [&] {
        return stage_peer == verified || stage_peer == verified_fail || is_finished();
    });
    log(stage_peer == verified ? "verified ok" : KO_6017);
    return stage_peer == verified ? ok : KO_6017;
}

bool c::process_work(datagram* d) {
    assert(d != nullptr);
    log("process_work", "svc", d->service);
    assert(d->service < protocol::id_end);
    if (d->service < protocol::id_begin) {
        return b::process_work(d);
    }
    if (stage_peer != anonymous) {
        auto r = "KO 30299 stage_peer != anonymous";
        log(r, stagestr[stage_peer]);
        auto seq = d->decode_sequence();
        delete d;
        disconnect(seq, r);
        return true;
    }
    log("process_work", "using keys at", &get_keys(), get_keys().pub, "svc", d->service);
    using namespace us::gov::protocol;
    #ifdef has_us_gov_id_api
        switch(d->service) {
            #include <us/api/generated/c++/gov/id/hdlr_svc-router>
        }
    #endif
    auto r = "KO 30298 service handled here in id::peer";
    log(r, d->service);
    auto seq = d->decode_sequence();
    delete d;
    disconnect(seq, r);
    return true;
}

#ifdef has_us_gov_id_api

#include <us/api/generated/c++/gov/id/hdlr_svc_handler-impl>


/*
  initiator                                                        responder
=================================                                  ===========================================
hs(me,peer)=(0,0)                                                  hs(me,peer)=(0,0)

initiate_dialogue()
-------------------
hs(me,peer)=(new,0)
send (hs.me)      ----------------------gov_id_request---------->  process_request()
                                                                   -----------------
                                                                   hs(me,peer)=(0,new)
                                                                   sign(hs.peer) msg:=(role,pport,random))
                                                                   hs(me,peer)=(new,*)
process_peer_challenge <-------------gov_id_peer_challenge-------- send (signature,hs.me)
----------------------
hs(me,peer)=(*,new)
verify signature
sign(hs.peer) msg:=(role,pport,random)
send (signature)-------------------gov_id_challenge_response-----> process_challenge_response
completed(hs.peer.pport)                                           --------------------------
                                                                   verify signature
                                                                   completed(hs.peer.pport)
*/

ko c::initiate_dialogue(role_t role, pport_t pport, pin_t pin) { //role '0'peer; '1'sysop; '2'device
    /*
    initiate_dialogue()
    -------------------
    hs(me,peer)=(new,0)
    send (hs.me)      ----------------------gov_id_request---------->
    */
    log("initiate_dialogue as ", rolestr[role], pport, pin);
    if (se != nullptr) {
        log("Found existing encryption module. destroying it.");
        delete se;
        se = nullptr;
    }
    if (unlikely(handshakes != nullptr)) {
        log("Found existing handshakes object. destroying it.");
        delete handshakes;
    }
    log("reseting state.");
    set_stage_peer(anonymous);
    pubkey.zero();
    handshakes = new handshakes_t(role, pport, pin);
    logdump("  initiate_dialogue: my handshake: ", *handshakes->me);
    log("sending id_request", protocol::id_request, handshakes->me->msg, "role", rolestr[handshakes->me->parse_role()]);
    assert(role == handshakes->me->parse_role());
    return call_request(handshakes->me->msg);
}

//------------------apitool - API Spec defined @ us/api/generated/c++/gov/id/hdlr_local-impl

ko c::handle_request(seq_t seq, sigmsg_hash_t&& msg) {
    log("request", seq);
    /*
              ----------------------gov_id_request---------->  process_request()
                                                               -----------------
                                                               hs(me,peer)=(0,new)
                                                               sign(hs.peer) msg:=(role,pport,random))
                                                               hs(me,peer)=(new,*)
                           <-------------gov_id_peer_challenge-------- send (signature,hs.me)
    */
    const keys& mykeys = get_keys();
    if (unlikely(handshakes != nullptr)) {
        delete handshakes;
/*
        auto r = "KO 99101 Invalid handshake. Bad sequence.";
        log(r);
        stage_peer = peer_t::verified_fail;
        disconnect(seq, r);
        return r;
*/
    }
    handshakes = new handshakes_t();
    handshakes->peer->msg = msg;
    logdump("  process_request: their handshake: ", *handshakes->peer);
    auto peer_role = handshakes->peer->parse_role();
    if (peer_role != role_peer && peer_role != role_sysop && peer_role != role_device) {
        auto r = "KO 75690 Invalid role.";
        log("role", peer_role, r);
        stage_peer = verified_fail;
        disconnect(seq, r);
        return r;
    }
    role = peer_role;
    assert(role == handshakes->peer->parse_role());
    log("role", rolestr[role]);
    sig_t sig;
    auto r = crypto::ec::instance.sign(mykeys.priv, handshakes->peer->msg, sig);
    if (unlikely(is_ko(r))) {
        set_stage_peer(verified_fail);
        disconnect(seq, r);
        return r;
    }
    handshakes->me = new handshake_t(role, static_cast<daemon_t&>(daemon).pport, 0);
    logdump("  process_request: my handshake: ", *handshakes->me);
    assert(mykeys.pub.valid);
    assert(mykeys.pub == keys::get_pubkey(mykeys.priv));
    crypto::ec::sig_der_t sig_der;
    if (role == role_device) {
        log("device using spongycastle(java) verifies with der");
        sig_der = crypto::ec::instance.sig_encode_der(sig);
    }
    log("sending svc id_peer_challenge", protocol::id_peer_challenge, handshakes->me->msg, mykeys.pub, sig.to_b58(), "sig(der)", us::gov::crypto::ec::instance.sig_encode_der_b58(sig), "sig(der)", us::gov::crypto::b58::to_hex(us::gov::crypto::ec::instance.sig_encode_der(sig)));
    return call_peer_challenge(peer_challenge_in_t(handshakes->me->msg, mykeys.pub, sig, sig_der));
}

ko c::handle_peer_challenge(seq_t seq, peer_challenge_in_dst_t&& o_in) {
    log("peer_challenge", seq);
    /// in:
    ///     sigmsg_hash_t msg;
    ///     pub_t pub;
    ///     sig_t sig;
    ///     sig_der_t sig_der;

    /*
    process_peer_challenge <-------------gov_id_peer_challenge--------
    ----------------------
    hs(me,peer)=(*,new)
    verify signature
    sign(hs.peer) msg:=(role,pport,random)
    send (signature)-------------------gov_id_challenge_response----->
    */
    auto& mykeys = get_keys();
    log("process_peer_challenge. seq", seq);
    if (unlikely(handshakes == nullptr)) {
        auto r = "KO 84410 Invalid handshakes object.";
        log(r);
        set_stage_peer(verified_fail);
        disconnect(seq, r);
        return r;
    }
    if (unlikely(handshakes->peer != nullptr)) {
        auto r = "KO 44930 Invalid object handshakes->peer";
        log(r);
        set_stage_peer(verified_fail);
        disconnect(seq, r);
        return r;
    }
    handshakes->peer = new handshake_t();
    handshakes->peer->msg = o_in.msg;
    pubkey = o_in.pub;
    logdump("  process_peer_challenge: their handshake: ", *handshakes->peer);
    if (unlikely(!pubkey.valid)) {
        auto r = "KO 59448 Invalid pubkey.";
        log(r);
        pubkey.zero();
        set_stage_peer(verified_fail);
        disconnect(seq, r);
        return r;
    }
    auto peer_role = handshakes->peer->parse_role();
    if (peer_role != role_peer && peer_role != role_sysop && peer_role != role_device) {
        auto r = "KO 72010 Invalid role.";
        log(r);
        set_stage_peer(verified_fail);
        disconnect(seq, r);
        return r;
    }
    role = peer_role;
    log("role", rolestr[role]);
    log("msgh", handshakes->me->msg, "pubk", pubkey, "sig", o_in.sig.to_b58(), "sig_der", us::gov::crypto::b58::encode(o_in.sig_der));
    if (o_in.sig.is_zero()) {
        log("using sig_der");
        o_in.sig = crypto::ec::instance.sig_from_der(o_in.sig_der);
    }
    if (unlikely(!crypto::ec::instance.verify_not_normalized(pubkey, handshakes->me->msg, o_in.sig))) {
        auto r = "KO 20147 Invalid signature.";
        set_stage_peer(verified_fail);
        log(r, handshakes->me->msg, pubkey, o_in.sig.to_b58());
        disconnect(seq, r);
        return r;
    }
    set_stage_peer(stage_t::verified);
    log("verified", pubkey);
    assert(mykeys.pub.valid);
    log("peer msg for me to sign", handshakes->peer->msg);
    sig_t sig;
    auto r = crypto::ec::instance.sign(mykeys.priv, handshakes->peer->msg, sig); //sign their message
    if (unlikely(is_ko(r))) {
        set_stage_peer(verified_fail);
        disconnect(seq, r);
        return r;
    }
    crypto::ec::sig_der_t sig_der;
    {
        log("sending id_challenge_response", protocol::id_challenge_response, mykeys.pub, sig.to_b58());
        auto r = call_challenge_response(challenge_response_in_t(mykeys.pub, sig, sig_der));
        if (unlikely(is_ko(r))) {
            set_stage_peer(verified_fail);
            disconnect(seq, r);
            return r;
        }
    }
    {
        auto r = turn_on_encryption();
        if (unlikely(is_ko(r))) {
            set_stage_peer(verified_fail);
            disconnect(seq, r);
            return r;
        }
    }
    log("call verification_completed", handshakes->peer->parse_pport(), handshakes->peer->parse_pin());
    verification_completed(handshakes->peer->parse_pport(), handshakes->peer->parse_pin());
    delete handshakes;
    handshakes = nullptr;
    log("signaling VERIF_COMPLETED.");
    cv_auth.notify_all();
    return ok;
}

ko c::handle_challenge_response(seq_t seq, challenge_response_in_dst_t&& o_in) {
    log("challenge_response", seq);
    /// in:
    ///     pub_t pub;
    ///     sig_t sig;
    ///     sig_der_t sig_der;

    /*
            -------------------gov_id_challenge_response-----> process_challenge_response
                                                               --------------------------
                                                               verify signature
                                                               completed(hs.peer.pport)
    */
    log("process_challenge_response. seq", seq);
    if (unlikely(handshakes == nullptr)) {
        auto r = "KO 75040 Invalid handshake.";
        set_stage_peer(verified_fail);
        disconnect(seq, r);
        return r;
    }
    if (unlikely(handshakes->peer == nullptr)) {
        auto r = "KO 63201 handshakes->peer==nullptr";
        set_stage_peer(verified_fail);
        disconnect(seq, r);
        return r;
    }
    pubkey = o_in.pub;
    if (unlikely(!pubkey.valid)) {
        auto r = "KO 85048 Invalid public key";
        set_stage_peer(verified_fail);
        disconnect(seq, r);
        return r;
    }
    log("msgh", handshakes->me->msg, "pubk", pubkey, "sig", o_in.sig.to_b58(), "sig_der", us::gov::crypto::b58::encode(o_in.sig_der));
    if (o_in.sig.is_zero()) {
        assert(role == role_device);
        log("using sig_der sent by HMI device");
        o_in.sig = crypto::ec::instance.sig_from_der(o_in.sig_der);
    }
    if (unlikely(!crypto::ec::instance.verify_not_normalized(pubkey, handshakes->me->msg, o_in.sig))) { //verify my message signed by peer
        auto r = "KO 10210 Invalid signature.";
        set_stage_peer(verified_fail);
        disconnect(seq, r);
        return r;
    }
    set_stage_peer(verified);
    log("verified", pubkey);
    {
        auto r = turn_on_encryption();
        if (unlikely(is_ko(r))) {
            disconnect(seq, r);
            return r;
        }
    }
    log("call verification_completed", handshakes->peer->parse_pport(), handshakes->peer->parse_pin());
    verification_completed(handshakes->peer->parse_pport(), handshakes->peer->parse_pin());
    delete handshakes;
    handshakes = nullptr;
    log("signaling VERIF_COMPLETED.");
    cv_auth.notify_all();
    return ok;
}

//-/----------------apitool - End of API implementation.

#include <us/api/generated/c++/gov/id/cllr_rpc-impl>

#endif

