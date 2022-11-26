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
import androidx.appcompat.app.AlertDialog;                                                     // AlertDialog
import android.widget.ArrayAdapter;                                                            // ArrayAdapter
import java.util.ArrayList;                                                                    // ArrayList
import android.graphics.BitmapFactory;                                                         // BitmapFactory
import android.graphics.Bitmap;                                                                // Bitmap
import us.wallet.trader.bookmarks_t;                                                           // bookmarks_t
import us.wallet.trader.bookmark_t;                                                            // bookmark_t
import android.os.Build;                                                                       // Build
import android.os.Bundle;                                                                      // Bundle
import java.util.Collections;                                                                  // Collections
import android.widget.CompoundButton;                                                          // CompoundButton
import android.content.Context;                                                                // Context
import androidx.appcompat.view.ContextThemeWrapper;                                            // ContextThemeWrapper
import us.gov.socket.datagram;                                                                 // datagram
import static android.graphics.BitmapFactory.decodeResource;                                   // decodeResource
import android.content.DialogInterface;                                                        // DialogInterface
import android.widget.EditText;                                                                // EditText
import us.wallet.trader.endpoint_t;                                                            // endpoint_t
import java.util.HashMap;                                                                      // HashMap
import us.gov.crypto.ripemd160.hash_t;                                                         // hash_t
import android.widget.ImageView;                                                               // ImageView
import static us.ko.*;                                                                         // *
import static us.stdint.*;                                                                     // *
import android.view.inputmethod.InputMethodManager;                                            // InputMethodManager
import android.text.InputType;                                                                 // InputType
import android.content.Intent;                                                                 // Intent
import static us.ko.is_ko;                                                                     // is_ko
import java.util.Iterator;                                                                     // Iterator
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import java.util.Map;                                                                          // Map
import android.view.Menu;                                                                      // Menu
import android.view.MenuItem;                                                                  // MenuItem
import android.view.View.OnFocusChangeListener;                                                // OnFocusChangeListener
import us.pair;                                                                                // pair
import us.wallet.protocol;                                                                     // protocol
import us.wallet.trader.qr_t;                                                                  // qr_t
import android.widget.RelativeLayout;                                                          // RelativeLayout
import androidx.annotation.RequiresApi;                                                        // RequiresApi
import us.string;                                                                              // string
import android.widget.Switch;                                                                  // Switch
import android.widget.TextView;                                                                // TextView
import android.widget.Toast;                                                                   // Toast
import androidx.appcompat.widget.Toolbar;                                                      // Toolbar
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View
//import us.wallet.engine.bookmark_index_t;
import com.google.android.material.tabs.TabLayout; 
import java.util.TreeMap;
import us.wallet.trader.protocol_selection_t;
import us.wallet.trader.bootstrap.protocols_t;

public final class nodes extends activity implements datagram_dispatcher_t.handler_t {

    static void log(final String line) {          //--strip
        CFG.log_android("nodes: " + line);        //--strip
    }                                             //--strip

    public static class adapter_t extends ArrayAdapter<pair<String, bookmark_t>> {
        private LayoutInflater inflater;

