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
package us.gov.dfs;
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import us.CFG;                                                                                 // CFG
import us.gov.socket.datagram;                                                                 // datagram
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.gov.io.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import us.gov.id.types.*;                                                                      // *
import us.ko;                                                                                  // ko
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import us.gov.protocol;                                                                        // protocol
import java.security.PublicKey;                                                                // PublicKey
import java.net.Socket;                                                                        // Socket
import us.string;                                                                              // string

public abstract class rpc_peer_t extends us.gov.relay.rpc_peer_t implements caller_api {

    static void log(final String line) {                        //--strip
        CFG.log_gov_dfs("rpc_peer_t " + line);                  //--strip
    }                                                           //--strip

    public rpc_peer_t(rpc_daemon_t rpc_daemon) {
        super(rpc_daemon);
    }

    public rpc_peer_t(rpc_daemon_t rpc_daemon, Socket sock) {
        super(rpc_daemon, sock);
    }

    //#include <us/api/generated/gov/java/dfs/cllr_rpc-impl>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/gov/java/dfs/cllr_rpc-impl>
    @Override public ko call_file_request(final hash_t content_digest, bin_t content) {
        log("file_request "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.dfs_file_request), new seq_t(0), content_digest), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.dfs_file_request_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.dfs_file_request_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, content);
        return r;
    }

    @Override public ko call_file_request_response(final bin_t content) {
        log("file_request_response "); //--strip
        return send1(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.dfs_file_request_response), new seq_t(0), content));
    }
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.

    @Override public void dump(final String prefix, PrintStream os) {
        os.println(prefix + "dfs::rpc_peer_t");
        super.dump(prefix, os);
    }

}

