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
import android.widget.ArrayAdapter;                                                            // ArrayAdapter
import java.util.ArrayList;                                                                    // ArrayList
import android.os.Bundle;                                                                      // Bundle
import android.content.Context;                                                                // Context
import us.gov.socket.datagram;                                                                 // datagram
import com.google.android.material.floatingactionbutton.FloatingActionButton;                  // FloatingActionButton
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import android.widget.ImageView;                                                               // ImageView
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import android.content.Intent;                                                                 // Intent
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import android.view.Menu;                                                                      // Menu
import android.view.MenuItem;                                                                  // MenuItem
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import android.widget.TextView;                                                                // TextView
import android.widget.Toast;                                                                   // Toast
import androidx.appcompat.widget.Toolbar;                                                      // Toolbar
import android.net.Uri;                                                                        // Uri
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View

public final class protocol_choose extends activity implements datagram_dispatcher_t.handler_t {

    static void log(final String line) {                 //--strip
        CFG.log_android("protocol_choose: " + line);     //--strip
    }                                                    //--strip

    private final static class ListViewItem_adapter extends ArrayAdapter<String> {

        public ListViewItem_adapter(Activity ac, ArrayList<String> data) {
            super(ac, R.layout.trade_list_item, data);
            this.activity = ac;
            inflater = (LayoutInflater)activity.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        @Override public View getView(int position, View view, ViewGroup parent) {
            View vi=view;
            if (vi == null) {
                vi = inflater.inflate(R.layout.trade_list_item, null, true);
            }
            TextView tvitem = (TextView)vi.findViewById(R.id.the_item);
            TextView tvitem2 = (TextView)vi.findViewById(R.id.the_item2);
            String tr = getItem(position);
            tvitem.setText("");
            tvitem2.setText(tr);
            ImageView imageView = (ImageView) vi.findViewById(R.id.img);
            imageView.setImageResource(R.drawable.ic_basket);
            return vi;
        }

        private Activity activity;
        private LayoutInflater inflater = null;
    }

    @Override protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        set_content_layout(R.layout.activity_trades);
//        Toolbar toolbar = findViewById(R.id.toolbar);
//        setSupportActionBar(findViewById(R.id.toolbar));
        shit = new ArrayList<String>();
        adapter = new ListViewItem_adapter(this,shit);
        lv = (no_scroll_list_view) findViewById(R.id.listviewX);
        if (lv == null) {           //--strip
            log(("lv is NULL"));    //--strip
        }                           //--strip
        lv.setAdapter(adapter);
        if (getIntent().hasExtra("tid")) {
            Bundle bundle = getIntent().getExtras();
            tid = new hash_t(bundle.getByteArray("tid"));
        }
        else {
            log("KO 8550 - missing tid"); //--strip
            finish();
        }
        lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            public void onItemClick(AdapterView parentView, View childView, int position, long id) {
                Intent data = new Intent();
                data.setData(Uri.parse(shit.get(position)));
                setResult(RESULT_OK, data);
                finish();
            }});
        FloatingActionButton refresh = (FloatingActionButton) findViewById(R.id.refresh);
        refresh.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                dorefresh();
            }
        });
        log("connect network-datagram hose");//--strip
        dispatchid = a.datagram_dispatcher.connect_sink(this);
    }

    @Override public void onDestroy() {
        super.onDestroy();
        log("onDestroy"); //--strip
        final app a = (app)getApplication();
        log("disconnect network-datagram hose");//--strip
        a.datagram_dispatcher.disconnect_sink(dispatchid);
    }

    @Override public void onResume() {
        super.onResume();
        log("onResume"); //--strip
        dorefresh();
    }

    @Override public void onPause() {
        super.onPause();
        log("onPause"); //--strip
    }

    @Override public void on_push(final hash_t target_tid, final uint16_t code, final byte[] payload) {
    }

    void dorefresh() {
        app.assert_ui_thread(); //--strip
        if (selected_protocol == null) {
            a.hmi.command_trade(tid, "show roles");
        }

    }

    no_scroll_list_view lv;
    ArrayList<String> shit;
    hash_t tid;
    ListViewItem_adapter adapter = null;
    String selected_protocol = null;
    int dispatchid;
}

