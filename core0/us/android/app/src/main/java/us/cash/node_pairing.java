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
import androidx.core.app.ActivityCompat;                                                       // ActivityCompat
import android.widget.AdapterView;                                                             // AdapterView
import androidx.appcompat.app.AlertDialog;                                                     // AlertDialog
import android.widget.ArrayAdapter;                                                            // ArrayAdapter
import us.wallet.trader.bookmarks_t;                                                           // bookmarks_t
import us.wallet.trader.bookmark_t;                                                            // bookmark_t
import android.os.Build;                                                                       // Build
import android.os.Bundle;                                                                      // Bundle
import android.graphics.drawable.ColorDrawable;                                                // ColorDrawable
import android.graphics.Color;                                                                 // Color
import android.widget.CompoundButton;                                                          // CompoundButton
import android.content.res.Configuration;                                                      // Configuration
import androidx.core.content.ContextCompat;                                                    // ContextCompat
import android.content.Context;                                                                // Context
import androidx.appcompat.view.ContextThemeWrapper;                                            // ContextThemeWrapper
import us.gov.socket.datagram;                                                                 // datagram
import android.content.DialogInterface;                                                        // DialogInterface
import android.text.method.DigitsKeyListener;                                                  // DigitsKeyListener
import android.graphics.drawable.Drawable;                                                     // Drawable
import us.gov.crypto.ec;                                                                       // ec
import android.text.Editable;                                                                  // Editable
import android.widget.EditText;                                                                // EditText
import android.graphics.drawable.GradientDrawable;                                             // GradientDrawable
import androidx.core.view.GravityCompat;                                                       // GravityCompat
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static android.Manifest.permission.*;                                                   // *
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
import us.wallet.engine.ip4_endpoint_t;                                                        // ip4_endpoint_t
import org.json.JSONArray;                                                                     // JSONArray
import org.json.JSONException;                                                                 // JSONException
import org.json.JSONObject;                                                                    // JSONObject
import android.text.method.KeyListener;                                                        // KeyListener
import java.security.KeyPair;                                                                  // KeyPair
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import androidx.appcompat.widget.LinearLayoutCompat;                                           // LinearLayoutCompat
import android.widget.LinearLayout;                                                            // LinearLayout
import java.util.Locale;                                                                       // Locale
import android.Manifest;                                                                       // Manifest
import java.util.Map;                                                                          // Map
import com.google.android.material.button.MaterialButton;                                      // MaterialButton
import android.view.Menu;                                                                      // Menu
import android.view.MenuItem;                                                                  // MenuItem
import android.view.View.OnFocusChangeListener;                                                // OnFocusChangeListener
import android.content.pm.PackageManager;                                                      // PackageManager
import us.pair;                                                                                // pair
import us.wallet.protocol;                                                                     // protocol
import android.widget.RelativeLayout;                                                          // RelativeLayout
import androidx.annotation.RequiresApi;                                                        // RequiresApi
import android.content.res.Resources;                                                          // Resources
import android.text.SpannableStringBuilder;                                                    // SpannableStringBuilder
import android.widget.Spinner;                                                                 // Spinner
import java.lang.StringBuilder;                                                                // StringBuilder
import android.annotation.SuppressLint;                                                        // SuppressLint
import android.widget.Switch;                                                                  // Switch
import com.google.android.material.textfield.TextInputEditText;                                // TextInputEditText
import com.google.android.material.textfield.TextInputLayout;                                  // TextInputLayout
import android.widget.TextView;                                                                // TextView
import android.text.TextWatcher;                                                               // TextWatcher
import java.util.Timer;                                                                        // Timer
import java.util.TimerTask;                                                                    // TimerTask
import android.widget.Toast;                                                                   // Toast
import android.media.ToneGenerator;                                                            // ToneGenerator
import android.util.TypedValue;                                                                // TypedValue
import us.gov.io.types.vector_tuple_hash_host_port;                                            // vector_tuple_hash_host_port
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View

public final class node_pairing extends activity {

    private static void log(final String s) {               //--strip
        System.out.println("node_pairing: " + s);           //--strip
    }                                                       //--strip

    static class state_t {
        public ip4_endpoint_t ip4_endpoint = null;
        public pin_t pin;
        public String subhome = "";
        public boolean success = false;
        public boolean called = false;
        public boolean lookingup = false;
        public boolean testing = false;
        public ko test_result = new ko("Not tested");
        public String advice = null;

        public boolean start_lookup() {
            if (lookingup) return false;
            if (testing) return false;
            lookingup = true;
            return true;
        }

        public void start_testing(ip4_endpoint_t ep, pin_t pin_, String subhome_) {
            log("start_testing " + (ep == null ? "NULL EP" : ep.to_string())); //--strip
            ip4_endpoint = ep;
            pin = pin_;
            subhome = subhome_;
            testing = true;
            called = true;
            success = false;
            test_result = new ko("Test not finished yet.");
            advice = null;
        }

        public void end_testing(ko err, String advice_) {
            assert testing == true;
            log("end_testing " + (err == ok ? "OK" : err.msg) + " "); //--strip
            testing = false;
            success = err == ok;
            test_result = err;
            advice = advice_;
            toast_result();
        }

