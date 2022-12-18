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
import android.view.LayoutInflater;                                                            // LayoutInflater
import us.pair;                                                                                // pair
import us.string;                                                                              // string
import android.widget.Toast;  
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View
import android.os.Bundle;                                                                      // Bundle
import android.widget.Button;

public class fragment_b2r_b extends role_fragment  {

    static void log(final String line) {                      //--strip
        CFG.log_android("fragment_b2r_b: " + line);    //--strip
    }                                                         //--strip

    public fragment_b2r_b() {
        under_construction = true;
    }


    @Override public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle saved_state) {
        View v = super.onCreateView(inflater, container, saved_state);

        log("onCreateView"); //--strip

        Button myButton = new Button(getContext());
        myButton.setText("ask for resolution about exporting item id=857467634");

        myButton.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                on_click();
            }
        });

        content.addView(myButton);

        return v;
    }

    @Override public String init_cards() {
        log("init_cards"); //--strip
        String cards = "";
        return cards;
    }


    void on_click() {
        a.hmi().command_trade(tid, "request_resolution");
    }
}

