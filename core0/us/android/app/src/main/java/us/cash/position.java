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
import androidx.appcompat.app.ActionBar;                                                       // ActionBar
import android.app.Activity;                                                                   // Activity
import androidx.appcompat.app.AlertDialog;                                                     // AlertDialog
import android.widget.ArrayAdapter;                                                            // ArrayAdapter
import java.util.ArrayList;                                                                    // ArrayList
import android.graphics.BitmapFactory;                                                         // BitmapFactory
import android.graphics.Bitmap;                                                                // Bitmap
import android.os.Build;                                                                       // Build
import android.os.Bundle;                                                                      // Bundle
import android.content.Context;                                                                // Context
import us.gov.socket.datagram;                                                                 // datagram
import java.text.DecimalFormat;                                                                // DecimalFormat
import android.content.DialogInterface;                                                        // DialogInterface
import androidx.drawerlayout.widget.DrawerLayout;                                              // DrawerLayout
import android.widget.FrameLayout;                                                             // FrameLayout
import java.util.HashMap;                                                                      // HashMap
import us.gov.crypto.ripemd160.hash_t;                                                         // hash_t
import android.widget.ImageButton;                                                             // ImageButton
import android.widget.ImageView;                                                               // ImageView
import static us.ko.*;                                                                         // *
import static us.stdint.*;                                                                     // *
import java.io.InputStream;                                                                    // InputStream
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import android.widget.LinearLayout;                                                            // LinearLayout
import android.widget.ListView;                                                                // ListView
import android.view.Menu;                                                                      // Menu
import android.view.MenuItem;                                                                  // MenuItem
import com.google.android.material.navigation.NavigationView;                                  // NavigationView
import java.text.NumberFormat;                                                                 // NumberFormat
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import android.widget.ProgressBar;                                                             // ProgressBar
import us.wallet.protocol;                                                                     // protocol
import android.widget.RelativeLayout;                                                          // RelativeLayout
import androidx.annotation.RequiresApi;                                                        // RequiresApi
import us.string;                                                                              // string
import android.widget.TextView;                                                                // TextView
import android.widget.Toast;                                                                   // Toast
import androidx.appcompat.widget.Toolbar;                                                      // Toolbar
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View

public final class position extends activity {

    private static void log(final String line) {          //--strip
        CFG.log_android("position: " + line);             //--strip
    }                                                     //--strip

    public static class adapter_t extends ArrayAdapter<cryptocurrency> {

        private activity activity_;
        private LayoutInflater inflater = null;

        public adapter_t(activity ac, ArrayList<cryptocurrency> data) {
            super(ac, R.layout.balance_item, data);
            this.activity_ = ac;
            inflater = (LayoutInflater)activity_.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        @Override public View getView(int position, View view, ViewGroup parent) {
            View vi = view;
            if (vi == null) {
                vi = inflater.inflate(R.layout.balance_item, null, true);
            }
            TextView token_address = vi.findViewById(R.id.token_address);
            TextView balance_tv = vi.findViewById(R.id.balance_tv);
            cryptocurrency cripto = getItem(position);
            token_address.setText(cripto.get_token());
            double finalbalance = cripto.get_amount();
            NumberFormat nFormat = NumberFormat.getInstance(activity.a.main.getResources().getConfiguration().locale);
            nFormat.setGroupingUsed(true);
            if (cripto.get_decimals() > 0) {
                nFormat.setMaximumFractionDigits(2);
                nFormat.setMinimumFractionDigits(2);
                finalbalance = cripto.get_amount() / Math.pow(10, cripto.get_decimals());
            }
            balance_tv.setText(nFormat.format(finalbalance));
            return vi;
        }

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

    }

    @RequiresApi(api = Build.VERSION_CODES.O)
    @Override protected void onCreate(Bundle savedInstanceState) {
        log("OnCreate"); //--strip
        try {
            super.onCreate(savedInstanceState);
            set_content_layout(R.layout.activity_position);
            toolbar.setTitle(R.string.balance);
            log("Filling Coin bookmarks - TODO well done"); //--strip
            shit = new ArrayList<cryptocurrency>();
            adapter = new adapter_t(this, shit);
            lv = findViewById(R.id.listview);
            lv.setAdapter(adapter);
            refresh = findViewById(R.id.refresh);
            refresh.setVisibility(View.VISIBLE);
            refresh.setOnClickListener(new View.OnClickListener() {
                @Override public void onClick(View view) {
                    fetch();
                }
            });
        }
        catch(Exception ex) {
            error_manager.manage(ex, ex.getMessage() + "    " + ex.toString());
            ex.printStackTrace();
            finish();
        }
    }

    @Override public void onDestroy() {
        super.onDestroy();
    }

    @Override public void onResume() {
        super.onResume();
        log("onResume"); //--strip
        fetch();
    }

    @Override public void onPause() {
        super.onPause();
    }

    void set_balances(final String[] ashit) {
        app.assert_ui_thread(); //--strip
        shit.clear();
        for (String itm : ashit) {
            String[] txts = itm.split("\\s");
            cryptocurrency c = new cryptocurrency(txts[0], txts[0], 0, 0, "default_coin");
            c.set_amount(Double.parseDouble(txts[1]));
            shit.add(c);
        }
        refresh();
    }

    void fetch() {
        app.assert_ui_thread(); //--strip
        lv.setVisibility(View.GONE);
        adapter.clear();
        adapter.notifyDataSetChanged();
        final string s = new string();
        String r = a.balance(s);
        log("process response"); //--strip
        if (r != null) {
            log(r); //--strip
            Toast.makeText(getApplicationContext(), r, 6000).show();
            return;
        }
        final String[] ashit = s.value.split("\\r?\\n");
        set_balances(ashit);
    }

    @Override public void refresh() {
        app.assert_ui_thread(); //--strip
        if (!a.has_hmi()) {
            log("Closing activity hmi is null"); //--strip
            finish();
            return;
        }
        super.refresh();
        adapter.notifyDataSetChanged();
        lv.setVisibility(View.VISIBLE);
    }

    AbsListView lv;
    private toolbar_button refresh;
    private toolbar_button cryptos;
    adapter_t adapter = null;
    ArrayList<cryptocurrency> shit;
}

