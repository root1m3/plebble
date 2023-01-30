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
import com.google.android.material.tabs.TabLayout; 
import java.util.TreeMap;
import us.wallet.trader.protocol_selection_t;
import us.wallet.trader.bootstrap.protocols_t;
import us.wallet.trader.cert.cert_index_t;                                                                              // string
import us.wallet.trader.cert.cert_t;
import static us.gov.io.types.*;                                                               // *

public final class certs extends activity implements datagram_dispatcher_t.handler_t {

    static void log(final String line) {          //--strip
        CFG.log_android("certs: " + line);        //--strip
    }                                             //--strip

    public static class adapter_t extends ArrayAdapter<pair<hash_t, string>> {
        private LayoutInflater inflater;

        public adapter_t(Activity ac, ArrayList<pair<hash_t, string>> data) {
            super(ac, R.layout.certs_list_item, data);
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
            pair<hash_t, string> sbm = getItem(position);
            string bm = sbm.second;
            if (convert_view == null) {
                convert_view = inflater.inflate(R.layout.certs_list_item, null, true);
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
            holder.subject.setText(sbm.first.encode());
            holder.tvitem.setText(sbm.second.value);
            holder.tvitem2.setText("");
            holder.image.setImageResource(R.raw.cert);
            return convert_view;
        }
    }

    ArrayList<pair<hash_t, string>> convert(cert_index_t o0) {
        ArrayList<pair<hash_t, string>> o = new ArrayList<pair<hash_t, string>>();
        o.ensureCapacity(o0.size());
        for (Map.Entry<hash_t, string> entry : o0.entrySet()) {
            o.add(new pair<hash_t, string>(entry.getKey(), entry.getValue()));
        }
        return o;
    }

    boolean tabs_listener_disabled = false;

    @RequiresApi(api = Build.VERSION_CODES.O)
    @Override protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        set_content_layout(R.layout.certs);
        tabs = findViewById(R.id.tabs);
        lv = (no_scroll_list_view) findViewById(R.id.listviewX);
        assert lv != null;
        adapter = new adapter_t(this, new ArrayList<pair<hash_t, string>>());
        lv.setAdapter(adapter);

        Bundle bundle = getIntent().getExtras();
        if (bundle != null) {
            mode = bundle.getInt("mode", 0); //0 view/create; 1 select
        }

        lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override public void onItemClick(AdapterView parentView, View childView, int position, long id) {
                assert a.main != null;
                item_click(list.get(position), position);
                return;
            }
        });
        lv.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
            @Override public boolean onItemLongClick(AdapterView parentView, View childView, int position, long id) {
                return true;
/*
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
                                    InputMethodManager inputMethodManager= (InputMethodManager) certs.this.getSystemService(Context.INPUT_METHOD_SERVICE);
                                    inputMethodManager.showSoftInput(input, InputMethodManager.SHOW_IMPLICIT);
                                }
                            });
                        }
                    });
                input.requestFocus();
                input.setSingleLine();
                AlertDialog dialog = new AlertDialog.Builder(new ContextThemeWrapper(certs.this, R.style.myDialog))
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
*/
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
                ntab = tab.getPosition();
                log("tab selected. position " + ntab); //--strip
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

        toolbar_button new_btn = findViewById(R.id.new_btn);
        new_btn.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                new_cert();
            }
        });
        new_btn.setVisibility(View.VISIBLE);

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


//    void item_click(pair<String, bookmark_t> bm) {
/*
        String[] options = {"Banking (move coins)", a.getResources().getString(R.string.start_new_trade), a.getResources().getString(R.string.cancel)};
        final certs i = certs.this;
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
*/
//    }

