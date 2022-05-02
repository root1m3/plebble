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
import java.io.BufferedInputStream;                                                            // BufferedInputStream
import java.io.BufferedReader;                                                                 // BufferedReader
import us.gov.socket.busyled_t;                                                                // busyled_t
import java.io.ByteArrayOutputStream;                                                          // ByteArrayOutputStream
import java.nio.charset.Charset;                                                               // Charset
import java.util.Collections;                                                                  // Collections
import android.graphics.drawable.ColorDrawable;                                                // ColorDrawable
import java.util.concurrent.locks.Condition;                                                   // Condition
import android.content.res.Configuration;                                                      // Configuration
import android.content.Context;                                                                // Context
import org.acra.config.CoreConfigurationBuilder;                                               // CoreConfigurationBuilder
import us.gov.socket.datagram;                                                                 // datagram
import android.app.DialogFragment;                                                             // DialogFragment
import android.util.DisplayMetrics;                                                            // DisplayMetrics
import android.os.Environment;                                                                 // Environment
import java.io.File;                                                                           // File
import java.io.FileNotFoundException;                                                          // FileNotFoundException
import java.io.FileOutputStream;                                                               // FileOutputStream
import androidx.core.content.FileProvider;                                                     // FileProvider
import com.google.firebase.crashlytics.FirebaseCrashlytics;                                    // FirebaseCrashlytics
import java.security.GeneralSecurityException;                                                 // GeneralSecurityException
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import org.acra.config.HttpSenderConfigurationBuilder;                                         // HttpSenderConfigurationBuilder
import org.acra.sender.HttpSender;                                                             // HttpSender
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
import static us.ko.ok;                                                                        // ok
import java.io.OutputStream;                                                                   // OutputStream
import java.io.OutputStreamWriter;                                                             // OutputStreamWriter
import us.pair;                                                                                // pair
import java.net.PasswordAuthentication;                                                        // PasswordAuthentication
import android.preference.PreferenceManager;                                                   // PreferenceManager
import java.io.PrintStream;                                                                    // PrintStream
import us.wallet.protocol;                                                                     // protocol
import java.util.Random;                                                                       // Random
import java.util.concurrent.locks.ReentrantLock;                                               // ReentrantLock
import android.content.res.Resources;                                                          // Resources
import android.content.SharedPreferences;                                                      // SharedPreferences
import android.os.StrictMode;                                                                  // StrictMode
import org.acra.data.StringFormat;                                                             // StringFormat
import java.util.Timer;                                                                        // Timer
import java.util.TimerTask;                                                                    // TimerTask
import android.widget.Toast;                                                                   // Toast
import android.media.ToneGenerator;                                                            // ToneGenerator
import java.util.TreeMap;                                                                      // TreeMap
import java.io.UnsupportedEncodingException;                                                   // UnsupportedEncodingException
import java.net.URL;                                                                           // URL

public final class app extends Application {

    public static String KO_68874 = "KO 68874 Invalid endpoint";
    static final String SETTINGS_JSON_PRIVATE_FILE = "node_settings.json";

    static void log(final String line) {         //--strip
       CFG.log_android("app: " + line);          //--strip
    }                                            //--strip

    public static void assert_ui_thread() { //--strip
        if (!Looper.getMainLooper().isCurrentThread()) { //--strip
            log(Arrays.toString(Thread.currentThread().getStackTrace())); //--strip
            log("ASSERT UI THREAD FAILED"); //--strip
            throw new AssertionError("not UI Thread"); //--strip
        } //--strip
    } //--strip

    public static void assert_worker_thread() { //--strip
        if (Looper.getMainLooper().isCurrentThread()) { //--strip
            log(Arrays.toString(Thread.currentThread().getStackTrace())); //--strip
            log("ASSERT WORKER THREAD FAILED"); //--strip
            throw new AssertionError("UI Thread"); //--strip
        } //--strip
    } //--strip

