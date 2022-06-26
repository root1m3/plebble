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
import android.app.ActivityManager;                                                            // ActivityManager
import android.app.AlertDialog;                                                                // AlertDialog
import android.view.animation.AlphaAnimation;                                                  // AlphaAnimation
import android.view.animation.Animation;                                                       // Animation
import android.widget.ArrayAdapter;                                                            // ArrayAdapter
import java.util.ArrayList;                                                                    // ArrayList
import java.util.concurrent.atomic.AtomicInteger;                                              // AtomicInteger
import us.gov.crypto.base58;                                                                   // base58
import java.math.BigInteger;                                                                   // BigInteger
import us.wallet.trader.bookmarks_t;                                                           // bookmarks_t
import android.content.BroadcastReceiver;                                                      // BroadcastReceiver
import java.io.BufferedReader;                                                                 // BufferedReader
import android.os.Build;                                                                       // Build
import android.os.Bundle;                                                                      // Bundle
import android.widget.Button;                                                                  // Button
import java.nio.ByteOrder;                                                                     // ByteOrder
import android.content.res.Configuration;                                                      // Configuration
import android.net.ConnectivityManager;                                                        // ConnectivityManager
import androidx.core.content.ContextCompat;                                                    // ContextCompat
import android.content.Context;                                                                // Context
import us.gov.socket.datagram;                                                                 // datagram
import android.content.DialogInterface;                                                        // DialogInterface
import android.graphics.drawable.Drawable;                                                     // Drawable
import androidx.drawerlayout.widget.DrawerLayout;                                              // DrawerLayout
import us.wallet.trader.endpoint_t;                                                            // endpoint_t
import java.io.File;                                                                           // File
import java.io.FileInputStream;                                                                // FileInputStream
import java.io.FileNotFoundException;                                                          // FileNotFoundException
import androidx.fragment.app.Fragment;                                                         // Fragment
import androidx.fragment.app.FragmentManager;                                                  // FragmentManager
import androidx.fragment.app.FragmentTransaction;                                              // FragmentTransaction
import android.widget.FrameLayout;                                                             // FrameLayout
import androidx.core.view.GravityCompat;                                                       // GravityCompat
import android.os.Handler;                                                                     // Handler
import java.util.HashMap;                                                                      // HashMap
import us.gov.crypto.ripemd160.hash_t;                                                         // hash_t
import static us.stdint.*;                                                                     // *
import us.stdint.*;                                                                            // *
import us.tuple.*;                                                                             // *
import java.net.InetAddress;                                                                   // InetAddress
import java.io.InputStream;                                                                    // InputStream
import java.io.InputStreamReader;                                                              // InputStreamReader
import android.content.IntentFilter;                                                           // IntentFilter
import android.content.Intent;                                                                 // Intent
import java.io.IOException;                                                                    // IOException
import org.json.JSONArray;                                                                     // JSONArray
import org.json.JSONException;                                                                 // JSONException
import org.json.JSONObject;                                                                    // JSONObject
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import android.widget.LinearLayout;                                                            // LinearLayout
import java.util.List;                                                                         // List
import android.widget.ListView;                                                                // ListView
import java.util.Locale;                                                                       // Locale
import android.view.Menu;                                                                      // Menu
import android.view.MenuItem;                                                                  // MenuItem
import com.google.android.material.navigation.NavigationView;                                  // NavigationView
import android.net.NetworkInfo;                                                                // NetworkInfo
import androidx.annotation.NonNull;                                                            // NonNull
import android.app.NotificationChannel;                                                        // NotificationChannel
import androidx.core.app.NotificationCompat;                                                   // NotificationCompat
import androidx.core.app.NotificationManagerCompat;                                            // NotificationManagerCompat
import android.app.NotificationManager;                                                        // NotificationManager
import android.animation.ObjectAnimator;                                                       // ObjectAnimator
import static us.ko.ok;                                                                        // ok
import java.io.OutputStreamWriter;                                                             // OutputStreamWriter
import android.app.PendingIntent;                                                              // PendingIntent
import us.wallet.protocol;                                                                     // protocol
import us.wallet.trader.protocol_selection_t;                                                  // protocol_selection_t
import us.wallet.trader.qr_t;                                                                  // qr_t
import android.widget.RelativeLayout;                                                          // RelativeLayout
import androidx.annotation.RequiresApi;                                                        // RequiresApi
import android.net.wifi.ScanResult;                                                            // ScanResult
import android.provider.Settings;                                                              // Settings
import org.acra.data.StringFormat;                                                             // StringFormat
import android.annotation.SuppressLint;                                                        // SuppressLint
import androidx.core.app.TaskStackBuilder;                                                     // TaskStackBuilder
import android.widget.TextView;                                                                // TextView
import java.util.Timer;                                                                        // Timer
import java.util.TimerTask;                                                                    // TimerTask
import android.widget.Toast;                                                                   // Toast
import androidx.appcompat.widget.Toolbar;                                                      // Toolbar
import android.net.Uri;                                                                        // Uri
import android.animation.ValueAnimator;                                                        // ValueAnimator
import android.view.View;                                                                      // View
import android.net.wifi.WifiManager;                                                           // WifiManager
import android.view.Window;                                                                    // Window
import android.view.WindowManager;                                                             // WindowManager
import org.xmlpull.v1.XmlPullParser;                                                           // XmlPullParser
import android.view.MenuInflater;
import android.view.SubMenu;

