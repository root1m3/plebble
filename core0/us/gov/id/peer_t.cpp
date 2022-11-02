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
#if CFG_COUNTERS == 1
    c::counters_t c::counters;
#endif

c::peer_t(daemon_t& daemon, int sock): b(daemon, sock) {
    log("constructor");
}

c::~peer_t() {
    log("destructor");
    delete se;
    delete handshakes;
}

ko c::connect(const hostport_t& hostport, pport_t pport, pin_t pin, role_t asrole, const request_data_t& request_data, bool block) {
    log("connect_as", client::endpoint(hostport), "role", rolestr[asrole], "pport", pport, "pin", pin, "block", block);
    auto r = b::connect0(hostport, block);
    if (likely(r == ok)) {
        log("initiating dialogue as role '", asrole, "' pport", pport, "pin", pin, "req_data", request_data);
        initiate_dialogue(asrole, pport, pin, request_data);
    }
    log("result", r == ok ? "ok" : r);
    return r;
}

ko c::connect(const shostport_t& shostport, pport_t pport, pin_t pin, role_t role, const request_data_t& request_data, bool block) {
    return connect(ip4_encode(shostport), pport, pin, role, request_data, block);
}

void c::disconnectx(channel_t channel, seq_t seq, const reason_t& reason) {
    log("disconnect", channel, seq, reason);
    b::disconnectx(channel, seq, reason);
    cv_auth.notify_all();
}

void c::upgrade_software() {
    log("triggered upgrade_software signal!");
}

ko c::verification_completed(pport_t, pin_t, request_data_t&) {
    if (unlikely(!verification_is_fine())) {
        #if CFG_COUNTERS == 1
            ++counters.failed_verifications;
        #endif
        log("verification_not_fine");
        auto r = "KO 89742 verification_not_fine";
        log(r);
        return r;
    }
    #if CFG_COUNTERS == 1
        ++counters.successful_verifications;
    #endif
    version_fingerprint_t pv = handshakes->peer->parse_version_fingerprint();
    if (unlikely(CFG_MONOTONIC_VERSION_FINGERPRINT) == 0) {
        if (pv > 10) {
            return ok; // peer is older
        }
       //I'm older
    }
    else {
        if (pv <= CFG_MONOTONIC_VERSION_FINGERPRINT) {
            // peer is older or same
            return ok;
        }
       //I'm older
    }
    #if CFG_COUNTERS == 1
        ++counters.signals_upgrade_software;
    #endif
    upgrade_software(); //triger automatic updates check
    return ok;
}

#if CFG_COUNTERS == 1
    void c::counters_t::dump(ostream& os) const {
        os << "successful_verifications " << successful_verifications << '\n';
        os << "failed_verifications " << failed_verifications << '\n';
        os << "signals_upgrade_software " << signals_upgrade_software << '\n';
    }
#endif

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "    id: stage " << stagestr[stage_peer] << " pubk " << pubkey << " pubkh " << pubkey.hash() << " role " << rolestr[role] << " swver " << +version_fingerprint << '\n';
}

void c::dump_all(const string& prefix, ostream& os) const {
    dump(prefix, os);
    b::dump_all(prefix, os);
}

c::handshakes_t::handshakes_t(api_v_t api_v, role_t role, pport_t pport, pin_t pin, const request_data_t& request_data) {
    me = new handshake_t(api_v, role, pport, pin, request_data);
}

c::handshakes_t::handshakes_t() {
    peer = new handshake_t();
}

c::handshakes_t::handshakes_t(sigmsg_hash_t&& msg) {
    peer = new handshake_t(move(msg));
}

c::handshakes_t::~handshakes_t() {
    delete me;
    delete peer;
}

c::handshake_t::handshake_t(api_v_t api_v, role_t role, pport_t pport, pin_t pin, const request_data_t& request_data): request_data(request_data) {
    log("handshake_t constructor", role, pport, pin);
    *reinterpret_cast<uint8_t*>(&msg[0]) = CFG_MONOTONIC_VERSION_FINGERPRINT; //*reinterpret_cast<uint8_t*>(&us::vcs::codehash[0]); //LE
    *reinterpret_cast<uint8_t*>(&msg[1]) = api_v;
    *reinterpret_cast<uint16_t*>(&msg[2]) = pport; //LE
    *reinterpret_cast<uint16_t*>(&msg[4]) = pin; //LE
    msg[6] = role;
    ifstream f("/dev/urandom");
    f.read(reinterpret_cast<char*>(&msg[7]), sigmsg_hasher_t::output_size - 7);
    log("msg", msg, "version_fingerprint", +parse_version_fingerprint(), "api_v", +parse_api_v(), "pport", parse_pport(), "pin", parse_pin(), "role", parse_role());
}

c::handshake_t::handshake_t(sigmsg_hash_t&& msg): msg(move(msg)) {
    log("handshake_t sigmsg_hash_t constructor");
}

c::handshake_t::handshake_t() {
    log("handshake_t default constructor");
}

version_fingerprint_t c::handshake_t::parse_version_fingerprint() const {
    return *reinterpret_cast<const version_fingerprint_t*>(&msg[0]);
}

api_v_t c::handshake_t::parse_api_v() const {
    return *reinterpret_cast<const api_v_t*>(&msg[1]);
}

pport_t c::handshake_t::parse_pport() const {
    return *reinterpret_cast<const uint16_t*>(&msg[2]);
}

