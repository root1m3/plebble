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
    }

    private MaterialCardView redirect;
    private TextView redirecttext;
    bookmark_t bookmark = null;

}

