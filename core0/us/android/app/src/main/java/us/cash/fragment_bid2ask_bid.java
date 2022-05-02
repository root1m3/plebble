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
import java.util.ArrayList;                                                                    // ArrayList
import us.gov.crypto.base58;                                                                   // base58
import android.graphics.BitmapFactory;                                                         // BitmapFactory
import android.graphics.Bitmap;                                                                // Bitmap
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import android.os.Bundle;                                                                      // Bundle
import android.widget.Button;                                                                  // Button
import us.gov.socket.datagram;                                                                 // datagram
import us.wallet.trader.data_t;                                                                // data_t
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import android.widget.ImageView;                                                               // ImageView
import static us.gov.io.types.*;                                                               // *
import static us.ko.*;                                                                         // *
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import android.content.Intent;                                                                 // Intent
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import com.google.android.material.button.MaterialButton;                                      // MaterialButton
import com.google.android.material.card.MaterialCardView;                                      // MaterialCardView
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import us.wallet.protocol;                                                                     // protocol
import us.gov.crypto.ripemd160;                                                                // ripemd160
import android.widget.TextView;                                                                // TextView
import android.widget.Toast;                                                                   // Toast
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View

public class fragment_bid2ask_bid extends role_fragment  {
    static final int SCAN_RESULT = 722;
    static final int SELITEMS_RESULT = 715;

    static void log(final String line) {                    //--strip
       CFG.log_android("fragment_bid2ask_bid: " + line);    //--strip
    }                                                       //--strip

    @Override
    public String init_cards() {
        return "cat inv pay rcpt";
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle saved_state) {
        log("onCreateView"); //--strip
        View v = super.onCreateView(inflater, container, saved_state);
        _basket_view = (basket_view) inflater.inflate(R.layout.basket, null);
        return v;
    }

    @Override public void on_push(final hash_t target_tid, final uint16_t code, byte[] payload) {
        super.on_push(target_tid, code, payload);
        if (!target_tid.equals(tid)) {
            log("not for me"); //--strip
            return;
        }
        switch(code.value) {
            case us.wallet.trader.workflow.trader_protocol.push_workflow_item: {
                log("a workflow_item for me"); //--strip
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        on_cat(payload);
                    }
                });
                return;
            }
        }
    }

    void on_cat(final byte[] payload) {
        cat = payload;
        if (cat != null) {
            select_items();
        }
    }

    void fetch_cat() {
        a.hmi.command_trade(tr.tid, "show cat");
    }

    void request_cat() {
        a.hmi.command_trade(tr.tid, "request cat");
    }

    void checkout() {
        a.hmi.command_trade(tr.tid, "request inv");
        scroll_down();
    }

    void pay() {
        a.hmi.command_trade(tr.tid, "inv_pay trade_" + tid.encode());
        scroll_down();
    }

    void scan_items() {
        Intent intent = new Intent(getActivity().getApplicationContext(), scan.class);
        intent.putExtra("what", 1);
        intent.putExtra("tid", tid.value);
        startActivityForResult(intent, SCAN_RESULT);
    }

    void select_items() {
        if (cat == null) {
            fetch_cat();
            return;
        }
        Intent intent = new Intent(tr.getApplicationContext(), sel_catalogue.class);
        intent.putExtra("title", "Select product");
        intent.putExtra("cat_blob", cat);
        tr.startActivityForResult(intent, SELITEMS_RESULT);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        log("onActivityResult "); //--strip
        if (resultCode != activity.RESULT_OK) {
            return;
        }
        if (requestCode != SELITEMS_RESULT && requestCode != SCAN_RESULT) {
            return;
        }
        if (data.hasExtra("selection")) {
            Bundle extras = data.getExtras();
            String item = extras.getString("selection");
            log("SEL: " + item); //--strip
            a.hmi.command_trade(tr.tid, "select " + item + " 1");
        }
    }

    @Override
    public boolean refresh() {
        boolean b = super.refresh();
        _workflow_view.add_view_after("cat", _basket_view);
        _basket_view.pay_coin = tr.get_data().find("remote__coin_pay");
        _basket_view.reward_coin = tr.get_data().find("remote__coin_reward");
        _basket_view.init(tr, new basket_view.on_click_listener() {
                @Override
                public void on_remove(basket_item_view v) {
                    log("on_remove" + v.prod.encode()); //--strip
                    a.hmi.command_trade(tr.tid, "select " + v.prod.encode() + " -1");
                }

                @Override
                public void on_add_catalogue() {
                    log("on_add_catalogue"); //--strip
                    select_items();
                }

                @Override
                public void on_add_scan() {
                    log("on_add_scan"); //--strip
                    scan_items();
                }

                @Override
                public void on_checkout() {
                    log("on_checkout"); //--strip
                    checkout();
                }
            }
        );
        basket_t basket = new basket_t();
        {
            String bb58 = tr.get_data().find("basket");
            if (bb58 == null) {
                return false;
            }
            byte[] blob = base58.decode(bb58);
            blob_reader_t reader = new blob_reader_t(new blob_t(blob));
            ko r = reader.read(basket);
            if (is_ko(r)) {
                return false;
            }
        }
        _basket_view.refresh(basket);
        _workflow_view.set_action("inv", getResources().getString(R.string.pay), R.drawable.ic_pay_48dp, new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    pay();
                }
            });
        if (trade_state == 7) {
            request_cat();
        }
        if (trade_state == 1) { //finished
            _basket_view.disable();
            _workflow_view.disable_action("inv");
            add_archive_button();
        }
        return b;
    }

    basket_view _basket_view;
    byte[] cat = null;

}

