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

public final class position extends activity { //implements datagram_dispatcher_t.handler_t {

    static void log(final String line) {         //--strip
       CFG.log_android("position: " + line);     //--strip
    }                                            //--strip

    public static class adapter_t extends ArrayAdapter<cryptocurrency> {

        private activity activity_;
        private LayoutInflater inflater=null;
        //TextView walletdaddress;

        public adapter_t(activity ac, ArrayList<cryptocurrency> data) {
            super(ac, R.layout.balance_item, data);
            this.activity_ = ac;
            inflater = (LayoutInflater)activity_.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        @Override
        public View getView(int position, View view, ViewGroup parent) {
            // LayoutInflater inflater = context.getLayoutInflater();
            View vi=view;
            if (vi==null) {
                vi = inflater.inflate(R.layout.balance_item, null, true);
            }
            //TextView nemonic_tv = vi.findViewById(R.id.nemonic_tv);
            TextView token_address = vi.findViewById(R.id.token_address);
            TextView balance_tv = vi.findViewById(R.id.balance_tv);
            //ImageView currencyimg = vi.findViewById(R.id.currencyimg);
            //walletdaddress = vi.findViewById(R.id.walletd_address);
            cryptocurrency cripto = getItem(position);
            token_address.setText(cripto.get_token());
            //walletdaddress.setText(""); //trader_address()
            //currencyimg.setImageResource(getContext().getResources().getIdentifier("raw/" + cripto.get_logo(), null, getContext().getPackageName()));
            //nemonic_tv.setText(cripto.get_mnemonic());
            double finalbalance = cripto.get_amount();
            NumberFormat nFormat = NumberFormat.getInstance();
            nFormat.setGroupingUsed(true);
            if(cripto.get_decimals() > 0) {
                nFormat.setMaximumFractionDigits(2);
                nFormat.setMinimumFractionDigits(2);
                finalbalance = cripto.get_amount()/Math.pow(10, cripto.get_decimals());
            }
            balance_tv.setText(nFormat.format(finalbalance));
/*
            walletdaddress.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    String feedback =  getContext().getResources().getString(R.string.addresscopied);
                    setClipboard(getContext(), walletdaddress.getText().toString(), feedback);
                    Toast.makeText(getContext(), feedback, 6000).show();
                }
            });
*/
           View.OnClickListener lner=new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    String[] options = {"Send "+cripto.get_token()};
                    new AlertDialog.Builder(adapter_t.this.activity_).setTitle("Select action:")
                        .setItems(options, new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                switch(which) {
                                    case 0:
                                        adapter_t.this.activity_.main.transfer(cripto.get_token());
                                        break;
                                }
                            }
                        })
                    .show();
                }
            };
            //currencyimg.setOnClickListener(lner);
            token_address.setOnClickListener(lner);
            balance_tv.setOnClickListener(lner);


            return vi;
        }

        /*
        public String trader_address() {
            return ((app) activity_.getApplication()).hmi.trader_address();
        }
        */

        private void setClipboard(Context context, String text, String message) {
            if(android.os.Build.VERSION.SDK_INT < android.os.Build.VERSION_CODES.HONEYCOMB) {
                android.text.ClipboardManager clipboard = (android.text.ClipboardManager) context.getSystemService(Context.CLIPBOARD_SERVICE);
                clipboard.setText(text);
            } else {
                android.content.ClipboardManager clipboard = (android.content.ClipboardManager) context.getSystemService(Context.CLIPBOARD_SERVICE);
                android.content.ClipData clip = android.content.ClipData.newPlainText(message, text);
                clipboard.setPrimaryClip(clip);
            }
        }

    }

    AbsListView lv;
    private toolbar_button refresh;
    private toolbar_button cryptos;
    adapter_t adapter=null;
    //String[] shit = {};
    ArrayList<cryptocurrency> shit;
    RelativeLayout progressbarcontainer;
    HashMap<String,cryptocurrency> coinbookmarks=null;
    Toolbar toolbar;
    ListView listview;

    DrawerLayout drawerLayout;
    NavigationView navigationView;

    int dispatchid;

    @RequiresApi(api = Build.VERSION_CODES.O)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        try {
            super.onCreate(savedInstanceState);
            setContentView(R.layout.activity_position);

            Toolbar toolbar = findViewById(R.id.toolbar);
            setSupportActionBar(findViewById(R.id.toolbar));
//TODO review
//            drawerLayout = findViewById(R.id.drawer_layout);
//            ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(this, drawerLayout, toolbar, R.string.bookmarks, R.string.bookmarks);
//            drawerLayout.addDrawerListener(toggle);
//            toggle.syncState();
//            navigationView = findViewById(R.id.navigation_view);
//            navigationView.setNavigationItemSelectedListener(this);
//            Menu nav_menu = navigationView.getMenu();
//            MenuItem menuItem = nav_menu.findItem(R.id.nav_balance);
//            menuItem.setChecked(true);
            toolbar.setTitle(R.string.balance);

            progressbarcontainer = findViewById(R.id.progressbarcontainer);

            listview  = findViewById(R.id.listview);

            log("Filling Coin bookmarks - TODO well done"); //--strip
            coinbookmarks = new HashMap<String, cryptocurrency>();
            log("CoinBookmarks size: " + coinbookmarks.size()); //--strip

            shit=new ArrayList<cryptocurrency>();
            adapter = new adapter_t(this, shit);

            lv = findViewById(R.id.listview);
            lv.setAdapter(adapter);
            refresh = findViewById(R.id.refresh);
            //cryptos = findViewById(R.id.cryptocurrency);

            refresh.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    fetch();
                    //new RuntimeException("Test ACRA Crash");
                }
            });

            //final app a = (app)getApplication();
            //log("connect network-datagram hose");//--strip
            //dispatchid=a.datagram_dispatcher.connect_sink(this);
        }
        catch(Exception ex) {
            error_manager.manage(ex, ex.getMessage() + "    " + ex.toString());
            ex.printStackTrace();
            finish();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        log("onDestroy"); //--strip
        //log("disconnect network-datagram hose");//--strip
       // a.datagram_dispatcher.disconnect_sink(dispatchid);
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

/*
    @Override    //entry point. Datagrams from the wire are processed here, called for every arriving datagram
    public boolean dispatch(datagram d) { //returns true if the datagram has been handled
        log("dispatch svc "+d.service); //--strip
        switch(d.service) {
            case protocol.wallet_balance_response: {
                log("handling svc "+d.service); //--strip
                final String b=d.parse_string();
                setbalances(b);
                return true;
            }
        }
        log("unhandled svc "+d.service); //--strip
        return false;
    }
*/
/*
    @Override
    public void on_push(hash_t target_tid, uint16_t code, String payload) {
        switch(code.value) {
            case us.wallet.trader.trader_t.push_trade: {
                log("a new trade for me"); //--strip
                on_new_trade(target_tid);
                log("OK - Got it"); //--strip
                return; //block propagation
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

    @Override
    public void on_push(hash_t target_tid, uint16_t code, byte[] payload) {
    }
*/
    void setbalances(final String b) {
        app.assert_worker_thread(); //--strip

        final String[] ashit=b.split("\\r?\\n");

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                shit.clear();
                if (b.startsWith("KO")) {
                    if (b.startsWith("KO 2018 ")) {
                        cryptocurrency c = new cryptocurrency();
                        c.set_mnemonic(getResources().getString(R.string.nocoinsinwallet));
                        c.set_amount(0);
                        c.set_decimals(0);
                        c.set_token("-");
                        shit.add(c);
                    }
                    else {
                        cryptocurrency c = new cryptocurrency();
                        c.set_mnemonic(b.substring(0,7));
                        c.set_amount(0);
                        c.set_decimals(0);
                        c.set_token("-");
                        shit.add(c);
                    }
                }
                else {
                    for (String itm : ashit) {
                        String[] txts = itm.split("\\s");
                        cryptocurrency c = coinbookmarks.get(txts[0]);
                        if (c == null) {
                            c = new cryptocurrency(txts[0], txts[0], 0, 0, "default_coin");
                        }
                        c.set_amount(Double.parseDouble(txts[1]));
                        shit.add(c);
                    }
                }
                refresh();
            }
        });
    }

    void fetch() {
        app.assert_ui_thread(); //--strip
        progressbarcontainer.setVisibility(View.VISIBLE);
        listview.setVisibility(View.GONE);
        adapter.clear();
        adapter.notifyDataSetChanged();
        Thread thread = new Thread(new Runnable() {
            public void run() {
                final app a = (app) getApplication();
                string s = new string();
                ko r = a.hmi.rpc_peer.call_balance(new uint16_t(0), s);
                if (is_ko(r)) {
                    return;
                }
                setbalances(s.value);
            }
        });
        thread.start();
    }

    void refresh() {
        app.assert_ui_thread(); //--strip
        adapter.notifyDataSetChanged();
        progressbarcontainer.setVisibility(View.GONE);
        listview.setVisibility(View.VISIBLE);
    }
}

