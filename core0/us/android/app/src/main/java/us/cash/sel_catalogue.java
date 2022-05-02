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
import android.widget.AdapterView;                                                             // AdapterView
import android.widget.ArrayAdapter;                                                            // ArrayAdapter
import java.util.ArrayList;                                                                    // ArrayList
import us.gov.crypto.base58;                                                                   // base58
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import android.os.Build;                                                                       // Build
import android.os.Bundle;                                                                      // Bundle
import android.widget.Button;                                                                  // Button
import java.util.Collections;                                                                  // Collections
import android.content.Context;                                                                // Context
import us.wallet.trader.data_t;                                                                // data_t
import static us.gov.io.types.*;                                                               // *
import static us.ko.*;                                                                         // *
import android.content.Intent;                                                                 // Intent
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import android.widget.LinearLayout;                                                            // LinearLayout
import com.google.android.material.button.MaterialButton;                                      // MaterialButton
import android.view.Menu;                                                                      // Menu
import android.view.MenuItem;                                                                  // MenuItem
import android.widget.RelativeLayout;                                                          // RelativeLayout
import androidx.annotation.RequiresApi;                                                        // RequiresApi
import android.widget.TextView;                                                                // TextView
import androidx.appcompat.widget.Toolbar;                                                      // Toolbar
import android.net.Uri;                                                                        // Uri
import android.view.View;                                                                      // View

public class sel_catalogue extends activity {

    static void log(final String line) {         //--strip
       CFG.log_android("sel_catalogue: " + line);   //--strip
    }                                            //--strip

//TODO review
    void maproc() {
        //drawerLayout = ma.findViewById(R.id.drawer_layout);
//        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(main, main.drawerLayout, main.toolbar, R.string.bookmarks, R.string.bookmarks);
//        main.drawerLayout.addDrawerListener(toggle);
//        toggle.syncState();
        //ma.navigationView = findViewById(R.id.navigation_view);
//        ma.navigationView.setNavigationItemSelectedListener(this);
//        Toolbar toolbar = findViewById(R.id.toolbar);
//        setSupportActionBar(findViewById(R.id.toolbar));
    }


    @RequiresApi(api = Build.VERSION_CODES.O)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.sel_catalogue);


        maproc();

        toolbar = findViewById(R.id.toolbar);

//        Menu nav_menu = navigationView.getMenu();
        //MenuItem menuItem = nav_menu.findItem(R.id.nav_balance);
        //menuItem.setChecked(true);
        progressbarcontainer = findViewById(R.id.progressbarcontainer);
        LinearLayout content = findViewById(R.id.content);

        cancel = findViewById(R.id.cancel);
        Bundle bundle = getIntent().getExtras();
        toolbar.setTitle(bundle.getString("title"));
        byte[] cat_blob = bundle.getByteArray("cat_blob");
        catalogue_t.item_t item = new catalogue_t.item_t();
        log("cat_blob length = " + cat_blob.length); //--strip
        {
            ko r = blob_reader_t.parse(new blob_t(cat_blob), item);
            if (is_ko(r)) {
                log(r.msg); //--strip
                finish();
                return;
            }
        }
        log("item.name = " + item.name); //--strip
        log("item.doc null = " + (item.doc == null ? "NULL" : "not null")); //--strip
        if (!item.name.equals("cat")) {
            log("KO 20912"); //--strip
            finish();
        }

        LayoutInflater inflater = (LayoutInflater)getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        _basket_view = (basket_view) inflater.inflate(R.layout.basket, null);
        content.addView(_basket_view);

        us.wallet.trader.workflow.doc0_t doc = item.doc;
        if (doc == null) {
            log("KO 57643"); //--strip
            finish();
        }
        log("doc.params pay_coin " + doc.params.get("pay_coin")); //--strip
        log("doc.params reward_coin " + doc.params.get("reward_coin")); //--strip
        _basket_view.pay_coin = doc.params.get("pay_coin");
        _basket_view.reward_coin = doc.params.get("reward_coin");
        _basket_view.init2(this, new basket_view.on_click_listener2() {
                @Override
                public void on_select(basket_item_view v) {
                    log("on_select " + v.prod.encode()); //--strip
                    Intent data = new Intent();
                    data.putExtra("selection", v.prod.encode());
                    setResult(RESULT_OK, data);
                    finish();
                }
            }
        );
        /*
        String[] x = src.split(" ");
        if (x.length < 15) return;
        if (!x[0].equals("cat")) {
            return;
        }
        if (!x[5].equals("compact_form")) {
            return;
        }
        */
        String v = doc.params.get("compact_form");
        if (v == null) {
            log("KO 20913"); //--strip
            finish();
        }
        log("catalogue in compact form: " + v); //--strip
        blob_t blob = new blob_t(base58.decode(v));
        log("catalogue blob sz: " + blob.value.length); //--strip
        blob_reader_t reader = new blob_reader_t(blob);
        cat_t cat = new cat_t();
        {
            ko r = reader.read(cat);
            if (is_ko(r)) {
                finish();
                return;
            }
        }
        basket_t basket = new basket_t();
        basket.from(cat);
        log("basket from catalogue sz " + basket.size()); //--strip
        _basket_view.refresh(basket);


//        shit = new ArrayList<String>();

//        Collections.addAll(shit, sourceshit);
//        Collections.sort(shit);

//        ArrayAdapter adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, shit); //R.layout.trades_listitem

//        lv = (no_scroll_list_view) findViewById(R.id.listview);
//        lv.setAdapter(adapter);

/*
        lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            public void onItemClick(AdapterView parentView, View childView, int position, long id) {
//                String[] items=shit.get(position).split("\\s");
//                go_trade(items[0],items[1],items[2]);
                Intent data = new Intent();
                data.setData(Uri.parse(shit.get(position)));
                setResult(RESULT_OK, data);
                finish();

            }
        });
*/

        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                setResult(RESULT_CANCELED);
                finish();
            }
        });

        progressbarcontainer.setVisibility(View.GONE);

    }



//    String tradeid;
//    no_scroll_list_view lv;
//    ArrayList<String> shit;
    private MaterialButton cancel;
    RelativeLayout progressbarcontainer;
    Toolbar toolbar;
    basket_view _basket_view;
}

