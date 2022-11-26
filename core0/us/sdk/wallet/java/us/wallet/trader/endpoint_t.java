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
package us.wallet.trader;
import java.util.Arrays;                                                                       // Arrays
import us.gov.crypto.base58;                                                                   // base58
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import java.io.ByteArrayInputStream;                                                           // ByteArrayInputStream
import java.io.ByteArrayOutputStream;                                                          // ByteArrayOutputStream
import us.CFG;                                                                                 // CFG
import java.lang.Comparable;                                                                   // Comparable
import us.gov.socket.datagram;                                                                 // datagram
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.stdint.*;                                                                     // *
import us.gov.socket.types.*;                                                                  // *
import java.io.InputStream;                                                                    // InputStream
import java.io.IOException;                                                                    // IOException
import static us.ko.is_ko;                                                                     // is_ko
import us.ko;                                                                                  // ko
import java.lang.NullPointerException;                                                         // NullPointerException
import java.lang.NumberFormatException;                                                        // NumberFormatException
import static us.ko.ok;                                                                        // ok
import java.io.OutputStream;                                                                   // OutputStream
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import us.gov.crypto.ripemd160;                                                                // ripemd160
import java.util.Scanner;                                                                      // Scanner
import static us.gov.io.types.blob_t.serid_t;                                                  // serid_t

public class endpoint_t implements Comparable<endpoint_t>, us.gov.io.seriable {

    static void log(final String line) {                //--strip
       CFG.log_wallet_trader("endpoint_t: " + line);    //--strip
    }                                                   //--strip

    public endpoint_t() {
        chan = new channel_t(0);
        pkh = new hash_t(0);
        wloc = "";
    }

    public endpoint_t(hash_t pkh_, String subhome) {
        chan = new channel_t(0);
        pkh = pkh_;
        wloc = subhome;
    }

    public endpoint_t(hash_t pkh_) {
        chan = new channel_t(0);
        pkh = pkh_;
        wloc = "";
    }

    public endpoint_t(channel_t chan_, hash_t pkh_) {
        chan = chan_;
        pkh = pkh_;
        wloc = "";
    }

    public endpoint_t(endpoint_t other) {
        chan = new channel_t(other.chan.value);
        pkh = new hash_t(other.pkh);
        wloc = new String(other.wloc);
    }

    public endpoint_t(String s) {
        try {
            InputStream is = new ByteArrayInputStream(s.getBytes());
            from_stream(is);
        }
        catch (Exception e) {
        }
    }

    @Override public serid_t serial_id() { return serid_t.no_header; }

    @Override public int blob_size() {
        return blob_writer_t.blob_size(chan) + blob_writer_t.blob_size(pkh) + blob_writer_t.blob_size(wloc);
    }

    @Override public void to_blob(blob_writer_t writer) {
        log("writing endpoint"); //--strip
        writer.write(chan);
        writer.write(pkh);
        writer.write(wloc);
    }

    @Override public ko from_blob(blob_reader_t reader) {
        log("reading endpoint"); //--strip
        {
            chan = new channel_t();
            ko r = reader.read(chan);
            if (is_ko(r)) {
                return r;
            }
        }
        {
            pkh = new hash_t(0);
            ko r = reader.read(pkh);
            if (is_ko(r)) {
                return r;
            }
        }
        {
            pair<ko, String> r = reader.read_string();
            if (is_ko(r.first)) {
                return r.first;
            }
            wloc = r.second;
        }
        return ok;
    }

    public hostport_t decode_ip4() {
        return decode_ip4(pkh);
    }

    public static hostport_t decode_ip4(hash_t h) {
        if (uint64_t.from(h.value, 0).value != 0L) return new hostport_t(new host_t(0), new port_t(0));
        if (uint32_t.from(h.value, 8).value != 0L) return new hostport_t(new host_t(0), new port_t(0));
        if (uint16_t.from(h.value, 12).value != 0L) return new hostport_t(new host_t(0), new port_t(0));
        host_t ho = new host_t();
        ho.read(h.value, 16);
        port_t p = new port_t();
        p.read(h.value, 14);
        return new hostport_t(ho, p);
    }

    public String decode_ip4_string() {
        return us.gov.socket.client.endpoint(decode_ip4());
    }

    public static String decode_ip4_string(hash_t h) {
        return us.gov.socket.client.endpoint(decode_ip4(h));
    }

    public static hash_t encode_ip4(hostport_t ipport) {
        hash_t h = new hash_t();
        new uint64_t(0).write(h.value, 0);
        new host_t(0).write(h.value, 8);
        new port_t(0).write(h.value, 12);
        ipport.second.write(h.value, 14);
        ipport.first.write(h.value, 16);
        return h;
    }

