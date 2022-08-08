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
package us.gov.id;
import java.util.Arrays;                                                                       // Arrays
import us.gov.crypto.base58;                                                                   // base58
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import java.nio.ByteBuffer;                                                                    // ByteBuffer
import java.nio.ByteOrder;                                                                     // ByteOrder
import us.CFG;                                                                                 // CFG
import java.util.concurrent.locks.Condition;                                                   // Condition
import us.gov.socket.datagram;                                                                 // datagram
import java.util.Date;                                                                         // Date
import us.gov.crypto.ec;                                                                       // ec
import java.io.File;                                                                           // File
import java.io.FileInputStream;                                                                // FileInputStream
import java.io.FileNotFoundException;                                                          // FileNotFoundException
import java.security.GeneralSecurityException;                                                 // GeneralSecurityException
import static us.gov.crypto.types.*;                                                           // *
import static us.gov.id.types.*;                                                               // *
import static us.gov.io.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import us.stdint.*;                                                                            // *
import us.tuple.*;                                                                             // *
import java.io.InputStream;                                                                    // InputStream
import java.security.spec.InvalidKeySpecException;                                             // InvalidKeySpecException
import java.io.IOException;                                                                    // IOException
import java.security.KeyPair;                                                                  // KeyPair
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import java.security.PrivateKey;                                                               // PrivateKey
import us.gov.protocol;                                                                        // protocol
import java.security.PublicKey;                                                                // PublicKey
import java.util.concurrent.locks.ReentrantLock;                                               // ReentrantLock
import us.gov.crypto.sha256;                                                                   // sha256
import java.net.Socket;                                                                        // Socket
import us.string;                                                                              // string
import us.gov.crypto.symmetric_encryption;                                                     // symmetric_encryption
import java.util.concurrent.TimeUnit;                                                          // TimeUnit

public class peer_t extends us.gov.socket.peer_t implements api {

    static void log(final String line) {              //--strip
        CFG.log_gov_id("peer_t: " + line);            //--strip
    }                                                 //--strip

    public static ko KO_6017 = new ko("KO 6017 authentication failed.");

    public enum stage_t {
        anonymous,
        verified,
        verified_fail,
        num_stages;

        public short asShort() {
            return (short)ordinal();
        }

        public static stage_t fromShort(short i) {
            return stage_t.values()[i];
        }
    }

    public static final String[] stagestr = {"anonymous", "verified", "verified_fail"};

    public enum role_t {
        role_peer,
        role_sysop,
        role_device,
        num_roles;

        public byte code() {
            return (byte)ordinal();
        }

        public static final String[] rolestr = {"peer", "sysop", "device"};

        public String str() {
            return rolestr[ordinal()];
        }

        public static role_t from_byte(byte b) {
            return role_t.values()[(int)b];
        }
    }

    public peer_t(rpc_daemon_t rpc_daemon) {
        super(rpc_daemon);
    }

    public peer_t(rpc_daemon_t rpc_daemon, Socket sock) {
        super(rpc_daemon, sock);
    }

    static class handshake_t {
        handshake_t(role_t role, pport_t pport, pin_t pin) {
            log("get_random_message"); //--strip
            try {
                File file = new File("/dev/urandom");
                FileInputStream urandom = new FileInputStream(file);
                if (urandom.read(msg.value) != 32) {
                    log("I am dead. /dev/urandom"); //--strip
                    throw new RuntimeException("/dev/urandom not readable?");
                }
            }
            catch (Exception e) {
                throw new RuntimeException("/dev/urandom not readable?");
            }
            version_fingerprint_t version_fingerprint = new version_fingerprint_t(0); //TODO version gov
            encode(version_fingerprint, 0);
            encode(pport, 2);
            encode(pin, 4);
            msg.value[6] = role.code();
            log("created message " + msg.b58()); //--strip
            log("pport " + parse_pport().value); //--strip
            log("pin " + parse_pin().value); //--strip
            log("version fingerprint " + parse_version_fingerprint().value); //--strip
            log("role " + parse_role().str()); //--strip
        }

        handshake_t() {
        }

        void encode(final uint16_t v, int offset) {
            v.write(msg.value, offset);
        }

        version_fingerprint_t parse_version_fingerprint() {
            version_fingerprint_t o = new version_fingerprint_t();
            o.read(msg.value, 0);
            return o;
        }

