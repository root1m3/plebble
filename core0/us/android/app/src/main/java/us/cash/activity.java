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
import com.google.android.material.navigation.NavigationView;                                  // NavigationView
import androidx.appcompat.widget.Toolbar;                                                      // Toolbar
import android.widget.RelativeLayout;                                                          // RelativeLayout
import android.view.Menu;                                                                      // Menu
import android.view.MenuItem;                                                                  // MenuItem
import android.widget.LinearLayout;                                                            // LinearLayout
import android.view.LayoutInflater;                                                            // LayoutInflater
import android.content.Context;                                                                // Context
import android.view.Window;                                                                    // Window
import androidx.drawerlayout.widget.DrawerLayout;                                              // DrawerLayout
import androidx.appcompat.app.ActionBarDrawerToggle;                                           // ActionBarDrawerToggle
import android.view.ViewGroup.LayoutParams;

public class activity extends AppCompatActivity implements NavigationView.OnNavigationItemSelectedListener {

    static void log(final String line) {         //--strip
        CFG.log_android("activity: " + line);    //--strip
    }                                            //--strip

    public activity() {
        locale.update(this);
    }

    @Override protected void onCreate(@Nullable Bundle savedInstanceState) {
        log("onCreate"); //--strip
        log("requestWindowFeature"); //--strip
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        log("super.onCreate"); //--strip
        super.onCreate(savedInstanceState);
        log("setContentView"); //--strip
        setContentView(R.layout.activity);
    }

    public void set_content_layout(int res) {
        log("set_content_layout"); //--strip
        LinearLayout contentparent = findViewById(R.id.home_content);
        LinearLayout content = (LinearLayout) View.inflate(this, res, null);
        content.setLayoutParams(new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));
        contentparent.addView(content);
        contentparent.invalidate();
        toolbar = findViewById(R.id.toolbar);
        toolbar_button refresh = findViewById(R.id.refresh);
        drawer_layout = findViewById(R.id.drawer_layout);
        navigation = findViewById(R.id.navigation_view);
        toolbar = findViewById(R.id.toolbar);
        refresh.setVisibility(View.GONE);
        //toolbar.setTitle("");
        log("toolbar visible");//--strip
        toolbar.setVisibility(View.VISIBLE);
        setSupportActionBar(toolbar);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(this, drawer_layout, toolbar, R.string.bookmarks, R.string.bookmarks);
        drawer_layout.addDrawerListener(toggle);
        toggle.syncState();
        navigation.setNavigationItemSelectedListener(this);
        //tests to recognize new IPs
        //if (!wifiManager.isWifiEnabled()) {
        //    Toast.makeText(this, "WiFi is disabled ... We need to enable it", Toast.LENGTH_LONG).show();
        //    wifiManager.setWifiEnabled(true);
        //}
        //scanWifiNetworks();
        Menu nav_menu = navigation.getMenu();
        updateavailable = nav_menu.findItem(R.id.nav_updateavailable);
        if (a.sw_updates.is_updateavailable){
            updateavailable.setVisible(true);
        }
        else {
            updateavailable.setVisible(false);
        }
        boolean showiot = false;
        //release builds (which don't incluce lines marked '--strip' ) doesn't show the experimental IoT menu.
        showiot = true; //--strip
        if (showiot) {
            nav_menu.add(R.id.group5, 5948, Menu.NONE, "IoT");
        }
    }

    @Override public void onResume() {
        log("onResume"); //--strip
        super.onResume();
        a.set_foreground_activity(this, true);
        resume_leds();
        if (a.hmi == null) {
            log("HMI is null"); //--strip
            return;
        }
        if (walletd_leds != null || govd_leds != null) {
            Thread worker = new Thread(new Runnable() {
                @Override public void run() {
                    if (walletd_leds != null) a.hmi.report_status();
                    if (govd_leds != null) a.hmi.report_status_gov();
                }
            });
            worker.start();
        }
    }

    @Override public void onPause() {
        super.onPause();
        a.set_foreground_activity(this, false);
    }

    void locale_init() {
        String lang = locale.get_lang(this);
        String country = locale.get_country(this);
        if (lang == null || country == null){
            locale.set(new Locale("en", "GB"));
        }
        else {
            locale.set(new Locale(lang, country));
        }
    }

    @SuppressWarnings("StatementWithEmptyBody")
    @Override public boolean onNavigationItemSelected(MenuItem menu_item) {
        return main.onNavigationItemSelected(menu_item);
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
        activity ac = this;
        return new View.OnClickListener() {
            @Override public void onClick(View v) {
                String[] options = {"Go to network settings...", "Test leds.", a.getResources().getString(R.string.cancel)};
                new AlertDialog.Builder(ac).setTitle("Quick access to...").setItems(options,new DialogInterface.OnClickListener() {
                    @Override public void onClick(DialogInterface dialog, int which) {
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
                }).setIcon(R.drawable.ic_world).show();
            }
        };
    }

    void resume_walletd_leds() {
        app.assert_ui_thread(); //--strip
        View v = findViewById(R.id.walletd_leds);
        if (v != null && leds_visible_wallet && walletd_leds == null) {
            walletd_leds = new leds_t(this, v);
            walletd_leds.set_visibility(true);
            walletd_leds.view.setOnClickListener(get_on_click_listener());
            return;
        }
        if (!leds_visible_wallet && walletd_leds != null) {
            walletd_leds.set_visibility(false);
            walletd_leds = null;
            return;
        }
    }

    void resume_govd_leds() {
        app.assert_ui_thread(); //--strip
        View v = findViewById(R.id.govd_leds);
        if (v != null && leds_visible_gov && govd_leds == null) {
            govd_leds = new leds_t(this, v);
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
            @Override public void run() {
                report_ko(r);
            }
        });
    }

    void resume_leds() {
        app.assert_ui_thread(); //--strip
        resume_govd_leds();
        resume_walletd_leds();
    }

    public void set_leds_visibility_gov(boolean b) {
        app.assert_worker_thread(); //--strip
        leds_visible_gov = b;
        runOnUiThread(new Runnable() {
            @Override public void run() {
                resume_govd_leds();
            }
        });
    }

    public void set_leds_visibility_wallet(boolean b) {
        app.assert_worker_thread(); //--strip
        leds_visible_wallet = b;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                resume_walletd_leds();
            }
        });
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
    Toolbar toolbar;
    public NavigationView navigation;
    public DrawerLayout drawer_layout;
    private MenuItem updateavailable;

}

