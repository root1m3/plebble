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
import android.app.Application;                                                                // Application
import java.util.ArrayList;                                                                    // ArrayList
import java.util.Arrays;                                                                       // Arrays
import android.media.AudioManager;                                                             // AudioManager
import java.net.Authenticator;                                                                 // Authenticator
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import static us.gov.io.types.blob_t;                                                          // blob_t
import java.io.BufferedInputStream;                                                            // BufferedInputStream
import java.io.BufferedReader;                                                                 // BufferedReader
import us.gov.socket.busyled_t;                                                                // busyled_t
import java.io.ByteArrayOutputStream;                                                          // ByteArrayOutputStream
import java.nio.charset.Charset;                                                               // Charset
import java.util.Collections;                                                                  // Collections
import android.graphics.drawable.ColorDrawable;                                                // ColorDrawable
import java.util.Comparator;                                                                   // Comparator
import java.util.concurrent.locks.Condition;                                                   // Condition
import android.content.res.Configuration;                                                      // Configuration
import android.net.ConnectivityManager;                                                        // ConnectivityManager
import android.content.Context;                                                                // Context
import us.gov.socket.datagram;                                                                 // datagram
import java.util.Date;                                                                         // Date
import android.app.DialogFragment;                                                             // DialogFragment
import android.util.DisplayMetrics;                                                            // DisplayMetrics
import android.os.Environment;                                                                 // Environment
import java.io.File;                                                                           // File
import java.io.FileNotFoundException;                                                          // FileNotFoundException
import java.io.FileOutputStream;                                                               // FileOutputStream
import androidx.core.content.FileProvider;                                                     // FileProvider
import java.security.GeneralSecurityException;                                                 // GeneralSecurityException
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import java.net.HttpURLConnection;                                                             // HttpURLConnection
import static us.gov.id.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import java.io.InputStream;                                                                    // InputStream
import java.io.InputStreamReader;                                                              // InputStreamReader
import android.content.Intent;                                                                 // Intent
import java.io.IOException;                                                                    // IOException
import static us.ko.is_ko;                                                                     // is_ko
import org.json.JSONArray;                                                                     // JSONArray
import org.json.JSONException;                                                                 // JSONException
import org.json.JSONObject;                                                                    // JSONObject
import java.security.KeyPair;                                                                  // KeyPair
import us.ko;                                                                                  // ko
import java.util.Locale;                                                                       // Locale
import java.util.concurrent.locks.Lock;                                                        // Lock
import android.util.Log;                                                                       // Log
import android.os.Looper;                                                                      // Looper
import android.view.Menu;                                                                      // Menu
import android.view.MenuItem;                                                                  // MenuItem
import android.net.NetworkInfo;                                                                // NetworkInfo
import static us.ko.ok;                                                                        // ok
import java.io.OutputStream;                                                                   // OutputStream
import java.io.OutputStreamWriter;                                                             // OutputStreamWriter
import us.pair;                                                                                // pair
import java.net.PasswordAuthentication;                                                        // PasswordAuthentication
import android.preference.PreferenceManager;                                                   // PreferenceManager
import java.io.PrintStream;                                                                    // PrintStream
import us.wallet.protocol;                                                                     // protocol
import us.wallet.trader.qr_t;                                                                  // qr_t
import java.util.Random;                                                                       // Random
import java.util.concurrent.locks.ReentrantLock;                                               // ReentrantLock
import android.content.res.Resources;                                                          // Resources
import android.content.SharedPreferences;                                                      // SharedPreferences
import android.os.StrictMode;                                                                  // StrictMode
import us.string;                                                                              // string
import android.net.wifi.SupplicantState;                                                       // SupplicantState
import android.telephony.TelephonyManager;                                                     // TelephonyManager
import java.util.Timer;                                                                        // Timer
import java.util.TimerTask;                                                                    // TimerTask
import android.widget.Toast;                                                                   // Toast
import android.media.ToneGenerator;                                                            // ToneGenerator
import java.util.TreeMap;                                                                      // TreeMap
import java.io.UnsupportedEncodingException;                                                   // UnsupportedEncodingException
import java.net.URL;                                                                           // URL
import android.net.wifi.WifiInfo;                                                              // WifiInfo
import android.net.wifi.WifiManager;                                                           // WifiManager

