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
package us.wallet.cli;
import java.util.ArrayList;                                                                    // ArrayList
import us.gov.crypto.base58;                                                                   // base58
import us.wallet.trader.bookmarks_t;                                                           // bookmarks_t
import java.io.BufferedReader;                                                                 // BufferedReader
import us.gov.socket.busyled_t;                                                                // busyled_t
import us.gov.io.cfg0;                                                                         // cfg0
import us.gov.io.cfg1;                                                                         // cfg1
import us.CFG;                                                                                 // CFG
import java.nio.charset.Charset;                                                               // Charset
import static us.wallet.trader.chat.chat_t;                                                    // chat_t
import us.gov.socket.client;                                                                   // client
import java.util.concurrent.locks.Condition;                                                   // Condition
import us.gov.socket.datagram;                                                                 // datagram
import us.wallet.trader.data_t;                                                                // data_t
import java.util.Date;                                                                         // Date
import us.gov.crypto.ec;                                                                       // ec
import java.io.File;                                                                           // File
import java.nio.file.Files;                                                                    // Files
import java.security.GeneralSecurityException;                                                 // GeneralSecurityException
import us.gov.crypto.ripemd160.hash_t;                                                         // hash_t
import java.security.*;                                                                        // *
import static us.gov.crypto.types.*;                                                           // *
import static us.gov.id.types.*;                                                               // *
import static us.gov.io.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import us.stdint.*;                                                                            // *
import us.wallet.*;                                                                            // *
import java.io.InputStreamReader;                                                              // InputStreamReader
import java.io.IOException;                                                                    // IOException
import static us.ko.is_ko;                                                                     // is_ko
import java.security.KeyPair;                                                                  // KeyPair
import us.ko;                                                                                  // ko
import java.util.ListIterator;                                                                 // ListIterator
import static us.ko.ok;                                                                        // ok
import java.io.OutputStream;                                                                   // OutputStream
import us.pair;                                                                                // pair
import java.nio.file.Paths;                                                                    // Paths
import java.io.PrintStream;                                                                    // PrintStream
import java.security.PrivateKey;                                                               // PrivateKey
import java.security.PublicKey;                                                                // PublicKey
import java.io.RandomAccessFile;                                                               // RandomAccessFile
import java.util.concurrent.locks.ReentrantLock;                                               // ReentrantLock
import us.gov.crypto.ripemd160;                                                                // ripemd160
import us.wallet.trader.roles_t;                                                               // roles_t
import java.util.Scanner;                                                                      // Scanner
import us.gov.io.screen;                                                                       // screen
import us.gov.crypto.sha256;                                                                   // sha256
import us.gov.io.shell_args;                                                                   // shell_args
import us.string;                                                                              // string
import us.gov.crypto.symmetric_encryption;                                                     // symmetric_encryption

public class hmi {

    private static void log(final String line) {     //--strip
        CFG.log_wallet_cli("hmi: " + line);          //--strip
    }                                                //--strip

    private static void logdump(final shell_args args) {    //--strip
        if (CFG.logs.wallet)                                //--strip
        args.dump("hmi: args>",System.out);                 //--strip
    }                                                       //--strip

    public static final ko KO_11000 = new ko("KO 11000 Invalid command.");
    public static final ko KO_10020 = new ko("KO 10020 Feature not available in RPC mode.");
    public static final ko KO_10021 = new ko("KO 10021 Feature only available in RPC mode.");
    public static final ko KO_91810 = new ko("KO 91810 Not an offline command.");
    public static final ko KO_40322 = new ko("KO 40322 Not implemented.");
    public static final ko KO_39201 = new ko("KO 39201 already started.");

    public static class dispatcher_t implements datagram.dispatcher_t {

        private static void log(final String line) {    //--strip
           CFG.log_wallet(line);                        //--strip
        }                                               //--strip

        public dispatcher_t(hmi i) {
            m = i;
        }

        @Override public boolean dispatch(datagram d) {
            log("scr lock - dispatch " + d.service); //--strip
            String pfx = "   ";
            screen scr = m.scr;
            scr.lock.lock();
            try {
                if (scr.capturing) {
                    scr.os.println();
                }
                switch(d.service.value) {
                    case us.gov.protocol.relay_push: {
                        rpc_peer_t.push_in_dst_t o_in = new rpc_peer_t.push_in_dst_t();
                        {
                            us.gov.io.blob_reader_t reader = new us.gov.io.blob_reader_t(d);
                            ko r = reader.read(o_in);
                            if (is_ko(r)) {
                                scr.println("HMI: [D " + d.service + "] " + r.msg);
                                break;
                            }
                        }
                        log("dispatcher_t::dispatch relay_push" + o_in.tid + " " + o_in.code + " " + o_in.blob.size()); //--strip
                        switch(o_in.code.value) {
                            case us.wallet.trader.trader_t.push_trade: {
                                log("trader_t::push_trade " + o_in.tid); //--strip
                                scr.println("New trade. " + o_in.tid);
                                m.new_trade(o_in.tid);
                            }
                            break;
                            case us.wallet.trader.trader_t.push_killed: {
                                log("trader_t::kill_trade " + o_in.tid); //--strip
                                scr.println("Trade " + o_in.tid + " has been killed.");
                            }
                            break;
                        }
                        if (m.cur.equals(o_in.tid)) {
                            switch (o_in.code.value) {
                                case us.wallet.trader.trader_t.push_help:
                                case us.gov.relay.pushman.push_ko:
                                case us.gov.relay.pushman.push_ok: {
                                    log("dispatch push blob sz " + o_in.blob.size() + "code" + o_in.code); //--strip
                                    string payload = new string();
                                    ko r = us.gov.io.blob_reader_t.parse(o_in.blob, payload);
                                    if (is_ko(r)) {
                                        scr.println(r.msg);
                                        break;
                                    }
                                    scr.println(payload.value);
                                }
                                break;
                                case us.wallet.trader.trader_t.push_data: {
                                    log("trader_t::push_data " + o_in.tid); //--strip
                                    string payload = new string();
                                    ko r = us.gov.io.blob_reader_t.parse(o_in.blob, payload);
                                    if (is_ko(r)) {
                                        scr.println(r.msg);
                                        break;
                                    }
                                    {
                                        //lock_guard<mutex> lock(m.assets_mx);
                                        m.data.from(payload.value);
                                    }
                                    scr.println("Data:");
                                    m.data.dump(pfx, scr.os);
                                }
                                break;
                                case us.wallet.trader.trader_t.push_chat: {
                                    log("trader_t::push_chat " + o_in.tid); //--strip
                                    chat_t chat = new chat_t();
                                    ko r = us.gov.io.blob_reader_t.parse(o_in.blob, chat);
                                    if (is_ko(r)) {
                                        scr.println(r.msg);
                                        break;
                                    }
                                    scr.println("Chat:");
                                    chat.dump("   ", scr.os);
                                }
                                break;
                                case us.wallet.trader.trader_t.push_roles: {
                                    log("trader_t::push_roles " + o_in.tid); //--strip
                                    //lock_guard<mutex> locka(m.assets_mx);
                                    ko r = us.gov.io.blob_reader_t.parse(o_in.blob, m.roles);
                                    if (is_ko(r)) {
                                        scr.println(r.msg);
                                        break;
                                    }
                                    scr.println("Matching roles:");
                                    m.roles.dump(pfx, scr.os);
                                }
                                break;
                                case us.wallet.trader.trader_t.push_roles_mine: {
                                    roles_t roles = new roles_t();
                                    ko r = us.gov.io.blob_reader_t.parse(o_in.blob, roles);
                                    if (is_ko(r)) {
                                        scr.println(r.msg);
                                        break;
                                    }
                                    log("trader_t::push_roles_mine " + o_in.tid); //--strip
                                    roles.dump("My roles: ", scr.os);
                                }
                                break;
                                case us.wallet.trader.trader_t.push_roles_peer: {
                                    roles_t roles = new roles_t();
                                    ko r = us.gov.io.blob_reader_t.parse(o_in.blob, roles);
                                    if (is_ko(r)) {
                                        scr.println(r.msg);
                                        break;
                                    }
                                    log("trader_t::push_roles_peer " + o_in.tid); //--strip
                                    roles.dump("Peer roles: ", scr.os);
                                }
                                break;
                                case us.wallet.trader.trader_t.push_log: {
                                    string payload = new string();
                                    ko r = us.gov.io.blob_reader_t.parse(o_in.blob, payload);
                                    if (is_ko(r)) {
                                        scr.println(r.msg);
                                        break;
                                    }
                                    log("trader_t::push_log " + o_in.tid); //--strip
                                    scr.println("Trade Log:");
                                    scr.println(payload.value);
                                }
                                break;
                                case us.wallet.trader.workflow.trader_protocol.push_workflow_item: {
                                    log("trader_t::push_workflow_item " + o_in.tid); //--strip
                                    scr.println("Workflow item.");
                                }
                                break;
                                case us.wallet.trader.workflow.trader_protocol.push_doc: {
                                    string payload = new string();
                                    ko r = us.gov.io.blob_reader_t.parse(o_in.blob, payload);
                                    if (is_ko(r)) {
                                        scr.println(r.msg);
                                        break;
                                    }
                                    log("trader_t::push_doc " + o_in.tid); //--strip
                                    scr.println("Workflow doc:");
                                    scr.println(payload.value);
                                }
                                break;
                                case us.wallet.trader.trader_protocol.push_logo: {
                                    log("trader_protocol::push_logo " + o_in.tid); //--strip
                                    scr.println("Received logo file of size " + o_in.blob.size() + " bytes.");
                                }
                                break;
                                case us.wallet.trader.trader_protocol.push_ico: {
                                    log("trader_protocol::push_ico " + o_in.tid); //--strip
                                    scr.println("Received ico file of size " + o_in.blob.size() + " bytes.");
                                }
                                break;
                                case us.wallet.trader.workflow.trader_protocol.push_redirects: {
                                    log("trader_protocol::push_redirects " + o_in.tid); //--strip
                                    bookmarks_t bookmarks = new bookmarks_t();
                                    ko r = us.gov.io.blob_reader_t.readB(o_in.blob, bookmarks);
                                    if (is_ko(r)) {
                                        scr.println(r.msg);
                                        break;
                                    }
                                    scr.println("Remote redirects:");
                                    bookmarks.dump("   ", scr.os);
                                }
                                break;
                                case us.wallet.trader.trader_t.push_bookmarks: {
                                    log("trader_t::push_bookmarks "); //--strip
                                    bookmarks_t bookmarks = new bookmarks_t();
                                    ko r = us.gov.io.blob_reader_t.readB(o_in.blob, bookmarks);
                                    if (is_ko(r)) {
                                        scr.println(r.msg);
                                        break;
                                    }
                                    scr.println("Bookmarks:");
                                    bookmarks.dump("   ", scr.os);
                                }
                                break;
                            }
                        }

                    }
                    break;
                    default:
                        scr.println("HMI: [D " + d.service + "] KO 79968 Not handled.");
                }
                if (scr.capturing) {
                    scr.print_prompt();
                }
                log("scr lock - dispatch"); //--strip
            }
            finally {
                scr.lock.unlock();
                log("scr unlock - dispatch"); //--strip
            }
            return true;
        }