        pport_t parse_pport() {
            pport_t o = new pport_t();
            o.read(msg.value, 2);
            return o;
        }

        pin_t parse_pin() {
            pin_t o = new pin_t();
            o.read(msg.value, 4);
            return o;
        }

        role_t parse_role() {
            return role_t.from_byte(msg.value[6]);
        }

        sha256.hash_t msg = new sha256.hash_t();
    }

    static class handshakes_t {
        handshakes_t(role_t role, pport_t pport, pin_t pin) {
            me = new handshake_t(role, pport, pin);
        }
        handshakes_t() {
            peer = new handshake_t();
        }
        handshake_t me = null;
        handshake_t peer = null;
    };

    public ko connect(final shostport_t shostport, final pport_t pport, final pin_t pin, final role_t role, boolean block) {
        log("connect_as " + endpoint(shostport) +" role " + role.str() + " pport " + pport.value + " pin " + pin.value + " block " + block); //--strip
        ko r = super.connect0(shostport, block);
        if (ko.is_ok(r)) {
            initiate_dialogue(role, pport, pin);
        }
        return r;
    }

    public ko connect(final String ipport, final pport_t pport, final pin_t pin, final role_t role, boolean block) {
        log("connect " + ipport + " " + pport.value + " " + pin.value+" " + role.str()); //--strip
        pair<ko, shostport_t> r = parse_endpoint(ipport);
        if (ko.is_ko(r.first)) {
            on_connect(r.first);
            return r.first;
        }
        return connect(r.second, pport, pin, role, block);
    }