public final class main_activity extends activity implements datagram_dispatcher_t.handler_t, NavigationView.OnNavigationItemSelectedListener {
    static final int AC_RESULT = 0;

    static void log(final String line) {            //--strip
       CFG.log_android("main_activity: " + line);   //--strip
    }                                               //--strip

    BroadcastReceiver wifiReceiver = new BroadcastReceiver() {
        @Override public void onReceive(Context context, Intent intent) {
            results = wifiManager.getScanResults();
            unregisterReceiver(this);
            for (ScanResult scanResult : results) {
                String output = scanResult.SSID + " - " + scanResult.capabilities;
            }
        };
    };

    @RequiresApi(api = Build.VERSION_CODES.O)
    @Override protected void onCreate(Bundle savedInstanceState) {
        log("main_activity.onCreate"); //--strip
        locale_init();
        overridePendingTransition(R.anim.fadein, R.anim.fadeout);
        setTheme(R.style.AppTheme);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN);
        setContentView(R.layout.activity_main);
        super.onCreate(savedInstanceState);
        mainprogressbarcontainer = findViewById(R.id.progressbarcontainer);
        toolbar = findViewById(R.id.toolbar);
        toolbar_button refresh = findViewById(R.id.refresh);
        drawer_layout = findViewById(R.id.drawer_layout);
        navigation = findViewById(R.id.navigation_view);
        toolbar = findViewById(R.id.toolbar);
        mainprogressbarcontainer.setVisibility(View.VISIBLE);
        refresh.setVisibility(View.GONE);
        toolbar.setTitle(""); //toolbar.setTitle(R.string.select_menu);
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

