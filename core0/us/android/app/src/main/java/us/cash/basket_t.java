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
package us.cash;
import us.gov.crypto.base58;                                                                   // base58
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import java.io.BufferedReader;                                                                 // BufferedReader
import java.io.ByteArrayInputStream;                                                           // ByteArrayInputStream
import java.io.ByteArrayOutputStream;                                                          // ByteArrayOutputStream
import us.wallet.trader.data_t;                                                                // data_t
import java.util.HashMap;                                                                      // HashMap
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.gov.crypto.types.*;                                                           // *
import static us.stdint.*;                                                                     // *
import us.stdint.*;                                                                            // *
import java.io.InputStream;                                                                    // InputStream
import java.io.InputStreamReader;                                                              // InputStreamReader
import static us.ko.is_ko;                                                                     // is_ko
import us.ko;                                                                                  // ko
import java.util.Map;                                                                          // Map
import static us.ko.ok;                                                                        // ok
import java.io.OutputStream;                                                                   // OutputStream
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import java.util.Scanner;                                                                      // Scanner
import static us.gov.io.types.blob_t.serid_t;                                                  // serid_t
import us.string;                                                                              // string
import java.util.TreeMap;                                                                      // TreeMap

@SuppressWarnings("serial")
public class basket_t extends HashMap<hash_t, pair<types.vol_t, basket_item_t>> implements us.gov.io.seriable {

    public static class serial_t extends uint64_t {

        public serial_t() { super(); }
        public serial_t(long v) { super(v); }

    }

    static void log(final String line) {         //--strip
       CFG.log_android("basket_t: " + line);     //--strip
    }                                            //--strip

    public basket_t() {
    }

    public ko from(cat_t cat) {
        clear();
        for (Map.Entry<hash_t, basket_item_t> entry: cat.entrySet()) {
            put(entry.getKey(), new pair<types.vol_t, basket_item_t>(new types.vol_t(1), entry.getValue()));
        }
        return ok;
    }

    public pair<types.cash_t, types.cash_t> value() {
        pair<types.cash_t, types.cash_t> sum = new pair<types.cash_t, types.cash_t>(new types.cash_t(0), new types.cash_t(0));
        for (Map.Entry<hash_t, pair<types.vol_t, basket_item_t>> i: entrySet()) {
            sum.first.value += i.getValue().first.value * i.getValue().second.first.value;
            sum.second.value += i.getValue().first.value * i.getValue().second.second.value;
        }
        return sum;
    }

    public pair<types.vol_t, basket_item_t> find(hash_t key) {
        return get(key);
    }

    public void emplace(hash_t k, pair<types.vol_t, basket_item_t> v) {
        put(k, v);
    }

    @Override public serid_t serial_id() { return serid_t.no_header; }

    @Override public int blob_size() {
        int sz = blob_writer_t.blob_size(name) + blob_writer_t.blob_size(serial);
        sz += blob_writer_t.sizet_size(size());
        for (Map.Entry<hash_t, pair<types.vol_t, basket_item_t>> entry: entrySet()) {
            sz += blob_writer_t.blob_size(entry.getKey());
            pair<types.vol_t, basket_item_t> v = entry.getValue();
            sz += blob_writer_t.blob_size(v.first);
            sz += blob_writer_t.blob_size(v.second);
        }
        return sz;
    }

    @Override public void to_blob(blob_writer_t writer) {
        writer.write(new string(name));
        writer.write(serial);
        writer.write_sizet(size());
        for (Map.Entry<hash_t, pair<types.vol_t, basket_item_t>> entry : entrySet()) {
            writer.write(entry.getKey());
            pair<types.vol_t, basket_item_t> v = entry.getValue();
            writer.write(v.first);
            writer.write(v.second);
        }
    }

    @Override public ko from_blob(blob_reader_t reader) {
        {
            string s = new string();
            ko r = reader.read(s);
            if (is_ko(r)) return r;
            name = s.value;
        }
        {
            ko r = reader.read(serial);
            if (is_ko(r)) return r;
        }
        uint64_t sz = new uint64_t();
        {
            ko r = reader.read_sizet(sz);
            if (is_ko(r)) return r;
        }
        clear();
        for (long i = 0; i < sz.value; ++i) {
            hash_t key = new hash_t();
            {
                ko r = reader.read(key);
                if (is_ko(r)) return r;
            }
            pair<types.vol_t, basket_item_t> value = new pair<types.vol_t, basket_item_t>(new types.vol_t(), new basket_item_t());
            {
                {
                    ko r = reader.read(value.first);
                    if (is_ko(r)) return r;
                }
                {
                    ko r = reader.read(value.second);
                    if (is_ko(r)) return r;
                }
            }
            value.second.title = "Product id: " + base58.encode(key.value);
            value.second.description = "Description not available.";
            put(key, value);
        }
        return ok;
    }

    String name;
    serial_t serial = new serial_t(0);
}

