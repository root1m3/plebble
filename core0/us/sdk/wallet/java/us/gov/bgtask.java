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
package us.gov;
import java.util.concurrent.atomic.AtomicBoolean;                                              // AtomicBoolean
import java.util.concurrent.atomic.AtomicInteger;                                              // AtomicInteger
import us.CFG;                                                                                 // CFG
import java.util.concurrent.locks.Condition;                                                   // Condition
import java.util.Date;                                                                         // Date
import java.lang.InterruptedException;                                                         // InterruptedException
import us.ko;                                                                                  // ko
import java.util.concurrent.locks.Lock;                                                        // Lock
import static us.ko.ok;                                                                        // ok
import java.util.concurrent.locks.ReentrantLock;                                               // ReentrantLock
import java.lang.Runnable;                                                                     // Runnable
import java.lang.Thread;                                                                       // Thread
import java.util.concurrent.TimeUnit;                                                          // TimeUnit

public class bgtask implements Runnable {

    static void log(final String line) {                        //--strip
        CFG.log_gov("bgtask: " + line);                         //--strip
    }                                                           //--strip

    public static ko KO_81021 = new ko("KO 81021 Ready status never reached.");

    public interface callback {
        void run_body();
        void onwakeup();
    }

    public bgtask() {
        cb = null;
    }

    public bgtask(callback cb0) {
        cb = cb0;
    }

    public void set_callback(callback r) {
        cb = r;
    }

    public ko start() {
        log("start"); //--strip
        if (active) {
            log("already started"); //--strip
            return null;
        }
        if (cb == null) return new ko("KO 70694 Callback unset.");

        reset();
        active = true;
        th = new Thread(this);
        th.start();
        return ok;
    }

    public ko wait_ready(Date deadline) {
        mx_ready.lock();
        try {
            if (ready) {
                return ok;
            }
            while(true) {
                if (ready || isdown()) {
                    break;
                }
                try {
                    cv_ready.awaitUntil(deadline);
                }
                catch (InterruptedException e) {
                }
                if (new Date().compareTo(deadline) >= 0) {
                    return ready ? ok : KO_81021;
                }
            }
            if (ready) {
                return ok;
            }
            return new ko("KO 40392 shut down.");
        }
        finally {
            mx_ready.unlock();
        }
    }

    public void stop() {
        log("stop"); //--strip
        if (!active) {
            log("already stopped");  //--strip
            return;
        }
        log("stopping");  //--strip
        th = null;
        resume.set(2);
        task_wakeup();
        return;
    }

    public void on_stop() {}

    public void join() {
        if (th != null) {
            try {
                th.join();
            }
            catch (InterruptedException e) {
            }
            resume.set(0);
        }
    }

    public void task_wakeup() {
        log("wakeup"); //--strip
        reset_resume();
        if (cb != null) cb.onwakeup();
    }

    public void run() {
        if (cb == null) return;
        {
            mx_ready.lock();
            try {
                ready = true;
                cv_ready.signalAll();
            }
            finally {
                mx_ready.unlock();
            }
        }
        cb.run_body();
        {
            mx_ready.lock();
            try {
                ready = false;
            }
            finally {
                mx_ready.unlock();
            }
        }
        active = false;
        on_stop();
    }

    public boolean reset_wait() {
        int r = resume.get();
        if (r > 0) {
            if (r == 1) resume.set(0);
            return true;
        }
        return false;
    }

    public boolean reset_resume() { //resume state is anything !=0
        log("reset resume"); //--strip
        int r = resume.get();
        if (r == 0) {
            resume.set(1);
            return true;
        }
        return false;
    }

    public boolean isup() { return resume.get() != 2; }
    public boolean isdown() { return resume.get() == 2; }
    public boolean resumed() { return resume.get() > 0; }
    public void reset() { resume.set(0); }
    public boolean is_active(){ return active; }

    protected AtomicInteger resume = new AtomicInteger(0);
    protected Thread th = null;
    protected boolean active = false;
    protected callback cb = null;
    protected ReentrantLock mx_ready = new ReentrantLock();
    protected boolean ready = false;
    protected Condition cv_ready = mx_ready.newCondition();

}

