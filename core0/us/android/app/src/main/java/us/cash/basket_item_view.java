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
import androidx.appcompat.app.AlertDialog;                                                     // AlertDialog
import android.util.AttributeSet;                                                              // AttributeSet
import android.graphics.Color;                                                                 // Color
import android.content.Context;                                                                // Context
import java.util.Date;                                                                         // Date
import android.content.DialogInterface;                                                        // DialogInterface
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

public class basket_item_view extends LinearLayout {

    static void log(final String line) {                //--strip
       CFG.log_android("basket_item_view: " + line);    //--strip
    }                                                   //--strip

    public basket_item_view(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public void set_visible(boolean v) {
        setVisibility(v ? View.VISIBLE : View.GONE);
    }

    public interface on_click_listener {
        public void on_click(basket_item_view v);
    }

    public void init_(activity ac_, hash_t prod_, pair<types.vol_t, basket_item_t> v) {
        ac = ac_;
        prod = prod_;
        item = v.second;
        set_visible(true);
        TextView title = findViewById(R.id.product_title);
        TextView desc = findViewById(R.id.product_description);
        TextView units = findViewById(R.id.units);
        TextView price = findViewById(R.id.price);
        TextView reward = findViewById(R.id.reward);
        ImageView pic = findViewById(R.id.product_pic);
        pic.setImageResource(R.drawable.ic_productitem_24);
        title.setText("Product " + prod.encode());
        desc.setText("Description not available.");
        units.setText("" + v.first.value + " units.");
        price.setText("Pay: " + v.second.first.value);
        reward.setText("Reward: " + v.second.second.value);
    }

    public void init(activity ac_, hash_t prod_, pair<types.vol_t, basket_item_t> v, basket_view.on_click_listener click_handler_) {
        init_(ac_, prod_, v);
        mode = 0;
        MaterialCardView itmbtn = findViewById(R.id.basket_item);
        itmbtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String[] options = {"Remove product", ac.getResources().getString(R.string.cancel)};
                final basket_item_view i = basket_item_view.this;
                new AlertDialog.Builder(ac).setTitle(btitle)
                    .setItems(options, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            switch(which) {
                                case 0:
                                    click_handler.on_remove(i);
                                    break;
                            }
                        }
                    })
                    .setIcon(R.drawable.ic_productitem_24).show();
            }
        });
        click_handler = click_handler_;
    }

    public void init2(activity ac_, hash_t prod_, pair<types.vol_t, basket_item_t> v, basket_view.on_click_listener2 click_handler2_) {
        init_(ac_, prod_, v);
        mode = 1;
        MaterialCardView itmbtn = findViewById(R.id.basket_item);
        itmbtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String[] options = {"Add this product to the basket", ac.getResources().getString(R.string.cancel)};
                final basket_item_view i = basket_item_view.this;
                new AlertDialog.Builder(ac).setTitle(btitle)
                    .setItems(options, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            switch(which) {
                                case 0:
                                    click_handler2.on_select(i);
                                    break;
                            }
                        }
                    })
                    .setIcon(R.drawable.ic_productitem_24).show();
            }
        });
        click_handler2 = click_handler2_;
    }

    public void disable() {
        MaterialCardView itmbtn = findViewById(R.id.basket_item);
        itmbtn.setOnClickListener(null);
    }

    activity ac;
    basket_view.on_click_listener click_handler;
    basket_view.on_click_listener2 click_handler2;
    int mode = 0;
    public hash_t prod;
    public basket_item_t item;
    String btitle;
    String bdescription;
}

