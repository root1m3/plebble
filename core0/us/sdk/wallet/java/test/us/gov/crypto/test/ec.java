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
package us.gov.crypto.test;

import java.security.PrivateKey;
import org.spongycastle.jce.spec.ECPrivateKeySpec;
import org.spongycastle.jce.spec.ECPublicKeySpec;
import org.spongycastle.jce.spec.ECParameterSpec;
import org.spongycastle.crypto.params.ECPrivateKeyParameters;
import java.security.PublicKey;
import java.security.KeyPair;
import java.security.NoSuchProviderException;
import java.security.NoSuchAlgorithmException;
import java.security.InvalidAlgorithmParameterException;
import java.security.spec.InvalidKeySpecException;
import java.security.GeneralSecurityException;
import java.math.BigInteger;
import us.gov.crypto.base58;
import java.util.Arrays;

public class ec {

    KeyPair a;
    KeyPair b;
    KeyPair c;

    //tests that conversion between raw private key and java PrivateKey type is successful.
    private boolean test_key_conversion() {
        BigInteger priv1 = us.gov.crypto.ec.instance.generate_private_int();
        if (priv1==null) return false;
        PrivateKey priv = us.gov.crypto.ec.instance.get_private_key(priv1);
        if (priv==null) return false;
        BigInteger priv2 = us.gov.crypto.ec.instance.get_private_int(priv);
        if (priv2==null) return false;
        return priv1==priv2;
    }

    //tests shared secret generation with real raw key values.
    private boolean test_shared_secret_raw_keys() {
        PrivateKey k = us.gov.crypto.ec.instance.get_private_key(base58.decode("9pKQDhcZsi9V1qVhaDDnqV7HyiatxUEwTiLjqtqD7ZR6"));
        if (k==null) return false;
        PublicKey p = us.gov.crypto.ec.instance.get_public_key(base58.decode("or69BumA7ZALzHNKjuxDLtHithXo3BfzJ2VYg73uNizk"));
        if (p==null) return false;
        byte[] key = us.gov.crypto.ec.instance.generate_shared_key(k, p, 16);
        if (key==null) return false;
        return base58.encode(key).equals("C1w1ufPafGrkgPCZRFRT1x");
    }

    //tests that two keypairs will generate the same shared secret using their own private key and the other's public key.
    private boolean test_shared_secret()  {
        KeyPair a = us.gov.crypto.ec.instance.generate_keypair();
        if (a==null) return false;
        KeyPair b = us.gov.crypto.ec.instance.generate_keypair();
        if (b==null) return false;
        PrivateKey priv_a = a.getPrivate();
        PublicKey pub_a = a.getPublic();
        PrivateKey priv_b = b.getPrivate();
        PublicKey pub_b = b.getPublic();
        byte[] key1 = us.gov.crypto.ec.instance.generate_shared_key(priv_a, pub_b, 16);
        if (key1==null) return false;
        byte[] key2 = us.gov.crypto.ec.instance.generate_shared_key(priv_b, pub_a, 16);
        if (key2==null) return false;
        return Arrays.equals(key1, key2);
    }

    public boolean test() {
        assert(test_shared_secret());
        assert(test_shared_secret_raw_keys());
        assert(test_key_conversion());
        return true;
    }

/*
    private boolean testSignAndVerify() throws GeneralSecurityException {

        KeyPair a = us.gov.crypto.ec.instance.generateKeyPair();
        String message_string = "How do you know that this message is from who you think it is?";
        String signature = us.gov.crypto.ec.instance.sign_encode(a.getPrivate(), message_string);

        return us.gov.crypto.ec.instance.verify(a.getPublic(), message_string, signature);
    }
*/
    /*
    private boolean testKeyConversion2() throws NoSuchAlgorithmException, NoSuchProviderException, InvalidAlgorithmParameterException, InvalidKeySpecException{

        byte[] privateKeyBytes = base58.decode("4ACKBcXXhtGb3NtZzSfwgSs3GqCgCBY65juF2UVSJQR2");
        byte[] publicKeyBytes = base58.decode("dPtUg631Hh7tL8t3wK6KHDwFfQzLmcHtkEAH5mSsANX3");

        PrivateKey privateKey = ec.getPrivateKey(privateKeyBytes);
        PublicKey publicKey = ec.getPublicKey(publicKeyBytes);

        PublicKey publicKey2 = ec.getPublicKeyFromPrivate(privateKey);

        //byte[] publicKeyBytes2 =
        //System.out.println("pub2 bytes" + publicKeyBytes2);
        System.out.println("pub bytes" + publicKeyBytes);
        //return Arrays.equals(publicKeyBytes,publicKeyBytes2);
        return true;

    }
*/
}
