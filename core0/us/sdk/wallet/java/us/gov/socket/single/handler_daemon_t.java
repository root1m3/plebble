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
package us.gov.socket.single;
import us.gov.bgtask;                                                                          // bgtask
import us.gov.socket.busyled_t;                                                                // busyled_t
import us.CFG;                                                                                 // CFG
import us.gov.socket.client;                                                                   // client
import java.util.concurrent.locks.Condition;                                                   // Condition
import us.gov.socket.datagram;                                                                 // datagram
import java.util.Date;                                                                         // Date
import static us.gov.socket.datagram.dispatcher_t;                                             // dispatcher_t
import static us.gov.socket.types.*;                                                           // *
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import us.gov.socket.peer_t;                                                                   // peer_t
import java.io.PrintStream;                                                                    // PrintStream
import java.util.concurrent.locks.ReentrantLock;                                               // ReentrantLock
import us.gov.socket.rpc_daemon_t;                                                             // rpc_daemon_t

public class handler_daemon_t extends bgtask implements bgtask.callback {

    public static ko KO_4093 = new ko("KO 4093 wrong service.");
    public static ko KO_40032 = new ko("KO 40032 Not connected.");

    static void log(final String line) {                                   //--strip
        CFG.log_gov_socket("handler_daemon_t: " + line);                   //--strip
    }                                                                      //--strip

    public handler_daemon_t(rpc_daemon_t daemon_, dispatcher_t dispatcher_) {
        super();
        set_callback(this);
        daemon = daemon_;
        dispatcher = dispatcher_;
    }

    public ko connect() {
        return daemon.connect();
    }

    public void stop() {
        super.stop();
        log("stopping thread: recv"); //--strip
        signal_recv();
        signal_connected();
        log("stopping"); //--strip
    }

    public void signal_connected() {
        lock_connected.lock();
        try {
            cv_connected.signalAll();
        }
        finally {
            lock_connected.unlock();
        }
    }

    public void signal_recv() {
        lock_recv.lock();
        try {
            cv_recv.signalAll();
        }
        finally {
            lock_recv.unlock();
        }
    }

    String dbg_durprint(Date fut) {    //--strip
        long nowms = System.currentTimeMillis(); //--strip
        return "" + (fut.getTime() - nowms) + " ms"; //--strip
    } //--strip

    void wait(final Date deadline) {
        Date now = new Date();
        if (now.compareTo(deadline) >= 0) {
            return;
        }
        log("throttle control: waiting for " + dbg_durprint(deadline) + " before trying again..."); //--strip
        lock_connected.lock();
        try {
            while(new Date().compareTo(deadline) < 0) {
                cv_connected.awaitUntil(deadline);
            }
        }
        catch(InterruptedException e) {
        }
        finally {
            lock_connected.unlock();
        }
    }

    public peer_t get_peer() {
        return daemon.peer;
    }

    public boolean is_connected() {
        if (!is_active()) return false;
        if (get_peer() == null) return false;
        return get_peer().connected();
    }

    public ko wait_connected() {
        if (is_connected()) {
            return ok;
        }
        lock_connected.lock();
        try {
            while (!is_connected() && isup()) {
                cv_connected.await();
            }
        }
        catch(InterruptedException e) {
        }
        finally {
            lock_connected.unlock();
        }
        return is_connected() ? ok : new ko("KO 40032 Not connected.");
    }

    public ko wait_connection(boolean initiate) {
        log("wait_connection. initiate=" + initiate); //--strip
        if (initiate) {
            if (is_connected()) {
                log("already connected"); //--strip
                return null;
            }
            Date birthline = new Date(last_try_connect.getTime() + ms);
            wait(birthline);
            last_try_connect = new Date();
            log("connect"); //--strip
            return connect();
        }
        else {
            log("wait_connected"); //--strip
            return wait_connected();
        }
    }

    @Override //run_recv
    public void run_body() {
        log("run_body. stop_on_disconnection = " + stop_on_disconnection); //--strip
        busyled.set();
        while (isup()) {
            {
                ko r = wait_connection(connect_for_recv);
                if (ko.is_ko(r)) {
                    log("stop_on_disconnection = " + stop_on_disconnection); //--strip
                    if (stop_on_disconnection) {
                        log("stopping"); //--strip
                        stop();
                        break;
                    }
                    continue;
                }
            }
            log("entering recv6"); //--strip
            pair<ko, datagram> r = get_peer().recv6(busyled);
            log("exited recv6");  //--strip
            if (isdown()) {
                break;
            }
            if (ko.is_ko(r.first)) {
                log("disconnect " + r.first.msg); //--strip
                if (r.first == datagram.KO_9021) {
                    assert r.second == null;
                    get_peer().disconnect(new seq_t(0), new reason_t());
                }
                else if (r.first == datagram.KO_0001) {
                    assert r.second != null;
                    channel_t ch = r.second.decode_channel();
                    log("dgram arrived for wrong channel " + ch.value + " " + daemon.channel.value); //--strip
                    reason_t msg = new reason_t("wrong channel " + ch.value + ". This is channel " + daemon.channel.value);
                    get_peer().disconnectx(ch, new seq_t(0), msg);
                }
                else {
                    get_peer().disconnect(new seq_t(0), new reason_t(r.first.msg));
                }
                continue;
            }
            log("process_work"); //--strip
            assert r.second != null;
            if (!get_peer().process_work(r.second)) {
                daemon.process_unhandled(get_peer(), r.second);
            }
        }
        busyled.reset();
        log("end"); //--strip
    }

    @Override
    public void onwakeup() {
        log("don't know (yet) how to force a wake up on ::recv"); //--strip
    }

    public void set_busy_handler(busyled_t.handler_t h) {
        busyled.set_handler(h);
    }

    public boolean process_unhandled(client peer, datagram d) {
        log("process_unhandled. svc" + d.service.value); //--strip
        if (dispatcher != null) {
            log("handed over to dispatcher. svc " + d.service.value); //--strip
            if (dispatcher.dispatch(d)) {
                log("dispatcher consumed the datagram");  //--strip
                return true;
            }
        }
        log("backend sent an unknown service. " + d.service);  //--strip
        return true;
    }

    ReentrantLock lock_recv = new ReentrantLock();
    Condition cv_recv = lock_recv.newCondition();

    public boolean connect_for_recv = true;
    public boolean stop_on_disconnection = true;

    int ms = 10000; //wait after failed connection
    Date last_try_connect = new Date(new Date().getTime() - ms);

    ReentrantLock lock_connected = new ReentrantLock();
    Condition cv_connected = lock_connected.newCondition();

    public busyled_t busyled = new busyled_t();
    public dispatcher_t dispatcher = null;
    rpc_daemon_t daemon = null;
}

