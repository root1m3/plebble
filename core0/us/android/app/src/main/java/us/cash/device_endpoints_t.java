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
package us.cash;
import java.util.ArrayList;                                                                    // ArrayList
import us.gov.crypto.base58;                                                                   // base58
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.types.blob_t;                                                                 // blob_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import java.io.BufferedInputStream;                                                            // BufferedInputStream
import java.io.BufferedReader;                                                                 // BufferedReader
import java.util.Collections;                                                                  // Collections
import java.util.Comparator;                                                                   // Comparator
import android.content.Context;                                                                // Context
import java.util.Date;                                                                         // Date
import us.gov.crypto.ec;                                                                       // ec
import java.io.FileNotFoundException;                                                          // FileNotFoundException
import static us.gov.id.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import us.stdint.*;                                                                            // *
import java.io.InputStream;                                                                    // InputStream
import java.io.InputStreamReader;                                                              // InputStreamReader
import java.io.IOException;                                                                    // IOException
import us.wallet.engine.ip4_endpoint_t;                                                        // ip4_endpoint_t
import static us.ko.is_ko;                                                                     // is_ko
import org.json.JSONArray;                                                                     // JSONArray
import org.json.JSONException;                                                                 // JSONException
import org.json.JSONObject;                                                                    // JSONObject
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import java.io.OutputStream;                                                                   // OutputStream
import java.io.OutputStreamWriter;                                                             // OutputStreamWriter
import us.pair;                                                                                // pair
import us.gov.io.types.blob_t.serid_t;                                                         // serid_t
import us.string;                                                                              // string
import static us.gov.io.types.blob_t.version_t;                                                // version_t
import us.wallet.engine.wallet_connections_t;                                                  // wallet_connections_t
import us.wallet.engine.wallet_connection_t;                                                   // wallet_connection_t

public final class device_endpoints_t extends ArrayList<device_endpoint_t> implements us.gov.io.seriable {

    static final String device_endpoints_file = "device_endpoints";

    private static void log(final String line) {                         //--strip
        CFG.log_android("device_endpoints_t: " + line);                  //--strip
    }                                                                    //--strip

    public device_endpoints_t(app a_) {
        a = a_;
    }

    int add_default_wallet_connection2() {
        if (CFG.default_wallet_connections.isEmpty()) {
            return -1;
        }
        blob_t blob = new blob_t(base58.decode(CFG.default_wallet_connections));
        if (blob.value == null) {
            log("default connection blob is null"); //--strip
            return -1;
        }
        wallet_connections_t wallet_connections = new wallet_connections_t();
        blob_reader_t reader = new blob_reader_t(blob);
        ko r = reader.read(wallet_connections);
        if (is_ko(r)) {
            log(r.msg); //--strip
            return -1;
        }
        log("adding default device_endpoints "); //--strip
        for (wallet_connection_t entry: wallet_connections) {
            add(new device_endpoint_t(this, entry));
        }
        if (isEmpty()) {
            return -1; //off if multiple templates
        }
        if (size() > 1) {
            return -1; //off if multiple templates
        }
        return 0; //connect slot 0 if only 1 template
    }    

    int add_default_wallet_connection() {
        int on = add_default_wallet_connection2();
        if (isEmpty()) {
            log("No valid default connection found."); //--strip
            add(new device_endpoint_t(this));
            on = -1;
        }
        return on;
    }
/*
    public void add_default_wallet_connection_off() {
        device_endpoint_t dd = defdep();
        if (dd == null) return;
        dd.hmi_req_on = false;
        add(dd);
    }
*/
    public int init() throws Exception {
/*
        {
            //TODO: purge >= alpha-44
            log("import_old_settings"); //--strip
            pair<ko, Integer> r = import_old_settings(a.getApplicationContext());
            if (ko.is_ok(r.first)) {
                log("retrieved old_setting " + size()); //--strip
                log("saving old_setting with new format"); //--strip
                home = "device_endpoints";
                save();
                clear();
            }
        }
*/

        home = "device_endpoints";
        log("loading configuration. sz=" + size()); //--strip
        pair<ko, Integer> r = load_();
        log("loaded configuration. sz=" + size()); //--strip
        if (isEmpty()) {
            log("load returned empty set"); //--strip
            int on = add_default_wallet_connection();
            if (on > -1) {
                set_cur(on);
            }
            else {
                set_cur(0);
            }
            return on;
        }
        if (is_ko(r.first)) {
            return -1;
        }
        log("returning pwr = " + r.second.intValue()); //--strip
        return r.second.intValue();
    }

