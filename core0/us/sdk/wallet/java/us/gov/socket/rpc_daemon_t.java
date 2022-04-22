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
package us.gov.socket;
import us.CFG;                                                                                 // CFG
import java.util.Date;                                                                         // Date
import static us.gov.socket.datagram.dispatcher_t;                                             // dispatcher_t
import us.gov.socket.single.handler_daemon_t;                                                  // handler_daemon_t
import static us.gov.socket.types.*;                                                           // *
import java.io.IOException;                                                                    // IOException
import static us.ko.is_ko;                                                                     // is_ko
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import us.gov.socket.outbound.rendezvous_t;

public abstract class rpc_daemon_t extends daemon0_t {

    static void log(final String line) {                               //--strip
        CFG.log_gov_socket("rpc_daemon_t: " + line);                   //--strip
    }                                                                  //--strip

    public rpc_daemon_t(channel_t channel, dispatcher_t dispatcher) {
        super(channel);
        handler = new handler_daemon_t(this, dispatcher);
    }

    public boolean sendq_active() {
        return is_active();
    }

    @Override public ko send1(client peer, datagram d) {
        log("send1"); //--strip
        ko r = wait_recv_connection(true);
        if (ko.is_ko(r)) {
            return r;
        }
        return super.send1(peer, d);
    }

    public void on_destroy_(client peer) {}
    public abstract ko connect();

    public ko wait_recv_connection(boolean initiate) {
        return handler.wait_connection(initiate);
    }

    public client create_client() {
        return new peer_t(this);
    }

    @Override public final boolean process_unhandled(client peer, datagram d) {
        log("process_unhandled " + d.service.value); //--strip
        if (super.process_unhandled(peer, d)) {
            return true;
        }
        return handler.process_unhandled(peer, d);
    }

    public void set_busy_handlers(busyled_t.handler_t hsend, busyled_t.handler_t hrecv) {
        log("set_busy_handlers"); //--strip
        handler.set_busy_handler(hrecv);
        super.set_busy_handler(hsend);
    }

    @Override public void dump(final String prefix, PrintStream os) {
        os.println(prefix + "socket::rpc_daemon_t:");
        super.dump(prefix, os);
    }

    @Override public final busyled_t recv_led() {
        return handler.busyled;
    }

    public void wakeup_handler() {
        handler.task_wakeup();
    }

    public void wakeup_caller() {
        super.task_wakeup();
    }

    public String rewrite(ko r) {
        if (r == rendezvous_t.KO_20190) {
            String x = peer_t.lasterror.get();
            assert x != null;
            return x;
        }
        return super.rewrite(r);
    }

    public ko start() {
        log("start"); //--strip
        assert peer == null;
        client o = create_client();
        attach(o);
        {
            ko r = super.start();
            if (ko.is_ko(r)) {
                return r;
            }
            log("started thread: send"); //--strip
        }
        {
            ko r = handler.start();
            if (ko.is_ko(r)) {
                super.stop();
                log("stopped thread: send"); //--strip
                return r;
            }
            log("started thread: recv"); //--strip
        }
        return ok;
    }

    public ko wait_ready(Date deadline) {
        ko r = handler.wait_ready(deadline);
        if (ko.is_ko(r)) {
            return r;
        }
        return super.wait_ready(deadline);
    }

    public ko wait_ready(int seconds_deadline) {
        return wait_ready(new Date(System.currentTimeMillis() + seconds_deadline * 1000L));
    }

    public void stop() {
        log("stop"); //--strip
        if (peer != null) {
            if (!peer.is_finished()) {
                peer.disconnect(new seq_t(0), new reason_t("rpc_api service stopped"));
            }
        }
        log("stopping recv"); //--strip
        handler.stop();
        log("stopping send"); //--strip
        super.stop();
        handler.signal_connected();

        log("flushing rendezvous"); //--strip
        rendezvous.flush();
    }

    public void join() {
        log("waiting for thread: recv"); //--strip
        handler.join();
        log("waiting for thread: send"); //--strip
        super.join();
        peer = null;
    }

    @Override public void on_stop() {
        //if one stops stop the other
        if (super.is_active()) super.stop();
        if (handler.is_active()) handler.stop();
    }

    @Override public void attach(client peer_) {
        log("attach " + peer_.endpoint()); //--strip
        assert peer == null;
        peer = (peer_t) peer_;
    }

    @Override public void detach(client peer_) {
        log("detach " + peer_.endpoint()); //--strip
        assert peer_ == peer;
        peer.set_finish(); //follows client::on_destroy_ called from server thread - update clients - before going to gc
        if (peer.sock != null) {
            log("shutdown recv channel"); //--strip
            try {
                log("sock.close()");  //--strip
                peer.sock.close();
            }
            catch (IOException e) {
                log("Exception closing socket. " + e.getMessage());  //--strip
            }
        }
        on_destroy_(peer);
        if (handler.stop_on_disconnection) {
            log("stop_on_disconnection is true"); //--strip
            stop();
        }
    }

    public peer_t peer = null;

    public handler_daemon_t handler;

}