public final class app extends Application implements datagram_dispatcher_t.handler_t {

    private static void log(final String line) {          //--strip
        CFG.log_android("app: " + line);                  //--strip
    }                                                     //--strip

    public static void assert_ui_thread() {                                 //--strip
        if (!Looper.getMainLooper().isCurrentThread()) {                    //--strip
            log(Arrays.toString(Thread.currentThread().getStackTrace()));   //--strip
            log("ASSERT UI THREAD FAILED");                                 //--strip
            throw new AssertionError("not UI Thread");                      //--strip
        }                                                                   //--strip
    }                                                                       //--strip

    public static void assert_worker_thread() {                             //--strip
        if (Looper.getMainLooper().isCurrentThread()) {                     //--strip
            log(Arrays.toString(Thread.currentThread().getStackTrace()));   //--strip
            log("ASSERT WORKER THREAD FAILED");                             //--strip
            throw new AssertionError("UI Thread");                          //--strip
        }                                                                   //--strip
    }                                                                       //--strip

    public interface progress_t {
        void on_progress(final String line);
    }

    @Override protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
        error_manager.setup(base, this);
    }

    @Override public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
    }

    public app() {
        assert_ui_thread();                                                 //--strip
        log("----------------------APP CONSTRUCTOR--------------------");   //--strip
        log("Stack Trace at begining");                                     //--strip
        log(Arrays.toString(Thread.currentThread().getStackTrace()));       //--strip
CFG.sdk_logs = true; //--strip
        log("SDK logs: " + CFG.sdk_logs);                                   //--strip
        us.CFG.logs.set(CFG.sdk_logs);                                      //--strip

    }

    void abort(String reason) {
        log("ABORT"); //--strip
        //error_manager.manage(new GeneralSecurityException(reason), "KO 50493");
        android.os.Process.killProcess(android.os.Process.myPid());
        //finishAndRemoveTask();

        System.exit(0);
    }

    public static class walletd_status_handler_t implements hmi_t.status_handler_t {

        public walletd_status_handler_t(app a_) {
            a = a_;
        }

        @Override public void on_status(ColorDrawable led_status, final String msg) {
            log("walletd_status_handler_t: on_status. " + msg); //--strip
            a.set_walletd_led(2, led_status);
        }

        app a;
    }

    static interface i18n_t {
        enum sid {
            unknown,
            app, pres, ehr, aireq, aires, send, new_, cat, inv, pay, rcpt
        }
        String resolve(sid s);
    }

    static class i18n_en implements i18n_t {
        @Override
        public String resolve(sid s) {
            switch(s) {
                case app: return "Appointment";
                case pres: return "Prescription";
                case ehr: return "Electronic Health Record (EHR)";
                case aireq: return "A.I. Service referral";
                case aires: return "A.I. Service results";
                case send: return "Send";
                case new_: return "New";
                case cat: return "Catalogue";
                case inv: return "Invoice";
                case pay: return "Payment";
                case rcpt: return "Receipt";
            }
            return "unknown";
        }
    };

    static class i18n_es implements i18n_t {
        @Override
        public String resolve(sid s) {
            switch(s) {
                case app: return "Cita";
                case pres: return "Receta";
                case ehr: return "Historia medica (EHR)";
                case aireq: return "Volante servicio I.A";
                case aires: return "Resultados servicio I.A";
                case send: return "Enviar";
                case new_: return "Nuevo";
                case cat: return "Catalogo";
                case inv: return "Factura";
                case pay: return "Pago";
                case rcpt: return "Recibo";
            }
            return "desconocido";
        }
    };

    static i18n_t.sid i18n_sid(String s) {
        if (s.equals("app")) return i18n_t.sid.app;
        if (s.equals("pres")) return i18n_t.sid.pres;
        if (s.equals("ehr")) return i18n_t.sid.ehr;
        if (s.equals("aireq")) return i18n_t.sid.aireq;
        if (s.equals("aires")) return i18n_t.sid.aires;
        if (s.equals("send")) return i18n_t.sid.send;
        if (s.equals("new")) return i18n_t.sid.new_;
        if (s.equals("cat")) return i18n_t.sid.cat;
        if (s.equals("inv")) return i18n_t.sid.inv;
        if (s.equals("pay")) return i18n_t.sid.pay;
        if (s.equals("rcpt")) return i18n_t.sid.rcpt;
        return i18n_t.sid.unknown;
    }

    @Override public void onCreate() {
        super.onCreate();
        log("onCreate"); //--strip
        activity.a = (app) this;
        log("Init crypto"); //--strip
        us.gov.crypto.ec.create_instance();
        locale.set(new Locale(locale.get_lang(this), locale.get_country(this)));
        locale.update(this, getResources().getConfiguration());
        String lang = locale.get_lang(this);
        if (lang != null && lang.equals("es")) {
            i18n = new i18n_es();
        }
        else {
            i18n = new i18n_en();
        }
        StrictMode.VmPolicy.Builder builder = new StrictMode.VmPolicy.Builder();
        StrictMode.setVmPolicy(builder.build());
        
        log("boot test"); //--strip
        tests(); //--strip

        tone = new ToneGenerator(AudioManager.STREAM_MUSIC, 100);

        hmi = null;
        int pwr = create_device_endpoints();
        if (a.device_endpoints == null) {
            log("KO 55061"); //--strip
            abort("device_endpoints failure");
            return;
        }
        log("pwr = " + pwr); //--strip
        if (pwr != -1) {
            log("powering ON pos " + pwr); //--strip
            HMI_power_on(pwr, new pin_t(0), null);
        }
        else {
            hmi = a.device_endpoints.cur.hmi;
        }
    }

    void test_hmi() {
        if (!hmi_t.test()) {
            abort("failed hmi tests");
            return;
        }
        log("hmi test succeed"); //--strip
    }

    void test_device_endpoint() {
        device_endpoints_t x = new device_endpoints_t(this);

        blob_t blob = new blob_t(us.gov.crypto.base58.decode("3TpsNSjGp84MBxXgTBpnvffsZSnQaaTJF7qGcrD3i2Xstf4Py4LsiVW6ng27n8vWSBG1eG1UXSg5Dpo1ezfxUYaZm1DaLsFazVveUDxgx7fdoATTuWkB8fA"));
        if (blob.value == null) {
            log("default connection blob is null"); //--strip
            assert false;
        }
        us.wallet.trader.wallet_connection_t wallet_connection = new us.wallet.trader.wallet_connection_t();
        blob_reader_t reader = new blob_reader_t(blob);
        ko r = reader.read(wallet_connection);
        if (is_ko(r)) {
            log(r.msg); //--strip
            assert false;
        }
    }

    void tests() {                                  //--strip
        test_hmi();                                 //--strip
        test_device_endpoint();                     //--strip
        log("boot test succeed");                   //--strip
    }                                               //--strip

    public int create_device_endpoints() { // returns the index to poweron, -1 for none
        log("create_device_endpoints"); //--strip
        if (device_endpoints != null) {
            log("Already created"); //--strip
            return -1;
        }
        try {
            device_endpoints = new device_endpoints_t(this);
            return device_endpoints.init();
        }
        catch (Exception e) {
            log("could not create device_endpoints." + e.getMessage()); //--strip
            device_endpoints = null;
        }
        return -1;
    }

    public void set_foreground_activity(activity ac, boolean up) {
        log("set_foreground_activity " + up); //--strip
        lock_active_ac.lock();
        try {
            if (up) {
                active_ac = ac;
            }
            else {
                if (active_ac == ac) {
                    active_ac = null;
                }
            }
        }
        finally {
            lock_active_ac.unlock();
        }
    }

    void set_led(int daemon, int i, ColorDrawable color) { //0 gov 1 wallet
        assert_worker_thread(); //--strip
        if (daemon == 0) set_walletd_led(i, color);
        else set_govd_led(i, color);
    }

    void set_walletd_led(int i, ColorDrawable color) {
        assert_worker_thread(); //--strip
        log("set_walletd_led " + i + " " + color); //--strip
        lock_active_ac.lock();
        try {
            if (active_ac == null) {
                log("active_ac is null"); //--strip
                return;
            }
            if (active_ac.walletd_leds == null) {
                log("active_ac.walletd_leds is null"); //--strip
                return;
            }
            active_ac.walletd_leds.set_led(i, color);
        }
        finally {
            lock_active_ac.unlock();
        }
    }

    void set_walletd_leds_off() {
        assert_worker_thread(); //--strip
        log("set_walletd_leds off"); //--strip
        lock_active_ac.lock();
        try {
            if (active_ac == null) {
                log("active_ac is null"); //--strip
                return;
            }
            if (active_ac.walletd_leds == null) {
                log("active_ac.walletd_leds is null"); //--strip
                return;
            }
            active_ac.walletd_leds.set_all_off();
        }
        finally {
            lock_active_ac.unlock();
        }
    }

    void set_govd_led(int i, ColorDrawable color) {
        assert_worker_thread(); //--strip
        log("set_govd_led "); //--strip
        lock_active_ac.lock();
        try {
            if (active_ac == null) {
                log("active_ac is null"); //--strip
                return;
            }
            if (active_ac.walletd_leds == null) {
                log("active_ac.walletd_leds is null"); //--strip
                return;
            }
            if (active_ac.govd_leds != null) {
                active_ac.govd_leds.set_led(i, color);
            }
        }
        finally {
            lock_active_ac.unlock();
        }
    }

    void set_govd_leds_off() {
        assert_worker_thread(); //--strip
        log("set_govd_leds off"); //--strip
        lock_active_ac.lock();
        try {
            if (active_ac == null) {
                log("active_ac is null"); //--strip
                return;
            }
            if (active_ac.walletd_leds == null) {
                log("active_ac.walletd_leds is null"); //--strip
                return;
            }
            if (active_ac.govd_leds != null) {
                active_ac.govd_leds.set_all_off();
            }
        }
        finally {
            lock_active_ac.unlock();
        }
    }

    static class walletd_led_handler implements busyled_t.handler_t {

        walletd_led_handler(app a_, int i_) {
            a = a_;
            i = i_;
        }

        @Override public void on_busy() {
            a.set_walletd_led(i, leds_t.led_green);
        }

        @Override public void on_idle() {
            a.set_walletd_led(i, leds_t.led_off);
        }

        int i;
        app a;
    }

    static class govd_led_handler implements busyled_t.handler_t {

        govd_led_handler(app a_, int i_) {
            a = a_;
            i = i_;
        }

        @Override public void on_busy() {
            a.set_govd_led(i, leds_t.led_green);
        }

        @Override public void on_idle() {
            a.set_govd_led(i, leds_t.led_off);
        }

        int i;
        app a;
    }

    int rnd(int min, int max) {
        return new Random().nextInt(1 + min + (max - min));
    }

    public void led_test__worker(int d) {
        assert_worker_thread(); //--strip
        log("led_test__worker " + d); //--strip
        boolean prev;
        lock_active_ac.lock();
        try {
            if (active_ac == null) return;
            if (d == 0) {
                prev = active_ac.leds_visible_wallet;
                active_ac.set_leds_visibility_wallet(true);
            }
            else {
                prev = active_ac.leds_visible_gov;
                active_ac.set_leds_visibility_gov(true);
            }
        }
        finally {
            lock_active_ac.unlock();
        }
        try {
            set_led(d, 0, leds_t.led_blue);
            set_led(d, 1, leds_t.led_blue);
            set_led(d, 2, leds_t.led_blue);
            Thread.sleep(rnd(2000, 2500));
            set_led(d, 0, leds_t.led_red);
            set_led(d, 1, leds_t.led_red);
            set_led(d, 2, leds_t.led_red);
            Thread.sleep(2000);
            set_led(d, 0, leds_t.led_amber);
            set_led(d, 1, leds_t.led_amber);
            set_led(d, 2, leds_t.led_amber);
            Thread.sleep(2000);
            set_led(d, 0, leds_t.led_green);
            set_led(d, 1, leds_t.led_green);
            set_led(d, 2, leds_t.led_green);
            Thread.sleep(2000);
            set_led(d, 0, leds_t.led_blue);
            set_led(d, 1, leds_t.led_blue);
            set_led(d, 2, leds_t.led_blue);
            Thread.sleep(2000);
            set_led(d, 2, leds_t.led_red);
            for (int i = 0; i < 5; ++i) {
                ColorDrawable s=leds_t.led_green;
                set_led(d, 0, s);
                set_led(d, 1, s);
                Thread.sleep(200 + 5 * i);
                s = leds_t.led_off;
                set_led(d, 0, s);
                set_led(d, 1, s);
                Thread.sleep(100 - 5 * i);
            }
            set_led(d, 2, leds_t.led_amber);
            for (int i = 5; i < 10; ++i) {
                set_led(d, 0, leds_t.led_green);
                set_led(d, 1, leds_t.led_off);
                Thread.sleep(200 + 5 * i);
                set_led(d, 0, leds_t.led_off);
                set_led(d, 1, leds_t.led_green);
                Thread.sleep(100 - 5 * i);
            }
            set_led(d, 2, leds_t.led_green);
            for (int i = 10; i < 15; ++i) {
                set_led(d, 0, leds_t.led_off);
                set_led(d, 1, leds_t.led_off);
                Thread.sleep(200 + 5 * i);
                set_led(d, 0, leds_t.led_green);
                set_led(d, 1, leds_t.led_green);
                Thread.sleep(100 - 5 * i);
            }
            set_led(d, 0, leds_t.led_off);
            set_led(d, 1, leds_t.led_off);
            set_led(d, 2, leds_t.led_off);
        }
        catch(Exception e) {
        }
        lock_active_ac.lock();
        try {
            if (active_ac == null) return;
            if (d == 0) {
                active_ac.set_leds_visibility_wallet(prev);
            }
            else {
                active_ac.set_leds_visibility_gov(prev);
            }
        }
        finally {
            lock_active_ac.unlock();
        }
        log("report status"); //--strip
        if (hmi != null) {
            hmi.report_status();
        }
    }

    public void led_test(final int daemon) {
        assert_ui_thread();  //--strip
        Thread thread = new Thread(new Runnable() {
            @Override public void run() {
                led_test__worker(daemon);
            }
        });
        thread.start();
    }

    public void led_test() {
        log("led_test"); //--strip
        led_test(0);
        led_test(1);
    }

    public void use_personality(final hash_t tid, final String personality) {
        assert_ui_thread();  //--strip
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                hmi.command_trade(tid, "use_personality " + personality);
            }
        });
        thread.start();
    }

    public void start_protocol(final hash_t tid, final String procol) {
        log("start_protocol " + tid.encode() + " " + procol); //--strip
        Thread thread = new Thread(new Runnable() {
            @Override public void run() {
                log("starting protocol " + procol); //--strip
                hmi.command_trade(tid, "start " + procol);
            }
        });
        thread.start();
    }