        hmi m;
    }

    public hmi(PrintStream os) {
        constructor(os);
    }

    public hmi(String[] args0, PrintStream os) {
        p = new params(new shell_args(args0));
        constructor(os);
    }

    public hmi(params p_, PrintStream os) {
        p = p_;
        constructor(os);
    }

    public hmi(shell_args a, PrintStream os) {
        p = new params(a);
        constructor(os);
    }

    void constructor(PrintStream os) {
        scr = new screen(os);
        home = p.get_home_wallet();
        log("arguments:"); //--strip
        logdump(p.args); //--strip
    }

    public static class blhandler_t implements us.gov.socket.busyled_t.handler_t {

        public blhandler_t(hmi h, int n) {
            this.h = h;
            this.n = n;
        }

        @Override public void on_busy() {
            //screen::lock_t lock(h.scr, h.interactive);
            //lock.os << (n == 0 ? "S-->" : "      <--R");
        }

        @Override public void on_idle() {
            //screen::lock_t lock(h.scr, h.interactive);
            //lock.os << (n == 0 ? "S---" : "      ---R");
        }

        int n;
        hmi h;
    }

    public void on_connect(ko r) {
        log("on_connect."); //--strip
        if (is_ko(r)) {
            scr.lock.lock();
            scr.os.println(rewrite(r));
            scr.lock.unlock();
        }
    }

    public void on_stop() {
        log("on_stop"); //--strip
    }

    public void verification_completed(boolean verif_ok) {
        log("verification_completed " + verif_ok); //--strip
    }

    public void verification_result(request_data_t request_data) {
        log("verification_result " + request_data.value); //--strip
//        scr.lock.lock();
//        scr.os.println("subhome is " + request_data.value);
//        scr.lock.unlock();
    }

    public void on_I_disconnected(final reason_t reason) {
        log("I disconnected. Reason: " + reason.value); //--strip
    }

    public void on_peer_disconnected(final reason_t reason) {
        log("peer disconnected. Reason: " + reason.value); //--strip
        scr.lock.lock();
        scr.os.println("Peer disconnected with reason: " + reason.value);
        scr.lock.unlock();
    }

    void setup_signals(boolean on) {
        if (on) {
            //signal(SIGPIPE, SIG_IGN);
        }
        else {
            //signal(SIGPIPE, SIG_DFL);
        }
    }

    public void upgrade_software() {
        log("Peer is signaling the existence of a upgrade_software."); //--strip
    }

    ko start_rpc_daemon(busyled_t.handler_t busyled_handler_send, busyled_t.handler_t busyled_handler_recv, datagram.dispatcher_t dis) {
        log("stating rpc_daemon"); //--strip
        if (rpc_daemon != null) {
            ko r = new ko("KO 39201 already started");
            log(r.msg); //--strip
            return r;
        }
        log("rpc mode. home " + p.get_home_wallet_rpc_client()); //--strip
        {
            pair<ko, us.gov.io.cfg1> r = load_cfg(p.get_home_wallet_rpc_client(), true);
            if (is_ko(r.first)) {
                return r.first;
            }
            cfg = r.second;
        }
        shostport_t shostport = new shostport_t(p.walletd_host, p.walletd_port);
        rpc_daemon = new us.wallet.cli.rpc_daemon_t(this, cfg.keys, shostport, rpc_peer_t.role_t.role_device, p.subhome, dis);
        rpc_daemon.handler.connect_for_recv = p.rpc__connect_for_recv;
        rpc_daemon.handler.stop_on_disconnection = p.rpc__stop_on_disconnection;
        log("rpc_daemon.handler.connect_for_recv = " + rpc_daemon.handler.connect_for_recv); //--strip
        log("rpc_daemon.handler.stop_on_disconnection = " + rpc_daemon.handler.stop_on_disconnection); //--strip
        log("set_busy_handlers send: " + (busyled_handler_send == null ? "null" : "not null") + " recv: " + (busyled_handler_recv == null ? "null" : "not null")); //--strip
        rpc_daemon.set_busy_handlers(busyled_handler_send, busyled_handler_recv);
        {
            ko r = rpc_daemon.start();
            if (is_ko(r)) {
                rpc_daemon = null;
                cfg = null;
                return r;
            }
        }
        {
            ko r = rpc_daemon.wait_ready(CFG.HMI_READY_TIMEOUT_SECS);
            if (is_ko(r)) {
                rpc_daemon = null;
                cfg = null;
                return r;
            }
        }
        rpc_peer = (rpc_peer_t)rpc_daemon.peer;
        assert(rpc_peer != null);
        if (rpc_daemon.handler.connect_for_recv) {
            {
                log("wait_connected"); //--strip
                ko r = rpc_daemon.handler.wait_connected();
                if (is_ko(r)) {
                    rpc_daemon.stop();
                    rpc_peer = null;
                    rpc_daemon = null;
                    cfg = null;
                    return r;
                }
            }
            {
                log("waiting for auth handshake to complete"); //--strip
                ko r = rpc_peer.wait_auth();
                if (is_ko(r)) {
                    rpc_daemon.stop();
                    rpc_peer = null;
                    rpc_daemon = null;
                    cfg = null;
                    return r;
                }
                log("ready"); //--strip
            }
        }
        else { //--strip
            log("rpc_daemon.handler.connect_for_recv is 0. Not waiting for auth"); //--strip
        } //--strip
        return ok;
    }

