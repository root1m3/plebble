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
import us.gov.crypto.test.symmetric_encryption;
import us.gov.crypto.test.ec;
import us.gov.crypto.base58;
import java.security.KeyPair;
import java.security.*;
import us.gov.cash.*;
import us.gov.socket.datagram;
import us.gov.crypto.sha256;
import us.gov.crypto.ripemd160.hash_t;
import us.wallet.cli.hmi;
import us.gov.io.shell_args;
import java.io.File;
import java.nio.file.Files;
import java.io.RandomAccessFile;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Paths;
import us.CFG;
import us.stdint.*;
import us.ko;
import static us.ko.is_ko;
import static us.ko.is_ok;
import us.pair;
import us.tuple.*;
import static us.ko.ok;
import java.security.PrivateKey;                                                               // PrivateKey
import java.security.PublicKey;
import java.security.KeyPair;
import java.util.Random;
import java.util.Arrays;
import java.util.ArrayList;
import java.security.PrivateKey;
import us.gov.crypto.base58;
import us.gov.io.blob_writer_t;
import us.gov.io.blob_reader_t;
import static us.gov.io.types.*;
import static us.gov.crypto.types.*;
import us.wallet.engine.wallet_connection_t;
import us.wallet.engine.ip4_endpoint_t;
import us.string;
import static us.gov.socket.types.*;
import us.wallet.engine.wallet_connection_t;
import us.wallet.engine.bookmark_index_t;
import static us.gov.io.types.blob_t.serid_t;                                                         // serid_t
import us.wallet.engine.ip4_endpoint_t;
import us.gov.io.blob_writer_t;
import us.gov.io.blob_reader_t;
import us.gov.io.blob_reader_t.blob_header_t;
import static us.gov.io.types.blob_t.version_t;

public class main_t extends us.wallet.cli.hmi {

    private static void log(final String line) {      //--strip
        System.out.println("main_t: " + line);        //--strip
    }                                                 //--strip

    static class wrapper_wallet_connection_t extends wallet_connection_t {

        public wrapper_wallet_connection_t() {
            super("nm1", "subhome_1", new ip4_endpoint_t(new shost_t("192.167.66.3"), new port_t(18782), new channel_t(0)));
            ts.value = 52;
            addr.value = "addr_1";
            ssid.value = "ssid_1";
        }

        @Override public serid_t serial_id() { return serid_t.no_header; }

    }

    static class container_wc_t extends ArrayList<wrapper_wallet_connection_t> implements us.gov.io.seriable {
        private static final long serialVersionUID = 1857204L;

        public static serid_t my_serid_id = new serid_t((short)'X');

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

        public ko read(final String blob_b58) {
            log("readable::read from encoded blob"); //--strip
            return read(new blob_t(us.gov.crypto.base58.decode(blob_b58)));
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

        public String encode() {
            string s = new string();
            blob_t blob = new blob_t();
            write(blob);
            s.value = us.gov.crypto.base58.encode(blob.value);
            return s.value;
        }

        @Override public serid_t serial_id() { return my_serid_id; }

        @Override public int blob_size() {
            int sz = blob_writer_t.sizet_size(size());
            for (wrapper_wallet_connection_t entry: this) {
                sz += blob_writer_t.blob_size(entry);
            }
            return sz;
        }

        @Override public void to_blob(blob_writer_t writer) {
            log("to_blob " + size() + " entries"); //--strip
            writer.write_sizet(size());
            for (wrapper_wallet_connection_t entry: this) {
                writer.write(entry);
            }
        }

        @Override public ko from_blob(blob_reader_t reader) {
            clear();
            log("from_blob"); //--strip
            uint64_t sz = new uint64_t();
            {
                ko r = reader.read_sizet(sz);
                if (is_ko(r)) return r;
            }
            log("loading " + sz.value + " items from blob"); //--strip
            try {
                for (long i = 0; i < sz.value; ++i) {
                    log("loading item " + i); //--strip
                    wrapper_wallet_connection_t wrapper_wallet_connection = new wrapper_wallet_connection_t();
                    {
                        ko r = reader.read(wrapper_wallet_connection);
                        if (is_ko(r)) {
                            log(r.msg); //--strip
                            return r;
                        }
                    }
                    add(wrapper_wallet_connection);
                }
            }
            catch (Exception e) {
                return new ko("KO 89782 Invalid read. wrapper_wallet_connection.");
            }
            log("num items " + size()); //--strip
            return ok;
        }

    }

    public main_t() {
        super(new shell_args("-home " + System.getenv("HOME") + "/.us -whost localhost -wp 16673"), System.out);
    }

    public void test_encryption() {
        log("symmetric_encryption"); //--strip
        us.gov.crypto.test.symmetric_encryption se = new us.gov.crypto.test.symmetric_encryption();
        assert se.main();
    }

