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
import androidx.appcompat.app.ActionBarDrawerToggle;                                           // ActionBarDrawerToggle
import androidx.appcompat.app.AlertDialog;                                                     // AlertDialog
import androidx.appcompat.app.AppCompatActivity;                                               // AppCompatActivity
import android.os.Bundle;                                                                      // Bundle
import java.util.concurrent.locks.Condition;                                                   // Condition
import android.content.Context;                                                                // Context
import android.content.DialogInterface;                                                        // DialogInterface
import androidx.drawerlayout.widget.DrawerLayout;                                              // DrawerLayout
import androidx.core.view.GravityCompat;                                                       // GravityCompat
import us.gov.crypto.ripemd160.hash_t;                                                         // hash_t
import static us.gov.id.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import android.content.Intent;                                                                 // Intent
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import android.view.ViewGroup.LayoutParams;                                                    // LayoutParams
import android.widget.LinearLayout;                                                            // LinearLayout
import java.util.Locale;                                                                       // Locale
import java.util.concurrent.locks.Lock;                                                        // Lock
import android.view.Menu;                                                                      // Menu
import android.view.MenuItem;                                                                  // MenuItem
import com.google.android.material.navigation.NavigationView;                                  // NavigationView
import androidx.annotation.NonNull;                                                            // NonNull
import androidx.annotation.Nullable;                                                           // Nullable
import android.content.pm.PackageManager;                                                      // PackageManager
import us.wallet.trader.qr_t;                                                                  // qr_t
import java.util.concurrent.locks.ReentrantLock;                                               // ReentrantLock
import android.widget.RelativeLayout;                                                          // RelativeLayout
import android.provider.Settings;                                                              // Settings
import java.util.Timer;                                                                        // Timer
import java.util.TimerTask;                                                                    // TimerTask
import android.widget.Toast;                                                                   // Toast
import androidx.appcompat.widget.Toolbar;                                                      // Toolbar
import android.net.Uri;                                                                        // Uri
import android.view.View;                                                                      // View
import android.view.Window;                                                                    // Window
import android.graphics.Bitmap;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Date;
import android.os.Environment;

public class activity extends AppCompatActivity implements NavigationView.OnNavigationItemSelectedListener {

    private static void log(final String line) {         //--strip
        CFG.log_android("activity: " + line);            //--strip
    }                                                    //--strip

    public activity() {
        locale.update(this);
    }

    public int menuid() {
        if (!a.has_hmi()) {
            log("menuid menu_nohmi"); //--strip
            return R.menu.menu_nohmi;
        }
        if (a.hmi().online) {
            log("menuid menu_hmi_online"); //--strip
            return R.menu.menu_hmi_online;
        }
        log("menuid menu_nohmi"); //--strip
        return R.menu.menu_nohmi;
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
        content.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        contentparent.addView(content);
        contentparent.invalidate();
        toolbar = findViewById(R.id.toolbar);
        drawer_layout = findViewById(R.id.drawer_layout);
        navigation = findViewById(R.id.navigation_view);
        toolbar_button refresh = findViewById(R.id.refresh);
        refresh.setVisibility(View.GONE);
        toolbar.setVisibility(View.VISIBLE);
        setSupportActionBar(toolbar);
        //tests to recognize new IPs
        //if (!wifiManager.isWifiEnabled()) {
        //    Toast.makeText(this, "WiFi is disabled ... We need to enable it", Toast.LENGTH_LONG).show();
        //    wifiManager.setWifiEnabled(true);
        //}
        //scanWifiNetworks();
        //set_menu(menuid());
 
        navigation.setNavigationItemSelectedListener(this);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(this, drawer_layout, toolbar, R.string.bookmarks, R.string.bookmarks);
        drawer_layout.addDrawerListener(toggle);
        toggle.syncState();
    }

    @Override public void onResume() {
        log("onResume"); //--strip
        super.onResume();
        a.set_foreground_activity(this, true);
        resume_leds();
        refresh();
        if (!a.has_hmi()) {
            log("HMI is null"); //--strip
            return;
        }
        if (walletd_leds != null || govd_leds != null) {
            Thread worker = new Thread(new Runnable() {
                @Override public void run() {
                    if (walletd_leds != null) a.hmi().report_status();
                    if (govd_leds != null) a.hmi().report_status_gov();
                }
            });
            worker.start();
        }
    }

