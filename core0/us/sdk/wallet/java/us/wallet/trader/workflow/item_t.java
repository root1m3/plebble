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
import static us.ko.*;                                                                         // *
import static us.stdint.*;                                                                     // *
import us.ko;                                                                                  // ko
import us.pair;                                                                                // pair
import static us.gov.io.types.blob_t.serid_t;                                                  // serid_t
import us.string;                                                                              // string

public abstract class item_t implements us.gov.io.seriable {

    static void log(final String line) {                    //--strip
       CFG.log_wallet_trader("workflow/item_t: " + line);   //--strip
    }                                                       //--strip

    public abstract doc0_t create_doc();

    public pair<ko, doc0_t> doc_from_blob(blob_reader_t reader) {
        uint8_t hasdoc = new uint8_t();
        {
            ko r = reader.read(hasdoc);
            if (is_ko(r)) return new pair<ko, doc0_t>(r, null);
        }
        if (hasdoc.value == 0) {
            log("blob brought null doc"); //--strip
            return new pair<ko, doc0_t>(ok, null);
        }
        doc0_t d = create_doc();
        assert d != null;
        {
            ko r = reader.read(d);
            if (is_ko(r)) {
                return new pair<ko, doc0_t>(r, null);
            }
        }
        return new pair<ko, doc0_t>(ok, d);
    }

    @Override public serid_t serial_id() { return serid_t.no_header; }

    @Override public int blob_size() {
        int sz = blob_writer_t.blob_size(name) + 1;
        if (doc != null) sz += blob_writer_t.blob_size(doc);
        return sz;
    }

    @Override public void to_blob(blob_writer_t writer) {
        writer.write(name);
        uint8_t hasdoc = new uint8_t(doc == null ? (short)0 : (short)1);
        writer.write(hasdoc);
        if (doc != null) writer.write(doc);
    }

    @Override public ko from_blob(blob_reader_t reader) {
        {
            string n = new string();
            {
                ko r = reader.read(n);
                if (is_ko(r)) return r;
            }
            if (name.isEmpty()) {
                name = n.value;
            }
            else {
                if (!n.value.equals(name)) {
                    ko r = new ko("KO 40329 Workflow item name mismatch");
                    log(r.msg + " " + n.value + " " + name); //--strip
                    return r;
                }
            }
        }
        {
            pair<ko, doc0_t> r = doc_from_blob(reader);
            if (is_ko(r.first)) {
                assert r.second == null;
                return r.first;
            }
            assert r.second != null;
            doc = r.second;
        }
        return ok;
    }

    public String name = "";
    public doc0_t doc = null;

}

