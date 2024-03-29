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
import android.widget.CompoundButton;                                                          // CompoundButton
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
import us.string;                                                                              // string
import android.view.SubMenu;                                                                   // SubMenu
import android.annotation.SuppressLint;                                                        // SuppressLint
import android.widget.Switch;                                                                  // Switch
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
import android.widget.ImageButton;

public final class connections extends activity {

    private static void log(final String line) {           //--strip
        CFG.log_android("connections: " + line);           //--strip
    }                                                      //--strip

    public connections() {
    }

    public static class adapter_t extends ArrayAdapter<device_endpoint_t> {

        private connections activity_;
        private LayoutInflater inflater = null;

        public adapter_t(connections ac, ArrayList<device_endpoint_t> data) {
            super(ac, R.layout.device_endpoint, data);
            this.activity_ = ac;
            inflater = (LayoutInflater)activity_.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        static final int pwr_off = Color.parseColor("#a0a0a0");
        static final int pwr_on = Color.parseColor("#3399ff");

        static int darkgreen = Color.parseColor("#009900");
        static int orange = Color.parseColor("#ffa500");


        @Override public View getView(int position, View view, ViewGroup parent) {
            View vi = view;
            if (vi == null) {
                vi = inflater.inflate(R.layout.device_endpoint, null, true);
            }
            ImageButton b = vi.findViewById(R.id.hmibutton);
            TextView label = vi.findViewById(R.id.label);
            TextView ssid = vi.findViewById(R.id.ssid);
            TextView address = vi.findViewById(R.id.address);
            Switch poweron = vi.findViewById(R.id.poweron);
            device_endpoint_t itm = getItem(position);
            String caption = itm.get_title();
            //String subcaption = itm.get_subtitle();
            String subhome = itm.get_subhome();
            String addr_ = itm.get_address();
            String ssid_ = itm.get_ssid();
            
            label.setBackgroundColor(pwr_off);
            poweron.setOnCheckedChangeListener(null);
            if (itm.hmi == null) {
                poweron.setChecked(false);
            }        
            else {
                poweron.setChecked(true);
            }

            poweron.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                    log("human touched power switch"); //--strip
                    if (isChecked) {
                        activity_.on_user_req_poweron(position);
                    }
                    else {
                        activity_.on_user_req_poweroff(position);
                    }
                }
            });

            if (itm.hmi != null) {
                label.setBackgroundColor(pwr_on);
            }
            label.setText(caption);
            if (itm.has_addr()) {
                if (subhome.isEmpty()) {
                    address.setText("Privacy wallet: " + addr_);
                    address.setTextColor(darkgreen);
                }
                else {
                    address.setText("Custodial wallet: " + addr_ + "." + subhome);
                    address.setTextColor(orange);
                }
                address.setVisibility(View.VISIBLE);
            }
            else {
                address.setVisibility(View.GONE);
            }
            if (!ssid_.isEmpty()) {
                ssid.setText("tied to wifi SSID: " + ssid_);
                ssid.setVisibility(View.VISIBLE);
            }
            else {
                ssid.setVisibility(View.GONE);
            }
            View.OnClickListener lner = new View.OnClickListener() {
                @Override public void onClick(View v) {
                    activity_.select_device_endpoint(position);
                }
            };
            b.setOnClickListener(lner);
            return vi;
        }


    }

