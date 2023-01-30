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
package us.wallet.engine;
import java.util.ArrayList;                                                                    // ArrayList
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import us.wallet.trader.bookmark_info_t;                                                       // bookmark_info_t
import us.wallet.trader.bookmarks_t;                                                           // bookmarks_t
import us.wallet.trader.bookmark_t;                                                            // bookmark_t
import us.CFG;                                                                                 // CFG
import us.gov.socket.datagram;                                                                 // datagram
import us.wallet.trader.endpoint_t;                                                            // endpoint_t
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.gov.socket.types.*;                                                           // *
import static us.stdint.*;                                                                     // *
import us.gov.crypto.types.*;                                                                  // *
import us.gov.id.types.*;                                                                      // *
import us.gov.io.types.*;                                                                      // *
import us.ko;                                                                                  // ko
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import us.wallet.protocol;                                                                     // protocol
import us.wallet.trader.protocol_selection_t;                                                  // protocol_selection_t
import java.security.PublicKey;                                                                // PublicKey
import us.wallet.trader.qr_t;                                                                  // qr_t
import java.net.Socket;                                                                        // Socket
import us.string;                                                                              // string
import us.wallet.trader.bootstrap.protocols_t;
import us.wallet.trader.cert.cert_index_t;                                                                              // string
import us.wallet.trader.cert.cert_t;                                                                              // string

public abstract class rpc_peer_t extends us.gov.relay.rpc_peer_t implements caller_api {

    static void log(final String line) {                        //--strip
        CFG.log_wallet_engine("rpc_peer_t: " + line);           //--strip
    }                                                           //--strip

    public rpc_peer_t(rpc_daemon_t rpc_daemon) {
        super(rpc_daemon);
    }

    public rpc_peer_t(rpc_daemon_t rpc_daemon, Socket sock) {
        super(rpc_daemon, sock);
    }

    public svc_t translate_svc(svc_t svc, boolean inbound) {
        if (inbound) {
            rpc_daemon_t.svcfish.from_prev(svc);
        }
        else {
            rpc_daemon_t.svcfish.to_prev(svc);
        }
        log("Using API versioning translator. newsvc " + svc.value);  //--strip
        return svc;    
    }

    @Override public boolean process_work(datagram d) {
        log("process_work svc " + d.service.value); //--strip
        /// Assuming the following svc ordering:
        /// master definition: us/api/model.cpp
        //static_assert(socket_end <= id_end);
        //static_assert(id_end <= auth_end);
        //static_assert(auth_end <= peer_end);
        //static_assert(peer_end <= relay_end);
        if (d.service.value < us.gov.protocol.relay_end) {
            log("processing net_daemon datagram" + d.service.value); //--strip
            return super.process_work(d);
        }
        return false;
    }