    public ko erase(int position) {
        device_endpoint_t p = get(position);
        if (p.hmi != null) {
            ko r = new ko("KO 82711 Cannot delete an active connection.");
            log(r.msg); //--strip
            return r;
        }
        if (size() < 2) {
            ko r = new ko("KO 82712 Cannot delete the only existing connection.");
            log(r.msg); //--strip
            return r;
        }
        remove(position);
        save();
        return ok;
    }

    public void set_cur(int position) {
        log("set_cur " + position); //--strip
        cur_index.value = position;
        cur = get(cur_index.value);
    }

    public ko new_endpoint() {
        log("new_endpoint"); //--strip
        try {
            add(new device_endpoint_t(this));
        }
        catch (Exception e) {
            ko r = new ko("KO 82752.");
            log(r.msg); //--strip
            return r;
        }
        log("save"); //--strip
        save();
        return ok;
    }

    public ko copy_device_endpoint(int ndx) {
        log("copy_endpoint " + ndx); //--strip
        device_endpoint_t dep = get(ndx);
        device_endpoint_t copy = new device_endpoint_t(this, "Copy of " + dep.name_.value, "", dep.ip4_endpoint);
        add(copy);
        log("save"); //--strip
        save();
        return ok;
    }

/*
    private pair<ko, Integer> import_old_settings(Context ctx) {
        pair<ko, Integer> r = load_prev();
        if (is_ko(r.first)) {
            log("No prev setings found"); //--strip
            return r;
        }
        //log("################## I'd delete file node_settings.json"); //--strip
        cfg_android_private_t.delete_file(ctx, "", "node_settings.json");
        return r;
    }
*/

/*
    public pair<ko, JSONObject> read_settings_json() {
        log("read_settings_json"); //--strip
        String sdata = cfg_android_private_t.read_file_string(a.getApplicationContext(), "", "node_settings.json");
        if (sdata == null) {
            ko r = new ko("KO 65784 Could not load settings");
            log(r.msg); //--strip
            return new pair<ko, JSONObject>(r, null);
        }
        log("data retrieved: " + sdata); //--strip
        JSONObject root;
        try {
            root = new JSONObject(sdata);
        }
        catch (JSONException e) {
            ko r = new ko("KO 65781 Invalid JSON data.");
            log(r.msg); //--strip
            return new pair<ko, JSONObject>(r, null);
        }
        if (root == null) {
            ko r = new ko("KO 93827 Invalid JSON data.");
            log(r.msg); //--strip
            return new pair<ko, JSONObject>(r, null);
        }
        return new pair<ko, JSONObject>(ok, root);
    }
*/

    public void poweron(app a, int pos, final pin_t pin, app.progress_t progress) {
        log("poweron pos " + pos); //--strip
        set_cur(pos);
        log("============== cur.ip4_endpoint.to_string()= " + cur.ip4_endpoint.to_string()); //--strip
//        device_endpoint_t dep = get(pos);
        cur.poweron(a, pin, progress);
        save();
        //return dep.hmi;
    }

    public void poweroff(app a, int pos, boolean save_, app.progress_t progress) {
//        set_cur(pos);
        device_endpoint_t dep = get(pos);
        dep.poweroff(progress);
        assert dep.hmi == null; //--strip

//        device_endpoint_t dep = get(pos);
//        assert dep != null; //--strip
//        cur.poweroff(progress);
        if (save_) {
            save();
        }
    }

    void write(blob_t blob) {
        log("writable::write to blob"); //--strip
        serid_t serid = serial_id();
        int sz = (serid.value != 0 ? blob_writer_t.header_size() : 0) + blob_size();
        if (sz == 0) {
            blob.clear();
            return;
        }
        blob_writer_t w = new blob_writer_t(blob, sz);
        if (serid.value != 0) {
            w.write_header(serid);
        }
        to_blob(w);
        assert w.cur == blob.value.length;
    }

