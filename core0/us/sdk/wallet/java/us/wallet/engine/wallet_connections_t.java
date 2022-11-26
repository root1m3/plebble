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

@SuppressWarnings("serial")
public class wallet_connections_t extends ArrayList<wallet_connection_t> implements us.gov.io.seriable {

    private static void log(final String line) {                                 //--strip
        CFG.log_wallet_trader("wallet_connections_t: " + line);                  //--strip
    }                                                                            //--strip

    public wallet_connections_t() {
    }

    public static serid_t my_serid_id = new serid_t((short)'x');

    @Override public serid_t serial_id() { return my_serid_id; }

    @Override public int blob_size() {
        int sz = blob_writer_t.sizet_size(size());
        for (wallet_connection_t entry: this) {
            sz += blob_writer_t.blob_size(entry);
        }
        return sz;
    }

    @Override public void to_blob(blob_writer_t writer) {
        log("to_blob " + size() + " entries"); //--strip
        writer.write_sizet(size());
        for (wallet_connection_t entry: this) {
            writer.write(entry);
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
                wallet_connection_t wallet_connection = new wallet_connection_t();
                {
                    ko r = reader.read(wallet_connection);
                    if (is_ko(r)) {
                        log(r.msg); //--strip
                        return r;
                    }
                }
                log("adding wallet_connection"); //--strip
                add(wallet_connection);
            }
        }
        catch (Exception e) {
            return new ko("KO 89782 Invalid read. wallet_connection.");
        }
        return ok;
    }

}

