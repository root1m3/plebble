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

public class main extends us.wallet.cli.hmi {

    public static void log(final String line) { //--strip
       // System.out.println("walletj.java: "+line); //--strip
    } //--strip

    public main() {
        super(new shell_args("-home " + System.getenv("HOME") + "/.us -whost localhost -wp 16673"), System.out);
    }
/*
    public byte[] hexStringToByteArray(String s) {
        int len = s.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
                    + Character.digit(s.charAt(i+1), 16));
        }
        return data;
    }

    String readFile(String path, Charset encoding) throws IOException {
        byte[] encoded = Files.readAllBytes(Paths.get(path));
        return new String(encoded, encoding);
    }

    byte[] readBinFile(String path) throws IOException {
        return Files.readAllBytes(Paths.get(path));
    }

    public void store_file(String path, byte[] bytes) {
//          api.store_file(hexStringToByteArray("71156A681DA0E72A81BB092D4E7BBC3A3664B473"),b, System.out);
          api.store_file(address, path, bytes, System.out);
    }

    public void req_file(String hashb58) {
          api.file(base58.decode(hashb58),System.out);
    }

    public void store_kv(String k, String v) {
          api.store_kv(address,k,v,System.out);
    }

    public void rm_kv(String k) {
          api.rm_kv(address,k,System.out);
    }

    public void rm_f(String path, String hashb58) {
          api.rm_file(address,path,base58.decode(hashb58),System.out);
    }

    public static class console_dispatcher_t implements datagram.dispatcher_t {

        static void log(final String line) { //--strip
            System.out.println("main.java.in: "+line); //--strip
        } //--strip

        void string_dgram(datagram d) {
            String x=d.parse_string();
            if (x.startsWith("KO")) {
                log(x); //--strip
                return;
            }
            System.out.println(x);
            return;
        }

        @Override
        public void dispatch(datagram d) {
            switch(d.service) {
            case protocol.wallet_store_kv+(1<<2):    {
                string_dgram(d);
                return;
            }
            case protocol.wallet_rm_kv+(1<<2):    {
                string_dgram(d);
                return;
            }
            case protocol.wallet_rm_file+(1<<2):    {
                string_dgram(d);
                return;
            }
            case protocol.wallet_search+(1<<2):    {
                string_dgram(d);
                return;
            }
            case protocol.wallet_store_file+(1<<2):    {
                string_dgram(d);
                return;
            }
            case protocol.wallet_file+(1<<2):    {
                d.write_bin(System.out);
                return;
            }
            case protocol.wallet_balance+(1<<2):    {
                string_dgram(d);
                return;
            }


            default: {
                log("KO 9672 - UNHANDLED DGRAM svc="+d.service+" default tratment as string"); //--strip
                string_dgram(d);
            }
        }


    }
    }
*/

/*
    void check(String a, String b) {
        assert a!=null;
        assert b!=null;
        assert a.equals(b);
    }

    void check(hash_t a, hash_t b) {
        assert a!=null;
        assert b!=null;
        assert a.equals(b);
    }

    void check(sha256.hash_t a, sha256.hash_t b) {
        assert a!=null;
        assert b!=null;
        assert a.equals(b);
    }

    void check(byte[] a, byte[] b) {
        assert a!=null;
        assert b!=null;
        assert Arrays.equals(a,b);
    }

    void check(uint64_t a, uint64_t b) {
        assert a!=null;
        assert b!=null;
        assert a.value==b.value;
    }

    void check(uint32_t a, uint32_t b) {
        assert a!=null;
        assert b!=null;
        //System.out.println("uint32_t "+ a.value+" "+ b.value);
        assert a.value==b.value;
    }

    void check(uint16_t a, uint16_t b) {
        assert a!=null;
        assert b!=null;
        assert a.value==b.value;
    }

    void check(uint8_t a, uint8_t b) {
        assert a!=null;
        assert b!=null;
        assert a.value==b.value;
    }

    void check(int64_t a, int64_t b) {
        assert a!=null;
        assert b!=null;
        assert a.value==b.value;
    }

    void check(PublicKey a, PublicKey b) {
        assert a!=null;
        assert b!=null;
        assert a.equals(b);
    }

    void check(PrivateKey a, PrivateKey b) {
        assert a!=null;
        assert b!=null;
        assert a.equals(b);
    }

    void check_v_hs_(ArrayList<pair<us.gov.crypto.ripemd160.hash_t,String>> a, ArrayList<pair<us.gov.crypto.ripemd160.hash_t,String>> b) {
        assert a!=null;
        assert b!=null;
        assert a.size() == b.size();
        for (int i=0; i<a.size(); ++i) {
            check(a.get(i).first, b.get(i).first);
            check(a.get(i).second, b.get(i).second);
        }
    }

    void check_v_hu4u2_(ArrayList<tuple3<us.gov.crypto.ripemd160.hash_t, uint32_t, uint16_t>> a, ArrayList<tuple3<us.gov.crypto.ripemd160.hash_t, uint32_t, uint16_t>>  b) {
        assert a!=null;
        assert b!=null;
        assert a.size() == b.size();
        //System.out.println("sz="+b.size());
        for (int i=0; i<a.size(); ++i) {
        //System.out.println("i="+i+"/"+b.size());
            check(a.get(i).item0, b.get(i).item0);
        //System.out.println("a.itm1="+Long.toUnsignedString(a.get(i).item1.value)+" b.itm1="+b.get(i).item1.value);

            check(a.get(i).item1, b.get(i).item1);
            check(a.get(i).item2, b.get(i).item2);
        }
    }

    void check_v_h_(ArrayList<us.gov.crypto.ripemd160.hash_t> a, ArrayList<us.gov.crypto.ripemd160.hash_t> b) {
        assert a!=null;
        assert b!=null;
        assert a.size() == b.size();
        for (int i=0; i<a.size(); ++i) {
            check(a.get(i), b.get(i));
        }
    }

    void check_v_s_(ArrayList<String> a, ArrayList<String> b) {
        assert a!=null;
        assert b!=null;
        assert a.size() == b.size();
        for (int i=0; i<a.size(); ++i) {
            check(a.get(i), b.get(i));
        }
    }

    void check_vss(ArrayList<pair<String, String>> a, ArrayList<pair<String, String>> b) {
        assert a!=null;
        assert b!=null;
        assert a.size() == b.size();
        for (int i=0; i<a.size(); ++i) {
            check(a.get(i).first, b.get(i).first);
            check(a.get(i).second, b.get(i).second);
        }
    }


    int mode=0;
    Random rand = new Random();

    sha256.hash_t create_instance_H() {
        if (mode==0) return new sha256.hash_t(0);
        byte[] x=new byte[30];
        rand.nextBytes(x);
        return us.gov.crypto.sha256.hash(x);
    }

    PrivateKey create_instance_k() {
        KeyPair k=us.gov.crypto.ec.instance.generate_keypair();
        return k.getPrivate();
    }

    PublicKey create_instance_p() {
        KeyPair k=us.gov.crypto.ec.instance.generate_keypair();
        return k.getPublic();
    }

    hash_t create_instance_h() {
        if (mode==0) return new hash_t(0);
        byte[] x=new byte[30];
        rand.nextBytes(x);
        return us.gov.crypto.ripemd160.hash(x);
    }

    uint64_t create_instance_u8() {
        if (mode==0) return new uint64_t(0);
        if (mode==1) return new uint64_t("18446744073709551615");
        return new uint64_t(rand.nextLong());
    }

    int64_t create_instance_i8() {
        if (mode==0) return new int64_t(-9223372036854775808L);
        if (mode==1) return new int64_t(9223372036854775807L);
        return new int64_t(rand.nextLong());
    }

    uint16_t create_instance_u2() {
        if (mode==0) return new uint16_t(0);
        if (mode==1) return new uint16_t(65535);
        return new uint16_t(rand.nextInt(65536));
    }

    uint8_t create_instance_u1() {
        if (mode==0) return new uint8_t((short)0);
        if (mode==1) return new uint8_t((short)255);
        return new uint8_t((short)rand.nextInt(256));
    }

    uint32_t create_instance_u4() {
        if (mode==0) return new uint32_t(0);
        if (mode==1) return new uint32_t(0x00000000FFFFFFFF); //4294967295L);
        return new uint32_t((long)(rand.nextInt(0x10000)<<16)+(long)(rand.nextInt(0x10000)));
    }

    byte[] create_instance_v() {
        if (mode==0) return new byte[0];
        byte[] x=new byte[1+rand.nextInt(300)];
        rand.nextBytes(x);
        return x;
    }

    String create_instance_s() {
        if (mode==0) return new String();
        return new String("The lazy dog jumped over the tiny nasty ant and ant blood stained its toes.");
    }

    pair<String, String> create_instance_ss() {
        if (mode == 0) return new pair<String, String>();
        return new pair<String, String>(new String("The lazy dog jumped over the tiny nasty ant and ant blood stained its toes."), new String("Lorem ipsum donor guns'n roses"));
    }

    ArrayList<pair<hash_t, String>> create_instance_v_hs_() {
        ArrayList<pair<hash_t, String>> a=new ArrayList<pair<hash_t, String>>();
        if (mode==0) return a;
        int sz=1+rand.nextInt(300);
        for (int i=0; i<sz; ++i) {
            a.add(new pair<hash_t, String>(create_instance_h(), create_instance_s()));
        }
        return a;

    }


    ArrayList<tuple3<hash_t, uint32_t, uint16_t>> create_instance_v_hu4u2_() {
        ArrayList<tuple3<hash_t, uint32_t, uint16_t>> a=new ArrayList<tuple3<hash_t, uint32_t, uint16_t>>();
        if (mode==0) return a;
        int sz=1+rand.nextInt(300);
        for (int i=0; i<sz; ++i) {
            uint32_t u4=create_instance_u4();
            a.add(new tuple3<hash_t, uint32_t, uint16_t>(create_instance_h(), u4, create_instance_u2()));
            //System.out.println("==> "+i+" "+u4.value+" "+a.get(i).item1.value);
        }
        return a;

    }

    ArrayList<hash_t> create_instance_v_h_() {
        ArrayList<hash_t> a = new ArrayList<hash_t>();
        if (mode == 0) return a;
        int sz = 1 + rand.nextInt(300);
        for (int i = 0; i < sz; ++i) {
            a.add(create_instance_h());
        }
        return a;
    }

    ArrayList<String> create_instance_v_s_() {
        ArrayList<String> a = new ArrayList<String>();
        if (mode == 0) return a;
        int sz = 1 + rand.nextInt(300);
        for (int i = 0; i < sz; ++i) {
            a.add(create_instance_s());
        }
        return a;
    }

    ArrayList<pair<String, String>> create_instance_vss() {
        ArrayList<pair<String, String>> a = new ArrayList<pair<String, String>>();
        if (mode == 0) return a;
        int sz = 1 + rand.nextInt(300);
        for (int i = 0; i < sz; ++i) {
            a.add(create_instance_ss());
        }
        return a;
    }
*/

