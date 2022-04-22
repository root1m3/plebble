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
import java.util.ArrayList;                                                                    // ArrayList
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import java.io.ByteArrayInputStream;                                                           // ByteArrayInputStream
import java.io.ByteArrayOutputStream;                                                          // ByteArrayOutputStream
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
import static us.gov.io.types.blob_t.serid_t;                                                  // serid_t

public class bookmark_t extends bookmark_info_t implements us.gov.io.seriable {

    static void log(final String line)    {             //--strip
       CFG.log_wallet_trader("bookmark_t: " + line);    //--strip
    }                                                   //--strip

    public bookmark_t() {
        qr = new qr_t();
    }

    public bookmark_t(qr_t qr_, bookmark_info_t bookmark_info_) {
        super(bookmark_info_);
        qr = qr_;
    }

    public bookmark_t(endpoint_t endpoint) {
        qr = new qr_t(endpoint);
    }

    static serid_t serid = new serid_t((short)'b');
    @Override public serid_t serial_id() { return serid; }

    @Override public int blob_size() {
        return super.blob_size() + qr.blob_size();
    }

    @Override public void to_blob(blob_writer_t writer) {
        log("writing bookmark"); //--strip
        writer.write(qr);
        super.to_blob(writer);
    }

    @Override public ko from_blob(blob_reader_t reader) {
        log("reading bookmark"); //--strip
        {
            ko r = reader.read(qr);
            if (is_ko(r)) {
                return r;
            }
        }
        {
            ko r = super.from_blob(reader);
            if (is_ko(r)) {
                return r;
            }
        }
        return ok;
    }

    public ko from_stream(InputStream is) {
        Scanner sc = new Scanner(is);
        return from_scanner(sc);
    }

    public ko from_scanner(Scanner sc) {
        ko r = qr.from_scanner(sc);
        if (is_ko(r)) {
            return r;
        }
        return super.from_scanner(sc);
    }

    public ko from_string(String s) {
        ByteArrayInputStream is = new ByteArrayInputStream(s.getBytes());
        return from_stream(is);
    }

    public String to_string() {
        return qr.to_string() + ' ' + super.to_string();
    }

    public String get_label() {
        if (label.isEmpty()) {
            return "Unknown [" + qr.to_string().trim() + "]";
        }
        return label;
    }

    public qr_t qr;
}

