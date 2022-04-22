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
package us.gov.socket.outbound;
import java.util.ArrayList;                                                                    // ArrayList
import java.util.Arrays;                                                                       // Arrays
import us.gov.bgtask;                                                                          // bgtask
import us.gov.socket.busyled_t;                                                                // busyled_t
import us.CFG;                                                                                 // CFG
import us.gov.socket.client;                                                                   // client
import java.lang.Comparable;                                                                   // Comparable
import java.util.concurrent.locks.Condition;                                                   // Condition
import us.gov.socket.datagram;                                                                 // datagram
import java.util.HashMap;                                                                      // HashMap
import static us.gov.socket.types.*;                                                           // *
import us.stdint.*;                                                                            // *
import java.lang.InterruptedException;                                                         // InterruptedException
import java.io.IOException;                                                                    // IOException
import us.ko;                                                                                  // ko
import java.util.concurrent.locks.Lock;                                                        // Lock
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import java.util.PriorityQueue;                                                                // PriorityQueue
import java.util.concurrent.locks.ReentrantLock;                                               // ReentrantLock
import java.net.SocketException;                                                               // SocketException

@SuppressWarnings("serial")
public class send_queue_t extends bgtask implements bgtask.callback {

    public static int wmh = CFG.SENDQ_WMH;
    public static int schunk = CFG.SENDQ_SCHUNK;

    static void log(final String line) {                                        //--strip
        CFG.log_gov_socket("outbound.send_queue_t: " + line);                   //--strip
    }                                                                           //--strip

    public static class qi extends pair<datagram, client> implements Comparable<qi> {
        public qi(datagram d, client c) {
            super(d, c);
            log("new qi. dgram sz " + first.size() + " pri " + first.service.value + " sendref " + second.sendref.get()); //--strip
            first.dend = 0;
            second.sendref.incrementAndGet();
        }

        public void destroy() {
            first = null;
            second.sendref.decrementAndGet();
        }

        public int compareTo(qi other) {
            if (first.service.value == other.first.service.value) return 0;
            return first.service.value > other.first.service.value? -1 : 1;
        }

        public static long prev_uid = 0;
        public long uid = ++prev_uid;
    }

    public static class ongoing_t extends HashMap<Long, pair<client, qi>>  {
        public void set(qi x) {
            pair<client, qi> i = get(x.second.uid);
            if (i != null) {
                return;
            }
            put(x.second.uid, new pair<client, qi>(x.second, x));
        }

        public qi reset(qi x) {
            pair<client,qi> i = get(x.second.uid);
            if (i == null) return null;
            remove(x.second.uid);
            return x;
        }
    };

    public send_queue_t() {
         set_callback(this);
    }

    public void onwakeup() {
        log("onwakeup"); //--strip
        lock.lock();
        try {
            cv.signalAll();
        }
        finally {
            lock.unlock();
        }
    }

    public void clear() {
        lock.lock();
        try {
            ongoing.clear();
            q.clear();
        }
        finally {
            lock.unlock();
        }
    }

    public void dump(final String prefix, PrintStream os) {
        lock.lock();
        try {
            os.println(prefix + "active " + is_active());
            os.println(prefix + "wmh " + wmh);
            os.println(prefix + "schunk " + schunk);
            os.println(prefix + "queued " + q.size());
        }
        finally {
            lock.unlock();
        }
    }

    int get_size() {
        lock.lock();
        int s = 0;
        try {
            s = q.size();
        }
        finally {
            lock.unlock();
        }
        return s;
    }

    void flood(datagram d, client cli) {
        log("watermark high has been touched. The given delivery has been discarded. wmh " + wmh + " size " + q.size()); //--strip
    }

    void send(datagram d, client cli, int pri) {
        log("admission pri " + pri + " sz " + d.size() + " seq " + d.decode_sequence().value + " recipient " + cli.endpoint() + " dgram svc field (to be overwritten) " + d.service.value); //--strip
        assert d.completed();
        log("overwriting service field with pri " + pri); //--strip
        d.service.value = pri;
        String ret = null;
        lock.lock();
        try {
            if (q.size() >= wmh) {
                log("watermarkhigh touched(2) " + wmh + "flushing some datagrams"); //--strip
                log("TODO"); //--strip //TODO
                qi o = q.poll();
                flood(o.first, o.second);
                log("destroyed unsent dgram svc" + o.first.service.value); //--strip
            }
            q.add(new qi(d, cli));
            task_wakeup();
            assert q.size() < wmh;
        }
        finally {
            lock.unlock();
        }
    }

    void purge(client c) {
        log("purge client" + c.endpoint() + " initial size " + q.size()); //--strip
        lock.lock();
        try {
            ArrayList<qi> v = new ArrayList<qi>(q.size());
            while(!q.isEmpty()) {
                qi x = q.poll();
                if (x.second.uid == c.uid) {
                    log("deleting datagram " + x.first + " progress " + x.first.dend + '/' + x.first.size()); //--strip
                    ongoing.reset(x);
                    continue;
                }
                v.add(x);
            }
            for (qi i:v) {
                q.add(i);
            }
            log("ended purging client" + c.endpoint() + " final size " + q.size()); //--strip
        }
        finally {
            lock.unlock();
        }
    }

