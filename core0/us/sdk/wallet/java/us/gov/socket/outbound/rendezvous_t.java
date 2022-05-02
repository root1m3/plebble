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
package us.gov.socket.outbound;
import java.util.Arrays;                                                                       // Arrays
import us.gov.bgtask;                                                                          // bgtask
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.socket.busyled_t;                                                                // busyled_t
import java.io.ByteArrayOutputStream;                                                          // ByteArrayOutputStream
import us.CFG;                                                                                 // CFG
import java.nio.charset.Charset;                                                               // Charset
import java.time.temporal.ChronoUnit;                                                          // ChronoUnit
import us.gov.socket.client;                                                                   // client
import java.util.concurrent.locks.Condition;                                                   // Condition
import us.gov.socket.datagram;                                                                 // datagram
import java.util.Date;                                                                         // Date
import java.util.HashMap;                                                                      // HashMap
import java.util.HashSet;                                                                      // HashSet
import static us.gov.socket.types.*;                                                           // *
import us.stdint.*;                                                                            // *
import java.time.Instant;                                                                      // Instant
import java.io.IOException;                                                                    // IOException
import java.security.KeyPair;                                                                  // KeyPair
import us.ko;                                                                                  // ko
import java.util.LinkedList;                                                                   // LinkedList
import java.time.LocalTime;                                                                    // LocalTime
import static us.ko.ok;                                                                        // ok
import java.io.OutputStream;                                                                   // OutputStream
import us.pair;                                                                                // pair
import us.gov.auth.peer_t;                                                                     // peer_t
import java.io.PrintStream;                                                                    // PrintStream
import us.gov.protocol;                                                                        // protocol
import java.security.PublicKey;                                                                // PublicKey
import java.util.concurrent.locks.ReentrantLock;                                               // ReentrantLock
import java.util.Set;                                                                          // Set
import java.net.SocketException;                                                               // SocketException
import java.net.Socket;                                                                        // Socket
import java.util.Stack;                                                                        // Stack
import us.string;                                                                              // string
import java.util.TimerTask;                                                                    // TimerTask
import java.io.UnsupportedEncodingException;                                                   // UnsupportedEncodingException

@SuppressWarnings("serial")
public final class rendezvous_t extends HashMap<Integer, datagram> {

    public static ko KO_3029 = new ko("KO 3029.2 Timeout");
    public static ko KO_20190 = new ko("KO 20190 Backend returned a KO code.");

    static datagram waiting_arrival = new datagram(new channel_t(0), 0);

    static void log(final String line) {                                        //--strip
        CFG.log_gov_socket("outbound.rendezvous_t: " + line);                   //--strip
    }                                                                           //--strip

    public rendezvous_t(caller_daemon_t caller_daemon) {
        this.caller_daemon = caller_daemon;
    }

    public void flush() {
        //new Exception().printStackTrace(System.out);
        log("flush"); //--strip
        finished = true;
        lock.lock();
        try {
            cv.signalAll();
        }
        finally {
            lock.unlock();
        }
    }

    public boolean arrived(datagram d) {
        assert d != null;
        lock.lock();
        try {
            seq_t seq = d.decode_sequence();
            log("rendezvous: arrived datagram svc " + d.service.value + " seq " + seq.value); //--strip
            datagram i = get(seq.value);
            if (i == null) {
                log("no process waiting for this dgram"); //--strip
                return false;
            }
            if (i != waiting_arrival) {
                log("sequence conflict, same seq arrived twice"); //--strip
            }
            put(seq.value, d);
            log("wake sendrecv up"); //--strip
            cv.signalAll();
        }
        finally {
            lock.unlock();
        }
        return true;
    }

    public pair<ko, datagram> sendrecv(client peer, datagram d, string remote_error) {
        log("sendrecv. " + d.service.value);  //--strip
        lock.lock();
        log("entered critical section");  //--strip
        try {
            int iseq = ++next_seq;
            d.encode_sequence(new seq_t(iseq));
            put(iseq, waiting_arrival);
            log("sendrecv. sending dgram seq=" + iseq + " decoded seq " + d.decode_sequence().value); //--strip
            assert iseq == d.decode_sequence().value; //--strip
            ko ret = caller_daemon.send1(peer, d);
            if (ko.is_ko(ret)) {
                log("sent failed"); //--strip
                remote_error.value = "";
                return new pair<ko, datagram>(ret, null);
            }
            log("waiting for " + CFG.DGRAM_ROUNDTRIP_TIMEOUT_SECS + " seconds"); //--strip
            //lock.lock();
            try {
                long t0 = peer.activity.get();
                while(true) {
                    Date deadline = new Date(System.currentTimeMillis() + (CFG.DGRAM_ROUNDTRIP_TIMEOUT_SECS * 1000));
                    datagram i = null;
                    while(new Date().compareTo(deadline) < 0) {
                        cv.awaitUntil(deadline);
                        if (finished) {
                            return new pair<ko, datagram>(new ko("KO 60593 Interrupted while waiting for a response."), null);
                        }
                        i = get(iseq);
                        if (i == null) break; //cancelled
                        if (i != waiting_arrival) break; //arrived
                        log("keep waiting"); //--strip
                    }
                    if (i == waiting_arrival) {
                        long t1 = peer.activity.get();
                        if (t1 > t0) { //something arrived, not whole dgram, but something.
                            t0 = t1;
                            continue;
                        }
                    }
                    break;
                }
            }
            catch (InterruptedException e) {
//System.out.println(e.getMessage());
                ko r = new ko("KO 61593 Interrupted");
                log(r.msg + " - " + e.getMessage()); //--strip
                remote_error.value = "";
                return new pair<ko, datagram>(r, null);
            }
            //finally {
            //    lock.unlock();
            //}
            //log("exited critical section"); //--strip
            log("sendrecv: wakeup"); //--strip
            datagram dr = get(iseq);
            if (dr == null) {
                ko r = new ko("KO 30291 Cancelled.");
                log(r.msg); //--strip
                log("seq not found in rendezvous table. Cancelled. seq=" + iseq); //--strip
                remote_error.value = "";
                return new pair<ko, datagram>(r, null);
            }
            remove(dr);
            if (dr == waiting_arrival) {
                log("Timeout. seq=" + iseq); //--strip
                log(KO_3029.msg); //--strip
                remote_error.value = "";
                return new pair<ko, datagram>(KO_3029, null);
            }
            log("returning dgram svc" + dr.service.value + " seq " + dr.decode_sequence().value); //--strip
            if (dr.service.value == protocol.socket_error) {
                blob_reader_t reader = new blob_reader_t(dr);
                ko r = reader.read(remote_error);
                if (ko.is_ko(r)) {
                    remote_error.value = r.msg;
                }
                log(KO_20190.msg + " " + remote_error.value); //--strip
                return new pair<ko, datagram>(KO_20190, null);
            }
            else {
                remote_error.value = "";
            }
            return new pair<ko, datagram>(ko.ok, dr);
        }
        finally {
            lock.unlock();
            log("exited critical section"); //--strip
        }
    }

    caller_daemon_t caller_daemon;
    int next_seq = 0;
    ReentrantLock lock = new ReentrantLock();
    Condition cv = lock.newCondition();
    boolean finished = false;
}

