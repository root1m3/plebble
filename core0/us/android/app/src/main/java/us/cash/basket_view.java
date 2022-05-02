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
import android.util.AttributeSet;                                                              // AttributeSet
import us.gov.crypto.base58;                                                                   // base58
import android.os.Bundle;                                                                      // Bundle
import android.content.Context;                                                                // Context
import us.gov.socket.datagram;                                                                 // datagram
import us.wallet.trader.data_t;                                                                // data_t
import java.util.HashMap;                                                                      // HashMap
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import android.widget.ImageView;                                                               // ImageView
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import android.widget.LinearLayout;                                                            // LinearLayout
import java.util.Map;                                                                          // Map
import com.google.android.material.button.MaterialButton;                                      // MaterialButton
import com.google.android.material.card.MaterialCardView;                                      // MaterialCardView
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import us.wallet.protocol;                                                                     // protocol
import us.gov.crypto.ripemd160;                                                                // ripemd160
import android.widget.TableLayout;                                                             // TableLayout
import android.widget.TextView;                                                                // TextView
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View

public class basket_view extends LinearLayout {

    static void log(final String line) {         //--strip
       CFG.log_android("basket_view: " + line);  //--strip
    }                                            //--strip

    public interface on_click_listener {
        public void on_remove(basket_item_view v);
        public void on_add_catalogue();
        public void on_add_scan();
        public void on_checkout();
    }

    public interface on_click_listener2 {
        public void on_select(basket_item_view v);
    }

    public basket_view(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    void init_() {
        title = findViewById(R.id.title);
        basket_status = findViewById(R.id.basket_status);
        recv_coin = findViewById(R.id.recv_coin);
        send_coin = findViewById(R.id.send_coin);
        total_pay = findViewById(R.id.total_pay);
        total_reward = findViewById(R.id.total_reward);
        controls = findViewById(R.id.controls);
        items = findViewById(R.id.items);
        icon = findViewById(R.id.icon);

    }

    public void init(activity ac_, on_click_listener click_handler_) {
        init_();
        mode = 0;
        ac = ac_;
        click_handler = click_handler_;
    }

    public void init2(activity ac_, on_click_listener2 click_handler_) {
        init_();
        mode = 1;
        ac = ac_;
        click_handler2 = click_handler_;
    }

    boolean refresh_mode1(basket_t b) {
        controls.setVisibility(View.GONE);
        title.setText("Catalogue");
        icon.setImageResource(R.raw.catalogue);
        return true;
    }

    boolean refresh_mode0(basket_t b) {
        pair<types.cash_t, types.cash_t> sums = b.value();
        total_pay.setText("Total pay: " + sums.first.value);
        total_reward.setText("Total reward: " + sums.second.value);
        MaterialButton add_cat = findViewById(R.id.itemlist);
        add_cat.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                click_handler.on_add_catalogue();
            }
        });
        MaterialButton add_scan = findViewById(R.id.scan_items);
        add_scan.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                click_handler.on_add_scan();
            }
        });
        MaterialButton checkout_btn = findViewById(R.id.checkout);
        checkout_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                click_handler.on_checkout();
            }
        });
        return true;
    }

    public void disable() {
        MaterialButton add_cat = findViewById(R.id.itemlist);
        add_cat.setEnabled(false);
        MaterialButton add_scan = findViewById(R.id.scan_items);
        add_scan.setEnabled(false);
        MaterialButton checkout_btn = findViewById(R.id.checkout);
        checkout_btn.setEnabled(false);
        for (int i = 0; i < items.getChildCount(); i++) {
            final View child = items.getChildAt(i);
            if (child instanceof basket_item_view) {
                basket_item_view o = (basket_item_view) child;
                o.disable();
            }
        }
    }

    public boolean refresh(basket_t b) {
        log("refresh " + b.size() + " Items"); //--strip
        basket_status.setText("> " + b.size() + " Items");

        if (pay_coin == null) {
            recv_coin.setVisibility(View.GONE);
        }
        else {
            recv_coin.setText("Pay coin: " + pay_coin);
        }

        if (reward_coin == null) {
            send_coin.setVisibility(View.GONE);
        }
        else {
            send_coin.setText("Reward coin: " + reward_coin);
        }
        LayoutInflater inflater = ac.getLayoutInflater();
        items.removeAllViews();
        for (Map.Entry<hash_t, pair<types.vol_t, basket_item_t>> i: b.entrySet()) {
            log("add card"); //--strip
            basket_item_view iv = (basket_item_view) inflater.inflate(R.layout.basket_item, null);
            if (mode == 0) {
                iv.init(ac, i.getKey(), i.getValue(), click_handler);
            }
            else {
                iv.init2(ac, i.getKey(), i.getValue(), click_handler2);
            }
            items.addView(iv);
        }

        if (mode == 0) return refresh_mode0(b);
        return refresh_mode1(b);
    }

    ImageView icon;
    TableLayout controls;
    LinearLayout items;
    TextView title;
    TextView basket_status;
    TextView recv_coin;
    TextView send_coin;
    TextView total_pay;
    TextView total_reward;
    activity ac;
    on_click_listener click_handler;
    on_click_listener2 click_handler2;
    int mode = 0;
    String pay_coin;
    String reward_coin;
}

