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
import java.util.Arrays;                                                                       // Arrays
import us.gov.crypto.base58;                                                                   // base58
import us.CFG;                                                                                 // CFG
import us.gov.socket.datagram;                                                                 // datagram
import us.gov.crypto.ec;                                                                       // ec
import java.lang.Exception;                                                                    // Exception
import us.gov.crypto.ripemd160.hash_t;                                                         // hash_t
import static us.gov.crypto.types.*;                                                           // *
import static us.gov.io.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import static us.tuple.*;                                                                      // *
import us.stdint.*;                                                                            // *
import java.io.InputStream;                                                                    // InputStream
import static us.ko.is_ko;                                                                     // is_ko
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import java.security.PrivateKey;                                                               // PrivateKey
import java.security.PublicKey;                                                                // PublicKey
import static us.gov.id.types.request_data_t;                                                  // request_data_t
import us.gov.crypto.ripemd160;                                                                // ripemd160
import us.gov.io.types.blob_t.serid_t;                                                         // serid_t
import us.gov.crypto.sha256;                                                                   // sha256
import us.string;                                                                              // string
import static us.gov.io.types.blob_t.version_t;                                                // version_t

public class blob_reader_t {

    static ko KO_67217 = new ko("KO 67217 Overflow.");
    //static ko KO_60499 = new ko("KO 60499 Invalid blob version.");
    static ko KO_60498 = new ko("KO 60498 Invalid blob object.");

    public static final blob_t.version_t current_version = CFG.BLOB_VERSION;

    static void log(final String line) {                     //--strip
        CFG.log_gov_io("blob_reader_t: " + line);            //--strip
    }                                                        //--strip

    public static class blob_header_t {

        public static int sersize = 2;

        public blob_header_t(version_t version, serid_t serid) {
            this.version = version;
            this.serid = serid;
        }

        public version_t version;
        public serid_t serid;
    }

    public interface readable_if {
        public serid_t serial_id();
        ko from_blob(blob_reader_t reader);
    }

    //------------------------------------------------------------------------------
    public static abstract class readable implements readable_if {

        public serid_t serial_id() { return new serid_t((short)0); }

        public ko read(final blob_t blob) {
            log("readable::read from blob " + blob.size());  //--strip
            blob_reader_t reader = new blob_reader_t(blob);
            serid_t serid = serial_id();
            if (serid.value != 0) {
                ko r = reader.read_header(serid);
                if (ko.is_ko(r)) {
                    return r;
                }
            }
            return reader.read(this);
        }

        public ko read(final datagram d) {
            log("readable::read from datagram"); //--strip
            blob_reader_t reader = new blob_reader_t(d);
            serid_t serid = serial_id();
            if (serid.value != 0) {
                ko r = reader.read_header(serid);
                if (ko.is_ko(r)) {
                    return r;
                }
            }
            return reader.read(this);
        }

        public ko read(final String blob_b58) {
            log("readable::read from encoded blob"); //--strip
            return read(new blob_t(us.gov.crypto.base58.decode(blob_b58)));
        }

        public ko load(final String filename) {
            log("readable::load" + filename); //--strip
            blob_t blob = new blob_t();
            {
                ko r = us.gov.io.cfg0.read_file_(filename, blob);
                if (ko.is_ko(r)) {
                    return r;
                }
            }
            return read(blob);
        }

        public pair<ko, blob_header_t> read1(final blob_t blob) {
            pair<ko, blob_header_t> ret = new pair<ko, blob_header_t>(ok, null);
            blob_reader_t reader = new blob_reader_t(blob);
            serid_t serid = serial_id();
            if (serid.value != 0) {
                ko r = reader.read_header(serid);
                if (ko.is_ko(r)) {
                    ret.first = r;
                    return ret;
                }
            }
            ret.first = reader.read(this);
            ret.second = reader.header;
            return ret;
        }

        public pair<ko, blob_header_t> read1(final String blob_b58) {
            return read1(new blob_t(us.gov.crypto.base58.decode(blob_b58)));
        }

