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
import java.io.BufferedInputStream;                                                            // BufferedInputStream
import java.io.BufferedReader;                                                                 // BufferedReader
import java.util.Collections;                                                                  // Collections
import java.util.Comparator;                                                                   // Comparator
import android.content.Context;                                                                // Context
import java.util.Date;                                                                         // Date
import java.io.FileNotFoundException;                                                          // FileNotFoundException
//import com.google.firebase.crashlytics.FirebaseCrashlytics;                                    // FirebaseCrashlytics
import static us.gov.id.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import java.io.InputStream;                                                                    // InputStream
import java.io.InputStreamReader;                                                              // InputStreamReader
import java.io.IOException;                                                                    // IOException
import static us.ko.is_ko;                                                                     // is_ko
import org.json.JSONArray;                                                                     // JSONArray
import org.json.JSONException;                                                                 // JSONException
import org.json.JSONObject;                                                                    // JSONObject
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import java.io.OutputStream;                                                                   // OutputStream
import java.io.OutputStreamWriter;                                                             // OutputStreamWriter
import us.pair;                                                                                // pair

public final class device_endpoints_t extends ArrayList<device_endpoint_t> /*implements hmi_t.hmi_callback_t*/ {

    static final String SETTINGS_JSON_PRIVATE_FILE = "node_settings.json";
    static final String device_endpoints_file = "device_endpoints";

    private static void log(final String line) {                         //--strip
        CFG.log_android("device_endpoints_t: " + line);                  //--strip
    }                                                                    //--strip

    private String import_old_k() {
        log("import old k?"); //--strip
        String k = cfg_android_private_t.k_filename();
        Context ctx = a.getApplicationContext();
        if (!cfg_android_private_t.file_exists2(ctx, "", k)) {
            log("old k doesn't exist."); //--strip
            return null;
        }
        String content = cfg_android_private_t.read_file_string(ctx, "", k);
        if (content == null || content.isEmpty() ) {
            log("content of old_k was unreadable."); //--strip
            return null;
        }
        cfg_android_private_t.delete_file(ctx, "", k);
        log("removed old_k."); //--strip
        return k;
    }

    public device_endpoints_t(app a_) {
        a = a_;
    }

    public int init() throws Exception {
        //TODO: purge >= alpha-39
        {
            log("import_old_settings"); //--strip
            device_endpoint_t old = import_old_settings(a.getApplicationContext());
            if (old != null) {
                log("retrieved old_setting " + old.name_); //--strip
                add(old);
                log("overwriting settings with old_setting"); //--strip
                save();
            }
        }
        home = "device_endpoints";
        pair<ko, Integer> r = load_();
        if (isEmpty()) {
            add(new device_endpoint_t(a, this, home));
            set_cur(0);
            return -1;
        }
        if (is_ko(r.first)) {
            return -1;
        }
        log("returning pwr = " + r.second.intValue()); //--strip
        return r.second.intValue();
    }

/*
    public boolean copy(int position) {
        try {
            add(new device_endpoint_t(a, home, get(position)));
        }
        catch (Exception e) {
            return false;
        }
        return true;
    }
*/

    public ko erase(int position) {
        device_endpoint_t p = get(position);
        if (p.hmi != null) {
            return new ko("KO 82711 Cannot delete an active connection.");
        }
        if (size() < 2) {
            return new ko("KO 82712 Cannot delete the only existing connection.");
        }
        remove(position);
        save();
        return ok;
    }

    public void set_cur(int position) {
        log("set_cur " + position); //--strip
        cur_index = position;
        cur = get(cur_index);
    }

    public boolean new_endpoint() {
        log("new_endpoint"); //--strip
        try {
            add(new device_endpoint_t(a, this, home));
        }
        catch (Exception e) {
            return false;
        }
        log("save"); //--strip
        save();
        return true;
    }

/*
    public String read_settings() {
        try {
            log("read_settings " + SETTINGS_JSON_PRIVATE_FILE); //--strip
            InputStream inputStream = a.getApplicationContext().openFileInput(SETTINGS_JSON_PRIVATE_FILE);
            if (inputStream == null) {
                log("inputStream is null"); //--strip
                return null;
            }
            InputStreamReader inputStreamReader = new InputStreamReader(inputStream);
            BufferedReader bufferedReader = new BufferedReader(inputStreamReader);
            String receiveString = "";
            StringBuilder stringBuilder = new StringBuilder();
            while ((receiveString = bufferedReader.readLine()) != null) {
                stringBuilder.append(receiveString);
            }
            inputStream.close();
            return stringBuilder.toString();
        }
        catch (FileNotFoundException e) {
            log("File not found: " + e.getMessage() + "    " + e.toString()); //--strip
        }
        catch (IOException e) {
            log("Exception: " + e.getMessage() + "    " + e.toString()); //--strip
            error_manager.manage(e, e.getMessage() + "    " + e.toString());
            log("Can not read file: " + e.toString()); //--strip
        }
        return null;
    }
*/
    private device_endpoint_t import_old_settings(Context ctx) {
        device_endpoint_t dep = load_prev();
        if (dep == null) {
            log("No prev setings found"); //--strip
            return null;
        }
        log("removing old_file."); //--strip
        cfg_android_private_t.delete_file(ctx, "", "node_settings.json");
        return dep;
    }