        public adapter_t(Activity ac, ArrayList<pair<String, bookmark_t>> data) {
            super(ac, R.layout.trade_list_item, data);
            log("adapter constructor"); //--strip
            inflater = (LayoutInflater)ac.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        private class view_holder {
            TextView subject;
            TextView tvitem;
            TextView tvitem2;
            ImageView image;
        }

        @Override public View getView(int position, View convert_view, ViewGroup parent) {
            view_holder holder = null;
            pair<String, bookmark_t> sbm = getItem(position);
            bookmark_t bm = sbm.second;
            if (convert_view == null) {
                convert_view = inflater.inflate(R.layout.node_list_item, null, true);
                holder = new view_holder();
                holder.subject = convert_view.findViewById(R.id.subject);
                holder.tvitem = convert_view.findViewById(R.id.the_item);
                holder.tvitem2 = convert_view.findViewById(R.id.the_item2);
                holder.image = convert_view.findViewById(R.id.img);
                convert_view.setTag(holder);
            }
            else {
                holder = (view_holder) convert_view.getTag();
            }
            holder.subject.setText(sbm.first);
            holder.tvitem.setText(bm.get_label());
            holder.tvitem2.setText(bm.qr.to_string());
            if (bm.ico == null || bm.ico.length == 0) {
                holder.image.setImageResource(R.drawable.ic_node_busy);
            }
            else {
                try {
                    Bitmap bmp = BitmapFactory.decodeByteArray(bm.ico, 0, bm.ico.length);
                    holder.image.setImageBitmap(Bitmap.createScaledBitmap(bmp, 64, 64, false));
                }
                catch(Exception e) {
                    holder.image.setImageResource(R.drawable.ic_node_busy);
                }
            }
            return convert_view;
        }
    }

    ArrayList<pair<String, bookmark_t>> convert(bookmarks_t bm) {
        ArrayList<pair<String, bookmark_t>> o = new ArrayList<pair<String, bookmark_t>>();
        o.ensureCapacity(bm.size());
        for (Map.Entry<String, bookmark_t> entry : bm.entrySet()) {
            o.add(new pair<String, bookmark_t>(entry.getKey(), entry.getValue()));
        }
        return o;
    }

    ArrayList<pair<String, bookmark_t>> convert(us.gov.io.types.vector_hash bm) {
        ArrayList<pair<String, bookmark_t>> o = new ArrayList<pair<String, bookmark_t>>();
        o.ensureCapacity(bm.size());
        for (hash_t i: bm) {
            try {
                endpoint_t ep = new endpoint_t(a.hmi().p.channel, i);
                o.add(new pair<String, bookmark_t>("wallet", new bookmark_t(ep)));
            }
            catch (Exception e) {
                error_manager.manage(e, e.getMessage() + "    " + e.toString());
                log("KO 6950 - Invalid endpoint "); //--strip
            }
        }
        return o;
    }

    boolean tabs_listener_disabled = false;

    @RequiresApi(api = Build.VERSION_CODES.O)
    @Override protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        set_content_layout(R.layout.activity_world);
        //mode = (Switch) findViewById(R.id.mode);
        //mode_cap = (TextView) findViewById(R.id.mode_cap);
        tabs = findViewById(R.id.tabs);
//        tabs.selectTab(tabs.get(a.main._nodes_mode_tab), true);

//        tabs.addTab(tabs.newTab().setText("Doctor"));

