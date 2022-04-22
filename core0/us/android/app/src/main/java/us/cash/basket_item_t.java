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
package us.cash;
import us.gov.crypto.base58;                                                                   // base58
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import java.io.BufferedReader;                                                                 // BufferedReader
import java.io.ByteArrayInputStream;                                                           // ByteArrayInputStream
import java.io.ByteArrayOutputStream;                                                          // ByteArrayOutputStream
import java.util.HashMap;                                                                      // HashMap
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.stdint.*;                                                                     // *
import us.stdint.*;                                                                            // *
import java.io.InputStream;                                                                    // InputStream
import java.io.InputStreamReader;                                                              // InputStreamReader
import static us.ko.is_ko;                                                                     // is_ko
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import java.io.OutputStream;                                                                   // OutputStream
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import java.util.Scanner;                                                                      // Scanner
import static us.gov.io.types.blob_t.serid_t;                                                  // serid_t
import us.string;                                                                              // string
import java.util.TreeMap;                                                                      // TreeMap

@SuppressWarnings("serial")
public class basket_item_t extends pair<types.cash_t, types.cash_t> implements us.gov.io.seriable {

    static void log(final String line) {            //--strip
       CFG.log_android("basket_item_t: " + line);   //--strip
    }                                               //--strip

    public basket_item_t() {
    }

    public types.cash_t sale_price_per_unit() { return first; }
    public types.cash_t reward_price_per_unit() { return second; }

    @Override public serid_t serial_id() { return serid_t.no_header; }

    @Override public int blob_size() {
        return blob_writer_t.blob_size(first) + blob_writer_t.blob_size(second);
    }

    @Override public void to_blob(blob_writer_t writer) {
        writer.write(first);
        writer.write(second);
    }

    @Override public ko from_blob(blob_reader_t reader) {
        {
            first = new types.cash_t();
            ko r = reader.read(first);
            if (is_ko(r)) return r;
        }
        {
            second = new types.cash_t();
            ko r = reader.read(second);
            if (is_ko(r)) return r;
        }
        return ok;
    }

    String title;
    String description;
}

