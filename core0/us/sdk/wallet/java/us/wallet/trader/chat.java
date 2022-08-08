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
import us.gov.crypto.base58;                                                                   // base58
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import us.CFG;                                                                                 // CFG
import us.gov.socket.datagram;                                                                 // datagram
import java.util.Date;                                                                         // Date
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.gov.socket.types.*;                                                           // *
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import static us.ko.is_ko;                                                                     // is_ko
import us.ko;                                                                                  // ko
import java.util.Map;                                                                          // Map
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import us.wallet.protocol;                                                                     // protocol
import java.util.concurrent.locks.ReentrantLock;                                               // ReentrantLock
import static us.gov.io.types.blob_t.serid_t;                                                  // serid_t
import java.util.Timer;                                                                        // Timer
import java.util.TimerTask;                                                                    // TimerTask
import java.util.concurrent.TimeUnit;                                                          // TimeUnit
import java.util.TreeMap;                                                                      // TreeMap

public final class chat {

    private static void log(final String line) {                //--strip
        CFG.log_wallet_trader("chat: " + line);                 //--strip
    }                                                           //--strip

    public static class paragraphs extends ArrayList<String> implements us.gov.io.seriable {

        private static final long serialVersionUID = 1234324L;

        public paragraphs() {}

        public String to_string() {
            StringBuilder sb = new StringBuilder(blob_size());
            for (String entry : this) {
                sb.append(entry);
                sb.append("\n");
            }
            return sb.toString();
        }

        @Override public serid_t serial_id() { return serid_t.no_header; }

        @Override public int blob_size() {
            int sz = blob_writer_t.sizet_size(size());
            for (String entry : this) {
                sz += blob_writer_t.blob_size(entry);
            }
            return sz;
        }

        @Override public void to_blob(blob_writer_t writer) {
            writer.write_sizet(size());
            for (String entry : this) {
                writer.write(entry);
            }
        }

        @Override public ko from_blob(blob_reader_t reader) {
            log("from_blob"); //--strip
            uint64_t sz = new uint64_t();
            ko r = reader.read_sizet(sz);
            if (is_ko(r)) return r;
            clear();
            log("from_blob " + sz.value + " entries"); //--strip
            for (long i = 0; i < sz.value; ++i) {
                String s;
                {
                    pair<ko, String> rs = reader.read_string();
                    if (is_ko(rs.first)) return rs.first;
                    s = rs.second;
                    log("READ-chat-from-blob " + s); //--strip
                }
                add(s);
            }
            return ok;
        }
    }

    public static class chat_entry extends paragraphs implements us.gov.io.seriable {

        private static final long serialVersionUID = 1234325L;

        public chat_entry() {
        }

        public chat_entry(final String p) {
            add(p);
        }

        public chat_entry(final paragraphs p) {
            for (String entry: p) {
                add(entry);
            }
        }

        public void add(final paragraphs p) {
            for (String entry: p) {
                add(entry);
            }
        }

        public boolean add(final String p) {
            return super.add(p);
        }

        public void dump(final String pfx, PrintStream os) {
            if (me) os.print("me"); else os.print("peer");
            os.println(":");
            if (me) {
                for (String i: this) {
                    os.println(pfx + i);
                }
            }
            else {
                String indent = pfx + "                   ";
                for (String i: this) {
                    os.print(indent + i);
                }
            }
        }

        @Override public serid_t serial_id() { return serid_t.no_header; }

        @Override public int blob_size() {
            if (isEmpty()) return super.blob_size();
            return blob_writer_t.blob_size(me) + super.blob_size();
        }

        @Override public void to_blob(blob_writer_t writer) {
            super.to_blob(writer);
            if (isEmpty()) {
                return;
            }
            writer.write(me);
        }

        @Override public ko from_blob(blob_reader_t reader) {
            {
                ko r = super.from_blob(reader);
                if (is_ko(r)) return r;
            }
            if (isEmpty()) return ok;
            {

                pair<ko, Boolean> r = reader.read_bool();
                if (is_ko(r.first)) return r.first;
                me = r.second.booleanValue();
            }
            return ok;
        }

        public boolean me = true;
    }

    @SuppressWarnings("serial")
    public static final class chat_t extends TreeMap<ts_t, chat_entry> implements us.gov.io.seriable {

        public chat_t() {
        }

        @Override public serid_t serial_id() { return serid_t.no_header; }

        @Override public int blob_size() {
            int sz = blob_writer_t.sizet_size(size());
            for (Map.Entry<ts_t, chat_entry> entry : entrySet()) {
                sz += ts_t.size();
                sz += entry.getValue().blob_size();
            }
            return sz;
        }

        @Override public void to_blob(blob_writer_t writer) {
            writer.write_sizet(size());
            for (Map.Entry<ts_t, chat_entry> entry : entrySet()) {
                writer.write(entry.getKey());
                entry.getValue().to_blob(writer);
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
                ts_t ts = new ts_t();
                {
                    ko r = reader.read(ts);
                    if (is_ko(r)) return r;
                }
                chat_entry e = new chat_entry();
                {
                    ko r = e.from_blob(reader);
                    if (is_ko(r)) return r;
                }
                put(ts, e);
            }
            return ok;
        }

        public String formatts(ts_t ts) {
            String s = "" + ts.value;
            return s;
        }

        public void dump(String pfx, PrintStream os) {
            String indent = pfx + "  ";
            for (Map.Entry<ts_t, chat_entry> entry : entrySet()) {
                os.print(pfx + formatts(entry.getKey()) + " ");
                entry.getValue().dump(indent, os);
                os.println();
            }
        }

    }

}