        public void end_testing(ko report) {
            log("end_testing " + ko.as_string(report)); //--strip
            if (report != ok) {
                if (report == us.gov.socket.client.KO_83912) {
                    end_testing(report, "Node is unreachable. Please double check your inputs and consider if they are compatible with your WIFI/GSM configuration.");
                }
                else if (report == us.gov.id.peer_t.KO_6017) {
                    end_testing(report, "New devices must be paired first.");
                }
                else if (report == hmi_t.KO_10000) {
                    end_testing(report, "New devices must be paired first.");
                }
                else if (report == hmi_t.KO_61210) {
                    end_testing(report, "Oops this shouldn't happen. Handshake error. ");
                }
                else {
                    end_testing(report, "Oops this shouldn't happen.");
                }
            }
            else {
                end_testing(ok, "");
            }
        }

        public void toast_result() {
            if (!success) {
                Toast.makeText(a, test_result.msg + '\n' + advice, Toast.LENGTH_LONG).show();
            }
        }
    }

    void set_handlers() {
        log("set button handlers"); //--strip
        connection_status.setTextIsSelectable(true);
        connection_status.setKeyListener(null);
        connection_status.setOnClickListener(new View.OnClickListener() {
           @Override public void onClick(View view) {
               state.toast_result();
           }
        });

        addr.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override public void onFocusChange(View v, boolean hasFocus) {}
        });

        connect_btn.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                connect_btn.requestFocus();
                hide_keyboard();
                show_PIN_dialog(a);
            }
        });

        unpair_btn.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                do_unpair();
            }
        });

        done.setOnClickListener(new View.OnClickListener() {
           @Override public void onClick(View view) {
               finish();
           }
        });

        connection_status_lo.setEndIconOnClickListener(new View.OnClickListener() {
           @Override public void onClick(View view) {
               connection_log();
           }
        });

        //Disabled feature (unneeded for now as users can select their non custodial wallet or which custodial wallet they'd like to connect with).
        unpair_btn.setVisibility(View.GONE);

    }

    void do_unpair() {
        log("unpair"); //--strip
        if (dep.hmi == null) return;
        log("unpair!"); //--strip
        dep.hmi.unpair();

        on_user_req_poweroff();
    }

    @SuppressWarnings("StatementWithEmptyBody")
    @Override public boolean onNavigationItemSelected(MenuItem menu_item) {
        log("onNavigationItemSelected"); //--strip
        int item_id = menu_item.getItemId();
        switch (item_id) {
            case R.id.nav_hmi_poweron_withpin:
                show_PIN_dialog(this);
                break;

            case R.id.nav_hmi_poweron:
                do_test(new pin_t(0));
                break;

            case R.id.nav_hmi_poweroff:
                on_user_req_poweroff();
                break;

            case R.id.nav_softwareupdates:
//                if (dep.hmi.sw_updates != null) {
                if (dep.hmi != null) {
                    if (dep.hmi.sw_updates != null) {
                        dep.hmi.sw_updates.show_ui(node_pairing.this);
                    }
                }
                //else {
                //    toast("software updates not available here. Check Google's Play store.");
                //}
                break;

            default:
                return super.onNavigationItemSelected(menu_item);
        }
        drawer_layout.closeDrawer(GravityCompat.START);
        return true;
    }

    @Override public int menuid() {
        if (dep == null) return 0;
        return dep.hmi == null ? R.menu.menu_hmi_off : R.menu.menu_hmi_on;
    }

    @RequiresApi(api = Build.VERSION_CODES.O)
    @SuppressLint("ResourceType")
    @Override protected void onCreate(Bundle savedInstanceState) {
        log("on create"); //--strip
        super.onCreate(savedInstanceState);
        set_content_layout(R.layout.activity_node_pairing);
        addr = findViewById(R.id.walletd_address);
        addr_lo=findViewById(R.id.walletd_address_lo);
        port = findViewById(R.id.walletd_port);
        channel = findViewById(R.id.channel);
        nodepkh = findViewById(R.id.nodepkh);
        connection_status = findViewById(R.id.connection_status);
        connection_status_lo = findViewById(R.id.connection_status_lo);
        devicepk = findViewById(R.id.devicepk);
        done = findViewById(R.id.done);
        refresh = findViewById(R.id.refresh);
        connect_btn = findViewById(R.id.connect_btn);
        unpair_btn = findViewById(R.id.unpair_btn);

        mode = findViewById(R.id.mode);
        upgrade2noncustodial = findViewById(R.id.upgrade2noncustodial);
        imagenode = findViewById(R.id.imagenode);
        current_endpoint = findViewById(R.id.current_endpoint);

        name = findViewById(R.id.name);
        save_name = findViewById(R.id.save_name);

        ssid = findViewById(R.id.ssid);
        read_ssid = findViewById(R.id.read_ssid);
        save_ssid = findViewById(R.id.save_ssid);

        subhome = findViewById(R.id.subhome);
        save_subhome = findViewById(R.id.save_subhome);

        poweron = findViewById(R.id.poweron);
        layoutnode = findViewById(R.id.layoutnode);
        redled = ContextCompat.getDrawable(this, R.drawable.led).mutate();
        amberled = ContextCompat.getDrawable(this, R.drawable.led).mutate();
        greenled = ContextCompat.getDrawable(this, R.drawable.led).mutate();
        blueled = ContextCompat.getDrawable(this, R.drawable.led).mutate();
        ((GradientDrawable) redled).setColor(Color.RED);
        ((GradientDrawable) amberled).setColor(leds_t.amber);
        ((GradientDrawable) greenled).setColor(Color.GREEN);
        ((GradientDrawable) blueled).setColor(Color.CYAN);
        refresh.setVisibility(View.GONE);
        setTitle(R.string.settings);
        addr.setSingleLine();
        addr.setInputType(InputType.TYPE_CLASS_NUMBER);
        addr.setKeyListener(DigitsKeyListener.getInstance("0123456789."));
        addr.setSelectAllOnFocus(true);
        addr.setTextSize(TypedValue.COMPLEX_UNIT_SP, 16);
        addr_lo.setEndIconOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View v) {
                String[] options = {"Find IP Address", "Show network seeds.", "Connection log.", a.getResources().getString(R.string.cancel)};
                final node_pairing i = node_pairing.this;
                new AlertDialog.Builder(i).setTitle("Asking nodes in the network...")
                    .setItems(options, new DialogInterface.OnClickListener() {
                        @Override public void onClick(DialogInterface dialog, int which) {
                            switch(which) {
                                case 0:
                                    i.find_ip_address();
                                    break;
                                case 1:
                                    i.show_seeds();
                                    break;
                                case 2:
                                    i.connection_log();
                                    break;
                                case 3:
                            }
                        }
                    }).setIcon(R.drawable.ic_world).show();
            }
        });
        port.setSingleLine();
        port.setInputType(InputType.TYPE_CLASS_NUMBER);
        port.setSelectAllOnFocus(true);
        channel.setSingleLine();
        channel.setInputType(InputType.TYPE_CLASS_NUMBER);
        channel.setSelectAllOnFocus(true);
        if (!getIntent().hasExtra("conf_index")) {
            log("KO 77869 Missing conf_index"); //--strip
            finish();
        }
        conf_index = getIntent().getExtras().getInt("conf_index", 0);
        log("conf_index " + conf_index); //--strip
        dep = a.device_endpoints.get(conf_index);
        dep.ui = this;
        //dep.log_blob(); //--strip
        final ip4_endpoint_t ep = dep.ip4_endpoint;
        log("ip4_endpoint " + (ep == null ? "Null" : ep.to_string())); //--strip
        if (ep != null) {
            log("Set UI Texts"); //--strip
            addr.setText(ep.shost.value);
            port.setText("" + ep.port.value);
            channel.setText("" + ep.channel.value);
        }
        done.setVisibility(View.GONE);
        devicepk.setInputType(InputType.TYPE_NULL);
        devicepk.setTextIsSelectable(true);
        devicepk.setKeyListener(null);
        nodepkh.setInputType(InputType.TYPE_NULL);
        nodepkh.setTextIsSelectable(true);
        nodepkh.setKeyListener(null);
        upgrade2noncustodial.setVisibility(View.GONE);
        set_handlers();
        toolbar_button refresh_btn = findViewById(R.id.refresh);
        refresh_btn.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                refresh();
            }
        });
        refresh_btn.setVisibility(View.VISIBLE);

        name.addTextChangedListener(new TextWatcher() {
            @Override public void afterTextChanged(Editable s) { save_name.setVisibility(View.VISIBLE); }
            @Override public void beforeTextChanged(CharSequence s, int start, int count, int after) {}
            @Override public void onTextChanged(CharSequence s, int start, int before, int count) {}
        });

        save_name.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                dep.name_.value = name.getText().toString();
                a.device_endpoints.save();
                save_name.setVisibility(View.GONE);
            }
        });

        ssid.addTextChangedListener(new TextWatcher() {
            @Override public void afterTextChanged(Editable s) { save_ssid.setVisibility(View.VISIBLE); }
            @Override public void beforeTextChanged(CharSequence s, int start, int count, int after) {}
            @Override public void onTextChanged(CharSequence s, int start, int before, int count) {}
        });

        save_ssid.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                dep.ssid.value = ssid.getText().toString();
                a.device_endpoints.save();
                save_ssid.setVisibility(View.GONE);
            }
        });

        read_ssid.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                set_ssid();
            }
        });

