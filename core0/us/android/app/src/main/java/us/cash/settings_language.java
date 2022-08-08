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
import androidx.appcompat.app.ActionBarDrawerToggle;                                           // ActionBarDrawerToggle
import android.widget.AdapterView;                                                             // AdapterView
import androidx.appcompat.app.AlertDialog;                                                     // AlertDialog
import android.widget.ArrayAdapter;                                                            // ArrayAdapter
import us.wallet.trader.bookmarks_t;                                                           // bookmarks_t
import us.wallet.trader.bookmark_t;                                                            // bookmark_t
import android.os.Build;                                                                       // Build
import android.os.Bundle;                                                                      // Bundle
import android.graphics.drawable.ColorDrawable;                                                // ColorDrawable
import android.graphics.Color;                                                                 // Color
import android.content.res.Configuration;                                                      // Configuration
import androidx.core.content.ContextCompat;                                                    // ContextCompat
import android.content.Context;                                                                // Context
import androidx.appcompat.view.ContextThemeWrapper;                                            // ContextThemeWrapper
import us.gov.socket.datagram;                                                                 // datagram
import android.content.DialogInterface;                                                        // DialogInterface
import android.text.method.DigitsKeyListener;                                                  // DigitsKeyListener
import android.graphics.drawable.Drawable;                                                     // Drawable
import us.gov.crypto.ec;                                                                       // ec
import android.widget.EditText;                                                                // EditText
//import com.google.firebase.crashlytics.FirebaseCrashlytics;                                    // FirebaseCrashlytics
import android.graphics.drawable.GradientDrawable;                                             // GradientDrawable
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.gov.id.types.*;                                                               // *
import static us.gov.io.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import static us.ko.*;                                                                         // *
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import android.view.inputmethod.InputMethodManager;                                            // InputMethodManager
import android.text.InputType;                                                                 // InputType
import android.content.Intent;                                                                 // Intent
import java.io.IOException;                                                                    // IOException
import org.json.JSONArray;                                                                     // JSONArray
import org.json.JSONException;                                                                 // JSONException
import org.json.JSONObject;                                                                    // JSONObject
import java.security.KeyPair;                                                                  // KeyPair
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import androidx.appcompat.widget.LinearLayoutCompat;                                           // LinearLayoutCompat
import java.util.Locale;                                                                       // Locale
import java.util.Map;                                                                          // Map
import com.google.android.material.button.MaterialButton;                                      // MaterialButton
import android.view.Menu;                                                                      // Menu
import android.view.MenuItem;                                                                  // MenuItem
import android.view.View.OnFocusChangeListener;                                                // OnFocusChangeListener
import us.pair;                                                                                // pair
import us.wallet.protocol;                                                                     // protocol
import android.widget.RelativeLayout;                                                          // RelativeLayout
import androidx.annotation.RequiresApi;                                                        // RequiresApi
import android.content.res.Resources;                                                          // Resources
import android.text.SpannableStringBuilder;                                                    // SpannableStringBuilder
import android.widget.Spinner;                                                                 // Spinner
import java.lang.StringBuilder;                                                                // StringBuilder
import android.annotation.SuppressLint;                                                        // SuppressLint
import com.google.android.material.textfield.TextInputEditText;                                // TextInputEditText
import com.google.android.material.textfield.TextInputLayout;                                  // TextInputLayout
import android.widget.TextView;                                                                // TextView
import java.util.Timer;                                                                        // Timer
import java.util.TimerTask;                                                                    // TimerTask
import android.widget.Toast;                                                                   // Toast
import android.util.TypedValue;                                                                // TypedValue
import us.gov.io.types.vector_tuple_hash_host_port;                                            // vector_tuple_hash_host_port
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View

public final class settings_language extends activity {

    private static void log(final String s) {                       //--strip
        System.out.println("settings_language: " + s);              //--strip
    }                                                               //--strip

    void set_handlers() {
        log("set button handlers"); //--strip

        setlang.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override public void onItemSelected(AdapterView<?> parentView, View selectedItemView, int position, long id) {
                String selected_string = setlang.getSelectedItem().toString();
                String[] x = selected_string.split("_");
                assert x.length == 2;
                String l = x[0];
                String c = x[1];
                if (lang != null && !lang.equals(l)) {
                    locale.set_lang(getApplicationContext(), l);
                    locale.set_country(getApplicationContext(), c);
                    locale.set(new Locale(l, c));
                    Toast.makeText(a, R.string.changelangnext, Toast.LENGTH_SHORT).show();
                }
            }

            @Override public void onNothingSelected(AdapterView<?> parentView) {
            }
        });

        close.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                finish();
            }
        });

    }

    @RequiresApi(api = Build.VERSION_CODES.O)
    @SuppressLint("ResourceType")
    @Override protected void onCreate(Bundle savedInstanceState) {
        log("on create"); //--strip
        super.onCreate(savedInstanceState);
        set_content_layout(R.layout.settings_language);

        toolbar.setTitle("Language");

        setlang = findViewById(R.id.lang_selector);
        close = findViewById(R.id.close);

        Resources res = getResources();
        Configuration conf = res.getConfiguration();
        lang = locale.get_lang(this);
        country = locale.get_country(this);

        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(this, R.array.langs_array, android.R.layout.simple_spinner_item);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        setlang.setAdapter(adapter);
        if (lang != null && lang.equals("es")) {
            setlang.setSelection(1);
        }
        else {
            setlang.setSelection(0);
        }
        set_handlers();
    }

    @Override public int menuid() {
        return R.menu.menu_modal;
    }

    @Override public void onBackPressed() {
        log("finish"); //--strip
        finish();
    }

    Spinner setlang;
    String lang;
    String country;
    private MaterialButton close;
}