    public pair<ko, JSONObject> read_settings_json() {
        String sdata = cfg_android_private_t.read_file_string(a.getApplicationContext(), "", SETTINGS_JSON_PRIVATE_FILE);
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

    public ko write_settings_json() {
        try {
            JSONObject root = new JSONObject();
            {    
                JSONArray data = new JSONArray();
                for (int i = 0; i < size(); i++) {
                    device_endpoint_t dep = get(i);
                    endpoint_t ep = dep.endpoint;
                    JSONObject json = new JSONObject();
                    json.put("addr", dep.addr);
                    json.put("name", dep.name_);
                    json.put("ssid", dep.ssid);
                    json.put("power", dep.hmi != null ? 1 : 0);
                    json.put("k", us.gov.crypto.ec.instance.to_b58(dep.cfg.keys.getPrivate()));
                    json.put("ts", dep.ts);
                    json.put("ip", ep.shost.value);
                    json.put("port", ep.port.value);
                    json.put("channel", ep.channel.value);
                    data.put(json);
                }
                root.put("data", data);
                root.put("global", cur_index);
            }
            log("saving data: " + root.toString()); //--strip
            return cfg_android_private_t.write_file_string(a.getApplicationContext(), "", SETTINGS_JSON_PRIVATE_FILE, root.toString());
        }
        catch (JSONException e) {
            log("Exception " + e.getMessage()); //--strip
            ko r = new ko("KO 43021 Could not write settings.");
            log(r.msg); //--strip
            return r;
        }
    }

    public hmi_t poweron(app a, int pos, pin_t pin, app.progress_t progress) {
        device_endpoint_t dep = get(pos);
        dep.poweron(a, pin, progress);
        return dep.hmi;
    }

    public void poweroff(app a, int pos, app.progress_t progress) {
        device_endpoint_t dep = get(pos);
        dep.poweroff(progress);
    }

    public pair<ko, Integer> load_() {
        log("load"); //--strip
        int pwr = -1;
        try {
            pair<ko, JSONObject> root = read_settings_json();
            if (is_ko(root.first)) {
                log(root.first.msg); //--strip
                return new pair<ko, Integer>(root.first, new Integer(-1));
            }
            clear();
            JSONArray data = root.second.getJSONArray("data");
            for (int i = 0; i < data.length(); i++) {
                JSONObject o = data.getJSONObject(i);
                //IP
                //port
                //Name
                //channel
                //ts
                String k = null;
                String nm = null;
                String addr = null;
                String ssid = null;
                String x = null;
                int power = 0;
                int p = 0;
                int ch = 0;
                long ts = 0;
                k = o.getString("k");
                addr = o.getString("addr");
                x = o.getString("ip");
                p = o.getInt("port");
                nm = o.getString("name");
                power = o.getInt("power");
                ch = o.getInt("channel");
                ts = o.getLong("ts");
                ssid = o.getString("ssid");
                log("adding new device_endpoint_t " + nm); //--strip
                add(new device_endpoint_t(a, this, ts, addr, nm, ssid, k, home, new endpoint_t(new shost_t(x), new port_t(p), new channel_t(ch))));
                if (power == 1) {
                    pwr = size() - 1;
                    log("power is ON on pos " + pwr); //--strip
                }
            }
            int ndx = root.second.getInt("global");
            log("NDX=" + ndx); //--strip
            set_cur(ndx);
        }
        catch (JSONException e) {
            ko r = new ko("KO 68857 Exception");
            log(r.msg); //--strip
            return new pair<ko, Integer>(r, new Integer(-1));
        }
        catch (Exception e) {
            ko r = new ko("KO 68858 Exception");
            log(r.msg + " " + e.getMessage()); //--strip
            return new pair<ko, Integer>(r, new Integer(-1));
        }
        log("loaded - ok"); //--strip
        return new pair<ko, Integer>(ok, new Integer(pwr));
    }

    public JSONArray read_settings_json_prev() {
        String file = "node_settings.json";
        Context ctx = a.getApplicationContext();
        if (!cfg_android_private_t.file_exists2(ctx, "", file)) {
            log("no old data"); //--strip
            return null;
        }
        String sdata = cfg_android_private_t.read_file_string(a.getApplicationContext(), "", file);
        if (sdata == null) {
            ko r = new ko("KO 65784 Could not load settings");
            log(r.msg); //--strip
            return null;
        }
        log("data retrieved: " + sdata); //--strip
        JSONArray data;
        try {
            data = new JSONArray(sdata);
        }
        catch (JSONException e) {
            ko r = new ko("KO 65781 Invalid JSON data.");
            log(r.msg); //--strip
            return null;
        }
        if (data == null) {
            ko r = new ko("KO 93827 Invalid JSON data.");
            log(r.msg); //--strip
            return null;
        }
        return data;
    }

    public device_endpoint_t load_prev() {
        log("load_prev"); //--strip
        String oldk = import_old_k();
        if (oldk == null) {
            log("No old settings found"); //--strip
            return null;
        }
        JSONArray data = read_settings_json_prev();
        if (data == null) {
            return null;
        }
        clear();
        for (int i = 0; i < data.length(); i++) {
            JSONObject o;
            try {
                o = data.getJSONObject(i);
            }
            catch (JSONException e) {
                o = null;
                continue;
            }
            //IP
            //port
            //Name
            //channel
            //ts
            String k = oldk;
            String nm = null;
            String addr = "";
            String ssid = "";
            String x = null;
            int p = 0;
            int ch = 0;
            long ts = 0;
            try {
                if (!o.isNull("default")) { // old version we're intereste in recovering
                    if (o.getString("default").equals("1")) { // old version we're intereste in recovering
                        x = o.getString("lanip");
                        p = o.getInt("lanport");
                        nm = o.getString("label");
                        ch = o.getInt("channel");
                        ts = new Date().getTime();
                    }
                    else {
                        continue;
                    }
                }
                else {
                    continue;
                }
            }
            catch (JSONException e) {
                continue;
            }
            try {
                return new device_endpoint_t(a, this, ts, addr, nm, ssid, k, home, new endpoint_t(new shost_t(x), new port_t(p), new channel_t(ch)));
            }
            catch (Exception e) {
                log("Exception!"); //--strip
            }
        }
        return null;
    }

/*
    public void write_settings(String data) {
        log("writing settings " + data); //--strip
        try {
            OutputStreamWriter os = new OutputStreamWriter(a.openFileOutput(SETTINGS_JSON_PRIVATE_FILE, Context.MODE_PRIVATE));
            os.write(data);
            os.close();
        }
        catch (IOException e) {
            error_manager.manage(e, e.getMessage() + "    " + e.toString());
            log("File write failed: " + e.toString()); //--strip
        }
    }
*/
/*
    pair<ko, blob> load(blob_t blob) {
    }

    ko save(blob_t blob) {
        
        log("writing settings " + data); //--strip
        try {
            OutputStreamWriter os = new OutputStreamWriter(a.openFileOutput(device_endpoints_file, Context.MODE_PRIVATE));
            os.write(data);
            os.close();
        }
        catch (IOException e) {
            error_manager.manage(e, e.getMessage() + "    " + e.toString());
            log("File write failed: " + e.toString()); //--strip
        }
    }
*/
    public ko set_name(String name) {
        cur.name_ = name;
        return save();
    }

    public ko set_ssid(String ssid) {
        cur.ssid = ssid;
        return save();
    }

    ko save() {
        Collections.sort(this,
            new Comparator<device_endpoint_t>() {
                @Override public int compare(device_endpoint_t o1, device_endpoint_t o2) {
                    if (o1.ts < o2.ts) return 1;
                    if (o1.ts == o2.ts) return 0;
                    return -1;
                }
        });
        return write_settings_json();
//            blob_t blob;
//            save(blob);

        //assert ep0 != null;
        //endpoint_t ep0;

        //endpoint_t ep = new endpoint_t(ep0);
        //ep.channel = new channel_t(((int)(ep.channel.value)));
        //log("save endpoint to settings " + ep.to_string()); //--strip
        //JSONArray data = read_settings2();
        //if (data == null) {
        //    data = new JSONArray();
        //}
        //boolean found = false;
//        return true;
    }

    device_endpoint_t cur = null;
    int cur_index = -1;
    //cfg_android_private_t cfg;
    String home;
    app a;
}