    public ko start(busyled_t.handler_t busyled_handler_send, busyled_t.handler_t busyled_handler_recv, datagram.dispatcher_t dis) {
        log("stating hmi"); //--strip
        //us.gov.socket.datagram.system_channel = p.channel;
        //log("set system chanel " + us.gov.socket.datagram.system_channel.value); //--strip
        ko r = start_rpc_daemon(busyled_handler_send, busyled_handler_recv, dis);
        if (is_ko(r)) {
            return r;
        }
        setup_signals(true);
        return ok;
    }

    public ko start(datagram.dispatcher_t dispatcher) {
        log("start"); //--strip
        return start(new blhandler_t(this, 0), new blhandler_t(this, 1), dispatcher);
    }

    public ko start() {
        log("start"); //--strip
        return start(create_dispatcher());
    }

    public void stop() {
        log("stopping hmi"); //--strip
        setup_signals(false);
        if (rpc_daemon != null) {
            rpc_daemon.stop();
        }
    }

    public void join() {
        log("waiting for hmi"); //--strip
        if (rpc_daemon != null) {
            rpc_daemon.join();
            rpc_daemon = null;
        }
        cfg = null;
        log("hmi is stopped."); //--strip
    }

    void new_trade(final hash_t tid) {
        if (interactive) {
            //screen::lock_t lock(scr, true);
            //lock.os << "New trade " << tid << '\n';
        }
        if (cur.is_zero()) {
            cur = tid;
            curpro = "";
        }
    }

    datagram.dispatcher_t create_dispatcher() {
        return new dispatcher_t(this);
    }

/*
    bool c::trade_interactive(shell_args& args, const hash_t& tid) { //return: exit parent loop
        log("trade. interactive"); //--strip
        scr << "Trading shell. Trade " << tid << '\n';
        string line = args.next_line();
        cur = tid;
        curpro = "";
        bool ret;
        while (rpc_daemon->is_active()) {
            log("wapi.exec_trade", tid, line); //--strip
            if (line == "trade" || line == "exit" || line == "q") {
                ret = true;
                break;
            }
            if (line == "wallet") {
                ret = false;
                break;
            }
            rpc_daemon->get_peer().call_exec_trade(rpc_peer_t::exec_trade_in_t(tid, line));
            scr.flush();
            { /// Set prompt
                lock_guard<mutex> lock(data.mx);
                auto i = data.find("protocol");
                if (i == data.end()) {
                    scr.prompt = "";
                }
                else {
                    ostringstream os;
                    curpro = i->second;
                    if (curpro == "not set") {
                        curpro = "";
                    }
                    us::gov::io::cfg0::trim(curpro);
                    os << tid << "-" << curpro;
                    scr.prompt = os.str();
                }
            }
            line = scr.capture_input(*rpc_daemon);
            if (!rpc_daemon->is_active()) {
                ret = false;
                break;
            }
        }
        cur = hash_t(0);
        curpro = "";
        return ret;
    }
  */


    void tx_help(params p, PrintStream os) {
        os.println("    tx new                                            Creates an empty transaction.");
        os.println("    \"  add_section <tx> <token>                       Creates a coin section in the given transaction.");
        os.println("    \"  add_input <tx> <section> <address> <amount>    Creates an input in the specified section of the given transaction.");
        os.println("    \"  add_output <tx> <section> <address> <amount>   Creates an output in the specified section of the given transaction.");
        os.println("    \"  make_sigcode exec_time <open|close> [s <section> [i <input>]* [o <output>]* ] [-]");
        os.println("    \"  decode_sigcode <sigcode_b58>");
        os.println("    \"  ts <tx> <secs in future>                            Sets a new exec-time for the given transaction");
        os.println("    \"  sign_input <tx> <section> <input> <sigcode> <secret key>    Signs the given tx input");
        os.println("    \"  make_p2pkh <dest account> <amount> <token> [<send>]");
        os.println("    \"  check <tx>");
        os.println("    \"  send <tx>");
        os.println("    \"  sign <tx> <sigcodeb58>");
        os.println("    \"  decode <evidenceB58>");
    }