    //#include <us/api/generated/wallet/java/engine/cllr_rpc-impl>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/wallet/java/engine/cllr_rpc-impl>
    @Override public ko call_print_grid(string data) {
        log("print_grid "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.engine_print_grid), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.engine_print_grid_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.engine_print_grid_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, data);
        return r;
    }

    @Override public ko call_print_socket_connections(string data) {
        log("print_socket_connections "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.engine_print_socket_connections), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.engine_print_socket_connections_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.engine_print_socket_connections_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, data);
        return r;
    }

    @Override public ko call_allw(string data) {
        log("allw "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.engine_allw), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.engine_allw_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.engine_allw_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, data);
        return r;
    }

    @Override public ko call_sync(string data) {
        log("sync "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.engine_sync), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.engine_sync_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.engine_sync_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, data);
        return r;
    }

    @Override public ko call_reload_file(final string filename, string ans) {
        log("reload_file "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.engine_reload_file), new seq_t(0), filename), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.engine_reload_file_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.engine_reload_file_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, ans);
        return r;
    }

    @Override public ko call_get_component_update(final get_component_update_in_t o_in, get_component_update_out_dst_t o_out) {
        log("get_component_update "); //--strip
        /// in:
        ///     string brandcode;
        ///     string component;
        ///     string vcsname;

        /// out:
        ///    string vcsname;
        ///    bin_t bin_pkg;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.engine_get_component_update_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.engine_get_component_update_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, o_out);
        return r;
    }

    @Override public ko call_get_component_hash(final get_component_hash_in_t o_in, string curver) {
        log("get_component_hash "); //--strip
        /// in:
        ///     string component;
        ///     string filename;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.engine_get_component_hash_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.engine_get_component_hash_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, curver);
        return r;
    }

    @Override public ko call_harvest(final harvest_in_t o_in, string ans) {
        log("harvest "); //--strip
        /// in:
        ///     uint16_t id;
        ///     uint16_t timeout;
        ///     uint16_t decay;
        ///     priv_t priv;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.engine_harvest_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.engine_harvest_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, ans);
        return r;
    }

    @Override public ko call_world(vector_hash wallets) {
        log("world "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.engine_world), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.engine_world_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.engine_world_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, wallets);
        return r;
    }

    @Override public ko call_lookup_wallet(final hash_t addr, lookup_wallet_out_dst_t o_out) {
        log("lookup_wallet "); //--strip
        /// out:
        ///    uint32_t net_addr;
        ///    uint16_t port;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.engine_lookup_wallet), new seq_t(0), addr), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.engine_lookup_wallet_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.engine_lookup_wallet_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, o_out);
        return r;
    }

    @Override public ko call_random_wallet(random_wallet_out_dst_t o_out) {
        log("random_wallet "); //--strip
        /// out:
        ///    hash_t addr;
        ///    uint32_t net_addr;
        ///    uint16_t port;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.engine_random_wallet), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.engine_random_wallet_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.engine_random_wallet_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, o_out);
        return r;
    }

    @Override public ko call_register_w(final uint32_t net_addr, string ans) {
        log("register_w "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.engine_register_w), new seq_t(0), net_addr), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.engine_register_w_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.engine_register_w_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, ans);
        return r;
    }

    @Override public ko call_conf(final conf_in_t o_in, string ans) {
        log("conf "); //--strip
        /// in:
        ///     string key;
        ///     string value;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.engine_conf_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.engine_conf_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, ans);
        return r;
    }

    @Override public ko call_r2r_all_index_hdr(protocols_t protocols) {
        log("r2r_all_index_hdr "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.engine_r2r_all_index_hdr), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.engine_r2r_all_index_hdr_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.engine_r2r_all_index_hdr_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, protocols);
        return r;
    }

    @Override public ko call_r2r_all_bookmarks(final protocol_selection_t protocol_selection, bookmarks_t bookmarks) {
        log("r2r_all_bookmarks "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.engine_r2r_all_bookmarks), new seq_t(0), protocol_selection), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.engine_r2r_all_bookmarks_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.engine_r2r_all_bookmarks_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, bookmarks);
        return r;
    }

    @Override public ko call_r2r_all_index(bookmark_index_t bookmark_index) {
        log("r2r_all_index "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.engine_r2r_all_index), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.engine_r2r_all_index_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.engine_r2r_all_index_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, bookmark_index);
        return r;
    }
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.
    //#include <us/api/generated/wallet/java/pairing/cllr_rpc-impl>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/wallet/java/pairing/cllr_rpc-impl>
    @Override public ko call_pair_device(final pair_device_in_t o_in, string ans) {
        log("pair_device "); //--strip
        /// in:
        ///     pub_t pub;
        ///     string subhome;
        ///     string name;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.pairing_pair_device_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.pairing_pair_device_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, ans);
        return r;
    }

    @Override public ko call_unpair_device(final pub_t pub, string ans) {
        log("unpair_device "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.pairing_unpair_device), new seq_t(0), pub), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.pairing_unpair_device_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.pairing_unpair_device_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, ans);
        return r;
    }

    @Override public ko call_list_devices(string data) {
        log("list_devices "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.pairing_list_devices), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.pairing_list_devices_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.pairing_list_devices_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, data);
        return r;
    }

    @Override public ko call_attempts(vector_string data) {
        log("attempts "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.pairing_attempts), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.pairing_attempts_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.pairing_attempts_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, data);
        return r;
    }

    @Override public ko call_prepair_device(final prepair_device_in_t o_in, prepair_device_out_dst_t o_out) {
        log("prepair_device "); //--strip
        /// in:
        ///     uint16_t pin;
        ///     string subhome;
        ///     string name;

        /// out:
        ///    uint16_t pin;
        ///    string subhome;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.pairing_prepair_device_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.pairing_prepair_device_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, o_out);
        return r;
    }

    @Override public ko call_unprepair_device(final uint16_t pin, string ans) {
        log("unprepair_device "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.pairing_unprepair_device), new seq_t(0), pin), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.pairing_unprepair_device_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.pairing_unprepair_device_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, ans);
        return r;
    }
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.
    //#include <us/api/generated/wallet/java/r2r/cllr_rpc-impl>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/wallet/java/r2r/cllr_rpc-impl>
    @Override public ko call_trading_msg(final trading_msg_in_t o_in) {
        log("trading_msg "); //--strip
        /// in:
        ///     hash_t tid;
        ///     uint16_t code;
        ///     bin_t payload;

        return send1(o_in.get_datagram(daemon.channel, new seq_t(0)));
    }

    @Override public ko call_trading_msg2(final trading_msg2_in_t o_in) {
        log("trading_msg2 "); //--strip
        /// in:
        ///     uint64_t route;
        ///     hash_t tid;
        ///     uint16_t code;
        ///     bin_t payload;

        return send1(o_in.get_datagram(daemon.channel, new seq_t(0)));
    }
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.
    //#include <us/api/generated/wallet/java/wallet/cllr_rpc-impl>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/wallet/java/wallet/cllr_rpc-impl>
    @Override public ko call_balance(final uint16_t detail, string data) {
        log("balance "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_balance), new seq_t(0), detail), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_balance_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_balance_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, data);
        return r;
    }

    @Override public ko call_list(final uint16_t detail, string data) {
        log("list "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_list), new seq_t(0), detail), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_list_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_list_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, data);
        return r;
    }

    @Override public ko call_recv_address(hash_t addr) {
        log("recv_address "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_recv_address), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_recv_address_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_recv_address_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, addr);
        return r;
    }

    @Override public ko call_new_address(hash_t addr) {
        log("new_address "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_new_address), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_new_address_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_new_address_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, addr);
        return r;
    }

    @Override public ko call_add_address(final priv_t priv, hash_t addr) {
        log("add_address "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_add_address), new seq_t(0), priv), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_add_address_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_add_address_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, addr);
        return r;
    }

    @Override public ko call_transfer(final transfer_in_t o_in, blob_t blob_ev) {
        log("transfer "); //--strip
        /// in:
        ///     hash_t addr;
        ///     int64_t amount;
        ///     hash_t coin;
        ///     uint8_t relay;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_transfer_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_transfer_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, blob_ev);
        return r;
    }

    @Override public ko call_transfer_from(final transfer_from_in_t o_in, blob_t blob_ev) {
        log("transfer_from "); //--strip
        /// in:
        ///     hash_t src_addr;
        ///     hash_t rcpt_addr;
        ///     int64_t amount;
        ///     hash_t coin;
        ///     uint8_t relay;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_transfer_from_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_transfer_from_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, blob_ev);
        return r;
    }

    @Override public ko call_tx_charge_pay(final tx_charge_pay_in_t o_in, blob_t blob_ev) {
        log("tx_charge_pay "); //--strip
        /// in:
        ///     hash_t addr;
        ///     int64_t samount;
        ///     hash_t scoin;
        ///     int64_t ramount;
        ///     hash_t rcoin;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_tx_charge_pay_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_tx_charge_pay_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, blob_ev);
        return r;
    }

    @Override public ko call_invoice_verify(final invoice_verify_in_t o_in, string err) {
        log("invoice_verify "); //--strip
        /// in:
        ///     blob_t blob_ev;
        ///     int64_t oamount;
        ///     hash_t ocoin;
        ///     int64_t iamount;
        ///     hash_t icoin;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_invoice_verify_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_invoice_verify_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, err);
        return r;
    }

    @Override public ko call_payment_verify(final payment_verify_in_t o_in, string err) {
        log("payment_verify "); //--strip
        /// in:
        ///     blob_t blob_ev;
        ///     int64_t oamount;
        ///     hash_t ocoin;
        ///     int64_t iamount;
        ///     hash_t icoin;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_payment_verify_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_payment_verify_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, err);
        return r;
    }

    @Override public ko call_tx_pay(final tx_pay_in_t o_in, blob_t blob_ev2) {
        log("tx_pay "); //--strip
        /// in:
        ///     blob_t blob_ev;
        ///     uint8_t relay;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_tx_pay_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_tx_pay_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, blob_ev2);
        return r;
    }

    @Override public ko call_tx_new(blob_t blob_ev) {
        log("tx_new "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_tx_new), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_tx_new_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_tx_new_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, blob_ev);
        return r;
    }

    @Override public ko call_tx_add_section(final tx_add_section_in_t o_in, blob_t blob_ev) {
        log("tx_add_section "); //--strip
        /// in:
        ///     blob_t blob_ev;
        ///     hash_t coin;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_tx_add_section_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_tx_add_section_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, blob_ev);
        return r;
    }

    @Override public ko call_tx_add_input(final tx_add_input_in_t o_in, blob_t blob_ev) {
        log("tx_add_input "); //--strip
        /// in:
        ///     blob_t blob_ev;
        ///     uint16_t section;
        ///     hash_t addr;
        ///     int64_t amount;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_tx_add_input_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_tx_add_input_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, blob_ev);
        return r;
    }

    @Override public ko call_tx_add_output(final tx_add_output_in_t o_in, blob_t blob_ev) {
        log("tx_add_output "); //--strip
        /// in:
        ///     blob_t blob_ev;
        ///     uint16_t section;
        ///     hash_t addr;
        ///     int64_t amount;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_tx_add_output_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_tx_add_output_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, blob_ev);
        return r;
    }

    @Override public ko call_tx_sign(final tx_sign_in_t o_in, blob_t blob_ev) {
        log("tx_sign "); //--strip
        /// in:
        ///     blob_t blob_ev;
        ///     blob_t blob_sigcode;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_tx_sign_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_tx_sign_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, blob_ev);
        return r;
    }

    @Override public ko call_tx_send(final blob_t blob_ev, string info) {
        log("tx_send "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_tx_send), new seq_t(0), blob_ev), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_tx_send_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_tx_send_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, info);
        return r;
    }

    @Override public ko call_tx_decode(final blob_t blob_ev, string tx_pretty) {
        log("tx_decode "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_tx_decode), new seq_t(0), blob_ev), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_tx_decode_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_tx_decode_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, tx_pretty);
        return r;
    }

    @Override public ko call_tx_check(final blob_t blob_ev, string data) {
        log("tx_check "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_tx_check), new seq_t(0), blob_ev), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_tx_check_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_tx_check_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, data);
        return r;
    }

    @Override public ko call_set_supply(final set_supply_in_t o_in, blob_t blob_ev) {
        log("set_supply "); //--strip
        /// in:
        ///     hash_t addr;
        ///     int64_t amount;
        ///     uint8_t relay;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_set_supply_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_set_supply_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, blob_ev);
        return r;
    }

    @Override public ko call_store_kv(final store_kv_in_t o_in, blob_t blob_ev) {
        log("store_kv "); //--strip
        /// in:
        ///     hash_t addr;
        ///     string key;
        ///     string value;
        ///     uint8_t relay;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_store_kv_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_store_kv_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, blob_ev);
        return r;
    }

    @Override public ko call_rm_kv(final rm_kv_in_t o_in, blob_t blob_ev) {
        log("rm_kv "); //--strip
        /// in:
        ///     hash_t addr;
        ///     string key;
        ///     uint8_t relay;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_rm_kv_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_rm_kv_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, blob_ev);
        return r;
    }

    @Override public ko call_store_file(final store_file_in_t o_in, store_file_out_dst_t o_out) {
        log("store_file "); //--strip
        /// in:
        ///     hash_t addr;
        ///     string path;
        ///     blob_t content;
        ///     uint8_t relay;

        /// out:
        ///    blob_t blob_ev;
        ///    string path;
        ///    hash_t digest;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_store_file_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_store_file_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, o_out);
        return r;
    }

    @Override public ko call_rm_file(final rm_file_in_t o_in, blob_t blob_ev) {
        log("rm_file "); //--strip
        /// in:
        ///     hash_t addr;
        ///     string path;
        ///     hash_t digest;
        ///     uint8_t relay;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_rm_file_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_rm_file_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, blob_ev);
        return r;
    }

    @Override public ko call_search(final search_in_t o_in, string result) {
        log("search "); //--strip
        /// in:
        ///     hash_t addr;
        ///     string pattern;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_search_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_search_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, result);
        return r;
    }

    @Override public ko call_file(final hash_t digest, bin_t content) {
        log("file "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_file), new seq_t(0), digest), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_file_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_file_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, content);
        return r;
    }

    @Override public ko call_exec(final string cmd) {
        log("exec "); //--strip
        return send1(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_exec), new seq_t(0), cmd));
    }

    @Override public ko call_get_subhome(string subhome) {
        log("get_subhome "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_get_subhome), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_get_subhome_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_get_subhome_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, subhome);
        return r;
    }

    @Override public ko call_net_info(net_info_out_dst_t o_out) {
        log("net_info "); //--strip
        /// out:
        ///    hash_t wallet_address;
        ///    string subhome;
        ///    vector_tuple_hash_host_port seeds;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_net_info), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_net_info_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_net_info_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, o_out);
        return r;
    }

    @Override public ko call_compilance_report(final compilance_report_in_t o_in, string data) {
        log("compilance_report "); //--strip
        /// in:
        ///     string jurisdiction;
        ///     string date_from;
        ///     string date_to;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_compilance_report_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_compilance_report_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, data);
        return r;
    }

    @Override public ko call_priv_key(final priv_t priv, priv_key_out_dst_t o_out) {
        log("priv_key "); //--strip
        /// out:
        ///    pub_t pub;
        ///    hash_t addr;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_priv_key), new seq_t(0), priv), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_priv_key_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_priv_key_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, o_out);
        return r;
    }

    @Override public ko call_pub_key(final pub_t pub, pub_key_out_dst_t o_out) {
        log("pub_key "); //--strip
        /// out:
        ///    pub_t pub;
        ///    hash_t addr;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_pub_key), new seq_t(0), pub), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_pub_key_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_pub_key_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, o_out);
        return r;
    }

    @Override public ko call_gen_keys(gen_keys_out_dst_t o_out) {
        log("gen_keys "); //--strip
        /// out:
        ///    priv_t priv;
        ///    pub_t pub;
        ///    hash_t addr;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_gen_keys), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_gen_keys_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_gen_keys_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, o_out);
        return r;
    }

    @Override public ko call_sign(final sign_in_t o_in, sign_out_dst_t o_out) {
        log("sign "); //--strip
        /// in:
        ///     bin_t msg;
        ///     priv_t priv;

        /// out:
        ///    pub_t pub;
        ///    sig_t sig;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_sign_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_sign_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, o_out);
        return r;
    }

    @Override public ko call_verify(final verify_in_t o_in, uint8_t result) {
        log("verify "); //--strip
        /// in:
        ///     bin_t msg;
        ///     pub_t pub;
        ///     sig_t sig;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_verify_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_verify_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, result);
        return r;
    }

    @Override public ko call_encrypt(final encrypt_in_t o_in, bin_t msg_scrambled) {
        log("encrypt "); //--strip
        /// in:
        ///     bin_t msg;
        ///     priv_t src_priv;
        ///     pub_t rcpt_pub;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_encrypt_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_encrypt_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, msg_scrambled);
        return r;
    }

    @Override public ko call_decrypt(final decrypt_in_t o_in, bin_t msg) {
        log("decrypt "); //--strip
        /// in:
        ///     bin_t msg_scrambled;
        ///     pub_t src_pub;
        ///     priv_t rcpt_priv;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_decrypt_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_decrypt_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, msg);
        return r;
    }

    @Override public ko call_patch_os(final patch_os_in_t o_in, patch_os_out_dst_t o_out) {
        log("patch_os "); //--strip
        /// in:
        ///     string script;
        ///     priv_t groot_key;
        ///     uint8_t relay;

        /// out:
        ///    blob_t blob_ev;
        ///    hash_t job;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_patch_os_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_patch_os_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, o_out);
        return r;
    }

    @Override public ko call_list_protocols(string data) {
        log("list_protocols "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_list_protocols), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_list_protocols_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_list_protocols_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, data);
        return r;
    }

    @Override public ko call_trade(final trade_in_t o_in, hash_t tid) {
        log("trade "); //--strip
        /// in:
        ///     hash_t parent_trade;
        ///     string datasubdir;
        ///     blob_t qr;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_trade_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_trade_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, tid);
        return r;
    }

    @Override public ko call_list_trades(string data) {
        log("list_trades "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_list_trades), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_list_trades_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_list_trades_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, data);
        return r;
    }

    @Override public ko call_kill_trade(final hash_t tid, string data) {
        log("kill_trade "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_kill_trade), new seq_t(0), tid), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_kill_trade_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_kill_trade_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, data);
        return r;
    }

    @Override public ko call_exec_trade(final exec_trade_in_t o_in) {
        log("exec_trade "); //--strip
        /// in:
        ///     hash_t tid;
        ///     string cmd;

        return send1(o_in.get_datagram(daemon.channel, new seq_t(0)));
    }

    @Override public ko call_qr(bookmarks_t bookmarks) {
        log("qr "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_qr), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_qr_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_qr_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, bookmarks);
        return r;
    }

    @Override public ko call_bookmark_add(final bookmark_add_in_t o_in, string ans) {
        log("bookmark_add "); //--strip
        /// in:
        ///     string name;
        ///     bookmark_t bookmark;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_bookmark_add_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_bookmark_add_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, ans);
        return r;
    }

    @Override public ko call_bookmark_delete(final string key, string ans) {
        log("bookmark_delete "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_bookmark_delete), new seq_t(0), key), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_bookmark_delete_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_bookmark_delete_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, ans);
        return r;
    }

    @Override public ko call_bookmark_list(bookmarks_t bookmarks) {
        log("bookmark_list "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_bookmark_list), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_bookmark_list_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_bookmark_list_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, bookmarks);
        return r;
    }

    @Override public ko call_timeseries_list(string ans) {
        log("timeseries_list "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_timeseries_list), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_timeseries_list_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_timeseries_list_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, ans);
        return r;
    }

    @Override public ko call_timeseries_show(final timeseries_show_in_t o_in, string ans) {
        log("timeseries_show "); //--strip
        /// in:
        ///     hash_t addr;
        ///     uint64_t ts;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_timeseries_show_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_timeseries_show_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, ans);
        return r;
    }

    @Override public ko call_timeseries_new(final timeseries_new_in_t o_in, string ans) {
        log("timeseries_new "); //--strip
        /// in:
        ///     hash_t addr;
        ///     uint8_t monotonic;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_timeseries_new_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_timeseries_new_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, ans);
        return r;
    }

    @Override public ko call_timeseries_add(final timeseries_add_in_t o_in, string ans) {
        log("timeseries_add "); //--strip
        /// in:
        ///     hash_t addr;
        ///     bin_t payload;

        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(o_in.get_datagram(daemon.channel, new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_timeseries_add_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_timeseries_add_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, ans);
        return r;
    }

    @Override public ko call_r2r_index_hdr(protocols_t protocols) {
        log("r2r_index_hdr "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_r2r_index_hdr), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_r2r_index_hdr_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_r2r_index_hdr_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, protocols);
        return r;
    }

    @Override public ko call_r2r_bookmarks(final protocol_selection_t protocol_selection, bookmarks_t bookmarks) {
        log("r2r_bookmarks "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_r2r_bookmarks), new seq_t(0), protocol_selection), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_r2r_bookmarks_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_r2r_bookmarks_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, bookmarks);
        return r;
    }

    @Override public ko call_r2r_index(bookmark_index_t bookmark_index) {
        log("r2r_index "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_r2r_index), new seq_t(0)), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_r2r_index_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_r2r_index_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, bookmark_index);
        return r;
    }

    @Override public ko call_cert_create(final string msg, hash_t nft) {
        log("cert_create "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_cert_create), new seq_t(0), msg), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_cert_create_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_cert_create_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, nft);
        return r;
    }

    @Override public ko call_cert_import(final cert_t cert, hash_t nft) {
        log("cert_import "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_cert_import), new seq_t(0), cert), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_cert_import_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_cert_import_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, nft);
        return r;
    }

    @Override public ko call_cert_list(final uint8_t id, cert_index_t certs) {
        log("cert_list "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_cert_list), new seq_t(0), id), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_cert_list_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_cert_list_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, certs);
        return r;
    }

    @Override public ko call_cert_get(final hash_t nft, cert_t cert) {
        log("cert_get "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_cert_get), new seq_t(0), nft), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_cert_get_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_cert_get_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, cert);
        return r;
    }

    @Override public ko call_cert_show(final hash_t nft, string cert) {
        log("cert_show "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.wallet_cert_show), new seq_t(0), nft), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.wallet_cert_show_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.wallet_cert_show_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, cert);
        return r;
    }
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.


    @Override public void dump(final String prefix, PrintStream os) {
        os.println(prefix + "wallet::engine::rpc_peer_t");
        super.dump(prefix, os);
    }

}