/*
    public void write_settings(String data) {
        log("writing settings " + data); //--strip
        try {
            OutputStreamWriter os = new OutputStreamWriter(openFileOutput(SETTINGS_JSON_PRIVATE_FILE, Context.MODE_PRIVATE));
            os.write(data);
            os.close();
        }
        catch (IOException e) {
            //error_manager.manage(e, e.getMessage() + "    " + e.toString());
            log("File write failed: " + e.toString()); //--strip
        }
    }

    public String read_settings() {
        try {
            log("read_settings " + SETTINGS_JSON_PRIVATE_FILE); //--strip
            InputStream inputStream = getApplicationContext().openFileInput(SETTINGS_JSON_PRIVATE_FILE);
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
            //error_manager.manage(e, e.getMessage() + "    " + e.toString());
            log("Can not read file: " + e.toString()); //--strip
        }
        return null;
    }

    public JSONArray read_settings2() {
        String sdata = read_settings();
        if (sdata == null) {
            log("KO 65784 Could not load settings"); //--strip
            return null;
        }
        JSONArray data;
        try {
            data = new JSONArray(sdata);
        }
        catch (JSONException e) {
            //error_manager.manage(e, e.getMessage() + "    " + e.toString());
            return null;
        }
        if (data == null) {
            log("KO 93827 not JSON"); //--strip
            return null;
        }
        return data;
    }

    public endpoint_t endpoint_from_settings() {
        JSONArray data = read_settings2();
        if (data == null) {
            log("KO 93824 Could not read settings."); //--strip
            return null;
        }
        try {
            for (int i = 0; i < data.length(); i++) {
                JSONObject o = data.getJSONObject(i);
                if (o != null) {
                    if (o.getString("default").equals("1")) {
                        return new endpoint_t(new shost_t(o.getString("lanip")), new port_t(o.getInt("lanport")), new channel_t(o.getInt("channel")));
                    }
                }
            }
        }
        catch (JSONException e) {
            //error_manager.manage(e, e.getMessage() + "    " + e.toString());
            return null;
        }
        return null;
    }
    boolean save(endpoint_t ep0) {
        assert ep0 != null;
        endpoint_t ep = new endpoint_t(ep0);
        ep.channel = new channel_t(((int)(ep.channel.value)));
        log("save endpoint to settings " + ep.to_string()); //--strip
        JSONArray data = read_settings2();
        if (data == null) {
            data = new JSONArray();
        }
        boolean found = false;
        try {
            for(int i = 0; i < data.length(); i++) {
                JSONObject o = data.getJSONObject(i);
                endpoint_t item = new endpoint_t(o);
                if (item.equals(ep)) {
                    o.remove("default");
                    o.remove("verified");
                    o.put("default", "1");
                    o.put("verified", "1");
                    found = true;
                }
                else { //Update others
                    o.remove("default");
                    o.put("default", "0");
                }
            }
            if (!found) {
                JSONObject settnew = new JSONObject();
                settnew.put("label", "Node @ " + ep.shost.value);
                settnew.put("wanip", ep.shost.value);
                settnew.put("lanip", ep.shost.value);
                settnew.put("wanport", ep.port.value);
                settnew.put("lanport", ep.port.value);
                settnew.put("channel", ep.channel.value);
                settnew.put("default", "1");
                settnew.put("verified", "1");
                data.put(settnew);
            }
            write_settings(data.toString());
        }
        catch(JSONException e) {
            log("Exception " + e.getMessage()); //--strip
            //FirebaseCrashlytics.getInstance().recordException(e); //--strip
            return false;
        }
        return true;
    }
*/

