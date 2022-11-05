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
            super("nm1", new ip4_endpoint_t(new shost_t("192.167.66.3"), new port_t(18782), new channel_t(0)));
            ts.value = 52;
            addr.value = "addr_1";
            subhome.value = "subhome_1";
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

    public void main_test() {
        CFG.logs.set(true); //--strip

        test_encryption();
        test_startstopdaemon();
        test_blob();
        test_wallet_connection();

        test_wallet_connectionX();
        wrapper_test_wallet_connection2();
    }

    public static void main(String[] args)  {
        System.out.println("Init EC.");
        us.gov.crypto.ec.create_instance();
        System.out.println("Starting test program.");
        main_t o = new main_t();
        o.main_test();
    }

}

