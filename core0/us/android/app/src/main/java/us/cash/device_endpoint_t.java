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
import us.gov.crypto.base58;                                                                   // base58
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.types.blob_t;                                                                 // blob_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import java.util.Date;                                                                         // Date
import us.gov.crypto.ec;                                                                       // ec
import static us.gov.crypto.types.*;                                                           // *
import static us.gov.id.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import us.stdint.*;                                                                            // *
import us.wallet.engine.ip4_endpoint_t;                                                        // ip4_endpoint_t
import static us.ko.is_ko;                                                                     // is_ko
import java.security.KeyPair;                                                                  // KeyPair
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import java.security.PrivateKey;                                                               // PrivateKey
import static us.gov.io.types.blob_t.serid_t;                                                  // serid_t
import us.string;                                                                              // string
import us.wallet.engine.wallet_connection_t;                                                   // wallet_connection_t

public final class device_endpoint_t extends wallet_connection_t { // implements hmi_t.hmi_callback_t {

    private static void log(final String line) {                        //--strip
        CFG.log_android("device_endpoint_t: " + line);                  //--strip
    }                                                                   //--strip

    public device_endpoint_t(device_endpoints_t parent_, int for_read_from_blob) {
        parent = parent_;
        hmi = null;
        cfg = null;
    }

    public device_endpoint_t(device_endpoints_t parent_, String k_b58, String subhome, wallet_connection_t wallet_connection) {
        super(wallet_connection.name_.value, subhome, wallet_connection.ip4_endpoint);
        parent = parent_;
        hmi = null;
        cfg = new cfg_android_private_t(parent.a.getApplicationContext(), k_b58);
        cfg.home = get_home();
    }

    public device_endpoint_t(device_endpoints_t parent_, String name, String subhome, ip4_endpoint_t ip4_endpoint) {
        super(name, subhome, ip4_endpoint);
        parent = parent_;
        hmi = null;
        cfg = new cfg_android_private_t(parent.a.getApplicationContext());
        cfg.home = get_home();
    }

    public device_endpoint_t(device_endpoints_t parent_, wallet_connection_t wallet_connection) {
        super(wallet_connection.name_.value, wallet_connection.subhome.value, wallet_connection.ip4_endpoint);
        parent = parent_;
        hmi = null;
        cfg = new cfg_android_private_t(parent.a.getApplicationContext());
        cfg.home = get_home();
    }

    public device_endpoint_t(device_endpoints_t parent_) {
        parent = parent_;
        hmi = null;
        cfg = new cfg_android_private_t(parent.a.getApplicationContext());
        cfg.home = get_home();
    }

    private device_endpoint_t(device_endpoint_t other) {}
/*
 {
        super(other);
        parent = other.parent;
        hmi = null;
        cfg = new cfg_android_private_t(parent.a.getApplicationContext());
        cfg.home = get_home();
    }
*/

    String get_home() {
        return parent.home + "/" + us.gov.crypto.ec.instance.to_encoded_address(cfg.keys.getPublic());
    }

    public void poweron(app a, pin_t pin, app.progress_t progress) {
        log("poweron"); //--strip
        app.assert_worker_thread();  //--strip
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
        p.homedir = cfg.home + "/hmi";
        p.subhome = subhome.value;
        log("hmi homedir=" + p.homedir); //--strip
        p.rpc__connect_for_recv = true;
        p.rpc__stop_on_disconnection = false;
        log("creating datagram dispatcher"); //--strip
        hmi = new hmi_t(this, p, new app.walletd_status_handler_t(a), new datagram_dispatcher_t(3));
        hmi.cfg = cfg;
        hmi.busyled_handler_recv = new app.walletd_led_handler(a, 0);
        hmi.busyled_handler_send = new app.walletd_led_handler(a, 1);
        log("start HMI"); //--strip

        hmi.try_renew_ip_on_connect_failed = true;
        if (ip4_endpoint == null) {
            hmi = null;
            ko r = new ko("KO 54093 invalid ep in settings. Restart of HMI's been aborted.");
            log(r.msg); //--strip
            progress.on_progress(r.msg);
            return;
        }
        if (ui != null) {
            ui.refresh__worker();
        }

        log("starting HMI with ep " + ip4_endpoint.to_string() + "; pin " + pin.value); //--strip
        progress.on_progress("Starting connection to " + (ip4_endpoint == null ? "null" : ip4_endpoint.to_string()));
        ko r = hmi.start(ip4_endpoint, pin);
        if (is_ko(r)) {
            log("HMI Failed to start: " + r.msg); //--strip
            progress.on_progress("Failed to connect to " + (ip4_endpoint == null ? "null" : ip4_endpoint.to_string()));
            return;
        }
        log("hmi started"); //--strip
        progress.on_progress("HMI restarted " + (ip4_endpoint == null ? "null" : ip4_endpoint.to_string()));
        if (ui != null) {
            ui.refresh__worker();
        }
    }

