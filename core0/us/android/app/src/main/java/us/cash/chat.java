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
import java.util.ArrayList;                                                                    // ArrayList
import android.os.AsyncTask;                                                                   // AsyncTask
import us.gov.crypto.base58;                                                                   // base58
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import android.os.Build;                                                                       // Build
import android.os.Bundle;                                                                      // Bundle
import us.gov.socket.datagram;                                                                 // datagram
import java.util.Date;                                                                         // Date
import android.widget.EditText;                                                                // EditText
//import com.google.firebase.crashlytics.FirebaseCrashlytics;                                    // FirebaseCrashlytics
import androidx.fragment.app.Fragment;                                                         // Fragment
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import android.widget.ImageButton;                                                             // ImageButton
import static us.gov.io.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import static us.ko.*;                                                                         // *
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import static us.wallet.trader.chat.*;                                                         // *
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import android.widget.LinearLayout;                                                            // LinearLayout
import androidx.recyclerview.widget.LinearLayoutManager;                                       // LinearLayoutManager
import java.util.Map;                                                                          // Map
import us.pair;                                                                                // pair
import us.wallet.protocol;                                                                     // protocol
import androidx.recyclerview.widget.RecyclerView;                                              // RecyclerView
import java.util.concurrent.locks.ReentrantLock;                                               // ReentrantLock
import android.widget.RelativeLayout;                                                          // RelativeLayout
import androidx.annotation.RequiresApi;                                                        // RequiresApi
import android.annotation.SuppressLint;                                                        // SuppressLint
import android.widget.TextView;                                                                // TextView
import java.util.Timer;                                                                        // Timer
import java.util.TimerTask;                                                                    // TimerTask
import java.util.concurrent.TimeUnit;                                                          // TimeUnit
import java.util.TreeMap;                                                                      // TreeMap
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View

public final class chat extends Fragment implements datagram_dispatcher_t.handler_t {

    private static void log(final String line) {    //--strip
       CFG.log_android("chat: " + line);            //--strip
    }                                               //--strip

    void set_handlers() {
        refreshbtn.setOnClickListener(v -> {
            log("refresh"); //--strip
            fetch();
        });

        sent.setOnClickListener(v -> {
            if (smessage.getText().toString().isEmpty()) {
                log("empty msg"); //--strip
                return;
            }
            app.assert_ui_thread(); //--strip
            log("MSG " + smessage.getText().toString()); //--strip
            a.hmi.command_trade(tid, "msg " + smessage.getText().toString());
            smessage.setText("");
        });
    }

