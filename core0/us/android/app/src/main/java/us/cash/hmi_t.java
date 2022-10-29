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
import java.util.Arrays;                                                                       // Arrays
import us.gov.crypto.base58;                                                                   // base58
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import us.wallet.trader.bookmarks_t;                                                           // bookmarks_t
import us.wallet.trader.bookmark_t;                                                            // bookmark_t
import java.io.BufferedReader;                                                                 // BufferedReader
import us.gov.socket.busyled_t;                                                                // busyled_t
import java.io.ByteArrayOutputStream;                                                          // ByteArrayOutputStream
import android.graphics.drawable.ColorDrawable;                                                // ColorDrawable
import android.content.Context;                                                                // Context
import us.gov.socket.datagram;                                                                 // datagram
import us.gov.crypto.ec;                                                                       // ec
import android.os.Environment;                                                                 // Environment
import java.io.File;                                                                           // File
import java.io.FileInputStream;                                                                // FileInputStream
import java.io.FileOutputStream;                                                               // FileOutputStream
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.gov.id.types.*;                                                               // *
import static us.gov.io.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import us.wallet.engine.ip4_endpoint_t;                                                  // hash_t
import java.io.InputStreamReader;                                                              // InputStreamReader
import java.io.IOException;                                                                    // IOException
import static us.ko.is_ko;                                                                     // is_ko
import java.security.KeyPair;                                                                  // KeyPair
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import java.security.PrivateKey;                                                               // PrivateKey
import us.wallet.trader.qr_t;                                                                  // qr_t
import java.nio.charset.StandardCharsets;                                                      // StandardCharsets
import java.lang.StringBuilder;                                                                // StringBuilder
import us.string;                                                                              // string
import java.io.UnsupportedEncodingException;                                                   // UnsupportedEncodingException

public class hmi_t extends us.wallet.cli.hmi {

    public static final ko KO_54503 = new ko("KO 54503 already looking up.");
    public static final ko KO_50493 = new ko("KO 50493 Unknown node pkh. Unable to resolve IP.");
    public static final ko KO_10000 = new ko("KO 10000 Disconnected by peer.");
    public static final ko KO_10001 = new ko("KO 10001 I Disconnected.");
    public static final ko KO_61210 = new ko("KO 61210 Handshake could not be completed.");

    private static void log(final String line) {         //--strip
        CFG.log_android("hmi_wallet_t: " + line);        //--strip
    }                                                    //--strip

    public interface status_handler_t {
        void on_status(ColorDrawable led, final String msg);
    }

    public interface hmi_callback_t {
        void on_hmi__worker(final ko status);
    }

    boolean online = false;

    public void on_hmi__worker(final ko status) {
        online = (status == ok);
        if (cb != null) {
            cb.on_hmi__worker(status);
        }
    }

    public hmi_t(app a_, hmi_callback_t cb_, final us.wallet.cli.params p, status_handler_t status_handler_, datagram_dispatcher_t dispatcher_) {
        super(p, System.out);
        a = a_;
        cb = cb_;
        dispatcher = dispatcher_;
        status_handler = status_handler_;
        set_status(leds_t.led_red, "Stopped");
    }

    public String get_msg_log() {
        return msg_log.toString();
    }

    public void clear_msg_log() {
        msg_log = new StringBuilder(100);
    }

    void set_status(ColorDrawable led, String msg) {
        app.assert_worker_thread(); //--strip
        log("set_status " + led + " " + msg); //--strip
        if (led != leds_t.led_red) {
            freeze = false;
        }
        if (freeze) {
            msg_log.append("* IGN:");
            msg_log.append(msg);
            msg_log.append('\n');
            return;
        }
        if (led == leds_t.led_red) {
            freeze = true;
        }
        cur_led = led;
        cur_msg = msg;
        msg_log.append("* ");
        msg_log.append(msg);
        msg_log.append('\n');
        report_status();
    }