        public pair<ko, pair<blob_header_t, hash_t>> read2(final blob_t blob) {
            pair<ko, pair<blob_header_t, hash_t>> ret = new pair<ko, pair<blob_header_t, hash_t>>(ok, null);
            pair<ko, blob_header_t> r = read1(blob);
            if (ko.is_ko(r.first)) {
                ret.first = r.first;
                return ret;
            }
            ret.second = new pair<blob_header_t, hash_t>(r.second, us.gov.crypto.ripemd160.hash(blob.value));
            return ret;
        }

        public pair<ko, pair<blob_header_t, hash_t>> read2(final String blob_b58) {
            return read2(new blob_t(us.gov.crypto.base58.decode(blob_b58)));
        }

        public pair<ko, blob_header_t> load1(final String filename) {
            pair<ko, blob_header_t> ret = new pair<ko, blob_header_t>(ok, null);
            blob_t blob = new blob_t();
            {
                ko r = us.gov.io.cfg0.read_file_(filename, blob);
                if (ko.is_ko(r)) {
                    ret.first = r;
                    return ret;
                }
            }
            return read1(blob);
        }

        public pair<ko, pair<blob_header_t, hash_t>> load2(final String filename) {
            blob_t blob = new blob_t();
            {
                ko r = us.gov.io.cfg0.read_file_(filename, blob);
                if (ko.is_ko(r)) {
                    pair<ko, pair<blob_header_t, hash_t>> ret = new pair<ko, pair<blob_header_t, hash_t>>(r, null);
                    return ret;
                }
            }
            return read2(blob);
        }

        public pair<ko, blob_t> load3(final String filename) {
            pair<ko, blob_t> ret = new pair<ko, blob_t>(ok, new blob_t());
            {
                ko r = us.gov.io.cfg0.read_file_(filename, ret.second);
                if (ko.is_ko(r)) {
                    ret.first = r;
                    return ret;
                }
            }
            return ret;
        }
    }
    //------------------------------------------------------------------------------

    public blob_reader_t(final blob_t blob_) {
        assert blob_ != null;
        assert blob_.value != null;
        blob = blob_;
        cur = 0;
        end = blob.value.length;
    }

    public blob_reader_t(datagram d) {
        blob = new blob_t(d.bytes);
        cur = datagram.h;
        end = blob.value.length;
        assert d.size() >= datagram.h; //--strip
    }

    public static ko readD(final datagram d, uint8_t o) {
        blob_reader_t reader = new blob_reader_t(d);
        return reader.read(o);
    }

    public static ko readD(final datagram d, string o) {
        blob_reader_t reader = new blob_reader_t(d);
        return reader.read(o);
    }

    public static ko readD(final datagram d, hash_t o) {
        blob_reader_t reader = new blob_reader_t(d);
        return reader.read(o);
    }

    public static ko readD(final datagram d, sha256.hash_t o) {
        blob_reader_t reader = new blob_reader_t(d);
        return reader.read(o);
    }

    public static ko readD(final datagram d, readable o) {
        blob_reader_t reader = new blob_reader_t(d);
        return reader.read(o);
    }

    public static ko readD(final datagram d, bin_t o) {
        blob_reader_t reader = new blob_reader_t(d);
        return reader.read(o);
    }

    public static ko readD(final datagram d, blob_t o) {
        blob_reader_t reader = new blob_reader_t(d);
        return reader.read(o);
    }

    public static ko readD(final datagram d, vector_hash o) {
        blob_reader_t reader = new blob_reader_t(d);
        return reader.read(o);
    }

    public static ko readD(final datagram d, vector_string o) {
        blob_reader_t reader = new blob_reader_t(d);
        return reader.read(o);
    }

    public static ko readD(final datagram d, request_data_t o) {
        blob_reader_t reader = new blob_reader_t(d);
        return reader.read(o);
    }

    public static ko readD(final datagram d, readable_if o) {
        log("readD. readable_if"); //--strip
        blob_reader_t reader = new blob_reader_t(d);
        serid_t serid = o.serial_id();
        if (serid.value != 0) {
            ko r = reader.read_header(serid);
            if (ko.is_ko(r)) {
                return r;
            }
        }
        return reader.read(o);
    }