    @Override public boolean process_work(datagram d) {
        log("process_work"); //--strip
        assert(d.service.value < protocol.id_end);
        if (d.service.value < protocol.id_begin) {
            return super.process_work(d);
        }
        if (stage_peer != stage_t.anonymous) {
            ko r = new ko("KO 30299 stage_peer != anonymous");
            log(r.msg + " " + stagestr[stage_peer.ordinal()]); //--strip
            seq_t seq = d.decode_sequence();
            disconnect(seq, new reason_t(r.msg));
            return true;
        }
        switch(d.service.value) {
            //#include <us/api/generated/java/gov/id/hdlr_svc-router>
            //------------------------------------------------------------__begin__------generated by configure, do not edit.
            //content of file: <us/api/generated/java/gov/id/hdlr_svc-router>
            // id - master file: us/api/data/gov/id

            case protocol.id_request: { return process_async_api__id_request(d); }
            case protocol.id_peer_challenge: { return process_async_api__id_peer_challenge(d); }
            case protocol.id_challenge_response: { return process_async_api__id_challenge_response(d); }
            //-/----------------------------------------------------------___end___------generated by configure, do not edit.

            default:
                log("Unidentified service. "+ d.service.value + ". Unhandled dgram.");  //--strip
        }
        ko r = new ko("KO 30298 service handled here in id::peer");
        log(r.msg + " " + d.service.value); //--strip
        seq_t seq = d.decode_sequence();
        disconnect(seq, new reason_t(r.msg));
        return true;
    }
//#include <us/api/generated/java/gov/id/hdlr_svc_handler-impl>
//------------------------------------------------------------__begin__------generated by configure, do not edit.
//content of file: <us/api/generated/java/gov/id/hdlr_svc_handler-impl>
// id - master file: us/api/data/gov/id

boolean process_async_api__id_request(datagram d) {
    log("protocol.id_request"); //--strip
    sha256.hash_t msg = new sha256.hash_t();
    {
        ko r = blob_reader_t.readD(d, msg);
        if (ko.is_ko(r)) {
            log(r.msg); //--strip
            return true; //processed
        }
    }
    {
        ko r = handle_request(d.decode_sequence(), msg);
        if (ko.is_ko(r)) {
            process_ko_work(d.decode_channel(), d.decode_sequence(), r);
            return true; //processed
        }
    }
    return true; //processed
}

boolean process_async_api__id_peer_challenge(datagram d) {
    log("protocol.id_peer_challenge"); //--strip
    peer_challenge_in_dst_t o_in = new peer_challenge_in_dst_t();
    {
        ko r = blob_reader_t.readD(d, o_in);
        if (ko.is_ko(r)) {
            log(r.msg); //--strip
            return true; //processed
        }
    }
    {
        ko r = handle_peer_challenge(d.decode_sequence(), o_in);
        if (ko.is_ko(r)) {
            process_ko_work(d.decode_channel(), d.decode_sequence(), r);
            return true; //processed
        }
    }
    return true; //processed
}

boolean process_async_api__id_challenge_response(datagram d) {
    log("protocol.id_challenge_response"); //--strip
    challenge_response_in_dst_t o_in = new challenge_response_in_dst_t();
    {
        ko r = blob_reader_t.readD(d, o_in);
        if (ko.is_ko(r)) {
            log(r.msg); //--strip
            return true; //processed
        }
    }
    {
        ko r = handle_challenge_response(d.decode_sequence(), o_in);
        if (ko.is_ko(r)) {
            process_ko_work(d.decode_channel(), d.decode_sequence(), r);
            return true; //processed
        }
    }
    return true; //processed
}
//-/----------------------------------------------------------___end___------generated by configure, do not edit.

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
public ko initiate_dialogue(final role_t role, final pport_t pport, final pin_t pin) {  //role '0'peer; '1'sysop; '2'device
    /*
    initiate_dialogue()
    -------------------
    hs(me,peer)=(new,0)
    send (hs.me)      ----------------------gov_id_request---------->
    */
    log("Initiate dialogue. Role=" + role.str() + " pport " + pport.value + " pin " + pin.value);  //--strip
    if (se != null) {
        log("Found existing encryption module. destroying it.");  //--strip
        se = null;
    }
    if (handshakes != null) {
        log("Found existing handshakes object. destroying it."); //--strip
    }
    log("reseting state."); //--strip
    set_stage_peer(stage_t.anonymous);
    pubkey = null;
    handshakes = new handshakes_t(role, pport, pin);
    log("Sending gov_id_request " + protocol.id_request);  //--strip
    assert role == handshakes.me.parse_role();
    return call_request(handshakes.me.msg);
}

//------------------apitool - API Spec defined @ us/api/generated/java/gov/id/hdlr_local-impl

@Override public ko handle_request(final seq_t seq, final sha256.hash_t msg) {
    log("request " + seq.value); //--strip
    /*
              ----------------------gov_id_request---------->  process_request()
                                                               -----------------
                                                               hs(me,peer)=(0,new)
                                                               sign(hs.peer) msg:=(role,pport,random))
                                                               hs(me,peer)=(new,*)
                   <-------------gov_id_peer_challenge-------- send (signature,hs.me)
    */

    KeyPair mykeys = get_keys();
    if (handshakes != null) {
        handshakes = null;
/*
        ko r = new ko("KO 99101 Invalid handshake. Bad sequence.");
        log(r.msg); //--strip
        set_stage_peer(stage_t.verified_fail);
        disconnect(seq, new reason_t(r.msg));
        return r;
*/
    }
    handshakes = new handshakes_t();
    handshakes.peer.msg = msg;
    role_t peer_role = handshakes.peer.parse_role();
    if (peer_role != role_t.role_peer && peer_role != role_t.role_sysop && peer_role != role_t.role_device) {
        ko r = new ko("KO 75690 Invalid role.");
        log(r.msg); //--strip
        set_stage_peer(stage_t.verified_fail);
        disconnect(seq, new reason_t(r.msg));
        return r;
    }
    role = peer_role;
    log("role " + role); //--strip
    sig_der_t sig_der = ec.instance.sign(mykeys.getPrivate(), handshakes.peer.msg);
    if (sig_der == null) {
        ko r = new ko("KO 76054 Problem signing.");
        log(r.msg); //--strip
        set_stage_peer(stage_t.verified_fail);
        disconnect(seq, new reason_t(r.msg));
        return r;
    }
    pport_t daemon_pport = new pport_t(0);
    if (daemon_pport.value == 0) {
        ko r = new ko("KO 20919 This end is not receiving connections.");
        log(r.msg); //--strip
        set_stage_peer(stage_t.verified_fail);
        disconnect(seq, new reason_t(r.msg));
        return r;
    }
    handshakes.me = new handshake_t(role, daemon_pport, new pin_t(0));
    log("sending out response handshake"); //--strip
    sig_t sig = new sig_t();
    return call_peer_challenge(new peer_challenge_in_t(handshakes.me.msg, new pub_t(mykeys.getPublic()), sig, sig_der));
}

@Override public ko handle_peer_challenge(final seq_t seq, final peer_challenge_in_dst_t o_in) {
    log("peer_challenge " + seq.value); //--strip
    /// in:
    ///     sha256.hash_t msg;
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

    log("process_peer_challenge"); //--strip
    KeyPair mykeys = get_keys();
    if (handshakes == null) {
        ko r = new ko("KO 84410 Invalid handshakes object.");
        log(r.msg);  //--strip
        set_stage_peer(stage_t.verified_fail);
        disconnect(seq, new reason_t(r.msg));
        return r;
    }
    if (handshakes.peer != null) {
        ko r = new ko("KO 44930 Invalid object handshakes->peer");
        log(r.msg);  //--strip
        set_stage_peer(stage_t.verified_fail);
        disconnect(seq, new reason_t(r.msg));
        return r;
    }
    handshakes.peer = new handshake_t();
    handshakes.peer.msg = o_in.msg;
    pubkey = o_in.pub.value;
    role_t peer_role = handshakes.peer.parse_role();
    if (peer_role != role_t.role_peer && peer_role != role_t.role_sysop && peer_role != role_t.role_device) {
        ko r = new ko("KO 72010 Invalid role.");
        log(r.msg); //--strip
        set_stage_peer(stage_t.verified_fail);
        disconnect(seq, new reason_t(r.msg));
        return r;
    }
    role = peer_role;
    try {
        if (!ec.instance.verify(pubkey, handshakes.me.msg, o_in.sig_der)) {
            ko r = new ko("KO 20147 Invalid signature.");
            log(r.msg + " " + handshakes.me.msg.b58() + " " + base58.encode(o_in.sig.value)); //--strip
            set_stage_peer(stage_t.verified_fail);
            pubkey = null;
            disconnect(seq, new reason_t(r.msg));
            return r;
        }
        set_stage_peer(stage_t.verified);
    }
    catch (Exception e) {
        ko r = new ko("KO 20148 Invalid signature.");
        log(r.msg + " " + handshakes.me.msg.b58() + " " + base58.encode(o_in.sig.value) + " " + e.getMessage()); //--strip
        set_stage_peer(stage_t.verified_fail);
        pubkey = null;
        disconnect(seq, new reason_t(r.msg));
        return r;
    }
    sig_der_t sig_der = ec.instance.sign(mykeys.getPrivate(), handshakes.peer.msg);
    if (sig_der == null) {
        ko r = new ko("KO 76054 Problem signing.");
        log(r.msg); //--strip
        set_stage_peer(stage_t.verified_fail);
        disconnect(seq, new reason_t(r.msg));
        return r;
    }
    sig_t sig = new sig_t();
    {
        ko r = call_challenge_response(new challenge_response_in_t(new pub_t(mykeys.getPublic()), sig, sig_der));
        if (ko.is_ko(r)) {
            log(r.msg); //--strip
            set_stage_peer(stage_t.verified_fail);
            disconnect(seq, new reason_t(r.msg));
            return r;
        }
    }
    {
        ko r = turn_on_encryption();
        if (ko.is_ko(r)) {
            log(r.msg); //--strip
            set_stage_peer(stage_t.verified_fail);
            disconnect(seq, new reason_t(r.msg));
            return r;
        }
    }
    log("signaling verification_completed " + handshakes.peer.parse_pport().value); //--strip
    verification_completed(handshakes.peer.parse_pport(), handshakes.peer.parse_pin());
    handshakes = null;
    log("Signaling VERIF_COMPLETED. cv_auth");  //--strip
    mx_auth.lock();
    try {
        cv_auth.signalAll();
    }
    finally {
        mx_auth.unlock();
        log("unlocked mx_auth :TID" + Thread.currentThread().getId());  //--strip
    }
    log("Signaled VERIF_COMPLETED. cv_auth"); //--strip
    return ok;
}

@Override public ko handle_challenge_response(final seq_t seq, final challenge_response_in_dst_t o_in) {
    log("challenge_response " + seq.value); //--strip
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

    log("process_challenge_response"); //--strip
    if (handshakes == null) {
        ko r = new ko("KO 75040 Invalid handshake.");
        log(r.msg); //--strip
        set_stage_peer(stage_t.verified_fail);
        disconnect(seq, new reason_t(r.msg));
        return r;
    }
    if (handshakes.peer == null) {
        ko r = new ko("KO 63201 handshakes->peer==nullptr");
        log(r.msg); //--strip
        set_stage_peer(stage_t.verified_fail);
        disconnect(seq, new reason_t(r.msg));
        return r;
    }
    pubkey = o_in.pub.value;
    try {
        if (!ec.instance.verify(pubkey, handshakes.me.msg, o_in.sig_der)) {
            ko r = new ko("KO 10210 Invalid signature.");
            log(r.msg); //--strip
            set_stage_peer(stage_t.verified_fail);
            disconnect(seq, new reason_t(r.msg));
            return r;
        }
    }
    catch(Exception e) {
        ko r = new ko("KO 10211 Invalid signature.");
        log(r.msg); //--strip
        set_stage_peer(stage_t.verified_fail);
        disconnect(seq, new reason_t(r.msg));
        return r;
    }
    set_stage_peer(stage_t.verified);
    {
        ko r = turn_on_encryption();
        if (ko.is_ko(r)) {
            log(r.msg); //--strip
            disconnect(seq, new reason_t(r.msg));
            return r;
        }
    }
    verification_completed(handshakes.peer.parse_pport(), handshakes.peer.parse_pin());
    log("Signaling VERIF_COMPLETED. cv_auth"); //--strip
    handshakes = null;
    mx_auth.lock();
    try {
        cv_auth.signalAll();
    }
    finally {
        mx_auth.unlock();
    }
    log("Signaled VERIF_COMPLETED. cv_auth");  //--strip
    return ok;
}

//-/----------------apitool - End of API implementation.

