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
package us.gov.id;
import us.CFG;                                                                                 // CFG
import us.gov.socket.client;                                                                   // client
import static us.gov.socket.types.channel_t;                                                                   // client
import static us.gov.socket.datagram.dispatcher_t;                                             // dispatcher_t
import us.gov.crypto.ec;                                                                       // ec
import java.io.PrintStream;                                                                    // PrintStream
import us.gov.crypto.ripemd160;                                                                // ripemd160

public abstract class rpc_daemon_t extends us.gov.socket.rpc_daemon_t implements daemon0_t {

    static void log(final String line) {                                           //--strip
        CFG.log_gov_id("rpc_daemon_t :" + line);                                   //--strip
    }                                                                              //--strip

    public rpc_daemon_t(channel_t channel, dispatcher_t dispatcher) {
        super(channel, dispatcher);
    }

    @Override public client create_client() {
        return new peer_t(this);
    }

    @Override public void dump(final String prefix, PrintStream os) {
        os.println(prefix + "id::rpc_daemon_t: caller public key: " + ec.instance.to_b58(get_keys().getPublic()) + " addr " + ec.instance.to_encoded_address(get_keys().getPublic()));
        super.dump(prefix, os);
    }

}

