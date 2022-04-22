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
using Secp256k1Net;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Text;

namespace us.gov.crypto
{
    public class KeyPair
    {        
        public byte[] PrivateKey { get; set; }
        public byte[] PublicKey { get;set; }

        public KeyPair()
        {

        }

        public KeyPair(byte[] _public, byte[] _private)
        {
            this.PublicKey = _public;
            this.PrivateKey = _private;
        }
    }

    public class ec
    {        
        private readonly static ec _instance = new ec();

        private ec()
        {
        }

        public static ec Instance
        {
            get
            {
                return _instance;
            }
        }

        /*** Error PInvokeStackImbalance -->
           1. CTRL + ALT + E
           2. Bajo "Asistentes de depuración administrados" desmarque PInvokeStackImbalance.
        ***/

        public byte[] getPublicKey(byte[] priv)
        {
            byte[] ret = null;
            try
            {
                using (var secp256k1 = new Secp256k1())
                {
                    byte[] publicKey = new byte[64];
                    secp256k1.PublicKeyCreate(publicKey, priv);
                    byte[] publicKeyCompressed = new byte[33];
                    secp256k1.PublicKeySerialize(publicKeyCompressed, publicKey, Flags.SECP256K1_EC_COMPRESSED);
                    return publicKeyCompressed;
                }
            }
            catch (Exception)
            {     
                
            }
            return ret;
        }


        public KeyPair getKeyPair()
        {
           KeyPair ret = new KeyPair();
            try
            {
                using (var secp256k1 = new Secp256k1())
                {
                    byte[] privateKey = new byte[32];
                    var rnd = System.Security.Cryptography.RandomNumberGenerator.Create();
                    do { rnd.GetBytes(privateKey); }
                    while (!secp256k1.SecretKeyVerify(privateKey));
                    byte[] publicKey = new byte[64];
                    secp256k1.PublicKeyCreate(publicKey, privateKey);
                    byte[] publicKeyCompressed = new byte[33];
                    secp256k1.PublicKeySerialize(publicKeyCompressed, publicKey, Flags.SECP256K1_EC_COMPRESSED);
                    ret = new KeyPair(publicKeyCompressed, privateKey);
                }
            }
            catch (Exception)
            {

            }
            return ret;
        }

        public byte[] generate_shared_key(byte[] priv, byte[] pub, int length) {
            try
            {
                using (var secp256k1 = new Secp256k1())
                {
                    byte[] result = new Byte[32];
                    if (secp256k1.Ecdh(result, pub, priv))
                    {
                        return result;
                    }
                }
            }
            catch (Exception)
            {             
            }
            return null;
        }

        public String to_b58(byte[] k)
        {
            try
            {
                return base58.encode(k);
            }
            catch (Exception)
            {
                return null;
            }
        }

        public bool verify(byte[] pub, String text, String signature_der_b58)
        {
            try
            {
                using (var secp256k1 = new Secp256k1())
                {
                    byte[] signatureOutput = new byte[Secp256k1.SIGNATURE_LENGTH];
                    byte[] pubkeyParsed = new byte[Secp256k1.PUBKEY_LENGTH];
                    var messageBytes = Encoding.UTF8.GetBytes(text);
                    var messageHash = System.Security.Cryptography.SHA256.Create().ComputeHash(messageBytes);
                    //secp256k1.SignatureNormalize()
                    var DERsignature = base58.decode(signature_der_b58);
                    Debug.Assert(secp256k1.PublicKeyParse(pubkeyParsed, pub));
                    Debug.Assert(secp256k1.SignatureParseDer(signatureOutput, DERsignature));
                    return secp256k1.Verify(signatureOutput, messageHash, pubkeyParsed);
                }
            }
            catch (Exception e)
            {
                System.Diagnostics.Debug.WriteLine(e.Message);
                return false;
            }
        }

        //delegate int secp256k1_ecdsa_signature_serialize_der(const secp256k1_context* ctx, unsigned char* output, size_t *outputlen, const secp256k1_ecdsa_signature* sig) 

        public byte[] sign(byte[] priv, String message)
        {
            try
            {
                using (var secp256k1 = new Secp256k1())
                {
                    var messageBytes = Encoding.UTF8.GetBytes(message);
                    var messageHash = System.Security.Cryptography.SHA256.Create().ComputeHash(messageBytes);
                    var signature = new byte[64];
                    secp256k1.Sign(signature, messageHash, priv);

                    byte[] signatureDer = new byte[Secp256k1.SERIALIZED_DER_SIGNATURE_MAX_SIZE];
                    int outL = 0;
                    Debug.Assert(secp256k1.SignatureSerializeDer(signatureDer, signature, out outL));
                    Array.Resize(ref signatureDer, outL);

                    return signatureDer; 
                }
            }
            catch (Exception)
            {
                //System.out.println(e.getMessage());
                return new byte[0];
            }
        }

        public String sign_encode(byte[] priv, String message)
        {
            return base58.encode(sign(priv, message));
        }
    }

    public static class Extensions
    {
        public static string ToHexString(this byte[] span)
        {
            return "0x" + BitConverter.ToString(span).Replace("-", "").ToLowerInvariant();
        }

        public static byte[] HexToBytes(this string hexString)
        {
            int chars = hexString.Length;
            byte[] bytes = new byte[chars / 2];
            for (int i = 0; i < chars; i += 2)
            {
                bytes[i / 2] = Convert.ToByte(hexString.Substring(i, 2), 16);
            }
            return bytes;
        }
    }

    public abstract class BigIntegerConverter
    {
        /// <summary>
        /// Obtains the bytes that represent the BigInteger as if it was a big endian 256-bit integer.
        /// </summary>
        /// <param name="bigInteger">The BigInteger to obtain the byte representation of.</param>
        /// <returns>Returns the bytes that represent BigInteger as if it was a 256-bit integer.</returns>
        public static byte[] GetBytes(BigInteger bigInteger, int byteCount = 32)
        {
            // Obtain the bytes which represent this BigInteger.
            byte[] result = bigInteger.ToByteArray();

            // We'll operate on the data in little endian (since we'll extend the array anyways and we'd have to copy the data over anyways).
            if (!BitConverter.IsLittleEndian)
                Array.Reverse(result);

            // Store the original size of the data, then resize it to the size of a word.
            int originalSize = result.Length;
            Array.Resize(ref result, byteCount);

            // BigInteger uses the most significant bit as sign and optimizes to return values like -1 as 0xFF instead of as 0xFFFF or larger (since there is no bound size, and negative values have all leading bits set)
            // Instead if we wanted to represent 256 (0xFF), we would add a leading zero byte so the sign bit comes from it, and will be zero (positive) (0x00FF), this way, BigInteger knows to represent this as a positive value.
            // Because we resized the array already, it would have added leading zero bytes which works for positive numbers, but if it's negative, all extended bits should be set, so we check for that case.

            // If the integer is negative, any extended bits should all be set.
            if (bigInteger.Sign < 0)
                for (int i = originalSize; i < result.Length; i++)
                    result[i] = 0xFF;

            // Flip the array so it is in big endian form.
            Array.Reverse(result);

            return result;
        }
    }
}