        //bookmarks = new ArrayList<pair<String, bookmark_t>>();
        //world = new ArrayList<pair<String, bookmark_t>>();
        lv = (no_scroll_list_view) findViewById(R.id.listviewX);
        assert lv != null;
        adapter = new adapter_t(this, new ArrayList<pair<String, bookmark_t>>());
        lv.setAdapter(adapter);
        lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override public void onItemClick(AdapterView parentView, View childView, int position, long id) {
                assert a.main != null;
                if (a.main._nodes_mode_custom != null) {
                    item_click2(bookmarks.get(position), position);
                    return;
                }
                if (a.main._nodes_mode_tab == 0 ) {
                    item_click(world.get(position));
                    return;
                }
                if (a.main._nodes_mode_tab == 1 ) {
                    item_click1(bookmarks.get(position));
                }
                else {
                    item_click1(r2r_bookmarks.get(position));
                }
            }
        });
        lv.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
            @Override public boolean onItemLongClick(AdapterView parentView, View childView, int position, long id) {
                if (a.main._nodes_mode_custom != null) {
                    return true;
                }
                if (a.main._nodes_mode_tab != 0 ) {
                    return true;
                }
                final EditText input = new EditText(parentView.getContext());
                input.setOnFocusChangeListener(new OnFocusChangeListener() {
                        @Override public void onFocusChange(View v, boolean hasFocus) {
                            input.post(new Runnable() {
                                @Override public void run() {
                                    InputMethodManager inputMethodManager= (InputMethodManager) nodes.this.getSystemService(Context.INPUT_METHOD_SERVICE);
                                    inputMethodManager.showSoftInput(input, InputMethodManager.SHOW_IMPLICIT);
                                }
                            });
                        }
                    });
                input.requestFocus();
                input.setSingleLine();
                AlertDialog dialog = new AlertDialog.Builder(new ContextThemeWrapper(nodes.this, R.style.myDialog))
                    .setTitle("Bookmark")
                    .setMessage("Enter label for qr " + world.get(position).second.qr.to_string())
                    .setView(input)
                    .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                            @Override public void onClick(DialogInterface dialog, int which) {
                                pair<String, bookmark_t> bm = world.get(position);
                                bm.second.label = String.valueOf(input.getText()).trim();
                                us.wallet.engine.rpc_peer_t.bookmark_add_in_t o = new us.wallet.engine.rpc_peer_t.bookmark_add_in_t(new string(bm.first), bm.second);
                                string s = new string();
                                ko r = a.hmi().rpc_peer.call_bookmark_add(o, s);
                                if (is_ko(r)) {             //--strip
                                    log(r.msg);             //--strip
                                }                           //--strip
                                else {                      //--strip
                                    log("added bookmark");  //--strip
                                }                           //--strip
                             }
                        }).setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                            @Override public void onClick(DialogInterface dialog, int which) {}
                        }).create();
                dialog.show();
                return true;
            }
        });

        toolbar_button refresh_btn = findViewById(R.id.refresh);
        refresh_btn.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                reload(true);
            }
        });
        refresh_btn.setVisibility(View.VISIBLE);


/*
        mode.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                log("TOUCHED ROLE VIEW"); //--strip
                if (isChecked) {
                    log("checked"); //--strip
                    a.main._nodes_mode_all = false;
               }
                else {
                    log("not checked"); //--strip
                    a.main._nodes_mode_all = true;
                }
                refresh();
            }
        });
*/

        tabs.addOnTabSelectedListener(new TabLayout.OnTabSelectedListener() {
            @Override public void onTabSelected(TabLayout.Tab tab) {
                if (tabs_listener_disabled) return;
                a.main._nodes_mode_tab = tab.getPosition();
                protocol_selection = (protocol_selection_t) tab.getTag();
                log("tab selected. position " + a.main._nodes_mode_tab); //--strip
                reload(false);
//                refresh();
            }

            @Override public void onTabUnselected(TabLayout.Tab tab) {
                log("tab unselected. position " + tab.getPosition()); //--strip
                if (tabs_listener_disabled) return;
                adapter.clear();
            }

            @Override public void onTabReselected(TabLayout.Tab tab) {
                log("tab reselected. position " + tab.getPosition()); //--strip
                if (tabs_listener_disabled) return;
                reload(true); //--strip
            }


        });


//        load_index();

/*
        //---------------------
        Thread th = new Thread(new Runnable() {
            @Override public void run() {
                load_protocols__worker();
            }
        });

        th.start();
*/
        reload(true);
    }


