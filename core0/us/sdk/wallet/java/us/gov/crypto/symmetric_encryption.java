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
package us.gov.crypto;
import org.bouncycastle.util.Arrays;                                                           // Arrays
import org.bouncycastle.jce.provider.BouncyCastleProvider;                                     // BouncyCastleProvider
import us.CFG;                                                                                 // CFG
import javax.crypto.Cipher;                                                                    // Cipher
import org.bouncycastle.math.ec.ECPoint;                                                       // ECPoint
import java.security.GeneralSecurityException;                                                 // GeneralSecurityException
import javax.crypto.spec.IvParameterSpec;                                                      // IvParameterSpec
import java.security.KeyPair;                                                                  // KeyPair
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import java.security.PrivateKey;                                                               // PrivateKey
import java.security.PublicKey;                                                                // PublicKey
import javax.crypto.SecretKey;                                                                 // SecretKey
import javax.crypto.spec.SecretKeySpec;                                                        // SecretKeySpec
import java.security.SecureRandom;                                                             // SecureRandom
import java.security.Security;                                                                 // Security
import java.nio.charset.StandardCharsets;                                                      // StandardCharsets

public class symmetric_encryption {

    static void log(final String line) {                        //--strip
        CFG.log_gov_crypto("symmetric_encryption: " + line);    //--strip
    }                                                           //--strip

    static SecureRandom random = null;
    public static final int iv_size = 12;
    public static final int key_size = 16;

    public static final ko KO_89601 = new ko("KO 89601 Invalid length.");
    public static final ko KO_40751 = new ko("KO 40751 Unable to decrypt.");
    public static final ko KO_44302 = new ko("KO 44302 Unable to encrypt.");

    Cipher enc;
    Cipher dec;
    byte[] key;
    byte [] iv = new byte[iv_size];

    static {
        new linux_secure_random();
        random = new SecureRandom();
    }

    public symmetric_encryption() {
        log("symmetric_encryption");  //--strip
    }

    public ko init(PrivateKey priv_a, PublicKey pub_b) {
        log("init"); //--strip
        try {
            log("adding security provider [Spongy/Bouncy]Castle"); //--strip
            Security.addProvider(new BouncyCastleProvider());
            log("added security provider [Spongy/Bouncy]Castle"); //--strip
            enc = Cipher.getInstance("AES/GCM/NoPadding");
            dec = Cipher.getInstance("AES/GCM/NoPadding");
            key = ec.instance.generate_shared_key(priv_a, pub_b, key_size);
            if (key == null) {
                ko r=new ko("KO 66053 Invalid key.");
                log(r.msg); //--strip
                return r;
            }
            if(key.length != key_size) {
                ko r=new ko("KO 66054 Invalid key size.");
                log(r.msg); //--strip
                return r;
            }
        }
        catch (Exception e) {
                ko r=new ko("KO 66055 Invalid keys.");
                log(r.msg); //--strip
                return r;
        }
        return ok;
    }

    public int get_key_size() {
        return key_size;
    }

    public pair<ko, byte[]> encrypt(byte[] plaintext) {
        try {
            random.nextBytes(iv);
            enc.init(Cipher.ENCRYPT_MODE, new SecretKeySpec(key, "AES"), new IvParameterSpec(iv), random);
            return new pair<ko, byte[]>(ok, Arrays.concatenate(enc.doFinal(plaintext), iv));
        }
        catch (Exception e) {
            ko r = KO_44302;
            log(r.msg); //--strip
            return new pair<ko, byte[]>(r, null);
        }
    }

    //Decrypt returns an empty byte array if the ciphertext is invalid. Invalid ciphertext would
    //otherwise cause an exception as the algorithm is unable to authenticate the ciphertext.
    public pair<ko, byte[]> decrypt(byte[] encrypted, int offset, int length) {
        int sz = length - iv_size;
        if (sz<0){
            ko r=KO_89601;
            log(r.msg); //--strip
            return new pair<ko, byte[]>(r, null);
        }
        try {
            dec.init(Cipher.DECRYPT_MODE, new SecretKeySpec(key, "AES"), new IvParameterSpec(encrypted, offset+sz, iv_size), random);
            return new pair<ko, byte[]>(ok, dec.doFinal(encrypted, offset, sz));
        }
        catch (Exception e) {
            ko r = KO_40751;
            log(r.msg + " " +e.getMessage()); //--strip
            return new pair<ko, byte[]>(r, null);
        }
    }

    public pair<ko, byte[]> decrypt(byte[] encrypted) {
        return decrypt(encrypted, 0, encrypted.length);
    }

    public static boolean test(String plaintext_string, PrivateKey priv_a, PublicKey pub_b, PrivateKey priv_b, PublicKey pub_a) {
        log("testing " + plaintext_string); //--strip
        byte[] plaintext = plaintext_string.getBytes();
        us.gov.crypto.symmetric_encryption se_a = new us.gov.crypto.symmetric_encryption();
        ko r0 = se_a.init(priv_a, pub_b);
        if (ko.is_ko(r0)) {
            log(r0.msg); //--strip
            return false;
        }
        byte[] ciphertext;
        {
            pair<ko, byte[]> r = se_a.encrypt(plaintext);
            if (ko.is_ko(r.first)) {
                return false;
            }
            ciphertext = r.second;
        }
        us.gov.crypto.symmetric_encryption se_b= new us.gov.crypto.symmetric_encryption();
        if (se_b.init(priv_b, pub_a)!=ok) {
            return false;
        }
        byte[] plaintext2;
        {
            pair<ko, byte[]> r = se_b.decrypt(ciphertext);
            if (ko.is_ko(r.first)) {
                return false;
            }
            plaintext2=r.second;
        }
        boolean eq = java.util.Arrays.equals(plaintext, plaintext2);
        System.out.println("decrypted: " + new String(plaintext2));
        System.out.println("Symmetric encryption tests: " + (eq ? "OK" : "KO"));
        return eq;
    }

    public static boolean test(String plaintext) {
        log("Symmetric encryption tests"); //--strip
        KeyPair a = us.gov.crypto.ec.instance.generate_keypair();
        if (a == null) {
            log("E1"); //--strip
            return false;
        }
        KeyPair b = us.gov.crypto.ec.instance.generate_keypair();
        if (b == null) {
            log("E2"); //--strip
            return false;
        }
        PrivateKey priv_a = a.getPrivate();
        PublicKey pub_a = a.getPublic();
        PrivateKey priv_b = b.getPrivate();
        PublicKey pub_b = b.getPublic();
        return test(plaintext, priv_a, pub_b, priv_b, pub_a);
    }

    public static boolean test() {
        if (!test("The Times 03/Jan/2009 Chancellor on brink of second bailout for banks.")) {
            System.out.println("KO 60500 Test Failed.");
            return false;
        }
        if (!test("washingtonpost.com 16/Apr/2020 Glitches prevent $1,200 stimulus checks from reaching millions.")) {
            System.out.println("KO 60501 Test Failed.");
            return false;
        }
        System.out.println("Tests PASSED.");
        return true;
    }

}

