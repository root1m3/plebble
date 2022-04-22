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
package us.wallet.trader;
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import us.CFG;                                                                                 // CFG
import static us.gov.io.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import static us.ko.*;                                                                         // *
import static us.stdint.*;                                                                     // *
import us.ko;                                                                                  // ko
import java.util.Map;                                                                          // Map
import us.pair;                                                                                // pair
import static us.gov.io.types.blob_t.serid_t;                                                  // serid_t
import us.string;                                                                              // string
import java.util.TreeMap;                                                                      // TreeMap

@SuppressWarnings("serial")
public final class kv extends TreeMap<String, String> implements us.gov.io.seriable {

    static void log(final String line) {         //--strip
       CFG.log_wallet_trader("kv: " + line);     //--strip
    }                                            //--strip

    public kv() {
    }

    @Override public serid_t serial_id() { return serid_t.no_header; }

    @Override public int blob_size() {
        int sz = blob_writer_t.sizet_size(size());
        for (Map.Entry<String, String> entry: entrySet()) {
            sz += blob_writer_t.blob_size(entry.getKey());
            sz += blob_writer_t.blob_size(entry.getValue());
        }
        return sz;
    }

    @Override public void to_blob(blob_writer_t writer) {
        writer.write_sizet(size());
        for (Map.Entry<String, String> entry: entrySet()) {
            writer.write(entry.getKey());
            writer.write(entry.getValue());
        }
    }

    @Override public ko from_blob(blob_reader_t reader) {
        uint64_t sz = new uint64_t();
        {
            ko r = reader.read_sizet(sz);
            if (is_ko(r)) return r;
        }
        clear();
        for (long i = 0; i < sz.value; ++i) {
            String key;
            {
                pair<ko, String> r = reader.read_string();
                if (is_ko(r.first)) return r.first;
                key = r.second;
            }
            String value;
            {
                pair<ko, String> r = reader.read_string();
                if (is_ko(r.first)) return r.first;
                value = r.second;
            }
            put(key, value);
        }
        return ok;
    }

}