//    bookmark_index_t index = null;

    @Override public void onPause() {
        super.onPause();
        log("onPause"); //--strip
        if (!a.has_hmi()) {
            dispatchid = -1;
        }
        else {
            if (dispatchid != -1) {
                a.hmi().dispatcher.disconnect_sink(dispatchid);
            }
        }
    }

    @Override public void onResume() {
        super.onResume();
        log("onResume"); //--strip
        if (!a.has_hmi()) {
            dispatchid = -1;
        }
        else {
            log("Connecting to datagram dispatcher");//--strip
            dispatchid = a.hmi().dispatcher.connect_sink(this);
        }
    }


    @Override public void onDestroy() {
        super.onDestroy();
        log("onDestroy"); //--strip
        if (a.main == null) return;
        a.main._nodes_mode_custom = null;
    }

    void item_click(pair<String, bookmark_t> bm) {
        String[] options = {"Banking (move coins)", a.getResources().getString(R.string.start_new_trade), a.getResources().getString(R.string.cancel)};
        final nodes i = nodes.this;
        new AlertDialog.Builder(i).setTitle(bm.second.get_label())
            .setItems(options, new DialogInterface.OnClickListener() {
                @Override public void onClick(DialogInterface dialog, int which) {
                    switch(which) {
                        case 0:
                            qr_t qr = new qr_t(bm.second.qr);
                            qr.protocol_selection.first = "w2w";
                            qr.protocol_selection.second = "w";
                            log("selected " + qr.to_string()); //--strip
                            a.new_trade(new hash_t(0), "", qr);
                            break;
                        case 1:
                            a.new_trade(new hash_t(0), "", bm.second.qr);
                            break;
                    }
                }
            }).setIcon(R.drawable.ic_world).show();
    }

    void item_click1(pair<String, bookmark_t> bm) {
        String[] options = {a.getResources().getString(R.string.start_new_trade), "edit bookmark", "delete bookmark", a.getResources().getString(R.string.cancel)};
        final nodes i = nodes.this;
        new AlertDialog.Builder(i).setTitle(bm.second.get_label())
            .setItems(options, new DialogInterface.OnClickListener() {
                @Override public void onClick(DialogInterface dialog, int which) {
                    switch(which) {
                        case 0:
                            a.new_trade(new hash_t(0), "", bm.second.qr);
                            break;
                        case 1:
                            break;
                        case 2:
                            string ans = new string();
                            ko r = a.hmi().rpc_peer.call_bookmark_delete(new string(bm.first), ans);
                            Toast.makeText(a.main, ans.value, Toast.LENGTH_LONG).show();
                            refresh();
                            break;
                    }
                }
            }).setIcon(R.drawable.ic_world).show();
    }

    void item_click2(pair<String, bookmark_t> bm, final int pos) {
        String[] options = {a.getResources().getString(R.string.start_new_trade), "Copy to my bookmarks", a.getResources().getString(R.string.cancel)};
        final nodes i = nodes.this;
        new AlertDialog.Builder(i).setTitle(bm.second.get_label())
            .setItems(options, new DialogInterface.OnClickListener() {
                @Override public void onClick(DialogInterface dialog, int which) {
                    switch(which) {
                        case 0:
                            a.new_trade(new hash_t(0), "", bm.second.qr);
                            break;
                        case 1:
                            log("Copy to my bookmarks"); //--strip
                            a.hmi().command_trade(a.main._nodes_mode_custom_tid, "copybm " + (pos + 1));
                            break;
                    }
                }
            }).setIcon(R.drawable.ic_world).show();
    }

    @Override public void on_push(hash_t target_tid, uint16_t code, byte[] payload) {
    }

    int redraw_prev_content = -1;

    void redraw() {
        log("redraw tab=" + a.main._nodes_mode_tab); //--strip
        if (a.main._nodes_mode_custom == null && redraw_prev_content == a.main._nodes_mode_tab) {
            log("redraw aborted prev=" + redraw_prev_content + " cur " + a.main._nodes_mode_tab); //--strip
            return;
        }
        adapter.clear();
        if (a.main._nodes_mode_custom != null) {
            if (bookmarks == null) {
                log("bookmarks is null"); //--strip
                return;
            }
            adapter.addAll(bookmarks);
            return;
        }
        if (a.main._nodes_mode_tab == 0) {
            if (world == null) {
                log("world is null"); //--strip
                return;
            }
            adapter.addAll(world);
            log("redraw prev=" + redraw_prev_content + " cur " + a.main._nodes_mode_tab); //--strip
            redraw_prev_content = a.main._nodes_mode_tab;
            return;
        }
        if (a.main._nodes_mode_tab == 1) {
            if (bookmarks == null) {
                log("bookmarks is null"); //--strip
                return;
            }
            adapter.addAll(bookmarks);
            log("redraw prev=" + redraw_prev_content + " cur " + a.main._nodes_mode_tab); //--strip
            redraw_prev_content = a.main._nodes_mode_tab;
            return;
        }
        //bookmarks_t bm = (bookmarks_t)tabs.getTabAt(a.main._nodes_mode_tab).getTag();
        if (r2r_bookmarks == null) {
            log("r2r_bookmarks is null"); //--strip
            return;
        }
        adapter.addAll(r2r_bookmarks);
        log("redraw prev=" + redraw_prev_content + " cur " + a.main._nodes_mode_tab); //--strip
        redraw_prev_content = a.main._nodes_mode_tab;

    }

    void load_remote() {
        set_busy(true);
        log("load_remote " + a.main._nodes_mode_custom.size()); //--strip
        bookmarks = convert(a.main._nodes_mode_custom);
        set_busy(false);
    }

    void on_protocols() {
        a.assert_ui_thread(); //--strip
        refresh();
    }

    void on_r2r_bookmarks__worker(final bookmarks_t bookmarks) {
        a.assert_worker_thread(); //--strip
        log("on_r2r_bookmarks__worker " + bookmarks.size()); //--strip
        r2r_bookmarks = convert(bookmarks);
        if (a.main._nodes_mode_tab >= 2) {
            redraw_prev_content = -1;
        }

        runOnUiThread(new Runnable() {
            @Override public void run() {
                redraw();
            }
        });
    }

    void load_r2r_bookmarks__worker() {
        //assert protocol_selection != null; //--strip
        log("load_r2r_bookmarks__worker protocol_selection=" + protocol_selection.to_string()); //--strip
//        TabLayout.Tab tab = tabs.getTabAt(tabs.getSelectedTabPosition());
        bookmarks_t bm = new bookmarks_t();
        ko r = a.hmi().rpc_peer.call_r2r_bookmarks(protocol_selection, bm);
        log("returned from rpc call"); //--strip
        if (is_ko(r)) {
            report_ko__worker(r);
            return;
        }
        on_r2r_bookmarks__worker(bm);
    }

    protocol_selection_t cur_protocol_selection = null;

    void load_r2r_bookmarks() { //(bookmarks_t bm
        a.assert_ui_thread(); //--strip
        if (protocols == null) return;
        if (cur_protocol_selection == protocol_selection) {
            return;
        }
        cur_protocol_selection = protocol_selection;
        log("load_r2r_bookmarks"); //--strip
        set_busy(true);
        Thread thread1 = new Thread(new Runnable() {
            @Override public void run() {
                load_r2r_bookmarks__worker();
                set_busy__worker(false);
            }
        });
        thread1.start();
    }

    void on_protocols__worker(final protocols_t protocols_) {
        a.assert_worker_thread(); //--strip
        log("on_protocols__worker. sz=" + protocols_); //--strip
        protocols = protocols_;
        runOnUiThread(new Runnable() {
            @Override public void run() {
                on_protocols();
            }
        });

        r2r_bookmarks = null;
        if (a.main._nodes_mode_tab == 0) { 
        }
        else if (a.main._nodes_mode_tab == 1) { 
        }
        else {
            load_r2r_bookmarks__worker();
        }
    }

    void load_protocols__worker() {
        a.assert_worker_thread(); //--strip
        log("load_protocols__worker"); //--strip
        protocols_t protocols_ = new protocols_t();
        ko r = a.hmi().rpc_peer.call_r2r_index_hdr(protocols_);
        if (is_ko(r)) {
            set_busy__worker(false);
            report_ko__worker(r);
            return;
        }
        on_protocols__worker(protocols_);
    }


    void load_protocols() { //(bookmarks_t bm
        a.assert_ui_thread(); //--strip
        if (protocols != null) {
            redraw();
            return;
        }
        log("load_protocols"); //--strip
        set_busy(true);
        Thread thread1 = new Thread(new Runnable() {
            @Override public void run() {
                load_protocols__worker();
                set_busy__worker(false);
            }
        });
        thread1.start();
    }

    void load_bookmarks() {
        if (bookmarks != null) {
            redraw();
            return;
        }
        log("load_bookmarks 0args"); //--strip
        set_busy(true);
        Thread thread1 = new Thread(new Runnable() {
            @Override public void run() {
                log("load_bookmarks-run"); //--strip
                bookmarks_t bm = new bookmarks_t();
                ko r = a.hmi().rpc_peer.call_bookmark_list(bm);
                if (is_ko(r)) {
                    set_busy__worker(false);
                    report_ko__worker(r);
                    return;
                }
                bookmarks = convert(bm);
                log("OK " + bookmarks.size()); //--strip
                set_busy__worker(false);
            }
        });
        thread1.start();
    }

