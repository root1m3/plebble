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
import java.util.ArrayList;                                                                    // ArrayList
import java.util.Date;                                                                         // Date
import us.gov.crypto.ec;                                                                       // ec
import static us.gov.id.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import static us.ko.is_ko;                                                                     // is_ko
import java.security.KeyPair;                                                                  // KeyPair
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok

public final class device_endpoint_t implements hmi_t.hmi_callback_t {

    private static void log(final String line) {                        //--strip
        CFG.log_android("device_endpoint_t: " + line);                  //--strip
    }                                                                   //--strip

    public device_endpoint_t(app a, device_endpoints_t parent_, String home0) throws Exception {
        parent = parent_;
        name_ = "new_connection";
        ssid = "";
        addr = "";
        endpoint = new endpoint_t();
        hmi = null;
        ts = 0;
        KeyPair keys = ec.instance.generate_keypair();
        if (keys == null) {
            ko r = new ko("KO 86964");
            log(r.msg); //--strip
            throw new Exception(r.msg);
        }
        set_home(home0, keys);
        cfg = new cfg_android_private_t(a.getApplicationContext(), keys.getPrivate(), home);
    }

    public device_endpoint_t(app a, device_endpoints_t parent_, long ts_, String addr_, String nm, String ssid_, String k_b58, String home0, endpoint_t ep)  throws Exception {
        parent = parent_;
        name_ = nm;
        ssid = ssid_;
        addr = addr_;
        endpoint = ep;
        hmi = null;
        ts = ts_;
        KeyPair keys;
        if (k_b58 != null && !k_b58.isEmpty()) {
            keys = ec.instance.generate_keypair(k_b58);
        }
        else {
            keys = ec.instance.generate_keypair();
        }
        if (keys == null) {
            ko r = new ko("KO 86965");
            log(r.msg); //--strip
            throw new Exception(r.msg);
        }
        set_home(home0, keys);
        cfg = new cfg_android_private_t(a.getApplicationContext(), keys.getPrivate(), home);
    }

    public device_endpoint_t(app a, String home0, device_endpoint_t other) throws Exception {
        parent = other.parent;
        name_ = "copy_" + other.name_;
        ssid = "";
        addr = "";
        endpoint = new endpoint_t(other.endpoint);
        hmi = null;
        ts = 0;
        KeyPair keys = ec.instance.generate_keypair();
        if (keys == null) {
            ko r = new ko("KO 86966");
            log(r.msg); //--strip
            throw new Exception(r.msg);
        }
        set_home(home0, keys);
        cfg = new cfg_android_private_t(a.getApplicationContext(), keys.getPrivate(), home);
    }

    void set_home(final String home0, KeyPair keys) {
        home = us.gov.crypto.ec.instance.to_address(keys.getPublic()).encode();
        if (!home0.isEmpty()) {
            home = home0 + "/" + home;
        }
        log("home " + home); //--strip
    }

    public void poweron(app a, pin_t pin, app.progress_t progress) {
        log("poweron"); //--strip
        if (progress == null) {
            log("adding default progress"); //--strip
            progress = new app.progress_t() {
                @Override public void on_progress(final String report) {
                    log("progress: " + report); //--strip
                }
            };
        }
        if (hmi != null) {
            if (hmi.is_active()) {
                progress.on_progress("Already powered ON");
                return;
            }
            log("found HMI instantiated but not active"); //--strip
            hmi = null;
        }
        log("Instantiating new HMI ..."); //--strip
        us.wallet.cli.params p = new us.wallet.cli.params();
        p.daemon = false;
        p.homedir = home + "/hmi";
        log("hmi homedir=" + p.homedir); //--strip
        p.rpc__connect_for_recv = true;
        p.rpc__stop_on_disconnection = false;
        log("creating datagram dispatcher"); //--strip
        hmi = new hmi_t(a, this, p, new app.walletd_status_handler_t(a), new datagram_dispatcher_t(3));
        hmi.cfg = cfg;
        hmi.busyled_handler_recv = new app.walletd_led_handler(a, 0);
        hmi.busyled_handler_send = new app.walletd_led_handler(a, 1);
        log("start HMI"); //--strip

/*
        endpoint_t ep = endpoint_from_settings();
        if (ep == null) {
            log("ep is null"); //--strip
            save(new endpoint_t());
            ep = endpoint_from_settings();
            assert ep != null;
        }
*/
        hmi.try_renew_ip_on_connect_failed = true;
        if (endpoint == null) {
            hmi = null;
            ko r = new ko("KO 54093 invalid ep in settings. Restart of HMI's been aborted.");
            log(r.msg); //--strip
            progress.on_progress(r.msg);
//            call_on_power();
            return;
        }

//    public ko restart_hmi(final endpoint_t ep, pin_t pin, progress_t report) {
        log("starting HMI with ep " + endpoint.to_string() + "; pin " + pin.value); //--strip
        progress.on_progress("Starting connection to " + (endpoint == null ? "null" : endpoint.to_string()));
        ko r = hmi.start(endpoint, pin);
        if (is_ko(r)) {
            log("HMI Failed to start: " + r.msg); //--strip
            progress.on_progress("Failed to connect to " + (endpoint == null ? "null" : endpoint.to_string()));
//            call_on_power();
            return;
        }
        log("hmi started"); //--strip
        progress.on_progress("HMI restarted " + (endpoint == null ? "null" : endpoint.to_string()));
//        call_on_power();
    }

    public void poweroff(app.progress_t progress) {
        log("poweroff"); //--strip
        if (hmi == null) {
            log("hmi was null"); //--strip
            progress.on_progress("HMI was already powered OFF");
//            call_on_power();
            return;
        }
        hmi.set_manual_mode(true);
        hmi.stop();
        log("hmi stopping"); //--strip
        hmi.join();
        log("hmi stopped"); //--strip
        hmi = null;
        log("hmi is now null"); //--strip
        progress.on_progress("HMI powered OFF");
//        call_on_power();
    }

    public @Override void on_hmi__worker(final ko status) {
        app.assert_worker_thread(); //--strip
        log("on_hmi__worker"); //--strip
        if (status == ok) {
            log("HMI is ok. Node is up"); //--strip
            log("saving endpoint " + endpoint.to_string()); //--strip
            if (hmi.wallet_address != null) {
                addr = hmi.wallet_address.encode();
            }
            ts = new Date().getTime();
            parent.save();
        }

/*
        else {
            log("HMI couldn't start. " + status.msg); //--strip
            runOnUiThread(new Runnable() {
                @Override public void run() {
                    sw_updates.stop();
                }
            });
            //if (!hmi.manual_mode) {
                //launch_settings__worker();
                
            //}
        }
*/
    }

    public ko set_endpoint(endpoint_t endpoint_) {
        if (hmi != null) return new ko("KO 92001 Cannot change endpoint while HMI is running.");
        endpoint = endpoint_;
        return ok;
    }

    public String get_title() {
        String s = name_;
        log("get title for " + s); //--strip
        if (ssid != null && !ssid.isEmpty()) {
            log("adding ssid " + ssid); //--strip
            s = s + "_" + ssid;
        }
        if (addr != null && !addr.isEmpty()) {
            log("adding addr " + addr); //--strip
            s = s + "_" + addr;
        }
        log("returning " + s); //--strip
        return s;
    }

    cfg_android_private_t cfg;
    String home;

    long ts = 0;
    public String name_;
    public String ssid;
    public String addr;
    public endpoint_t endpoint;

    public hmi_t hmi;
    hmi_t.hmi_callback_t cb;
    device_endpoints_t parent;



}