/*
        subhome.addTextChangedListener(new TextWatcher() {
            @Override public void afterTextChanged(Editable s) { save_subhome.setVisibility(View.VISIBLE); }
            @Override public void beforeTextChanged(CharSequence s, int start, int count, int after) {}
            @Override public void onTextChanged(CharSequence s, int start, int before, int count) {}
        });
*/

        save_subhome.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                String x = subhome.getText().toString();
                if (x.equals("[Non-Custodial]")) {
                    x = "";
                }
                dep.subhome.value = x;
                a.device_endpoints.save();
                save_subhome.setVisibility(View.GONE);
            }
        });

        subhome_key_listener = subhome.getKeyListener();

    }

    @Override public void onPause() {
       log("onPause"); //--strip
       super.onPause();
       update_hmi(false);
    }

    @Override public void onResume() {
       log("onResume"); //--strip
       super.onResume();
       update_hmi(true);
    }

    @Override public void onDestroy() {
        super.onDestroy();
        log("onDestroy"); //--strip
        update_hmi(false);
        dep.ui = null;
    }

    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
    }

    private static String[] PERMISSIONS_WIFI_SSID = {Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_COARSE_LOCATION};
    private static final int REQUEST_PERMISSION_WIFI_SSID = 1832;


    public void ask_permission_continue() {
        log("request Permission"); //--strip
        ActivityCompat.requestPermissions(this, PERMISSIONS_WIFI_SSID, REQUEST_PERMISSION_WIFI_SSID);
        int permission = ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION);
        log("checkSelfPermission returned " + permission + " (granted=" + PackageManager.PERMISSION_GRANTED + ")"); //--strip
        String msg = (permission == PackageManager.PERMISSION_GRANTED) ? "Thanks for granting access to current wifi SSID." : "Permissions for reading the SSID has been denied.";
        Toast.makeText(node_pairing.this, msg, 6000).show();
        if (permission != PackageManager.PERMISSION_GRANTED) {
            a.tone.startTone(ToneGenerator.TONE_CDMA_NETWORK_BUSY, 200);
        }
    }

    public void ask_permission0() {
        log("ask_permission"); //--strip
        String[] options = {"Proceed to FINE_LOCATION permissions.", a.getResources().getString(R.string.cancel)};
        new AlertDialog.Builder(this).setTitle(CFG.app_name + " collects location data to enable dynamic parameter selection even when the app is closed or not in use. Grant access to WIFI network name (SSID) in order to ebanle this feature.")
            .setItems(options, new DialogInterface.OnClickListener() {
                @Override public void onClick(DialogInterface dialog, int which) {
                    if (which == 0) {
                        ask_permission_continue();
                    }
                 }
            })
            .setIcon(android.R.drawable.ic_dialog_info).show();
    }

    private void ask_permission() {
        log("ask_permission"); //--strip
        AlertDialog dialog = new AlertDialog.Builder(new ContextThemeWrapper(this, R.style.myDialog))
            .setTitle("FINE_LOCATION permissions.")
            .setMessage("This app collects location data to enable dynamic parameter selection even when the app is closed or not in use. The app maintains a user-defined list of connections and uses the network id (SSID) to select the most appropriate one based on this value. This data is never transmitted nor stored in public storage. Please grant access to your WIFI network name (SSID) in the following dialogue in order to enable this feature...")
            .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                    @Override public void onClick(DialogInterface dialog, int which) {
                        log("user clicked ok"); //--strip
                        ask_permission_continue();
                    }
                })
            .create();
        dialog.show();
    }
    
    public ko verify_ssid_permissions() {        // Check if we have write permission
        a.assert_ui_thread(); //--strip
        log("verify_ssid_permissions"); //--strip
        int permission = ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION);
        if (permission == PackageManager.PERMISSION_GRANTED) {
            log("Permission already granted"); //--strip
            return ok;
        }
        return new ko("KO 43097 Permission denied");
    }

    String ssid_verified = null;

    void set_ssid() {
        if (ssid_verified != null) {
            log("in cache ssid_verified" + ssid_verified); //--strip
        }
        else {
            a.assert_ui_thread(); //--strip
            if (is_ko(verify_ssid_permissions())) {
                log("asking for permissions"); //--strip
                ask_permission();
            }
            else {
                ssid_verified = a.net_identifier();
                log("ssid_verified " + ssid_verified); //--strip
                ssid.setText(ssid_verified);
                toast("ssid read " + ssid_verified);
            }
        }
    }

