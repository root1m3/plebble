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
import java.util.ArrayList;                                                                    // ArrayList
import us.gov.crypto.base58;                                                                   // base58
import java.io.ByteArrayOutputStream;                                                          // ByteArrayOutputStream
import java.nio.charset.Charset;                                                               // Charset
import us.gov.socket.datagram;                                                                 // datagram
import java.util.Date;                                                                         // Date
import us.gov.crypto.ripemd160.hash_t;                                                         // hash_t
import static us.gov.socket.types.*;                                                           // *
import static us.ko.*;                                                                         // *
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import java.security.KeyPair;                                                                  // KeyPair
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import java.io.OutputStream;                                                                   // OutputStream
import us.pair;                                                                                // pair
import java.security.PublicKey;                                                                // PublicKey
import java.util.Random;                                                                       // Random
import java.io.UnsupportedEncodingException;                                                   // UnsupportedEncodingException

public class gov_client_t extends us.gov.cli.rpc_daemon_t {

    public static void log(final String line) {         //--strip
        CFG.log_android(line);                          //--strip
    }                                                   //--strip

    public gov_client_t(KeyPair k0, endpoint_t ep) {
        super(k0, ep.channel, ep.shostport(), us.gov.id.peer_t.role_t.role_device, null);
        //channel = ep.channel;
    }

    @Override
    public ko start() {
//        log("set system channel " + channel.value); //--strip
//        us.gov.socket.datagram.system_channel = channel;
        return super.start();
    }

    pair<ko, hostport_t> lookup_wallet_ip(hash_t wallet_address, app.progress_t progress) {
        log("lookup_wallet_ip " + wallet_address.encode()); //--strip
        progress.on_progress("Waiting for response from node " + peer.endpoint());
        us.gov.engine.rpc_peer_t.lookup_wallet_out_dst_t o = new us.gov.engine.rpc_peer_t.lookup_wallet_out_dst_t();
        ko r = get_peer().call_lookup_wallet(wallet_address, o);
        if (is_ko(r)) {
            progress.on_progress(r.msg);
            return new pair<ko, hostport_t>(r, null);
        }
        hostport_t ret = new hostport_t(new host_t(o.net_addr.value), new port_t(o.port.value));
        progress.on_progress("Valid response arrived from node " + peer.endpoint() + ": " + peer.endpoint(ret));
        return new pair<ko, hostport_t>(ok, ret);
    }

    static pair<ko, hostport_t> get_random_node(final ArrayList<tuple3<hash_t, host_t, port_t>> nodes) {
        Random rand = new Random(System.currentTimeMillis());
        if (nodes.size() > 0) {
            int n = rand.nextInt(nodes.size());
            log("GetRandomNode: " + n + "/nodes.length " + nodes.size() + " " + nodes.get(n).item0.encode() + " " + nodes.get(n).item1.value + " " + nodes.get(n).item2.value); //--strip
            return new pair<ko, hostport_t>(ok, new hostport_t(nodes.get(n).item1, nodes.get(n).item2));
        }
        ko r = new ko("KO 32013 Empty network.");
        log(r.msg); //--strip
        return new pair<ko, hostport_t>(r, null);
    }

    public static pair<ko, endpoint_t> lookup_wallet_ip(hash_t wallet_address, KeyPair keys, ArrayList<tuple3<hash_t, host_t, port_t>> nodes, channel_t channel, app.progress_t progress) {
        pair<ko, endpoint_t> ans = new pair<ko, endpoint_t>(ok, null);
        log("lookup_wallet_ip for " + wallet_address.encode() + " " + " nodes sz " + nodes.size()); //--strip;
        pair<ko, hostport_t> node = get_random_node(nodes);
        if (is_ko(node.first)) {
            ans.first = node.first;
            progress.on_progress(ans.first.msg);
            return ans;
        }
        progress.on_progress("starting gov client against node " + us.gov.socket.client.endpoint(node.second));
        gov_client_t cli = new gov_client_t(keys, new endpoint_t(node.second.first, node.second.second, channel));
        {
            log("cli.start"); //--strip
            ko r = cli.start();
            if (is_ko(r)) {
                ans.first = r;
                progress.on_progress(ans.first.msg);
                return ans;
            }
            log("wait ready"); //--strip
            r = cli.wait_ready(new Date(System.currentTimeMillis() + 3000L));
            if (is_ko(r)) {
                ans.first = r;
                progress.on_progress(ans.first.msg);
                return ans;
            }
            log("waiting for auth"); //--strip
            r = cli.get_peer().wait_auth();
            if (is_ko(r)) {
                ans.first = r;
                progress.on_progress(ans.first.msg);
                return ans;
            }
        }
        progress.on_progress("Asking node at " + us.gov.socket.client.endpoint(node.second) + " for wallet " + wallet_address.encode());
        pair<ko, hostport_t> r = cli.lookup_wallet_ip(wallet_address, progress);
        progress.on_progress("Stopping gov cli");
        log("stopping gov cli"); //--strip
        cli.stop();
        cli.join();
        progress.on_progress("Stopped gov cli");
        log("stopped gov cli"); //--strip
        if (is_ko(r.first)) {
            progress.on_progress(r.first.msg);
            ans.first = r.first;
            return ans;
        }
        ans.first = ok;
        ans.second = new endpoint_t(r.second.first, r.second.second, channel);
        log("solved: " + ans.second.to_string()); //--strip
        progress.on_progress("IP Address found: " + ans.second.to_string());
        return ans;
    }

    public pair<ko, endpoint_t> renew_ip(app.progress_t progress) {
        return new pair<ko, endpoint_t>(new ko("KO 87962 Not implemented"), null);
    }

    //channel_t channel = new channel_t(0);
};