    public void add_status_handler(status_handler_t sh) {
        secondary_status_handler = sh;
        if (secondary_status_handler == null) return;
        secondary_status_handler.on_status(cur_led, cur_msg);
    }

    public void report_status() {
        app.assert_worker_thread(); //--strip
        log("reporting status: " + cur_msg); //--strip
        status_handler.on_status(cur_led, cur_msg);
        if (secondary_status_handler != null) {
            secondary_status_handler.on_status(cur_led, cur_msg);
        }
        log("end reporting status"); //--strip
    }

    public void report_status__ui() {
        app.assert_ui_thread(); //--strip
        Thread thread = new Thread(new Runnable() {
            @Override public void run() {
                report_status();
            }
        });
        thread.start();
    }

    void set_status_gov(ColorDrawable led, String msg) {
        app.assert_worker_thread(); //--strip
        cur_led_gov = led;
        cur_msg_gov = msg;
        report_status_gov();
    }

    public void report_status_gov() {
        app.assert_worker_thread(); //--strip
        log("reporting status gov"); //--strip
        status_handler_gov.on_status(cur_led_gov, cur_msg_gov);
        log("end reporting status gov"); //--strip
    }

    @Override public pair<ko, us.gov.io.cfg1> load_cfg(String home, boolean gen) {
        return new pair<ko, us.gov.io.cfg1>(ok, cfg);
    }

    public cfg_android_private_t get_cfg() {
        return (cfg_android_private_t) cfg;
    }

    public ko restart(ip4_endpoint_t ep, pin_t pin) {
        app.assert_worker_thread(); //--strip
        log("restart hmi pin " + ep.to_string() + " pin>0 " + (pin.value > 0 ? "Yes" : "No")); //--strip
        freeze = false;
        set_status(leds_t.led_amber, "restarting HMI. " + ep.to_string());
        stop();
        join();
        log("starting hmi once stopped"); //--strip
        ip4_endpoint = ep;
        ko r = start(ep, pin); // busyled_handler_send, busyled_handler_recv, dispatcher);
        log("hmi start return code is " + ko.as_string(r)); //--strip
        return r;
    }

    pair<ko, us.wallet.cli.rpc_peer_t.net_info_out_dst_t> load_netinfo(channel_t channel) {
        log("load_netinfo(channel=" + channel.value + ")"); //--strip
        byte[] fc = ((cfg_android_private_t)cfg).read_file2("netinfo_ch_" + channel.value);
        if (fc == null) {
            ko r = new ko("KO 66094 netinfo cached file unavailable.");
            log(r.msg); //--strip
            return new pair<ko, us.wallet.cli.rpc_peer_t.net_info_out_dst_t>(r, null);
        }
        blob_t blob = new blob_t(fc);
        blob_reader_t reader = new blob_reader_t(blob);
        us.wallet.cli.rpc_peer_t.net_info_out_dst_t o = new us.wallet.cli.rpc_peer_t.net_info_out_dst_t();
        {
            ko r = reader.read(o);
            if (is_ko(r)) {
                return new pair<ko, us.wallet.cli.rpc_peer_t.net_info_out_dst_t>(r, null);
            }
        }
        return new pair<ko, us.wallet.cli.rpc_peer_t.net_info_out_dst_t>(ok, o);
    }

    ko save_netinfo(final us.wallet.cli.rpc_peer_t.net_info_out_dst_t data, final channel_t channel) {
        log("save_netinfo " + data.wallet_address.encode() + "." + data.subhome + " " + data.seeds.size() + " seeds.");  //--strip
        us.wallet.cli.rpc_peer_t.net_info_out_t o = new us.wallet.cli.rpc_peer_t.net_info_out_t(data.wallet_address, data.subhome, data.seeds);
        blob_t blob = new blob_t();
        blob_writer_t writer = new blob_writer_t(blob, o.blob_size());
        writer.write(o);
        ko r = get_cfg().write_file("netinfo_ch_" + channel.value, blob.value);
        if (r != null) {
            return new ko("KO 25934 Unable to write netinfo file in private storage.");
        }
        return ok;
    }