    public void test_startstopdaemon() {
        log("hmi"); //--strip
        assert ko.is_ok(start());
        ko r = start(); //again
        System.out.println(r.msg);
        //assert r == KO_39201;

        stop();
        join();
    }

    public void test_blob() {
        log("======test blob"); //--strip
        KeyPair a = us.gov.crypto.ec.instance.generate_keypair();
        priv_t sk = new priv_t(a.getPrivate());
        blob_t blob = new blob_t();
        blob_writer_t writer = new blob_writer_t(blob, blob_writer_t.blob_size(sk));
        writer.write(sk);
        String sks = base58.encode(blob.value);
        log("sk:" + sks); //--strip

        blob_reader_t reader = new blob_reader_t(blob);
        priv_t sk2 = new priv_t();
        ko r = reader.read(sk2);
        assert r == ok;

        blob_t blob2 = new blob_t();
        blob_writer_t writer2 = new blob_writer_t(blob2, blob_writer_t.blob_size(sk2));
        writer2.write(sk2);
        String sks2 = base58.encode(blob2.value);
        log("sk2:" + sks2); //--strip

        assert sks2.equals(sks);
    }

    void print_wallet_connection_blob() {
        blob_t blob = new blob_t(base58.decode("3uTXD2Xdohf9vUYcFQJCEdDgSYKDUE7CrWEsDkvtCeUyjiiLmQ7mLaPvGLfNo3YkYt9bHvvXEJTwUB"));
        if (blob.value == null) {
            log("default connection blob is null"); //--strip
            assert false;
            return;
        }
        wallet_connection_t wallet_connection = new wallet_connection_t();
        blob_reader_t reader = new blob_reader_t(blob);
        ko r = reader.read(wallet_connection);
        if (is_ko(r)) {
            log(r.msg); //--strip
            assert false;
            return;
        }
        wallet_connection.dump(System.out);
    }

    void test_wallet_connection_blob(wallet_connection_t wc) {
        log("======test wallet_connection blob");  //--strip
        blob_t blob = new blob_t();
        blob_writer_t writer = new blob_writer_t(blob, blob_writer_t.blob_size(wc));
        writer.write(wc);
        String wcs1 = base58.encode(blob.value);
        log("wcs1:" + wcs1); //--strip

        blob_reader_t reader = new blob_reader_t(blob);
        wallet_connection_t wc2 = new wallet_connection_t();
        ko r = reader.read(wc2);
        assert r == ok;

        blob_t blob2 = new blob_t();
        blob_writer_t writer2 = new blob_writer_t(blob2, blob_writer_t.blob_size(wc2));
        writer2.write(wc2);
        String wcs2 = base58.encode(blob2.value);
        log("wcs2:" + wcs2);  //--strip

        assert wcs2.equals(wcs1);
    }
    
    void test_wallet_connection() {
        String k_b58 = null;
        string nm = new string("");
        string addr = new string("");
        string ssid = new string("");
        String x = null;
        int power = 0;
        int p = 0;
        int ch = 0;
        uint64_t ts = new uint64_t(0);
        k_b58 = "8Guw8kvdJioZwKdUHoTSmmDZeuVzBohMEQZ2PfqDj9UJ";
        addr.value = "SVsSrkBWLmpVVasifpa8r4oCMRa";
        x = "123.156.189.222";
        p = 32;
        nm.value = "a name";
        power = 1;
        ch = 2;
        ts.value = 12345;
        ssid.value = "kraftwerk";

        wallet_connection_t wc = new wallet_connection_t(ts, addr, new string("subhome"), nm, ssid, new ip4_endpoint_t(new shost_t(x), new port_t(p), new channel_t(ch)));
        test_wallet_connection_blob(wc);
    }

