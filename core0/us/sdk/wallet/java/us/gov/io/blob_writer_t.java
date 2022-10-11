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
package us.gov.io;
import java.util.ArrayList;                                                                    // ArrayList
import us.gov.crypto.base58;                                                                   // base58
import us.CFG;                                                                                 // CFG
import us.gov.socket.datagram;                                                                 // datagram
import us.gov.crypto.ec;                                                                       // ec
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.gov.crypto.types.*;                                                           // *
import static us.gov.io.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import static us.tuple.*;                                                                      // *
import us.stdint.*;                                                                            // *
import java.io.InputStream;                                                                    // InputStream
import us.ko;                                                                                  // ko
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import java.security.PrivateKey;                                                               // PrivateKey
import java.security.PublicKey;                                                                // PublicKey
import us.gov.crypto.ripemd160;                                                                // ripemd160
import us.gov.io.types.blob_t.serid_t;                                                         // serid_t
import us.gov.crypto.sha256;                                                                   // sha256
import us.string;                                                                              // string
import us.gov.io.types.blob_t.version_t;                                                       // version_t

public class blob_writer_t {

    static void log(final String line) {                     //--strip
        CFG.log_gov_io("blob_writer_t: " + line);            //--strip
    }                                                        //--strip

    public static interface writable_if {
        public serid_t serial_id();
        int blob_size();
        void to_blob(blob_writer_t writer);
    }

    public static abstract class writable implements writable_if {

        public serid_t serial_id() {
            return new serid_t((short)0);
        }

        public int tlv_size() {
            return (serial_id().value != 0 ? blob_writer_t.header_size() : 0) + blob_size();
        }

        void write(blob_t blob) {
            log("writable::write to blob"); //--strip
            serid_t serid = serial_id();
            int sz = (serid.value != 0 ? blob_writer_t.header_size() : 0) + blob_size();
            if (sz == 0) {
                blob.clear();
                return;
            }
            blob_writer_t w = new blob_writer_t(blob, sz);
            if (serid.value != 0) {
                w.write_header(serid);
            }
            to_blob(w);
            assert w.cur == blob.value.length;
        }

        public datagram get_datagram(final channel_t channel, final svc_t svc, final seq_t seq) {
            log("writable::get_datagram svc" + svc.value); //--strip
            serid_t serid = serial_id();
            int sz = (serid.value != 0 ? blob_writer_t.header_size() : 0) + blob_size();
            datagram d = new datagram(channel, svc, seq, sz);
            blob_writer_t w = new blob_writer_t(d);
            if (sz == 0) {
                return d;
            }
            if (serid.value != 0) {
                w.write_header(serid);
            }
            w.write(this);
            assert w.cur == d.bytes.length;
            return d;
        }

        public String encode() {
            string s = new string();
            blob_t blob = new blob_t();
            write(blob);
            s.value = us.gov.crypto.base58.encode(blob.value);
            return s.value;
        }

        public ko save(final String filename) {
            log("writable::save " + filename); //--strip
            blob_t blob = new blob_t();
            write(blob);
            return us.gov.io.cfg0.write_file_(blob.value, filename);
        }

    }

    public static void write(final writable_if o, blob_t blob) {
        log("writable::write to blob"); //--strip
        serid_t serid = o.serial_id();
        int sz = (serid.value != 0 ? blob_writer_t.header_size() : 0) + o.blob_size();
        if (sz == 0) {
            blob.clear();
            return;
        }
        blob_writer_t w = new blob_writer_t(blob, sz);
        if (serid.value != 0) {
            w.write_header(serid);
        }
        o.to_blob(w);
        assert w.cur == blob.value.length;
    }


    public blob_writer_t(blob_t blob, int sz) {
        this.blob = blob;
        blob.resize(sz);
        cur = 0;
    }

    public blob_writer_t(datagram d) {
        blob = new blob_t(d.bytes);
        cur = datagram.h;
    }

    public static int sizet_size(final long sz) {
        if (sz < 0xfd) { //8bit
            return uint8_t.size();
        }
        if (sz <= 0xffff) { //16bit
            return uint8_t.size() + uint16_t.size();
        }
        if (sz <= 0xffffffff) { //32bit
            return uint8_t.size() + uint32_t.size();
        }
        //64bit
        return uint8_t.size() + uint64_t.size();
    }

    public void write_sizet(final long sz) {
        if (sz < 0xfd) { //8bit
            blob.value[cur] = (byte)sz;
            ++cur;
            return;
        }
        if (sz <= 0xffff) { //16bit
            blob.value[cur++] = (byte)0xfd;
            cur = new uint16_t((int)sz).write(blob.value, cur);
            return;
        }
        if (sz <= 0xffffffff) { //32bit
            blob.value[cur++] = (byte)0xfe;
            cur = new uint32_t(sz).write(blob.value, cur);
            return;
        }
        //64bit
        blob.value[cur++] = (byte)0xff;
        ++cur;
        cur = new uint64_t(sz).write(blob.value, cur);
    }

    public static int blob_size(final writable_if o) {
        return o.blob_size();
    }

    public void write(final writable_if o) {
        o.to_blob(this);
    }