/*
    void confirmed_subhome(String sbhome) {
        a.assert_ui_thread(); //--strip
        state.subhome = sbhome;
        subhome.setText(sbhome);
        if (sbhome.isEmpty()) {
            toast("Non-Custodial wallet");
        }
        else {
            toast("Custodial wallet " + sbhome);
        }
    }
*/

    public void confirmed_subhome__worker(String subhome) {
        log("confirmed_subhome__worker " + subhome); //--strip
        dep.set_subhome(subhome);
        a.device_endpoints.save();
        runOnUiThread(new Runnable() {
            @Override public void run() {
                refresh_subhome_widgets();
                refresh_mode_widgets();
            }
        });
    }

    void update_hmi(boolean set_manual) {
        if (dep.hmi == null) {
            return;
        }
        if (set_manual) {
            if (dep.hmi.manual_mode_ui == null) {
                dep.hmi.set_manual_mode(this);
                dep.hmi.add_status_handler(new hmi_t.status_handler_t() {
                    public void on_status(ColorDrawable ledstate, final String msg) {
                        runOnUiThread(new Runnable() {
                            @Override public void run() {
                                log("HMI status: " + msg); //--strip
                                set_status_led(ledstate, msg);
                            }
                        });
                    }
                });
            }
        }
        else {
           dep.hmi.set_manual_mode(null);
           dep.hmi.add_status_handler(null);
        }
    }

    void show_connection_log() {
        if (dep.hmi == null) return;
        new AlertDialog.Builder(node_pairing.this).setTitle("Connection log:").setMessage(dep.hmi.get_msg_log()).show();
    }

    void techinfo() {
        log("techinfo"); //--strip
        StringBuilder b = new StringBuilder();
        b.append("HMI: " + (dep.hmi == null ? "KO Not" : "OK ") + " present.\n");
        if (dep.hmi != null) {
            b.append("  ip4_endpoint:\n    " + (dep.hmi.ip4_endpoint == null ? "Null" : "" + dep.hmi.ip4_endpoint.to_string()) + '\n');
            b.append("  active: " + dep.hmi.is_active() + '\n');
            b.append("  online: " + dep.hmi.is_online + '\n');
            b.append("  wallet_address:\n    " + (dep.hmi.wallet_address == null ? "KO null" : dep.hmi.wallet_address.encode()) + '\n');
            b.append("  wloc: " + (dep.subhome.value.isEmpty() ? "[Non Custodial (empty)]" : dep.subhome.value) + '\n');
            b.append("  trader_endpoints:\n");
            pair<ko, bookmarks_t> r = dep.hmi.bookmarks_me();
            if (is_ko(r.first)) {
                b.append(r.first.msg);
            }
            else {
                for (Map.Entry<String, bookmark_t> entry : r.second.entrySet()) {
                    b.append("    " + entry.getKey() + " - " + entry.getValue().to_string() + '\n');
                }
            }
        }
        new AlertDialog.Builder(node_pairing.this).setTitle("HMI Info:").setMessage(b.toString()).show();
    }

    void connection_log() {
        if (dep.hmi == null) return;
        String[] options = {"Connection log. Show.", "Connection log. Clear.", "HMI info.", a.getResources().getString(R.string.cancel)};
        new AlertDialog.Builder(node_pairing.this).setTitle("Connection log:")
            .setItems(options, new DialogInterface.OnClickListener() {
                @Override public void onClick(DialogInterface dialog, int which) {
                    switch(which) {
                        case 0:
                            show_connection_log();
                            break;
                        case 1:
                            if (dep.hmi != null) {
                                dep.hmi.clear_msg_log();
                                Toast.makeText(node_pairing.this, "Connection log cleared.", 6000).show();
                            }
                            else {
                                Toast.makeText(node_pairing.this, "HMI is off.", 6000).show();
                            }
                            break;
                        case 2:
                            techinfo();
                            break;
                        case 3:
                            break;
                    }
                }
            }).show();
    }

    void show_seeds() {
        if (dep.hmi == null) return;
        StringBuilder b = new StringBuilder(dep.hmi.seeds.size() * 50);
        for (tuple3<hash_t, host_t, port_t> i: dep.hmi.seeds) {
            b.append(i.item0.encode());
            b.append(' ');
            b.append(us.gov.socket.client.endpoint(i.item1, i.item2));
            b.append('\n');
        }
        new AlertDialog.Builder(node_pairing.this).setTitle("Nodes:").setMessage(b.toString()).show();
    }

    void find_ip_address() {
        log("looking up IP address..."); //--strip
        Thread t = new Thread(new Runnable() {
            @Override public void run () {
                ColorDrawable curled = dep.hmi.cur_led;
                String curmsg = dep.hmi.cur_msg;
                pair<ko, ip4_endpoint_t> r = dep.hmi.lookup_ip(new app.progress_t() {
                    @Override public void on_progress(final String msg) {
                        log("renew IP: " + msg); //--strip;
                        dep.hmi.set_status(leds_t.led_blue, msg);
                    }
                });
                dep.hmi.set_status(curled, curmsg);
            }
        });
        t.start();
    }

    void hide_keyboard() {
        try{
            View v = getCurrentFocus();
            if (v != null) {
                InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
                imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
            }
        }
        catch (Exception e) {
        }
    }

    private void show_PIN_dialog(Context c) {
        log("show_PIN_dialog"); //--strip
        final EditText input = new EditText(c);
        input.setOnFocusChangeListener(new OnFocusChangeListener() {
            @Override public void onFocusChange(View v, boolean hasFocus) {
                input.post(new Runnable() {
                    @Override public void run() {
                        InputMethodManager inputMethodManager= (InputMethodManager) node_pairing.this.getSystemService(Context.INPUT_METHOD_SERVICE);
                        inputMethodManager.showSoftInput(input, InputMethodManager.SHOW_IMPLICIT);
                    }
                });
            }
        });
        input.requestFocus();
        input.setSingleLine();
        input.setInputType(InputType.TYPE_CLASS_NUMBER);
        AlertDialog dialog = new AlertDialog.Builder(new ContextThemeWrapper(this, R.style.myDialog))
            .setTitle("Connecting to remote wallet.")
            .setMessage("Authentication for new devices. If you have a PIN number type it, or leave the field empty for sending default pin 00000. PINS are one-shot. Already authenticated devices don't need to enter any PIN.")
            .setView(input)
            .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                @Override public void onClick(DialogInterface dialog, int which) {
                    String v = String.valueOf(input.getText());
                    pin_t pin;
                    try {
                        pin = new pin_t(Integer.parseInt(v));
                    }
                    catch(Exception e) {
                        log("KO 30291 Exception " + e.getMessage()); //--strip
                        pin = new pin_t(0);
                    }
                    log("collected PIN " + pin.value); //--strip

                    do_test(pin);
                }
            }).create();
        dialog.show();
    }

    ip4_endpoint_t ip4_endpoint_from_widgets() {
        final String address = addr.getText().toString();
        port_t tcpport;
        channel_t chan;
        try {
            tcpport = new port_t(Integer.parseInt(port.getText().toString()));
            chan = new channel_t(Integer.parseInt(channel.getText().toString()));
        }
        catch(NumberFormatException e) {
            return null;
        }
        return new ip4_endpoint_t(new shost_t(address), tcpport, chan);
    }

    void set_status_led(ColorDrawable led_state, String msg) { //0 red 1 amber 2 green -1 unchanged
        log("set_status_led: " + msg); //--strip
        if (led_state == leds_t.led_red) {
            connection_status_lo.setEndIconDrawable(redled);
        }
        else if (led_state == leds_t.led_amber) {
            connection_status_lo.setEndIconDrawable(amberled);
        }
        else if (led_state == leds_t.led_green) {
            connection_status_lo.setEndIconDrawable(greenled);
        }
        else if (led_state == leds_t.led_blue) {
            connection_status_lo.setEndIconDrawable(blueled);
        }
        if (msg != null) {
            connection_status.setText(msg);
        }
    }

    void on_user_req_poweron() {
        a.assert_ui_thread(); //--strip
        log("on_user_req_poweron"); //--strip
        ip4_endpoint_t ip4_endpoint = ip4_endpoint_from_widgets();
        app.progress_t progress = new app.progress_t() {
            @Override public void on_progress(final String report) {
                runOnUiThread(new Runnable() {
                    @Override public void run() {
                        Toast.makeText(node_pairing.this, report, 6000).show();
                    }
                });
            }
        };
        ko ans = dep.set_ip4_endpoint(ip4_endpoint);
        a.HMI_power_on(conf_index, new pin_t(0), progress);
        Timer timer = new Timer();
        timer.schedule(new TimerTask() {
          @Override public void run() {
            runOnUiThread(new Runnable() {
                @Override public void run() {
                    log("Saving settings"); //--strip
                    a.device_endpoints.save();
                    refresh();
                }
            });
          }
        }, 1000);
    }

    void on_user_req_poweroff() {
        a.assert_ui_thread(); //--strip
        log("on_user_req_poweroff"); //--strip
        app.progress_t progress = new app.progress_t() {
            @Override public void on_progress(final String report) {
                runOnUiThread(new Runnable() {
                    @Override public void run() {
                        Toast.makeText(node_pairing.this, report, 6000).show();
                    }
                });
            }
        };
        a.HMI_power_off(conf_index, true, progress);
        Timer timer = new Timer();
        timer.schedule(new TimerTask() {
          @Override public void run() {
            runOnUiThread(new Runnable() {
                @Override public void run() { 
                    log("Saving settings"); //--strip
                    a.device_endpoints.save();
                    refresh(); 
                }
            });
          }
        }, 1000);
    }

    void test_task() {
        log("test_task " + state.ip4_endpoint.to_string()); //--strip
        a.assert_worker_thread(); //--strip
        if (state.ip4_endpoint == null) {
            log("wrong ip4_endpoint from widgets"); //--strip
            test_result_worker(new ko("KO 60954 Wrong input"));
            return;
        }
        log("ip4_endpoint from state " + state.ip4_endpoint.to_string() + " PIN > 0 " + (state.pin.value > 0 ? "yes" : "no"));  //--strip
        try {
            app.progress_t progress = new app.progress_t() {
                @Override public void on_progress(final String report) {
                    runOnUiThread(new Runnable() {
                        @Override public void run() {
                            Toast.makeText(node_pairing.this, report, 6000).show();
                        }
                    });
                }
            };
            log("Connecting to " + state.ip4_endpoint.to_string() + " using PIN > 0 " + (state.pin.value > 0 ? "yes" : "no") + " subhome " + state.subhome); //--strip
            if (dep.hmi != null) {
                a.HMI_power_off__worker(conf_index, false, progress);
            }
            log("Connecting to " + state.ip4_endpoint.to_string() + " using PIN > 0 " + (state.pin.value > 0 ? "yes" : "no") + " subhome " + state.subhome); //--strip
            dep.set_ip4_endpoint(state.ip4_endpoint);
            dep.set_subhome(state.subhome);
            a.device_endpoints.save();
            a.HMI_power_on__worker(conf_index, state.pin, progress);

            update_hmi(true);

            Timer timer = new Timer();
            timer.schedule(new TimerTask() {
              @Override public void run() {
                runOnUiThread(new Runnable() {
                    @Override public void run() { refresh(); }
                });
              }
            }, 3000);

        }
        catch (Exception e) {
            log("Exception " + e.getMessage()); //--strip
            //if (ans == ok) ans = new ko("KO 60394 " + e.toString());
            //error_manager.manage(e, ans.msg);
            log(e.toString()); //--strip
        }
        log("WP 58749 "); //--strip
    }

    void do_test(final pin_t pin) {
        log ("do_test " + pin.value); //--strip
        ip4_endpoint_t ep = ip4_endpoint_from_widgets();
        log ("ip4_endpoint_t from widgets " + ep.to_string()); //--strip
        String sbhome = subhome.getText().toString();
        if (sbhome.equals("[Non-Custodial]")) {
            sbhome = "";
        }
        log("do_test" + pin.value + " subhome " + sbhome); //--strip
        state.start_testing(ep, pin, sbhome);
        refresh();
        set_status_led(leds_t.led_amber, "Trying " + (state.ip4_endpoint != null ? state.ip4_endpoint.to_string() : ""));
        Thread thread = new Thread(new Runnable() {
            @Override public void run () {
                test_task();
            }
        });
        thread.start();
    }

    String r_(int id) {
        return getResources().getString(id);
    }

    boolean isok(final String report) {
        log("isok"); //--strip
        if (report.equals("?")) {
            log("DEPRECATED: still using '?'. not ok. "); //--strip
            return false;
        }
        if (report.startsWith("KO")) {
            log(report); //--strip
            return false;
        }
        else {
            log("is ok!"); //--strip
            return true;
        }
    }

    void toast_msg(final String msg, int bg, int fg) {
        Toast toast = Toast.makeText(a.getApplicationContext(), msg, Toast.LENGTH_LONG);
        View view = toast.getView();
        view.setBackgroundColor(bg);
        TextView text = view.findViewById(android.R.id.message);
        text.setTextColor(fg);
        toast.show();
    }

    void test_result_ui(final ko report) {
        log("test_result_ui"); //--strip
        a.assert_ui_thread(); //--strip
        state.end_testing(report);
        if (report == ok) {
            log("========saving ip4_ep " + state.ip4_endpoint.to_string()); //--strip
            dep.ip4_endpoint = state.ip4_endpoint;
            a.device_endpoints.save();
        }
        refresh();
    }

    void test_result_worker(final ko report) {
        a.assert_worker_thread();  //--strip
        runOnUiThread(new Runnable() {
            public void run() {
                test_result_ui(report);
            }
        });
    }

    String get_pubkey() {
        log("get_pubkey"); //--strip
        if (dep == null) return "";
        if (dep.cfg == null) return "";
        if (dep.cfg.keys == null) return "";
        return ec.instance.to_b58(dep.cfg.keys.getPublic());
    }

    void refresh_mode_widgets() {
        log("custodial_info"); //--strip
        if (dep.hmi == null) {
            mode.setVisibility(View.GONE);
            //upgrade2noncustodial.setVisibility(View.GONE);
            return;            
        }
        if (!dep.hmi.is_online) {
            log("hmi is down"); //--strip
            mode.setVisibility(View.GONE);
            dep.hmi.report_status__ui();
            return;
        }
        log("hmi is up"); //--strip
        String subhome = dep.subhome.value;
        if (subhome.isEmpty()) {
            mode.setText(R.string.mode_non_custodial);
            mode.setTextColor(darkgreen);
            //upgrade2noncustodial.setVisibility(View.GONE);
        }
        else {
            String mcs = r_(R.string.mode_custodial);
            log("mcs=" + mcs); //--strip
            mode.setText(mcs + "\n(subhome " + subhome + ")");
            mode.setTextColor(orange);
            //upgrade2noncustodial.setVisibility(View.VISIBLE);
        }
        mode.setVisibility(View.VISIBLE);
        dep.hmi.report_status__ui();
    }

    void refresh_node_address_n_lookupip() {
        boolean lookupip_enable = true;
        hash_t wallet_address = null;
        if (dep.hmi == null) {
            layoutnode.setVisibility(View.GONE);
            return;
        }
        layoutnode.setVisibility(View.VISIBLE);

        if (dep.hmi != null) {
            wallet_address = dep.hmi.wallet_address;
        }
        if (wallet_address == null || wallet_address.is_zero()) {
            nodepkh.setText("");
            lookupip_enable = false;
        }
        else {
            log("wallet_address " + wallet_address.encode()); //--strip
            String addr = dep.hmi.wallet_address.encode();
            if (!dep.subhome.value.isEmpty()) {
                addr += "." + dep.subhome.value;
            }
            nodepkh.setText(addr);
        }
        if (state.lookingup || state.testing) {
            lookupip_enable = false;
        }
    }

    void refresh_device_widgets() {
        log("devicepk"); //--strip
        devicepk.setText(get_pubkey());
    }

    void refresh_connect_button() {
        log("refresh_connect_button " + state.lookingup + " " + state.testing); //--strip
        if (dep.hmi != null) {
            ip4_endpoint_t ep = dep.ip4_endpoint;
            log("ip4_endpoint " + (ep == null ? "Null" : ep.to_string())); //--strip
            if (ep != null) {
                current_endpoint.setText("Current: " + ep.to_string());
            }

            connect_btn.setEnabled(true);
            connect_btn.setVisibility(View.VISIBLE);
            current_endpoint.setVisibility(View.VISIBLE);
            //unpair_btn.setEnabled(true);
            //unpair_btn.setVisibility(View.VISIBLE);
        }
        else {
            connect_btn.setVisibility(View.GONE);
            current_endpoint.setVisibility(View.GONE);
            //unpair_btn.setVisibility(View.GONE);
        }    
    }

    void refresh_ssid_widgets() {
        read_ssid.setVisibility(View.VISIBLE);
        save_ssid.setVisibility(View.GONE);

        String curnet = ssid_verified;
        log("curnet " + curnet); //--strip
        if (ssid.getText().equals(curnet)) {
            read_ssid.setVisibility(View.GONE);
        }
        else {
            read_ssid.setVisibility(View.VISIBLE);
        }

        String uissid = ssid.getText().toString();
        log("ssid.getText()='" + uissid + "'"); //--strip
        if (uissid.isEmpty()) {
            log("ssid.setText()->'" + dep.ssid.value + "'"); //--strip
            ssid.setText(dep.ssid.value);
            save_ssid.setVisibility(View.GONE);
        }
    }

    KeyListener subhome_key_listener = null;

    TextWatcher subhome_TextWatcher = new TextWatcher() {
            @Override public void afterTextChanged(Editable s) { save_subhome.setVisibility(View.VISIBLE); }
            @Override public void beforeTextChanged(CharSequence s, int start, int count, int after) {}
            @Override public void onTextChanged(CharSequence s, int start, int before, int count) {}
    };

    void refresh_subhome_widgets() {
        String subhometxt = subhome.getText().toString();
        log("subhome.getText() = '" + subhometxt + "'"); //--strip

        subhome.removeTextChangedListener(subhome_TextWatcher);
        if (dep.subhome.value.isEmpty()) {
            subhome.setText("[Non-Custodial]");
            subhome.setTextColor(darkgreen);
        }
        else {
            subhome.setText(dep.subhome.value);
            subhome.setTextColor(orange);
        }
        save_subhome.setVisibility(View.GONE);
        subhome.setTextIsSelectable(true);
        if (dep.hmi == null) { //only allow taking input in power-off mode.
            if (subhome_key_listener != null) { //restore original keylistener and allow input
                subhome.setKeyListener(subhome_key_listener);
                subhome.addTextChangedListener(subhome_TextWatcher);
            }
            else {
                toast("KO 78699 look at this");
            }
        }
        else {
            subhome.setKeyListener(null);
        }
    }

    boolean disable_listener = false;

    void refresh_poweron() {
        log("refresh_poweron dep.hmi=" + (dep.hmi != null ? "ON" : "OFF")); //--strip
        poweron.setOnCheckedChangeListener(null);

        disable_listener = true;
        if (dep.hmi == null) {
            poweron.setChecked(false);
        }        
        else {
            poweron.setChecked(true);
        }
        disable_listener = false;

        poweron.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (disable_listener) {
                    log("power switch not actioned by human"); //--strip
                    return;
                }
                log("human touched power switch"); //--strip
                if (isChecked) {
                    on_user_req_poweron();
                }
                else {
                    on_user_req_poweroff();
                }
            }
        });
    }

    public void refresh__worker() {
        app.assert_worker_thread(); //--strip
        runOnUiThread(new Thread(new Runnable() {
            public void run() {
                refresh();
            }
        }));

    }

    public void refresh() {
        log("refresh"); //--strip
        app.assert_ui_thread();  //--strip
        update_hmi(true);

        name.setText(dep.name_.value);
        save_name.setVisibility(View.GONE);

        refresh_ssid_widgets();
        refresh_subhome_widgets();

        refresh_device_widgets();
        refresh_poweron();
        refresh_node_address_n_lookupip();
        refresh_connect_button();
        refresh_mode_widgets();

        super.refresh();

        log("end refresh"); //--strip
    }

    void lookup() {
        app.assert_worker_thread(); //--strip
        if (dep.hmi == null) return;
        log("lookup ip"); //--strip
        pair<ko, ip4_endpoint_t> r = dep.hmi.lookup_ip(new app.progress_t() {
            @Override public void on_progress(final String txt) {
                runOnUiThread(new Thread(new Runnable() {
                    public void run() {
                        Toast.makeText(getApplicationContext(), txt, Toast.LENGTH_LONG).show();
                    }
                }));
            }
        });
    }

    private TextInputEditText name;
    MaterialButton save_name;
    
    private TextInputEditText ssid;
    MaterialButton save_ssid;
    MaterialButton read_ssid;

    private TextInputEditText subhome;
    MaterialButton save_subhome;

    private Switch poweron;

    static int darkgreen = Color.parseColor("#009900");
    static int orange = Color.parseColor("#ffa500");

    private TextView current_endpoint;
    private TextInputEditText addr;
    private TextInputEditText port;
    private TextInputEditText channel;
    private toolbar_button done;
    private toolbar_button refresh;
    private MaterialButton connect_btn;
    private MaterialButton unpair_btn;
    private TextInputEditText devicepk;
    private TextInputEditText connection_status;
    private TextInputEditText nodepkh;
    LinearLayout layoutnode;
    TextInputLayout connection_status_lo;
    TextInputLayout addr_lo;
    private TextView mode;
    MaterialButton upgrade2noncustodial;
    private TextInputLayout imagenode;
    Drawable redled;
    Drawable amberled;
    Drawable greenled;
    Drawable blueled;
    state_t state = new state_t();

    device_endpoint_t dep = null;
    int conf_index = 0;
}