    public ko start(final ip4_endpoint_t ep, final pin_t pin) {
        log("start"); //--strip
        if (cfg == null) {
            return new ko("KO 80799 cfg is null");
        }
        freeze = false; //show the first error, then ign the rest
        is_online = false;
        ip4_endpoint = ep;
        p.pin = pin;
        if (ip4_endpoint != null) {
            p.walletd_host = ip4_endpoint.shost;
            p.walletd_port = ip4_endpoint.port;
            p.channel = ip4_endpoint.channel;
        }
        else {
            p.walletd_host = new shost_t("localhost");
            p.walletd_port = us.CFG.WALLET_PORT;
            p.channel = us.CFG.CHANNEL;
        }
        set_status(leds_t.led_amber, "Starting HMI. " + ip4_endpoint.to_string());
        log("super.start. stop_on_disconnection = " + p.rpc__stop_on_disconnection); //--strip
        ko r = super.start(busyled_handler_send, busyled_handler_recv, dispatcher);
        if (is_ko(r)) {
            log("super returned " + r.msg); //--strip
            set_status(leds_t.led_red, r.msg);
            on_hmi__worker(r);
            return r;
        }
        set_status(leds_t.led_amber, "Loading previous network info for channel " + p.channel.value);
        update_network_info_from_cache(p.channel);
        set_status(leds_t.led_amber, (seeds == null ? "0" : "" + seeds.size()) + " seeds");
        cfg_android_public_t cfg_pub = new cfg_android_public_t(a, get_cfg().home);

        sw_updates = new sw_updates_t(a, this, get_cfg(), cfg_pub);
/*
        r = sw_updates.start();
        if (is_ko(r)) {
            set_status(leds_t.led_red, r.msg);
            on_hmi__worker(r);
            sw_updates = null;
            super.stop();
            super.join();
            return r;
        }
*/
        set_status(leds_t.led_amber, "HMI running. Peer " + ip4_endpoint.to_string());
        log("super returned ok"); //--strip
        return ok;
    }

    @Override public void stop() {
//        if (sw_updates != null) sw_updates.stop();
        super.stop();
        set_status(leds_t.led_amber, "Stopping HMI.");
        is_online = false;
    }

    @Override public void join() {
//        if (sw_updates != null) sw_updates.join();
        sw_updates = null;
        super.join();
        set_status(leds_t.led_red, "HMI stop.");
    }

    void on_connect__worker(final ko error) {
        app.assert_worker_thread(); //--strip
        log("on_connect. " + (error == ok ? "ok" : error.msg)); //--strip
        if (is_ko(error)) {
            log("try_renew_ip_on_connect_failed = " + try_renew_ip_on_connect_failed); //--strip
            if (try_renew_ip_on_connect_failed) {
                set_status(leds_t.led_blue, "Trying to renew IP");
                final hmi_t o = this;
                final ko err = error;
                Thread task = new Thread(new Runnable () {
                    @Override public void run() {
                        log("looking up IP address..."); //--strip
                        pair<ko, ip4_endpoint_t> r = lookup_ip(new app.progress_t() {
                            @Override public void on_progress(final String msg) {
                                log("renew IP:" + msg); //--strip;
                                o.set_status(leds_t.led_blue, msg);
                            }
                        });
                        if (r.first != ok) {
                            log("lookup task returned: " + r.first.msg); //--strip
                            if (r.first.equals(KO_54503)) { //reentry
                                log("reentry. lookupip. ignored. " + r.first.msg); //--strip
                            }
                            else if (r.first.equals(KO_50493)) { //no pkh
                                o.set_status(leds_t.led_red, err.msg);
                                on_hmi__worker(err);
                            }
                            else {
                                o.set_status(leds_t.led_red, err.msg);
                                on_hmi__worker(err);
                            }
                            return;
                        }
                    }
                });
                task.start();
            }
            else {
                set_status(leds_t.led_red, error.msg);
                on_hmi__worker(error);
            }
        }
    }