    public static int blob_size(final boolean o) {
        return uint8_t.size();
    }

    public void write(final boolean o) {
        blob.value[cur++] = (byte) (o ? 1 : 0);
        cur += uint8_t.size();
    }


    public static int blob_size(final int64_t o) {
        return int64_t.size();
    }

    public void write(final int64_t o) {
        cur = o.write(blob.value, cur);
    }


    public static int blob_size(final int32_t o) {
        return int32_t.size();
    }

    public void write(final int32_t o) {
        cur = o.write(blob.value, cur);
    }

    public static int blob_size(final uint8_t o) {
        return uint8_t.size();
    }

    public void write(final uint8_t o) {
        cur = o.write(blob.value, cur);
    }


    public static int blob_size(final uint16_t o) {
        return uint16_t.size();
    }

    public void write(final uint16_t o) {
        cur = o.write(blob.value, cur);
    }


    public static int blob_size(final int16_t o) {
        return int16_t.size();
    }

    public void write(final int16_t o) {
        cur = o.write(blob.value, cur);
    }


    public static int blob_size(final uint32_t o) {
        return uint32_t.size();
    }

    public void write(final uint32_t o) {
        cur = o.write(blob.value, cur);
    }


    public static int blob_size(final uint64_t o) {
        return uint64_t.size();
    }

    public void write(final uint64_t o) {
        cur = o.write(blob.value, cur);
    }

    public static int blob_size(final String o) {
        return sizet_size(o.length()) + o.length();
    }

    public void write(final String o) {
        write_sizet(o.length());
        byte[] src = o.getBytes();
        System.arraycopy(src, 0, blob.value, cur, o.length());
        cur += o.length();
    }

    public static int blob_size(final string o) {
        return sizet_size(o.value.length()) + o.value.length();
    }

    //public void write(final string o) {
    //    write(o.value);
    //}

    public static int blob_size(final byte[] o) {
        return sizet_size(o.length) + o.length;
    }

    public static int blob_size(final hash_t o) {
        return ripemd160.output_size;
    }

    public void write(final hash_t o) {
        cur = o.write(blob.value, cur);
    }


    public static int blob_size(final sha256.hash_t o) {
        return sha256.output_size;
    }

    public void write(final sha256.hash_t o) {
        cur = o.write(blob.value, cur);
    }

    public static int blob_size(final pub_t o) {
        return pub_t.ser_size;
    }

    public void write(final pub_t o) {
        byte[] v = ec.instance.to_vector(o.value);
        assert v.length == pub_t.ser_size;
        System.arraycopy(v, 0, blob.value, cur, pub_t.ser_size);
        cur += pub_t.ser_size;
    }

    public static int blob_size(final priv_t o) {
        return priv_t.ser_size;
    }

    public void write(final priv_t o) {
        byte[] v = ec.instance.to_vector(o.value);
        assert v.length == priv_t.ser_size;
        System.arraycopy(v, 0, blob.value, cur, priv_t.ser_size);
        cur += priv_t.ser_size;
    }

    public static int blob_size(final sig_t o) {
        return sig_t.ser_size;
    }

    public void write(final sig_t o) {
        assert o.value.length == sig_t.ser_size;
        System.arraycopy(o.value, 0, blob.value, cur, sig_t.ser_size);
        cur += sig_t.ser_size;
    }

    public static int blob_size(final sig_der_t o) {
        return sizet_size(o.value.length) + (o.value == null ? 0 : o.value.length);
    }

    public void write(final sig_der_t o) {
        if (o.value ==null) {
            write_sizet(0);
            return;
        }
        write_sizet(o.value.length);
        System.arraycopy(o.value, 0, blob.value, cur, o.value.length);
        cur += o.value.length;
    }

    public static int blob_size(final pair<String, String> o) {
        return sizet_size(o.first.length()) + sizet_size(o.second.length());
    }

    public void write(final pair<String, String> o) {
        write(o.first);
        write(o.second);
    }


    public static int blob_size(final vector_hash o) {
        int sz = sizet_size(o.size());
        for (hash_t i: o) {
            sz += blob_size(i);
        }
        return sz;
    }

    public void write(final vector_hash o) {
        write_sizet(o.size());
        for (hash_t i: o) {
            write(i);
        }
    }


    public static int blob_size(final vector_pair_string_string o) {
        int sz = sizet_size(o.size());
        for (pair<String, String> i: o) {
            sz += blob_size(i);
        }
        return sz;
    }

    public void write(final vector_pair_string_string o) {
        write_sizet(o.size());
        for (pair<String, String> i: o) {
            write(i);
        }
    }


    public static int blob_size(final tuple3<hash_t, host_t, port_t> o) {
        return blob_size(o.item0) + blob_size(o.item1) + blob_size(o.item2);
    }

    public void write(final tuple3<hash_t, host_t, port_t> o) {
        write(o.item0);
        write(o.item1);
        write(o.item2);
    }


    public static int blob_size(final vector_tuple_hash_host_port o) {
        int sz = sizet_size(o.size());
        for (tuple3<hash_t, host_t, port_t> i: o) {
            sz += blob_size(i);
        }
        return sz;
    }

