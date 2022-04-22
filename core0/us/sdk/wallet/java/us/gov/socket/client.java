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
import java.util.Arrays;                                                                       // Arrays
import java.util.concurrent.atomic.AtomicBoolean;                                              // AtomicBoolean
import java.util.concurrent.atomic.AtomicInteger;                                              // AtomicInteger
import java.util.concurrent.atomic.AtomicLong;                                                 // AtomicLong
import java.nio.ByteBuffer;                                                                    // ByteBuffer
import java.nio.ByteOrder;                                                                     // ByteOrder
import us.CFG;                                                                                 // CFG
import java.util.Date;                                                                         // Date
import static us.gov.socket.types.*;                                                           // *
import static us.tuple.*;                                                                      // *
import us.stdint.*;                                                                            // *
import java.net.InetAddress;                                                                   // InetAddress
import java.net.InetSocketAddress;                                                             // InetSocketAddress
import java.io.IOException;                                                                    // IOException
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import us.gov.protocol;                                                                        // protocol
import java.net.Socket;                                                                        // Socket
import java.util.concurrent.TimeUnit;                                                          // TimeUnit

public class client {

    public static final ko KO_60541 = new ko("KO 60541 Invalid address.");
    public static final ko KO_58961 = new ko("KO 58961 Invalid port.");
    public static final ko KO_83911 = new ko("KO 83911 IO Exception.");
    public static final ko KO_83912 = new ko("KO 83912 Exception init_sock. connect.");
    public static final ko KO_10100 = new ko("KO 10100 Unreachable host.");
    public static final ko KO_10160 = new ko("KO 10160 Timeout connecting to host.");
    public static final ko KO_10161 = new ko("KO 10161 Failed to connect.");
    public static final ko KO_27190 = new ko("KO 27190 Invalid endpoint port.");
    public static final ko KO_27191 = new ko("KO 27191 Invalid endpoint host.");

    static void log(final String line) {                         //--strip
        CFG.log_gov_socket("client: " + line);                   //--strip
    }                                                            //--strip

    public client(daemon0_t daemon_) {
        log("contructor-outbound"); //--strip
        daemon = daemon_;
        sock = null;
        long t = new Date().getTime();
        activity.set(t);
        since = activity_recv = t;
    }

    public client(daemon0_t daemon_, Socket sock_) {
        daemon = daemon_;
        sock = sock_;
        if (connected()) {
            hostport = raddress();
        }
        if (sock != null) { //--strip
            log("contructor-inbound"); //--strip
        } //--strip
        else { //--strip
            log("contructor-outbound"); //--strip
        } //--strip
        long t = new Date().getTime();
        activity.set(t);
        since = activity_recv = t;
    }

    public void register_reason(final channel_t channel, final seq_t seq, final reason_t reason) {
        if (finished_reason == null) {
            finished_reason = new tuple3<channel_t, seq_t, reason_t>(channel, seq, reason);
        }
        if (finished_reason.item2.value.isEmpty()) {
            finished_reason = new tuple3<channel_t, seq_t, reason_t>(channel, seq, reason);
            log("adding finishing reason " + reason.value + " seq " + seq.value); //--strip
        }
        else {
            log("preserving existing finishing reason" + finished_reason.item2.value + " seq " + finished_reason.item1.value); //--strip
        }
    }

    public void disconnectx(final channel_t channel, final seq_t seq, final reason_t reason) {
        log("disconnect" + endpoint() + " chan " + channel.value); //--strip
        register_reason(channel, seq, reason);
        daemon.detach(this);
        on_I_disconnected(reason);
    }

    public void disconnect(final seq_t seq, final reason_t reason) {
        log("disconnect. ch" + daemon.channel.value); //--strip
        disconnectx(daemon.channel, seq, reason);
    }

    public void on_I_disconnected(final reason_t reason) {}

    public pair<ko, datagram> decrypt0(final datagram d) {
        log("not decrypting. method not overriden"); //--strip
        return new pair<ko, datagram>(ok, d); //no encryption;
    }

    public pair<ko, datagram> encrypt0(final datagram d) {
        log("not encrypting. method not overriden"); //--strip
        return new pair<ko, datagram>(ok, d); //no encryption;
    }

    public boolean is_finished() {
        return finished.get();
    }

    public void set_finish() {
        log("set_finish " + endpoint()); //--strip
        finished.set(true);
        if (sock == null) return;
        try {
            log("sock.close()");  //--strip
            sock.close();
        }
        catch (IOException e) {
            log("Exception closing socket. " + e.getMessage());  //--strip
        }
        log("set sock=null"); //--strip
        sock = null;
    }