/*
    void item_click1(pair<String, bookmark_t> bm) {
        String[] options = {a.getResources().getString(R.string.start_new_trade), "edit bookmark", "delete bookmark", a.getResources().getString(R.string.cancel)};
        final certs i = certs.this;
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
*/

    void item_click(pair<hash_t, string> bm, final int pos) {
        if (mode == 1) {
            String[] options = {"Select this certificate", "View certificate", a.getResources().getString(R.string.cancel)};
            final certs i = certs.this;
            new AlertDialog.Builder(i).setTitle(bm.first.encode() + " " + bm.second.value)
                .setItems(options, new DialogInterface.OnClickListener() {
                    @Override public void onClick(DialogInterface dialog, int which) {
                        switch(which) {
                            case 0:
                                Intent data = new Intent();
                                data.putExtra("nft", bm.first);
                                setResult(RESULT_OK, data);
                                finish();
                                break;
                            case 1:
                                open_cert(bm.first);
                                break;
                        }
                    }
                }).setIcon(R.drawable.ic_world).show();
        }
        else {
            open_cert(bm.first);
        }
    }

    hash_t marked = null;

    @Override public void on_push(final hash_t target_tid, final uint16_t code, final byte[] payload) {
        log("on_push"); //--strip
        switch(code.value) {
            case us.wallet.trader.trader_t.push_cert: {
                log("recv cert "); //--strip
                hash_t nft = new hash_t();
                ko r = us.gov.io.blob_reader_t.parse(new blob_t(payload), nft);
                if (is_ko(r)) {
                    log(r.msg); //--strip
                    return;
                }
                toast__worker("Trade " + target_tid.encode() + " received cert: " + nft.encode());
                marked = nft;
                load_list__worker();
            }
            break;
        }
    }

    void redraw() {
        log("redraw tab=" + a.main._nodes_mode_tab); //--strip
        adapter.clear();
        adapter.addAll(list);
    }
