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
import us.string;

public class api_balance extends example_base {

    @Override public String description() {
        return "obtain wallet balance and print via rpc API";
    }

    @Override public ko run(String[] args) {

        bring_up();

        System.out.println("\nWallet balance:");
        uint16_t detailed = new uint16_t(0); //level of detail 0|1|2
        string data = new string();
        ko r = rpc_daemon.get_peer().call_balance(detailed, data);
        if (is_ko(r)) {
            System.err.println("Error: " + r.msg);
            System.exit(1);
        }
        System.out.println(data.value);

        shut_down();
        return ok;
    }

}