    @Override public void onPause() {
        super.onPause();
        a.set_foreground_activity(this, false);
    }

    @Override public void onDestroy() {
        super.onDestroy();
        a.set_foreground_activity(this, false);
    }

    public void stop_hmi(final boolean save_) {
        app.progress_t progress = new app.progress_t() {
            @Override public void on_progress(final String report) {
                runOnUiThread(new Runnable() {
                    @Override public void run() {
                        toast(report);
                    }
                });
            }
        };
        a.HMI_power_off(a.device_endpoints.cur_index.value, save_, progress);
        Timer timer = new Timer();
        timer.schedule(new TimerTask() {
          @Override public void run() {
            runOnUiThread(new Runnable() {
                @Override public void run() { 
                    log("Saving settings"); //--strip
                    a.device_endpoints.save();
                    refresh(); 
                }
            });
          }
        }, 2000);
    }

    public void start_hmi(int conf_index) {
        app.progress_t progress = new app.progress_t() {
            @Override public void on_progress(final String report) {
                runOnUiThread(new Runnable() {
                    @Override public void run() {
                        Toast.makeText(activity.this, report, 6000).show();
                    }
                });
            }
        };
        a.HMI_power_on(conf_index, new pin_t(0), progress);
        Timer timer = new Timer();
        timer.schedule(new TimerTask() {
          @Override public void run() {
            runOnUiThread(new Runnable() {
                @Override public void run() {
                    log("Saving settings"); //--strip
                    a.device_endpoints.save();
                    refresh();
                }
            });
          }
        }, 3000);
    }

    private void set_menu(int res_menu) {
        log("set_menu " + res_menu); //--strip
        log("clear menu"); //--strip
        navigation.getMenu().clear();
        if (res_menu > 0) {
            log("set menu " + res_menu); //--strip
            navigation.inflateMenu(res_menu);
        }
    }

    public String acid() {
        return "activity";
    }

    public void refresh() {
        log("refresh"); //--strip
        a.assert_ui_thread(); //--strip
        set_menu(menuid());
        log("end refresh " + acid()); //--strip
    }

    public void refresh__worker() {
        log("refresh"); //--strip
        a.assert_worker_thread(); //--strip
        runOnUiThread(new Runnable() {
            @Override public void run() {
                refresh();
            }
        });
    }

    void locale_init() {
        String lang = locale.get_lang(this);
        String country = locale.get_country(this);
        if (lang == null || country == null) {
            locale.set(new Locale("en", "GB"));
        }
        else {
            locale.set(new Locale(lang, country));
        }
    }

    public void launch_myqr() {
        log("menu myqr"); //--strip
        Intent intent = new Intent(this, endpoint.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NO_HISTORY);
        startActivity(intent);
    }

    public void launch_position() {
        log("menu position"); //--strip
        Intent intent = new Intent(this, position.class);
        startActivity(intent);
    }

    public void world() {
        log("menu world"); //--strip
        Intent intent = new Intent(this, nodes.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NO_HISTORY);
        startActivity(intent);
    }

    public void go_certs() {
        log("menu certs"); //--strip
        Intent intent = new Intent(this, certs.class);
        //intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NO_HISTORY);
        startActivity(intent);
    }