/*
    void load_remote() {
        set_busy(true);
        log("load_remote " + a.main._nodes_mode_custom.size()); //--strip
        bookmarks = convert(a.main._nodes_mode_custom);
        set_busy(false);
    }
*/
/*
    void on_protocols() {
        a.assert_ui_thread(); //--strip
        refresh();
    }
*/
/*
    void on_r2r_bookmarks__worker(final bookmarks_t bookmarks) {
        a.assert_worker_thread(); //--strip
        log("on_r2r_bookmarks__worker " + bookmarks.size()); //--strip
        r2r_bookmarks = convert(bookmarks);

        runOnUiThread(new Runnable() {
            @Override public void run() {
                redraw();
            }
        });
    }
*/
/*
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
*/
/*
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
*/
/*
    void load_r2r_bookmarks() {
    }
*/
    void load_list__worker() {
        Thread thread1 = new Thread(new Runnable() {
            @Override public void run() {
                log("load_world-run"); //--strip
                set_busy__worker(true);
                //us.gov.io.types.vector_hash vh = new us.gov.io.types.vector_hash();
                if (!a.has_hmi() || a.hmi().rpc_peer == null) {
                    ko r = new ko("KO 70699 HMI is not on"); //--strip
                    set_busy__worker(false);
                    log(r.msg); //--strip
                    return;
                }

    //            using cert_index_t = us::wallet::trader::cert::cert_index_t;
    //            int mode = args.next<int>(0);
                
                cert_index_t cert_index = new cert_index_t();
                uint8_t mode = new uint8_t(ntab == 0 ? (byte)1 : (byte)0);
                ko r = a.hmi().rpc_peer.call_cert_list(mode, cert_index);
                if (is_ko(r)) {
                    set_busy__worker(false);
                    log(r.msg); //--strip
                    report_ko__worker(r);
                    return;
                }
                list = convert(cert_index);
                set_busy__worker(false);
                return;
            }
        });
        thread1.start();
    }

    void load_list() {
        load_list__worker();
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
/*
    public static class role_metadata_t {
        public role_metadata_t(String title_) {
            title = title_;
        }
        String title;
        //int res;
    };
*/
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
        if (tabs.getTabCount() == 2) {
            return;
        }
        log("retab "); //--strip
        tabs_listener_disabled = true;
        {
        TabLayout.Tab tab = tabs.newTab().setText("Issued");
        tab.setIcon(R.raw.cert);
        //tab.setTag(i);
        tabs.addTab(tab);
        }
        {
        TabLayout.Tab tab = tabs.newTab().setText("All");
        tab.setIcon(R.raw.cert);
        //tab.setTag(i);
        tabs.addTab(tab);
        }

        tabs_listener_disabled = false;
    }

    @Override public String acid() {
        return "certs";
    }

    @Override public void refresh() {
        log("refresh"); //--strip
        a.assert_ui_thread(); //--strip
        if (!a.has_hmi()) {
            log("Closing activity hmi is null"); //--strip
            finish();
            return;
        }
        super.refresh();
        retab();
        log("select tab " + ntab); //--strip
        tabs_listener_disabled = true;
        tabs.selectTab(tabs.getTabAt(ntab), true);
        tabs_listener_disabled = false;
        log("switch " + ntab); //--strip

        if (mode == 1) {
            setTitle("Select cert");
        }
        else {
            //toolbar.setTitle("Certs");
            setTitle("Certs");
        }

        tabs.setVisibility(View.VISIBLE);
    }


    void reload(boolean force) {
        a.assert_ui_thread(); //--strip
        log("reload force=" + force); //--strip
//        if (force) {
            //protocols = null;
            //redraw_prev_content = -1;
//        }
        //load_protocols();
/*
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
*/
        load_list();

    }

    static final int DOC_VIEWER_RESULT = 832;

    void open_cert(final hash_t nft) {
        String title = "cert " + nft.encode();
        String fname = "cert_" + nft.encode();
        String fetch_content_cmd = "";
        String action_cmd = "";
        String action_caption = "";
        int mode = 0;
        launch_doc_viewer(title, fname, fetch_content_cmd, action_cmd, action_caption, mode, nft);
    }

    void new_cert() {
        String title = "New cert";
        String fname = "cert";
        String fetch_content_cmd = "";
        String action_cmd = "tade cert create";
        String action_caption = "Create";
        int mode = 1;
        launch_doc_viewer(title, fname, fetch_content_cmd, action_cmd, action_caption, mode, null);
    }

    public void launch_doc_viewer(final String title, final String fname, final String fetch_content_cmd, final String action_cmd, final String action_caption, int mode, hash_t nft) {
        Intent intent = new Intent(this, doc_viewer.class);
        intent.putExtra("tid", new hash_t(0));
        if (nft != null) {
            intent.putExtra("nft", nft);
        }
        intent.putExtra("title", title);
        intent.putExtra("fname", fname);
        intent.putExtra("fetch_content_cmd", fetch_content_cmd);
        intent.putExtra("action_cmd", action_cmd);
        intent.putExtra("action_caption", action_caption);
        intent.putExtra("mode", mode);
        startActivityForResult(intent, DOC_VIEWER_RESULT);
    }

    @Override public void onActivityResult(int request_code, int result_code, Intent data) {
        log("onActivityResult " + request_code + " " + result_code); //--strip
        super.onActivityResult(request_code, result_code, data);
        log("onActivityResult impl " + request_code + " " + result_code); //--strip
        if (request_code != DOC_VIEWER_RESULT) {
            log("not from doc_viewer"); //--strip
            return;
        }
        if (result_code != doc_viewer.RESULT_OK) {
            log("doc_viewer was cancelled"); //--strip
            return;
        }
        String cmd = data.getStringExtra("actioncommand");
        String msg = data.getStringExtra("msg");
        log("Xecuting action: " + cmd); //--strip
        if (msg.isEmpty()) {
            toast("Empty msg. Aborted: cert create." + msg);
            return;
        }
        set_busy(true);
        hash_t nft = new hash_t();
        ko r = a.hmi().rpc_peer.call_cert_create(new string(msg), nft);
        if (is_ko(r)) {
            set_busy(false);
            log(r.msg); //--strip
            report_ko(r);
            return;
        }
        toast("New cert: " + nft.encode());    
        set_busy(false);
        reload(true);
        //a.hmi().command_trade(new hash_t(0), cmd);
    }

    no_scroll_list_view lv;

    ArrayList<pair<hash_t, string>> list = null;


    adapter_t adapter = null;
    int dispatchid;
    TabLayout tabs;
    int ntab = 0;
    int _busy = 0;
    int mode = 0; //0-view; 1-select
}

