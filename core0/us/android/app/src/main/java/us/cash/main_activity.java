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
import android.widget.AbsListView;                                                             // AbsListView
import androidx.appcompat.app.ActionBarDrawerToggle;                                           // ActionBarDrawerToggle
import android.app.ActivityManager;                                                            // ActivityManager
import android.app.AlertDialog;                                                                // AlertDialog
import android.widget.ArrayAdapter;                                                            // ArrayAdapter
import java.util.ArrayList;                                                                    // ArrayList
import java.util.concurrent.atomic.AtomicInteger;                                              // AtomicInteger
import us.gov.crypto.base58;                                                                   // base58
import java.math.BigInteger;                                                                   // BigInteger
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.wallet.trader.bookmarks_t;                                                           // bookmarks_t
import android.content.BroadcastReceiver;                                                      // BroadcastReceiver
import java.io.BufferedReader;                                                                 // BufferedReader
import android.os.Build;                                                                       // Build
import android.os.Bundle;                                                                      // Bundle
import android.widget.Button;                                                                  // Button
import java.nio.ByteOrder;                                                                     // ByteOrder
import android.graphics.Color;                                                                 // Color
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
import static us.gov.id.types.*;                                                               // *
import static us.gov.io.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import static us.stdint.*;                                                                     // *
import us.stdint.*;                                                                            // *
import us.tuple.*;                                                                             // *
import java.net.InetAddress;                                                                   // InetAddress
import java.io.InputStream;                                                                    // InputStream
import java.io.InputStreamReader;                                                              // InputStreamReader
import android.content.IntentFilter;                                                           // IntentFilter
import android.content.Intent;                                                                 // Intent
import java.io.IOException;                                                                    // IOException
import static us.ko.is_ko;                                                                     // is_ko
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
import android.view.MenuInflater;                                                              // MenuInflater
import android.view.MenuItem;                                                                  // MenuItem
import android.net.NetworkInfo;                                                                // NetworkInfo
import androidx.annotation.NonNull;                                                            // NonNull
import android.app.NotificationChannel;                                                        // NotificationChannel
import androidx.core.app.NotificationCompat;                                                   // NotificationCompat
import androidx.core.app.NotificationManagerCompat;                                            // NotificationManagerCompat
import android.app.NotificationManager;                                                        // NotificationManager
import static us.ko.ok;                                                                        // ok
import java.io.OutputStreamWriter;                                                             // OutputStreamWriter
import android.app.PendingIntent;                                                              // PendingIntent
import us.wallet.protocol;                                                                     // protocol
import us.wallet.trader.protocol_selection_t;                                                  // protocol_selection_t
import us.wallet.trader.qr_t;                                                                  // qr_t
import androidx.annotation.RequiresApi;                                                        // RequiresApi
import android.net.wifi.ScanResult;                                                            // ScanResult
import android.provider.Settings;                                                              // Settings
//import org.acra.data.StringFormat;                                                             // StringFormat
import us.string;                                                                              // string
import android.view.SubMenu;                                                                   // SubMenu
import android.annotation.SuppressLint;                                                        // SuppressLint
import androidx.core.app.TaskStackBuilder;                                                     // TaskStackBuilder
import android.widget.TextView;                                                                // TextView
import java.util.Timer;                                                                        // Timer
import java.util.TimerTask;                                                                    // TimerTask
import android.widget.Toast;                                                                   // Toast
import android.net.Uri;                                                                        // Uri
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View
import android.net.wifi.WifiManager;                                                           // WifiManager
import android.view.Window;                                                                    // Window
import android.view.WindowManager;                                                             // WindowManager
import org.xmlpull.v1.XmlPullParser;                                                           // XmlPullParser

public final class main_activity extends activity implements datagram_dispatcher_t.handler_t /*, device_endpoint_t.hmi_power_listener_t*/ {

    private static void log(final String line) {             //--strip
        CFG.log_android("main_activity: " + line);           //--strip
    }                                                        //--strip

    public static class adapter_t extends ArrayAdapter<device_endpoint_t> {

        private main_activity activity_;
        private LayoutInflater inflater = null;

        public adapter_t(main_activity ac, ArrayList<device_endpoint_t> data) {
            super(ac, R.layout.device_endpoint, data);
            this.activity_ = ac;
            inflater = (LayoutInflater)activity_.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        static final int pwr_off = Color.parseColor("#a0a0a0");
        static final int pwr_on = Color.parseColor("#0010BB");

        @Override public View getView(int position, View view, ViewGroup parent) {
            View vi = view;
            if (vi == null) {
                vi = inflater.inflate(R.layout.device_endpoint, null, true);
            }
            Button b = vi.findViewById(R.id.hmibutton);
            device_endpoint_t itm = getItem(position);
            String caption = itm.get_title();
            b.setBackgroundColor(pwr_off);
            if (itm.hmi != null) {
                caption = "[ON] " + caption;
                b.setBackgroundColor(pwr_on);
            }
            b.setText(caption);
            View.OnClickListener lner = new View.OnClickListener() {
                @Override public void onClick(View v) {
                    activity_.select_device_endpoint(position);
                }
            };
            b.setOnClickListener(lner);
            return vi;
        }

/*
        private void setClipboard(Context context, String text, String message) {
            if(android.os.Build.VERSION.SDK_INT < android.os.Build.VERSION_CODES.HONEYCOMB) {
                android.text.ClipboardManager clipboard = (android.text.ClipboardManager) context.getSystemService(Context.CLIPBOARD_SERVICE);
                clipboard.setText(text);
            }
            else {
                android.content.ClipboardManager clipboard = (android.content.ClipboardManager) context.getSystemService(Context.CLIPBOARD_SERVICE);
                android.content.ClipData clip = android.content.ClipData.newPlainText(message, text);
                clipboard.setPrimaryClip(clip);
            }
        }
*/

    }

