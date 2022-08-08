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
import android.graphics.drawable.ColorDrawable;                                                // ColorDrawable
import android.graphics.Color;                                                                 // Color
import android.widget.ImageView;                                                               // ImageView
import android.view.View;                                                                      // View

public class leds_t {

    static void log(final String line) {          //--strip
        CFG.log_android("leds_t: " + line);       //--strip
    }                                             //--strip

    static final int off = Color.parseColor("#a0a0a0");
    static final int amber = Color.parseColor("#FFBF00");
    public static final ColorDrawable led_off = new ColorDrawable(off);
    public static final ColorDrawable led_red = new ColorDrawable(Color.RED);
    public static final ColorDrawable led_amber = new ColorDrawable(amber);
    public static final ColorDrawable led_green = new ColorDrawable(Color.GREEN);
    public static final ColorDrawable led_blue = new ColorDrawable(Color.CYAN);

    public leds_t(activity ac_, View view_) {
        view = view_;
        ac = ac_;
        led[0] = view.findViewById(R.id.pb_send);
        led[1] = view.findViewById(R.id.pb_receive);
        led[2] = view.findViewById(R.id.pb_online);
        log("led[0]=" + led[0] + " led[1]=" + led[1] + " led[2]=" + led[2]); //--strip
        set_all_off();
        //set_led(0, led_off);
        //set_led(1, led_off);
        //set_led(2, led_off);
    }

    public void set_visibility(final boolean b) {
        view.setVisibility(b ? View.VISIBLE : View.INVISIBLE);
    }

    public void set_all_off() {
        ac.runOnUiThread(new Runnable() {
            @Override public void run() {
//                log("Leds set. UI th."); //--strip
                led[0].setImageDrawable(led_off);
                led[1].setImageDrawable(led_off);
                led[2].setImageDrawable(led_off);
                //led[0].invalidate();
                //led[1].invalidate();
                //led[2].invalidate();
            }
        });
    }

    public void set_led(final int i, final ColorDrawable color) { // -1 off
        if (led[i] == null) {
            //log("Led " + i + " not set"); //--strip
            return;
        }
        //log("Led " + i + " set"); //--strip
        ac.runOnUiThread(new Runnable() {
            @Override public void run() {
                //log("Led " + i + " set. UI th."); //--strip
                led[i].setImageDrawable(color);
                //led[i].invalidate();
            }
        });
    }

    ImageView led[] = new ImageView[3];
    View view;
    activity ac;
}

