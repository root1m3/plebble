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
import android.app.Activity;                                                                   // Activity
import android.widget.AdapterView;                                                             // AdapterView
import androidx.appcompat.app.AlertDialog;                                                     // AlertDialog
import java.util.ArrayList;                                                                    // ArrayList
import android.util.AttributeSet;                                                              // AttributeSet
import android.graphics.BitmapFactory;                                                         // BitmapFactory
import android.graphics.Bitmap;                                                                // Bitmap
import us.wallet.trader.bookmarks_t;                                                           // bookmarks_t
import us.wallet.trader.bookmark_t;                                                            // bookmark_t
import android.os.Bundle;                                                                      // Bundle
import android.content.Context;                                                                // Context
import us.gov.socket.datagram;                                                                 // datagram
import us.wallet.trader.data_t;                                                                // data_t
import android.content.DialogInterface;                                                        // DialogInterface
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import android.widget.ImageView;                                                               // ImageView
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import us.gov.crypto.ripemd160.*;                                                              // *
import us.stdint.*;                                                                            // *
import us.tuple.*;                                                                             // *
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import android.widget.LinearLayout;                                                            // LinearLayout
import java.util.Map;                                                                          // Map
import com.google.android.material.card.MaterialCardView;                                      // MaterialCardView
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import android.widget.TextView;                                                                // TextView
import android.widget.Toast;                                                                   // Toast
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View

public class redirects_view extends LinearLayout {

    static void log(final String s) {           //--strip
        System.out.println("tip_view: " + s);   //--strip
    }                                           //--strip

    public redirects_view(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public void init(role_fragment rf_, Activity ac) {
        rf = rf_;

        card = findViewById(R.id.card);
        lv = (no_scroll_list_view) findViewById(R.id.listviewX);
        assert lv != null;
        if (redirects == null) redirects = convert(new bookmarks_t());
        adapter = new nodes.adapter_t(ac, redirects);
        lv.setAdapter(adapter);
        lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView parentView, View childView, int position, long id) {
                item_click2(redirects.get(position), position);
            }
        });

        lv.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
            @Override
            public boolean onItemLongClick(AdapterView parentView, View childView, int position, long id) {
                return true;
            }
        });
        set_redirects(new bookmarks_t());
    }

    ArrayList<pair<String, bookmark_t>> convert(bookmarks_t bm) {
        ArrayList<pair<String, bookmark_t>> o = new ArrayList<pair<String, bookmark_t>>();
        o.ensureCapacity(bm.size());
        for (Map.Entry<String, bookmark_t> entry : bm.entrySet()) {
            o.add(new pair<String, bookmark_t>(entry.getKey(), entry.getValue()));
        }
        return o;
    }

    void item_click2(pair<String, bookmark_t> bm, final int pos) {
        app.assert_ui_thread();  //--strip
        final role_fragment rf0 = rf;
        final app a = rf.a;
        String[] options = {a.getResources().getString(R.string.start_new_child_trade), "Copy to my bookmarks", a.getResources().getString(R.string.cancel)};
        new AlertDialog.Builder(rf.tr).setTitle(bm.second.get_label())
            .setItems(options, new DialogInterface.OnClickListener() {
                @Override public void onClick(DialogInterface dialog, int which) {
                    switch(which) {
                        case 0:
                            data_t data = rf.tr.get_data();
                            if (data == null) {
                                Toast.makeText(rf.tr.getApplicationContext(), "KO 87986 Trade data not available.", 6000).show();
                                return;
                            }
                            hash_t parent_tid;
                            {
                                String st = data.find("parent_tid");
                                if (st == null) {
                                    parent_tid = new hash_t(0);
                                }
                                else {
                                    parent_tid = new hash_t(st);
                                }
                            }
                            log("new trade parent = " + parent_tid.encode()); //--strip
                            rf0.tr.a.new_trade(parent_tid, "", bm.second.qr);
                            break;
                        case 1:
                            log("Copy to my bookmarks"); //--strip
                            rf0.a.hmi().command_trade(rf0.tr.tid, "copyredirect " + (pos + 1));

                            break;
                    }
                }
            })
            .setIcon(R.drawable.ic_world).show();
    }

    public void set_redirects(final bookmarks_t bookmarks) {
        redirects = convert(bookmarks);
        log("set redirects"); //--strip
        rf.tr.runOnUiThread(new Runnable() {
            @Override public void run() {
                refresh();
            }
        });
    }

    public boolean refresh() {
        log("refresh"); //--strip
        data_t data = rf.tr.get_data();
        assert data != null;
        String st = data.find("local__redirects");
        if (st != null) {
            if (st.equals("Y")) {
                card.setVisibility(View.VISIBLE);
                if (redirects.isEmpty()) {
                    rf.a.hmi().command_trade(rf.tr.tid, "show redirects");
                }
            }
            else {
                redirects.clear();
                card.setVisibility(View.GONE);
            }
        }
        adapter.clear();
        adapter.addAll(redirects);

        return true;
    }

    ArrayList<pair<String, bookmark_t>> redirects = null;
    no_scroll_list_view lv;
    nodes.adapter_t adapter = null;
    MaterialCardView card;
    role_fragment rf;

}