    @Override public void on_connect(final ko error) {
        super.on_connect(error);
        if (app.is_ui_thread()) {
            Thread task = new Thread(new Runnable () {
                @Override public void run() {
                    on_connect__worker(error);
                }
            });
            task.start();
        }
        else {
            on_connect__worker(error);
        }
    }

    public ko update_network_info_from_cache(final channel_t channel) {
        log("update_network_info_from_cache ch=" + channel.value); //--strip
        pair<ko, us.wallet.cli.rpc_peer_t.net_info_out_dst_t> o = load_netinfo(channel);
        if (is_ko(o.first)) {
            return o.first;
        }
        seeds = o.second.seeds;
        log("seeds length=" + seeds.size()); //--strip
        wallet_address = o.second.wallet_address;
        subhome = o.second.subhome.value;
        log("wallet_address=" + wallet_address.encode() + " subhome=" + subhome);  //--strip
        return ok;
    }

    public ko update_network_info(channel_t channel) {
        set_status(leds_t.led_amber, "Fetching network data...");
        us.wallet.cli.rpc_peer_t.net_info_out_dst_t o = new us.wallet.cli.rpc_peer_t.net_info_out_dst_t();
        ko r = rpc_peer.call_net_info(o);
        if (is_ko(r)) {
            return r;
        }
        save_netinfo(o, channel);
        seeds = o.seeds;
        log("seeds length=" + seeds.size()); //--strip
        wallet_address = o.wallet_address;
        subhome = o.subhome.value;
        log("wallet_address=" + wallet_address.encode() + " subhome=" + subhome);  //--strip
        return ok;
    }

    @Override public void on_I_disconnected(final reason_t reason) {
        super.on_I_disconnected(reason);
        log("I disconnected. Reason: " + reason.value); //--strip
        set_status(leds_t.led_red, KO_10001.msg + " Reason: " + reason.value);
        is_online = false;
        on_hmi__worker(KO_10001);
    }

    @Override public void on_peer_disconnected(final reason_t reason) {
        log("peer disconnected. Reason: " + reason.value); //--strip
        set_status(leds_t.led_red, KO_10000.msg + " Reason given: " + reason.value);
        is_online = false;
        on_hmi__worker(KO_10000);
        super.on_peer_disconnected(reason);
    }

    @Override public void verification_completed(boolean verok) { //boolean means if peer pubkey is verified
        log("verification_completed " + verok); //--strip
        super.verification_completed(verok);
        is_online = verok;
        set_status(leds_t.led_amber, "handshake completed with " + ip4_endpoint.to_string());
        Thread thread = new Thread(new Runnable() {
            @Override public void run() {
                try {
                    Thread.sleep(1000);
                }
                catch(Exception e) {
                }

                if (is_online) {
                    log("online task"); //--strip
                    ko r = update_network_info(p.channel);
                    if (r != ok) {
                        set_status(leds_t.led_red, r.msg);
                        on_hmi__worker(r);
                        return;
                    }
                    set_status(leds_t.led_green, "online with " + ip4_endpoint.to_string());
                    on_hmi__worker(ok);
                }
                else {
                    log("Not online"); //--strip
                }
            }
        });
        thread.start();
    }

    public pair<ko, String> ping() {
        string ans = new string();
        ko r = rpc_peer.call_ping(new string("ping"), ans);
        if (is_ko(r)) {
            return new pair<ko, String>(r, "");
        }
        return new pair<ko, String>(ok, ans.value);
    }