    public void set_finished() {
        log("set finished"); //--strip
        finished.set(true);
    }

    public void set_finishing_reason(final channel_t peer_channel, final seq_t seq, final reason_t reason) {
        log("adding finishing reason" + reason + "seq" + seq.value); //--strip
        if (reason == null || reason.value.isEmpty()) {
            log("no given reason"); //--strip
            return;
        }
        finished_reason = new tuple3<channel_t, seq_t, reason_t>(peer_channel, seq, reason);
    }

    public static shost_t ip4_decode(host_t host) {
        try {
            byte[] x = new byte[4];
            byte[] t = new byte[8];
            ByteBuffer bb = ByteBuffer.wrap(t);
            bb.order(ByteOrder.BIG_ENDIAN);
            bb.asLongBuffer().put(host.value);
            x[3] = t[4];
            x[2] = t[5];
            x[1] = t[6];
            x[0] = t[7];
            InetAddress i = InetAddress.getByAddress(x);
            return new shost_t(i.getHostAddress());
        }
        catch (Exception e) {
            return new shost_t("");
        }
    }

    public static host_t ip4_encode(shost_t addr0) {
        try {
            InetAddress a = InetAddress.getByName(addr0.value);
            byte[] x = a.getAddress();
            byte[] t = new byte[8];
            t[0] = t[1] = t[2] = t[3] = 0;
            t[4] = x[3];
            t[5] = x[2];
            t[6] = x[1];
            t[7] = x[0];
            ByteBuffer bb = ByteBuffer.wrap(t);
            bb.order(ByteOrder.BIG_ENDIAN);
            return new host_t(bb.getLong());
        }
        catch (Exception e) {
            log("KO 50943 " + e.getMessage()); //--strip
            return new host_t(0);
        }
    }

    public hostport_t ip4_encode(final shostport_t shostport) {
        return new hostport_t(ip4_encode(shostport.first), shostport.second);
    }

    public static String endpoint(final host_t host, final port_t port) {
        return ip4_decode(host).value + ":" + port.value;
    }

    public static String endpoint(final hostport_t hostport) {
        return ip4_decode(hostport.first).value + ":" + hostport.second.value;
    }

    public static String endpoint(final shostport_t shostport) {
        return shostport.first.value + ":" + shostport.second.value;
    }

    public static String endpoint(final shost_t a, final port_t p) {
        return a.value + ":" + p.value;
    }

    public String endpoint() {
        return endpoint(hostport);
    }

    public static pair<ko, shostport_t> parse_endpoint(final String ipport) {
        String[] f = ipport.split(":", 2);
        if (f.length != 2) {
            return new pair<ko, shostport_t>(KO_27190, null);
        }
        port_t p;
        try {
            p = new port_t(Integer.parseInt(f[1]));
        }
        catch (Exception e) {
            return new pair<ko, shostport_t>(KO_27191, null);
        }
        if (p.value == 0) {
            return new pair<ko, shostport_t>(KO_27191, null);
        }
        return new pair<ko, shostport_t>(ok, new shostport_t(f[0], p));
    }

    public hostport_t raddress() {
        return new hostport_t(ip4_encode(new shost_t(sock.getInetAddress().getHostAddress())), new port_t(sock.getLocalPort()));
    }

    static public String age(long msts) {
        long s = (new Date().getTime() - msts) / 1000;
        return String.format("%d:%02d:%02d", s / 3600, (s % 3600) / 60, (s % 60));
    }

    public String refdata() {
        return endpoint();
    }

    public void dump(final String prefix, PrintStream os) {
        os.println(prefix + "socket::client: inet_addr " + endpoint() + " age " + age(since) + " idle " + age(activity.get()) + " idle(recv) " + age(activity_recv));
    }

    public void disable_recv_timeout() {
    }

