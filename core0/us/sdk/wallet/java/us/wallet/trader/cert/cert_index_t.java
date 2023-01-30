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
package us.wallet.trader.cert;
import java.util.ArrayList;                                                                    // ArrayList
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import java.io.ByteArrayInputStream;                                                           // ByteArrayInputStream
import us.CFG;                                                                                 // CFG
import java.io.DataInputStream;                                                                // DataInputStream
import static us.ko.*;                                                                         // *
import us.stdint.*;                                                                            // *
import java.io.InputStream;                                                                    // InputStream
import us.ko;                                                                                  // ko
import us.string;                                                                                  // ko
import java.util.Map;                                                                          // Map
import java.io.OutputStream;                                                                   // OutputStream
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import java.util.Scanner;                                                                      // Scanner
import static us.gov.io.types.blob_t.serid_t;                                                  // serid_t
import java.nio.charset.StandardCharsets;                                                      // StandardCharsets
import java.util.TreeMap;                                                                      // TreeMap
import static us.gov.crypto.ripemd160.hash_t;

@SuppressWarnings("serial")
public class cert_index_t extends TreeMap<hash_t, string> implements us.gov.io.seriable {

    static void log(final String line) {                  //--strip
       CFG.log_wallet_trader("cert_index_t: " + line);    //--strip
    }                                                     //--strip

    static serid_t serid = new serid_t((short)'C');
    @Override public serid_t serial_id() { return serid; }


    @Override public int blob_size() {
        int sz = blob_writer_t.sizet_size(size());
        for (Map.Entry<hash_t, string> entry : entrySet()) {
            sz += blob_writer_t.blob_size(entry.getKey());
            sz += blob_writer_t.blob_size(entry.getValue());
        }
        return sz;
    }

    @Override public void to_blob(blob_writer_t writer) {
        log("writing cert_index"); //--strip
        writer.write_sizet(size());
        for (Map.Entry<hash_t, string> entry : entrySet()) {
            writer.write(entry.getKey());
            writer.write(entry.getValue());
        }
    }

    @Override public ko from_blob(blob_reader_t reader) {
        log("reading cert_index"); //--strip
        uint64_t sz = new uint64_t();
        ko r = reader.read_sizet(sz);
        if (is_ko(r)) return r;
        log("reading " + sz.value + " cert entries"); //--strip
        clear();
        for (long i = 0; i < sz.value; ++i) {
            hash_t key = new hash_t();
            {
                r = reader.read(key);
                if (is_ko(r)) return r;
            }
            string value = new string();
            {
                r = reader.read(value);
                if (is_ko(r)) return r;
            }
            put(key, value);
        }
        return ok;
    }

    public void dump(String pfx, PrintStream os) {
        int n = 0;
        for (Map.Entry<hash_t, string> entry : entrySet()) {
            os.print(pfx + entry.getKey().encode() + " " + entry.getValue().value);
        }
    }

}

