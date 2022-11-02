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
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import android.widget.Toast;                                                                   // Toast
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View

public class logo_view extends LinearLayout {

    static void log(final String s) {                   //--strip
        System.out.println("logo_view: " + s);          //--strip
    }                                                   //--strip

    public logo_view(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public void init(role_fragment rf_) {
        rf = rf_;
        banner = findViewById(R.id.banner);
        banner.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                banner_clicked();
            }
        });
    }

    void querylogo() {
        log("querylogo. tries="+ tries); //--strip
        app.assert_ui_thread(); //--strip
        log("query logo"); //--strip
        if (tries > 3) {
            log("tries " + tries + " Giving up retrieving logo"); //--strip
            Toast.makeText(rf.getActivity().getApplicationContext(), getResources().getString(R.string.givingupretrievinglogo), 6000).show();
            return;
        }
        tries++;
        log("tries " + tries); //--strip
        rlogo = null;
        rf.a.hmi().command_trade(rf.tr.tid, "request logo");
    }

    void loadlogo() {
        log("loadlogo"); //--strip
        app.assert_ui_thread(); //--strip
        if (rlogo != null) {
            log("rlogo already loaded"); //--strip
            return;
        }
        log("get_sourceshit logo"); //--strip
        rf.tr.get_sourceshit("logo", rf.getActivity().getApplicationContext());
    }

    public void setlogo(final byte[] l) {
        app.assert_worker_thread(); //--strip
        log("setlogo " + l.length); //--strip
        rf.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                rlogo = BitmapFactory.decodeByteArray(l, 0, l.length);
                if (rlogo == null) {
                    log("KO 50059 Bitmap is null " + l.length); //--strip
                    rlogo = null;
                }
                else {
                    try {
                        log("banner is " + banner.getWidth() + " " + banner.getHeight()); //--strip
                        Bitmap sb = Bitmap.createScaledBitmap(rlogo, banner.getWidth(), banner.getHeight(), false);
                        if (sb == null) {
                            log("Bitmap is null (2) " + l.length); //--strip
                            rlogo = null;
                        }
                        else {
                            log("Logo set"); //--strip
                            rlogo = sb;
                        }
                    }
                    catch (Exception e) {
                        error_manager.manage(e, e.getMessage() + "    " + e.toString());
                        log("KO 6997 - Exception " + e.getMessage()); //--strip
                        rlogo = null;
                    }
                }
                refresh();
            }
        });
    }

    public void banner_clicked() {
    }

    public boolean refresh() {
        log("refresh"); //--strip
        data_t data = rf.tr.get_data();
        assert data != null;

        boolean haslogo = false;
        String logoval = data.find("logo");
        if (logoval != null) {
            if (logoval.equals("Y")) {
                haslogo = true;
            }
        }
        log("haslogo " + haslogo); //--strip
        if (haslogo) {
            if (rlogo != null) {
                log("rlogo!=null -> display image"); //--strip
                banner.setImageBitmap(rlogo);
            }
            else {
                log("refresh->loadlogo"); //--strip
                loadlogo();
            }
        }
        else {
            log("refresh->querylogo "); //--strip
            tries = 0;
            querylogo();
        }
        return true;
    }

    View v = null;
    public Bitmap rlogo = null;
    public ImageView banner;
    int tries = 0;
    role_fragment rf;

}