/*
    void load_r2r_bookmarks() {
        set_busy(true);
        Thread thread1 = new Thread(new Runnable() {
            @Override public void run() {
                log("load_r2rindex-run"); //--strip
                bookmark_index_t bm = new bookmark_index_t();
                ko r = a.hmi().rpc_peer.call_r2r_bookmarks(protocol_selection, bm);
                if (is_ko(r)) {
                    set_busy__worker(false);
                    log(r.msg); //--strip
                    report_ko__worker(r);
                    return;
                }
                //bookmarks = convert(bm);
                log("OK " + bookmarks.size()); //--strip
                set_busy__worker(false);
            }
        });
        thread1.start();
    }
*/
    void load_world() {
        if (world != null) {
            redraw();
            return;
        }
        set_busy(true);
        Thread thread1 = new Thread(new Runnable() {
            @Override public void run() {
                log("load_world-run"); //--strip
                us.gov.io.types.vector_hash vh = new us.gov.io.types.vector_hash();
                if (!a.has_hmi() || a.hmi().rpc_peer == null) {
                    ko r = new ko("KO 70699 HMI is not on"); //--strip
                    set_busy__worker(false);
                    log(r.msg); //--strip
                    return;
                }
                ko r = a.hmi().rpc_peer.call_world(vh);
                if (is_ko(r)) {
                    set_busy__worker(false);
                    return;
                }
                world = convert(vh);
                set_busy__worker(false);
                return;
            }
        });
        thread1.start();
    }

    void set_busy__worker(boolean b) {
        runOnUiThread(new Runnable() {
            @Override public void run() {
                set_busy(b);
            }
        });
    }

    void set_busy(boolean b) {
        if (b) {
            ++_busy;
            if (_busy == 1) {
                log("busy=1"); //--strip
                adapter.clear();
            }
            return;
        }
        --_busy;
        if (_busy == 0) {
            log("busy=0"); //--strip
            redraw();
        }
    }

    public static class role_metadata_t {
        public role_metadata_t(String title_) {
            title = title_;
        }
        String title;
        //int res;
    };