    ko init_sock2(final hostport_t hostport, boolean block) {
        try {
            log("SetSOTimeout " + endpoint(hostport) + " " + us.gov.socket.datagram.timeout);  //--strip
            sock = new Socket();
            sock.connect(new InetSocketAddress(ip4_decode(hostport.first).value, hostport.second.value), us.gov.socket.datagram.timeout);
            log("Socket t/o: " + sock.getSoTimeout()); //--strip
        }
        catch(IOException e) {
            log("IO Exception! " + endpoint(hostport) + " " + e.getMessage());  //--strip
            if (e.getMessage().contains("Connection refused")) {
                log(KO_10100.msg); //--strip
                return KO_10100;
            }
            else if (e.getMessage().contains("ETIMEDOUT")) {
                log(KO_10160.msg); //--strip
                return KO_10160;
            }
            else if (e.getMessage().contains("failed to connect to")) {
                log(KO_10161.msg); //--strip
                return KO_10161;
            }
            log(KO_83911.msg); //--strip
            return KO_83911;
        }
        catch(Exception e) {
            log("Exception! " + endpoint(hostport) + " " + e.getMessage());  //--strip
            log(KO_83912.msg); //--strip
            return KO_83912;
        }
        return ok;
    }

    public ko connect0(final hostport_t hostport, boolean block) {
        log("connect0 " + endpoint(hostport) + " block?=" + block);  //--strip
        if (hostport.first.value == 0) {
            log(KO_60541.msg); //--strip
            on_connect(KO_60541);
            return KO_60541;
        }
        if (hostport.second.value == 0) {
            log(KO_58961.msg); //--strip
            on_connect(KO_58961);
            return KO_58961;
        }
        finished.set(false);
        finished_reason = null;
        ko r = init_sock2(hostport, block);
        if (ko.is_ko(r)) {
            log("connect-outbound-FAILED " + endpoint(hostport)); //--strip
            on_connect(r);
            return r;
        }
        this.hostport = hostport;
        log("reset ts activity = now 2"); //--strip
        long t = new Date().getTime();
        since = t;
        activity.set(t);
        on_connect(ok);
        return ok;
    }

    public ko connect0(final shostport_t shostport, boolean block) {
        log("connect0 " + endpoint(shostport) + " block?=" + block);  //--strip
        return connect0(ip4_encode(shostport), block);
    }

    public boolean connected() { return sock != null && sock.isConnected() && !finished.get(); } //!called_disconnect; }

    public void on_connect(ko err) {
        log("on_connect " + (ko.is_ok(err) ? "ok" : err.msg)); //--strip
        if (ko.is_ok(err)) {
            activity_recv = new Date().getTime();
        }
        daemon.on_connect(this, err);
    }

    public pair<ko, datagram> recv4x(datagram d, busyled_t busyled) {
        log("recv4"); //--strip
        pair<ko, datagram> r = new pair<ko, datagram>(ok, null);
        if (d != null) {
            log("continuing with existing datagram");  //--strip
            r.second = d;
        }
        else {
            log("starting new datagram");  //--strip
            r.second = new datagram(daemon.channel, 2);
        }
        long prevdend = r.second.dend;
        log("before: dend= " + prevdend); //--strip
        ko ans = r.second.recvfrom(sock, daemon.channel, busyled);
        if (ko.is_ko(ans)) {
            log("recvfrom answer: " + ans.msg); //--strip
            r.first = ans;
            if (r.first != datagram.KO_0001) {
                r.second = null;
            }
            return r;
        }
        assert r.second != null;
        if (prevdend < r.second.dend) {
            log("reset ts activity=now 1 dend:" + prevdend + "->" + r.second.dend); //--strip
            long t = new Date().getTime();
            activity.set(t);
            activity_recv = t;
        }
        log("after: dend=" + r.second.dend); //--strip
        return r;
    }

    public pair<ko, datagram> recv6(busyled_t busyled) {
        log("recv6. channel " + daemon.channel.value); //--strip
        pair<ko, datagram> ans = new pair<ko, datagram>(ok, null);
        while(true) {
            ans = recv4x(ans.second, busyled);
            if (ko.is_ko(ans.first)) {
                log(ans.first.msg); //--strip
                assert ans.second == null;
                break;
            }
            assert ans.second != null;
            if (ans.second.completed()) {
                ans = decrypt0(ans.second);
                break;
            }
            log("dgram is not yet complete."); //--strip
        }
        return ans;
    }

    public static long prev_uid = 0;
    public long uid = ++prev_uid;
    hostport_t hostport = new hostport_t(new host_t(0), new port_t(0));
    public Socket sock;
    public long since;
    public AtomicLong activity = new AtomicLong(0L);
    public daemon0_t daemon;
    AtomicBoolean finished = new AtomicBoolean(false);
    tuple3<channel_t, seq_t, reason_t> finished_reason = null; //    pair<uint16_t, String> finished_reason = null;
    public AtomicInteger sendref = new AtomicInteger(0);
    long activity_recv;

}