pin_t c::handshake_t::parse_pin() const {
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
    os << pfx << "version fingerprint " << +parse_version_fingerprint() << '\n';
    os << pfx << "api_v " << +parse_api_v() << '\n';
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
        log("dgram arrived without encryption. svc", e->service);
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
        if (d->service != protocol::id_verification_result) {
            auto r = "KO 30299 stage_peer != anonymous";
            log(r, stagestr[stage_peer]);
            auto seq = d->decode_sequence();
            delete d;
            disconnect(seq, r);
            return true;
        }
    }
    log("process_work", "using keys at", &get_keys(), get_keys().pub, "svc", d->service);
    using namespace us::gov::protocol;
    #ifdef has_us_gov_id_api
        switch(d->service) {
            #include <us/api/generated/gov/c++/id/hdlr_svc-router>
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

#include <us/api/generated/gov/c++/id/hdlr_svc_handler-impl>


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

ko c::initiate_dialogue(role_t role, pport_t pport, pin_t pin, const request_data_t& req_data) {
    /*
    initiate_dialogue() - Executed by Initiator
    -------------------
    hs(me,peer)=(new,0)
    send (hs.me)      ----------------------gov_id_request2---------->
    */
    log("initiate_dialogue as ", rolestr[role], pport, pin, req_data);
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
    assert(daemon.api_v != 0);
    handshakes = new handshakes_t(daemon.api_v, role, pport, pin, req_data);
    logdump("  initiate_dialogue: my handshake: ", *handshakes->me);
    log("sending id_request", protocol::id_request, handshakes->me->msg, "role", rolestr[handshakes->me->parse_role()]);
    assert(role == handshakes->me->parse_role());
    return call_request(handshakes->me->msg);
}

//------------------apitool - API Spec defined @ us/api/generated/gov/c++/id/hdlr_local-impl

ko c::handle_request(seq_t seq, sigmsg_hash_t&& msg) {
    log("request2", seq);
    /*
     - Executed by Follower
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
    handshakes = new handshakes_t(move(msg));
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
    peer_api_v = handshakes->peer->parse_api_v();
    log("peer api_v", +peer_api_v);
    handshakes->me = new handshake_t(daemon.api_v, role, static_cast<daemon_t&>(daemon).pport, 0, request_data_t());
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
    log("peer_challenge2", seq);
    /// in:
    ///     sigmsg_hash_t msg;
    ///     pub_t pub;
    ///     sig_t sig;
    ///     sig_der_t sig_der;

    /*
     - Executed by Initiator
    process_peer_challenge <-------------gov_id_peer_challenge--------
    ----------------------
    hs(me,peer)=(*,new)
    verify signature
    sign(hs.peer) msg:=(role,pport,random)
    send (signature)-------------------gov_id_challenge_response----->
    */
    auto& mykeys = get_keys();
    log("process_peer_challenge2. seq", seq);
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
    handshakes->peer = new handshake_t(move(o_in.msg));
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

    peer_api_v = handshakes->peer->parse_api_v();
    log("peer api_v", +peer_api_v);

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
        log("sending id_challenge_response", protocol::id_challenge2_response, mykeys.pub, sig.to_b58());
        ko r;
        if (daemon.api_v == handshakes->peer->parse_api_v()) {
            r = call_challenge2_response(challenge2_response_in_t(mykeys.pub, sig, sig_der, handshakes->me->request_data));
        }
        else {
            r = call_challenge_response(challenge_response_in_t(mykeys.pub, sig, sig_der));
        }
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

    request_data_t follower_request_data = ""; // Follower is not sending request_data
    request_data_t new_request_data = follower_request_data; // Follower is not sending request_data
    {
        auto r = verification_completed(handshakes->peer->parse_pport(), handshakes->peer->parse_pin(), new_request_data); // Deliver to upper logic layers. We havent collected request_data from non-initiator peer TODO (Exchangue capabilities: Verificator Address-Range, CPU, Disc, bandwidth capacity/free)
        if (is_ko(r)) {
            disconnect(seq, r);
            return r;
        }
    }
    if (new_request_data != follower_request_data) {
        log("sending verification_result", protocol::id_verification_result, new_request_data);
        call_verification_result(move(new_request_data));
    }
    
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
     - Executed by Follower
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
    request_data_t request_data;
    auto r = verification_completed(handshakes->peer->parse_pport(), handshakes->peer->parse_pin(), request_data);
    delete handshakes;
    handshakes = nullptr;
    if (is_ko(r)) {
        disconnect(seq, r);
        return r;
    }
    log("signaling VERIF_COMPLETED.");
    cv_auth.notify_all();
    return ok;
}

ko c::handle_challenge2_response(seq_t seq, challenge2_response_in_dst_t&& o_in) {
    log("challenge2_response", seq);
    /// in:
    ///     pub_t pub;
    ///     sig_t sig;
    ///     sig_der_t sig_der;
    ///     request_data_t request_data;

    /*
     - Executed by Follower
            -------------------gov_id_challenge_response-----> process_challenge_response
                                                               --------------------------
                                                               verify signature
                                                               completed(hs.peer.pport)
    */
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
    auto new_request_data = o_in.request_data;
    {
        auto r = verification_completed(handshakes->peer->parse_pport(), handshakes->peer->parse_pin(), new_request_data); //deliver to upper logic layers
        if (is_ko(r)) {
            disconnect(seq, r);
            return r;
        }
    }
    if (new_request_data != o_in.request_data) {
        log("sending verification_result", protocol::id_verification_result, new_request_data);
        call_verification_result(move(new_request_data));
    }
    delete handshakes;
    handshakes = nullptr;
    log("signaling VERIF_COMPLETED.");
    cv_auth.notify_all();
    return ok;
}

ko c::handle_verification_result(seq_t seq, request_data_t&& request_data) {
    log("verification_result", seq);
    verification_result(move(request_data));
    return ok;
}

//-/----------------apitool - End of API implementation.

#include <us/api/generated/gov/c++/id/cllr_rpc-impl>

#endif

