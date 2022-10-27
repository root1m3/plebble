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
import static us.stdint.*;                                                                     // *
import us.ko;                                                                                  // ko
import us.pair;                                                                                // pair
import us.string;                                                                              // string

public class types {

    public static class channel_t extends uint16_t {

        public channel_t() { super(); }
        public channel_t(int v) { super(v); }

    }

    public static class svc_t extends uint16_t {

        public svc_t() { super(); }
        public svc_t(int v) { super(v); }

    }

    public static class seq_t extends uint16_t {

        public seq_t() { super(); }
        public seq_t(int v) { super(v); }

    }

    public static class shost_t extends string {

        public shost_t() {
            super("");
//            value = "";
        }

        public shost_t(String shost) {
            super(shost.trim());
//            value = shost.trim();
        }

        //public String value;
    }

    public static class host_t extends uint32_t {

        public host_t() { super(); }
        public host_t(long v) { super(v); }

    }

    public static class port_t extends uint16_t {

        public port_t() { super(); }
        public port_t(int v) { super(v); }

    }

    public static class shostport_t extends pair<shost_t, port_t> {

        public shostport_t() {
            super(new shost_t(), new port_t(0));
        }

        public shostport_t(shost_t shost, port_t port) {
            super(shost, port);
        }

        public shostport_t(String shost, port_t port) {
            super(new shost_t(shost), port);
        }

    }

    public static class hostport_t extends pair<host_t, port_t> {

        public hostport_t() {
            super(new host_t(0), new port_t(0));
        }

        public hostport_t(host_t host, port_t port) {
            super(host, port);
        }
    }

    public static class reason_t {

        public reason_t() {
            value = "";
        }

        public reason_t(String reason) {
            value = reason;
        }

        public String value;
    }

    public static class ts_t extends uint64_t { //nanosecs

        public ts_t() {
            super();
        }

        public ts_t(long v) {
            super(v);
        }

    }

    public static class ts_ms_t extends uint64_t { //millisecs

        public ts_ms_t(long v) {
            super(v);
        }

    }

    public static class api_v_t extends uint8_t {

        public api_v_t() { super(); }
        public api_v_t(int v) { super((short)v); }

    }

}