/*
    public boolean is_HMI_poweron() {
        if (hmi == null) return false;
        return true;
    }
*/

/*
    public void launch_settings__worker() {
        assert_worker_thread(); //--strip
        lock_active_ac.lock();
        try {
            if (main == null) {
                log("Cannot launch settings main = null"); //--strip
                return;
            }
            main.launch_settings__worker();
        }
        finally {
            lock_active_ac.unlock();
        }
    }

    public void launch_settings() {
        a.assert_ui_thread(); //--strip
        lock_active_ac.lock();
        try {
            if (main == null) {
                log("Cannot launch settings main = null"); //--strip
                return;
            }
            main.launch_settings();
        }
        finally {
            lock_active_ac.unlock();
        }
    }
*/
    public void runOnUiThread(Runnable uitask) {
        assert_worker_thread(); //--strip
        if (main == null) {
            log("main is null"); //--strip
            return;
        }
        main.runOnUiThread(uitask);
    }



/*
    void init_hmi_(progress_t progress) {
        log("init_hmi"); //--strip
        assert_ui_thread();  //--strip
        final app a = this;
        Thread worker = new Thread(new Runnable() {
            @Override public void run() {
                HMI_power_on(progress);
            }
        });
        worker.start();
    }
*/

    String r_(int id) {
        return getResources().getString(id);
    }

    public void new_trade(final hash_t parent_trade, final String datasubdir, final qr_t qr) {
        assert_ui_thread();  //--strip
        log("new_trade " + qr.to_string()); //--strip
        hmi.new_trade(parent_trade, datasubdir, qr);
        log("invoked API new_trade"); //--strip
/*
        Thread thread = new Thread(new Runnable() {
            @Override public void run() {
                a.hmi.new_trade(parent_trade, datasubdir, qr);
                log("invoked API new_trade"); //--strip
            }
        });
        thread.start();
*/
        Toast.makeText(main, r_(R.string.newtrade) + "\n" + qr.to_string(), 600).show();
    }

    public void call_human(int code, final String info) {
/*
        switch(code) {
            case 1: //trade #tid received a chat update
                String tid = info;
                log("CALL_HUMAN "+code+" "+info); //--strip
                show_notification(1, info);
                //TODO: ... Show ! icon on every view there a tid is shown
            break;
       }
*/
    }

    public void go_trade__worker(final hash_t tid) {
        assert_worker_thread();  //--strip
        log("go_trade__worker " + tid.encode()); //--strip
        runOnUiThread(new Runnable() {
            @Override public void run() {
                a.go_trade(tid);
            }
        });
    }

    public void go_trade(final hash_t tid) {
        assert_ui_thread();  //--strip
        log("go_trade " + tid.encode());  //--strip
        if (main == null) {
            log("KO 77968 main is null"); //--strip
            return;
        }
        Intent intent = new Intent(main, trader.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        intent.putExtra("tid", tid.value);
        startActivity(intent);
    }

    public String balance(string s) {
        if (hmi == null) {
            return KO_89700;
        }
        final ko r = hmi.rpc_peer.call_balance(new uint16_t(0), s);
        String x = null;
        if (is_ko(r)) {
            x = a.hmi.rewrite(r);
        }
        return x;
    }

    public void toast__worker(final String msg) {
        log("toast__worker " + msg); //--strip
        lock_active_ac.lock();
        try {
            if (active_ac == null) {
            	log("Could not toast, active activity = null. " + msg); //--strip
            }
            active_ac.toast__worker(msg);
        }
        finally {
            lock_active_ac.unlock();
        }
    }

    @Override public void on_push(hash_t target_tid, uint16_t code, byte[] payload) {
        log("on_push " + target_tid.encode() + " code " + code.value + " payload BIN sz: " + payload.length + " bytes"); //--strip
        switch(code.value) {
            case us.wallet.trader.trader_t.push_trade: {
                log("a new trade for me"); //--strip
                go_trade__worker(target_tid);
                return;
            }
            case us.wallet.trader.trader_t.push_chat: {
                runOnUiThread(new Runnable() {
                    @Override public void run() {
                        call_human(1, target_tid.encode());
                    }
                });
                break;
            }
            case us.gov.relay.pushman.push_ko: {
                string s = new string();
                blob_reader_t rder = new blob_reader_t(new blob_t(payload));
                ko r = rder.read(s);
                if (is_ko(r)) {
                    toast__worker("KO decode payload trade " + target_tid.encode() + ": " + r.msg);
                    return;
                }
                toast__worker("KO trade " + target_tid.encode() + ": " + s.value);
                return;
            }
            case us.gov.relay.pushman.push_ok: {
                string s = new string();
                blob_reader_t rder = new blob_reader_t(new blob_t(payload));
                ko r = rder.read(s);
                if (is_ko(r)) {
                    toast__worker("OK. later got " + r.msg);
                    return;
                }
                //toast__worker("OK trade " + target_tid.encode() + ": " + s.value);
                return;
            }
        }
    }

    public void HMI_power_on__worker(int pos, final pin_t pin, progress_t progress) {
        assert_worker_thread();  //--strip
        set_walletd_leds_off();
        set_govd_leds_off();
        if (progress == null) {
            log("adding default progress"); //--strip
            progress = new app.progress_t() {
                @Override public void on_progress(final String report) {
                    log("progress: " + report); //--strip
                }
            };
        }
        if (hmi != null) {
            progress.on_progress("Cannot power on other endpoint while " + device_endpoints.cur.name_ + " is already powereded ON.");
            return;
        }
        hmi = device_endpoints.poweron(this, pos, pin, progress);
        if (hmi != null) {
            dispatchid = hmi.dispatcher.connect_sink(this);
        }

    }

    public void HMI_power_off__worker(final int pos, final progress_t progress) {
        assert_worker_thread();  //--strip
        log("HMI_power_off__worker"); //--strip
        if (hmi == null) {
            dispatchid = -1;
            return;
        }
        if (dispatchid != -1) {
            hmi.dispatcher.disconnect_sink(dispatchid);
            dispatchid = -1;
        }
        log("set leds off"); //--strip
        device_endpoints.poweroff(this, pos, progress);
        set_walletd_leds_off();
        set_govd_leds_off();
        hmi = null;
    }

    public void HMI_power_on(final int pos, final pin_t pin, final progress_t progress) {
        assert_ui_thread();  //--strip
        Thread thread = new Thread(new Runnable() {
            @Override public void run() {
                HMI_power_on__worker(pos, pin, progress);
            }
        });
        thread.start();
    }

    public void HMI_power_off(final int pos, final progress_t progress) {
        a.assert_ui_thread(); //--strip
        Thread thread = new Thread(new Runnable() {
            @Override public void run() {
                HMI_power_off__worker(pos, progress);
            }
        });
        thread.start();
    }

    final app a = (app) this;
    PrintStream cout = System.out;
    activity active_ac = null;
    main_activity main = null;

    Lock lock_active_ac = new ReentrantLock();
    hmi_t hmi = null; ///current hmi

/*
    public void set_hmi(int position) {
        log("set_hmi " + position); //--strip
        a.device_endpoints.set_cur(position);
        hmi = a.device_endpoints.cur.hmi;
    }
*/

    public String net_identifier() {
        ConnectivityManager cm;
        NetworkInfo info = null;
        try {
            cm = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
            info = cm.getActiveNetworkInfo();
        }
        catch (Exception e) {
            return ""; //e.printStackTrace();
        }
        if (info == null) {
            return "";
        }
        if (!info.isConnected()) {
            return "";
        }
        if (info.getType() == ConnectivityManager.TYPE_WIFI) {
            WifiManager wifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
            WifiInfo wifiInfo;
            wifiInfo = wifiManager.getConnectionInfo();
            if (wifiInfo.getSupplicantState() == SupplicantState.COMPLETED) {
                String ssid = wifiInfo.getSSID();
                ssid = ssid.replaceAll("\"", "");
                if (!ssid.equals("<unknown ssid>")) {
                    return ssid;
                }
            }
            return "wifi";
        }
        if (info.getType() == ConnectivityManager.TYPE_MOBILE) { 
            switch (info.getSubtype()) {
                case TelephonyManager.NETWORK_TYPE_GPRS:
                case TelephonyManager.NETWORK_TYPE_EDGE:
                case TelephonyManager.NETWORK_TYPE_CDMA:
                case TelephonyManager.NETWORK_TYPE_1xRTT:
                case TelephonyManager.NETWORK_TYPE_IDEN:
                case TelephonyManager.NETWORK_TYPE_GSM:
                     return "2G";
                case TelephonyManager.NETWORK_TYPE_UMTS:
                case TelephonyManager.NETWORK_TYPE_EVDO_0:
                case TelephonyManager.NETWORK_TYPE_EVDO_A:
                case TelephonyManager.NETWORK_TYPE_HSDPA:
                case TelephonyManager.NETWORK_TYPE_HSUPA:
                case TelephonyManager.NETWORK_TYPE_HSPA:
                case TelephonyManager.NETWORK_TYPE_EVDO_B:
                case TelephonyManager.NETWORK_TYPE_EHRPD:
                case TelephonyManager.NETWORK_TYPE_HSPAP:
                case TelephonyManager.NETWORK_TYPE_TD_SCDMA:
                    return "3G";
                case TelephonyManager.NETWORK_TYPE_LTE:
                case TelephonyManager.NETWORK_TYPE_IWLAN:
                case 19:
                    return "4G";
                case TelephonyManager.NETWORK_TYPE_NR:
                    return "5G";
                default:
                    return "mobile";
            }
        }
        return "unknown";
    }

    public static String KO_68874 = "KO 68874 Invalid endpoint.";
    public static String KO_89700 = "KO 89700 HMI is off.";

    device_endpoints_t device_endpoints = null;
    //device_endpoint_t cur;
    //ArrayList<cryptocurrency> shit

    //public String homedir = null;
//    public ArrayList<String> notification_trades_id =  new ArrayList<String>();
    public i18n_t i18n;
    ToneGenerator tone;
    int dispatchid = -1;

}