    public final KeyPair get_keys() {
        log("get_keys"); //--strip
        return ((daemon0_t)daemon).get_keys();
    }

    public ko turn_on_encryption() {
        log("turn_on_encryption"); //--strip
        if (se != null) {
            log("WARNING 37290 Encryption is already ON."); //--strip
            return ok;
        }
        if (get_stage_peer() != stage_t.verified) {
            ko r = new ko("KO 37190 Encryption cannot be turned on.");
            log(r.msg); //--strip
            return r;
        }
        se = new symmetric_encryption();
        ko r = se.init(get_keys().getPrivate(), pubkey);
        if (ko.is_ko(r)) {
            se = null;
            return r;
        }
        if (daemon.outbound_traffic__goes_encrypted()) {                                        //--strip
            log("================== ENCRYPTION TURNED ON ==================");                  //--strip
        }                                                                                       //--strip
        else {                                                                                  //--strip
            log("================== ENCRYPTION TURNED send:OFF recv:ON  ==================");   //--strip
        }                                                                                       //--strip
        return ok;
    }

    @Override public pair<ko, datagram> encrypt0(datagram d) {
        log("encrypt datagram"); //--strip;
        svc_t svc = d.decode_service(); //pre-decoded field (service) is 0 when sending
        if (svc.value == 0) {
            log("Datagram is already encrypted. d.service is 0. length " + d.size()); //--strip;
            return new pair<ko, datagram>(ok, d);
        }
        if (svc.value < protocol.id_end) {
            boolean clear_text = true;
            if (svc.value < protocol.id_begin) {
                if (svc.value != protocol.socket_ping && svc.value != protocol.socket_ping_response) {
                    clear_text = false;
                }
            }
            if (clear_text) {
                log("Not encrypting. Service " + svc.value + " forced to go cleartext. (auth handshake or ping)"); //--strip
                return new pair<ko, datagram>(ok, d);
            }
        }
        if (se == null) {
            log("Not encrypting. Encryption not turned on"); //--strip;
            return new pair<ko, datagram>(ok, d);
        }
        log("outbound_traffic__goes_encrypted " + daemon.outbound_traffic__goes_encrypted()); //--strip
        pair<ko, datagram> r;
        if (daemon.outbound_traffic__goes_encrypted()) {
            r = d.encrypt(se);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
            }
            if (d != r.second) {
                log("datagram encrypted. svc " + svc.value + " length " + d.size() + " -> svc " + r.second.decode_service().value + " length " + r.second.size()); //--strip;
            }
        }
        else {
            r = new pair<ko, datagram>(ok, d);
        }
        return r;
    }

    @Override
    public pair<ko, datagram> decrypt0(datagram e) {
        log("decrypt datagram"); //--strip;
        if (e.service.value != 0) {
            log ("dgram arrived without encryption. svc" + e.service.value); //--strip
            return new pair<ko, datagram>(ok, e);
        }
        if (se == null) {
            log("encryption is not turned on"); //--strip
            return new pair<ko, datagram>(ok, e);
        }
        pair<ko, datagram> d = e.decrypt(daemon.channel, se);
        if (ko.is_ko(d.first)) { //--strip
            assert d.second == null; //--strip
        } //--strip
        return d;
    }

    public boolean verification_is_fine() { return get_stage_peer() == stage_t.verified; }

    public void verification_completed(final pport_t rpport, final pin_t pin) {}

    boolean is_role_peer() { return role == role_t.role_peer; }
    boolean is_role_sysop() { return role == role_t.role_sysop; }
    boolean is_role_device() { return role == role_t.role_device; }

    void set_stage_peer(stage_t s) {
        mx_auth.lock();
        try {
            stage_peer = s;
        }
        finally {
            mx_auth.unlock();
        }
    }

    stage_t get_stage_peer() {
        mx_auth.lock();
        try {
            return stage_peer;
        }
        finally {
            mx_auth.unlock();
        }
    }

    String dbg_durprint(Date fut) {    //--strip
        long nowms = System.currentTimeMillis(); //--strip
        return "" + (fut.getTime() - nowms) + " ms"; //--strip
    } //--strip

    public ko wait_auth() {
        log("wait_auth " + CFG.AUTH_TIMEOUT_SECS); //--strip
        Date deadline = new Date(since + CFG.AUTH_TIMEOUT_SECS * 1000);
        ko r = ok;
        mx_auth.lock();
        try {
            if (stage_peer == stage_t.verified || stage_peer == stage_t.verified_fail) {
                log("auth finished"); //--strip
                return ok;
            }
            while (new Date().compareTo(deadline) < 0) {
                if (stage_peer == stage_t.verified || stage_peer == stage_t.verified_fail) {
                    break;
                }
                log("cv_auth.wait " + dbg_durprint(deadline) + " secs"); //--strip
                cv_auth.awaitUntil(deadline);
                log("/cv_auth.wait"); //--strip
            }
            r = (stage_peer == stage_t.verified) ? ok : KO_6017;
        }
        catch(InterruptedException e) {
            r = new ko("KO 40309 IO Exception");
            log(r.msg); //--strip
        }
        finally {
            mx_auth.unlock();
            log("unlocked mx_auth :TID" + Thread.currentThread().getId()); //--strip
        }
//        log("mx_auth should not be Locked. "+ mx_auth.isLocked()+" :TID"+Thread.currentThread().getId()); //--strip
        log("wait_auth returned " + (ko.is_ok(r) ? "ok" : r.msg)); //--strip
        return r;
    }

    String short_version() {
        byte[] arr = new byte[] { (byte)(version_fingerprint.value >>> 8), (byte)(version_fingerprint.value & 0xFF) };
        return base58.encode(arr);
    }

    //#include <us/api/generated/java/gov/id/cllr_rpc-impl>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/gov/id/cllr_rpc-impl>
    @Override public ko call_request(final sha256.hash_t msg) {
        log("request "); //--strip
        return send1(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.id_request), new seq_t(0), msg));
    }

    @Override public ko call_peer_challenge(final peer_challenge_in_t o_in) {
        log("peer_challenge "); //--strip
        /// in:
        ///     sha256.hash_t msg;
        ///     pub_t pub;
        ///     sig_t sig;
        ///     sig_der_t sig_der;

        return send1(o_in.get_datagram(daemon.channel, new seq_t(0)));
    }

    @Override public ko call_challenge_response(final challenge_response_in_t o_in) {
        log("challenge_response "); //--strip
        /// in:
        ///     pub_t pub;
        ///     sig_t sig;
        ///     sig_der_t sig_der;

        return send1(o_in.get_datagram(daemon.channel, new seq_t(0)));
    }
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.

    @Override public void dump(final String prefix, PrintStream os) {
        os.println(prefix + "id::peer_t: idle(recv) ");
        super.dump(prefix, os);
    }


    public stage_t stage_peer = stage_t.anonymous;
    handshakes_t handshakes = null;
    role_t role = role_t.role_peer;
    public PublicKey pubkey = null;
    version_fingerprint_t version_fingerprint = new version_fingerprint_t(0);
    ReentrantLock mx_auth = new ReentrantLock();
    Condition cv_auth = mx_auth.newCondition();
    symmetric_encryption se = null;
}

