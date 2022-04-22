//===-                           P L E B B L E
//===-                         https://plebble.us
//===-
//===-              Copyright (C) 2017-2022 root1m3@plebble.us
//===-
//===-                      GNU GENERAL PUBLIC LICENSE
//===-                       Version 3, 29 June 2007
//===-
//===-    This program is free software: you can redistribute it and/or modify
//===-    it under the terms of the AGPLv3 License as published by the Free
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
import android.util.AttributeSet;                                                              // AttributeSet
import android.graphics.BitmapFactory;                                                         // BitmapFactory
import android.graphics.Bitmap;                                                                // Bitmap
import android.os.Bundle;                                                                      // Bundle
import android.content.Context;                                                                // Context
import us.gov.socket.datagram;                                                                 // datagram
import us.wallet.trader.data_t;                                                                // data_t
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
import com.google.android.material.card.MaterialCardView;                                      // MaterialCardView
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import android.widget.TextView;                                                                // TextView
import android.widget.Toast;                                                                   // Toast
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View

public class tip_view extends LinearLayout {

    static void log(final String s) { //--strip
        System.out.println("tip_view: " + s); //--strip
    } //--strip

    public tip_view(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public void init(role_fragment rf_) {
        rf = rf_;
        tip_explain = findViewById(R.id.tip_explain);
        state = findViewById(R.id.state);
        card = findViewById(R.id.card);
        card.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                toast_info();
            }
        });
        set_tip("");
        set_state("0 Ready");
    }

    void toast_info() {
        String m = "";
        String s2 = state.getText().toString();
        if (!s2.isEmpty()) {
            m += getResources().getString(R.string.status) + ": " + s2 +"\n";
        }

        String s = tip_explain.getText().toString();
        if (!s.isEmpty()) {
            m += getResources().getString(R.string.hint) + ": " + s +"\n";
        }
        Toast.makeText(rf.tr.getApplicationContext(), m, 6000).show();
    }

    public void set_tip(final String tip) {
        log("set tip:" + tip); //--strip
        activity.main.runOnUiThread(new Runnable() {
            public void run() {
                tip_explain.setText(tip);
                tip_explain.setVisibility(View.VISIBLE);
//                tip_explain.setVisibility(tip.isEmpty() ? View.GONE : View.VISIBLE);
            }
        });
    }

    public void set_state(final String st) {
        final String[] x = st.split(" ", 2);
        log("set state:" + x[0]); //--strip
        id = new uint32_t(x[0]);
        activity.main.runOnUiThread(new Runnable() {
            public void run() {
                state.setText(x[1]);
//                card.setVisibility(id.value == 0 ? View.GONE : View.VISIBLE);
                card.setVisibility(View.VISIBLE);
            }
        });
    }

    public boolean refresh() {
        log("refresh"); //--strip
        data_t data = rf.tr.get_data();
        assert data != null;

        String st = data.find("trade_state");
        if (st != null) {
            set_state(st);
        }

        String hint = data.find("user_hint");
        if (hint != null) {
            set_tip(hint);
        }
        return true;
    }


/*
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle saved_instance) {
        view parent = super.onCreateView(inflater, container, saved_instance);
        v = inflater.inflate(R.layout.tip, container, false);
        if (parent != null) {
            parent.addView(v);
            return parent;
        }
        return v;
    }
*/
    public TextView tip_explain;
    public TextView state;
    MaterialCardView card;
    role_fragment rf;
    public uint32_t id = new uint32_t(0);

}

