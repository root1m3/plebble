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
import android.graphics.Color;                                                                 // Color
import android.content.Context;                                                                // Context
import us.wallet.trader.data_t;                                                                // data_t
import java.util.Date;                                                                         // Date
import android.widget.ImageView;                                                               // ImageView
import com.google.android.material.button.MaterialButton;                                      // MaterialButton
import com.google.android.material.card.MaterialCardView;                                      // MaterialCardView
import android.widget.TextView;                                                                // TextView
import java.util.concurrent.TimeUnit;                                                          // TimeUnit
import android.view.View;                                                                      // View

public class docwidget {

    public Context ctx;
    public MaterialCardView cardviewbtn;
    public TextView st;
    public TextView st2;
    public TextView doc_explain;
    public ImageView pic;

    static void log(final String line) {         //--strip
       CFG.log_android("docwidget: " + line);    //--strip
    }                                            //--strip

    public docwidget(Context ctx_, View v, int R_img, String explain) {  //int idbtn, int idst, int idst2
        ctx = ctx;
        cardviewbtn=(MaterialCardView)v;
        assert cardviewbtn != null;
        st=v.findViewById(R.id.doc_st);
        st2=v.findViewById(R.id.doc_st2);
        assert st != null;
        assert st2 != null;
        doc_explain=v.findViewById(R.id.doc_explain);
        pic=v.findViewById(R.id.pic);
        assert doc_explain != null;
        assert pic != null;
        pic.setImageResource(R_img);
        doc_explain.setText(explain);
    }

    public void set_visible(boolean v) {
        cardviewbtn.setVisibility(v?View.VISIBLE:View.GONE);
    }

    public boolean is_visible() {
        return cardviewbtn.getVisibility()==View.VISIBLE;
    }

    public void set_btn(String s) {
    }

    public void set_st(String s) {
    }

    public String entxt(long secs) {
        if (secs < 60) return "" + secs + " " + ctx.getResources().getString(R.string.seconds);
        long mins = secs / 60;
        secs -= mins * 60;
        if (mins < 60) return "" + mins + " " + ctx.getResources().getString(R.string.minutes) + " " + secs + " " + ctx.getResources().getString(R.string.seconds);
        long hours = mins / 60;
        mins -= hours*60;
        if (hours < 24) return "" + hours + " " + ctx.getResources().getString(R.string.hours) + " " + mins + " " + ctx.getResources().getString(R.string.minutes);
        long days = hours / 24;
        hours -= days * 24;
        return "" + days + " " + ctx.getResources().getString(R.string.days) + " " + hours + " " + ctx.getResources().getString(R.string.hours);
    }

    public boolean st_proc_expiry(data_t data, final String thing) {
        if (!is_visible()) return false;
        if (data == null) {
            st.setTextColor(Color.RED);
            st.setText("KO 1850");
            return false;
        }
        String val = data.find(thing + "_expiry_ts");
        if (val==null) {
            st.setTextColor(Color.RED);
            st.setText("KO 2041");
            return false;
        }
        Long nanosec=Long.parseLong(val.trim());
        if (nanosec==0) {
            st.setTextColor(Color.RED);
            st.setText("KO 2001");
            return false;
        }
        Date d = new Date(TimeUnit.MILLISECONDS.convert(nanosec, TimeUnit.NANOSECONDS));
        Date now = new Date();
        long diff = (d.getTime() - now.getTime()) / 1000;
        if (diff < 0) {
            st.setText(ctx.getResources().getString(R.string.documentexpired) + " " + entxt(-diff) + " " + ctx.getResources().getString(R.string.ago));
            st.setTextColor(Color.RED);
            return false;
        }
        else {
            st.setText(ctx.getResources().getString(R.string.documentexpiresin) + " " + entxt(diff) + ".");
            st.setTextColor(Color.BLACK);
            return true;
        }
    }

    public boolean st_proc(data_t data, final String thing) {
        if (!is_visible()) return false;
        if (data == null) {
            st.setTextColor(Color.RED);
            st.setText("KO 1850");
            return false;
        }
        String val = data.find(thing + "_ts");
        if (val==null) {
            st.setTextColor(Color.RED);
            st.setText("KO 2041");
            return false;
        }

        Long nanosec=Long.parseLong(val.trim());
        if (nanosec == 0) {
            st.setTextColor(Color.RED);
            st.setText("KO 2001");
            return false;
        }
        Date d = new Date(TimeUnit.MILLISECONDS.convert(nanosec, TimeUnit.NANOSECONDS));
        Date now = new Date();
        long diff = (now.getTime() - d.getTime()) / 1000;
        if (diff < 0) {
            st.setTextColor(Color.RED);
            st.setText("KO 0118 " + ctx.getResources().getString(R.string.documentinfuture) + " " + entxt(-diff) + ".");
            return false;
        }
        else {
            st.setTextColor(Color.BLACK);
            st.setText(ctx.getResources().getString(R.string.documentupdated) + " " + entxt(diff) + " " + ctx.getResources().getString(R.string.ago));
            return true;
        }
    }

    static int darkgreen = Color.parseColor("#009900");

    public enum remote_doc_status {
        invisible,
        missing,
        diff,
        same
    }

    // Flow submit (me -> peer)
    // -1 Invisible
    // 1 Peer hasn't got this document
    // 2 Peer has a different version of this document
    // 3 Peer already has this document
    public remote_doc_status st2_proc_me_to_peer(data_t data, final String lprefix, final String rprefix, final String thing, final String suffix) {
        log("st2_proc_me_to_peer "+thing); //--strip
        if (!is_visible()) {
            log("no visible"); //--strip
            return remote_doc_status.invisible;
        }
        if (data == null) {
            st2.setText("");
            st2.setVisibility(View.GONE);
            log("data=null"); //--strip
            return remote_doc_status.invisible;
        }
        String lval = data.find(lprefix + thing + suffix);
        if (lval==null) {
            st2.setText("");
            st2.setVisibility(View.GONE);
            log("lval=null "+lprefix + thing + suffix); //--strip
            return remote_doc_status.invisible;
        }
        log("lval="+lval+" ["+lprefix + thing + suffix+"]"); //--strip

        long lnanosec=Long.parseLong(lval.trim());
        if (lnanosec==0) {
            st2.setText("");
            st2.setVisibility(View.GONE);
            log("l ts=0 "+lval); //--strip
            return remote_doc_status.invisible;
        }

        String rval = data.find(rprefix + thing + suffix);
        if (rval==null) {
            st2.setText("");
            st2.setVisibility(View.GONE);
            log("rval=null "+rprefix + thing + suffix); //--strip
            return remote_doc_status.invisible;
        }
        log("rval="+rval+" ["+rprefix + thing + suffix+"]"); //--strip

        long rnanosec=Long.parseLong(rval.trim());
        if (rnanosec==0) {
            st2.setText(ctx.getResources().getString(R.string.peerhasnotdocument));
            st2.setTextColor(Color.RED);
            st2.setVisibility(View.VISIBLE);
            log("r ts=0 "+rval); //--strip
            return remote_doc_status.missing;
        }

        if (lnanosec!=rnanosec) {
            st2.setText(ctx.getResources().getString(R.string.peerdifferentdocversion));
            st2.setTextColor(Color.RED);
            st2.setVisibility(View.VISIBLE);
            log("diff "); //--strip
            return remote_doc_status.diff;
        }
        st2.setText(ctx.getResources().getString(R.string.peerhasdocumentalready));
        st2.setTextColor(darkgreen);
        st2.setVisibility(View.VISIBLE);
        log("synced "); //--strip
        return remote_doc_status.same;
    }

}

