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
package us.gov.cli;
import us.CFG;                                                                                 // CFG
import us.gov.socket.client;                                                                   // client
import static us.gov.socket.datagram.dispatcher_t;                                             // dispatcher_t
import us.gov.crypto.ec;                                                                       // ec
import static us.gov.id.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import static us.ko.is_ko;                                                                     // is_ko
import java.security.KeyPair;                                                                  // KeyPair
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import java.io.PrintStream;                                                                    // PrintStream
import static us.gov.id.peer_t.role_t;                                                         // role_t
import us.gov.io.screen;                                                                       // screen
import java.util.Stack;                                                                        // Stack
import static us.gov.socket.types.channel_t;                                             // dispatcher_t

public class rpc_daemon_t extends us.gov.engine.rpc_daemon_t {

    static void log(final String line) {                        //--strip
        CFG.log_gov_cli("rpc_daemon_t: " + line);               //--strip
    }                                                           //--strip

    public rpc_daemon_t(final KeyPair keys, channel_t channel, final shostport_t shostport, final role_t role, dispatcher_t dispatcher) {
        super(channel, dispatcher);
        this.id = keys;
        this.shostport = shostport;
        this.role = role;
        this.parent = null;
        log("constructor"); //--strip
    }

    public rpc_daemon_t(hmi parent, final KeyPair keys, final shostport_t shostport, final role_t role, dispatcher_t dispatcher) {
        super(parent.p.channel, dispatcher);
        this.id = keys;
        this.shostport = shostport;
        this.role = role;
        this.parent = parent;
        log("constructor-hmi"); //--strip
    }

    @Override public us.gov.socket.client create_client() {
        log("create_client"); //--strip
        return new rpc_peer_t(this);
    }

    @Override public ko connect() {
        log("connect"); //--strip
        return connect(new pin_t(0));
    }

    public ko connect(pin_t pin) {
        log("connect with pin "+ pin.value); //--strip
        ko r = get_peer().connect(shostport, new pport_t(0), pin, role, true);
        if (is_ko(r)) {
            return r;
        }
        peer.disable_recv_timeout();
        handler.signal_connected();
        return ok;
    }

    @Override public void on_connect(client peer, ko r) {
        log("on_connect" + (is_ko(r) ? r.msg : "ok")); //--strip
        super.on_connect(peer, r);
        if (parent != null) parent.on_connect(r);
    }

    public void verification_completed(boolean verif_ok) {
        log("verification_completed " + verif_ok); //--strip
        if (parent != null) parent.verification_completed(verif_ok);
    }

    public void on_I_disconnected(reason_t reason) {
        log("on_I_disconnected. Reason: " + reason.value); //--strip
        if (parent != null) parent.on_I_disconnected(reason);
    }

    public void on_peer_disconnected(reason_t reason) {
        log("peer disconnected. Reason: " + reason.value); //--strip
        if (parent != null) parent.on_peer_disconnected(reason);
    }

    public static void apihelp(final String prefix, PrintStream os) {
        os.println(prefix + "track <xtime>           Tracking info for transaction.");
        os.println(prefix + "n|nodes                 Node list.");
        os.println(prefix + "lookup_node <addr>      Node info by addresas.");
        os.println(prefix + "w|wallets               Wallet list.");
        os.println(prefix + "lookup_wallet <addr>    Wallet info by address.");
        os.println(prefix + "fetch_accounts {<addr>} Accounts by addreses.");
    }

    @Override public void on_stop() {
        log("on_stop"); //--strip
        super.on_stop();
        if (parent != null) parent.on_stop();
    }

    @Override public KeyPair get_keys() { return id; }

    public rpc_peer_t get_peer() { return (rpc_peer_t)peer; }

    public KeyPair id;
    public shostport_t shostport;
    public role_t role;

    hmi parent;
}