/*
    public static class roles_metadata_t extends TreeMap<protocol_selection_t, role_metadata_t> {
        roles_metadata_t() {
            put(new protocol_selection_t("pat2slt", "slt"), new role_metadata_t("Doctor"));
            put(new protocol_selection_t("pat2slt", "pat"), new role_metadata_t("Patient"));
//            emplace(new protocol_selection_t("pat2slt", "slt"), new role_metadata_t("Sports Club"));
//            emplace(new protocol_selection_t("pat2slt", "pat"), new role_metadata_t("Fan"));
        }

        String get_title(final protocol_selection_t protocol_selection) {
            // Get the value associated with a given key in a TreeMap
            role_metadata_t data = get(protocol_selection);
            if (data == null) {
                return protocol_selection.to_string();
            }
            return data.title;
        }
    };

    static roles_metadata_t roles_metadata = new roles_metadata_t();
*/

    void retab() {
        log("retab"); //--strip
        if (protocols == null) {
            tabs_listener_disabled = true;
            while (tabs.getTabCount() > 2) {
                tabs.removeTabAt(tabs.getTabCount() - 1);
            }
            tabs_listener_disabled = false;
            log("null protocols"); //--strip
            return;
        }
        if (tabs.getTabCount() > 2) {
            return;
        }
        log("retab " + protocols.size()); //--strip
        tabs_listener_disabled = true;
        for (protocol_selection_t i: protocols) {
//            bookmarks_t value = entry.getValue();
            TabLayout.Tab tab = tabs.newTab().setText(i.to_string());
            tab.setIcon(R.raw.r2rtab);
            tab.setTag(i);
            tabs.addTab(tab);
        }
        tabs_listener_disabled = false;
    }

    @Override public void refresh() {
        a.assert_ui_thread(); //--strip
        if (!a.has_hmi()) {
            log("Closing activity hmi is null"); //--strip
            finish();
            return;
        }
        super.refresh();
        if (a.main._nodes_mode_custom != null) {
            toolbar.setTitle("Listing remote bookmarks");
            tabs.setVisibility(View.GONE);
            load_remote();
            return;
        }
        retab();
        if (a.main._nodes_mode_tab >= tabs.getTabCount()) {
            if (CFG.default_view_bookmarks == 1) {
                a.main._nodes_mode_tab = 1;
            }
            else {
                a.main._nodes_mode_tab = 0;
            }
            log("reset tab to " + a.main._nodes_mode_tab); //--strip
        } 
        log("select tab " + a.main._nodes_mode_tab); //--strip
        tabs_listener_disabled = true;
        tabs.selectTab(tabs.getTabAt(a.main._nodes_mode_tab), true);
        tabs_listener_disabled = false;
        log("switch " + a.main._nodes_mode_tab); //--strip

        if (a.main._nodes_mode_tab == 0) { 
            toolbar.setTitle(R.string.world);
//            mode.setChecked(false);
//            mode_cap.setText("Listing all world. turn for listing my bookmarks.");
            return;
        }
        if (a.main._nodes_mode_tab == 1) { 
            toolbar.setTitle(R.string.label_bookmarks);
//            mode.setChecked(true);
//            mode_cap.setText("Listing my bookmarks. turn for listing all world.");
            return;
        }
        log("refresh r2r tab " + tabs.getSelectedTabPosition()); //--strip
        TabLayout.Tab tab = tabs.getTabAt(tabs.getSelectedTabPosition());
        toolbar.setTitle(tab.getText());
        if (r2r_bookmarks == null) {

        }

//        mode.setChecked(true);
//        mode_cap.setText("Listing my bookmarks. turn for listing all world.");
        tabs.setVisibility(View.VISIBLE);
    }


    void reload(boolean force) {
        a.assert_ui_thread(); //--strip
        log("reload force=" + force + " tab=" + a.main._nodes_mode_tab); //--strip
        if (force) {
            protocols = null;
            redraw_prev_content = -1;
        }
        load_protocols();
        if (a.main._nodes_mode_tab == 0) { 
            if (force) world = null;
            load_world();
            return;
        }
        if (a.main._nodes_mode_tab == 1) { 
            if (force) bookmarks = null;
            load_bookmarks();
            return;
        }
        load_r2r_bookmarks();
        //redraw();
//        TabLayout.Tab tab = tabs.getTabAt(tabs.getSelectedTabPosition());
//        load_bookmarks2((bookmarks_t)tab.getTag());
    }

    no_scroll_list_view lv;

    ArrayList<pair<String, bookmark_t>> world = null;
    ArrayList<pair<String, bookmark_t>> bookmarks = null;
    ArrayList<pair<String, bookmark_t>> r2r_bookmarks = null;
    protocol_selection_t protocol_selection = null;

    protocols_t protocols = null;


    adapter_t adapter = null;
    int dispatchid;
    //private Switch mode;
    //private TextView mode_cap;
    TabLayout tabs;
    int _busy = 0;
}