    public void main_test() {
        CFG.logs.set(true); //--strip
        log("datagram_parse"); //--strip

        log("symmetric_encryption"); //--strip
        us.gov.crypto.test.symmetric_encryption se = new us.gov.crypto.test.symmetric_encryption();
        assert se.main();

        log("hmi"); //--strip
        assert ko.is_ok(start());
        assert start() == KO_39201;


        stop();
        join();
/*
        try {
            System.out.println("make sure the hardcoded private key match the content of ~/.us/wallet/rpc_client/k");

//            PrivateKey privateKey = us.gov.crypto.ec.instance.getPrivateKey(base58.decode("5P66ZcBBNLDPmwAJPCJGGofQ2wbDnZ6tbvwNm4tpib3x"));
            PrivateKey privateKey = us.gov.crypto.ec.instance.getPrivateKey(base58.decode("81HboP5rztCrtUZhELsyiPH13ziwz4k5uRgQziXu59Pi"));
//            PrivateKey privateKey = us.gov.crypto.ec.instance.getPrivateKey(base58.decode("2kQ7syZtfeWLEvdjZRRNSvPXcyRKEnp1BP1rUjtQ8gzj"));
            KeyPair k = us.gov.crypto.ec.instance.generateKeyPair(privateKey);

            System.out.println("Private key: " +us.gov.crypto.ec.instance.to_b58(k.getPrivate()));
            System.out.println("Public key: " +us.gov.crypto.ec.instance.to_b58(k.getPublic()));

            short port=CFG.WALLET_PORT;
            short pin=0;
            dapi=new us.wallet.daemon_rpc_api(k,"127.0.0.1",port,pin,new console_dispatcher_t());
            api=dapi.get_wallet_api("");
            dapi.start();
            System.out.println("");
            System.out.println("search");
            api.search(base58.decode("3b4b1iZoD82mve3ryzwoDmRTwyme"),"Camion", System.out);
            System.out.println("");

            System.out.println("");
            System.out.println("ping");
            dapi.ping(System.out);
            System.out.println("");

            System.out.println("");
            System.out.println("balance");
            api.balance(0,System.out);
            System.out.println("");

            System.out.println("");
            System.out.println("list");
            api.list(false,System.out);
            System.out.println("");

            System.out.println("");
            System.out.println("new_address");
            api.new_address(System.out);
            System.out.println("");

            System.out.println("");
            System.out.println("add_address");
            api.add_address(base58.decode("5vvscYyiYzSMxEdPVqsPstB9wugHuanp6ts1FSWAsvFk"), System.out);
            System.out.println("");

            System.out.println("");
            System.out.println("list");
            api.list(false,System.out);
            System.out.println("");

            System.out.println("");
            System.out.println("transfer");
            api.transfer(base58.decode("gU5xx5Cvbm7NpPKz7TvdvBQmzXa"),1,base58.decode("11111111111111111111"),System.out);
            System.out.println("");

            System.out.println("");
            System.out.println("tx_make_p2pkh");
            tx_make_p2pkh_input i = new tx_make_p2pkh_input("gU5xx5Cvbm7NpPKz7TvdvBQmzXa",10,false);
            api.tx_make_p2pkh(i, System.out);
            System.out.println("");

            / *
            System.out.println("");
            System.out.println("tx_sign");
            api.tx_sign("gU5xx5Cvbm7NpPKz7TvdvBQmzXa", tx.sigcode_t.sigcode_all, tx.sigcode_t.sigcode_all, System.out);
            System.out.println("");

            System.out.println("");
            System.out.println("tx_send");
            api.tx_send("gU5xx5Cvbm7NpPKz7TvdvBQmzXa", System.out);
            System.out.println("");
            * /

            System.out.println("");
            System.out.println("tx_decode");
            api.tx_decode("2d7XqBkZ4drM16cNXBg5xrkhWLBZoo85oz5SnSLvYXmVc3Q9kqxQGtivLW3JVDYA1HCrEcaiagX7bdQdCg72NiNzrWwzZaCRhS6bvERETvy5NqdZYgGBqBdds1U5hHUqmi3Ei3bJZPiTwz61LKNxUyDMjEh5Tg3pBLfpxeAobPjBoayrFjZLMVjH32Lm6jaGxUHYUQXUm1uxGEU1KhStcMBCSMfpsbqdSzCWGJm2ZyQoBcdME33EwxwmdJ844Q4qr9esy7vSTyQZGwBXN7HdMrusfrBcg3Esqmp2nFLpxRRZYh6f6h6T5bVFadgj6PbwU7VVB8Ebi5AiGNrV92VBXdGVqJqKutsauBHhxFJsYPxNzHZbcHyZf2Tzp4fvsqNtybGwt7aVHhtGz3YjbxuyPLMeXCoWrsbs5", System.out);
            System.out.println("");

            System.out.println("");
            System.out.println("tx_check");
            api.tx_check("2d7XqBkZ4drM16cNXBg5xrkhWLBZoo85oz5SnSLvYXmVc3Q9kqxQGtivLW3JVDYA1HCrEcaiagX7bdQdCg72NiNzrWwzZaCRhS6bvERETvy5NqdZYgGBqBdds", System.out);
            System.out.println("");

            System.out.println("");
            System.out.println("list_devices");
            dapi.list_devices(System.out);
            System.out.println("");

            System.out.println("");
            System.out.println("pair");
            dapi.pair(base58.decode("26smcjHgGsGgvUEUpoCKpUnwHhL5W9y48Wtz7tryn5Yab"), "", "test", System.out);
            System.out.println("");

            System.out.println("");
            System.out.println("unpair");
            dapi.unpair(base58.decode("26smcjHgGsGgvUEUpoCKpUnwHhL5W9y48Wtz7tryn5Yab"), System.out);
            System.out.println("");

            dapi.stop();
            dapi.join();

        }
        catch(Exception e) {
            System.out.println("FAILED:" + e);
            Thread.dumpStack();
        }
*/
    }

    public static void main(String[] args)  {
        System.out.println("Init EC.");
        us.gov.crypto.ec.create_instance();
        System.out.println("Starting test program.");
        main o = new main();
        o.main_test();
    }

}

