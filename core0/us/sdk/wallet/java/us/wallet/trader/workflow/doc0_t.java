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
import us.wallet.trader.kv;                                                                    // kv
import us.pair;                                                                                // pair
import static us.gov.io.types.blob_t.serid_t;                                                  // serid_t
import us.string;                                                                              // string

public abstract class doc0_t implements us.gov.io.seriable {

    static void log(final String line) {                    //--strip
       CFG.log_wallet_trader("workflow/doc0_t: " + line);   //--strip
    }                                                       //--strip

    public enum type_t {
        type_string,
        type_image,
        type_bookmarks,

        num_types;

        public byte code() {
            return (byte)ordinal();
        }

        public static final String[] typestr = {"string", "image", "bookmarks"};

        public String str() {
            return typestr[ordinal()];
        }

        public static type_t from_byte(byte b) {
            return type_t.values()[(int)b];
        }
    }

    public static class magic_t extends uint16_t {

        public magic_t() { super(); }
        public magic_t(int v) { super(v); }

    }

    public abstract magic_t get_magic();
    public abstract String get_name();
    public abstract String title();
    public abstract String title_es();

    @Override public serid_t serial_id() { return serid_t.no_header; }

    @Override public int blob_size() {
        return blob_writer_t.blob_size(ts) + blob_writer_t.blob_size(params) + blob_writer_t.blob_size(buf) + uint8_t.size();
    }

    @Override public void to_blob(blob_writer_t writer) {
        writer.write(ts);
        writer.write(params);
        writer.write(new bin_t(buf));
        writer.write(new uint8_t(type.code()));
    }

    @Override public ko from_blob(blob_reader_t reader) {
        {
            ts = new ts_t();
            ko r = reader.read(ts);
            if (is_ko(r)) return r;
        }
        {
            params = new kv();
            ko r = reader.read(params);
            if (is_ko(r)) return r;
        }
        {
            bin_t o = new bin_t();
            ko r = reader.read(o);
            if (is_ko(r)) return r;
            buf = o.value;
        }
        {
            uint8_t v = new uint8_t();
            ko r = reader.read(v);
            if (is_ko(r)) return r;
            type = type_t.from_byte((byte)v.value);
        }
        return ok;
    }

    public ts_t ts;
    public kv params;
    public byte[] buf;
    public type_t type = type_t.type_string;
}