    public pair<ko, String> balance(final uint16_t detail) {
        string ans = new string();
        ko r = rpc_peer.call_balance(detail, ans);
        if (is_ko(r)) {
            return new pair<ko, String>(r, "");
        }
        return new pair<ko, String>(ok, ans.value);
    }

    public pair<ko, blob_t> pay(int amount, int fee, hash_t rcpt_address, String sig) {
        us.wallet.cli.rpc_peer_t.transfer_in_t o = new us.wallet.cli.rpc_peer_t.transfer_in_t(rcpt_address, new int64_t(amount+fee), new hash_t(0), new uint8_t((short)1));
        us.gov.io.types.blob_t blob = new us.gov.io.types.blob_t();
        ko r = rpc_peer.call_transfer(o, blob);
        if (is_ko(r)) {
            return new pair<ko, blob_t>(r, null);
        }
        return new pair<ko, blob_t>(ok, blob);
    }

    public boolean isAddressValid(final String addr) {
        try {
            byte[] decoded = base58.decode(addr);
            return decoded.length > 0;
        }
        catch(Exception e) {
            error_manager.manage(e, e.getMessage() + "    " + e.toString());
            return false;
        }
    }

    public pair<ko, String> list_trades() {
        string s = new string();
        ko r = rpc_peer.call_list_trades(s);
        if (is_ko(r)) {
            return new pair<ko, String>(r, null);
        }
        return new pair<ko, String>(ok, s.value);
    }

    public pair<ko, us.gov.io.types.vector_hash> list_wallets() {
        us.gov.io.types.vector_hash o = new us.gov.io.types.vector_hash();
        ko r = rpc_peer.call_world(o);
        if (is_ko(r)) {
            return new pair<ko, us.gov.io.types.vector_hash>(r, null);
        }
        return new pair<ko, us.gov.io.types.vector_hash>(ok, o);
    }

    public ko command_trade(final hash_t tradeid, final String command) {
        log("command_trade " + command); //--strip
        if (tradeid == null) return new ko("KO 78684 Missing Trade id");
        us.wallet.cli.rpc_peer_t.exec_trade_in_t o = new us.wallet.cli.rpc_peer_t.exec_trade_in_t(tradeid, new string(command));
        return rpc_peer.call_exec_trade(o);
    }

    public pair<ko, hash_t> new_trade(final hash_t parent_trade, final String datasubdir, final qr_t qr) {
        log("new trade in datasubdir = " + datasubdir); //--strip
        string dsubdir = new string(datasubdir);
        blob_t blob = new blob_t();
        blob_writer_t.write(qr, blob);
        hash_t tid = new hash_t(0);
        us.wallet.cli.rpc_peer_t.trade_in_t o = new us.wallet.cli.rpc_peer_t.trade_in_t(parent_trade, dsubdir, blob);
        log("new_trade " + qr.to_string()); //--strip
        ko r = rpc_peer.call_trade(o, tid);
        if (is_ko(r)) {
            return new pair<ko, hash_t>(r, null);
        }
        return new pair<ko, hash_t>(ok, tid);
    }

    public pair<ko, String> kill_trade(final hash_t tradeid) {
        string data = new string();
        ko r = rpc_peer.call_kill_trade(tradeid, data);
        if (is_ko(r)) {
            return new pair<ko, String>(r, null);
        }
        return new pair<ko, String>(ok, data.value);
    }

    public pair<ko, bookmarks_t> bookmarks_me() {
        log("bookmarks_me"); //--strip
        if (rpc_peer == null) {
            ko r = new ko("KO 25534 HMI is not connected.");
            log(r.msg); //--strip
            return new pair<ko, bookmarks_t>(r, null);
        }
        pair<ko, bookmarks_t> ret = new pair<ko, bookmarks_t>();
        ret.second = new bookmarks_t();
        ko r = rpc_peer.call_qr(ret.second);
        if (is_ko(r)) {
            ret.first = r;
            ret.second = null;
            return ret;
        }
        ret.first = ok;
        return ret;
    }

