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
package us.wallet.trader;
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import us.CFG;                                                                                 // CFG
import static us.gov.io.types.*;                                                               // *
import static us.ko.is_ko;                                                                     // is_ko
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import us.gov.io.seriable;                                                                     // seriable
import static us.gov.io.types.blob_t.serid_t;                                                  // serid_t
import static us.gov.io.types.vector_pair_string_string;                                       // vector_pair_string_string

@SuppressWarnings("serial")
public class roles_t extends vector_pair_string_string implements us.gov.io.seriable {

    static void log(final String line) {    //--strip
       CFG.log_wallet_trader("roles_t " + line);                //--strip
    }                                       //--strip

    @Override public serid_t serial_id() { return serid_t.no_header; }

    @Override public int blob_size() {
        return blob_writer_t.blob_size((vector_pair_string_string)this);
    }

    @Override public void to_blob(blob_writer_t writer) {
        writer.write((vector_pair_string_string)this);
    }

    public String[] as_options() {
        String[] o = new String[size()];
        int n = 0;
        for (pair<String, String> i: this) {
            o[n++] = i.first + " " + i.second;
        }
        return o;
    }

    @Override
    public ko from_blob(blob_reader_t reader) {
        {
            ko r = reader.read((vector_pair_string_string)this);
            if (is_ko(r)) {
                return r;
            }
        }
        return ok;
    }

    public void dump(final String pfx, PrintStream os) {
        for (pair<String, String> i: this) {
            os.println(pfx + i.first + " " + i.second);
        }
    }

//    mutable mutex mx;
}

