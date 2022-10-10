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
import androidx.appcompat.app.AppCompatActivity;                                               // AppCompatActivity
import us.gov.crypto.base58;                                                                   // base58
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.wallet.trader.bookmark_t;                                                            // bookmark_t
import com.google.android.material.bottomsheet.BottomSheetBehavior;                            // BottomSheetBehavior
import android.os.Build;                                                                       // Build
import android.os.Bundle;                                                                      // Bundle
import static us.wallet.trader.chat.chat_t;                                                    // chat_t
import android.graphics.Color;                                                                 // Color
import android.widget.CompoundButton;                                                          // CompoundButton
import android.content.Context;                                                                // Context
import us.gov.socket.datagram;                                                                 // datagram
import us.wallet.trader.data_t;                                                                // data_t
import us.wallet.trader.endpoint_t;                                                            // endpoint_t
import androidx.fragment.app.Fragment;                                                         // Fragment
import androidx.fragment.app.FragmentTransaction;                                              // FragmentTransaction
import android.widget.FrameLayout;                                                             // FrameLayout
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import android.widget.ImageView;                                                               // ImageView
import static us.gov.io.types.*;                                                               // *
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import android.content.Intent;                                                                 // Intent
import static us.ko.is_ko;                                                                     // is_ko
import us.ko;                                                                                  // ko
import android.widget.LinearLayout;                                                            // LinearLayout
import com.google.android.material.button.MaterialButton;                                      // MaterialButton
import android.view.Menu;                                                                      // Menu
import android.view.MenuItem;                                                                  // MenuItem
import androidx.annotation.NonNull;                                                            // NonNull
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import us.wallet.protocol;                                                                     // protocol
import us.wallet.trader.qr_t;                                                                  // qr_t
import android.widget.RelativeLayout;                                                          // RelativeLayout
import androidx.annotation.RequiresApi;                                                        // RequiresApi
import us.string;                                                                              // string
import android.widget.Switch;                                                                  // Switch
import android.widget.TextView;                                                                // TextView
import java.util.Timer;                                                                        // Timer
import java.util.TimerTask;                                                                    // TimerTask
import android.widget.Toast;                                                                   // Toast
import android.media.ToneGenerator;                                                            // ToneGenerator
import androidx.appcompat.widget.Toolbar;                                                      // Toolbar
import android.view.View;                                                                      // View

//import com.google.firebase.crashlytics.FirebaseCrashlytics;                                    // FirebaseCrashlytics

public class trader extends activity implements datagram_dispatcher_t.handler_t  {

    private static void log(final String line) {           //--strip
        CFG.log_android("trader: " + line);                //--strip
    }                                                      //--strip

    public synchronized data_t get_data() {
        return data_;
    }

    public synchronized String get_src_data() {
        return src_data;
    }

    private synchronized void set_data(String src, data_t x) {
        src_data = src;
        data_ = x;
    }

    @Override protected void onSaveInstanceState(Bundle outState) {
        log("WORKAROUND_FOR_BUG_19917"); //--strip
        outState.putString("WORKAROUND_FOR_BUG_19917_KEY", "WORKAROUND_FOR_BUG_19917_VALUE");
        super.onSaveInstanceState(outState);
    }

    @RequiresApi(api = Build.VERSION_CODES.O)
    @Override protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        log("onCreate"); //--strip
        set_content_layout(R.layout.activity_trader);
        tradeid = findViewById(R.id.tradeid);
        if (getIntent().hasExtra("tid")) {
            Bundle bundle = getIntent().getExtras();
            tid = new hash_t(bundle.getByteArray("tid"));
            log("tid=" + tid.encode()); //--strip
        }
        else {
            log("KO 40310 Missing tid."); //--strip
            finish();
        }
        toolbar.setTitle(getResources().getString(R.string.trader) + " #" + tid.encode());
        tradeid.setText("id " + tid.encode());
        procol = findViewById(R.id.procol);
        procol_cap = findViewById(R.id.procol_cap);
        toolbar_button refresh = findViewById(R.id.refresh);
        bottom_sheet = findViewById(R.id.bottom_sheet);
        button_chat = findViewById(R.id.button_chat);
        button_chat.setVisibility(View.VISIBLE);
        sheetBehavior = BottomSheetBehavior.from(bottom_sheet);
        sheetBehavior.setState(BottomSheetBehavior.STATE_HIDDEN);
        sheetBehavior.setSkipCollapsed(true);

