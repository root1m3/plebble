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

public class main extends us.wallet.cli.hmi {

    private static void log(final String line) {    //--strip
        System.out.println("main: " + line);        //--strip
    }                                               //--strip

    public main() {
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

        wallet_connection_t wc = new wallet_connection_t(ts, addr, nm, ssid, new ip4_endpoint_t(new shost_t(x), new port_t(p), new channel_t(ch)));
        test_wallet_connection_blob(wc);
    }

    void test_device_endpoint() {
    }


    public void main_test() {
        CFG.logs.set(true); //--strip

        test_encryption();
        test_startstopdaemon();
        test_blob();
        test_wallet_connection();
        test_device_endpoint();
    }

    public static void main(String[] args)  {
        System.out.println("Init EC.");
        us.gov.crypto.ec.create_instance();
        System.out.println("Starting test program.");
        main o = new main();
        o.main_test();
    }

}

