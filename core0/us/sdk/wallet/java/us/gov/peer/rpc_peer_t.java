//===-                           P L E B B L E
//===-                         https://plebble.us
//===-
//===-              Copyright (C) 2017-2022 root1m3@plebble.us
//===-
//===-                      GNU GENERAL PUBLIC LICENSE
//===-                       Version 3, 29 June 2007
//===-
//===-    This program is free software: you can redistribute it and/or modify
//===-    it under the terms of the AGPLv3 License as published by the Free
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
package us.gov.peer;
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import us.CFG;                                                                                 // CFG
import static us.gov.socket.types.*;                                                           // *
import us.gov.id.types.*;                                                                      // *
import us.ko;                                                                                  // ko
import java.io.PrintStream;                                                                    // PrintStream
import us.gov.protocol;                                                                        // protocol
import java.security.PublicKey;                                                                // PublicKey
import java.net.Socket;                                                                        // Socket

public abstract class rpc_peer_t extends us.gov.auth.peer_t implements caller_api {

    static void log(final String line) {                        //--strip
        CFG.log_gov_peer("rpc_peer_t: " + line);                //--strip
    }                                                           //--strip

    public rpc_peer_t(rpc_daemon_t rpc_daemon) {
        super(rpc_daemon);
    }

    public rpc_peer_t(rpc_daemon_t rpc_daemon, Socket sock) {
        super(rpc_daemon, sock);
    }

    //#include <us/api/generated/java/gov/peer/cllr_rpc-impl>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/gov/peer/cllr_rpc-impl>
    @Override public ko call_f1() {
        log("f1 "); //--strip
        return send1(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.peer_f1), new seq_t(0)));
    }
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.

    @Override public void dump(final String prefix, PrintStream os) {
        os.println(prefix + "peer::rpc_peer_t");
        super.dump(prefix, os);
    }

}

