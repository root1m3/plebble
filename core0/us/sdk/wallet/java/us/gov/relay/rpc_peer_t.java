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
package us.gov.relay;
import us.CFG;                                                                                 // CFG
import us.gov.socket.datagram;                                                                 // datagram
import static us.gov.socket.types.*;                                                           // *
import us.gov.id.types.*;                                                                      // *
import us.ko;                                                                                  // ko
import java.io.PrintStream;                                                                    // PrintStream
import us.gov.protocol;                                                                        // protocol
import java.security.PublicKey;                                                                // PublicKey
import java.net.Socket;                                                                        // Socket

public abstract class rpc_peer_t extends us.gov.peer.rpc_peer_t implements caller_api {

    static void log(final String line) {                        //--strip
        CFG.log_gov_relay("rpc_peer_t: " + line);               //--strip
    }                                                           //--strip

    public rpc_peer_t(rpc_daemon_t rpc_daemon) {
        super(rpc_daemon);
    }

    public rpc_peer_t(rpc_daemon_t rpc_daemon, Socket sock) {
        super(rpc_daemon, sock);
    }

    @Override public boolean process_work(datagram d) {
        assert d.service.value < protocol.relay_end;
        if (d.service.value == protocol.relay_push) {
            if (((us.gov.socket.rpc_daemon_t)daemon).handler.dispatcher == null) {
                log("discarding push datagram. dispatcher not available."); //--strip
                return true;
            }
            log("push datagram handed over to dispatcher"); //--strip
            if (!((us.gov.socket.rpc_daemon_t)daemon).handler.dispatcher.dispatch(d)) {
                log("Not consumed"); //--strip
            }
            return true;
        }
        return super.process_work(d);
    }


    //#include <us/api/generated/java/gov/relay/cllr_rpc-impl>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/gov/relay/cllr_rpc-impl>
    @Override public ko call_push(final push_in_t o_in) {
        log("push "); //--strip
        /// in:
        ///     hash_t tid;
        ///     uint16_t code;
        ///     blob_t blob;

        return send1(o_in.get_datagram(daemon.channel, new seq_t(0)));
    }
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.

    @Override public void dump(final String prefix, PrintStream os) {
        os.println(prefix + "relay::rpc_peer_t");
        super.dump(prefix, os);
    }

}