/*
    @Override public int menuid() {
        if (!a.has_hmi()) {
            return R.menu.menu_nohmi;
        }
        else {
            return R.menu.menu_hmi_online;
        }
    }
*/

    void on_user_req_poweron(int position) {
        a.assert_ui_thread(); //--strip
        log("on_user_req_poweron"); //--strip
        app.progress_t progress = new app.progress_t() {
            @Override public void on_progress(final String report) {
                runOnUiThread(new Runnable() {
                    @Override public void run() {
                        Toast.makeText(connections.this, report, 6000).show();
                    }
                });
            }
        };
        a.HMI_power_on(position, new pin_t(0), progress);
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
        }, 1000);
    }

    void on_user_req_poweroff(int position) {
        a.assert_ui_thread(); //--strip
        log("on_user_req_poweroff"); //--strip
        app.progress_t progress = new app.progress_t() {
            @Override public void on_progress(final String report) {
                runOnUiThread(new Runnable() {
                    @Override public void run() {
                        Toast.makeText(connections.this, report, 6000).show();
                    }
                });
            }
        };
        a.HMI_power_off(position, true, progress);
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
        }, 1000);
    }

    @Override protected void onCreate(Bundle savedInstanceState) {
        log("onCreate"); //--strip
        super.onCreate(savedInstanceState);
        set_content_layout(R.layout.connections);
        setTheme(R.style.AppTheme);

        lv = findViewById(R.id.listview);
        lv.setVisibility(View.VISIBLE);

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
                log("refresh_btn onclick"); //--strip
                refresh();
            }
        });
        refresh_btn.setVisibility(View.VISIBLE);

        setTitle("Connections");
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
    }

    public void show_menu(final int pos, final String name) {
        final device_endpoint_t dep = a.device_endpoints.get(pos);

        String onoff_hmi;
        if (dep.hmi == null) {
            onoff_hmi = "Turn ON";
        }
        else {
            onoff_hmi = "Turn OFF";
        }
        String[] options;
//        if (CFG.default_wallet_connections.isEmpty()) {
        options = new String[]{"Open/Edit connection", onoff_hmi, "Delete connection", "Copy into a new connection", "Add default connection/s", a.getResources().getString(R.string.cancel)};
/*
        }
        else {
            String txt = "New custodial wallet.";
            if (!CFG.custodial_wallet_host.isEmpty()) {
                txt += " (hosted at " + CFG.custodial_wallet_host + ")";
            }
            options = new String[]{"Open/Edit connection", onoff_hmi, "Delete connection", "Copy into a new connection", txt, a.getResources().getString(R.string.cancel)};
        }
*/
        final connections i = this;
        new AlertDialog.Builder(this).setTitle(name).setItems(options, new DialogInterface.OnClickListener() {
            @Override public void onClick(DialogInterface dialog, int which) {
                switch(which) {
                    case 0:
                        i.select_device_endpoint2(pos);
                        break;

                    case 1:
                        if (dep.hmi != null) {
                            log("stop HMI"); //--strip
                            stop_hmi(true);
                        }
                        else {
                            log("start HMI"); //--strip
                            start_hmi(pos);
                        }
                        break;

                    case 2:
                        {
                            ko r = i.a.device_endpoints.erase(pos);
                            if (is_ko(r)) {
                                toast(r.msg);
                                break;
                            }
                            refresh();
                        }
                        break;
                    case 3:
                        {
                            ko r = i.a.device_endpoints.copy_device_endpoint(pos);
                            if (is_ko(r)) {
                                toast(r.msg);
                                break;
                            }
                            refresh();
                        }
                        break;

                    case 4:
                        {
                            if (CFG.default_wallet_connections.isEmpty()) {
                                toast("No default connections arte available");
                                break;
                            }
                            a.device_endpoints.add_default_wallet_connection2();
                            refresh();
                        }
                        break;

                    case 5:
                }
            }
        })
        .setIcon(R.drawable.ic_itemlist).show();
    }

    void select_device_endpoint2(int position) {
        go_conf(position);
    }

    void select_device_endpoint(int position) {
        show_menu(position, a.device_endpoints.get(position).get_title());
    }

    void new_connection() {
        if (a.device_endpoints == null) {
            toast("device_endpoints failure");
            return;
        }
        ko r = a.device_endpoints.new_endpoint();
        if (is_ko(r)) {
            toast(r.msg);
        }
        refresh();
    }

    AbsListView lv;
    adapter_t adapter = null;
}

