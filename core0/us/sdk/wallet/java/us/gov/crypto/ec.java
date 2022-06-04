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
package us.gov.crypto;
import java.util.Arrays;                                                                       // Arrays
import org.spongycastle.asn1.ASN1Integer;                                                      // ASN1Integer
import java.math.BigInteger;                                                                   // BigInteger
import org.spongycastle.util.BigIntegers;                                                      // BigIntegers
import org.spongycastle.jce.provider.BouncyCastleProvider;                                     // BouncyCastleProvider
import java.io.ByteArrayOutputStream;                                                          // ByteArrayOutputStream
import us.CFG;                                                                                 // CFG
import org.spongycastle.asn1.DERSequenceGenerator;                                             // DERSequenceGenerator
import org.spongycastle.math.ec.ECCurve;                                                       // ECCurve
import org.spongycastle.crypto.params.ECDomainParameters;                                      // ECDomainParameters
import org.spongycastle.jce.spec.ECNamedCurveParameterSpec;                                    // ECNamedCurveParameterSpec
import org.spongycastle.jce.spec.ECNamedCurveSpec;                                             // ECNamedCurveSpec
import org.spongycastle.jce.ECNamedCurveTable;                                                 // ECNamedCurveTable
import org.spongycastle.jce.spec.ECParameterSpec;                                              // ECParameterSpec
import org.spongycastle.math.ec.ECPoint;                                                       // ECPoint
import org.spongycastle.jce.ECPointUtil;                                                       // ECPointUtil
import org.spongycastle.jce.interfaces.ECPrivateKey;                                           // ECPrivateKey
import org.spongycastle.jce.spec.ECPrivateKeySpec;                                             // ECPrivateKeySpec
import org.spongycastle.jce.interfaces.ECPublicKey;                                            // ECPublicKey
import org.spongycastle.jce.spec.ECPublicKeySpec;                                              // ECPublicKeySpec
import java.security.GeneralSecurityException;                                                 // GeneralSecurityException
import org.spongycastle.util.encoders.Hex;                                                     // Hex
import static us.gov.crypto.types.*;                                                           // *
import java.security.InvalidAlgorithmParameterException;                                       // InvalidAlgorithmParameterException
import java.security.InvalidKeyException;                                                      // InvalidKeyException
import java.security.spec.InvalidKeySpecException;                                             // InvalidKeySpecException
import java.io.IOException;                                                                    // IOException
import java.security.KeyFactory;                                                               // KeyFactory
import java.security.KeyPairGenerator;                                                         // KeyPairGenerator
import java.security.KeyPair;                                                                  // KeyPair
import us.ko;                                                                                  // ko
import java.security.NoSuchAlgorithmException;                                                 // NoSuchAlgorithmException
import java.security.NoSuchProviderException;                                                  // NoSuchProviderException
import static us.ko.ok;                                                                        // ok
import java.security.PrivateKey;                                                               // PrivateKey
import java.security.Provider;                                                                 // Provider
import java.security.PublicKey;                                                                // PublicKey
import java.security.Security;                                                                 // Security
import org.spongycastle.crypto.digests.SHA256Digest;                                           // SHA256Digest
import java.security.Signature;                                                                // Signature

public class ec {
    linux_secure_random secure_random;
    KeyFactory factory;
    ECParameterSpec spec;
    ECDomainParameters curve;
    KeyPairGenerator generator;
    Signature dsa;

    public static ec instance = null;

    static void log(final String line) {            //--strip
        CFG.log_gov_crypto("ec: " + line);          //--strip
    }                                               //--strip

    public static void create_instance() {
        if (instance != null) {
            log("ec instance already created.");  //--strip
            return;
        }
        try {
            log("Initialising ec instance...");  //--strip
            Provider p = new BouncyCastleProvider();
            if (p == null) {
                System.err.println("KO 10199 BC Provider not found.");
                System.exit(1);
            }
            Security.insertProviderAt(p, 1);
            log("Added Provider");  //--strip
            instance = new ec("secp256k1", "ECDSA", "NONEwithECDSA");
        }
        catch (Exception e) {
            System.out.println("KO 10099 ECDSA: " + e.getMessage());
            System.exit(1);
        }
        log("ec instance Initialised.");  //--strip
    }

    public ec(String curve_name, String algorithm, String signature_algorithm) throws GeneralSecurityException {
        dsa = Signature.getInstance(signature_algorithm);
        spec = ECNamedCurveTable.getParameterSpec(curve_name);
        factory = KeyFactory.getInstance(algorithm);
        curve = new ECDomainParameters(spec.getCurve(), spec.getG(), spec.getN(), spec.getH());
        secure_random = new linux_secure_random();
        generator = KeyPairGenerator.getInstance(algorithm);
        generator.initialize(spec);
    }

    public BigInteger generate_private_int() {
        KeyPair keypair = generate_keypair();
        if (keypair == null) return null;
        return get_private_int(keypair);
    }

    public BigInteger get_private_int(PrivateKey k0) {
        ECPrivateKey k = (ECPrivateKey) k0;
        return k.getD();
    }

    private BigInteger get_private_int(KeyPair keypair) {
        return get_private_int(keypair.getPrivate());
    }

