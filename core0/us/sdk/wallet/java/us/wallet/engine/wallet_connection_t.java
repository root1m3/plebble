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
package us.wallet.engine;
import java.util.ArrayList;                                                                    // ArrayList
import us.gov.crypto.base58;                                                                   // base58
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.types.blob_t;                                                                 // blob_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import us.CFG;                                                                                 // CFG
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

    private static void log(final String line) {                                 //--strip
        CFG.log_wallet_trader("wallet_connection_t: " + line);                   //--strip
    }                                                                            //--strip

    public wallet_connection_t() {
        log("constructor 1 "); //--strip
        name_ = new string("default wallet");
        ssid = new string("");
        addr = new string("");
        subhome = new string("");
        ip4_endpoint = new ip4_endpoint_t();
        ts = new uint64_t(0);
    }

    public wallet_connection_t(String nm, ip4_endpoint_t ep) {
        log("constructor 3 "); //--strip
        name_ = new string(nm);
        ssid = new string("");
        addr = new string("");
        subhome = new string("");
        ip4_endpoint = new ip4_endpoint_t(ep);
        ts = new uint64_t(0);
    }

    public wallet_connection_t(uint64_t ts_, string addr_, string subhome_, string nm, string ssid_, ip4_endpoint_t ep) {
        log("constructor 2 "); //--strip
        name_ = nm;
        ssid = ssid_;
        addr = addr_;
        subhome = subhome_;
        ip4_endpoint = ep;
        ts = ts_;
    }

    private wallet_connection_t(wallet_connection_t other) {
        log("copy constructor"); //--strip
        name_ = new string(other.name_);
        ssid = new string(other.ssid);
        addr = new string(other.addr);
        subhome = new string(other.subhome);
        ip4_endpoint = new ip4_endpoint_t(other.ip4_endpoint);
        ts = new uint64_t(other.ts);
    }

    public wallet_connection_t copy() {
        log("copy"); //--strip
        wallet_connection_t wc = new wallet_connection_t(this);
        wc.name_.value = "copy_" + name_.value;
        wc.ssid.value = "";
        wc.addr.value = "";
        wc.subhome.value = "";
        wc.ts.value = 0;
        return wc;
    }

    void write(blob_t blob) {
        log("write to blob"); //--strip
        serid_t serid = serial_id();
        int sz = (serid.value != 0 ? blob_writer_t.header_size() : 0) + blob_size();
        if (sz == 0) {
            blob.clear();
            return;
        }
        blob_writer_t w = new blob_writer_t(blob, sz);
        if (serid.value != 0) {
            w.write_header(serid);
        }
        to_blob(w);
        assert w.cur == blob.value.length;
    }

    public void log_blob() {                                                               //--strip
        log("en-uk entry:");                                                               //--strip
        wallet_connection_t wc = new wallet_connection_t(name_.value, ip4_endpoint);             //--strip
        blob_t blob = new blob_t();                                                        //--strip
        wc.write(blob);                                                                    //--strip
        String b = us.gov.crypto.base58.encode(blob.value);                                //--strip
        log("str95=\"" + b + "\" #default wallet_connection blob b58");                    //--strip
    }                                                                                      //--strip
                                                                                           //--strip
    public ko set_endpoint(ip4_endpoint_t ip4_endpoint_) {
        ip4_endpoint = ip4_endpoint_;
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
                 blob_writer_t.blob_size(subhome) +
                 blob_writer_t.blob_size(ip4_endpoint) +
                 blob_writer_t.blob_size(ts);
        return sz;
    }

    @Override public void to_blob(blob_writer_t writer) {
        log("to_blob"); //--strip
        writer.write(name_);
        writer.write(ssid);
        writer.write(addr);
        writer.write(subhome);
        writer.write(ip4_endpoint);
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
        if (reader.header.serid.value == 'X' && reader.header.version.value == 11) {
            System.err.println("Remove old code");
            assert false; //--strip
            return new ko("KO 59200 Remove old code");
        }
        boolean miss_subhome = (reader.header.serid.value == 'X' && reader.header.version.value == 9);
        if (!miss_subhome) {
            ko r = reader.read(subhome);
            if (is_ko(r)) return r;
        }
        {
            ko r = reader.read(ip4_endpoint);
            if (ko.is_ko(r)) return r;
        }
        {
            ko r = reader.read(ts);
            if (ko.is_ko(r)) return r;
        }
        log("read wallet connection named " + name_.value); //--strip
        return ok;
    }

    public string name_;
    public string ssid;
    public string addr;
    public string subhome;
    public ip4_endpoint_t ip4_endpoint;
    public uint64_t ts = new uint64_t(0);
}

