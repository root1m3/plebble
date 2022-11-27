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
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import android.widget.LinearLayout;                                                            // LinearLayout
import java.util.Map;                                                                          // Map
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import us.wallet.protocol;                                                                     // protocol
import us.gov.crypto.ripemd160;                                                                // ripemd160
import android.widget.TextView;                                                                // TextView
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View

public class workflow_view extends LinearLayout {

    static void log(final String s) {                       //--strip
        System.out.println("workflow_view: " + s);          //--strip
    }                                                       //--strip

    public workflow_view(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public void init(role_fragment rf_, String order_, workflow_item_view.on_click_listener click_handler_) {
        rf = rf_;
        if (order_.isEmpty()) {
            order = new String[0];
        }
        else {
            order = order_.split(" ");
        }
        click_handler = click_handler_;
    }

    public void add_view_after(final String itm, View v) {
        log("add_view_after " + itm); //--strip;
        int n;
        for (n = 0; n < order.length; ++n) {
            if (order[n].equals(itm)) {
                log("found ref view at index " + n); //--strip;
                addView(v, n + 1);
                return;
            }
        }
    }

    public boolean refresh() {
        data_t data = rf.tr.get_data();
        if (data == null) {
            return false;
        }
        log("refresh"); //--strip
        workflow_t wf = new workflow_t(rf.tr.get_data());
        LayoutInflater inflater = ((Activity)rf.getContext()).getLayoutInflater();
        removeAllViews();
        for (int i = 0; i < order.length; ++i) {
            pair<workflow_item_t, workflow_item_t> n = wf.get(order[i]);
            if (n == null) {
                continue;
            }
            workflow_item_view iv = (workflow_item_view) inflater.inflate(R.layout.workflow_item, null);
            iv.init(rf, n, click_handler);
            addView(iv);
        }
        return true;
    }

    public void set_action(String docname, String caption, int pic, View.OnClickListener l) {
        for (int i = 0; i < getChildCount(); i++) {
            final View child = getChildAt(i);
            if (child instanceof workflow_item_view) {
                workflow_item_view o = (workflow_item_view) child;
                if (o.local.name.equals(docname)) {
                    o.set_action(caption, pic, l);
                    return;
                }
            }
        }
    }

    public void disable_action(String docname) {
        for (int i = 0; i < getChildCount(); i++) {
            final View child = getChildAt(i);
            if (child instanceof workflow_item_view) {
                workflow_item_view o = (workflow_item_view) child;
                if (o.local.name.equals(docname)) {
                    o.disable_action();
                    return;
                }
            }
        }
    }

    role_fragment rf;
    workflow_t wf;
    String[] order;
    workflow_item_view.on_click_listener click_handler;
}