    public void write(final vector_tuple_hash_host_port o) {
        write_sizet(o.size());
        for (tuple3<hash_t, host_t, port_t> i: o) {
            write(i);
        }
    }


    public static int header_size() {
        return version_t.size() + serid_t.size();
    }

    public static int blob_size(final blob_t o) {
        return sizet_size(o.value.length) + o.value.length;
    }

    public void write(final blob_t o) {
        write_sizet(o.value.length);
        System.arraycopy(o.value, 0, blob.value, cur, o.value.length);
        cur += o.value.length;
    }

    public static int blob_size(final bin_t o) {
        return sizet_size(o.value.length) + (o.value == null ? 0 : o.value.length);
    }

    public void write(final bin_t o) {
        if (o.value == null) {
            write_sizet(0);
            return;
        }
        write_sizet(o.value.length);
        System.arraycopy(o.value, 0, blob.value, cur, o.value.length);
        cur += o.value.length;
    }

    public void write_header(final serid_t serid) {
        log("write_header " + serid.value + " = " + (char)serid.value); //--strip
        assert serid.value != 0;
        log("writing blob header " + blob_reader_t.current_version.value + " " + serid.value); //--strip
        write(blob_reader_t.current_version);
        write(serid);
    }

    public static datagram get_datagram(final channel_t channel, final svc_t svc, final seq_t seq) {
        return new datagram(channel, svc, seq, 0);
    }


    public static <T> datagram get_datagram(final channel_t channel, final svc_t svc, final seq_t seq, final writable_if o) {
        datagram d = new datagram(channel, svc, seq, blob_size(o));
        blob_writer_t w = new blob_writer_t(d);
        w.write(o);
        return d;
    }

    public static <T> datagram get_datagram(final channel_t channel, final svc_t svc, final seq_t seq, final string o) {
        datagram d = new datagram(channel, svc, seq, blob_size(o));
        blob_writer_t w = new blob_writer_t(d);
        w.write(o);
        return d;
    }

    public static <T> datagram get_datagram(final channel_t channel, final svc_t svc, final seq_t seq, final sha256.hash_t o) {
        datagram d = new datagram(channel, svc, seq, blob_size(o));
        blob_writer_t w = new blob_writer_t(d);
        w.write(o);
        return d;
    }

    public static <T> datagram get_datagram(final channel_t channel, final svc_t svc, final seq_t seq, final hash_t o) {
        datagram d = new datagram(channel, svc, seq, blob_size(o));
        blob_writer_t w = new blob_writer_t(d);
        w.write(o);
        return d;
    }

    public static datagram get_datagram(final channel_t channel, final svc_t svc, final seq_t seq, final blob_t blob) {
        log("get_datagram from blob" + svc.value + " " + seq.value + " " + blob.value.length); //--strip
        datagram d = new datagram(channel, svc, seq, blob.value.length);
        System.arraycopy(blob.value, 0, d.bytes, datagram.h, blob.value.length);
        return d;
    }

    public static datagram get_datagram(final channel_t channel, final svc_t svc, final seq_t seq, final bin_t bin) {
        log("get_datagram from bin" + svc.value + " " + seq.value + " " + bin.value.length); //--strip
        datagram d = new datagram(channel, svc, seq, bin.value.length);
        System.arraycopy(bin.value, 0, d.bytes, datagram.h, bin.value.length);
        return d;
    }

    public static datagram get_datagram(final channel_t channel, final svc_t svc, final seq_t seq, final uint16_t o) {
        datagram d = new datagram(channel, svc, seq, blob_size(o));
        blob_writer_t w = new blob_writer_t(d);
        w.write(o);
        return d;
    }

    public static datagram get_datagram(final channel_t channel, final svc_t svc, final seq_t seq, final uint32_t o) {
        datagram d = new datagram(channel, svc, seq, blob_size(o));
        blob_writer_t w = new blob_writer_t(d);
        w.write(o);
        return d;
    }

    public static datagram get_datagram(final channel_t channel, final svc_t svc, final seq_t seq, final uint64_t o) {
        datagram d = new datagram(channel, svc, seq, blob_size(o));
        blob_writer_t w = new blob_writer_t(d);
        w.write(o);
        return d;
    }

    public static datagram get_datagram(final channel_t channel, final svc_t svc, final seq_t seq, final pub_t o) {
        datagram d = new datagram(channel, svc, seq, blob_size(o));
        blob_writer_t w = new blob_writer_t(d);
        w.write(o);
        return d;
    }

    public static datagram get_datagram(final channel_t channel, final svc_t svc, final seq_t seq, final priv_t o) {
        datagram d = new datagram(channel, svc, seq, blob_size(o));
        blob_writer_t w = new blob_writer_t(d);
        w.write(o);
        return d;
    }

    public static blob_t make_blob(final String payload) {
        log("blob from string"); //--strip
        blob_t blob = new blob_t();
        blob_writer_t w = new blob_writer_t(blob, blob_size(payload));
        w.write(payload);
        return blob;
    }

    public blob_t blob;
    public int cur;
}

