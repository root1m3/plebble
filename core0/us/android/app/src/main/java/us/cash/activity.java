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
import androidx.appcompat.app.AlertDialog;                                                     // AlertDialog
import androidx.appcompat.app.AppCompatActivity;                                               // AppCompatActivity
import android.os.Bundle;                                                                      // Bundle
import java.util.concurrent.locks.Condition;                                                   // Condition
import android.content.DialogInterface;                                                        // DialogInterface
import us.ko;                                                                                  // ko
import java.util.Locale;                                                                       // Locale
import java.util.concurrent.locks.Lock;                                                        // Lock
import androidx.annotation.Nullable;                                                           // Nullable
import java.util.concurrent.locks.ReentrantLock;                                               // ReentrantLock
import android.widget.Toast;                                                                   // Toast
import android.view.View;                                                                      // View

public class activity extends AppCompatActivity {

    static void log(final String line) {        //--strip
       CFG.log_android("activity: " + line);    //--strip
    }                                           //--strip

    public activity() {
        locale.update(this);
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public void onResume() {
        log("onResume"); //--strip
        super.onResume();
        log("onResume - A"); //--strip
        a.set_foreground_activity(this, true);
        log("onResume - A1.5"); //--strip
        resume_leds();
        log("onResume - A1"); //--strip
        if (a.hmi == null) {
            log("HMI is null"); //--strip
            return;
        }
        log("onResume - A2"); //--strip
        if (walletd_leds != null) a.hmi.report_status();
        if (govd_leds != null) a.hmi.report_status_gov();
        log("onResume - A3"); //--strip
    }

    @Override
    public void onPause() {
        super.onPause();
        a.set_foreground_activity(this, false);
    }

    void locale_init() {
        String lang = locale.get_lang(this);
        String country = locale.get_country(this);
        if (lang==null || country==null){
            locale.set(new Locale("en", "GB"));
        }
        else {
            locale.set(new Locale(lang, country));
        }
    }

    public static void wait_main() {
        log("waiting for main activity"); //--strip
        lock.lock();
        try {
            while (main == null) {
                cv.await();
            }
        }
        catch (Exception e) {
            log("KO 40398"); //--strip
        }
        finally {
            lock.unlock();
        }
        log("end waiting for main activity"); //--strip
    }

    public void main_ready(main_activity m) {
        log("received signal from main activity"); //--strip
        lock.lock();
        try {
            assert main == null;
            main=m;
            cv.signalAll();
        }
        finally {
            lock.unlock();
        }
        log("main activity is ready"); //--strip
    }

    View.OnClickListener get_on_click_listener() {
        activity ac=this;
        return new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String[] options = {"Go to network settings...", "Test leds.", a.getResources().getString(R.string.cancel)};
                new AlertDialog.Builder(ac).setTitle("Quick access to...")
                    .setItems(options,new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            switch(which) {
                                case 0:
                                    activity.main.launch_settings();
                                    break;
                                case 1:
                                    activity.a.led_test();
                                    break;
                                case 2:
                            }
                        }
                    })
                    .setIcon(R.drawable.ic_world).show();
            }
        };
    }

    void resume_walletd_leds() {
        if (leds_visible_wallet && walletd_leds == null) {
            walletd_leds = new leds_t(this, findViewById(R.id.walletd_leds));
            walletd_leds.set_visibility(true);
            walletd_leds.view.setOnClickListener(get_on_click_listener());
            return;
        }
        if (!leds_visible_wallet && walletd_leds!=null) {
            walletd_leds.set_visibility(false);
            walletd_leds = null;
            return;
        }
    }

    void resume_govd_leds() {
        if (leds_visible_gov && govd_leds == null) {
            govd_leds = new leds_t(this, findViewById(R.id.govd_leds));
            govd_leds.set_visibility(true);
            govd_leds.view.setOnClickListener(get_on_click_listener());
            return;
        }
        if (!leds_visible_gov && govd_leds != null) {
            govd_leds.set_visibility(false);
            govd_leds = null;
            return;
        }
    }

    public void report_ko(final ko r) {
        log("report_ko " + r.msg); //--strip
        Toast.makeText(this, r.msg, Toast.LENGTH_LONG).show();
    }

    public void report_ko__worker(final ko r) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                report_ko(r);
            }
        });
    }

    void resume_leds() {
        resume_govd_leds();
        resume_walletd_leds();
    }

    public void set_leds_visibility_gov(boolean b) {
        leds_visible_gov = b;
        resume_govd_leds();
    }

    public void set_leds_visibility_wallet(boolean b) {
        leds_visible_wallet=b;
        resume_walletd_leds();
    }

    public static main_activity main = null;
    public static app a = null;
    public static boolean leds_visible = true;
    public static boolean leds_visible_gov = false;
    public static boolean leds_visible_wallet = true;
    static Lock lock = new ReentrantLock();
    static Condition cv = lock.newCondition();
    leds_t walletd_leds = null;
    leds_t govd_leds = null;
}