    ko tx(shell_args args) {
        String command = args.next_string();
        PrintStream os = scr.os;
        if (command.equals("new")) {
            blob_t blob = new blob_t();
            ko r = rpc_daemon.get_peer().call_tx_new(blob);
            if (is_ko(r)) {
                return r;
            }
            scr.os.println(us.gov.crypto.base58.encode(blob.value));
            return ok;
        }
/*
        if (command.equals("add_section")) {
            String txb58 = args.next_string();
            if (txb58.isEmpty()) {
                return new ko("KO 54032 Missing encoded transaction.");
            }
            hash_t token=args.next_hash();
            if (token == null) {
                return new ko("KO 55043 Invalid coin.");
            }
            return wapi.tx_add_section(txb58, token);
        }
        if (command.equals("add_output")) {
            String txb58=args.next_string();
            if (txb58.isEmpty()) {
                return new ko("KO 54032 Missing encoded transaction.");
            }
            uint16_t n = args.next_uint16(null);
            if (n == null) {
                return new ko("KO 55044 Invalid section.");
            }
            hash_t addr=args.next_hash();
            if (addr == null) {
                return new ko("KO 40302 Invalid address.");
            }
            int64_t amount = args.next_cash();
            if (amount.value < 1) {
                return new ko("KO 40312 Invalid amount.");
            }
            return wapi.tx_add_output(txb58, n, addr, amount);
        }
        if (command.equals("add_input")) {
            String txb58=args.next_string();
            if (txb58.isEmpty()) {
                return new ko("KO 54032 Missing encoded transaction.");
            }
            uint16_t n=args.next_uint16(null);
            if (n == null) {
                return new ko("KO 55044 Invalid section.");
            }
            hash_t addr=args.next_hash();
            if (addr == null) {
                return new ko("KO 40302 Invalid address.");
            }
            int64_t amount=args.next_cash();
            if (amount.value<1) {
                return new ko("KO 40312 Invalid amount.");
            }
            return wapi.tx_add_input(txb58, n, addr, amount);
        }
        if (command.equals("make_sigcode")) {
            return new ko("KO 59958 not implemented.");
/ *
            gov::cash::tx::sigcode_t o;
            while (true) {
                string cmd=args.next_string();
                if (cmd.equals("exec_time")) {
                    string ts=args.next_string();
                    if (ts.equals("open")) {
                        o.ts=false;
                    }
                    else if (ts.equals("close")) {
                        o.ts=true;
                    }
                    else {
                        os.println("exec_time should be either 'open' or 'close'");
                        return false;
                    }
                }
                else if (cmd.equals("s")) {
                    int s=args.next_int(-1);
                    if (s<0) {
                        os.println("invalid section");
                        return false;
                    }
                    if (s>=o.size()) {
                        o.resize(s+1);
                    }
                    auto it=o.begin();
                    advance(it,s);
                    while(true) {
                        string cmd=args.next_string();
                        if (cmd.equals("i")) {
                            auto i=args.next_int(-1);
                            if (i<0) {
                                os.println("invalid input");
                                return false;
                            }
                            it->inputs.emplace_back(i);
                        }
                        else if (cmd.equals("o")) {
                            auto i=args.next_int(-1);
                            if (i<0) {
                                os.println("invalid output");
                                return false;
                            }
                            it->outputs.emplace_back(i);
                        }
                        else if (cmd.isEmpty()) {
                            break;
                        }
                        else if (cmd.equals("-")) {
                            break;
                        }
                        else {
                            os.println("I'm unable to parse "+cmd);
                            return false;
                        }
                    }

                }
                else if (cmd.isEmpty()) {
                    break;
                }
                else {
                    os.println("I don't understand "+cmd);
                    return false;
                }
            }
            os.println(o.to_b58());
* /
        }
        if (command.equals("decode_sigcode")) {
            return new ko("KO 59958 not implemented.");
/ *
            auto b58=args.next_string();
            auto o=gov::cash::tx::sigcode_t::from_b58(b58);
            if (o.first!=0) {
                os.println((const char*)o.first << '\n';
                return false;
            }
            o.second.dump(os);
* /
        }
        if (command.equals("ts")) {
            return new ko("KO 59958 not implemented.");
/ *
            auto txb58=args.next_string();
            auto r=gov::cash::tx::from_b58(txb58);
            if (r.first!=0) {
                os.println(r.first);
                return false;
            }
            gov::cash::tx& tx=*r.second;
            uint64_t n=args.next<uint64_t>(0);
            r.second->update_ts(n*1e9);
            os.println(r.second->to_b58());
            delete r.second;
* /
        }
        if (command.equals("sign_input")) {
            return new ko("KO 59958 not implemented.");
/ *
            auto txb58=args.next_string();
            auto r=gov::cash::tx::from_b58(txb58);
            if (r.first!=0) {
                os.println(r.first);
                return false;
            }
            gov::cash::tx& tx=*r.second;
            int n=args.next_int(-1);
            if (n<0 || n>=tx.sections.size()) {
                os.println("Error section not found\n";
                delete r.second;
                return false;
            }
            auto it=tx.sections.begin();
            advance(it,n);
            int m=args.next_int(-1);
            if (m<0 || m>=it->inputs.size()) {
                os.println("Error input not found\n";
                delete r.second;
                return false;
            }
            auto iti=it->inputs.begin();
            advance(iti,m);

            auto ssigcode=args.next_string();
            if (ssigcode.isEmpty()) {
                os.println("Invalid sigcode\n";
                return false;
            }
            auto sigcode=gov::cash::tx::sigcode_t::from_b58(ssigcode);
            if (sigcode.first!=0) {
                os.println((const char*)sigcode.first << '\n';
                return false;
            }

            auto privk=args.next<gov::crypto::ec::keys::priv_t>();
            if (privk.is_zero()) {
                os.println("invalid private key\n";
                return false;
            }

            iti->locking_program_input=gov::cash::p2pkh::create_input(tx,sigcode.second, privk);
            if (!gov::cash::p2pkh::check_input(iti->address,tx,iti->locking_program_input)) {
                os.println("could not create signature");
                return false;
            }

            os.println(tx);
            delete r.second;
* /
        }
        if (command.equals("set_supply")) {
            hash_t addr=args.next_hash();
            if (addr == null) {
                return new ko("KO 40302 Invalid address.");
            }
            int64_t amount=args.next_int64(null);
            if (amount == null) {
                return new ko("KO 40312 Invalid amount.");
            }
            wapi.set_supply(addr, amount);
        }
        if (command.equals("transfer")) {
            hash_t a = args.next_hash();
            int64_t b = args.next_cash(new int64_t(0L));
            hash_t c = args.next_token();
            return wapi.transfer(a, b, c);
        }
        if (command.equals("transfer_from")) {
            hash_t s = args.next_hash();
            hash_t a = args.next_hash();
            int64_t b = args.next_cash(new int64_t(0L));
            hash_t c = args.next_token();
            return wapi.transfer_from(s, a, b, c);
        }
        if (command.equals("invoice")) {
            return new ko("KO 59958 not implemented.");
/ *
            if (args.args_left()==0) {
                help_2steptx(os);
            }
            else {
                byte[] recv_addr=args.next_hash();
                long recv_amount=args.next_cash();
                byte[] recv_token=args.next_token();
                String option=args.next_string("");
                long send_amount=0;
                byte[] send_token=null;
                bool go=true;
                if (option.equals("-reward")) {
                    send_amount=args.next_cash(0L);
                    send_token=args.next_token();
                    if (send_amount==0) {
                        os.println("KO 9161 - reward amount.");
                        go=false;
                    }
                }
                else {
                    if (!option.isEmpty()) {
                        os.println("KO 9162 - invalid option "+option);
                        go=false;
                    }
                }
                if (go) {
                    wapi.tx_charge_pay(recv_addr,recv_amount,recv_token,send_amount,send_token,os);
                }
            }
* /
        }
        if (command.equals("pay")) {
            return new ko("KO 59958 not implemented.");
/ *
            if (args.args_left()==0) {
                help_2steptx(os);
            }
            else {
                String tx=args.next_string();
                wapi.tx_pay(tx,os);
            }
* /
        }
        if (command.equals("make_p2pkh")) {
            return new ko("KO 59958 not implemented.");
/ *
            wallet_api.tx_make_p2pkh_input i;
            i.rcpt_addr=args.next_hash();
            i.amount=args.next_cash();
            i.sendover=args.next_string("nopes").equals("send");
            wapi.tx_make_p2pkh(i,os);
* /
        }
        if (command.equals("sign")) {
            String b58=args.next_string();
            String sigcodeb58=args.next_string();
            return wapi.tx_sign(b58, sigcodeb58);
        }
        if (command.equals("decode")) {
            return new ko("KO 59958 not implemented.");
/ *
            String txb58=args.next_string();
            auto ev=gov::engine::evidence::from_b58(txb58);
            if (unlikely(ev==0)) {
                os.println("Cannot decode evidence.");
                return false;
            }
            ev.write_pretty(os);
            os.println("tx hash "+ev->compute_hash());
            delete ev;
* /
        }
        if (command.equals("check")) {
            String b58 = args.next_string();
            return wapi.tx_check(b58);
        }
        if (command.equals("send")) {
            String b58 = args.next_string();
            return wapi.tx_send(b58);
        }
*/
        tx_help(p, os);
        return KO_11000;
    }

    static ArrayList<String> fit(String s, int w, boolean left) {
        ArrayList<String> lines = new ArrayList<String>();
        String x = s;
        while (true) {
            if (x.isEmpty()) break;
            String s1 = x.substring(0, w > x.length() ? x.length() : w);
            String s2 = x.length() > w ? x.substring(w) : "";
            lines.add(s1);
            x = s2;
        }
        ArrayList<String> lines2 = new ArrayList<String>();
        if (left) {
            for (String i: lines) {
                lines2.add(i + new String(new char[w - i.length()]).replace('\0', ' '));
            }
        }
        else {
            for (String i: lines) {
                lines2.add(new String(new char[w - i.length()]).replace('\0', ' ') + i);
            }
        }
        return lines2;
    }

    static final int w = 80;