    BroadcastReceiver wifiReceiver = new BroadcastReceiver() {
        @Override public void onReceive(Context context, Intent intent) {
            results = wifiManager.getScanResults();
            unregisterReceiver(this);
            for (ScanResult scanResult : results) {
                String output = scanResult.SSID + " - " + scanResult.capabilities;
            }
        };
    };

    @Override public int menuid() {
        if (a.hmi == null) {
            return R.menu.menu_nohmi;
        }
        else {
            return R.menu.menu_hmi_online;
        }
    }

    void tweak_menu() {
        if (a.device_endpoints == null) {
            toast("device_endpoints failure");
            return;
        }
        Menu nav_menu = navigation.getMenu();
        updateavailable = nav_menu.findItem(R.id.nav_updateavailable);
        if (updateavailable != null) {
            if (a.hmi.sw_updates.is_updateavailable) {
                updateavailable.setVisible(true);
            }
            else {
                updateavailable.setVisible(false);
            }
        }
        boolean showiot = true;
        //release builds (which don't incluce lines marked '--strip' ) doesn't show the experimental IoT menu.
        showiot = true; //--strip
        if (showiot && a.hmi != null) {
            nav_menu.add(R.id.group5, 5948, Menu.NONE, "IoT");
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.O)
    @Override protected void onCreate(Bundle savedInstanceState) {
        log("main_activity.onCreate"); //--strip
        super.onCreate(savedInstanceState);
        set_content_layout(R.layout.activity_main);

        log("locale_init"); //--strip
        locale_init();

        log("setTheme"); //--strip
        setTheme(R.style.AppTheme);

        //log("setSystemUiVisibility"); //--strip
        //getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN);
        //log("Build.VERSION.SDK_INT=" + Build.VERSION.SDK_INT); //--strip
        //Window window = getWindow();
        //window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
        //window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
        //window.setStatusBarColor(ContextCompat.getColor(this, R.color.colorPrimary));
/*
        log("initating hmi in background");//--strip

*/

        if (a.main != null) {
            throw new AssertionError("Unexpected main_activity");                          //--strip
        }
        a.main = this;



        //toolbar.setTitle("");
        log("Filling Coin bookmarks - TODO well done"); //--strip
        //coinbookmarks = new HashMap<String, cryptocurrency>();
        //log("CoinBookmarks size: " + coinbookmarks.size()); //--strip
        lv = findViewById(R.id.listview);
        lv.setVisibility(View.VISIBLE);

        //progressbarcontainer.setVisibility(View.GONE);

        toolbar_button new_btn = findViewById(R.id.new_btn);
        new_btn.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                new_connection();
            }
        });
        new_btn.setVisibility(View.VISIBLE);

        toolbar_button refresh_btn = findViewById(R.id.refresh);
        refresh_btn.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                toast("refresh");
                refresh();
            }
        });
        refresh_btn.setVisibility(View.VISIBLE);

        //refresh();

//        log("Connecting to datagram dispatcher");//--strip
//        dispatchid = a.hmi.dispatcher.connect_sink(this);

        //main_ready(this);
        //a.init_hmi(progress);
    }

    @Override public void onPause() {
        super.onPause();
        log("onPause"); //--strip
        if (a.hmi == null) {
            dispatchid = -1;
        }
        else {
            if (dispatchid != -1) {
                a.hmi.dispatcher.disconnect_sink(dispatchid);
            }
        }
    }

    @Override public void onResume() {
        super.onResume();
        log("onResume"); //--strip
        if (a.hmi == null) {
            dispatchid = -1;
        }
        else {
            log("Connecting to datagram dispatcher");//--strip
            dispatchid = a.hmi.dispatcher.connect_sink(this);
        }
    }

    @Override public void refresh() {
        a.assert_ui_thread(); //--strip
        super.refresh();
        if (a.device_endpoints == null) {
            toast("device_endpoints failure.");
            return;
        }
        log("refresh " + a.device_endpoints.size() + " item"); //--strip
        adapter = new adapter_t(this, a.device_endpoints);
        lv.setAdapter(adapter);
        adapter.notifyDataSetChanged();

        tweak_menu();
    }

    public void show_menu(final int pos, final String name) {
        String[] options = {"Select", "delete", a.getResources().getString(R.string.cancel)};
        final main_activity i = this;
        new AlertDialog.Builder(this).setTitle(name)
                .setItems(options, new DialogInterface.OnClickListener() {
                    @Override public void onClick(DialogInterface dialog, int which) {
                        switch(which) {
                            case 0:
                                i.select_device_endpoint2(pos);
                                break;
                            case 1:
                                ko r = i.a.device_endpoints.erase(pos);
                                if (is_ko(r)) {
                                    toast(r.msg);
                                }
                                else {
                                    refresh();
                                }
                                break;
                            case 3:
                        }
                    }
                })
                .setIcon(R.drawable.ic_itemlist).show();
    }

    void select_device_endpoint2(int position) {
        //a.set_hmi(position);
        go_conf(position);
    }


    void select_device_endpoint(int position) {
        show_menu(position, a.device_endpoints.get(position).get_title());
    }

