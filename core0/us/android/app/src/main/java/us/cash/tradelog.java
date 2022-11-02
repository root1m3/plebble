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
import us.gov.crypto.base58;                                                                   // base58
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import android.os.Build;                                                                       // Build
import android.os.Bundle;                                                                      // Bundle
import us.gov.socket.datagram;                                                                 // datagram
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.gov.io.types.*;                                                               // *
import static us.ko.*;                                                                         // *
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import us.ko;                                                                                  // ko
import android.view.Menu;                                                                      // Menu
import android.view.MenuItem;                                                                  // MenuItem
import us.pair;                                                                                // pair
import us.wallet.protocol;                                                                     // protocol
import android.widget.RelativeLayout;                                                          // RelativeLayout
import androidx.annotation.RequiresApi;                                                        // RequiresApi
import us.string;                                                                              // string
import android.widget.TextView;                                                                // TextView
import android.widget.Toast;                                                                   // Toast
import androidx.appcompat.widget.Toolbar;                                                      // Toolbar
import android.view.View;                                                                      // View

public final class tradelog extends activity implements datagram_dispatcher_t.handler_t  {

    static void log(final String line) {          //--strip
        CFG.log_android("tradelog: " + line);     //--strip
    }                                             //--strip

    @RequiresApi(api = Build.VERSION_CODES.O)
    @Override protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        set_content_layout(R.layout.activity_tradelog);
        tvlog = findViewById(R.id.log);
        Bundle bundle = getIntent().getExtras();
        tid = new hash_t(bundle.getByteArray("tid"));
        String ct = bundle.getString("content_type");
        log("content_type" + ct); //--strip
        if (ct.equals("data")) {
            content_type = ct;
            toolbar.setTitle(getResources().getString(R.string.datatrade) + " " + tid.encode());
        }
        else if (ct.equals("log")) {
            log("setTitle Log trade #" + tid.encode()); //--strip
            toolbar.setTitle("Log trade #" + tid.encode());
        }
        toolbar_button refresh = (toolbar_button) findViewById(R.id.refresh);
        refresh.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                fetch();
            }
        });
        log("connect network-datagram hose");//--strip
        dispatchid = a.hmi().dispatcher.connect_sink(this);
    }

    @Override public void onDestroy() {
        super.onDestroy();
        log("onDestroy"); //--strip
        log("disconnect network-datagram hose");//--strip
        a.hmi().dispatcher.disconnect_sink(dispatchid);
    }

   @Override public void onPause() {
        super.onPause();
        log("onPause"); //--strip
    }

    @Override public void onResume() {
        super.onResume();
        log("onResume"); //--strip
        fetch();
    }

    @Override public void on_push(final hash_t target_tid, final uint16_t code, final byte[] payload) {
        if(!tid.equals(target_tid)) return;
        switch(code.value) {
            case us.wallet.trader.trader_t.push_log: {
                if (content_type.equals("log")) {
                    log("log for me. "); //--strip
                    string s = new string();
                    ko r = blob_reader_t.parse(new blob_t(payload), s);
                    if (is_ko(r)) {
                        log(r.msg); //--strip
                        setlog(r.msg);
                        return;
                    }
                    setlog(s.value);
                }
            }
            break;
            case us.wallet.trader.trader_t.push_data: {
                if (content_type.equals("data")) {
                    log("data for me. "); //--strip
                    string s = new string();
                    ko r = blob_reader_t.parse(new blob_t(payload), s);
                    if (is_ko(r)) {
                        log(r.msg); //--strip
                        setlog(r.msg);
                        return;
                    }
                    setlog(s.value);
                }
            }
            break;
        }
    }

    void setlog(final String txt) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                String l = txt.replace('\r','\n');
                log("log: " + l); //--strip
                tvlog.setText(l);
                log("go away courtain"); //--strip
            }
        });
    }

    void fetch() {
        app.assert_ui_thread(); //--strip
        if (content_type.equals("log")) {
            tvlog.setText("retrieving log...");
            a.hmi().command_trade(tid, "show log");
        }
        else if (content_type.equals("data")) {
            tvlog.setText("retrieving data...");
            a.hmi().command_trade(tid, "show data");
        }
    }

    hash_t tid;
    private TextView tvlog;
    String content_type = "log";
    int dispatchid;
}

