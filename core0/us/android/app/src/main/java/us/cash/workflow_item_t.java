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
import java.util.HashMap;                                                                      // HashMap
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import us.ko;                                                                                  // ko
import java.util.Map;                                                                          // Map
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import android.view.View;                                                                      // View

public class workflow_item_t {

    static void log(final String s) { //--strip
        System.out.println("workflow_item_t: " + s); //--strip
    } //--strip

    public enum mode_t {
        mode_send,
        mode_recv,

        num_modes
    }

    public workflow_item_t(String name_) {
        name = name_;
    }

    public void add(String prop, String value) {
        log("add name: " + name + " prop:" + prop + " value:" + value); //--strip
        if (prop == null) {
            if (value.equals("Y")) {
                has = true;
                return;
            }
            has = false;
            return;
        }
        if (prop.equals("mode")) {
            if (value.equals("recv")) {
                mode = mode_t.mode_recv;
            }
            return;
        }
        try {
            if (prop.equals("ts")) {
                ts = new uint64_t(Long.parseLong(value.trim()));
                return;
            }
        }
        catch(Exception e) {
            ts = new uint64_t(0);
        }
        try {
            if (prop.equals("expiry")) {
                expiry = new uint64_t(Long.parseLong(value.trim()));
                return;
            }
        }
        catch(Exception e) {
            expiry = new uint64_t(0);
        }
    }

    String name;
    boolean has = false;
    mode_t mode = mode_t.mode_send;
    uint64_t ts = null;
    uint64_t expiry = null;

}

