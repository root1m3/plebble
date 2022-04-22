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
package us.gov.crypto.test;

import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.KeyPair;
import java.security.GeneralSecurityException;
import java.util.Arrays;
import us.pair;
import us.ko;

public class symmetric_encryption {

    public static void log(final String line) { //--strip
       System.out.println("symmetric_encryption tests: "+line); //--strip
    } //--strip

    KeyPair a;
    KeyPair b;
    KeyPair c;

    public boolean main() {

        assert us.gov.crypto.symmetric_encryption.test();

        log("Creating keys"); //--strip
        a = us.gov.crypto.ec.instance.generate_keypair();
        if (a==null) return false;
        b = us.gov.crypto.ec.instance.generate_keypair();
        if (b==null) return false;
        c = us.gov.crypto.ec.instance.generate_keypair();
        if (c==null) return false;
        PrivateKey priv_a = a.getPrivate();
        PublicKey pub_a = a.getPublic();
        PrivateKey priv_b = b.getPrivate();
        PublicKey pub_b = b.getPublic();
        PrivateKey priv_c = c.getPrivate();
        PublicKey pub_c = c.getPublic();
        log("tests enc/dec"); //--strip
        //test that encryption then decryption will retrieve original plaintext.
        assert(test_encrypt_decrypt("encrypt this"                                                                                                                        ));
        assert(test_encrypt_decrypt(""                                                                                                                                    ));
        assert(test_encrypt_decrypt(",./;'#[]-=123456DFJLSKDFJERUEIUR  \n rtr"                                                                                          ));
        assert(test_encrypt_decrypt("0"                                                                                                                                    ));
        assert(test_encrypt_decrypt("𝕋𝕙𝕖 𝕢𝕦𝕚𝕔𝕜 𝕓𝕣𝕠𝕨𝕟 𝕗𝕠𝕩 𝕛𝕦𝕞𝕡𝕤 𝕠𝕧𝕖𝕣 𝕥𝕙𝕖 𝕝𝕒𝕫𝕪 𝕕𝕠𝕘"                                                                                     ));
        assert(test_encrypt_decrypt("パーティーへ行かないか"                                                                                                                 ));
        assert(test_encrypt_decrypt("˙ɐnbᴉlɐ ɐuƃɐɯ ǝɹolop ʇǝ ǝɹoqɐl ʇn ʇunpᴉpᴉɔuᴉ ɹodɯǝʇ poɯsnᴉǝ op pǝs 'ʇᴉlǝ ƃuᴉɔsᴉdᴉpɐ ɹnʇǝʇɔǝsuoɔ 'ʇǝɯɐ ʇᴉs ɹolop ɯnsdᴉ ɯǝɹo˥00˙Ɩ$-" ));
        assert(test_encrypt_decrypt("null"                                                                                                                                ));
        assert(test_encrypt_decrypt("-$1.00"                                                                                                                            ));
        log("tests enc 1"); //--strip

        //test that same plaintext is encrypted to different ciphertexts.
        assert(test_encrypt_multiple("The ciphertext should be different although the plaintext is the same"));

        //test that ciphertext is decrypted to the same plaintext.
        assert(test_decrypt_multiple("The plaintext should be the same each time we decrypt"));

        //test that message can't be decoded with the wrong key.
        assert(!test_encrypt_decrypt_keys("test that message can't be decoded with the wrong key", priv_a, pub_b, priv_c, pub_a));

        //decrypting invalid ciphertext should return an empty vector.
        assert(test_decrypt_nulls(""));
        assert(test_decrypt_nulls("this string hasn't been encrypted so decryption will fail"));
        return true;

    }

    public boolean test_encrypt_decrypt(String plaintext_string)  {
        KeyPair a = us.gov.crypto.ec.instance.generate_keypair();
        if (a==null) return false;
        KeyPair b = us.gov.crypto.ec.instance.generate_keypair();
        if (b==null) return false;
        PrivateKey priv_a = a.getPrivate();
        PublicKey pub_a = a.getPublic();
        PrivateKey priv_b = b.getPrivate();
        PublicKey pub_b = b.getPublic();
        log("test_encrypt_decrypt_keys"); //--strip
        return test_encrypt_decrypt_keys(plaintext_string, priv_a, pub_b, priv_b, pub_a);
    }