    void test_wallet_connectionX() {
        container_wc_t container_wc = new container_wc_t();
        for (int i = 0; i < 5; ++i) {
            container_wc.add(new wrapper_wallet_connection_t());
        }
        blob_t blob = new blob_t();
        String bl = container_wc.encode();        
        log("container_of_5_wrapper_wallet_connection_t a50 blob = " + bl); //--strip
    }

/*
    void wrapper_test_wallet_connection2() {
        String container_of_5_wrapper_wallet_connection_t_a49_blob = "24HGz9KDSkRWJvUzGCe2fJkngCb8v7XfmMMjnLtWXds1YaL5RtMcjTxoCZCGQKwsvCt7YJKRBDTq3k9aYoXzRvqMx4aModzMpjTqZduRBVbFbh9vyNsTC9jVLtER2Rod1WExpYC4My4ikxGMvrz7ZTtwDgoNAh9eTnPR1P8uqGLvErrNEXgDSbzHMEhgDv19Ckd9myzgXuDmRXMBxbr4p8obcwKGMDPR5ALCcdBvgWehGPqvhFZmXYwtrtedJ7zaJ7HB3AXLMtS2Fi9ZbeBc5orcBFycV7275nNcV1h";
        String container_of_5_wrapper_wallet_connection_t_a50_blob = "3JxT8NUbu3u9xYXNVCwQi1kqTuCJyiAQBc8h3kTvBTKVz9w898JiC1HAyB4eHaZiYQnjzAGCpWmATosNReVAeKATNz4MDdGEKUFcVANCpnu1V8Y2dHKFApuzxnKqJweYJ758hrRijLxKNtPkaKuW2r9b4HDHAtHHerUTaKsHDCBpAozpZ43taHWvtC8kBcGgVwonmT8FhoXCaiEsWWQJWkvStCtarrEbYAL9Q6jyVUDnLw3hDRa2JoFVdpZvwfa7mn1sKBrDUvnEFTRWKa2j1rVrhgiAHToYNQsW9bLCdVoQywCfYRNchKu25eKV1puU8kexK1uuf5bcp5qH6VgVFQQEQikydSeamY4621DnwUZiCF";
        log("container_of_5_wrapper_wallet_connection_t_a49_blob = " + container_of_5_wrapper_wallet_connection_t_a49_blob); //--strip
        log("container_of_5_wrapper_wallet_connection_t_a50_blob = " + container_of_5_wrapper_wallet_connection_t_a50_blob); //--strip
        container_wc_t cw = new container_wc_t();
        assert is_ko(cw.read(container_of_5_wrapper_wallet_connection_t_a49_blob));
        ko r = cw.read(container_of_5_wrapper_wallet_connection_t_a50_blob);
        if (is_ko(r)) {
            log(r.msg); //--strip
        }
        assert is_ok(r);

        String a49h = blob_writer_t.add_header(new blob_header_t(new version_t((short)(blob_reader_t.current_version.value - 1)), new serid_t((short)'X')), container_of_5_wrapper_wallet_connection_t_a49_blob);
        log("a49h = {blob_reader_t::current_version - 1}'X' + container_of_5_wrapper_wallet_connection_t_a49_blob = " + a49h); //--strip
        assert is_ok(cw.read(a49h));
        assert cw.size() == 5;
    }
*/
//    bookmark_index_t test_bookmark_index_read(String enc) {
/*
        blob_t blob = new blob_t(base58.decode(enc));
        if (blob.value == null) {
            log("blob is null"); //--strip
            assert false;
            return null;
        }
        blob_reader_t reader = new blob_reader_t(blob);
        ko r = reader.read(o);
        if (is_ko(r)) {
            log(r.msg); //--strip
            assert false;
            return null;
        }
        o.dump(System.out);
        return o;
*/
//    }

//    String encode_bookmark_index(bookmark_index_t o) {

/*
        blob_t blob = new blob_t();
        blob_writer_t writer = new blob_writer_t(blob, blob_writer_t.blob_size(o));
        writer.write(o);
        String s = base58.encode(blob.value);
        return s;
*/
//    }

    bookmark_index_t create_bookmark_index_0() {
        bookmark_index_t o = new bookmark_index_t();
        return o;
    }

    void test_bookmark_index() {
        bookmark_index_t o = create_bookmark_index_0();
        blob_writer_t.writable wo = new blob_writer_t.writable(o);
        String s = wo.encode();
        log("index0: " + s); //--strip

        bookmark_index_t o2 = new bookmark_index_t();
        blob_reader_t.readable ro = new blob_reader_t.readable(o2);
        ko r = ro.read(s);
        if (is_ko(r)) {
            log(r.msg); //--strip
            assert false;
            return;
        }
        o.dump(System.out);
        assert o.size() == o2.size();
    }

    void test_bookmark_index2() {
        String s = "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";

        bookmark_index_t o = new bookmark_index_t();
        blob_reader_t.readable ro = new blob_reader_t.readable(o);
        ko r = ro.read(s);
        if (is_ko(r)) {
            log(r.msg); //--strip
            assert false;
            return;
        }
        o.dump(System.out);
        assert o.size() == 7;
    }

    public void main_test() {
        CFG.logs.set(true); //--strip

        test_encryption();
        test_startstopdaemon();
        test_blob();
        test_wallet_connection();

        test_wallet_connectionX();
//        wrapper_test_wallet_connection2();
        log("Print"); //--strip
        print_wallet_connection_blob();

        test_bookmark_index();
        test_bookmark_index2();
    }

    public static void main(String[] args)  {
        System.out.println("Init EC.");
        us.gov.crypto.ec.create_instance();
        System.out.println("Starting test program.");
        main_t o = new main_t();
        o.main_test();
    }

}