    static void twocol(String prefix, String l, String r, String r2, PrintStream os) {
        ArrayList<String> l1 = fit(l, w, true);
        ArrayList<String> l2 = fit(r + '.' + r2, w, true);
        int l1i = 0;
        int l2i = 0;
        while (true) {
            if (l2i == l2.size() && l1i == l1.size()) break;
            if (l1i == 0) {
                os.print(prefix);
            }
            else {
                os.print(new String(new char[prefix.length()]).replace('\0', ' '));
            }
            if (l1i < l1.size()) {
                 if (l1i != 0) os.print("    ");
                 os.print(l1.get(l1i));
                 ++l1i;
            }
            else {
                os.print(new String(new char[w]).replace('\0', ' '));
            }
            os.print("  ");
            if (l2i < l2.size()) {
                 if (l2i != 0) os.print("    ");
                 os.print(l2.get(l2i));
                 ++l2i;
            }
            os.println();
        }
    }

    static void twocol(String prefix, String l, String r, PrintStream os) {
        twocol(prefix, l, r, "", os);
    }

    static String tostr(String o) {
        return " [" + o + "]";
    }

    static String tostr(int o) {
        return " [" + o + "]";
    }

    static String tostr(boolean o) {
        return " [" + (o ? 1 : 0) + "]";
    }

    public static void tx_help(String ind, final params p, PrintStream os) {
        twocol(ind, "tx new", "Creates an empty transaction", os);
        twocol(ind, "tx add_section <tx> <token>", "Creates a coin section in the given transaction", os);
        twocol(ind, "tx add_input <tx> <section> <address> <amount>", "Creates an input in the specified section of the given transaction", os);
        twocol(ind, "tx add_output <tx> <section> <address> <amount>", "Creates an output in the specified section of the given transaction", os);
        twocol(ind, "tx make_sigcode exec_time <open|close> [s <section> [i <input>]* [o <output>]* ] [-]", "", os);
        twocol(ind, "tx decode_sigcode <sigcode_b58>", "", os);
        twocol(ind, "tx ts <tx> <secs in future>", "Sets a new exec-time for the given transaction", os);
        twocol(ind, "tx sign_input <tx> <section> <input> <sigcode> <secret key>", "Signs the given tx input", os);
        twocol(ind, "tx check <tx>", "", os);
        twocol(ind, "tx send <tx>", "", os);
        twocol(ind, "tx sign <tx> <sigcodeb58>", "", os);
        twocol(ind, "tx decode <evidenceB58>", "", os);
    }

    public static void help_2steptx(String ind, PrintStream os) {
        twocol(ind, "invoice <recv account> <amount> <gas|coin> [-reward <award amount> <gas|coin>]", "Creates an invoice (incomplete transaction)", os);
        twocol(ind, "pay <tx>", "Pay invoice. Complete the transaction and send it over", os);
    }