    public boolean test_encrypt_decrypt_keys(String plaintext_string, PrivateKey priv_a, PublicKey pub_b, PrivateKey priv_b, PublicKey pub_a) {
        log("plaintext_string: "+plaintext_string); //--strip
        byte[] plaintext = plaintext_string.getBytes();
        us.gov.crypto.symmetric_encryption se_a= new us.gov.crypto.symmetric_encryption();
        ko r=se_a.init(priv_a,pub_b);
        if (ko.is_ko(r)) {
            log("A "+r.msg); //--strip
            assert false;
        }
        pair<ko, byte[]> ciphertext = se_a.encrypt(plaintext);
        us.gov.crypto.symmetric_encryption se_b= new us.gov.crypto.symmetric_encryption();
        ko r2=se_b.init(priv_b, pub_a);
        if (ko.is_ko(r2)) {
            log("B "+r2.msg); //--strip
            assert false;
        }

        {
            pair<ko, byte[]> decryptedtext = se_b.decrypt(ciphertext.second);
            if (ko.is_ko(decryptedtext.first)) return false;
            assert Arrays.equals(plaintext, decryptedtext.second);
        }
        {
            //System.out.println("E2");
            byte[] buf=new byte[ciphertext.second.length+500];
            for (int i=0; i<ciphertext.second.length; ++i) {
                buf[200+i]=ciphertext.second[i];
            }
            pair<ko, byte[]> decryptedtext = se_b.decrypt(buf, 200, ciphertext.second.length);
            //System.out.println("E2-0");
            if (ko.is_ko(decryptedtext.first)) return false;
            //System.out.println("E2-1");
            assert Arrays.equals(plaintext, decryptedtext.second);
            //System.out.println("E2-2");
        }
        return true;
    }

    public boolean test_encrypt_multiple(String plaintext_string) {

        KeyPair a = us.gov.crypto.ec.instance.generate_keypair();
        if (a==null) return false;
        KeyPair b = us.gov.crypto.ec.instance.generate_keypair();
        if (b==null) return false;

        PrivateKey priv_a = a.getPrivate();
        PublicKey pub_a = a.getPublic();
        PrivateKey priv_b = b.getPrivate();
        PublicKey pub_b = b.getPublic();

        byte[] plaintext = plaintext_string.getBytes();

        us.gov.crypto.symmetric_encryption se_a = new us.gov.crypto.symmetric_encryption();
        ko r=se_a.init(priv_a, pub_b);
        if (ko.is_ko(r)) return false;
        pair<ko, byte[]> ciphertext_1 = se_a.encrypt(plaintext);
        if (ko.is_ko(ciphertext_1.first)) return false;
        pair<ko, byte[]> ciphertext_2 = se_a.encrypt(plaintext);
        if (ko.is_ko(ciphertext_2.first)) return false;
        return !Arrays.equals(ciphertext_1.second, ciphertext_2.second);
    }

    public boolean test_decrypt_nulls(String ciphertext_string) {

        KeyPair a = us.gov.crypto.ec.instance.generate_keypair();
        if (a==null) return false;
        KeyPair b = us.gov.crypto.ec.instance.generate_keypair();
        if (b==null) return false;
//System.out.println("A0");
        PrivateKey priv_a = a.getPrivate();
        PublicKey pub_a = a.getPublic();
        PrivateKey priv_b = b.getPrivate();
        PublicKey pub_b = b.getPublic();

        byte[] ciphertext = ciphertext_string.getBytes();
        us.gov.crypto.symmetric_encryption se = new us.gov.crypto.symmetric_encryption();
        ko r=se.init(priv_a, pub_b);
//System.out.println("A1");
        if (ko.is_ko(r)) return false;
//System.out.println("A2 "+ ciphertext.length);
        pair<ko, byte[]> decryptedtext = se.decrypt(ciphertext);
//System.out.println("A3");
        if (ko.is_ok(decryptedtext.first)) return false;
        if (ciphertext.length<us.gov.crypto.symmetric_encryption.iv_size) {
 //           System.out.println("A3<");
            assert decryptedtext.first==us.gov.crypto.symmetric_encryption.KO_89601;
        }
        else {
 //           System.out.println("A3>");
            assert decryptedtext.first==us.gov.crypto.symmetric_encryption.KO_40751;
        }
//System.out.println("A4");
        return true;
//        return Arrays.equals(decryptedtext.second,new byte[0]);
    }

    public boolean test_decrypt_multiple(String plaintext_string) {

        KeyPair a = us.gov.crypto.ec.instance.generate_keypair();
        if (a==null) return false;
        KeyPair b = us.gov.crypto.ec.instance.generate_keypair();
        if (b==null) return false;

        PrivateKey priv_a = a.getPrivate();
        PublicKey pub_a = a.getPublic();
        PrivateKey priv_b = b.getPrivate();
        PublicKey pub_b = b.getPublic();

        byte[] plaintext = plaintext_string.getBytes();

        us.gov.crypto.symmetric_encryption se_a = new us.gov.crypto.symmetric_encryption();
        ko r=se_a.init(priv_a, pub_b);
        if (ko.is_ko(r)) return false;

        pair<ko, byte[]> ciphertext = se_a.encrypt(plaintext);
        if (ko.is_ko(ciphertext.first)) return false;
        us.gov.crypto.symmetric_encryption se_b = new us.gov.crypto.symmetric_encryption();
        ko r2=se_b.init(priv_b, pub_a);
        if (ko.is_ko(r2)) return false;
        pair<ko, byte[]> decryptedtext1 = se_b.decrypt(ciphertext.second);
        pair<ko, byte[]> decryptedtext2 = se_b.decrypt(ciphertext.second);
        if (ko.is_ko(decryptedtext1.first)) return false;
        if (ko.is_ko(decryptedtext2.first)) return false;
        return Arrays.equals(decryptedtext1.second, decryptedtext2.second);
    }

}

