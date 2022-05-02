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
import java.util.ArrayDeque;                                                                   // ArrayDeque
import java.util.concurrent.locks.Condition;                                                   // Condition
import us.gov.socket.datagram;                                                                 // datagram
import java.util.HashMap;                                                                      // HashMap
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import static us.ko.is_ko;                                                                     // is_ko
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import java.util.Queue;                                                                        // Queue
import java.util.concurrent.locks.ReentrantLock;                                               // ReentrantLock

public class datagram_dispatcher_t implements datagram.dispatcher_t, Runnable {

    static void log(final String line) {         //--strip
       CFG.log_android("datagram_dispatcher_t: " + line);     //--strip
    }                                            //--strip

    public static interface handler_t {
        void on_push(final hash_t target_tid, final uint16_t code, final byte[] payload);
    }

    public datagram_dispatcher_t(int num_workers) {
        workers = new Thread[num_workers];
        for (int i = 0; i < workers.length; ++i) {
            workers[i] = new Thread(this);
            workers[i].start();
        }
    }

    @Override
    public void run() {
        lock.lock();
        try {
            while(true) {
                datagram d = q.poll();
                if (d != null) {
                    lock.unlock();
                    process(d);
                    lock.lock();
                }
                else {
                    cv.await();
                }
            }
        }
        catch (InterruptedException e) {
        }
        finally {
            lock.unlock();
        }
    }

    int connect_sink(handler_t s) {
        mx.lock();
        int id = nextid++;
        try {
            sinks.put(id,s);
        }
        finally {
            mx.unlock();
        }
        log("connected sink " + id); //--strip
        return id;
    }

    void disconnect_sink(int id) {
        mx.lock();
        try {
            sinks.remove(id);
        }
        finally {
            mx.unlock();
        }
        log("disconnected sink " + id); //--strip
    }

    @Override public boolean dispatch(datagram d) {
        lock.lock();
        try {
            q.add(d);
            cv.signal();
        }
        finally {
            lock.unlock();
        }
        return true;
    }

    public void propagate(hash_t tid, uint16_t code, byte[] payload) {
        mx.lock();
        try {
            for (handler_t s: sinks.values()) {
                if (s == null) {
                    log("KO 0074 Why a null here?"); //--strip
                    continue;
                }
                s.on_push(tid, code, payload);
            }
        }
        finally {
            mx.unlock();
        }
    }

    public void process(datagram d) {
        log("main dispatch svc=" + d.service.value); //--strip
        try {
            switch(d.service.value) {
                case us.gov.protocol.relay_push: {
                    us.wallet.cli.rpc_peer_t.push_in_dst_t o_in = new us.wallet.cli.rpc_peer_t.push_in_dst_t();
                    us.gov.io.blob_reader_t reader = new us.gov.io.blob_reader_t(d);
                    ko r = reader.read(o_in);
                    if (is_ko(r)) {
                        log("KO 66985 Parse svc push"); //--strip
                        return;
                    }
                    propagate(o_in.tid, o_in.code, o_in.blob.value);
                    return;
                }
                default:
                    log("KO 79979 Unexpected service." + d.service.value); //--strip
                    return;
                }
        }
        catch(Exception e) {
            log("KO 94995 Exception " + e.getMessage()); //--strip
        }
    }

    HashMap<Integer, handler_t> sinks = new HashMap<Integer, handler_t>();

    Thread[] workers;
    int nextid = 0;
    Queue<datagram> q = new ArrayDeque<datagram>();
    ReentrantLock lock = new ReentrantLock();
    Condition cv = lock.newCondition();
    ReentrantLock mx = new ReentrantLock();
}