    public static void help(params p, PrintStream os) {
        if (p.banner) {
            banner(p, os);
        }
        os.println("Usage: " + us.CFG.PLATFORM + "-wallet [options] [command]");
        os.println("Options:");
        String ind = "    ";
        String ind____ = ind + "    ";
        twocol(ind, "General parameters:", "", os);
        if (p.advanced) {
            twocol(ind____, "-b", "Basic mode", os);
        }
        if (p.advanced) {
            twocol(ind____, "-home <homedir>", "homedir", tostr(p.get_home()), os);
            //twocol(ind____, "-d", "Runs daemon (backend) instead of an rpc-client.", tostr(p.listening_port.value), os);
            //twocol(ind____, "-e <edges>", "Max num walletd neightbours", tostr(p.max_trade_edges.value), os);
            //twocol(ind____, "-v <edges>", "Max num device neightbours", tostr(p.max_devices.value), os);
            //twocol(ind____, "-w <workers>", "Threadpool size", tostr(p.workers.value), os);
            //twocol(ind____, "-lp <port>", "Listening Port. (-d)", tostr(p.listening_port.value), os);
            //twocol(ind____, "-pp <port>", "Published listening port.", tostr(p.published_port.value), os);
            twocol(ind____, "-pin <PIN number>", "Connect using PIN.", tostr(p.pin.value), os);
            twocol(ind____, "-c <channel>", "System channel", tostr(p.channel.value), os);
            //if (us.CFG.logs.b)
            //    twocol(ind____, "-log", "Logs in screen.", os);
            //    twocol(ind____, "-logd <dir>", "Directory for writting log files.", os);
            //    twocol(ind____, "-nolog ", "Disable logs.", os);
            //#endif
            //twocol(ind____, "-om <output_mode>", "0:human, 1:text, 2:xml, 3:json", tostr(p.get_output_mode()), os);
            //twocol(ind____, "-dd <downloads_dir>", "Directory containing software updates blobs", tostr(p.downloads_dir), os);
            twocol(ind____, "-n", "Omit field names in output", tostr(p.names), os);
            twocol(ind____, "-nb", "Don't show the banner.", os);
            //twocol(ind, "RPC to backend(" + us.CFG.PLATFORM + "-gov) parameters:", "", os);
            //twocol(ind____, "-bhost <address>", "us-gov IP address" ,tostr(p.backend_host.value), os);
            //twocol(ind____, "-bp <port>", "TCP port", tostr(p.backend_port.value), os);
            twocol(ind, "RPC to wallet-daemon (" + us.CFG.PLATFORM + "-wallet) parameters:", "", os);
            twocol(ind____, "-whost <address>", "walletd address", tostr(p.walletd_host.value), os);
            twocol(ind____, "-wp <port>", "walletd port", tostr(p.walletd_port.value), os);
            twocol(ind____, "--custodial <string>", "(rpc) Select custodial wallet, or '-' for non-custodial.", p.subhome.isEmpty() ? "'-'" : p.subhome, os);
        }
        os.println();
        os.println("Commands:");
        twocol(ind, "misc:", "----------", os);
        twocol(ind____, "version", "Print software version.", os);
        os.println();
        twocol(ind, "cash: Query", "----------", os);
        twocol(ind____, "balance <0|1|2>", "If detail is 0 it displays a collapsed view of accounts. 1 or 2 shows the breakdown by account with the given level of detail", os);
        twocol(ind____, "recv_address", "Shows a suitable address for receiving transfers", os);
        os.println();
        twocol(ind, "cash: Functions for transfering to <dest account>", "----------", os);
        twocol(ind____, "transfer <dest account> <amount> <gas|coin>", "Source account/s will be choosen automatically", os);
        twocol(ind____, "transfer_from <source account> <dest account> <amount> <gas|coin>", "Selectable source account", os);
        os.println();
        twocol(ind, "cash: Two-step transactions", "----------", os);
        help_2steptx(ind____, os);
        if (p.advanced) {
            os.println();
            twocol(ind, "cash: Coin/token creation", "----------", os);
            twocol(ind____, "set_supply <address> <amount>", "Set or reset the coin mint balance", os);
            os.println();
            twocol(ind, "Transaction making", "----------", os);
            tx_help(ind____, p, os);
        }
        os.println();
        twocol(ind, "keys:", "----------", os);
        twocol(ind____, "address new", "Generates a new key-pair, adds the private key to the wallet and prints its asociated address", os);
        if (p.advanced) {
            twocol(ind____, "address add <privkey>", "Imports a given private key in the wallet", os);
        }
        twocol(ind____, "list [show_priv_keys=0|1]", "Lists all keys. Default value for show_priv_keys is 0", os);
        if (p.advanced) {
        twocol(ind____, "gen_keys", "Generates a key pair without adding them to the wallet.", os);
        twocol(ind____, "mine_public_key <string>", "Creates a vanity address containing the specified string", os);
        twocol(ind____, "priv_key <private key>", "Gives information about the given private key", os);
        twocol(ind____, "pub_key <public key>", "Gives information about the given public key", os);
        twocol(ind____, "digest <filename>", "Computes RIPEMD160+base58 to the content of the file.", os);
        twocol(ind____, "sign -f <filename> <private key>", "Sign file.", os);
        twocol(ind____, "sign \"<message>\" <private key>", "Sign message", os);
        twocol(ind____, "verify -f <filename> <pubkey> <signature>", "Verify signed file.", os);
        twocol(ind____, "verify \"<message>\" <pubkey> <signature>", "Verify signed message", os);
        twocol(ind____, "encrypt -f <filename> <sender_private_key> <recipient_public_key>", "Encrypts file.", os);
        twocol(ind____, "encrypt \"<message>\" <sender_private_key> <recipient_public_key>", "Encrypts message", os);
        twocol(ind____, "decrypt \"<b58>\" <sender_public_key> <recipient_private_key>", "Decrypts message", os);
        }
        os.println();
        if (p.advanced) {
            twocol(ind, "Device pairing:", "----------", os);
            twocol(ind____, "device_id", "Show this device Id.", os);
            twocol(ind____, "pair <pubkey> [<subhome>|-] [<name>]", "Authorize a device identified by its public key", os);
            twocol(ind____, "unpair <pubkey>", "Revoke authorization to the specified device", os);
            twocol(ind____, "prepair <pin|auto> [<subhome>|-] [<name>]", "Pre-Authorize an unknown device identified by a pin number [1-65535]", os);
            twocol(ind____, "unprepair <pin>", "Revoke Pre-authorization to the specified pin", os);
            twocol(ind____, "list_devices", "Prints the list of recognized devices, together with the list of recently unathorized attempts log", os);
            twocol(ind____, "net_info", "Prints contextual information and wallet type (root/guest)", os);
            os.println();
            twocol(ind, "Storage:", "----------", os);
            os.println(ind + "  key-value:");
            twocol(ind____, "store <address> -kv <key> <value>", "Store key-value pair in a funded account. cost: 1 per cycle", os);
            twocol(ind____, "rm <address> -kv <key>", "Removes the key-value pair", os);
            twocol(ind____, "search <address> <word1|word2...>", "Search key-value table", os);
            os.println(ind + "  file:");
            twocol(ind____, "store <address> -f <file> [path]", "Store file in a funded account. cost: 1 per kibibyte per cycle", os);
            twocol(ind____, "rm <address> -f <hash> <path>", "Removes a file. <path> example: /", os);
            twocol(ind____, "file <hash>", "Retrieves file content", os);
            twocol(ind____, "list <address> <path>", "List files starting with path", os);
            os.println();
            twocol(ind, "Private storage: (in-wallet)", "----------", os);
            twocol(ind____, "timeseries list", "List accounts holding timeseries.", os);
            twocol(ind____, "timeseries <address> new", "Creates a new time series in the given address.", os);
            twocol(ind____, "timeseries <address> add [-f <filename>|-b58 <b58>|\"text line\"]", "Appends a new entry.", os);
            twocol(ind____, "timeseries <address> show [<timemark>]", "List all events registered. Or obtain its content if a timemark is given.", os);
            os.println();
            if (us.CFG.NETWORK_TOPOLOGY.PERMISSIONED) {
                twocol(ind, "Permissions:", "----------", os);
                os.println(ind + "  Nodes:");
                twocol(ind____, "nodes allow <address> <node-master key>", "Accept node", os);
                twocol(ind____, "nodes deny <address> <node-master key>", "Reject node", os);
                os.println();
            }
            twocol(ind, "Law/Compilance:", "----------", os);
            twocol(ind____, "compilance_report <jurisdiction> <date-from> <date-to>", "Produces a private report including personal, financial and ownership data that voluntarily could be submitted to regulators", os);
            twocol(ind, "Trader:", "----------", os);
            twocol(ind, "trade <command>", "Access to trading functions", os);
            //us.wallet.trader.traders_t.help(ind____, os);
            os.println();
            twocol(ind, "Daemon control/monitoring:", "----------", os);
            twocol(ind____, "s", "Show socket connections", os);
            twocol(ind____, "gw", "Show neighbours", os);
            twocol(ind____, "allw", "Show all wallets", os);
            twocol(ind____, "regw <ip address>", "Register this wallet reachable at the specified address.", os);
            twocol(ind____, "sync", "Reload files", os);
            twocol(ind____, "reload_doc <full-path-doc>", "Invoke reload on all active trades in all wallets. Only via root wallet", os);
            os.println();
            twocol(ind, "Net-dev control:", "----------", os);
            twocol(ind____, "patch_os <script file> <skey>", "System upgrade/maintenance. Requires governance key", os);
            os.println(ind____ + "(Account for updates: " + CFG.AUTOMATIC_UPDATES_ACCOUNT + ")");
            os.println();
            twocol(ind, "Connectivity:", "----------", os);
            twocol(ind____, "ping", "Endpoint check", os);
            os.println();
            twocol(ind, "Software:", "----------", os);
            twocol(ind____, "version", "Print version.", os);
            twocol(ind____, "-h | h | help", "Print this help", os);
            os.println();
        }
        os.println();
    }

    void trade(boolean interactive, shell_args args) { //moved
        System.err.println("KO 30303");
        System.exit(1);
    }

    public String rewrite(ko r) {
        log("rewrite"); //--strip
        if (r == KO_11000) {
            log("rewrite returns help hint."); //--strip
            return r.msg + "; Type -h for help.";
        }
        if (rpc_daemon != null) return rpc_daemon.rewrite(r);
        log("rewriteX"); //--strip
        if (r != null) return r.msg;
        log("rewrite returns empty. AxzS."); //--strip
        return "";
    }

    String run_() {
        log("run cmd=" + p.cmd); //--strip
        {
            if (!p.cmd.isEmpty()) {
                ko r = exec_offline(p.cmd, p.args);
                if (r != KO_91810) {
                    return rewrite(r);
                }
            }
        }
        {
            ko r = start();
            if (ko.is_ko(r)) {
                return r.msg;
            }
        }
        String ret = "";
        if (!p.daemon) { //local or RPC to walletd
            if (p.cmd.isEmpty()) {
                interactive_shell();
            }
            else {
                ko r = exec_online(p.cmd, p.args);
                if (is_ko(r)) {
                    ret = rewrite(r);
                    assert ret != null;
                }
            }
            stop();
        }
        join();
        return ret;
    }

    public String run() {
        String r = run_();
        assert r != null;
        if (!r.isEmpty()) {
            scr.lock.lock();
            scr.os.println(r);
            scr.lock.unlock();
        }
        return r;
    }

