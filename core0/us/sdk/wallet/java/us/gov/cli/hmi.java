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
package us.gov.cli;
import static us.gov.id.types.*;                                                               // *
import static us.gov.io.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import static us.ko.is_ko;                                                                     // is_ko
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok

public class hmi {

    public void on_connect(ko r) {
    }

    public void on_stop() {
    }

    public void verification_completed(boolean verif_ok) {
    }

    public void on_I_disconnected(final reason_t reason) {
    }

    public void on_peer_disconnected(final reason_t reason) {
    }

    public void upgrade_software() {
        log("Peer is signaling the existence of a upgrade_software."); //--strip
    }

    static class params_t {
        channel_t channel = us.CFG.CHANNEL;
    }

    params_t p = new params_t();
}

