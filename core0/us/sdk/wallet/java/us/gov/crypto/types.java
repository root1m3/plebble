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
import static us.stdint.*;                                                                     // *
import us.ko;                                                                                  // ko
import us.pair;                                                                                // pair
import java.security.PrivateKey;                                                               // PrivateKey
import java.security.PublicKey;                                                                // PublicKey

public class types {

    public static class sig_t {

        public static int mem_size = 64;
        public static int ser_size = 64;

        public sig_t() {
            value = new byte[mem_size];
        }

        public sig_t(byte[] bytes) {
            assert bytes.length == mem_size;
            value = bytes;
        }

        public boolean is_zero() {
            for (int i = 0; i < mem_size; ++i) {
                if (value[i] != 0) return false;
            }
            return true;
        }

        public boolean is_not_zero() {
            for (int i = 0; i < mem_size; ++i) {
                if (value[i] != 0) return true;
            }
            return false;
        }


        public byte[] value;
    }

    public static class sig_der_t {

        public sig_der_t() {
            value = null;
        }

        public sig_der_t(byte[] bytes) {
            value = bytes;
        }

        public void resize(int sz) {
            if (sz == 0) {
                value = null;
                return;
            }
            value = new byte[sz];
        }

        public boolean is_zero() {
            for (int i = 0; i < value.length; ++i) {
                if (value[i] != 0) return false;
            }
            return true;
        }

        public boolean is_not_zero() {
            for (int i = 0; i < value.length; ++i) {
                if (value[i] != 0) return true;
            }
            return false;
        }

        public byte[] value;
    }

    public static class pub_t { //vector<uint8_t>

        public static int mem_size = 64;
        public static int ser_size = 33;

        public pub_t() {
            value = null;
        }

        public pub_t(PublicKey pk) {
            value = pk;
        }

        public PublicKey value;
    }

    public static class priv_t { //vector<uint8_t>

        public static int mem_size = 32;
        public static int ser_size = 32;

        public priv_t() {
            value = null;
        }

        public priv_t(PrivateKey pk) {
            value = pk;
        }

        public PrivateKey value;
    }
}