        toolbar_button action = findViewById(R.id.action);
        action.setVisibility(View.GONE);
        chat_handlers();
        vf = 0;
        ft = new fragment_trader();
        ft.setArguments(getIntent().getExtras());
        log("Set Fragment Trader"); //--strip
        getSupportFragmentManager().beginTransaction().add(R.id.fragment_container, ft).commitAllowingStateLoss();
        refresh.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                invalidate_data_fetch0();
            }
        });
        refresh.setVisibility(View.VISIBLE);
        procol.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                log("TOUCHED ROLE VIEW"); //--strip
                if (cur_protocol == null) {
                    log("cur_protocol is null"); //--strip
                }
                else {
                    if (procol.isChecked()) {
                        log("checked vf = " + vf); //--strip
                        if (vf != 1) {
                            log("vf must change to 1"); //--strip
                            vf = 1;
                        }
                    }
                    else {
                        log("not checked vf = " + vf); //--strip
                        if (vf != 0) {
                            log("vf must change to 0"); //--strip
                            vf = 0;
                        }
                    }
                }
                refresh();
            }
        });
    }

    @Override public void onPause() {
        super.onPause();
        log("onPause"); //--strip
        if (a.hmi == null) {
            dispatchid = -1;
        }
        else {
            if (dispatchid != -1) {
                a.hmi.dispatcher.disconnect_sink(dispatchid);
            }
        }
    }

    @Override public void onResume() {
        super.onResume();
        log("onResume"); //--strip
        if (a.hmi == null) {
            dispatchid = -1;
        }
        else {
            log("Connecting to datagram dispatcher");//--strip
            dispatchid = a.hmi.dispatcher.connect_sink(this);
            invalidate_data_fetch0();
        }
    }

    @Override public void on_push(final hash_t target_tid, final uint16_t code, final byte[] payload) {
        log("on_push"); //--strip
        if (!tid.equals(target_tid)) {
            return;
        }
        switch(code.value) {
            case us.wallet.trader.trader_t.push_data: {
                log("a data for me");  //--strip
                string s = new string();
                ko r = us.gov.io.blob_reader_t.parse(new blob_t(payload), s);
                if (is_ko(r)) {
                    return;
                }
                setdata__worker(s.value);
                log("OK - Imported data"); //--strip
            }
            break;
            case us.wallet.trader.trader_t.push_roles: {
                log("roles"); //--strip
                us.wallet.trader.roles_t roles = new us.wallet.trader.roles_t();
                ko r = us.gov.io.blob_reader_t.parse(new blob_t(payload), roles);
                if (is_ko(r)) {
                    log(r.msg); //--strip
                    return;
                }
                log("roles for me");  //--strip
                setroles(roles);
                log("OK - Imported roles"); //--strip
            }
            break;
            case us.wallet.trader.trader_t.push_chat: {
                log("a chat for me "); //--strip
                openchat__worker(payload);
            }
            break;
            case us.wallet.trader.trader_t.push_killed: {
                log("killed "); //--strip
                archive();
            }
            break;
        }
    }

    void chat_handlers() {
        button_chat.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                log("click on chat_handler"); //--strip
                int st = sheetBehavior.getState();
                if (st == BottomSheetBehavior.STATE_HIDDEN) {
                    sheetBehavior.setState(BottomSheetBehavior.STATE_EXPANDED); //BottomSheetBehavior.BottomSheetBehavior);
                }
                else if (st == BottomSheetBehavior.STATE_EXPANDED) {
                    sheetBehavior.setState(BottomSheetBehavior.STATE_HIDDEN);
                }
                else if (st == BottomSheetBehavior.STATE_COLLAPSED) {
                    sheetBehavior.setState(BottomSheetBehavior.STATE_EXPANDED);
                }
                else {
                    sheetBehavior.setState(BottomSheetBehavior.STATE_HIDDEN);
                }
            }
        });

        sheetBehavior.setBottomSheetCallback(new BottomSheetBehavior.BottomSheetCallback() {
            @Override public void onStateChanged(@NonNull View view, int newState) {
                switch (newState) {
                    case BottomSheetBehavior.STATE_HIDDEN:
                        break;
                    case BottomSheetBehavior.STATE_EXPANDED: {
                            log("bottomsheet expanded: opening chat chat_handler"); //--strip
                            openchat(null);
                        }
                        break;
                    case BottomSheetBehavior.STATE_COLLAPSED:
                        break;
                    case BottomSheetBehavior.STATE_DRAGGING:
                        break;
                    case BottomSheetBehavior.STATE_SETTLING:
                        break;
                }
            }

            @Override public void onSlide(@NonNull View view, float v) {}
        });
    }

    void openchat__worker(final byte[] chatpayload) {
        app.assert_worker_thread(); //--strip
        runOnUiThread(new Runnable() {
            @Override public void run() {
                openchat(chatpayload);
            }
        });
    }

    public String getendpoint() {
        data_t data = get_data();
        String endpoint = data.find("remote_endpoint");
        if (endpoint == null) {
            endpoint = "";
        }
        return endpoint;
    }

    public String peer_moniker() {
        data_t data = get_data();
        String s = data.find("peer_moniker");
        if (s == null) {
            s = "Anonymous";
        }
        return s;
    }

    public hash_t gettid() {
        return tid;
    }

    boolean setroles(final us.wallet.trader.roles_t roles) {
        this.roles = roles;
        if (forward_roles != null) {
            final fragment_trader fr = forward_roles;
            final us.wallet.trader.roles_t r = this.roles;
            forward_roles = null;
            runOnUiThread(new Runnable() {
                @Override public void run() {
                    fr.select_roles(r);
                }
            });
        }
        return true;
    }

    boolean setdata0(final String sdata) {
        log("sdata:"); //--strip
        log("-------------------------");//--strip
        log(sdata);//--strip
        log("-------------------------");//--strip
        data_t dat = new data_t();
        ko r = dat.from(sdata);
        if (is_ko(r)) {
            log(r.msg); //--strip
        }
        set_data(src_data, dat);
        return true;
    }

    boolean setdata1(final String sdata) {
        log("new data object "); //--strip
        if (!setdata0(sdata)) {
            log("KO 30306 failed setdata0"); //--strip
            return false;
        }
        data_t data = get_data();
        if (data == null) {
            log("data is null"); //--strip
            cur_protocol = null;
            vf = 0;
        }
        else {
            log("data is not null"); //--strip
            String r = data.find("protocol");
            if (r == null) {
                log("KO 40322 'protocol' entry cannot be found in data"); //--strip
                cur_protocol = null;
                vf = 0;
            }
            else {
                log("'protocol' entry found in data: " + r); //--strip
                if (r.equals("not set")) {
                    log("'protocol' is not set:"); //--strip
                    cur_protocol = null;
                    vf = 0;
                }
                else {
                    log("'protocol' set to " + r); //--strip
                    if (!r.equals(cur_protocol)) {
                        cur_protocol = r;
                        vf = 1;
                    }
                }
            }
            String pt = data.find("parent_trade");
            if (pt == null) {
                parent_trade = new hash_t(0);
            }
            else {
                parent_trade = new hash_t(base58.decode(pt));
            }
        }
        log("cur_protocol=" + cur_protocol); //--strip
        return true;
    }

    boolean setdata__worker(final String sdata) {
        app.assert_worker_thread(); //--strip
        setdata1(sdata);
        runOnUiThread(new Runnable() {
            @Override public void run() {
                log("data has been renewed -> refresh"); //--strip
                refresh();
            }
        });
        return true;
    }

    void get_sourceshit_qrs(String key, Context ctx) {
        app.assert_ui_thread(); //--strip
        if (command_show_param_qrs(key, ctx)) {
            return;
        }
        log("requesting " + key); //--strip
        request_item_qrs(key, ctx); //ask other node
    }

    void get_sourceshit(String key, Context ctx) {
        app.assert_ui_thread(); //--strip
        if (command_show_param(key, ctx)) {
            return;
        }
        log("requesting " + key); //--strip
        request_item(key, ctx);
    }

    void on_redirect(final bookmark_t redirect) {
        log("redirection to " + redirect.to_string()); //--strip
        log("referrertid " + tid.encode()); //--strip
        a.new_trade(tid, "", redirect.qr);
    }

    public void go_parent_trade() {
        if (parent_trade == null) {
            log("KO 01281 Nowhere to go"); //--strip
            return;
        }
        if (parent_trade.is_zero()) {
            log("KO 01282 Nowhere to go"); //--strip
            return;
        }
        Intent data = new Intent();
        a.go_trade(parent_trade);
    }

    public void setmode_loading() {
        app.assert_ui_thread(); //--strip
    }

    public void setmode_ready() {
        app.assert_ui_thread(); //--strip
    }

    public void setmode_wrong(final String reason) {
        log("setmode_wrong " + reason); //--strip
        app.assert_ui_thread(); //--strip
    }

    public void invalidate_data_fetch0() {
        app.assert_ui_thread(); //--strip
        if (a.hmi == null) {
            log("Closing activity hmi is null"); //--strip
            finish();
            return;
        }
        setmode_loading();
        log("invalidate_data_fetch"); //--strip
        set_data(null, null);
        log("show data"); //--strip
        a.hmi.command_trade(tid, "show data"); //backend will push data
    }

    public void refresh() {
        log("refresh"); //--strip
        app.assert_ui_thread(); //--strip
        if (a.hmi == null) {
            log("Closing activity hmi is null"); //--strip
            finish();
            return;
        }
        super.refresh();
        log("cur_protocol " + cur_protocol); //--strip
        log("procol.isChecked " + procol.isChecked()); //--strip
        if (get_data() == null) {
            setmode_wrong("KO 8675");
            return;
        }
        if (cur_protocol != null) {
            procol.setEnabled(true);
            if (vf == 1) {
                procol.setChecked(true);
                procol_cap.setText(" Role View - " + cur_protocol);
            }
            else {
                procol_cap.setText(" Trader View");
                procol.setChecked(false);
            }
        }
        else {
            procol.setChecked(false);
            procol.setEnabled(false);
            procol_cap.setText(" Trader View");
        }

        if (vf == 1) {
            log("role view"); //--strip
            log("create fragment for cur_protocol=" + cur_protocol); //--strip
            if (!create_ui(cur_protocol)) {
                log("KO 5032 - "); //--strip
                Toast.makeText(getApplicationContext(), "Cannot create UI " + cur_protocol, 6000).show();
            }
            else {
                log("replacing fragment ->1"); //--strip
                getSupportFragmentManager().beginTransaction().replace(R.id.fragment_container, specialized_fragment).commitNowAllowingStateLoss(); //AllowingStateLoss();
            }
        }
        else {
            log("trader view"); //--strip
            log("replacing fragment ->0"); //--strip
            try {
                getSupportFragmentManager().beginTransaction().replace(R.id.fragment_container, ft).commitNowAllowingStateLoss(); //AllowingStateLoss();
            }
            catch(Exception e) {
                Toast.makeText(getApplicationContext(), getResources().getString(R.string.cannotcreatetraderUI), 6000).show();
                error_manager.manage(e, e.getMessage() + "    " + e.toString());
            }
        }

        if (vf == 0) {
            log("call refreshfragment 0");  //--strip
            if (ft != null) {
                if (ft.getContext() != null) {
                    ft.refresh();
                }
            }
        }
        else {
            log("call refreshfragment 1");  //--strip
            if (specialized_fragment != null) {
                if (specialized_fragment.getContext() != null) {
                    specialized_fragment.refresh();
                }
            }
        }
    }

    boolean create_ui(String protocol) {
        if (protocol == null) return false;
        log("create_ui for " + protocol); // --strip
        if (protocol.equals("bid2ask bid")) {
            if (specialized_fragment != null) {
                if (specialized_fragment instanceof fragment_bid2ask_bid) {
                    log("right fragment in place"); // --strip
                    return true;
                }
            }
            log("new instance fragment_bid2ask_bid"); // --strip
            specialized_fragment = new fragment_bid2ask_bid();
        }
        else if (protocol.equals("pat2slt pat")) {
            if (specialized_fragment != null) {
                if (specialized_fragment instanceof fragment_pat2slt_pat) {
                    log("right fragment in place"); // --strip
                    return true;
                }
            }
            log("new instance"); // --strip
            specialized_fragment = new fragment_pat2slt_pat();
        }
        else if (protocol.equals("pat2phy pat")) {
            if (specialized_fragment != null) {
                if (specialized_fragment instanceof fragment_pat2phy_pat) {
                    log("right fragment in place"); // --strip
                    return true;
                }
            }
            log("new instance"); // --strip
            specialized_fragment = new fragment_pat2phy_pat();
        }
        else if (protocol.equals("pat2ai pat")) {
            if (specialized_fragment != null) {
                if (specialized_fragment instanceof fragment_pat2ai_pat) {
                    log("right fragment in place"); // --strip
                    return true;
                }
            }
            log("new instance"); // --strip
            specialized_fragment = new fragment_pat2ai_pat();
        }
        else if (protocol.equals("w2w w")) {
            if (specialized_fragment != null) {
                if (specialized_fragment instanceof fragment_w2w) {
                    log("right fragment in place"); // --strip
                    return true;
                }
            }
            log("new instance"); // --strip
            specialized_fragment = new fragment_w2w();
        }
        else {
            return false;
        }
        log("Setting args in specialized fragment"); // --strip
        specialized_fragment.setArguments(getIntent().getExtras());
        return true;
    }

    public void start_protocol(String selected_protocol) {
        log("start_protocol " + selected_protocol);  //--strip
        a.start_protocol(tid, selected_protocol);
    }

    public void onActivityResult(int requestCode, int resultCode, final Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        log("onActivityResult " + requestCode + " " + resultCode); //--strip
        if (resultCode == AppCompatActivity.RESULT_CANCELED) {
            return;
        }
        if (requestCode == PROTOCOLROLE_RESULT) {
            if (resultCode == AppCompatActivity.RESULT_OK) {
                String prtocol = data.getDataString();
                log("prtocol=" + prtocol); //--strip
                start_protocol(prtocol);
            }
            else if (resultCode == 111) {
                String KOreason = data.getDataString();
                Toast.makeText(this, KOreason, 6000).show();
            }
            else if (resultCode == 112) {
                String KOreason = data.getDataString();
                Toast.makeText(this, getResources().getString(R.string.noptrotocolsavailable) + KOreason, 6000).show();
            }
            else {
                Toast.makeText(this, getResources().getString(R.string.sorryselectionunsuccesful), 1000).show();
            }
            return;
        }
        specialized_fragment.onActivityResult(requestCode, resultCode, data);
    }

    boolean require_request(String key) {
        boolean rr = true;
        if (key.equals("log")) {
            rr = false;
        }
        return rr;
    }

    boolean command_show_param_qrs(final String key, Context ctx) {
        app.assert_ui_thread(); //--strip
        data_t data = get_data();
        String val = null;
        if (data != null) {
            val = data.find(key);
        }
        if (val != null) {
            log("val not null");  //--strip
            if (val.equals("Y")) {
                log("key '" + key + "' found Y");  //--strip
                a.hmi.command_trade(tid, "show " + key + " peer");
                return true;
            }
            log("val is " + val);  //--strip
        }
        else {
            log("val is null");  //--strip
        }
        return false;
    }

    boolean command_show_param(final String key, Context ctx) {
        app.assert_ui_thread(); //--strip
        boolean rr = require_request(key);
        if (rr) {
            data_t data = get_data();
            String val = null;
            if (data != null) {
                val = data.find(key);
            }
            if (val != null) {
                log("val not null");  //--strip
                if (val.equals("Y")) {
                    log("key '" + key + "' found Y");  //--strip
                    a.hmi.command_trade(tid, "show " + key);
                    return true;
                }
                log("val is " + val);  //--strip
            }
            else {
                log("val is null");  //--strip
            }
        }
        else {
            a.hmi.command_trade(tid, "show " + key);
            return true;
        }
        return false;
    }

    void request_item_qrs(final String key, Context ctx) {
        app.assert_ui_thread(); //--strip
        a.hmi.command_trade(tid, "request qrs");
    }

    void request_item(final String key, Context ctx) {
        app.assert_ui_thread(); //--strip
        String request_verb = "request";
        if (key.equals("invoice")) { //chapu until backend verb is consistent across keys
            request_verb = "query";
        }
        a.hmi.command_trade(tid, key + " " + request_verb);
    }

    public void openchat(byte[] chatpayload) {
        log("openchat"); //--strip
        app.assert_ui_thread(); //--strip
        if (chat_fragment == null) {
            log("chat_fragment is null. recreating."); //--strip
            String lbl = peer_moniker();
            log("Starting fragment chat_fragment"); //--strip
            chat_fragment = new chat();
            Bundle fbundle = new Bundle();
            fbundle.putByteArray("tid", tid.value);
            fbundle.putString("lbl", lbl);
            fbundle.putByteArray("raw", chatpayload);
            chat_fragment.setArguments(fbundle);
            final androidx.fragment.app.FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
            transaction.replace(R.id.bottom_sheet, chat_fragment);
            transaction.addToBackStack(null);
            try {
                log("Commit fragment tx"); //--strip
                transaction.commitAllowingStateLoss();
            }
            catch (Exception e) {
                log("Ignoring exception " + e.getMessage()); //--strip
            }
        }

        int st = sheetBehavior.getState();
        boolean sound = false;
        if (st == BottomSheetBehavior.STATE_HIDDEN) {
            sheetBehavior.setState(BottomSheetBehavior.STATE_EXPANDED);
            sound = true;
        }
        else if (st == BottomSheetBehavior.STATE_COLLAPSED) {
            sound = true;
        }
        if (sound) {
            log("SOUND"); //--strip
            a.tone.startTone(ToneGenerator.TONE_CDMA_CALL_SIGNAL_ISDN_NORMAL, 150);
        }

    }

    void archive() {
        runOnUiThread(new Thread(new Runnable() {
            @Override public void run() {
                Toast.makeText(a.main, "This trade has been archived.", 6000).show();
                finish();
            }
        }));
    }

    void close_trade() { //means archive
        app.assert_ui_thread(); //--strip
        final pair<ko, String> b = a.hmi.kill_trade(tid);
        if (is_ko(b.first)) {
            Toast.makeText(trader.this, a.hmi.rewrite(b.first), 6000).show();
            return;
        }
        finish();
    }

    static final int PROTOCOLROLE_RESULT = 902;

    private Switch procol;
    private TextView procol_cap;
    private TextView tradeid;
    private fragment_trader ft = null;
    private role_fragment specialized_fragment = null;
    private int vf; //displayed fragment
    public hash_t tid;
    public hash_t parent_trade = null;
    public String cur_protocol = null;
    int dispatchid;
    private data_t data_ = null;
    private String src_data = null;
    public us.wallet.trader.roles_t roles = null;
    public fragment_trader forward_roles = null;
    chat chat_fragment = null;
    private BottomSheetBehavior sheetBehavior;
    private FrameLayout bottom_sheet;
    private toolbar_button button_chat;

}