/*
    void on_hmi__ui(final ko status) {
        a.assert_ui_thread(); //--strip
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
    }
*/




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


/*
    @Override public void on_power__worker() {
        a.assert_worker_thread();  //--strip
    }
*/

/*
    public void on_hmi_power_on__worker() {
        a.assert_worker_thread();  //--strip
        runOnUiThread(new Runnable() {
            @Override public void run() {
                set_menu(R.menu.menu_hmi_online);
                log("Connecting to datagram dispatcher");//--strip
                dispatchid = a.hmi.dispatcher.connect_sink(main_activity.this);
            }
        });
    }

    public void on_hmi_power_off__worker() {
        a.assert_worker_thread();  //--strip
        runOnUiThread(new Runnable() {
            @Override public void run() {
                set_menu(R.menu.menu_nohmi);
//                if (a.hmi != null) {
//                    a.hmi.dispatcher.disconnect_sink(dispatchid);
//                }
                dispatchid = -1;
            }
        });
    }
*/


    private String CHANNEL_ID = "app_notify_channel0";

/*
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
*/

/*

    public void launch_settings__worker() {
        a.assert_worker_thread(); //--strip
        runOnUiThread(new Runnable() {
            @Override public void run() {
                launch_settings();
            }
        });
    }
*/

    @Override protected void onStart() {
        super.onStart();
        log("onStart"); //--strip
    }

    @Override protected void onStop() {
        super.onStop();
        log("onStop"); //--strip
    };

    @Override public void onDestroy() {
        super.onDestroy();
        log("onDestroy"); //--strip
        a.main = null;
        //wait_handler.removeCallbacksAndMessages(null); //Remove all the callbacks otherwise navigation will execute even after activity is killed or closed.
    }

    @Override public void on_push(hash_t target_tid, uint16_t code, byte[] payload) {
        log("on_push " + target_tid.encode() + " code " + code.value + " payload BIN sz: " + payload.length + " bytes"); //--strip
        switch(code.value) {
            case us.wallet.trader.trader_t.push_trade: {
                log("a new trade for me"); //--strip
                a.go_trade__worker(target_tid);
                return;
            }
            case us.wallet.trader.trader_t.push_chat: {
                runOnUiThread(new Runnable() {
                    @Override public void run() {
                        a.call_human(1, target_tid.encode());
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


    void new_connection() {
        if (a.device_endpoints == null) {
            toast("device_endpoints failure");
            return;
        }
        a.device_endpoints.new_endpoint();
        refresh();
    }

    void toast__worker(final String msg) {
        runOnUiThread(new Runnable() {
            @Override public void run() {
                Toast.makeText(main_activity.this, msg, 6000).show();
            }
        });
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
        }
        catch (Exception ex) {
            ipAddressString = null;
        }
        return ipAddressString;
    }

    private boolean isConnectedViaWifi() {
        ConnectivityManager connectivityManager = (ConnectivityManager) a.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo mWifi = connectivityManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
        return mWifi.isConnected();
    }

    @Override public void onBackPressed() {
        if (drawer_layout.isDrawerOpen(GravityCompat.START)) {
            drawer_layout.closeDrawer(GravityCompat.START);
        }
        else {
            ActivityManager mngr = (ActivityManager) getSystemService( ACTIVITY_SERVICE );
            List<ActivityManager.RunningTaskInfo> taskList = mngr.getRunningTasks(10);
            if (taskList.get(0).numActivities == 1 && taskList.get(0).topActivity.getClassName().equals(this.getClass().getName())) {
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

    public void go_conf(final int index) {
        a.assert_ui_thread(); //--strip
        log("launching settings..."); //--strip
        Intent intent = new Intent(main_activity.this, node_pairing.class);
        intent.putExtra("conf_index", index);
        startActivity(intent);
    }

/*
    boolean isTxValid(String tx) {
        if (tx.isEmpty()) return false;
        if (tx.startsWith("KO")) {
            return false;
        }
        return true;
    }
*/

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

/*
    public void transfer(String token) {
        log("===============================================TRANSFER"); //--strip
    }
*/

    AbsListView lv;
    adapter_t adapter = null;


    //Handler wait_handler = new Handler();
    WifiManager wifiManager;
    //ListView listView;
    Button buttonScan;
    int size = 0;
    List<ScanResult> results;
    int dispatchid = -1;
    //smart_card_reader reader;
    private MenuItem updateavailable;


}

