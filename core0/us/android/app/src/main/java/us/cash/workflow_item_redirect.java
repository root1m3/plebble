//===-                           P L E B B L E
//===-                         https://plebble.us
//===-
//===-              Copyright (C) 2017-2022 root1m3@plebble.us
//===-
//===-                      GNU GENERAL PUBLIC LICENSE
//===-                       Version 3, 29 June 2007
//===-
//===-    This program is free software: you can redistribute it and/or modify
//===-    it under the terms of the AGPLv3 License as published by the Free
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
import us.wallet.trader.bookmark_t;                                                            // bookmark_t
import android.content.Context;                                                                // Context
import com.google.android.material.button.MaterialButton;                                      // MaterialButton
import com.google.android.material.card.MaterialCardView;                                      // MaterialCardView
import android.widget.TextView;                                                                // TextView
import android.view.View;                                                                      // View

class workflow_item_redirect_off { //extends workflow_item {

    static void log(final String s) { //--strip
        System.out.println("workflow_item_redirect: "+s); //--strip
    } //--strip

    public workflow_item_redirect_off(Context ctx, View v, trader tr) {
/*
        super(ctx, v, tr);
        redirect= v.findViewById(R.id.redirection);
        redirecttext=v.findViewById(R.id.redirectiontext);
        redirect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                on_redirect_click();
            }
        });
*/
    }

/*
    void on_redirect_click() {
        if (bookmark == null) {
            log("KO 60596 No redirection available"); //--strip
            return;
        }
        log("redirection "+bookmark.label); //--strip
        tr.on_redirect(bookmark);
    }

    @Override
    void refresh(trader.data_t data) {
        if (data == null) {
            redirect.setVisibility(View.GONE);
            redirecttext.setVisibility(View.GONE);
            return;
        }
        boolean hasthing = false;
        String raw = data.find("redirect");
        if (raw != null) {
            hasthing = true;
            bookmark = new bookmark_t();
            bookmark.from_string(raw);
//            String[] s=raw.split("-",2);
//            if (s.length==2) {
            redirecttext.setText(v.getResources().getString(R.string.redirection)+"\n" + bookmark.label);
//            }
//            else {
//                redirecttext.setText(v.getResources().getString(R.string.redirection));
//            }
        }
        redirect.setVisibility(hasthing?View.VISIBLE:View.GONE);
        redirecttext.setVisibility(hasthing?View.VISIBLE:View.GONE);
    }
*/

    private MaterialCardView redirect;
    private TextView redirecttext;
    bookmark_t bookmark = null;
}

