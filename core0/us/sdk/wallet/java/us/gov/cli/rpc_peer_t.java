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
import static us.gov.crypto.types.*;                                                           // *
import static us.gov.id.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import us.ko;                                                                                  // ko
import java.io.PrintStream;                                                                    // PrintStream
import java.security.PublicKey;                                                                // PublicKey
import java.net.Socket;                                                                        // Socket

public class rpc_peer_t extends us.gov.engine.rpc_peer_t {

    static void log(final String line) {                        //--strip
        CFG.log_gov_cli("rpc_peer_t: " + line);                 //--strip
    }                                                           //--strip

    public rpc_peer_t(rpc_daemon_t rpc_daemon) {
        super(rpc_daemon);
    }

    public rpc_peer_t(rpc_daemon_t rpc_daemon, Socket sock) {
        super(rpc_daemon, sock);
    }

    @Override public void dump(final String prefix, PrintStream os) {
        os.println(prefix + "cli::rpc_peer_t");
        super.dump(prefix, os);
    }

    @Override public ko authorize(final PublicKey p, final pin_t pin, request_data_t request_data) { return ko.ok; }

    @Override public void on_peer_disconnected(final reason_t reason) {
        super.on_peer_disconnected(reason);
        log("on_peer_disconnected"); //--strip
        ((rpc_daemon_t)daemon).on_peer_disconnected(reason);
    }

    @Override public void on_I_disconnected(final reason_t reason) {
        super.on_I_disconnected(reason);
        log("on_I_disconnected"); //--strip
        ((rpc_daemon_t)daemon).on_I_disconnected(reason);
    }

    @Override public ko verification_completed(final pport_t rpport, final pin_t pin, request_data_t request_data) {
        ko r = super.verification_completed(rpport, pin, request_data);
        if (ko.is_ko(r)) {
            return r;
        }
        log("verification_completed"); //--strip
        ((rpc_daemon_t)daemon).verification_completed(verification_is_fine());
        return r;
    }

    @Override public void verification_result(request_data_t request_data) {
        ((rpc_daemon_t)daemon).verification_result(request_data);
    }

    @Override public void upgrade_software() {
        log("triggered upgrade_software signal!"); //--strip
        ((rpc_daemon_t)daemon).upgrade_software();
    }

}

