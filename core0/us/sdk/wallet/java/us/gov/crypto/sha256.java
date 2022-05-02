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
import us.CFG;                                                                                 // CFG
import java.security.MessageDigest;                                                            // MessageDigest

public class sha256 {

    static void log(final String line) {            //--strip
        CFG.log_gov_crypto("sha256 " + line);       //--strip
    }                                               //--strip

    public static final int output_size = 32;

    private sha256() {
    }

    public static class hash_t {

        public hash_t() {
            value = new byte[output_size];
        }

        public hash_t(int zero) {
            value = new byte[output_size];
            zero();
        }

        public hash_t(byte[] v) {
            assert v.length == output_size;
            value = v;
        }

        public void zero() {
            for (int i = 0; i < output_size; ++i) value[i] = 0;
        }

        public String b58() {
            return base58.encode(value);
        }

        public boolean equals(hash_t other) {
            for (int i = 0; i < output_size; ++i) if (value[i] != other.value[i]) return false;
            return true;
        }

        public int read(byte[] src, int pos) {
            for (int i = 0; i < output_size; ++i) {
                value[i] = src[pos++];
            }
            return pos;
        }

        public int write(byte[] dst, int pos) {
            for (int i = 0; i < output_size; ++i) {
                dst[pos++] = value[i];
            }
            return pos;
        }

        public byte[] value;
    }

    public static hash_t hash(String text) {
        return hash(text.getBytes());
    }

    public static hash_t hash(byte[] text) {
        try {
            MessageDigest hasher = MessageDigest.getInstance("SHA-256");
            return new hash_t(hasher.digest(text));
        }
        catch (Exception e) {
            return null;
        }
    }

}

