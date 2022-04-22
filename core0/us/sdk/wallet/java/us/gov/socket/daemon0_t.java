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
package us.gov.socket;
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import us.gov.socket.outbound.caller_daemon_t;                                                 // caller_daemon_t
import us.CFG;                                                                                 // CFG
import static us.gov.socket.types.*;                                                           // *
import us.ko;                                                                                  // ko
import static us.ko.is_ko;                                                                                  // ko
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import us.gov.protocol;                                                                        // protocol
import us.string;                                                                              // string

public abstract class daemon0_t extends caller_daemon_t {

    static void log(final String line) {                            //--strip
        CFG.log_gov_socket("daemon0_t: " + line);                   //--strip
    }                                                               //--strip

    public daemon0_t(channel_t channel) {
        log("daemon0 constructor on channel" + channel.value); //--strip
        this.channel = channel;
    }

    public void process_ok_work(peer_t peer, datagram d) {
        log("process_ok_work " + peer.endpoint() + " " + d.service); //--strip
        assert d != null;
        send1(peer, d);
    }

    public boolean process_unhandled(client peer, datagram d) {
        log("process_unhandled " + d.service.value); //--strip
        return rendezvous.arrived(d);
    }

    public String rewrite(ko r) {
        if (is_ko(r)) {
            assert r.msg != null;
            return r.msg;
        }
        return "";
    }

    public void process_ko_work(peer_t peer, final seq_t seq, ko r) {
        process_ko_work(peer, channel, seq, r);
    }

    public void process_ko_work(peer_t peer, final channel_t channel, final seq_t seq, ko r) {
        assert ko.is_ko(r);
        log("process_ko_work" + r.msg); //--strip
        String e = rewrite(r);
        peer.send1(blob_writer_t.get_datagram(channel, new svc_t(protocol.socket_error), seq, new string(e)));
    }

    public boolean outbound_traffic__goes_encrypted() {
        return encrypt_traffic;
    }

    @Override
    public void dump(final String prefix, PrintStream os) {
        os.println(prefix + "socket::daemon_t:");
        super.dump(prefix, os);
    }

    public void on_destroy_(client peer) {
    }

    public abstract busyled_t recv_led();
    public abstract void attach(client peer);
    public abstract void detach(client peer);

    public void on_connect(client peer, ko err) {}

    public pair<ko, datagram> sendrecv(client peer, datagram d, string remote_error) {
        return rendezvous.sendrecv(peer, d, remote_error);
    }

    public channel_t channel;

}