    public static ko readB(final blob_t b, readable_if o) {
        log("readB. readable_if"); //--strip
        blob_reader_t reader = new blob_reader_t(b);
        serid_t serid = o.serial_id();
        if (serid.value != 0) {
            ko r = reader.read_header(serid);
            if (ko.is_ko(r)) {
                return r;
            }
        }
        return reader.read(o);
    }

    public ko read(readable_if o) {
        log("read(readable_if)"); //--strip
        return o.from_blob(this);
    }

    public ko read_sizet(uint64_t o) {
        uint8_t pfx = new uint8_t();
        {
            ko r = read(pfx);
            if (ko.is_ko(r)) return r;
        }
        if (pfx.value < 0xfd) {
            o.value = pfx.value;
            return ok;
        }
        if (pfx.value == 0xfd) {
            uint16_t v = new uint16_t();
            ko r = read(v);
            if (ko.is_ko(r)) return r;
            o.value = v.value;
            return ok;
        }
        if (pfx.value == 0xfe) {
            uint32_t v = new uint32_t();
            ko r = read(v);
            if (ko.is_ko(r)) return r;
            o.value = v.value;
            return ok;
        }
        return read(o);
    }

    public pair<ko, Boolean> read_bool() {
        if ((cur + uint8_t.size()) > end) {
            return new pair<ko, Boolean>(overflow(), null);
        }
        uint8_t x = new uint8_t();
        log("read from blob offset " + cur); //--strip
        x.read(blob.value, cur);
        cur += uint8_t.size();
        return new pair<ko, Boolean>(ok, x.value != 0 ? Boolean.TRUE : Boolean.FALSE);
    }

    ko overflow() {
        log(KO_67217.msg); //--strip
        new Exception().printStackTrace(System.out); //--strip
        return KO_67217;
    }

    public ko read(uint64_t o) {
        if ((cur + uint64_t.size()) > end) return overflow();
        log("read from blob offset " + cur); //--strip
        cur = o.read(blob.value, cur);
        return ok;
    }

    public ko read(uint8_t o) {
        if ((cur + uint8_t.size()) > end) return overflow();
        log("read from blob offset " + cur); //--strip
        cur = o.read(blob.value, cur);
        return ok;
    }

    public ko read(serid_t o) {
        if ((cur + serid_t.size()) > end) return overflow();
        log("read from blob offset " + cur); //--strip
        cur = o.read(blob.value, cur);
        return ok;
    }

    public ko read(version_t o) {
        if ((cur + version_t.size()) > end) return overflow();
        log("read from blob offset " + cur); //--strip
        cur = o.read(blob.value, cur);
        return ok;
    }

    public ko read(uint16_t o) {
        if ((cur + uint16_t.size()) > end) return overflow();
        log("read from blob offset " + cur); //--strip
        cur = o.read(blob.value, cur);
        return ok;
    }

    public ko read(int16_t o) {
        if ((cur + int16_t.size()) > end) return overflow();
        log("read from blob offset " + cur); //--strip
        cur = o.read(blob.value, cur);
        return ok;
    }

    public ko read(uint32_t o) {
        if ((cur + uint32_t.size()) > end) return overflow();
        log("read from blob offset " + cur); //--strip
        cur = o.read(blob.value, cur);
        return ok;
    }

    public ko read(int32_t o) {
        if ((cur + int32_t.size()) > end) return overflow();
        log("read from blob offset " + cur); //--strip
        cur = o.read(blob.value, cur);
        return ok;
    }

    public ko read(int64_t o) {
        if ((cur + int64_t.size()) > end) return overflow();
        log("read from blob offset " + cur); //--strip
        cur = o.read(blob.value, cur);
        return ok;
    }

    public ko read(hash_t o) {
        if ((cur + ripemd160.output_size) > end) return overflow();
        log("read from blob offset " + cur); //--strip
        cur = o.read(blob.value, cur);
        return ok;
    }

