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
import android.os.Bundle;                                                                      // Bundle
import android.graphics.Color;                                                                 // Color
import us.gov.socket.datagram;                                                                 // datagram
import us.wallet.trader.data_t;                                                                // data_t
import static android.graphics.BitmapFactory.decodeResource;                                   // decodeResource
import android.app.Fragment;                                                                   // Fragment
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import android.content.Intent;                                                                 // Intent
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import android.widget.TextView;                                                                // TextView
import android.widget.Toast;                                                                   // Toast
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View

public class fragment_pat2slt_slt extends role_fragment  {

    public enum level_t {
        GP,
        specialist
    }

    static void log(final String line) {                    //--strip
       CFG.log_android("fragment_pat2slt_slt: " + line);    //--strip
    }                                                       //--strip

    void init_level() {
        data_t data = tr.get_data();
        String slevel = data.find("local__level");
        if (slevel != null) {
            if (slevel.equals("0")) {
                level = level_t.GP;
            }
            else if (slevel.equals("1")) {
                level = level_t.specialist;
            }
            else {
                level = level_t.GP;
            }
        }
        else {
            level = level_t.GP;
        }
    }

    @Override public String init_cards() {
        log("init_cards"); //--strip
        init_level();
        String cards;
        if (is_GP()) {
            log("is GP"); //--strip
            cards = "app";
//            tr.openchat(null);
        }
        else {
            log("is DERM"); //--strip
            cards = "app aireq aires pres";
        }
        return cards;
    }

    public boolean is_GP() {
        return level == level_t.GP;
    }

    public boolean is_specialist() {
        return level == level_t.specialist;
    }

    private level_t level = level_t.GP;

}