    public pair<ko, ip4_endpoint_t> lookup_ip(app.progress_t progress) {
        log("lookup_ip"); //--strip
        if (lookingup) {
            log(KO_54503.msg); //--strip
            return new pair<ko, ip4_endpoint_t>(KO_54503, null);
        }
        lookingup = true;
        set_status(leds_t.led_blue, "Updating IP address");
        if (wallet_address == null || wallet_address.is_zero()) {
            lookingup = false;
            log(KO_50493.msg); //--strip
            return new pair<ko, ip4_endpoint_t>(KO_50493, null);
        }
        log("seeds=" + seeds.size()); //--strip
        pair<ko, ip4_endpoint_t> r = gov_client_t.lookup_wallet_ip(wallet_address, get_keys(), seeds, p.channel, progress);
        if (r.first != ok) {
            lookingup = false;
            return r;
        }
        if (r.second.equals(ip4_endpoint)) {
        log("X16"); //--strip
            log("IP hasn't changed. " + ip4_endpoint.to_string()); //--strip
            lookingup = false;
            return r;
        }
        String msg = "Wallet IP address has changed (from " + ip4_endpoint.to_string() + " to " + r.second.to_string() + "). Reconnecting...";
        progress.on_progress(msg);
        log(msg); //--strip
        log("restarting hmi"); //--strip
        ko r2 = restart(r.second, new pin_t(0));
        if (r2 != ok) {
            lookingup = false;
            return new pair<ko, ip4_endpoint_t>(r2, r.second);
        }
        log("hmi started " + r.second.to_string()); //--strip
        lookingup = false;
        return new pair<ko, ip4_endpoint_t>(ok, r.second);
    }

    public void set_manual_mode(boolean b) {
        if (b == manual_mode) return;
        manual_mode = b;
        if (manual_mode) {
            log("manual mode is ON"); //--strip
            try_renew_ip_on_connect_failed = false;
            p.rpc__stop_on_disconnection = true;
        }
        else {
            log("manual mode is OFF"); //--strip
            try_renew_ip_on_connect_failed = true;
            p.rpc__stop_on_disconnection = false;
        }
        if (is_active()) {
            rpc_daemon.handler.stop_on_disconnection = p.rpc__stop_on_disconnection;
        }
    }

    @Override public void upgrade_software() {
        log("Peer is signaling the existence of a upgrade_software. Calling check4updates."); //--strip
        sw_updates.check4updates();

/*
        sw_updates_t sw_updates = new sw_updates_t(a, this, get_cfg(), cfg_pub);
        r = sw_updates.start();
        if (is_ko(r)) {
            set_status(leds_t.led_red, r.msg);
            on_hmi__worker(r);
            sw_updates = null;
            super.stop();
            super.join();
            return r;
        }
*/
    }

    app a;
    hmi_callback_t cb;
    public boolean is_online = false;
    public boolean try_renew_ip_on_connect_failed = true;
    status_handler_t status_handler = null;
    status_handler_t secondary_status_handler = null;
    ColorDrawable cur_led = leds_t.led_off;
    String cur_msg = null;
    StringBuilder msg_log = new StringBuilder(100);
    boolean freeze = false;
    status_handler_t status_handler_gov = null;
    ColorDrawable cur_led_gov = leds_t.led_off;
    String cur_msg_gov = null;
    busyled_t.handler_t busyled_handler_send = null;
    busyled_t.handler_t busyled_handler_recv = null;
    busyled_t.handler_t busyled_handler_online = null;
    public datagram_dispatcher_t dispatcher = null;
    boolean lookingup = false;
    hash_t wallet_address = null;
    public String subhome = "";
    us.gov.io.types.vector_tuple_hash_host_port seeds;
    public ip4_endpoint_t ip4_endpoint = null;
    boolean manual_mode = false;

    sw_updates_t sw_updates = null;
}

