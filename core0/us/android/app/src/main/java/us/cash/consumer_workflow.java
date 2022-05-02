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
import android.os.Bundle;                                                                      // Bundle
import android.content.Context;                                                                // Context
import us.gov.socket.datagram;                                                                 // datagram
import us.wallet.trader.data_t;                                                                // data_t
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View

public class consumer_workflow extends workflow_t {

    public static String layout = "consumer_workflow";

    static void log(final String line) {                //--strip
       CFG.log_android("consumer_workflow: " + line);   //--strip
    }                                                   //--strip

    public static class invoice_t extends workflow_item_doc {
        public static uint16_t magic=new uint16_t(203);
        public static String name="inv";
        public static String long_name="invoice";
        public invoice_t(Context ctx, View v, trader tr_, String data_Y_N_key_, boolean submit_, boolean expires_, int R_id, String cmd_content_, String cmd_action_, int icon_, String title_, int R_img, String explain, status_listener_t listener_) {
            super(ctx, v, tr_, data_Y_N_key_, submit_, expires_, R_id, magic, cmd_content_, cmd_action_, icon_, title_, R_img, explain, listener_);
        }
    }

    public static class bill_t extends workflow_item_doc {
        public static uint16_t magic=new uint16_t(205);
        public static String name="bill";
        public static String long_name="bill";
        public bill_t(Context ctx, View v, trader tr_, String data_Y_N_key_, boolean submit_, boolean expires_, int R_id, String cmd_content_, String cmd_action_, int icon_, String title_, int R_img, String explain, status_listener_t listener_) {
            super(ctx, v, tr_, data_Y_N_key_, submit_, expires_, R_id, magic, cmd_content_, cmd_action_, icon_, title_, R_img, explain, listener_);
        }
    }

    public consumer_workflow(data_t data) {
        super(data);
    }

    public void enable_invoice(boolean submit, workflow_item_doc.status_listener_t listener) {
    }

    public void enable_bill(boolean submit, workflow_item_doc.status_listener_t listener) {
    }

}