        log("Build.VERSION.SDK_INT=" + Build.VERSION.SDK_INT); //--strip
        createNotificationChannel();
        Context ctx = this;
        Window window = getWindow();
        window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
        window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
        window.setStatusBarColor(ContextCompat.getColor(this,R.color.colorPrimary));
        log("Connecting to datagram dispatcher");//--strip
        dispatchid = a.datagram_dispatcher.connect_sink(this);
        log("initating hmi in background");//--strip
        app.progress_t progress=new app.progress_t() {
            @Override public void on_progress(final String report) {
                runOnUiThread(new Runnable() {
                    @Override public void run() {
                        log("Toast hmi startup progress:"+ report); //--strip
                        Toast.makeText(main_activity.this, report, 6000).show();
                    }
                });
            }
        };
        main_ready(this);
        a.init_hmi(progress);
    }

    void on_hmi__worker(final ko status) {
        log("handling on_hmi__worker"); //--strip
        a.assert_worker_thread(); //--strip
        runOnUiThread(new Runnable() {
            public void run() {
                on_hmi(status);
            }
        });
    }

    void on_hmi(final ko status) {
        if (status == ok) {
            log("HMI is ok. Node is up"); //--strip
            a.sw_updates.start();
        }
        else {
            log("HMI couldn't start. " + status.msg); //--strip
            a.sw_updates.stop();
            if (!a.hmi.manual_mode) {
                launch_settings();
            }
        }
        mainprogressbarcontainer.setVisibility(View.GONE);
    }

    private animation_t animated_icon;

    public void launch_settings__worker() {
        a.assert_worker_thread(); //--strip
        runOnUiThread(new Runnable() {
            public void run() {
                launch_settings();
            }
        });
    }

    public void launch_settings() {
        a.assert_ui_thread(); //--strip
        log("launching settings..."); //--strip
        if (node_pairing.instances==1) {
            Toast.makeText(main_activity.this, r_(R.string.openingsettings), 6000).show();
        }
        Menu nav_menu = navigation.getMenu();
        MenuItem menuItem = nav_menu.findItem(R.id.nav_settings);
        onNavigationItemSelected(menuItem);
    }

    public void start_animation() {
        ValueAnimator animator = ObjectAnimator.ofInt(0, 360);
        animator.addUpdateListener(animation -> {
            animated_icon.set_rotation((int) animation.getAnimatedValue());
        });
        animator.start();
    }

    @Override public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        log("=========================== onRequestPermissionsResult " + requestCode); //--strip
    }

    public boolean _nodes_mode_all = true;
    public bookmarks_t _nodes_mode_custom = null;
    public hash_t _nodes_mode_custom_tid = null;

    public void show_remote_bookmarks(final hash_t tid, final bookmarks_t bm) {
        log("remote_bookmarks"); //--strip
        _nodes_mode_custom = bm;
        _nodes_mode_custom_tid = tid;
        Intent intent = new Intent(main_activity.this, nodes.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NO_HISTORY);
        startActivity(intent);
    }

    public void world() {
        log("menu world"); //--strip
        Intent intent = new Intent(main_activity.this, nodes.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NO_HISTORY);
        startActivity(intent);
    }

    @SuppressWarnings("StatementWithEmptyBody")
    @Override public boolean onNavigationItemSelected(MenuItem menu_item){
        int title;
        Drawable icon = menu_item.getIcon();
        animated_icon = new animation_t(a.getResources(), icon);
        menu_item.setIcon(animated_icon);
        start_animation();

        int item_id = menu_item.getItemId();

        if (!a.hmi.is_online) {
            if (item_id != R.id.nav_settings) {
                log("HMI is down. Opening settings instead"); //--strip
                item_id = R.id.nav_settings;
            }
        }

        Intent intent;
//        appinitlabel.setText(R.string.loading);
        mainprogressbarcontainer.setVisibility(View.VISIBLE);
        switch (item_id) {
            case R.id.nav_balance:
                //title = R.string.title_activity_position;
                intent = new Intent(main_activity.this, position.class);
                intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NO_HISTORY);
                startActivity(intent);
                break;

            case R.id.nav_world:
                world();
                break;

            case R.id.nav_scanendpoint:
                doscan(true);
                break;

            case R.id.nav_showmyendpoint:
                intent = new Intent(main_activity.this, endpoint.class);
                intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NO_HISTORY);
                intent.putExtra("prying_eyes", true);
                startActivity(intent);
                break;

            case R.id.nav_activetrades:
                go_trades();
                break;

            case R.id.nav_settings:
                intent = new Intent(main_activity.this, node_pairing.class);
                intent.addFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT | Intent.FLAG_ACTIVITY_NO_HISTORY);
                startActivity(intent);
                break;

            case R.id.nav_updateavailable:
                a.sw_updates.show_ui(this);
                break;

            case 5948:
                intent = new Intent(main_activity.this, us.wearable_JClife.DeviceScanActivity.class);
                intent.addFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT | Intent.FLAG_ACTIVITY_NO_HISTORY);
                startActivity(intent);

                break;

            case R.id.nav_exit:
                android.os.Process.killProcess(android.os.Process.myPid());
                System.exit(1);

            default:
                throw new IllegalArgumentException("menu option not implemented!!");
        }
        drawer_layout.closeDrawer(GravityCompat.START);
        return true;
    }

    private String CHANNEL_ID="app_notify_channel0";

    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            log("creating notification channel"); //--strip
            CharSequence name = "app_notify_channel0";
            String description = "app notify channel";
            int importance = NotificationManager.IMPORTANCE_DEFAULT;
            NotificationChannel channel = new NotificationChannel(CHANNEL_ID, name, importance);
            channel.setDescription(description);
            NotificationManager notificationManager = getSystemService(NotificationManager.class);
            notificationManager.createNotificationChannel(channel);
        }
        else {
            log("NOT creating notification channel. Build.VERSION.SDK_INT="+Build.VERSION.SDK_INT); //--strip
        }
    }

    private void show_notification(int code, final String info){
        Intent resultIntent = new Intent(this, trader.class);
        resultIntent.putExtra("tid", info);
        resultIntent.putExtra("from_notification", true);
        resultIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        String notifTitle = r_(R.string.newmessagefromtrade)+" " + info;
        String notifBody = r_(R.string.youhavenewmessagefromtrade)+" " + info;
        TaskStackBuilder stackBuilder = TaskStackBuilder.create(this);
        stackBuilder.addNextIntentWithParentStack(resultIntent);
        PendingIntent resultPendingIntent = stackBuilder.getPendingIntent(0, PendingIntent.FLAG_UPDATE_CURRENT);

        NotificationCompat.Builder builder = new NotificationCompat.Builder(this, CHANNEL_ID)
                .setSmallIcon(R.drawable.ic_notification_24dp)
                .setContentTitle(notifTitle)
                .setContentText(notifTitle)
                .setStyle(new NotificationCompat.BigTextStyle().bigText(notifBody))
                .setContentIntent(resultPendingIntent)
                .setPriority(NotificationCompat.PRIORITY_DEFAULT);

        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(this);
        AtomicInteger c = new AtomicInteger(0);
        int NOTIFICATION_ID = c.incrementAndGet();
        notificationManager.notify(NOTIFICATION_ID, builder.build());
        if (!a.notification_trades_id.contains(info)) a.notification_trades_id.add(info);
    }

    @Override
    protected void onStart() {
        super.onStart();
        log("onStart"); //--strip
    }

    @Override
    protected void onStop() {
        super.onStop();
        log("onStop"); //--strip
    };

    @Override
    public void onDestroy() {
        super.onDestroy();
        log("onDestroy"); //--strip
        wait_handler.removeCallbacksAndMessages(null); //Remove all the callbacks otherwise navigation will execute even after activity is killed or closed.
    }

    @Override
    public void onResume() {
        super.onResume();
        if (!first) {
            mainprogressbarcontainer.setVisibility(View.GONE);
        }
        else {
            first = false;
        }
        log("onResume"); //--strip
    }

    @Override
    public void onPause() {
        super.onPause();
        log("onPause"); //--strip
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
                    @Override
                    public void run() {
                        call_human(1, target_tid.encode());
                    }
                });
            }
            break;
        }
    }

    void call_human(int code, final String info) {
        switch(code) {
            case 1: //trade #tid received a chat update
                String tid=info;
                log("CALL_HUMAN "+code+" "+info); //--strip
                show_notification(1, info);
                //TODO: ... Show ! icon on every view there a tid is shown
            break;
       }
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        log("onConfigurationChanged"); //--strip
        //updateControls();
    }

    public static boolean isAndroidRuntime() {
        final String runtime = System.getProperty("java.runtime.name");
        log("runtime=" + runtime); //--strip
        return runtime != null && runtime.equals("Android Runtime");
    }

    public static boolean is_integer(String str) {
        boolean ret = true;
        try {
            Long.parseLong(str);
        }
        catch (Exception e) {
            ret = false;
        }
        return ret;
    }

    String r_(int id) {
        return a.getResources().getString(id);
    }

    private String getLocalWifiIpAddress() {
        WifiManager wifiManager = (WifiManager) getApplicationContext().getSystemService(WIFI_SERVICE);
        int ipAddress = wifiManager.getConnectionInfo().getIpAddress();

        if (ByteOrder.nativeOrder().equals(ByteOrder.LITTLE_ENDIAN)) {
            ipAddress = Integer.reverseBytes(ipAddress);
        }

        byte[] ipByteArray = BigInteger.valueOf(ipAddress).toByteArray();

        String ipAddressString;
        try {
            ipAddressString = InetAddress.getByAddress(ipByteArray).getHostAddress();
        } catch (Exception ex) {
            ipAddressString = null;
        }

        return ipAddressString;
    }

    private boolean isConnectedViaWifi() {
        ConnectivityManager connectivityManager = (ConnectivityManager) a.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo mWifi = connectivityManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
        return mWifi.isConnected();
    }

    void doscan(boolean frommainmenu) {
        Intent intent = new Intent(main_activity.this, scan.class);
        intent.putExtra("what", 0);
        if (frommainmenu) {
            intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NO_HISTORY);
        }
        startActivityForResult(intent, AC_RESULT);
    }

    @Override
    public void onBackPressed() {
        mainprogressbarcontainer.setVisibility(View.GONE);
        if (drawer_layout.isDrawerOpen(GravityCompat.START)) {
            drawer_layout.closeDrawer(GravityCompat.START);
        } else {
            ActivityManager mngr = (ActivityManager) getSystemService( ACTIVITY_SERVICE );
            List<ActivityManager.RunningTaskInfo> taskList = mngr.getRunningTasks(10);
            if(taskList.get(0).numActivities == 1 && taskList.get(0).topActivity.getClassName().equals(this.getClass().getName())) {
                android.os.Process.killProcess(android.os.Process.myPid());
                System.exit(1);
            }
        }
        int count = getSupportFragmentManager().getBackStackEntryCount();
        if (count > 0) {
            getSupportFragmentManager().popBackStack();
        }
        super.onBackPressed();
    }

    @Override public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        log("onActivityResult " + requestCode + " " + resultCode); //--strip
        if (requestCode == sw_updates_t.INST_RESULT) {
            if (resultCode == RESULT_CANCELED) {
                Toast.makeText(main_activity.this, r_(R.string.installationcacnelledbyuser), 6000).show();
                return;
            }
            if (resultCode != RESULT_OK) return;
            if (a.getPackageManager().canRequestPackageInstalls()) {
                a.sw_updates.do_inst(this);
            }
            return;
        }
        if (requestCode != AC_RESULT) {
            log("not AC_RESULT"); //--strip
            return;
        }
        if (resultCode != RESULT_OK) {
            log("not ok"); //--strip
            return;
        }
        log("GO QR: " + data.getStringExtra("go_qr")); //--strip
        qr_t go_qr = new qr_t();
        go_qr.from_string(data.getStringExtra("go_qr"));
        new_trade(new hash_t(0), "", go_qr);
        Toast.makeText(main_activity.this, "A new trade has been added to active_trades.", 6000).show();  //--strip
        Menu nav_menu = navigation.getMenu();
        MenuItem menu_item = nav_menu.findItem(R.id.nav_activetrades);
        onNavigationItemSelected(menu_item);
    }

    void toast(final String s) {
        runOnUiThread(new Thread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(getApplicationContext(), s, 6000).show();
            }
        }));
    }

    public void go_trade__worker(final hash_t tid) {
        log("go_trade__worker " + tid.encode()); //--strip
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                go_trade(tid);
            }
        });
    }

    public void go_trade(final hash_t tid) {
        log("go_trade " + tid.encode());  //--strip
        Intent intent = new Intent(main_activity.this, trader.class);
        intent.putExtra("tid", tid.value);
        startActivity(intent);
    }

    public void go_conf(final String conf) {
        Intent intent = new Intent(main_activity.this, node_pairing.class);
        intent.putExtra("conf", conf);
        startActivity(intent);
    }

    public void go_trades() {
        Intent intent = new Intent(main_activity.this, trades.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NO_HISTORY);
        startActivity(intent);
    }

    public void new_trade(final hash_t parent_trade, final String datasubdir, final qr_t qr) {
        log("new_trade " + qr.to_string()); //--strip
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                a.hmi.new_trade(parent_trade, datasubdir, qr);
                log("invoked API new_trade"); //--strip
            }
        });
        thread.start();
        Toast.makeText(main_activity.this, r_(R.string.newtrade) + "\n" + qr.to_string(), 600).show();
    }

    boolean isTxValid(String tx) {
        if (tx.isEmpty()) return false;
        if (tx.startsWith("KO")) {
            return false;
        }
        return true;
    }

    private void scanWifiNetworks() {
//        arrayList.clear();
//        registerReceiver(wifiReceiver, new IntentFilter(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION));
//        wifiManager.startScan();
//        Toast.makeText(this, "Scanning WiFi ...", Toast.LENGTH_SHORT).show();

        /*
        StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
        StrictMode.setThreadPolicy(policy);

        if (isAndroidRuntime()) new linux_secure_random(); //Asserts /dev/urandom is ok

        try {
            //InetAddress localhost = InetAddress.getLocalHost();
            InetAddress localhost = InetAddress.getByName(getLocalWifiIpAddress());
            byte[] ip = localhost.getAddress();

            for (int i = 1; i <= 254; i++) {
                try {
                    ip[3] = (byte) i;
                    InetAddress address = InetAddress.getByAddress(ip);
                    if (address.isReachable(100)) {
                        String output = address.toString().substring(1) + "  " + address.getCanonicalHostName() + "|" + address.getHostName();
                        System.out.print(output + " is on the network");
                    }
                } catch (Exception ex) {
                    //System.out.print("Exception:" + ex.getMessage());
                }
            }
        }catch(Exception iex){
            System.out.print("Exception:" + iex.getMessage());
            iex.printStackTrace();
        }
        */
    }

    public void transfer(String token) {
        log("===============================================TRANSFER"); //--strip
    }

    Handler wait_handler = new Handler();
    boolean first = true;
    RelativeLayout mainprogressbarcontainer;
    WifiManager wifiManager;
    ListView listView;
    Button buttonScan;
    int size = 0;
    List<ScanResult> results;
    Toolbar toolbar;
    private MenuItem updateavailable;
    public DrawerLayout drawer_layout;
    public NavigationView navigation;
    int dispatchid = -1;
    //smart_card_reader reader;

}

