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
package us.gov.socket.outbound;
import us.CFG;                                                                                 // CFG
import us.gov.socket.client;                                                                   // client
import us.gov.socket.datagram;                                                                 // datagram
import java.util.Date;                                                                         // Date
import us.ko;                                                                                  // ko
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import static us.gov.socket.types.*;

public class caller_daemon_t extends send_queue_t {

    static void log(final String line) {                                           //--strip
        CFG.log_gov_socket("outbound.caller_daemon_t: " + line);                   //--strip
    }                                                                              //--strip

    public caller_daemon_t() {
        rendezvous = new rendezvous_t(this);
    }

    public ko start() {
        log("starting caller_daemon"); //--strip
        return super.start();
    }

    public void stop() {
        log("stop"); //--strip
        super.stop();
        log("flushing rendezvous"); //--strip
        rendezvous.flush();
    }

    public ko send1(client peer, datagram d0) {
        log("send1"); //--strip
        if (d0 == null) {
            ko r = new ko("KO 30291 Not sending null datagram.");
            log(r.msg); //--strip
            return r;
        }
        if (peer.sendref.get() < 0) {
            ko r = new ko("KO 90162 Client waiting to die.");
            log(r.msg); //--strip
            return r;
        }
        int pri = d0.decode_service().value;
        assert api_v.value != 0; //top peer instance must set api_v //--strip
        if (peer.peer_api_v.value != api_v.value) { 
            if (peer.peer_api_v.value < api_v.value || peer.peer_api_v.value == 255) {
                svc_t svc = new svc_t(pri);
                peer.translate_svc(svc, false);
                log("Peer apiv is" + peer.peer_api_v.value + ".Translating svc" + pri + " -> " + svc.value); //--strip
                d0.encode_service(svc);
            }
        }
        log("encrypting"); //--strip
        pair<ko, datagram> d = peer.encrypt0(d0);
        if (ko.is_ko(d.first)) {
            assert d.second == null;
            return d.first;
        }
        log("deliver"); //--strip
        peer.activity.set(new Date().getTime());

        super.send(d.second, peer, pri);
        return ko.ok;
    }

    public void dump(final String prefix, PrintStream os) {
        os.println(prefix + "Hello from socket::caller_daemon");
        os.println("sendq daemon active:" + is_active());
        super.dump(prefix, os);
    }

    public rendezvous_t rendezvous;
    public boolean encrypt_traffic = true;
    public api_v_t api_v = new api_v_t(0);

}

