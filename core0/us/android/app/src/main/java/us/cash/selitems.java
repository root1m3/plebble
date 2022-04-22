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
import androidx.appcompat.app.ActionBarDrawerToggle;                                           // ActionBarDrawerToggle
import android.widget.AdapterView;                                                             // AdapterView
import android.widget.ArrayAdapter;                                                            // ArrayAdapter
import java.util.ArrayList;                                                                    // ArrayList
import android.os.Build;                                                                       // Build
import android.os.Bundle;                                                                      // Bundle
import android.widget.Button;                                                                  // Button
import java.util.Collections;                                                                  // Collections
import android.content.Intent;                                                                 // Intent
import com.google.android.material.button.MaterialButton;                                      // MaterialButton
import android.view.Menu;                                                                      // Menu
import android.view.MenuItem;                                                                  // MenuItem
import android.widget.RelativeLayout;                                                          // RelativeLayout
import androidx.annotation.RequiresApi;                                                        // RequiresApi
import android.widget.TextView;                                                                // TextView
import androidx.appcompat.widget.Toolbar;                                                      // Toolbar
import android.net.Uri;                                                                        // Uri
import android.view.View;                                                                      // View

public class selitems extends activity {
    String tradeid;
    no_scroll_list_view lv;
    ArrayList<String> shit;
    private MaterialButton cancel;
    RelativeLayout progressbarcontainer;
    Toolbar toolbar;

    static void log(final String line) {         //--strip
       CFG.log_android("selitems: " + line);   //--strip
    }                                            //--strip

//TODO review
    void maproc() {
        //drawerLayout = ma.findViewById(R.id.drawer_layout);
//        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(main, main.drawerLayout, main.toolbar, R.string.bookmarks, R.string.bookmarks);
//        main.drawerLayout.addDrawerListener(toggle);
//        toggle.syncState();
        //ma.navigationView = findViewById(R.id.navigation_view);
//        ma.navigationView.setNavigationItemSelectedListener(this);
//        Toolbar toolbar = findViewById(R.id.toolbar);
//        setSupportActionBar(findViewById(R.id.toolbar));
    }


    @RequiresApi(api = Build.VERSION_CODES.O)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_selitems);


        maproc();

        toolbar = findViewById(R.id.toolbar);

//        Menu nav_menu = navigationView.getMenu();
        //MenuItem menuItem = nav_menu.findItem(R.id.nav_balance);
        //menuItem.setChecked(true);
        progressbarcontainer = findViewById(R.id.progressbarcontainer);

        cancel = findViewById(R.id.cancel);
        Bundle bundle = getIntent().getExtras();
        toolbar.setTitle(bundle.getString("title"));
        String[] sourceshit = bundle.getStringArray("sourceshit");
        shit = new ArrayList<String>();

        Collections.addAll(shit, sourceshit);
        Collections.sort(shit);

        ArrayAdapter adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, shit); //R.layout.trades_listitem

        lv = (no_scroll_list_view) findViewById(R.id.listview);
        lv.setAdapter(adapter);

        lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            public void onItemClick(AdapterView parentView, View childView, int position, long id) {
//                String[] items=shit.get(position).split("\\s");
//                go_trade(items[0],items[1],items[2]);
                Intent data = new Intent();
                data.setData(Uri.parse(shit.get(position)));
                setResult(RESULT_OK, data);
                finish();

            }
        });

        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                setResult(RESULT_CANCELED);
                finish();
            }
        });

        progressbarcontainer.setVisibility(View.GONE);

    }
}

