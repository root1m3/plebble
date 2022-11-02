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
package us.gov.io;
import java.util.ArrayList;                                                                    // ArrayList
import us.CFG;                                                                                 // CFG
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.gov.crypto.types.*;                                                           // *
import static us.gov.socket.types.*;                                                           // *
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import us.ko;                                                                                  // ko
import us.pair;                                                                                // pair
import us.string;                                                                              // string

public class types {

    public static void log(final String line) { //--strip
        CFG.log_gov_io("types: " + line);       //--strip
    }                                           //--strip

    public static class blob_t {

        public static class version_t extends uint8_t {
            public version_t(short v) {
                value = v;
            }
        }


        public static class serid_t extends uint8_t {

            public static serid_t no_header = new serid_t((short)0);

            public serid_t(short v) {
                value = v;
            }

        }

        public blob_t() {
            value = null;
        }

        public blob_t(int sz) {
            value = new byte[sz];
        }

        public blob_t(byte[] bytes) {
            log("blob_t constructor " + (bytes != null ? bytes.length : -1)); //--strip
            value = bytes;
        }

        public blob_t(String b58) {
            log("blob_t String constructor "); //--strip
            value = us.gov.crypto.base58.decode(b58);
        }

        public blob_t(string b58) {
            log("blob_t String constructor "); //--strip
            value = us.gov.crypto.base58.decode(b58.value);
        }

        public String encode() {
            return us.gov.crypto.base58.encode(value);
        }

        public void resize(int sz) {
            value = new byte[sz];
        }

        public void clear() {
            value = new byte[0];
        }

        public int size() {
            return value.length;
        }

        public byte[] value;
    }


    public static class bin_t {

        public bin_t() {
            value = null;
        }

        public bin_t(int sz) {
            resize(sz);
        }

        public bin_t(byte[] bytes) {
            value = bytes;
        }

        public void resize(int sz) {
            if (sz == 0) {
                value = null;
                return;
            }
            value = new byte[sz];
        }

        public void clear() {
            value = null;
        }

        public int size() {
            if (value == null) {
                return 0;
            }
            return value.length;
        }

        public byte[] value;
    }

    @SuppressWarnings("serial")
    public static class vector_string extends ArrayList<String> {
    }

    @SuppressWarnings("serial")
    public static class vector_hash extends ArrayList<hash_t> {
    }

    @SuppressWarnings("serial")
    public static class vector_hash_string extends ArrayList<pair<hash_t, String>> {
    }

    @SuppressWarnings("serial")
    public static class vector_pair_string_string extends ArrayList<pair<String, String>> {
    }

    @SuppressWarnings("serial")
    public static class vector_tuple_hash_host_port extends ArrayList<tuple3<hash_t, host_t, port_t>> {
    }

}