    qi next() {
        qi ans = null;
        lock.lock();
        try {
            ans = next_();
        }
        finally {
            lock.unlock();
        }
        return ans;

    }

    qi next_() {
        while (true) {
            if(q.isEmpty()) {
                log("empty"); //--strip
                reset_wait();
                return null;
            }
            qi i = q.peek();
            if (i.first.size() == i.first.dend) {
                log("finished. comm completed send. dend= " + i.first.dend + " svc= " + i.first.service.value + " seq " + i.first.decode_sequence().value + " hash= " + i.first.compute_hash().encode()); //--strip
                q.poll();
                ongoing.reset(i);
                continue;
            }
            log("dgam cli "+i.second.endpoint()+" pri "+i.first.service.value+" progress "+i.first.dend+'/'+i.first.size()); //--strip
            //avoid starting a new dgram for a client that has another ongoing
            qi r = replace(i); //same i (if no other qi has been started for this client), othersise the unfinished qi
            if (r.uid != i.uid) { //--strip
                log("dgam cli " + i.second.uid + " pri " + i.first.service.value + " progress " + i.first.dend + "/" + i.first.size()); //--strip
            } //--strip
            return r;
        }
    }

    qi replace(qi x) {
        assert x != null;
        pair<client,qi> i = ongoing.get(x.second.uid);
        if (i != null) {
            if (i.second.uid != x.uid) {  //--strip
                log("replaced"); //--strip
            }                    //--strip
            if (i.second.first.dend == i.second.first.size()) {
                log("not replacing with a finished dgram"); //--strip
                ongoing.remove(i);
                return x;
            }
            return i.second;
        }
        return x;
    }

    public void run_body() {
        log("ethernet datagram estimated optimal size " + schunk); //--strip
        while (isup()) {
            log("take next"); //--strip
            qi d = next();
            if (d == null) {
                log("sleep"); //--strip
                await();
                log("awaken"); //--strip
                continue;
            }
            int rem = d.first.size() - d.first.dend;
            int sz = rem < 2 * schunk ? rem : schunk;
            log("dgram pri " + d.first.service.value + " " + (d.first.decode_service().value == 0 ? "encrypted" : "public") + " remaining " + rem + " dgram progress " + d.first.dend + " / " + d.first.size()); //--strip
            if (d.second.sendref.get() < 0) {
                log("client dying " + d.second + " dgram progress was " + d.first.dend + " / " + d.first.size()); //--strip
                d.first.dend = d.first.size();
                continue;
            }
            if (d.second.sock == null) {
                log("sock? dgram progress was " + d.first.dend + " / " + d.first.size()); //--strip
                d.first.dend = d.first.size();
                continue;
            }

            byte[] chunk = Arrays.copyOfRange(d.first.bytes, d.first.dend, d.first.dend+sz);
            boolean wrong = false;
            busyled.set();
            try {
                //The write method of OutputStream calls the write method of one argument on each of the bytes to be written out. Subclasses are encouraged to override this method and provide a more efficient implementation.
                d.second.sock.getOutputStream().write(chunk);
                log("done"); //--strip
            }
            catch (SocketException e) {
                log("KO 20120 " + e.getMessage()); //--strip
                wrong = true;
            }
            catch (IOException e) {
                log("KO 20121 " + e.getMessage()); //--strip
                wrong = true;
            }
            catch (Exception e) {
                log("KO 24922 " + e.getMessage()); //--strip
                wrong = true;
            }
            busyled.reset();
            if (!wrong) {
                log("::send sz"+ sz); //--strip
                d.first.dend += sz;
                log("sent " + sz + " to " + d.second.endpoint() + " progress " + d.first.dend + " / " + d.first.size() + " [" + (100.0*((float)d.first.dend)/((float)(d.first.bytes.length)))+" %]"); //--strip
                if (d.first.in_progress()) {
                    ongoing.set(d);
                }
                else {
                    ongoing.reset(d);
                }
                continue;
            }
            else {
                log("::send sz " + sz + " exceptn."); //--strip
                d.second.disconnect(new seq_t(0), new reason_t());
                d.first.dend = d.first.size(); //finish with this dgram
            }
        }
        log("end"); //--strip
    }

    public void await() {
        if (reset_wait()) {
            return;
        }
        log("wait"); //--strip
        lock.lock();
        try {
            while (!resumed()) {
                cv.await();
            }
        }
        catch (InterruptedException e) {
        }
        finally {
            lock.unlock();
        }
        reset_wait();
    }

    public void set_busy_handler(busyled_t.handler_t hsend) {
        busyled.set_handler(hsend);
    }

    PriorityQueue<qi> q = new PriorityQueue<qi>(CFG.SENDQ_SIZE);
    Lock lock = new ReentrantLock();
    Condition cv = lock.newCondition();
    ongoing_t ongoing = new ongoing_t();
    public busyled_t busyled = new busyled_t();

}

