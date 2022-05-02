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
import com.google.firebase.crashlytics.FirebaseCrashlytics;                                    // FirebaseCrashlytics
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

public final class trades extends activity { //implements  datagram_dispatcher_t.handler_t {

    public static class adapter_t extends ArrayAdapter<trade> {

        private Activity ac;
        private LayoutInflater inflater = null;
        private TextView badge_balloon;

        public adapter_t(Activity ac, ArrayList<trade> data) {
            super(ac, R.layout.trade_list_item, data);
            this.ac = ac;
            inflater = (LayoutInflater)ac.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        @Override
        public View getView(int position, View view, ViewGroup parent) {
            log("adapter. getView"); //--strip;
            View vi=view;
            if (vi==null) {
                log("inflating"); //--strip
                vi = inflater.inflate(R.layout.trade_list_item, null, true);
            }
            TextView tvitem = vi.findViewById(R.id.the_item);
            TextView tvitem2 = vi.findViewById(R.id.the_item2);
            TextView tvitem3 = vi.findViewById(R.id.the_item3);
            TextView badge_balloon = vi.findViewById(R.id.badge_balloon);
            ImageView img = vi.findViewById(R.id.img);
            trade tr = getItem(position);
            log("position "+position+ (tr==null?0:1)); //--strip
            log("tid "+(tr.tid==null?0:1)); //--strip
            if (tr.tid==null) { //.equals(getContext().getResources().getString(R.string.therearenotrades))){
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
            if (a.notification_trades_id.contains(tr.tid)){
                badge_balloon.setVisibility(View.VISIBLE);
                badge_balloon.setText("new");
            }
            else {
                badge_balloon.setVisibility(View.INVISIBLE);
            }
            log("/ adapter. getView"); //--strip;
            return vi;
        }
    }


    static final int SCAN_RESULT = 911;
    no_scroll_list_view lv;
    ArrayList<trade> shit;
    adapter_t adapter=null;
    //int dispatchid;
//    trader_endpoint_t filter_node=null;
//    toolbar_button newtrade_btn;
    TextView tip_explain;
    MaterialCardView tip_card;
    Toolbar toolbar;
    RelativeLayout progressbarcontainer;
    ReentrantLock lock=new ReentrantLock();

    static void log(final String s) { //--strip
        System.out.println("trades: "+s); //--strip
    } //--strip

    @SuppressLint("ResourceAsColor")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        log("onCreate"); //--strip


        setContentView(R.layout.activity_trades);

        toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(findViewById(R.id.toolbar));
//        newtrade_btn = findViewById(R.id.newtrade_btn);
//        newtrade_btn.setVisibility(View.VISIBLE);
        tip_card = findViewById(R.id.tip_card);
        tip_explain = findViewById(R.id.tip_explain);
//TODO review
//        drawerLayout = findViewById(R.id.drawer_layout);
//        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(this, drawerLayout, toolbar, R.string.bookmarks, R.string.bookmarks);
//        drawerLayout.addDrawerListener(toggle);
//        toggle.syncState();
//        navigationView = findViewById(R.id.navigation_view);
//        navigationView.setNavigationItemSelectedListener(this);
//        Menu nav_menu = navigationView.getMenu();
//        MenuItem menuItem = nav_menu.findItem(R.id.nav_activetrades);
//        menuItem.setChecked(true);
        toolbar = findViewById(R.id.toolbar);
        toolbar.setTitle(R.string.activetrades);
        progressbarcontainer = findViewById(R.id.progressbarcontainer);
//        toolbar_button newtrade_btn = findViewById(R.id.newtrade_btn);
        lv = findViewById(R.id.listviewX);
        progressbarcontainer.setVisibility(View.VISIBLE);
        shit = new ArrayList<trade>();
        adapter = new adapter_t(this, shit);
        lv.setAdapter(adapter);
/*
        if (getIntent().hasExtra("filter_endpoint")) {
            filter_node = new trader_endpoint_t(getIntent().getExtras().getString("filter_endpoint"));
            if (filter_node.wloc==null) {
                filter_node=null;
                setTitle(R.string.allactivetrades);
                tip_card.setVisibility(View.GONE);
            }
            else {
                if (filter_node.wloc.first.is_zero()) {
                    filter_node=null;
                    setTitle(R.string.allactivetrades);
                    tip_card.setVisibility(View.GONE);
               }
                else {
                    setTitle(R.string.activetrades);
                    tip_card.setVisibility(View.VISIBLE);
                    tip_explain.setText("List is filtered by endpoint\n"+filter_node.endpoint());
                }
            }
        }
        else {
            filter_node=null;
*/
            setTitle(R.string.allactivetrades);
            tip_card.setVisibility(View.GONE);
            //newtrade_btn.setVisibility(View.GONE);
//        }
        //dispatchid=a.datagram_dispatcher.connect_sink(this);

        lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            public void onItemClick(AdapterView parentView, View childView, int position, long id) {
                progressbarcontainer.setVisibility(View.VISIBLE);
                trade tr = shit.get(position);
//                if (tr.tid==null) {
//                    newtrade();
//                }
//                else {
                    log("onItemClick " + position); //--strip
                    Intent data = new Intent();
//                    hash_t referrertid = new hash_t(data.getByteArrayExtra("referrer_tid"));
//                    log("referrertid " + referrertid.encode()); //--strip
                    log("Open trade " + shit.get(position).tid.encode()); //--strip
                    main.go_trade(shit.get(position).tid);
//                }
            }
        });
/*
        newtrade_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                newtrade();
            }
        });
*/
        toolbar_button refresh = findViewById(R.id.refresh);
        refresh.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                fetch();
            }
        });
    }

