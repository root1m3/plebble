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
package us.wallet.trader.bootstrap;
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import us.CFG;                                                                                 // CFG
import us.stdint.*;                                                                            // *
import java.io.InputStream;                                                                    // InputStream
import static us.ko.is_ko;                                                                     // is_ko
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import java.io.OutputStream;                                                                   // OutputStream
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import java.util.Scanner;                                                                      // Scanner
import us.wallet.trader.protocol_selection_t;                                                                      // Scanner
import static us.gov.io.types.blob_t.serid_t;                                                  // serid_t
import java.util.ArrayList;

@SuppressWarnings("serial")
public class protocols_t extends ArrayList<protocol_selection_t> implements us.gov.io.seriable {

    static void log(final String line) {                //--strip
       CFG.log_wallet_trader("protocols_t: " + line);   //--strip
    }                                                   //--strip

    @Override public serid_t serial_id() { return serid_t.no_header; }

    @Override public int blob_size() {
        int sz = blob_writer_t.sizet_size(size());
        for (protocol_selection_t o: this) {
            sz += o.blob_size();
        }
        return sz;
    }

    @Override public void to_blob(blob_writer_t writer) {
        log("to_blob " + size() + " entries"); //--strip
        writer.write_sizet(size());
        for (protocol_selection_t o: this) {
            o.to_blob(writer);
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
                log("adding entry"); //--strip
                add(protocol_selection);
            }
        }
        catch (Exception e) {
            return new ko("KO 89782 Invalid read.");
        }
        return ok;
    }

}

