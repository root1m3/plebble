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
import android.app.Activity;                                                                   // Activity
import android.widget.AdapterView;                                                             // AdapterView
import android.app.Application;                                                                // Application
import android.widget.ArrayAdapter;                                                            // ArrayAdapter
import java.util.ArrayList;                                                                    // ArrayList
import android.os.Build;                                                                       // Build
import android.os.Bundle;                                                                      // Bundle
import java.util.Collections;                                                                  // Collections
import android.graphics.Color;                                                                 // Color
import android.content.Context;                                                                // Context
import us.gov.socket.datagram;                                                                 // datagram
import static android.graphics.BitmapFactory.decodeResource;                                   // decodeResource
//import com.google.firebase.crashlytics.FirebaseCrashlytics;                                    // FirebaseCrashlytics
import android.widget.FrameLayout;                                                             // FrameLayout
import java.util.HashMap;                                                                      // HashMap
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import android.widget.ImageView;                                                               // ImageView
import static us.ko.*;                                                                         // *
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import android.content.Intent;                                                                 // Intent
import java.util.Iterator;                                                                     // Iterator
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import android.widget.LinearLayout;                                                            // LinearLayout
import java.util.Map;                                                                          // Map
import com.google.android.material.card.MaterialCardView;                                      // MaterialCardView
import android.view.Menu;                                                                      // Menu
import android.view.MenuItem;                                                                  // MenuItem
import us.pair;                                                                                // pair
import us.wallet.protocol;                                                                     // protocol
import java.util.Random;                                                                       // Random
import java.util.concurrent.locks.ReentrantLock;                                               // ReentrantLock
import android.widget.RelativeLayout;                                                          // RelativeLayout
import androidx.annotation.RequiresApi;                                                        // RequiresApi
import us.string;                                                                              // string
import android.annotation.SuppressLint;                                                        // SuppressLint
import android.widget.TextView;                                                                // TextView
import android.widget.Toast;                                                                   // Toast
import androidx.appcompat.widget.Toolbar;                                                      // Toolbar
import android.util.TypedValue;                                                                // TypedValue
import android.net.Uri;                                                                        // Uri
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View

public final class trades extends activity {

    private static void log(final String s) {       //--strip
        System.out.println("trades: " + s);         //--strip
    }                                               //--strip

    public static class adapter_t extends ArrayAdapter<trade> {

        public adapter_t(Activity ac, ArrayList<trade> data) {
            super(ac, R.layout.trade_list_item, data);
            this.ac = ac;
            inflater = (LayoutInflater)ac.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        @Override public View getView(int position, View view, ViewGroup parent) {
            log("adapter. getView"); //--strip;
            View vi = view;
            if (vi == null) {
                log("inflating"); //--strip
                vi = inflater.inflate(R.layout.trade_list_item, null, true);
            }
            TextView tvitem = vi.findViewById(R.id.the_item);
            TextView tvitem2 = vi.findViewById(R.id.the_item2);
            TextView tvitem3 = vi.findViewById(R.id.the_item3);
            //TextView badge_balloon = vi.findViewById(R.id.badge_balloon);
            ImageView img = vi.findViewById(R.id.img);
            trade tr = getItem(position);
            log("position " + position + (tr == null ? 0 : 1)); //--strip
            log("tid " + (tr.tid == null ? 0 : 1)); //--strip
            if (tr.tid == null) {
                img.setVisibility(View.GONE);
                LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
                params.setMargins(10,30,10,10);
                tvitem.setText(getContext().getResources().getString(R.string.therearenotrades));
                tvitem.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
                tvitem.setTextColor(Color.RED);
                tvitem.setLayoutParams(params);
                tvitem2.setVisibility(View.GONE);
                tvitem3.setVisibility(View.GONE);
            }
            else {
                tvitem.setText("trade id: "+tr.tid.encode());
                tvitem2.setText(tr.caption(2));
                tvitem3.setText(tr.caption(3));
                tvitem2.setVisibility(View.VISIBLE);
                tvitem3.setVisibility(View.VISIBLE);
            }

            app a = (app) ac.getApplication();
            /*
            if (a.notification_trades_id.contains(tr.tid)){
                badge_balloon.setVisibility(View.VISIBLE);
                badge_balloon.setText("new");
            }
            else {
                badge_balloon.setVisibility(View.INVISIBLE);
            }
            */
            log("/ adapter. getView"); //--strip;
            return vi;
        }

        private Activity ac;
        private LayoutInflater inflater = null;
        //private TextView badge_balloon;
    }

    @SuppressLint("ResourceAsColor")
    @Override protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        log("onCreate"); //--strip
        set_content_layout(R.layout.activity_trades);
        tip_card = findViewById(R.id.tip_card);
        tip_explain = findViewById(R.id.tip_explain);
        toolbar.setTitle(R.string.activetrades);
        lv = findViewById(R.id.listviewX);
        shit = new ArrayList<trade>();
        adapter = new adapter_t(this, shit);
        lv.setAdapter(adapter);
        tip_card.setVisibility(View.GONE);
        lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            public void onItemClick(AdapterView parentView, View childView, int position, long id) {
                trade tr = shit.get(position);
                log("onItemClick " + position); //--strip
                Intent data = new Intent();
                log("Open trade " + shit.get(position).tid.encode()); //--strip
                a.go_trade(shit.get(position).tid);
            }
        });
        toolbar_button refresh = findViewById(R.id.refresh);
        refresh.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                fetch();
            }
        });
        refresh.setVisibility(View.VISIBLE);
    }

    @Override public void onDestroy() {
       super.onDestroy();
        log("onDestroy"); //--strip
    }

    @Override public void onResume() {
       super.onResume();
        log("onResume"); //--strip
        fetch();
    }

    @Override public void onPause() {
       super.onPause();
        log("onPause"); //--strip
    }

    void on_trades(ko e, String payload) {
        log("received payload: "); //--strip
        final ArrayList<trade> al = new ArrayList<trade>();
        if (is_ko(e)) {
            log(e.msg + " " + a.hmi.rewrite(e)); //--strip
            Toast.makeText(this, a.hmi.rewrite(e), Toast.LENGTH_LONG).show();
        }
        else {
            log(">" + payload + "<");  //--strip
            final String[] lines = payload.split("\\r?\\n");
            for (String line: lines) {
                log("line: " + line); //--strip
                trade t = new trade(line);
                if (t.tid != null) {
                    log("t.tid " + t.tid.encode()); //--strip
                    al.add(t);
                }
            }
        }
        Collections.sort(al);
        lock.lock();
        try {
            adapter.addAll(al);
        }
        finally {
            lock.unlock();
        }
        log("Y"); //--strip
    }

    void fetch() {
        a.assert_ui_thread(); //--strip
        if (a.hmi == null) {
            log("Closing activity hmi is null"); //--strip
            finish();
            return;
        }
        if (a.hmi.rpc_peer == null) {
            log("Closing activity hmi is not connected"); //--strip
            finish();
            return;
        }
        lock.lock();
        try {
            adapter.clear();
        }
        finally {
            lock.unlock();
        }
        log("calling API list_trades"); //--strip
        string s = new string();
        ko r = a.hmi.rpc_peer.call_list_trades(s);
        on_trades(r, s.value);
    }

    static final int SCAN_RESULT = 911;
    no_scroll_list_view lv;
    ArrayList<trade> shit;
    adapter_t adapter = null;
    TextView tip_explain;
    MaterialCardView tip_card;
    ReentrantLock lock = new ReentrantLock();
}