    ko save() {
        Collections.sort(this,
            new Comparator<device_endpoint_t>() {
                @Override public int compare(device_endpoint_t o1, device_endpoint_t o2) {
                    if (o1.ts.value < o2.ts.value) return 1;
                    if (o1.ts.value == o2.ts.value) return 0;
                    return -1;
                }
        });
        int newindex = -1;
        for (device_endpoint_t d: this) {
            ++newindex;
            if (d == cur) {
                break;
            }
        }
        if (newindex >= size()) { //--strip
            log("KO 78688 Unexpected. newindex " + newindex + " sz " + size()); //--strip
        }  //--strip
        assert newindex < size(); //--strip
        cur_index.value = newindex;
        blob_t blob = new blob_t();
        write(blob);
        return cfg_android_private_t.write_file(a.getApplicationContext(), "", device_endpoints_file, blob.value);
    }

    public ko read(final blob_t blob) {
        log("readable::read from blob " + blob.size());  //--strip
        blob_reader_t reader = new blob_reader_t(blob);
        serid_t serid = serial_id();
        if (serid.value != 0) {
            ko r = reader.read_header(serid);
            if (ko.is_ko(r)) {
                return r;
            }
        }
        return reader.read(this);
    }

    public pair<ko, Integer> load_() {
        log("load_"); //--strip
        blob_t blob = new blob_t();
        int pwr = -1;
        blob.value = cfg_android_private_t.read_file(a.getApplicationContext(), "", device_endpoints_file);
        if (blob.value == null) {
            ko r = new ko("KO 65714 Could not load settings");
            log(r.msg); //--strip
            return new pair<ko, Integer>(r, null);
        }
        ko r = read(blob);
        if (is_ko(r)) {
            log("check if it's prev version"); //--strip
            blob_reader_t.blob_header_t header = new blob_reader_t.blob_header_t(new version_t((short)(blob_reader_t.current_version.value - 1)), new serid_t((short)'X'));
            blob_t blob_a49 = blob_writer_t.add_header(header, blob);
            r = read(blob_a49);
            if (is_ko(r)) {
                log("It wasn't"); //--strip
                return new pair<ko, Integer>(r, null);
            }
            log("It was prev version"); //--strip
        }
        log("" + size() + " items."); //--strip
        int p = -1;
        for (device_endpoint_t i: this) {
            ++p;
            if (i.hmi_req_on) {
                pwr = p;
                i.hmi_req_on = false;
                break;
            }
        }
        return new pair<ko, Integer>(ok, new Integer(pwr));
    }

    public ko set_name(String name) {
        cur.name_.value = name;
        return save();
    }

    public ko set_ssid(String ssid) {
        cur.ssid.value = ssid;
        return save();
    }

    public static serid_t my_serid_id = new serid_t((short)'X');

    @Override public serid_t serial_id() { return my_serid_id; }

    @Override public int blob_size() {
        int sz = blob_writer_t.sizet_size(size());
        for (device_endpoint_t entry: this) {
            sz += blob_writer_t.blob_size(entry);
        }
        sz += blob_writer_t.blob_size(cur_index);
        return sz;
    }

    @Override public void to_blob(blob_writer_t writer) {
        log("to_blob " + size() + " entries"); //--strip
        writer.write_sizet(size());
        for (device_endpoint_t entry: this) {
            writer.write(entry);
        }
        writer.write(cur_index);
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
                device_endpoint_t device_endpoint = new device_endpoint_t(this, 1);
                {
                    ko r = reader.read(device_endpoint);
                    if (is_ko(r)) {
                        log(r.msg); //--strip
                        return r;
                    }
                }
                log("adding device_endpoint " + device_endpoint.cfg.home); //--strip
                add(device_endpoint);
            }
        }
        catch (Exception e) {
            return new ko("KO 89782 Invalid read. device_endpoint.");
        }
        log("num items " + size()); //--strip
        {
            ko r = reader.read(cur_index);
            if (is_ko(r)) return r;
        }
        log("cur_index " + cur_index.value); //--strip
        set_cur(cur_index.value);
        return ok;
    }

    device_endpoint_t cur = null;
    int16_t cur_index = new int16_t(-1);
    String home;
    app a;
}