    public ko read(us.gov.crypto.sha256.hash_t o) {
        if ((cur + us.gov.crypto.sha256.output_size) > end) return overflow();
        log("read from blob offset " + cur); //--strip
        cur = o.read(blob.value, cur);
        return ok;
    }

    public ko read(pub_t o) {
        if ((cur + pub_t.ser_size) > end) return overflow();
        log("read from blob offset " + cur); //--strip
        o.value = ec.instance.get_public_key(blob.value, cur);
        cur += pub_t.ser_size;
        if (o.value == null) {
            ko r = new ko("KO 82201 Invalid public key");
            log(r.msg); //--strip
            return r;
        }
        return ok;
    }

    public ko read(priv_t o) {
        if ((cur + priv_t.mem_size) > end) return overflow();
        log("read from blob offset " + cur); //--strip
        o.value = ec.instance.get_private_key(blob.value, cur);
        cur += priv_t.mem_size;
        if (o.value == null) {
            ko r = new ko("KO 82201 Invalid private key");
            log(r.msg); //--strip
            return r;
        }
        return ok;
    }

    public ko read(sig_t o) {
        if ((cur + sig_t.ser_size) > end) return overflow();
        log("read from blob offset " + cur); //--strip
        System.arraycopy(blob.value, cur, o.value, 0, sig_t.ser_size);
        cur += sig_t.ser_size;
        return ok;
    }

    public ko read(sig_der_t o) {
        uint64_t sz = new uint64_t();
        ko e = read_sizet(sz);
        if (is_ko(e)) return e;
        if ((cur + sz.value) > end) return overflow();
        o.resize((int) sz.value);
        if (sz.value == 0) return ok;
        log("read from blob offset " + cur); //--strip
        System.arraycopy(blob.value, cur, o.value, 0, (int)sz.value);
        cur += sz.value;
        return ok;
    }

    public ko read(blob_t o) {
        uint64_t sz = new uint64_t();
        ko e = read_sizet(sz);
        if (is_ko(e)) return e;
        if ((cur + sz.value) > end) return overflow();
        o.resize((int) sz.value);
        log("read from blob offset " + cur); //--strip
        System.arraycopy(blob.value, cur, o.value, 0, (int)sz.value);
        cur += sz.value;
        return ok;
    }

    public ko read(bin_t o) {
        uint64_t sz = new uint64_t();
        ko e = read_sizet(sz);
        if (is_ko(e)) return e;
        if ((cur + sz.value) > end) return overflow();
        o.resize((int) sz.value);
        if (sz.value == 0) return ok;
        log("read from blob offset " + cur); //--strip
        System.arraycopy(blob.value, cur, o.value, 0, (int)sz.value);
        cur += sz.value;
        return ok;
    }

    public ko read(pair<String, String> o) {
        {
            string s = new string();
            ko r = read(s);
            if (ko.is_ko(r)) return r;
            o.first = s.value;
        }
        {
            string s = new string();
            ko r = read(s);
            if (ko.is_ko(r)) return r;
            o.second = s.value;
        }
        return ok;
    }

    public ko read(vector_hash o) {
        o.clear();
        uint64_t sz = new uint64_t();
        {
            ko r = read_sizet(sz);
            if (ko.is_ko(r)) return r;
        }
        o.ensureCapacity((int)sz.value);
        for (int i =0; i< sz.value; ++i) {
            hash_t x = new hash_t();
            ko r = read(x);
            if (ko.is_ko(r)) return r;
            o.add(x);
        }
        return ok;

    }

    public ko read(vector_string o) {
        o.clear();
        uint64_t sz = new uint64_t();
        {
            ko r = read_sizet(sz);
            if (ko.is_ko(r)) return r;
        }
        o.ensureCapacity((int)sz.value);
        for (int i =0; i< sz.value; ++i) {
            string x = new string();
            ko r = read(x);
            if (ko.is_ko(r)) return r;
            o.add(x.value);
        }
        return ok;

    }