/*
    void newtrade() {
        Intent data = new Intent();
/ *
        if (filter_node!=null) {
            data.setData(Uri.parse("-"));
            data.putExtra("go_endpoint", filter_node.endpoint());
            log("go_endpoint " + filter_node.endpoint()); //--strip
        }
        else {
* /
        data.setData(Uri.parse("SCAN"));
//        }
        hash_t referrertid = new hash_t(data.getByteArrayExtra("referrer_tid"));
//        main.new_trade(filter_node, referrertid);
        main.new_trade(referrertid);
    }
*/

    @Override
    public void onDestroy() {
       super.onDestroy();
        log("onDestroy"); //--strip
        //a.datagram_dispatcher.disconnect_sink(dispatchid);
    }

    @Override
    public void onResume() {
       super.onResume();
        log("onResume"); //--strip
        fetch();
    }

    @Override
    public void onPause() {
       super.onPause();
        log("onPause"); //--strip
    }

    void on_trades(ko e, String payload) {
        log("received payload: "); //--strip
        final ArrayList<trade> al = new ArrayList<trade>();
        if (is_ko(e)) {
            log(e.msg); //--strip
            al.add(new trade(e.msg));
        }
        else {
            log(">" + payload + "<");  //--strip
            final String[] lines = payload.split("\\r?\\n");
            for (String line:lines) {
                log("line: "+line); //--strip
                trade t = new trade(line);
                if (t.tid != null) {
                    log("t.tid "+t.tid.encode()); //--strip
                    al.add(t);
/*
                    if (filter_node!=null) {
                        if (filter_node.wloc_equals(t.ep)) {
                            al.add(t);
                        }
                    }
                    else {
                        al.add(t);
                    }
*/
                }
            }
//            if (al.isEmpty()) {
//                al.add(new trade(getResources().getString(R.string.therearenotrades)));
//            }
        }
        Collections.sort(al);
        runOnUiThread(new Runnable() {
            public void run() {
                log("X");  //--strip
                lock.lock();
                try {
                    adapter.clear();
                    progressbarcontainer.setVisibility(View.GONE);
                    adapter.addAll(al);
                }
                finally {
                    lock.unlock();
                }
            }
        });
        log("Y"); //--strip
    }

/*
    @Override
    public boolean dispatch(datagram d) { //returns true if the datagram has been handled
        log("dispatch svc "+d.service.value); //--strip
        switch(d.service.value) {

            case protocol.wallet_list_trades_response: {
                log("handling svc "+d.service.value); //--strip
                final String b=d.parse_string();
                on_trades(b);
                return true;
            }

            case protocol.wallet_trade_response: {
                log("handling svc wallet_daemon_new_trade "+d.service.value); //--strip
                runOnUiThread(new Runnable() {
                    public void run() {
                        draw_fetch();
                    }
                });

                return true;
            }
        }
        log("unhandled svc "+d.service.value); //--strip
        return false;
    }
*/

    void fetch() {
        progressbarcontainer.setVisibility(View.VISIBLE);
        lock.lock();
        try {
            adapter.clear();
        }
        finally {
            lock.unlock();
        }
        log("calling API list_trades"); //--strip
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                string s = new string();
                ko r = a.hmi.rpc_peer.call_list_trades(s);
                on_trades(r, s.value);
            }
        });
        thread.start();


    }


}

