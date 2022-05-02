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
package us.example;
import us.ko;
import static us.ko.*;
import static us.stdint.uint16_t;
import us.gov.crypto.ripemd160.hash_t;
import static us.gov.io.types.*;
import us.string;

public class timeseries_add extends example_base {

    @Override public String description() {
        return "Event recording";
    }

    @Override public ko run(String[] args) {
        bring_up();

/*
# us-wallet gen_keys
priv C6QxBfz5kq4yoJNSuhXp4rrPaF5r1AuXWDTT1bsKHKPk
pub y1zTUnXeKUaxDXtyVdsdwqRz97oKPvW1QKoNM7YUG358
addr XEiGLZ9Xq8Kzp4NSMk553J6kPe1
*/
        {
            System.out.println("\nAsking for available timeseries:");
            bin_t data = new bin_t(new String("Dummy Readings 123. ").getBytes());
            string response = new string();
            ko r = rpc_daemon.get_peer().call_timeseries_list(response);
            if (is_ko(r)) {
                System.err.println("Error: " + rpc_daemon.rewrite(r));
                System.exit(1);
            }
            System.out.println(response.value);
        }



        {
            hash_t account = new hash_t("XEiGLZ9Xq8Kzp4NSMk553J6kPe1");
            System.out.println("\nStoring a piece of data in the private section of address:");
            bin_t data = new bin_t(new String("Dummy Readings 123. ").getBytes());
            string response = new string();
            ko r = rpc_daemon.get_peer().call_timeseries_add(new us.wallet.cli.rpc_peer_t.timeseries_add_in_t(account, data), response);
            if (is_ko(r)) {
                System.err.println("Error: " + rpc_daemon.rewrite(r));
                System.exit(1);
            }
            System.out.println(response.value);
        }



        System.out.println("shutting down");
        shut_down();
        return ok;
    }

}

