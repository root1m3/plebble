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
package us.gov.socket;
import java.util.concurrent.atomic.AtomicInteger;                                              // AtomicInteger
import us.CFG;                                                                                 // CFG

public class busyled_t {


    static void log(final String line) {                           //--strip
        CFG.log_gov_socket("busyled_t: " + line);                  //--strip
    }                                                              //--strip

    public static interface handler_t {
        void on_busy();
        void on_idle();
    }

    public busyled_t() {
        log("busyled constructor. on=false. " + this); //--strip
    }

    public busyled_t(boolean on) {
        log("busyled constructor2. on=" + on + " " + this); //--strip
        if (on) set();
    }


    public void set() {
        log("busyled set"); //--strip
        int r = ref.getAndIncrement();
        if (r != 0) {
            log("already set " + r); //--strip
            return;
        }
        if (handler == null) {
            log("handler is null. " + this); //--strip
            return;
        }
        log("trigger on_busy"); //--strip
        handler.on_busy();
    }

    public void reset() {
        log("busyled reset"); //--strip
        int r = ref.getAndDecrement();
        if (r != 1) {
            log("already unset " + r); //--strip
            return;
        }
        if (handler == null) {
            log("handler is null. " + this); //--strip
            return;
        }
        log("trigger on_idle"); //--strip
        handler.on_idle();
    }

    public void set_handler(handler_t h) {
        log("set_handler " + h + " " + this); //--strip;
        handler = h;
    }

    private handler_t handler = null;
    private AtomicInteger ref = new AtomicInteger(0);
}

