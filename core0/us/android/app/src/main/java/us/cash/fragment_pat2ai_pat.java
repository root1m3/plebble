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
import android.app.AlertDialog;                                                                // AlertDialog
import java.util.ArrayList;                                                                    // ArrayList
import us.gov.crypto.base58;                                                                   // base58
import android.graphics.BitmapFactory;                                                         // BitmapFactory
import android.graphics.Bitmap;                                                                // Bitmap
import android.os.Bundle;                                                                      // Bundle
import android.widget.Button;                                                                  // Button
import android.graphics.Color;                                                                 // Color
import us.gov.socket.datagram;                                                                 // datagram
import us.wallet.trader.data_t;                                                                // data_t
import android.content.DialogInterface;                                                        // DialogInterface
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import android.widget.ImageView;                                                               // ImageView
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import android.content.Intent;                                                                 // Intent
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import android.util.Log;                                                                       // Log
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

public class fragment_pat2ai_pat extends role_fragment {
    View v;

    static void log(final String line) {         //--strip
       CFG.log_android("fragment_pat2ai_pat: " + line);     //--strip
    }                                            //--strip

    @Override
    public String init_cards() {
        return "aireq ehr aires";
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        v = super.onCreateView(inflater, container, savedInstanceState);
        if (v == null) return null;
        create_ai_request = tr.findViewById(R.id.action);
        create_ai_request.set_stock_image(3);
        create_ai_request.set_custom_text(a.i18n.resolve(app.i18n_t.sid.new_));
        create_ai_request.setVisibility(View.VISIBLE);
        setup_handlers();
        return v;
    }

    void button_proc_create_ai_request(data_t data, Button b, final String thing) {
        if (data == null) {
            b.setVisibility(View.GONE);
            return;
        }
        boolean hasaireq = false;
        String appval = data.find("local__ai_request");
        if (appval != null) {
            if (appval.equals("Y")) {
                hasaireq = true;
            }
        }
        if (hasaireq) {
            b.setVisibility(View.GONE);
            return;
        }
        button_proc(data, b, thing);
    }

   void button_proc(data_t data, Button b, final String thing) {
        if (data == null) {
            b.setVisibility(View.GONE);
            return;
        }
        boolean hasthing=false;
        String appval = data.find(thing);
        if (appval != null) {
            if (appval.equals("Y")) {
                hasthing = true;
            }
        }
        b.setVisibility(hasthing ? View.VISIBLE : View.GONE);
    }

    void button_proc_syncrem(data_t data, docwidget b, final String thing) {
        if (data == null) {
            b.set_visible(false);
            return;
        }
        boolean hasthing=false;
        String appval = data.find(thing);
        if (appval != null) {
            if (!appval.equals("N")) {
                hasthing = true;
            }
        }
        String appvalr = data.find("remote__" + thing);
        if (appvalr != null) {
            if (appvalr.equals("Y")) {
                hasthing=false;
            }
        }
        b.set_visible(hasthing);
    }

    void on_create_ai_request(final String type) {
        a.hmi.command_trade(tr.tid, "create_aireq " + type + " " + "es");
        Toast.makeText(tr.getApplicationContext(), getResources().getString(R.string.pleasewait), 500).show();
    }

    @Override public boolean refresh() {
        boolean b = super.refresh();
        if (trade_state == 13) { //finished
            add_archive_button();
        }
        return b;
    }

    String[] parse_types() {
        data_t data = tr.get_data();
        if (data == null) {
            return null;
        }
        boolean has=false;
        String types = data.find("remote__request_types");
        if (types == null) return null;
        String[] x = types.split("\\s",2);
        if (x.length != 2) return null;
        int n = 0;
        try {
            n = Integer.parseInt(x[0]);
        }
        catch (Exception e) {
            error_manager.manage(e, e.getMessage() + ". " + e.toString());
            return null;
        }
        String[] r = x[1].split("\\s");
        if (r.length != n) return null;
        return r;
    }

    void setup_handlers() {
        create_ai_request.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String type;
                String[] options = parse_types();
                if (options!=null) {
                    new AlertDialog.Builder(getActivity()).setTitle(getResources().getString(R.string.selectrequesttype))
                        .setItems(options, new DialogInterface.OnClickListener() {
                           @Override
                           public void onClick(DialogInterface dialog, int which) {
                                if (which >= 0 && which < options.length) {
                                    on_create_ai_request(options[which]);
                                }
                           }
                       })
                       .show();

                }
                else {
                    on_create_ai_request("unspecified");
                }
            }
        });
    }

    private MaterialCardView referrer;
    private toolbar_button create_ai_request;
}

