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
import android.app.Application;                                                                // Application
import android.widget.ArrayAdapter;                                                            // ArrayAdapter
import java.util.ArrayList;                                                                    // ArrayList
import android.graphics.Color;                                                                 // Color
import android.content.Context;                                                                // Context
import us.gov.socket.datagram;                                                                 // datagram
import java.util.Date;                                                                         // Date
import us.wallet.trader.endpoint_t;                                                            // endpoint_t
import android.widget.FrameLayout;                                                             // FrameLayout
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import android.widget.ImageView;                                                               // ImageView
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import android.widget.LinearLayout;                                                            // LinearLayout
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import java.util.Random;                                                                       // Random
import java.text.SimpleDateFormat;                                                             // SimpleDateFormat
import android.widget.TextView;                                                                // TextView
import java.util.concurrent.TimeUnit;                                                          // TimeUnit
import android.util.TypedValue;                                                                // TypedValue
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View

public class trade implements Comparable<trade> {

    static void log(final String line) {                        //--strip
        System.out.println("trade: " + line);                   //--strip
    }                                                           //--strip

    public trade(String line0) {
        String line = line0.trim();
        if (line.isEmpty()) {
            log("empty line. tid = null");  //--strip
            return;
        }
        String[] field = line.split("\\s", 6);
        if (field.length == 6) {
            tid = new hash_t(field[0]);
            ts_creation = Long.parseLong(field[1]);
            ts_activity = Long.parseLong(field[2]);
            caption = field[3] + ' ' + field[4] + ' ' + field[5];
        }
    }

    public String caption(int field) {
        if (field == 3) {
            Date d = new Date();
            d.setTime(ts_activity);
            Date n = new Date();
            return "Idle for " + TimeUnit.SECONDS.toSeconds(n.getTime() - d.getTime()) + " seconds." + ts_activity;
        }
        return caption;
    }

    @Override public int compareTo(trade other) {
        if (ts_activity < other.ts_activity) return 1;
        if (ts_activity > other.ts_activity) return -1;
        return 0;
    }

    public hash_t tid = null;
    public Long ts_creation;
    public Long ts_activity;
    public String ts_activity2;
    public String caption;
}

