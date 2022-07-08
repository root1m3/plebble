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
import android.widget.AdapterView;                                                             // AdapterView
import androidx.appcompat.app.AlertDialog;                                                     // AlertDialog
import android.widget.ArrayAdapter;                                                            // ArrayAdapter
import java.util.ArrayList;                                                                    // ArrayList
import android.graphics.BitmapFactory;                                                         // BitmapFactory
import android.graphics.Bitmap;                                                                // Bitmap
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.wallet.trader.bookmarks_t;                                                           // bookmarks_t
import android.os.Bundle;                                                                      // Bundle
import android.widget.Button;                                                                  // Button
import java.util.Collections;                                                                  // Collections
import android.graphics.Color;                                                                 // Color
import android.widget.CompoundButton;                                                          // CompoundButton
import androidx.coordinatorlayout.widget.CoordinatorLayout;                                    // CoordinatorLayout
import us.wallet.trader.data_t;                                                                // data_t
import java.util.Date;                                                                         // Date
import static android.graphics.BitmapFactory.decodeResource;                                   // decodeResource
import android.content.DialogInterface;                                                        // DialogInterface
import android.graphics.drawable.Drawable;                                                     // Drawable
import androidx.fragment.app.FragmentActivity;                                                 // FragmentActivity
import androidx.fragment.app.Fragment;                                                         // Fragment
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import android.widget.ImageView;                                                               // ImageView
import static us.gov.io.types.*;                                                               // *
import static us.ko.*;                                                                         // *
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import android.content.Intent;                                                                 // Intent
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import android.widget.LinearLayout;                                                            // LinearLayout
import com.google.android.material.button.MaterialButton;                                      // MaterialButton
import android.view.MotionEvent;                                                               // MotionEvent
import androidx.annotation.NonNull;                                                            // NonNull
import us.pair;                                                                                // pair
import android.widget.Spinner;                                                                 // Spinner
import android.widget.Switch;                                                                  // Switch
import android.widget.TextView;                                                                // TextView
import java.util.concurrent.TimeUnit;                                                          // TimeUnit
import android.widget.Toast;                                                                   // Toast
import android.net.Uri;                                                                        // Uri
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View

public class fragment_trader extends Fragment implements datagram_dispatcher_t.handler_t {