    public void poweroff(app.progress_t progress) {
        log("poweroff"); //--strip
        app.assert_worker_thread();  //--strip
        if (hmi == null) {
            log("hmi was null"); //--strip
            progress.on_progress("HMI was already powered OFF");
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
        if (ui != null) {
            ui.refresh__worker();
        }
    }

    public void on_hmi__worker(final ko status) {
        app.assert_worker_thread(); //--strip
        log("on_hmi__worker"); //--strip
        if (status == ok) {
            log("HMI is ok. Node is up"); //--strip
            log("saving ip4_endpoint " + ip4_endpoint.to_string()); //--strip
            if (hmi.wallet_address != null) {
                addr = new string(hmi.wallet_address.encode());
            }
            ts = new uint64_t(new Date().getTime());
            parent.save();
        }
        if (parent.a.main != null) {
            parent.a.main.refresh__worker();
        }
        if (parent.a.active_ac != null) {
            if (parent.a.active_ac != parent.a.main) {
                parent.a.active_ac.refresh__worker();
            }
        }
    }

    public ko set_ip4_endpoint(ip4_endpoint_t ip4_endpoint_) {
//        if (hmi != null) {
//            return new ko("KO 92001 Cannot change ip4_endpoint while HMI is running.");
//        }
        log("set_ip4_endpoint " + ip4_endpoint_.to_string()); //--strip
        ip4_endpoint = ip4_endpoint_;
        return ok;
    }

    public void set_subhome(String subhome_) {
        subhome.value = subhome_;
    }

    public boolean confirm_subhome(String subhome_) {
        if (subhome_.equals(subhome)) {
            return false;
        }
        subhome.value = subhome_;
        parent.save();
        return true;
    }

    @Override public serid_t serial_id() { return serid_t.no_header; }

    @Override public int blob_size() {
        int sz = super.blob_size() +
                 priv_t.ser_size +
                 uint8_t.size();
        return sz;
    }

    @Override public void to_blob(blob_writer_t writer) {
        log("to_blob"); //--strip
        uint8_t pwr = new uint8_t(hmi != null ? (short)1 : (short)0);
        if (hmi_req_on) {
            log("hmi_req_on is 1"); //--strip
            pwr.value = 1;
        }
        priv_t priv = new priv_t(cfg.keys.getPrivate());
        super.to_blob(writer);
        writer.write(priv);
        writer.write(pwr);
        log("to_blob ok. name=" + name_.value + " pwr=" + pwr.value); //--strip
    }

    @Override public ko from_blob(blob_reader_t reader) {
        log("from_blob"); //--strip
        if (cfg != null) {
            return new ko("KO 89986 Object already initialized.");
        }
        {
            ko r = super.from_blob(reader);
            if (ko.is_ko(r)) return r;
        }
        {
            priv_t priv = new priv_t();
            ko r = reader.read(priv);
            if (ko.is_ko(r)) return r;
            cfg = new cfg_android_private_t(parent.a.getApplicationContext(), priv.value, "");
            cfg.home = get_home();
            log("cfg home " + cfg.home); //--strip
        }
        {
            uint8_t pwr = new uint8_t((short)0);
            ko r = reader.read(pwr);
            if (ko.is_ko(r)) return r;
            hmi_req_on = pwr.value != 0;
            log("hmi_req_on " + hmi_req_on); //--strip
        }
        log("from_blob ok. name=" + name_.value + " hmi_req_on=" + hmi_req_on); //--strip
        return ok;
    }

    public hmi_t hmi;
    public boolean hmi_req_on = false;
    //hmi_t.hmi_callback_t cb;
    device_endpoints_t parent;
    cfg_android_private_t cfg;
    node_pairing ui = null;
}

