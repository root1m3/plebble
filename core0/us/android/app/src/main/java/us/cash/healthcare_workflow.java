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

public class healthcare_workflow extends workflow_t {

    public static String layout = "healthcare_workflow";

    public static class appointment_t extends workflow_item_doc {
        public static uint16_t magic = new uint16_t(300);
        public static String name = "app";
        public static String long_name = "appointment";
        public appointment_t(Context ctx, View v, trader tr_, String data_Y_N_key_, boolean submit_, boolean expires_, int R_id, String cmd_content_, String cmd_action_, int icon_, String title_, int R_img, String explain, status_listener_t listener_) {
            super(ctx, v, tr_, data_Y_N_key_, submit_, expires_, R_id, magic, cmd_content_, cmd_action_, icon_, title_, R_img, explain, listener_);
        }
    }

    public static class prescription_t extends workflow_item_doc {
        public static uint16_t magic=new uint16_t(301);
        public static String name="pres";
        public static String long_name="prescription";
        public prescription_t(Context ctx, View v, trader tr_, String data_Y_N_key_, boolean submit_, boolean expires_, int R_id, String cmd_content_, String cmd_action_, int icon_, String title_, int R_img, String explain, status_listener_t listener_) {
            super(ctx, v, tr_, data_Y_N_key_, submit_, expires_, R_id, magic, cmd_content_, cmd_action_, icon_, title_, R_img, explain, listener_);
        }
    }

    public static class ai_request_t extends workflow_item_doc {
        public static uint16_t magic=new uint16_t(302);
        public static String name="aireq";
        public static String long_name="A.I. Request";
        public ai_request_t(Context ctx, View v, trader tr_, String data_Y_N_key_, boolean submit_, boolean expires_, int R_id, String cmd_content_, String cmd_action_, int icon_, String title_, int R_img, String explain, status_listener_t listener_) {
            super(ctx, v, tr_, data_Y_N_key_, submit_, expires_, R_id, magic, cmd_content_, cmd_action_, icon_, title_, R_img, explain, listener_);
        }
    }

    public static class ai_response_t extends workflow_item_doc {
        public static uint16_t magic=new uint16_t(303);
        public static String name="aires";
        public static String long_name="A.I. Response";
        public ai_response_t(Context ctx, View v, trader tr_, String data_Y_N_key_, boolean submit_, boolean expires_, int R_id, String cmd_content_, String cmd_action_, int icon_, String title_, int R_img, String explain, status_listener_t listener_) {
            super(ctx, v, tr_, data_Y_N_key_, submit_, expires_, R_id, magic, cmd_content_, cmd_action_, icon_, title_, R_img, explain, listener_);
        }
    }

    public static class ehr_t extends workflow_item_doc {
        public static uint16_t magic=new uint16_t(304);
        public static String name="ehr";
        public static String long_name="Electronic Health Records";
        public ehr_t(Context ctx, View v, trader tr_, String data_Y_N_key_, boolean submit_, boolean expires_, int R_id, String cmd_content_, String cmd_action_, int icon_, String title_, int R_img, String explain, status_listener_t listener_) {
            super(ctx, v, tr_, data_Y_N_key_, submit_, expires_, R_id, magic, cmd_content_, cmd_action_, icon_, title_, R_img, explain, listener_);
        }
    }

    static void log(final String s) {                           //--strip
        System.out.println("healthcare_workflow: " + s);        //--strip
    }                                                           //--strip

    public healthcare_workflow(data_t data) {
        super(data);
    }

    public void enable_appointment(boolean submit, workflow_item_doc.status_listener_t listener) {}
    public void enable_prescription(boolean submit, workflow_item_doc.status_listener_t listener) {}
    public void enable_ai_request(boolean submit, workflow_item_doc.status_listener_t listener) {}
    public void enable_ai_response(boolean submit, workflow_item_doc.status_listener_t listener) {}
    public void enable_ehr(boolean submit, workflow_item_doc.status_listener_t listener) {}

}