    static void log(final String line) {                //--strip
       CFG.log_android("fragment_trader: " + line);     //--strip
    }                                                   //--strip

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        this.inflater = inflater;
        this.container = container;
        view = inflater.inflate(R.layout.fragment_trader, container, false);
        tr = (trader)getActivity();
        a = (app)getActivity().getApplication();
        select_protocol_role = view.findViewById(R.id.initiator_selectprotocol);
        online = view.findViewById(R.id.switch_online);
        endpoint = view.findViewById(R.id.endpoint);
        onlinetime = view.findViewById(R.id.onlinetime);
        reason = view.findViewById(R.id.reason);
        state = view.findViewById(R.id.state);
        personality_initiator = view.findViewById(R.id.personality_initiator);
        initiator_title =  view.findViewById(R.id.initiator_title);
        pic_initiator = view.findViewById(R.id.pic_initiator);
        personality_follower= view.findViewById(R.id.personality_follower);
        follower_title = view.findViewById(R.id.follower_title);
        pic_follower = view.findViewById(R.id.pic_follower);
        closetrade_btn = view.findViewById(R.id.closetrade_btn);
        log_btn = view.findViewById(R.id.log_btn);
        create_bookmark_btn = view.findViewById(R.id.save_bookmark_btn);
        personality_exchange_btn = view.findViewById(R.id.personality_exchange_btn);
        reset = view.findViewById(R.id.button_reset);
        reload = view.findViewById(R.id.button_reload);
        data_btn = view.findViewById(R.id.data_btn);
        set_handlers();
        if (getActivity().getIntent().hasExtra("tid")) {
            Bundle bundle = getActivity().getIntent().getExtras();
            tid = new hash_t(bundle.getByteArray("tid"));
        }
        else { //--strip
            log("KO 4033 - missing tid"); //--strip
        } //--strip
        log("connect network-datagram hose");//--strip
        dispatchid = a.datagram_dispatcher.connect_sink(this);
        return view;
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        log("onDestroyView"); //--strip
        log("disconnect network-datagram hose.");//--strip
        a.datagram_dispatcher.disconnect_sink(dispatchid);
    }

    @Override
    public void onResume() {
        super.onResume();
        log("onResume"); //--strip
    }

    @Override
    public void onPause() {
        super.onPause();
        log("onPause"); //--strip
    }

    void reset() {
        a.hmi.command_trade(tid, "reset");
    }

    void reload() {
        a.hmi.command_trade(tid, "reload");
    }

    void show_roles() {
        app.assert_ui_thread(); //--strip
        log("show roles"); //--strip
        a.hmi.command_trade(tid, "show roles");
    }

    void close_trade() { //means archive
        tr.close_trade();
    }

    @Override
    public void setUserVisibleHint(boolean isVisibleToUser) {
        super.setUserVisibleHint(isVisibleToUser);
    }

    TextView get_my_personality() {
        if (initiator) {
            return personality_initiator;
        }
        return personality_follower;
    }

    void exchange_personality() {
        app.assert_ui_thread();  //--strip
        final String myp = get_my_personality().getText().toString();
        if (!a.hmi.isAddressValid(myp)) {
            Toast.makeText(getActivity().getApplicationContext(), getResources().getString(R.string.invalidpersonality), 6000).show();
            return;
        }
        a.use_personality(tid, myp);
    }

    public void select_pr(String pr) {
        app.assert_ui_thread();  //--strip
        log("select_pr " + pr); //--strip
        tr.start_protocol(pr);
    }

    public void select_roles(us.wallet.trader.roles_t roles) {
        app.assert_ui_thread();  //--strip
        if(a.hmi == null) return;
        String[] options = roles.as_options();
        new AlertDialog.Builder(tr).setTitle("Select role:")
            .setItems(options, new DialogInterface.OnClickListener() {
                @Override public void onClick(DialogInterface dialog, int which) {
                    if (which >= 0 && which < options.length) {
                        select_pr(options[which]);
                    }
                }
            })
        .show();
    }

    void create_bookmark() {
        tr.get_sourceshit_qrs("qrs", getActivity().getApplicationContext());
    }

    void set_handlers() {
        pic_initiator.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (initiator) {
                    on_personality_me(personality_initiator.getText().toString());
                }
                else {
                    on_personality_peer(personality_initiator.getText().toString());
                }

            }
        });
        data_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(getActivity().getApplicationContext(), tradelog.class);
                intent.putExtra("tid", tid.value);
                intent.putExtra("content_type", "data");
                startActivity(intent);
            }
        });
        reset.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                reset();
            }
        });
        reload.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                reload();
            }
        });
        closetrade_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                close_trade();
            }
        });
        log_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(getActivity().getApplicationContext(), tradelog.class);
                intent.putExtra("tid", tid.value);
                intent.putExtra("content_type", "log");
                startActivity(intent);
            }
        });
        personality_exchange_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                exchange_personality();
            }
        });
        create_bookmark_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                create_bookmark();
            }
        });
        select_protocol_role.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                tr.forward_roles = fragment_trader.this;
                tr.get_sourceshit("roles", getActivity().getApplicationContext());
            }
        });
        online.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override public void onCheckedChanged(CompoundButton buttonView, boolean is_checked) {
                if (a == null) return;
                if (a.hmi == null) return;
                a.hmi.command_trade(tid, is_checked ? "connect" : "disconnect");
            }
        });
    }

    @Override public void on_push(hash_t target_tid, uint16_t code, byte[] payload) {
        log("on_push"); //--strip
        if (!target_tid.equals(tid)) {
            log("not for me"); //--strip
            return;
        }
        switch(code.value) {
            case us.wallet.trader.trader_protocol.push_ico: {
                log("an ico for me"); //--strip
                set_peer_ico(payload);
                return;
            }
            case us.wallet.trader.trader_t.push_qr_peer: {
                log("a qr_peer for me"); //--strip
                set_qr_peer(payload);
                return;
            }
        }
    }

    public void set_qr_peer(final byte[] payload) {
        app.assert_worker_thread(); //--strip
        final bookmarks_t bm = new bookmarks_t();
        ko r = blob_reader_t.readB(new blob_t(payload), bm);
        if (is_ko(r)) {
            return;
        }
        final main_activity ma = tr.main;
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ma.show_remote_bookmarks(tid, bm);
            }
        });
    }

    public void set_peer_ico(final byte[] l) {
        app.assert_worker_thread(); //--strip
        log("set_peer_ico " + l.length); //--strip
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                r_peer_ico = BitmapFactory.decodeByteArray(l, 0, l.length);
                if (r_peer_ico == null) {
                    log("KO 50059 Bitmap is null " + l.length); //--strip
                    r_peer_ico = null;
                }
                else {
                    try {
                        log("ico is " + pic_initiator.getWidth() + " " + pic_initiator.getHeight()); //--strip
                        Bitmap sb = Bitmap.createScaledBitmap(r_peer_ico, pic_initiator.getWidth(), pic_initiator.getHeight(), false);
                        if (sb == null) {
                            log("Bitmap is null (2) " + l.length); //--strip
                            r_peer_ico = null;
                        }
                        else {
                            log("Logo set"); //--strip
                            r_peer_ico = sb;
                        }
                    }
                    catch (Exception e) {
                        error_manager.manage(e, e.getMessage() + "    " + e.toString());
                        log("KO 6997 - Exception " + e.getMessage()); //--strip
                        r_peer_ico = null;
                    }
                }
                refresh();
            }
        });
    }

    void queryico() {
        log("queryico. tries=" + tries); //--strip
        app.assert_ui_thread(); //--strip
        log("query logo"); //--strip
        assert tr != null;
        if (tries > 3) {
            log("tries " + tries + " Giving up retrieving ico"); //--strip
            Toast.makeText(getActivity().getApplicationContext(), getResources().getString(R.string.givingupretrievingico), 6000).show();
            return;
        }
        ++tries;
        log("tries " + tries); //--strip
        r_peer_ico = null;
        a.hmi.command_trade(tid, "request ico");
    }

    ImageView peer_ico_imageview(data_t data) {
        String sinitiator = data.find("initiator");
        if (sinitiator != null) {
            initiator = sinitiator.equals("Y");
        }
        return initiator ? pic_follower : pic_initiator; //If I am the initiator pic_follower is peer's pic.
    }

    void loadico() {
        log("loadico"); //--strip
        app.assert_ui_thread(); //--strip
        if (r_peer_ico != null) {
            log("r_peer_ico already loaded"); //--strip
            return;
        }
        log("get_sourceshit ico"); //--strip
        tr.get_sourceshit("ico", getActivity().getApplicationContext());
    }

    void refresh_peer_ico(data_t data) {
        boolean hasico = false;
        String icoval = data.find("ico");
        if (icoval != null) {
            if (icoval.equals("Y")) {
                hasico = true;
            }
        }
        log("hasico " + hasico); //--strip
        if (hasico) {
            if (r_peer_ico != null) {
                log("r_peer_ico != null -> display ico"); //--strip
                peer_ico_imageview(data).setImageBitmap(r_peer_ico);
            }
            else {
                log("refresh->loadico"); //--strip
                loadico();
            }
        }
        else {
            log("refresh->queryico"); //--strip
            //tries = 0;
            queryico();
        }
    }

    public void refresh() {
        if (tr == null) {
            log("KO 86677 REFRESH: activity is null"); //--strip
            return;
        }
        data_t data = tr.get_data();
        if (data == null) {
            tr.setmode_wrong("KO 43029");
            return;
        }
        data_btn.setVisibility(View.VISIBLE);
        String status = data.find("state");
        if (status != null) {
            if (status.equals("online")) {
                online.setChecked(true);
            }
            else {
                online.setChecked(false);
            }
        }
        else {
            online.setChecked(false);
        }

        String age = data.find("online_age");
        if (age == null) {
            onlinetime.setText("");
        }
        else {
            onlinetime.setText(age);
        }

        String ep1 = data.find("remote_endpoint");
        if (ep1 != null) {
            endpoint.setText(ep1);
        }
        if (data == null) {
            state.setText("...");
        }
        else {
            state.setText(data.find("state"));
            String rsn = data.find("reason");
            if (rsn == null) {
                reason.setText("");
            }
            else {
                reason.setText(rsn);
            }
        }
        String sinitiator = data.find("initiator");
        if (sinitiator != null) {
            initiator = sinitiator.equals("Y");
        }
        String peerp = getResources().getString(R.string.anonymous);
        String myp = getResources().getString(R.string.anonymous);
        String pper = data.find("peer_personality");
        if (pper != null) {
            peerp = pper;
        }
        String mper = data.find("peer_moniker");
        if (mper != null) {
            peerp += "\n" + mper;
        }

        String myper = data.find("my_personality");
        if (myper != null) {
            myp = myper;
        }
        String myperm = data.find("my_moniker");
        if (myperm != null) {
            myp += "\n" + myperm;
        }
        if (initiator) {
            personality_initiator.setText(myp);
            personality_follower.setText(peerp);
        }
        else {
            personality_initiator.setText(peerp);
            personality_follower.setText(myp);
        }
        initiator_title.setText(getResources().getString(R.string.initiator) + " (" + (initiator ? getResources().getString(R.string.me) : getResources().getString(R.string.peer)) + ")");
        follower_title.setText(getResources().getString(R.string.follower) + " (" + (!initiator ? getResources().getString(R.string.me) : getResources().getString(R.string.peer)) + ")");
        if (tr.cur_protocol == null) {
            select_protocol_role.setText(getResources().getString(R.string.select_protocol_role));
        }
        else {
            select_protocol_role.setText(getResources().getString(R.string.protocolandrole) + " Cur: " + tr.cur_protocol);
        }
        refresh_peer_ico(data);
        tr.setmode_ready();
    }

    void change_to_anon() {
        a.hmi.command_trade(tid, "change personality 0 Anonymous");
    }

    void on_personality_me(final String cur) {
        String[] options = {"Change to anonymous personality.", a.getResources().getString(R.string.cancel)};
        new AlertDialog.Builder(tr).setTitle("My Personality: " + cur).setItems(options,
            new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    switch(which) {
                        case 0:
                            change_to_anon();
                            break;
                    }
                }
            }).show();
    }

    void on_personality_peer(final String cur) {
    }

    private trader tr = (trader) getActivity();
    private app a;
    private hash_t tid;
    private boolean initiator = false;
    private MaterialButton select_protocol_role;
    private Switch online;
    private TextView endpoint;
    private TextView onlinetime;
    private TextView state;
    private TextView reason;
    private TextView personality_initiator;
    private TextView initiator_title;
    private ImageView pic_initiator;
    private TextView personality_follower;
    private TextView follower_title;
    private ImageView pic_follower;
    private ViewGroup container;
    private MaterialButton closetrade_btn;
    private MaterialButton create_bookmark_btn;
    private MaterialButton log_btn;
    private MaterialButton personality_exchange_btn;
    private MaterialButton data_btn;
    private MaterialButton reset;
    private MaterialButton reload;
    private LayoutInflater inflater;
    private View view;
    public Bitmap r_peer_ico = null;
    int tries = 0;
    int dispatchid;

}

