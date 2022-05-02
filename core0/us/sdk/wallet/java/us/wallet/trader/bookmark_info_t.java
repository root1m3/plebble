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
import java.util.ArrayList;                                                                    // ArrayList
import java.util.Arrays;                                                                       // Arrays
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import us.CFG;                                                                                 // CFG
import static us.gov.io.types.*;                                                               // *
import us.stdint.*;                                                                            // *
import java.io.InputStream;                                                                    // InputStream
import static us.ko.is_ko;                                                                     // is_ko
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import java.io.OutputStream;                                                                   // OutputStream
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import java.util.Scanner;                                                                      // Scanner
import static us.gov.io.types.blob_t.serid_t;                                                  // serid_t

public class bookmark_info_t implements us.gov.io.seriable  {

    static void log(final String line) {                    //--strip
       CFG.log_wallet_trader("bookmark_info_t: " + line);   //--strip
    }                                                       //--strip

    public bookmark_info_t() {
        label = "";
        ico = new byte[0];
    }

    public bookmark_info_t(String label_, byte[] ico_) {
        label = label_;
        ico = ico_;
    }

    public bookmark_info_t(bookmark_info_t other) {
        label = other.label;
        ico = other.ico;
    }

    @Override public serid_t serial_id() { return serid_t.no_header; }

    @Override public int blob_size() {
        return blob_writer_t.blob_size(label) + blob_writer_t.blob_size(ico);
    }

    @Override public void to_blob(blob_writer_t writer) {
        log("writing bookmark_info"); //--strip
        writer.write(label);
        writer.write(new bin_t(ico));
    }

    @Override public ko from_blob(blob_reader_t reader) {
        log("reading bookmark_info"); //--strip
        {
            pair<ko, String> r = reader.read_string();
            if (is_ko(r.first)) {
                return r.first;
            }
            label = r.second;
        }
        {
            bin_t bin = new bin_t();
            ko r = reader.read(bin);
            if (is_ko(r)) {
                return r;
            }
            ico = bin.value;
        }
        return ok;
    }

    public ko from_stream(InputStream is) {
        Scanner sc = new Scanner(is);
        return from_scanner(sc);
    }

    public ko from_scanner(Scanner sc) {
        try {
            String s = sc.next();
            if (s.equals("-")) {
                label = "";
            }
            else {
                label = us.gov.crypto.base58.decode_string(s);
            }
            s = sc.next();
            if (s.equals("-")) {
                ico = new byte[0];
            }
            else {
                ico = us.gov.crypto.base58.decode(s);
            }
        }
        catch (Exception e) {
            return new ko("KO 65902");
        }
        return ok;
    }

    public String to_string() {
        return label + " [" + (ico == null ? 0 : ico.length) + " ico bytes]";
    }

    public String label = "";
    public byte[] ico = null;
}

