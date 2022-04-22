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
package us.wallet.trader.workflow;
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import us.CFG;                                                                                 // CFG
import static us.gov.io.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import static us.ko.*;                                                                         // *
import static us.stdint.*;                                                                     // *
import us.ko;                                                                                  // ko
import us.pair;                                                                                // pair
import static us.gov.io.types.blob_t.serid_t;                                                  // serid_t
import us.string;                                                                              // string

public abstract class expiry_doc_t extends doc0_t {

    static void log(final String line) {                          //--strip
       CFG.log_wallet_trader("workflow/expiry_doc_t: " + line);   //--strip
    }                                                             //--strip

    public expiry_doc_t() {
    }

    @Override public serid_t serial_id() { return serid_t.no_header; }

    @Override public int blob_size() {
        return super.blob_size() + blob_writer_t.blob_size(expiry);
    }

    @Override public void to_blob(blob_writer_t writer) {
        super.to_blob(writer);
        writer.write(expiry);
    }

    @Override public ko from_blob(blob_reader_t reader) {
        {
            ko r = super.from_blob(reader);
            if (is_ko(r)) return r;
        }
        {
            expiry = new ts_t();
            ko r = reader.read(expiry);
            if (is_ko(r)) return r;
        }
        return ok;
    }

    public ts_t expiry;

}