    ko exec_online1(String cmd, shell_args args) {
        log("exec " + cmd); //--strip
        PrintStream os = scr.os;
        String command = cmd;
        if (command.equals("transfer") || command.equals("invoice") || command.equals("pay") || command.equals("transfer_from") || command.equals("set_supply")) {
            command = "tx";
            --args.n;
        }
        if (command.equals("tx")) {
            return tx(args);
        }
        if (command.equals("priv_key")) {
            PrivateKey priv = args.next_priv();
            rpc_peer_t.priv_key_out_dst_t o_out = new rpc_peer_t.priv_key_out_dst_t();
            ko r = rpc_daemon.get_peer().call_priv_key(new priv_t(priv), o_out);
            if (is_ko(r)) {
                return r;
            }
            scr.lock.lock();
            scr.os.println("pub " + o_out.pub);
            scr.os.println("addr " + o_out.addr);
            scr.lock.unlock();
            return ok;
        }
/*
        if (command.equals("pub_key")) {
            PublicKey pub = args.next_pub();
            return wapi.pub_key(pub);
        }
        if (command.equals("address")) {
            command = args.next_string();
            if (command.equals("new")) {
                return wapi.new_address();
            }
            if (command.equals("add")) {
                PrivateKey k = args.next_priv();
                return wapi.add_address(k);
            }
            return KO_11000;
        }
        if (command.equals("list")) {
            return wapi.list(args.next_uint16(new uint16_t(0)));
        }
*/
        if (command.equals("balance")) {
            uint16_t detailed = args.next_uint16(new uint16_t(0));
            string data = new string();
            ko r = rpc_daemon.get_peer().call_balance(detailed, data);
            if (is_ko(r)) {
                return r;
            }
            scr.lock.lock();
            scr.os.println(data.value);
            scr.lock.unlock();
            return ok;
        }
        if (command.equals("unpair")) {
            PublicKey pub = args.next_pub();
            pub_t pk = new pub_t(pub);
            string ans = new string();
            ko r = rpc_daemon.get_peer().call_unpair_device(pk, ans);
            if (is_ko(r)) {
                return r;
            }
            scr.lock.lock();
            scr.os.println(ans.value);
            scr.lock.unlock();
            return ok;
        }
/*
        if (command.equals("stress")) {
            int i = 0;
            while(dapi.get_api().is_active()) {
                scr.os.println("call #"+(i++));
                wapi.balance(new uint16_t(0));
            }
            return ok;
        }
        if (command.equals("gen_keys")) {
            return wapi.gen_keys();
        }
        else if (command.equals("mine_public_key")) {
            if (!p.local) return KO_10020;
            return KO_40322;
        }
        if (command.equals("digest")) {
            if (!p.local) return KO_10020;
            return KO_40322;
        }
        if (command.equals("sign")) {
            String msg = args.next_string();
            boolean file=false;
            if (msg.equals("-f")) {
                msg = args.next_string();
                file = true;
            }
            PrivateKey privkey = args.next_priv();
            if (file) {
                return wapi.sign_file(msg, privkey);
            }
            byte[] v = msg.getBytes();
            return wapi.sign(v, privkey);
        }
        if (command.equals("verify")) {
            String msg = args.next_string();
            boolean file = false;
            if (msg.equals("-f")) {
                msg = args.next_string();
                file = true;
            }
            PublicKey pubkey = args.next_pub();
            String sig = args.next_string();
            if (file) {
                return wapi.verify_file(msg, pubkey, base58.decode(sig));
            }
            byte[] v = msg.getBytes();
            return wapi.verify(v, pubkey, base58.decode(sig));
        }
        if (command.equals("encrypt")) {
            String msg = args.next_string();
            boolean file = false;
            if (msg.equals("-f")) {
                msg = args.next_string();
                file = true;
            }
            PrivateKey sender_privkey = args.next_priv();
            PublicKey recipient_pubkey = args.next_pub();
            if (file) {
                return wapi.encrypt_file(msg, sender_privkey, recipient_pubkey);
            }
            byte[] v = msg.getBytes();
            return wapi.encrypt(v, sender_privkey, recipient_pubkey);
        }
        if (command.equals("decrypt")) {
            String msg = args.next_string();
            boolean file = false;
            if (msg.equals("-f")) {
                msg = args.next_string();
                file = true;
            }
            PublicKey sender_pubkey = args.next_pub();
            PrivateKey recipient_privkey = args.next_priv();
            if (file) {
                return wapi.decrypt_file(msg, sender_pubkey, recipient_privkey);
            }
            return wapi.decrypt(base58.decode(msg), sender_pubkey, recipient_privkey);
        }
        if (command.equals("patch_os")) {
            String file = args.next_string();
            PrivateKey privkey = args.next_priv();
            return wapi.patch_os_file(file, privkey);
        }
        if (command.equals("component")) {
            if (!p.local) return KO_10020;
            return KO_40322;
        }
        if (command.equals("harvest")) {
            uint16_t id = args.next_uint16();
            uint16_t timeout = args.next_uint16();
            uint16_t decay = args.next_uint16();
            PrivateKey privkey = args.next_priv();
            return dapi.harvest(id, timeout, decay, privkey);
        }
        if (command.equals("store")) {
            log("store"); //--strip
            hash_t addr = args.next_hash();
            String opt = args.next_string();
            if (opt.equals("-kv")) {
                String key = args.next_string();
                String value = args.next_string();
                return wapi.store_kv(addr, key, value);
            }
            if (opt.equals("-f")) {
                String file = args.next_string();
                String path = args.next_string();
                path = cfg0.rewrite_path(path);
                byte[] content = cfg0.read_file(file);
                if (content == null) {
                    return new ko("KO 39928 Invalid file");
                }
                return wapi.store_file(addr, path, content);
            }
            return KO_11000;
        }
        if (command.equals("rm")) {
            hash_t addr = args.next_hash();
            String opt = args.next_string();
            if (opt.equals("-kv")) {
                String key = args.next_string();
                return wapi.rm_kv(addr, key);
            }
            if (opt.equals("-f")) {
                hash_t file_hash = args.next_hash();
                String path = args.next_string();
                return wapi.rm_file(addr, path, file_hash);
            }
            return KO_11000;
        }
        if (command.equals("file")) {
            hash_t file_hash = args.next_hash();
            return wapi.file(file_hash);
        }
        if (command.equals("search")) {
            hash_t addr = args.next_hash();
            String crit = args.next_string();
            return wapi.search(addr, crit);
        }
*/
        if (command.equals("timeseries")) {
            String subcmd = args.next_string();
            if (subcmd.equals("list")) {
                string o_out = new string();
                ko r = rpc_daemon.get_peer().call_timeseries_list(o_out);
                if (is_ko(r)) {
                    return r;
                }
                scr.lock.lock();
                scr.os.println(o_out.value);
                scr.lock.unlock();
                return ok;
            }
            hash_t addr = new hash_t(subcmd);
            if (addr.is_zero()) {
                ko r = new ko("KO 69302 Invalid command or address. Valid commands are: list.");
                log(r.msg); //--strip
                return r;
            }


            subcmd = args.next_string();
            if (subcmd.equals("show")) {
                string o_out = new string();
                ts_t ts = new ts_t(args.next_uint64().value);
                ko r = rpc_daemon.get_peer().call_timeseries_show(new rpc_peer_t.timeseries_show_in_t(addr, ts), o_out);
                if (is_ko(r)) {
                    return r;
                }
                scr.lock.lock();
                scr.os.println(o_out.value);
                scr.lock.unlock();
                return ok;
            }
            if (subcmd.equals("new")) {
                string o_out = new string();
                ko r = rpc_daemon.get_peer().call_timeseries_new(new rpc_peer_t.timeseries_new_in_t(addr, new uint8_t((short)1)), o_out);
                if (is_ko(r)) {
                    return r;
                }
                scr.lock.lock();
                scr.os.println(o_out.value);
                scr.lock.unlock();
                return ok;
            }
            if (subcmd.equals("add")) {
                String s = args.next_string();
                bin_t v;
                if (s.equals("-f")) {
                    String fname = args.next_string();
                    v = new bin_t();
                    ko r = us.gov.io.cfg0.read_file_(fname, v);
                    if (is_ko(r)) {
                        return r;
                    }
                }
                else if (s.equals("-b58")) {
                    String b58 = args.next_string();
                    v = new bin_t(us.gov.crypto.base58.decode(b58));
                }
                else {
                    v = new bin_t(s.getBytes());
                }
                       
                if (v.value == null || v.value.length == 0) {
                    ko r = new ko("KO 76037 Empty content.");
                    log(r.msg); //--strip
                    return r;
                }
                string o_out = new string();
                {
                    ko r = rpc_daemon.get_peer().call_timeseries_add(new rpc_peer_t.timeseries_add_in_t(addr, v), o_out);
                    if (is_ko(r)) {
                        return r;
                    }
                }
                scr.lock.lock();
                scr.os.println(o_out.value);
                scr.lock.unlock();
                return ok;
            }
            ko r = new ko("KO 52109 Valid commands are: show|new|add.");
            log(r.msg); //--strip
            return r;
        }
/*
        if (CFG.NETWORK_TOPOLOGY.PERMISSIONED) {
            if (command.equals("nodes")) {
/ *
                String verb=args.next_string();
                hash_t addr=args.next_hash();
                PrivateKey sk=args.next_priv();
                if (verb.equals("allow")) {
                    return wapi.nodes_allow(addr, sk);
                }
                if (verb.equals("deny")) {
                    return wapi.nodes_deny(addr, sk);
                }
                return KO_11000;
* /
                return KO_40322;
            }
        }
        if (command.equals("trade")) {
            if (p.local) return KO_10021;
            if (interactive) {
                scr.os.println("Entering trading shell...");
            }
            trade(interactive, args);
            if (dapi.get_api().is_active()) {
                if (interactive) {
                    scr.os.println("Back to wallet shell. type h for help.");
                }
            }
            return ok;
        }
        if (command.equals("gw")) {
            if (p.local) return KO_10021;
            return dapi.print_grid();
        }
        if (command.equals("allw")) {
            return dapi.allw();
        }
        if (command.equals("sync")) {
            return dapi.sync();
        }
        if (command.equals("reload_doc")) {
            String t = args.next_string();
            return dapi.reload_file(t);
        }
        if (command.equals("pair")) {
            PublicKey pub = args.next_pub();
            if (pub == null) {
                return new ko("KO 24019 Invalid public key.");
            }
            String subhome = args.next_string();
            String name = args.next_string();
            return dapi.pairing().pair_device(pub, subhome, name);
        }
        if (command.equals("unpair")) {
            PublicKey pub = args.next_pub();
            return dapi.pairing().unpair_device(pub);
        }
        if (command.equals("prepair")) {
            String spin = args.next_string();
            uint16_t pin = new uint16_t(0);
            if (spin.equals("auto")) {
                pin.value = 65535;
            }
            else {
                try {
                    pin = new uint16_t(Integer.parseInt(spin));
                }
                catch (Exception e) {
                    return new ko("KO 10241 Invalid PIN.");
                }
            }
            if (pin.value == 0) {
                return new ko("KO 10242 Invalid PIN.");
            }
            String subhome = args.next_string();
            String name = args.next_string();
            return dapi.pairing().prepair_device(pin, subhome, name);
        }
        if (command.equals("unprepair")) {
            uint16_t pin = args.next_uint16(new uint16_t(0));
            return dapi.pairing().unprepair_device(pin);
        }
        if (command.equals("get_subhome")) {
            return wapi.get_subhome();
        }
        if (command.equals("list_devices")) {
            return dapi.pairing().list_devices();
        }
        if (command.equals("ping")) {
            return dapi.ping();
        }
        if (command.equals("compilance_report")) {
            return KO_40322;
        }
        if(command.equals("h")) {
            help(p, scr.os);
            return ok;
        }
*/
        scr.lock.lock();
        scr.os.println("WARNING: Many functions in the java HMI has recently been disabled after an API upgrade. For the complete functional rpc-wallet use the c++ version.\n Functions currently working: priv_key, balance, unpair ");
        scr.lock.unlock();

        return KO_11000;
    }

