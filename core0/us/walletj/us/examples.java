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
package us;
import us.wallet.cli.hmi;
import us.ko;                                                                                  // ko
import static us.ko.*;                                                                        // ok
import java.util.TreeMap;
import java.util.Map;
import us.example.example_base;

public class examples extends TreeMap<String, example_base> {

    public examples() {
        put("1", new us.example.api_balance());        
        put("2", new us.example.timeseries_add());        
    }

    public void print() {
        for(Map.Entry<String, example_base> entry : entrySet()) {
            System.out.println(entry.getKey() + " - " + entry.getValue().description());
        }
    }

    public static void main(String[] args)  {
        us.gov.crypto.ec.create_instance();
        examples all = new examples();
        if (args.length < 1) {
            System.err.println("Missing example id. Valid ids are:");
            all.print();
            System.exit(1);
        }
        example_base x = all.get(args[0]);
        if (x == null) {
            System.err.println("Invalid example id. Valid ids are:");
            all.print();
            System.exit(1);
        }
        //us.CFG.logs.set(true);
        ko r = x.run(args);
        if (is_ko(r)) {
            System.exit(1);
        }
    }

}

