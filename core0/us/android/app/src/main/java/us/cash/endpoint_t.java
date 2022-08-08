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
import static us.stdint.*;                                                                     // *
import us.gov.socket.types.*;                                                                  // *
import org.json.JSONException;                                                                 // JSONException
import org.json.JSONObject;                                                                    // JSONObject
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok

public class endpoint_t {

    public endpoint_t() {
        shost = new shost_t("127.0.0.1");
        port = CFG.walletd_port;
        channel = us.CFG.CHANNEL;
    }

    public endpoint_t(shost_t host_, port_t port_, channel_t channel_) {
        shost = host_;
        port = port_;
        channel = channel_;
    }

    public endpoint_t(host_t host_, port_t port_, channel_t channel_) {
        shost = us.gov.socket.client.ip4_decode(host_);
        port = port_;
        channel = channel_;
    }

    public endpoint_t(endpoint_t other) {
        shost = other.shost;
        port = other.port;
        channel = other.channel;
    }

    public endpoint_t(JSONObject o) {
        try {
            shost = new shost_t(o.getString("wanip"));
            port = new port_t(o.getInt("wanport"));
            channel = new channel_t(o.getInt("channel"));
        }
        catch(JSONException e) {
            shost = new shost_t("127.0.0.1");
            port = CFG.walletd_port;
            channel = us.CFG.CHANNEL;
            //error_manager.manage(e, "KO 40937"); //--strip
        }
    }

    public boolean equals(endpoint_t other) {
        return shost.value.equals(other.shost.value) && port.value == other.port.value && channel.value == other.channel.value;
    }

    public String to_string() {
        return shost.value + ":" + port.value + " channel " + channel.value;
    }

    public shostport_t shostport() {
        return new shostport_t(shost, port);
    }

    shost_t shost;
    port_t port;
    channel_t channel;
}

