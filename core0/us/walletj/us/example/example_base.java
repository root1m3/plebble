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
package us.example;
import us.wallet.cli.hmi;
import us.ko;
import static us.ko.*;
import java.security.KeyPair;
import java.security.PublicKey;
import us.gov.id.peer_t;
import static us.gov.id.types.pin_t;
import static us.gov.socket.types.hostport_t;
import static us.gov.socket.types.shostport_t;
import static us.gov.id.types.pport_t;
import static us.gov.socket.types.port_t;
import static us.gov.socket.types.shost_t;
import static us.gov.socket.types.channel_t;
import static us.gov.id.peer_t.role_t;
import java.net.Socket;
import us.wallet.cli.rpc_daemon_t;
import us.wallet.cli.rpc_peer_t;
import us.string;
import us.pair;
import us.gov.io.cfg1;
import static us.stdint.uint16_t;

public abstract class example_base {

    public abstract String description();

    public example_base() {
    }


    public ko run(String[] args) {
        System.out.println("Running " + description());
        return ok;
    }

    public static class my_rpc_peer_t extends rpc_peer_t {

        public my_rpc_peer_t(rpc_daemon_t rpc_daemon) {
            super(rpc_daemon);
        }

        public my_rpc_peer_t(rpc_daemon_t rpc_daemon, Socket sock) {
            super(rpc_daemon, sock);
        }

        //here we as clients authenticate the backend so we can trust it is legit (protection against phising)
        @Override public boolean authorize(PublicKey p, pin_t pin) {
            System.out.println("Remote end identified with public key: " + us.gov.crypto.ec.instance.to_b58(p));
            // we actually trust the program running on the other side is our legit wallet daemon with the right public key
            return true;
        }
    }

    public static class my_rpc_daemon extends rpc_daemon_t {

        public my_rpc_daemon(final channel_t channel, final shostport_t shostport, KeyPair keys) {
            super(keys, channel, shostport, role_t.role_device, null); // null dispatcher // needed only for async api calls
        }

        @Override public us.gov.socket.client create_client() {
            return new my_rpc_peer_t(this);
        }

        @Override public ko connect() {
            System.out.println("Connecting to " + shostport.first.value + ":" + shostport.second.value);
            return connect(new pin_t(123));
        }

    }

    public static KeyPair init_keys() {
        System.out.println("Private key file: " + cfg1.k_file("."));
        pair<ko, cfg1> r = cfg1.load(channel, ".", true);  //[home, generate]; creating k file in cur dir
        if (is_ko(r.first)) {
            System.err.println("Error: " + r.first.msg);
            System.exit(1);
        }
        KeyPair keys = r.second.keys;
        //base58 encoding of keys:
        System.out.println("Private key: " + us.gov.crypto.ec.instance.to_b58(keys.getPrivate()));
        System.out.println("Public key: " + us.gov.crypto.ec.instance.to_b58(keys.getPublic()));
        return keys;
    }

    void bring_up() {
        System.out.println("channel: " + channel.value);
        rpc_daemon = new my_rpc_daemon(channel, shostport, keys);
        {
            ko r = rpc_daemon.start();
            if (is_ko(r)) {
                System.err.println("Error: " + r.msg);
                System.exit(1);
            }
        }
        {
            ko r = rpc_daemon.wait_ready(3); //timeout
            if (is_ko(r)) {
                System.err.println("Error: " + r.msg);
                System.exit(1);
            }
        }
        {
            //wait for auth handshake to complete
            ko r = rpc_daemon.handler.wait_connected();
            if (is_ko(r)) {
                System.err.println("Error: " + r.msg + ". Shutting down.");
                shut_down();
                System.err.println("Error: " + r.msg);
                System.exit(1);
            }
        }
        {
            //wait for auth handshake to complete
            ko r = rpc_daemon.get_peer().wait_auth();
            if (is_ko(r)) {
                System.err.println("Error: " + r.msg + ". Shutting down.");
                shut_down();
                System.err.println("Error: " + r.msg);
                System.exit(1);
            }
        }
    }

    void shut_down() {
        rpc_daemon.stop();
        rpc_daemon.join(); //wait until is stopped
    }

    static KeyPair keys = init_keys();
//    static shostport_t shostport = new shostport_t(new shost_t("104.131.186.38"), new port_t(16673));
    static shostport_t shostport = new shostport_t(new shost_t("localhost"), new port_t(16673));
    static channel_t channel = new channel_t(0);
    my_rpc_daemon rpc_daemon;
}