    public interface progress_t {
        void on_progress(final String line);
    }

    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
        error_manager.setup(base, this);
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
    }

    public app() {
        assert_ui_thread(); //--strip
        log("----------------------APP CONSTRUCTOR--------------------"); //--strip
        log("Stack Trace at begining"); //--strip
        log(Arrays.toString(Thread.currentThread().getStackTrace())); //--strip
CFG.sdk_logs = true; //--strip
        log("SDK logs: " + CFG.sdk_logs); //--strip
        us.CFG.logs.set(CFG.sdk_logs); //--strip
    }

    void abort(String reason) {
        error_manager.manage(new GeneralSecurityException(reason), "KO 50493");
        android.os.Process.killProcess(android.os.Process.myPid());
        System.exit(1);
    }

    public static class walletd_status_handler_t implements hmi_t.status_handler_t {

        public walletd_status_handler_t(app a_) {
            a = a_;
        }

        @Override
        public void on_status(ColorDrawable led_status, final String msg) {
            a.set_walletd_led(2, led_status);
        }

        app a;
    }

    void init_hmi(progress_t progress) {
        log("init_hmi"); //--strip
        final app a = this;
        Thread worker = new Thread(new Runnable() {
            @Override
            public void run() {
                log("Starting HMI ..."); //--strip
                us.wallet.cli.params p = new us.wallet.cli.params();
                p.daemon = false;
                p.homedir = homedir + "/hmi";
                log("homedir=" + p.homedir); //--strip
                p.rpc__connect_for_recv = true;
                p.rpc__stop_on_disconnection = false;
                hmi = new hmi_t(a, p, new walletd_status_handler_t(a), datagram_dispatcher, System.out);
                hmi.busyled_handler_recv = new walletd_led_handler(app.this, 0);
                hmi.busyled_handler_send = new walletd_led_handler(app.this, 1);
                endpoint_t ep = endpoint_from_settings();
                if (ep == null) {
                    log("ep is null"); //--strip
                    save(new endpoint_t());
                    ep = endpoint_from_settings();
                    assert ep != null;
                }
                hmi.try_renew_ip_on_connect_failed = true;
                if (ep == null) {
                    ko r = new ko("KO 54093 invalid ep in settings. Restart of HMI's been aborted.");
                    log(r.msg); //--strip
                    return;
                }
                log("restarting HMI with ep " + ep.to_string()); //--strip
                ko r = restart_hmi(ep, new pin_t(0), progress);
                if (is_ko(r)) {
                    log(r.msg); //--strip
                }
            }
        });
        worker.start();
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

    @Override
    public void onCreate() {
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
        homedir = getApplicationContext().getFilesDir().getPath();
        log("boot test"); //--strip
        if (!hmi_t.test()) {
            abort("failed hmi tests");
        }
        log("boot test succeed"); //--strip

        log("creating datagram dispatcher"); //--strip
        datagram_dispatcher = new datagram_dispatcher_t(3);
        log("Private storage  " + homedir); //--strip
        sw_updates = new sw_updates_t(this);
        tone = new ToneGenerator(AudioManager.STREAM_MUSIC, 100);
    }

    @Override
    public void onTerminate() {
        super.onTerminate();
        log("onTerminate"); //--strip
        log("disconnect hose"); //--strip
    }

    public endpoint_t get_endpoint() {
        if (hmi == null) return null;
        return hmi.endpoint;
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
        if (daemon == 0) set_walletd_led(i, color);
        else set_govd_led(i, color);
    }

    void set_walletd_led(int i, ColorDrawable color) {
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

    void set_govd_led(int i, ColorDrawable color) {
        lock_active_ac.lock();
        try {
            if (active_ac == null) return;
            if (active_ac.govd_leds == null) return;
            active_ac.govd_leds.set_led(i, color);
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

        @Override
        public void on_busy() {
            a.set_walletd_led(i, leds_t.led_green);
        }

        @Override
        public void on_idle() {
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

        @Override
        public void on_busy() {
            a.set_govd_led(i, leds_t.led_green);
        }

        @Override
        public void on_idle() {
            a.set_govd_led(i, leds_t.led_off);
        }

        int i;
        app a;
    }

    int rnd(int min, int max) {
        return new Random().nextInt(1 + min + (max - min));
    }

    public void led_test__worker(int d) {
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

        hmi.report_status();
    }

    public void led_test(final int daemon) {
        assert_ui_thread();  //--strip
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
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

    public ko restart_hmi(final endpoint_t ep, pin_t pin, progress_t report) {
        log("Restarting HMI... pin " + pin.value); //--strip
        report.on_progress("Starting connection to " + (ep == null ? "null" : ep.to_string()));
        ko r = hmi.restart(ep, pin);
        if (r != ok) {
            log("HMI Failed to start: " + r.msg); //--strip
            report.on_progress("Failed to connect to " + (ep == null ? "null" : ep.to_string()));
            return r;
        }
        log("hmi started"); //--strip
        report.on_progress("HMI restarted " + (ep == null ? "null" : ep.to_string()));
        return ok;
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
        log("start_protocol "+tid.encode()+" "+procol); //--strip
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                log("starting protocol "+procol); //--strip
                hmi.command_trade(tid, "start " + procol);
            }
        });
        thread.start();
    }

    public void write_settings(String data) {
        log("writing settings " + data); //--strip
        try {
            OutputStreamWriter os = new OutputStreamWriter(openFileOutput(SETTINGS_JSON_PRIVATE_FILE, Context.MODE_PRIVATE));
            os.write(data);
            os.close();
        }
        catch (IOException e) {
            error_manager.manage(e, e.getMessage() + "    " + e.toString());
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
            while ( (receiveString = bufferedReader.readLine()) != null ) {
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
            error_manager.manage(e, e.getMessage() + "    " + e.toString());
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
            for(int i = 0; i < data.length(); i++) {
                JSONObject o = data.getJSONObject(i);
                if (o.getString("default").equals("1")) {
                    return new endpoint_t(new shost_t(o.getString("lanip")), new port_t(o.getInt("lanport")), new channel_t(o.getInt("channel")));
                }
            }
        }
        catch (JSONException e) {
            error_manager.manage(e, e.getMessage() + "    " + e.toString());
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
                else {
                    //Update others
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
                settnew.put("channel", ep.channel.value);  //test renew ip
                settnew.put("default", "1");
                settnew.put("verified", "1");
                data.put(settnew);
            }
            write_settings(data.toString());
        }
        catch(JSONException e) {
            log("Exception " + e.getMessage()); //--strip
            FirebaseCrashlytics.getInstance().recordException(e); //--strip
            return false;
        }
        return true;
    }

    final app a = (app) this;
    PrintStream cout = System.out;
    activity active_ac = null;
    Lock lock_active_ac = new ReentrantLock();
    datagram_dispatcher_t datagram_dispatcher = null;
    hmi_t hmi = null;
    public String homedir = null;
    sw_updates_t sw_updates = null;
    public ArrayList<String> notification_trades_id =  new ArrayList<String>();
    public i18n_t i18n;
    ToneGenerator tone;
}