    public PrivateKey get_private_key(BigInteger k) {
        try {
            ECPrivateKeySpec ks = new ECPrivateKeySpec(k, spec);
            return factory.generatePrivate(ks);
        }
        catch (Exception e) {
            return null;
        }
    }

    public KeyPair generate_keypair() {
        try {
            return generator.generateKeyPair();
        }
        catch (Exception e) {
            return null;
        }
    }

    public KeyPair generate_keypair(PrivateKey k) {
        try {
            return new KeyPair(get_public_key(k), k);
        }
        catch (Exception e) {
            return null;
        }
    }

    public KeyPair get_keypair(BigInteger priv) {
        return generate_keypair(get_private_key(priv));
    }

    public byte[] generate_shared_key(PrivateKey priv, PublicKey pub, int length) {
        try {
            ECPrivateKey k = (ECPrivateKey) priv;
            ECPublicKey p = (ECPublicKey) pub;
            sha256.hash_t h = sha256.hash(multiply(p.getQ(), k.getD()).getEncoded(true));
            return Arrays.copyOf(h.value, 16);
        }
        catch (Exception e) {
            log("generate_shared_key " + e.getMessage()); //--strip
            return null;
        }
    }

    public byte[] to_vector(PublicKey pub) {
        try {
            ECPublicKey p = (ECPublicKey) pub;
            ECPoint point = p.getQ();
            return point.getEncoded(true);
        }
        catch (Exception e) {
            return null;
        }
    }

    public byte[] to_vector(PrivateKey priv) {
        try {
            ECPrivateKey k = (ECPrivateKey) priv;
            return BigIntegers.asUnsignedByteArray(32, k.getD());
        }
        catch (Exception e) {
            return null;
        }
    }

    public ripemd160.hash_t to_address(PublicKey k) {
        return ripemd160.hash(to_vector(k));
    }

    public String to_encoded_address(PublicKey k) {
        return base58.encode(to_address(k).value);
    }

    public String to_b58(PublicKey k) {
            return base58.encode(to_vector(k));
    }

    public String to_b58(PrivateKey k) {
        try {
            return base58.encode(to_vector(k));
        }
        catch (Exception e) {
            return null;
        }
    }

    public PrivateKey get_private_key(byte[] privateKey) {
        try {
            return get_private_key(BigIntegers.fromUnsignedByteArray(privateKey));
        }
        catch (Exception e) {
            return null;
        }
    }

    public PrivateKey get_private_key(byte[] buffer, int offset) {
        byte[] v = new byte[32];
        System.arraycopy(buffer, offset, v, 0, 32);
        return get_private_key(BigIntegers.fromUnsignedByteArray(v));
    }

    private ECPoint get_point(byte[] pub) {
        return spec.getCurve().decodePoint(pub);
    }

    public ECPoint multiply(ECPoint point, BigInteger d) {
        if (d.bitLength() > spec.getN().bitLength()) {
            d = d.mod(spec.getN());
        }
        return point.multiply(d);
    }

    public PublicKey get_public_key(BigInteger priv) {
        try {
            ECPoint point = multiply(spec.getG(), priv);
            return get_public_key(point);
        }
        catch (Exception e) {
            return null;
        }
    }

    public PublicKey get_public_key(PrivateKey priv) {
        try {
            BigInteger i = get_private_int(priv);
            return get_public_key(i);
        }
        catch (Exception e) {
            return null;
        }
    }

    public PublicKey get_public_key(ECPoint point) {
        try {
            ECPublicKeySpec s = new ECPublicKeySpec(point, spec);
            return factory.generatePublic(s);
        }
        catch (Exception e) {
            return null;
        }
    }

    public PublicKey get_public_key(byte[] public_key) {
        try {
            ECPoint point = get_point(public_key);
            return get_public_key(point);
        }
        catch (Exception e) {
            return null;
        }
    }

    public PublicKey get_public_key(byte[] buffer, int offset) {
        assert buffer.length >= offset + 33;
        byte[] v = new byte[33];
        System.arraycopy(buffer, offset, v, 0, 33);
        return get_public_key(v);
    }

    public boolean verify(PublicKey pub, sha256.hash_t msgh, sig_der_t sig_der) {
        log("verify msgh " + msgh.b58() + " pubk " + to_b58(pub) + " sig " + base58.encode(sig_der.value) + " siglen = " + sig_der.value.length); //--strip
        try {
            dsa.initVerify(pub);
            dsa.update(msgh.value);
            log("sig(der) length = " + sig_der.value.length + " hex " + Hex.toHexString(sig_der.value)); //--strip
            return dsa.verify(sig_der.value);
        }
        catch (Exception e) {
            log("verify failed " + e.getMessage()); //--strip
            return false;
        }
    }

    public sig_der_t sign(PrivateKey priv, sha256.hash_t msgh) {
        try {
            dsa.initSign(priv);
            dsa.update(msgh.value);
            byte[] r = dsa.sign();
            log("signature created length = " + r.length + " bytes. " + Hex.toHexString(r)); //--strip
            return new sig_der_t(r);
        }
        catch (Exception e) {
            return null;
        }
    }

}

