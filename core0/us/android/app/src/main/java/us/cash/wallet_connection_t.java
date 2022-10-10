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
import java.util.ArrayList;                                                                    // ArrayList
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import java.util.Date;                                                                         // Date
import us.gov.crypto.ec;                                                                       // ec
import static us.gov.crypto.types.*;                                                           // *
import static us.gov.id.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import us.stdint.*;                                                                            // *
import static us.ko.is_ko;                                                                     // is_ko
import java.security.KeyPair;                                                                  // KeyPair
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import static us.gov.io.types.blob_t.serid_t;                                                  // serid_t
import us.string;                                                                              // string

public class wallet_connection_t implements us.gov.io.seriable {

    private static void log(final String line) {                          //--strip
        CFG.log_android("wallet_connection_t: " + line);                  //--strip
    }                                                                     //--strip

    public wallet_connection_t() {
        name_ = new string("default wallet");
        ssid = new string("");
        addr = new string("");
        endpoint = new endpoint_t();
        ts = new uint64_t(0);
    }

    public wallet_connection_t(uint64_t ts_, string addr_, string nm, string ssid_, endpoint_t ep) {
        name_ = nm;
        ssid = ssid_;
        addr = addr_;
        endpoint = ep;
        ts = ts_;
    }

    public wallet_connection_t(wallet_connection_t other) {
        name_ = new string("copy_" + other.name_.value);
        ssid = new string("");
        addr = new string("");
        endpoint = new endpoint_t(other.endpoint);
        ts = new uint64_t(0);
    }

    public ko set_endpoint(endpoint_t endpoint_) {
        endpoint = endpoint_;
        return ok;
    }

    public String get_title() {
        String s = name_.value;
        log("get title for " + s); //--strip
        if (ssid.value != null && !ssid.value.isEmpty()) {
            log("adding ssid " + ssid.value); //--strip
            s = s + "_" + ssid.value;
        }
        if (addr.value != null && !addr.value.isEmpty()) {
            log("adding addr " + addr.value); //--strip
            s = s + "_" + addr.value;
        }
        log("returning " + s); //--strip
        return s;
    }

    @Override public serid_t serial_id() { return serid_t.no_header; }

    @Override public int blob_size() {
        int sz = blob_writer_t.blob_size(name_) +
                 blob_writer_t.blob_size(ssid) +
                 blob_writer_t.blob_size(addr) +
                 blob_writer_t.blob_size(endpoint) +
                 blob_writer_t.blob_size(ts);
        return sz;
    }

    @Override public void to_blob(blob_writer_t writer) {
        log("to_blob"); //--strip
        writer.write(name_);
        writer.write(ssid);
        writer.write(addr);
        writer.write(endpoint);
        writer.write(ts);
    }

    @Override public ko from_blob(blob_reader_t reader) {
        log("from_blob"); //--strip
        {
            ko r = reader.read(name_);
            if (ko.is_ko(r)) return r;
        }
        {
            ko r = reader.read(ssid);
            if (ko.is_ko(r)) return r;
        }
        {
            ko r = reader.read(addr);
            if (ko.is_ko(r)) return r;
        }
        {
            ko r = reader.read(endpoint);
            if (ko.is_ko(r)) return r;
        }
        {
            ko r = reader.read(ts);
            if (ko.is_ko(r)) return r;
        }
        log("read wallet connection named " + name_.value); //--strip
        return ok;


/*
                    json.put("addr", dep.addr);
                    json.put("name", dep.name_);
                    json.put("ssid", dep.ssid);
                    json.put("power", dep.hmi != null ? 1 : 0);
                    json.put("k", us.gov.crypto.ec.instance.to_b58(dep.cfg.keys.getPrivate()));
                    json.put("ts", dep.ts);
                    json.put("ip", ep.shost.value);
                    json.put("port", ep.port.value);
                    json.put("channel", ep.channel.value);
*/
    }

    public string name_;
    public string ssid;
    public string addr;
    public endpoint_t endpoint;
    uint64_t ts = new uint64_t(0);
}