    @RequiresApi(api = Build.VERSION_CODES.O)
    @SuppressLint("CutPasteId")
    @Override public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        log("OnCreate"); //--strip
        v = inflater.inflate(R.layout.fragment_chat, container, false);
        //progressbarcontainer = v.findViewById(R.id.progressbarcontainer);
        a = (app) getActivity().getApplication();
        smessage = v.findViewById(R.id.edittext_chatbox);
        sent = v.findViewById(R.id.button_chatbox_send);
        refreshbtn= v.findViewById(R.id.refreshbtn);
        message_recycler = v.findViewById(R.id.message_list);
        chat_header = v.findViewById(R.id.chat_header);
        LinearLayoutManager layout_manager = new LinearLayoutManager(v.getContext());
        layout_manager.setOrientation(LinearLayoutManager.VERTICAL);
        layout_manager.setStackFromEnd(true);
        layout_manager.setSmoothScrollbarEnabled(true);
        message_recycler.setLayoutManager(layout_manager);
        message_recycler.setItemViewCacheSize(20);
        message_recycler.setDrawingCacheEnabled(true);
        message_recycler.setDrawingCacheQuality(View.DRAWING_CACHE_QUALITY_HIGH);
        message_recycler.setHasFixedSize(true);
        Bundle bundle =  getArguments();
        tid = new hash_t(bundle.getByteArray("tid"));
        String lbl = bundle.getString("lbl");
        tx_title = v.findViewById(R.id.tx_title);
        endpoint_title = v.findViewById(R.id.endpoint_title);
        endpoint_title.setText(lbl == null ? "" : lbl);
        byte[] raw = bundle.getByteArray("raw");
        log("raw?" + (raw == null ? "N" : "Y")); //--strip
        tx_title.setText("id " + tid.encode());
        set_handlers();
        message_array = new ArrayList<>();
        message_adapter = new chat_adapter(v.getContext(), message_array);
        message_adapter.setHasStableIds(true);
        smessage.requestFocus();
        log("connect network-datagram hose"); //--strip
        dispatchid = a.hmi.dispatcher.connect_sink(this);
        set_payload(raw);
        return v;
    }

    @Override public void onDestroy() {
        super.onDestroy();
        log("onDestroy"); //--strip
        log("disconnect network-datagram hose");//--strip
        a.hmi.dispatcher.disconnect_sink(dispatchid);
    }

    @Override public void on_push(final hash_t target_tid, final uint16_t code, final byte[] payload) {
        log("on_push target_tid " + target_tid.encode() + " tid " + tid.encode()); //--strip
        if (!tid.equals(target_tid)) {
            log("not for me"); //--strip
            return;
        }
        switch(code.value) {
            case us.wallet.trader.trader_t.push_chat: {
                log("a chat for me. "); //--strip
                set_payload__worker(payload);
            }
            break;
        }
    }

    public void set_payload__worker(byte[] raw) {
        app.assert_worker_thread(); //--strip
        chat_t chat = new chat_t();
        blob_reader_t reader = new blob_reader_t(new blob_t(raw));
        ko r = chat.from_blob(reader);
        if (is_ko(r)) {
            log("chat corrupt. "); //--strip
            return;
        }
        log("received chat. lines " + chat.size()); //--strip
        update_shit(chat);
        paint__worker();
    }

    void paint__worker() {
        app.assert_worker_thread(); //--strip
        log("paint_chat_worker. scheduling for UI thread"); //--strip
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                paint();
            }
        });
    }

    public void set_payload(byte[] raw) {
        app.assert_ui_thread(); //--strip;
        if (raw == null) {
            log("fetch"); //--strip
            fetch();
        }
        else {
            log("raw copy available -> just paint"); //--strip
            chat_t chat = new chat_t();
            blob_reader_t reader = new blob_reader_t(new blob_t(raw));
            ko r = chat.from_blob(reader);
            if (is_ko(r)) {
                log("chat corrupt. 2."); //--strip
                return;
            }
            update_shit(chat);
            paint();
        }
    }

    void update_shit(final chat_t ch) {
        //ch.dump("chat> ", System.out);
        lock.lock();
        try {
            message_array.clear();
            for(Map.Entry<ts_t, us.wallet.trader.chat.chat_entry> entry : ch.entrySet()) {
                ts_t key = entry.getKey();
                us.wallet.trader.chat.chat_entry value = entry.getValue();
                Date d = new Date(TimeUnit.MILLISECONDS.convert(key.value, TimeUnit.NANOSECONDS));
                //chat_message cm = new chat_message();
                //cm.date = d;
                //cm.text = value.to_string();
                //log("TEXTHERE: " + cm.text); //--strip
                //cm.source = (value.me) ? chat_message.source_type.M : chat_message.source_type.P;
                //chat_messages.add(cm);

                message_t msg = new message_t(value.to_string(), value.me ? 0 : 1, d);
                message_array.add(msg);
            }
        }
        finally {
            lock.unlock();
        }
        log("last thing to do: updating Adapter"); //--strip
    }


    void paint() {
        app.assert_ui_thread(); //--strip
        log("paint chat"); //--strip
        message_recycler.setAdapter(message_adapter);
        boolean tracklast = true; //TODO scroll only if last item was visible
        if (tracklast) {
            TimerTask task = new TimerTask() {
                @Override public void run() {
                    if (message_adapter.getItemCount() > 0) {
                        message_recycler.smoothScrollToPosition(message_adapter.getItemCount() - 1);
                    }
                }
            };
            new Timer().schedule(task, 500);
        }
        //progressbarcontainer.setVisibility(View.GONE);
    }

    void fetch() {
        app.assert_ui_thread(); //--strip;
        //progressbarcontainer.setVisibility(View.VISIBLE);
        log("fetch - UI thread"); //--strip
        a.hmi.command_trade(tid, "show chat");
    }

    EditText smessage;
    ImageButton sent;
    TextView tx_title;
    TextView endpoint_title;
    toolbar_button refreshbtn;
    private Boolean exit = false;
    private RecyclerView message_recycler;
    private int REQUEST_CODE = 200;
    private app a;
    hash_t tid;
    //RelativeLayout progressbarcontainer;
    ReentrantLock lock = new ReentrantLock();
    ArrayList<message_t> message_array;
//    private ArrayList<chat_message> chat_messages = new ArrayList<chat_message>();
    private chat_adapter message_adapter;
    LinearLayout chat_header;
    int dispatchid;
    View v;
}

