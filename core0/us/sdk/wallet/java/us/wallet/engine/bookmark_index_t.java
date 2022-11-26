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
package us.wallet.engine;
import java.util.ArrayList;                                                                    // ArrayList
import us.gov.crypto.base58;                                                                   // base58
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.types.blob_t;                                                                 // blob_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import us.CFG;                                                                                 // CFG
import java.util.Date;                                                                         // Date
import us.gov.crypto.ec;                                                                       // ec
import static us.gov.crypto.types.*;                                                           // *
import static us.gov.id.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import us.stdint.*;                                                                            // *
import static us.ko.is_ko;                                                                     // is_ko
import java.security.KeyPair;                                                                  // KeyPair
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import java.io.PrintStream;                                                                    // PrintStream
import static us.gov.io.types.blob_t.serid_t;                                                  // serid_t
import us.string;                                                                              // string
import java.util.TreeMap;
import java.util.Set;
import java.util.Map;
import us.wallet.trader.protocol_selection_t;                                                                // blob_writer_t
import us.wallet.trader.bookmarks_t;                                                                // blob_writer_t

@SuppressWarnings("serial")
public class bookmark_index_t extends TreeMap<protocol_selection_t, bookmarks_t> implements us.gov.io.seriable {

    private static void log(final String line) {                                 //--strip
        CFG.log_wallet_trader("bookmark_index_t: " + line);                      //--strip
    }                                                                            //--strip

    public void dump(PrintStream os) {
        for (Map.Entry<protocol_selection_t, bookmarks_t> entry: entrySet()) {
            os.println(entry.getKey().to_string2() + ":");
            entry.getValue().dump("    ", os);
        }
    }

    public static serid_t my_serid_id = new serid_t((short)'N');

    @Override public serid_t serial_id() { return my_serid_id; }

    @Override public int blob_size() {
        int sz = blob_writer_t.sizet_size(size());
        Set<Map.Entry<protocol_selection_t, bookmarks_t>> entries = entrySet();
        for (Map.Entry<protocol_selection_t, bookmarks_t> entry: entries) {
            sz += entry.getKey().blob_size();
            sz += entry.getValue().blob_size();
        }
        return sz;
    }

    @Override public void to_blob(blob_writer_t writer) {
        log("to_blob " + size() + " entries"); //--strip
        writer.write_sizet(size());
        Set<Map.Entry<protocol_selection_t, bookmarks_t>> entries = entrySet();
        for (Map.Entry<protocol_selection_t, bookmarks_t> entry: entries) {
            entry.getKey().to_blob(writer);
            entry.getValue().to_blob(writer);
        }
    }

    @Override public ko from_blob(blob_reader_t reader) {
        clear();
        log("from_blob"); //--strip
        uint64_t sz = new uint64_t();
        {
            ko r = reader.read_sizet(sz);
            if (is_ko(r)) return r;
        }
        log("loading " + sz.value + " items from blob"); //--strip
        try {
            for (long i = 0; i < sz.value; ++i) {
                log("loading item " + i); //--strip
                protocol_selection_t protocol_selection = new protocol_selection_t();
                {
                    ko r = protocol_selection.from_blob(reader);
                    if (is_ko(r)) {
                        log(r.msg); //--strip
                        return r;
                    }
                }
                bookmarks_t bookmarks = new bookmarks_t();
                {
                    ko r = bookmarks.from_blob(reader);
                    if (is_ko(r)) {
                        log(r.msg); //--strip
                        return r;
                    }
                }
                log("adding entry"); //--strip
                put(protocol_selection, bookmarks);
            }
        }
        catch (Exception e) {
            return new ko("KO 89782 Invalid read.");
        }
        return ok;
    }

}