    public static hash_t encode_ip4(host_t ip, port_t port) {
        hash_t h=new hash_t();
        new uint64_t(0).write(h.value, 0);
        new uint32_t(0).write(h.value, 8);
        new uint16_t(0).write(h.value, 12);
        port.write(h.value, 14);
        ip.write(h.value, 16);
        return h;
    }

    public static hash_t encode_ip4(String ipport) {
        pair<ko, shostport_t> r = us.gov.socket.client.parse_endpoint(ipport);
        if (r.first != ok) {
            return new hash_t(0);
        }
        return encode_ip4(us.gov.socket.client.ip4_encode(r.second.first), r.second.second);
    }

    public boolean is_ip4() {
        return decode_ip4().first.value != 0;
    }

    public static boolean is_ip4(hash_t h) { return decode_ip4(h).first.value != 0; }

    static int find_first_not_num(String s) {
        for (int i = 0; i < s.length(); ++i) {
            char c = s.charAt(i);
            if (c < '0' || c > '9') return i;
        }
        return -1;
    }

    static int count(String s, char t) {
        int n = 0;
        for (int i = 0; i < s.length(); ++i) {
            char c = s.charAt(i);
            if (c == t) ++n;
        }
        return n;
    }

    public ko from_stream(InputStream is) {
        Scanner sc = new Scanner(is);
        return from_scanner(sc);
    }

    public ko from_scanner(Scanner sc) {
        try {
            String epf = sc.next();
            log("first word " + epf); //--strip
            if (!epf.isEmpty() && epf.length() < 6 && find_first_not_num(epf) == -1) { //integer->channel
                log("numeric value => channel " + epf); //--strip
                chan = new channel_t(Integer.parseInt(epf));
                epf = sc.next();
            }
            log("next word " + epf + " so far channel " + chan.value); //--strip
            int n = count(epf, '.');
            log("num dots " + n); //--strip
            if (n > 3) { //ip4+wloc
                log("ip4+wloc"); //--strip
                int pos = 0;
                for (int i = 0; i < 4; ++i) pos = epf.indexOf(".", pos) + 1;
                String spkh = epf.substring(0, pos - 1);
                if (epf.length() > pos - 1) {
                    wloc = epf.substring(pos, epf.length() - pos);
                }
                pkh = encode_ip4(spkh);
                log("pkh (enoded ip4) " + pkh.encode() + " wloc " + wloc); //--strip
            }
            else if (n == 3) { //ip4
                log("ip4"); //--strip
                pkh = encode_ip4(epf);
                log("pkh (enoded ip4) " + pkh.encode() + " wloc NO"); //--strip
            }
            else if (n > 0) { //pkh+wloc
                log("pkh+wloc"); //--strip
                int pos = epf.indexOf(".");
                String spkh = epf.substring(0, pos);
                if (epf.length() > pos) {
                    wloc = epf.substring(pos + 1, epf.length() - pos - 1);
                }
                pkh = hash_t.decode(spkh);
                log("pkh " + pkh.encode() + " wloc " + wloc); //--strip
            }
            else { //pkh
                pkh = hash_t.decode(epf);
            }
            if (!is_ip4()) {
                if (pkh.is_zero()) {
                    log("reset wloc on invalid ip4pkh"); //--strip
                    wloc = "";
                }
            }
            return ok;
        }
        catch (Exception e) {
            return new ko("KO 83729 " + e.getMessage());
        }
    }

    public void to_streamX(PrintStream os) {
        if (chan.value != 0) os.print(chan.value + ' ');
        os.print(pkh.encode());
        if (!wloc.isEmpty()) {
            os.print(".");
            os.print(wloc);
        }
        os.print(" ");
    }

    public void to_stream(OutputStream os0) {
        PrintStream os = new PrintStream(os0);
        try {
            if (chan.value != 0) {
                os.print(chan.value);
                os.print(' ');
            }
            os.write(pkh.encode().getBytes());
            if (!wloc.isEmpty()) {
                os.print('.');
                os.print(wloc);
            }
            os.print(' ');
        }
        catch (Exception e) {
        }
    }

    public boolean equals(endpoint_t other) {
        return pkh.equals(other.pkh) && wloc.equals(other.wloc) && chan.value == other.chan.value;
    }

    public String to_string() {
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        to_stream(os);
        return new String(os.toByteArray());
    }

    public boolean is_valid() {
        if (pkh == null) return false;
        if (pkh.is_zero()) return false;
        return true;
    }

    @Override public int compareTo(endpoint_t other) {
        if (pkh.equals(other.pkh) && wloc.equals(other.wloc)) return 0;
        return 1;
    }

    public channel_t chan = null;
    hash_t pkh = null;
    String wloc = null;
}