    ko exec_online(String cmd, shell_args args) {
        if (cmd.isEmpty()) {
            return new ko("KO 11000 Empty command.");
        }
        ko r = exec_online1(cmd, args);
        return r;
    }

    ko exec_offline(String cmd, shell_args args) {
        log("run_offline"); //--strip
        if (cmd.equals("license") || cmd.equals("licence")) {
            scr.os.println("Read the file /var/" + CFG.PLATFORM + "/licence");
            return ok;
        }
        if (cmd.equals("version")) {
            scr.os.println(us.vcs.version());
            return ok;
        }
        if (cmd.equals("device_id")) {
            pair<ko, cfg1> f = cfg1.load(p.channel, p.get_home_wallet_rpc_client(), true);
            if (ko.is_ko(f.first)) {
                return f.first;
            }
            scr.os.println(ec.instance.to_b58(f.second.keys.getPublic()));
            return ok;
        }
        if (cmd.equals("h") || cmd.equals("-h") || cmd.equals("help") || cmd.equals("--help") ) {
            help(p, scr.os);
            return ok;
        }
        if (cmd.equals("show")) {
            String subcmd = args.next_string();
            if (subcmd.equals("c") || subcmd.equals("w")) {
                String l = us.gov.io.cfg0.read_text_file(CFG.LICENSE_FILE);
                if (l == null) {
                    return new ko("KO 68759 Cannot read License file.");
                }
                scr.lock.lock();
                scr.os.println(l);
                scr.lock.unlock();
                return ok;
            }
            return new ko("KO 70693 Invalid command show");
        }
        if (cmd.isEmpty()) {
            if (interactive) {
                scr.lock.lock();
                help(p, scr.os);
                scr.lock.unlock();
                return ok;
            }
        }
        return KO_91810;
    }

    public ko exec(String cmdline) {
        shell_args args = new shell_args(cmdline);
        String cmd = args.next_string();
        ko r = exec_offline(cmd, args);
        if (r == KO_91810) {
            r = exec_online(cmd, args);
        }
        return r;
    }

    public pair<ko, cfg1> load_cfg(String home, boolean gen) {
        return cfg1.load(p.channel, home, gen);
    }

    static void banner(params p, PrintStream os) {
        String ind = "    ";
        os.println(CFG.PLATFORM + "-walletxj (rpc client). Channel " + p.channel.value + ". Copyright (C) " + CFG.COPYRIGHT_LINE);
        os.println(CFG.COPYRIGHT_LINE2);
        os.println(CFG.COPYRIGHT_LINE3);
        os.println(ind + "version: " + us.vcs.version());
        os.print(ind + "Build configuration: ");
        if (CFG.LOGS > 0) {
            os.print("[with logs] ");
        }
        else {
            os.print("[without logs] ");
        }
        if (CFG.DEBUG > 0) {
            os.print("[debug build] ");
        }
        else {
            os.print("[optimized build] ");
        }
        os.println();
        os.println(ind + "Parameters:");
        p.dump(ind + "    ", os);
        os.println("Copyright (C) " + us.CFG.COPYRIGHT_LINE);
    }

    public void interactive_shell() {
        scr.lock.lock();

        if (p.banner) {
            scr.os.println("\ndevice public key is " + us.gov.crypto.ec.instance.to_b58(cfg.keys.getPublic()) + " address " + us.gov.crypto.ec.instance.to_encoded_address(cfg.keys.getPublic()));
        }
        scr.os.println("KO 44039 Interactive shell not available.");
        scr.lock.unlock();
    }

    public static boolean test() {
        return us.gov.crypto.symmetric_encryption.test();
    }

    public KeyPair get_keys() {
        if (cfg == null) return null;
        return cfg.keys;
    }

    public boolean is_active() {
        if (rpc_daemon == null) return false;
        return rpc_daemon.is_active();
    }

    public String get_pubkey() {
        if (cfg == null) return "";
        if (cfg.keys == null) return "";
        return ec.instance.to_b58(cfg.keys.getPublic());
    }

    public ko unpair() {
        return exec("unpair " + get_pubkey());
    }

    public rpc_daemon_t rpc_daemon = null;
    public rpc_peer_t rpc_peer = null;

    String home;
    public screen scr;
    public boolean interactive = false;
    public params p;
    public cfg1 cfg = null;
    hash_t cur = new hash_t(0);
    String curpro;

    data_t data;
    roles_t roles;

    public String datasubdir = "";
}

