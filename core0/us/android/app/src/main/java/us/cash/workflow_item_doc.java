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
import android.content.Context;                                                                // Context
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import us.ko;                                                                                  // ko
import com.google.android.material.button.MaterialButton;                                      // MaterialButton
import com.google.android.material.card.MaterialCardView;                                      // MaterialCardView
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import android.view.View;                                                                      // View

public class workflow_item_doc { //extends workflow_item {

    static void log(final String s) { //--strip
        System.out.println("workflow_item_doc: " + s); //--strip
    } //--strip

    public static interface status_listener_t {
        void on_status(workflow_item_doc doc);
    }

    public static workflow_item_doc from_name(String name) {

        if (name.equals("app")) {

        }
        return null;

    }

    public workflow_item_doc(Context ctx, View v, trader tr_, String data_Y_N_key_, boolean submit_, boolean expires_, int R_id, uint16_t code_, String cmd_content_, String cmd_action_, int icon_, String title_, int R_img, String explain, status_listener_t listener_) {
    }

    public void show(boolean enable_send) {
    }

    private MaterialButton button_send;
    private docwidget dw;
    public String data_Y_N_key;
    public boolean submit;
    public boolean expires;
    status_listener_t listener = null;
    public boolean I_have_it = false;
    public boolean different_or_expired = false;
    public docwidget.remote_doc_status remote_doc_status = docwidget.remote_doc_status.invisible;

}

