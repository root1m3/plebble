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
import android.content.Context;                                                                // Context
import us.wallet.trader.data_t;                                                                // data_t
import java.util.HashMap;                                                                      // HashMap
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import us.ko;                                                                                  // ko
import java.util.Map;                                                                          // Map
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import android.view.View;                                                                      // View

public class workflow_t extends HashMap<String, pair<workflow_item_t, workflow_item_t>> { //local, remote

    static void log(final String s) { //--strip
        System.out.println("workflow: " + s); //--strip
    } //--strip

    public workflow_t(data_t data) {
        log("workflow_t constructor"); //--strip
        for (Map.Entry<String, String> i: data.entrySet()) {
            if (i.getKey().startsWith("local__wf_")) {
                add(0, i.getKey().substring(10), i.getValue());
                continue;
            }
            if (i.getKey().startsWith("remote__wf_")) {
                add(1, i.getKey().substring(11), i.getValue());
                continue;
            }
        }
    }

    void add(int section, String prop, String value) {
        log("add " + section + " prop:" + prop + " val: " + value); //--strip
        String[] docprop = prop.split("_", 2);
        log("docprop " + docprop.length + " doc:" + docprop[0] + " val: " + value); //--strip
        pair<workflow_item_t, workflow_item_t> i = get(docprop[0]);
        if (i == null) {
            i = new pair<workflow_item_t, workflow_item_t>(new workflow_item_t(docprop[0]), new workflow_item_t(docprop[0]));
            put(docprop[0], i);
        }
        String wprop = null;
        if (docprop.length == 2) {
            wprop = docprop[1];
        }
        if (section == 0) {
            i.first.add(wprop, value);
        }
        else {
            i.second.add(wprop, value);
        }

    }

/*
    public void refresh(trader.data_t data) {
        for (Map.Entry<String, workflow_item> set : entrySet()) {
            set.getValue().refresh(data);
        }
    }

    public void add_redirection() {
        put("redir", new workflow_item_redirect(ctx, v, tr));
    }
*/
//    trader tr;

}

