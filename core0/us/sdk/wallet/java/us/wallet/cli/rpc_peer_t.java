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
package us.wallet.cli;
import us.CFG;                                                                                 // CFG
import us.gov.socket.datagram;                                                                 // datagram
import us.gov.id.types.*;                                                                      // *
import us.gov.socket.types.*;                                                                  // *
import us.ko;                                                                                  // ko
import java.io.PrintStream;                                                                    // PrintStream
import us.gov.protocol;                                                                        // protocol
import java.security.PublicKey;                                                                // PublicKey
import java.net.Socket;                                                                        // Socket

public class rpc_peer_t extends us.wallet.engine.rpc_peer_t {

    static void log(final String line) {                        //--strip
        CFG.log_wallet_cli("rpc_peer_t: " + line);              //--strip
    }                                                           //--strip

    public rpc_peer_t(rpc_daemon_t rpc_daemon) {
        super(rpc_daemon);
    }

    public rpc_peer_t(rpc_daemon_t rpc_daemon, Socket sock) {
        super(rpc_daemon, sock);
    }

    @Override public boolean authorize(PublicKey p, pin_t pin) { return true; }

    @Override public void on_peer_disconnected(final reason_t reason) {
        super.on_peer_disconnected(reason);
        ((rpc_daemon_t)daemon).on_peer_disconnected(reason);
    }

    @Override public void on_I_disconnected(final reason_t reason) {
        super.on_I_disconnected(reason);
        ((rpc_daemon_t)daemon).on_I_disconnected(reason);
    }

    @Override public void verification_completed(final pport_t rpport, final pin_t pin) {
        super.verification_completed(rpport, pin);
        ((rpc_daemon_t)daemon).verification_completed(verification_is_fine());
    }

}