    void doscan(boolean frommainmenu) {
        Intent intent = new Intent(this, scan.class);
        intent.putExtra("what", 0);
        if (frommainmenu) {
            intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NO_HISTORY);
        }
        startActivityForResult(intent, SCAN_REQUESTCODE);
    }

    public void go_trades() {
        log("go_trades"); //--strip
        Intent intent = new Intent(this, trades.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NO_HISTORY);
        startActivity(intent);
    }

    public void launch_about() {
        a.assert_ui_thread(); //--strip
        log("launching about..."); //--strip
        Intent intent = new Intent(this, about.class);
        startActivity(intent);
    }

    public void launch_language() {
        a.assert_ui_thread(); //--strip
        log("launching language..."); //--strip
        Intent intent = new Intent(this, settings_language.class);
        startActivity(intent);
    }

    @SuppressWarnings("StatementWithEmptyBody")
    @Override public boolean onNavigationItemSelected(MenuItem menu_item) {
        log("onNavigationItemSelected"); //--strip
        int item_id = menu_item.getItemId();

        switch (item_id) {
            case R.id.mainmenu:
                Intent intent = new Intent(activity.this, main_activity.class);
                intent.setAction(Intent.ACTION_MAIN);
                intent.addCategory(Intent.CATEGORY_LAUNCHER);
                startActivity(intent);
                break;

            case R.id.nav_manage_connections:
                manage_connections();
                break;

            case R.id.nav_close:
                finish();
                break;

            case R.id.nav_about:
                launch_about();
                break;

            case R.id.nav_cancel:
                break;

        }

        if (!a.has_hmi()) {
            switch (item_id) {
                case R.id.nav_language:
                    log("nav_language"); //--strip
                    launch_language();
                    break;

                case R.id.nav_testleds:
                    log("nav_testleds"); //--strip
                    activity.a.led_test();
                    break;

                case R.id.nav_exit:
                    log("nav_exit"); //--strip
                    finishAffinity();
                    break;
                    //android.os.Process.killProcess(android.os.Process.myPid());
                    //System.exit(1);
            }
        }
        else {
            switch (item_id) {
                case R.id.nav_balance:
                    launch_position();
                    break;

                case R.id.nav_world:
                    world();
                    break;

                case R.id.nav_certs:
                    go_certs();
                    break;

                case R.id.nav_scanendpoint:
                    doscan(true);
                    break;

                case R.id.nav_showmyendpoint:
                    launch_myqr();
                    break;

                case R.id.nav_activetrades:
                    go_trades();
                    break;

                case R.id.nav_updateavailable:
                    if (a.has_hmi()) { // && a.hmi().sw_updates != null) {
                        if (a.hmi().sw_updates != null) {
                            a.hmi().sw_updates.show_ui(this);
                        }
                    }
                    break;

                case 5948:
                    Intent intent = new Intent(this, us.wearable_JClife.DeviceScanActivity.class);
                    startActivity(intent);
                    break;

                case R.id.nav_stophmi:
                    stop_hmi(true);
                    break;

                case R.id.nav_exit:
                    finishAffinity();
                    log("killing process in 1 sec"); //--strip
                    try {
                        java.lang.Thread.sleep(1000);
                    }
                    catch (Exception e) {
                    }
                    android.os.Process.killProcess(android.os.Process.myPid());
                    log("Exit 1"); //--strip
                    System.exit(1);
                    break;
            }
        }
        drawer_layout.closeDrawer(GravityCompat.START);
        return true;
    }

    void manage_connections() {
        a.assert_ui_thread(); //--strip
        log("launching connections..."); //--strip
        Intent intent = new Intent(activity.this, connections.class);
        startActivity(intent);        
    }

    public void go_conf(final int index) {
        a.assert_ui_thread(); //--strip
        log("launching connection settings..."); //--strip
        Intent intent = new Intent(activity.this, node_pairing.class);
        intent.putExtra("conf_index", index);
        startActivity(intent);
    }

    void new_connection() {
    }

    void toast(final String s) {
        log("Toast:" + s); //--strip
        app.assert_ui_thread(); //--strip
        Toast.makeText(getApplicationContext(), s, 6000).show();
    }

    void toast__worker(final String msg) {
        log("Toast__worker:" + msg); //--strip
        app.assert_worker_thread(); //--strip
        runOnUiThread(new Runnable() {
            @Override public void run() {
                toast(msg);
            }
        });
    }

    View.OnClickListener get_on_click_listener() {
        activity ac = this;
        return new View.OnClickListener() {
            @Override public void onClick(View v) {
                String[] options = {"Go to connection settings...", "Go to connections...", "Test leds.", (activity.a.is_HMI_poweron() ? "Stop HMI" : "Start HMI"), a.getResources().getString(R.string.cancel)};
                new AlertDialog.Builder(ac).setTitle("Quick access to...").setItems(options,new DialogInterface.OnClickListener() {
                    @Override public void onClick(DialogInterface dialog, int which) {
                        switch(which) {
                            case 0:
                                a.launch_connection_settings();
                                break;
                            case 1:
                                a.launch_connections();
                                break;
                            case 2:
                                a.led_test();
                                break;
                            case 3:
                                a.toggle_poweron();
                                break;
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

    public void report_ko(final String msg) {
        log("report_ko " + msg); //--strip
        app.assert_ui_thread(); //--strip
        Toast.makeText(this, msg, Toast.LENGTH_LONG).show();
    }

    public void report_ko__worker(final String msg) {
        app.assert_worker_thread(); //--strip
        runOnUiThread(new Runnable() {
            @Override public void run() {
                report_ko(msg);
            }
        });
    }

    public void report_ko(final ko r) {
        app.assert_ui_thread(); //--strip
        final String msg = a.has_hmi() ? a.hmi().rewrite(r) : r.msg;
        report_ko(msg);
    }

    public void report_ko__worker(final ko r) {
        app.assert_worker_thread(); //--strip
        final String msg = a.has_hmi() ? a.hmi().rewrite(r) : r.msg;
        report_ko__worker(msg);
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
            @Override public void run() {
                resume_walletd_leds();
            }
        });
    }

    @Override public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        switch (requestCode) {
            case INSTALL_PACKAGES_REQUESTCODE:
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                }
                else {
                    Intent intent = new Intent(Settings.ACTION_MANAGE_UNKNOWN_APP_SOURCES);
                    intent.setData(Uri.parse("package:" + getPackageName()));
                    startActivity(intent);
                }
                break;
        }
    }

    @Override public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        log("onActivityResult " + requestCode + " " + resultCode); //--strip
        if (requestCode == INSTALL_PACKAGES_REQUESTCODE) {
            if (resultCode == RESULT_CANCELED) {
                toast(a.r_(R.string.installationcacnelledbyuser));
                return;
            }
            if (resultCode != RESULT_OK) {
                log("not ok"); //--strip
                return;
            }
            if (a.has_hmi()) { // && a.hmi().sw_updates != null) {
                if (a.getPackageManager().canRequestPackageInstalls()) {
                    if (a.hmi().sw_updates != null) {
                        a.hmi().sw_updates.do_inst_local(this);
                    }
                }
            }
            else {
                log("sw updates not available"); //--strip
            }
            return;
        }
        if (requestCode == SCAN_REQUESTCODE) {
            if (resultCode == RESULT_CANCELED) {
                toast("Scan cancelled by user");
                return;
            }
            if (resultCode != RESULT_OK) {
                log("not ok"); //--strip
                return;
            }
            log("GO QR: " + data.getStringExtra("go_qr")); //--strip
            qr_t go_qr = new qr_t();
            go_qr.from_string(data.getStringExtra("go_qr"));
            a.new_trade(new hash_t(0), "", go_qr);
            toast("A new trade has been added to active_trades.");  //--strip
        }
    }

    public File screenshot(String filename) {
        View view = getWindow().getDecorView().getRootView();

        Date date = new Date();
          
        // Here we are initialising the format of our image name
        CharSequence format = android.text.format.DateFormat.format("yyyy-MM-dd_hh:mm:ss", date);
        try {
            // Initialising the directory of storage
            String dirpath = Environment.getExternalStorageDirectory() + "";
            File file = new File(dirpath);
            if (!file.exists()) {
                boolean mkdir = file.mkdir();
            }
            String path = dirpath + "/" + filename + "-" + format + ".png";
            view.setDrawingCacheEnabled(true);
            Bitmap bitmap = Bitmap.createBitmap(view.getDrawingCache());
            view.setDrawingCacheEnabled(false);
            File imageurl = new File(path);
            FileOutputStream outputStream = new FileOutputStream(imageurl);
            bitmap.compress(Bitmap.CompressFormat.PNG, 50, outputStream);
            outputStream.flush();
            outputStream.close();
            return imageurl;
  
        } catch (FileNotFoundException io) {
            io.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }
    static final int SCAN_REQUESTCODE = 3720;
    static final int INSTALL_PACKAGES_REQUESTCODE = 1791;

    public static app a = null;
    public static boolean leds_visible = true;
    public static boolean leds_visible_gov = false;
    public static boolean leds_visible_wallet = true;

    leds_t walletd_leds = null;
    leds_t govd_leds = null;
    Toolbar toolbar;
    public NavigationView navigation;
    public DrawerLayout drawer_layout;

}