    public ko read(vector_pair_string_string o) {
        o.clear();
        uint64_t sz = new uint64_t();
        {
            ko r = read_sizet(sz);
            if (ko.is_ko(r)) return r;
        }
        o.ensureCapacity((int)sz.value);
        for (int i =0; i< sz.value; ++i) {
            pair<String, String> x = new pair<String, String>(null, null);
            ko r = read(x);
            if (ko.is_ko(r)) return r;
            o.add(x);
        }
        return ok;
    }

    public ko read(tuple3<hash_t, host_t, port_t> o) {
        {
            if (o.item0 == null) o.item0 = new hash_t();
            ko r = read(o.item0);
            if (ko.is_ko(r)) return r;
        }
        {
            if (o.item1 == null) o.item1 = new host_t();
            ko r = read(o.item1);
            if (ko.is_ko(r)) return r;
        }
        {
            if (o.item2 == null) o.item2 = new port_t();
            ko r = read(o.item2);
            if (ko.is_ko(r)) return r;
        }
        return ok;
    }

    public ko read(vector_tuple_hash_host_port o) {
        o.clear();
        uint64_t sz = new uint64_t();
        {
            ko r = read_sizet(sz);
            if (ko.is_ko(r)) return r;
        }
        o.ensureCapacity((int)sz.value);
        for (int i =0; i< sz.value; ++i) {
            tuple3<hash_t, host_t, port_t> x = new tuple3<hash_t, host_t, port_t>(null, null, null);
            ko r = read(x);
            if (ko.is_ko(r)) return r;
            o.add(x);
        }
        return ok;
    }

    public pair<ko, String> read_string() {
        uint64_t sz = new uint64_t();
        ko e = read_sizet(sz);
        if (is_ko(e)) return new pair<ko, String>(e, null);
        log("read_string. sz = " + sz.value); //--strip
        if ((cur + sz.value) > end) return new pair<ko, String>(overflow(), null);
        log("read from blob offset " + cur); //--strip
        String o = new String(blob.value, cur, (int) sz.value);
        cur += sz.value;
        return new pair<ko, String>(ok, o);
    }

    public ko read(string o) {
        pair<ko, String> r = read_string();
        if (is_ko(r.first)) return r.first;
        o.value = r.second;
        return ok;
    }

    public pair<ko, blob_t> read_blob() {
        pair<ko, blob_t> ret = new pair<ko, blob_t>(ok, null);
        uint64_t sz = new uint64_t();
        ko e = read_sizet(sz);
        if (is_ko(e)) {
            ret.first = e;
            return ret;
        }
        if ((cur + sz.value) > end) {
            ret.first = overflow();
            return ret;
        }
        ret.second = new blob_t((int)sz.value);
        log("read from blob offset " + cur); //--strip
        Arrays.copyOfRange(blob.value, cur, cur + (int)sz.value);
        cur += sz.value;
        return ret;
    }

    public static ko parse(final blob_t blob, string o) {
        blob_reader_t reader = new blob_reader_t(blob);
        return reader.read(o);
    }

    public static ko parse(final blob_t blob, readable_if o) {
        blob_reader_t reader = new blob_reader_t(blob);
        return reader.read(o);
    }


    public ko read_header(final serid_t serid) {
        log("read_header " + serid.value); //--strip
        {
            ko r = read(header.version);
            if (is_ko(r)) return r;
        }
        {
            ko r = read(header.serid);
            if (is_ko(r)) return r;
        }
        if (header.serid.value != serid.value) {
            log(KO_60498.msg);  //--strip
            return KO_60498;
        }
        return ok;
    }

    public ko read_header() {
        log("read_header"); //--strip
        {
            ko r = read(header.version);
            if (is_ko(r)) return r;
        }
        {
            ko r = read(header.serid);
            if (is_ko(r)) return r;
        }
        /*
        if (header.version.value != current_version.value) {
            log(KO_60499.msg); //--strip
            return KO_60499;
        }
        */
        return ok;
    }

    blob_t blob;
    int cur;
    int end;
    public blob_header_t header = new blob_header_t(new version_t((short)0), new serid_t((short)0));
}

