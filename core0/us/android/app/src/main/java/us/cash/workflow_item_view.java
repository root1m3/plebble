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
import android.util.AttributeSet;                                                              // AttributeSet
import android.graphics.Color;                                                                 // Color
import android.content.Context;                                                                // Context
import java.util.Date;                                                                         // Date
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import android.widget.ImageView;                                                               // ImageView
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import us.ko;                                                                                  // ko
import android.widget.LinearLayout;                                                            // LinearLayout
import com.google.android.material.button.MaterialButton;                                      // MaterialButton
import com.google.android.material.card.MaterialCardView;                                      // MaterialCardView
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import android.widget.TextView;                                                                // TextView
import java.util.concurrent.TimeUnit;                                                          // TimeUnit
import android.view.View;                                                                      // View

public class workflow_item_view extends LinearLayout {

    static void log(final String s) { //--strip
        System.out.println("workflow_item_view: " + s); //--strip
    } //--strip

    public interface on_click_listener {
        public void on_send(workflow_item_view v);
        public void on_show(workflow_item_view v);
    }

    public workflow_item_view(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public void set_visible(boolean v) {
        setVisibility(v ? View.VISIBLE : View.GONE);
    }

    public String ts_words(long secs) {
        if (secs < 60) return "" + secs + " " + getResources().getString(R.string.seconds);
        long mins = secs / 60;
        secs -= mins * 60;
        if (mins < 60) return "" + mins + " " + getResources().getString(R.string.minutes) + " " + secs + " " + getResources().getString(R.string.seconds);
        long hours = mins / 60;
        mins -= hours * 60;
        if (hours < 24) return "" + hours + " " + getResources().getString(R.string.hours) + " " + mins + " " + getResources().getString(R.string.minutes);
        long days = hours / 24;
        hours -= days * 24;
        return "" + days + " " + getResources().getString(R.string.days) + " " + hours + " " + getResources().getString(R.string.hours);
    }

    public void init(role_fragment rf_, pair<workflow_item_t, workflow_item_t> i_, on_click_listener click_handler_) {
        rf = rf_;
        local = i_.first;
        remote = i_.second;
        click_handler = click_handler_;

        set_visible(local.has);
        if (!local.has) return;

        doc = workflow_item_doc.from_name(local.name);

        TextView title = findViewById(R.id.title);
        title.setText(activity.a.i18n.resolve(activity.a.i18n_sid(local.name)));

        ImageView pic = findViewById(R.id.pic);
        if (local.mode == workflow_item_t.mode_t.mode_send) {
            pic.setImageResource(R.raw.doc_up);
        }
        else {
            pic.setImageResource(R.raw.doc_down);
        }

        TextView st = findViewById(R.id.doc_st);
        TextView st2 = findViewById(R.id.doc_st2);
        TextView doc_explain = findViewById(R.id.doc_explain);

        boolean expired = false;
        if (local.expiry != null) {
            Date d = new Date(TimeUnit.MILLISECONDS.convert(local.expiry.value, TimeUnit.NANOSECONDS));
            Date now = new Date();
            long diff = (d.getTime() - now.getTime()) / 1000;
            if (diff < 0) {
                st.setText(getResources().getString(R.string.documentexpired) + " " + ts_words(-diff) + " " + getResources().getString(R.string.ago));
                st.setTextColor(Color.RED);
                expired = true;
            }
            else {
                st.setText(getResources().getString(R.string.documentexpiresin) + " " + ts_words(diff) + ".");
                st.setTextColor(Color.BLACK);
            }
        }
        else {
            Date d = new Date(TimeUnit.MILLISECONDS.convert(local.ts.value, TimeUnit.NANOSECONDS));
            Date now = new Date();
            long diff = (now.getTime() - d.getTime()) / 1000;
            if (diff < 0) {
                st.setText("KO 0118 " + getResources().getString(R.string.documentinfuture) + " " + ts_words(-diff) + ".");
                st.setTextColor(Color.RED);
            }
            else {
                st.setText(getResources().getString(R.string.documentupdated) + " " + ts_words(diff) + " " + getResources().getString(R.string.ago));
                st.setTextColor(Color.BLACK);
            }
        }

        MaterialButton button_action = findViewById(R.id.button_action);
        boolean btn_action_visible = false;
        if (local.mode == workflow_item_t.mode_t.mode_send && !expired) {
            if (remote.has) {
                if (remote.ts.value != local.ts.value) {
                    btn_action_visible = true;
                    st2.setText(getResources().getString(R.string.peerdifferentdocversion));
                    st2.setTextColor(Color.RED);
                }
                else {
                    st2.setText(getResources().getString(R.string.peerhasdocumentalready));
                    st2.setTextColor(darkgreen);
                }
                st2.setVisibility(View.VISIBLE);
            }
            else {
                btn_action_visible = true;
                st2.setText(getResources().getString(R.string.peerhasnotdocument));
                st2.setTextColor(Color.RED);
                st2.setVisibility(View.VISIBLE);
            }
        }
        button_action.setText(activity.a.i18n.resolve(activity.a.i18n_sid("send")));
        button_action.setVisibility(btn_action_visible ? View.VISIBLE : View.GONE);

        button_action.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                click_handler_.on_send(workflow_item_view.this);
            }
        });

        MaterialCardView cardviewbtn = findViewById(R.id.card);
        cardviewbtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                click_handler_.on_show(workflow_item_view.this);
            }
        });

    }

    public void set_action(String caption, int icon, View.OnClickListener l) {
        MaterialButton button_action = findViewById(R.id.button_action);
        button_action.setText(caption);
        button_action.setIconResource(icon);
        button_action.setOnClickListener(l);
        button_action.setVisibility(View.VISIBLE);
    }

    public void disable_action() {
        MaterialButton button_action = findViewById(R.id.button_action);
        button_action.setEnabled(false);
    }

    static int darkgreen = Color.parseColor("#009900");

    public trader tr;
    workflow_item_t local;
    workflow_item_t remote;
    workflow_item_doc doc;
    on_click_listener click_handler;
    role_fragment rf;

}

