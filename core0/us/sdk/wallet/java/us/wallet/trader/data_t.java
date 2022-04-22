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
package us.wallet.trader;
import java.io.BufferedReader;                                                                 // BufferedReader
import java.io.ByteArrayInputStream;                                                           // ByteArrayInputStream
import java.io.ByteArrayOutputStream;                                                          // ByteArrayOutputStream
import us.CFG;                                                                                 // CFG
import java.util.HashMap;                                                                      // HashMap
import us.stdint.*;                                                                            // *
import java.io.InputStream;                                                                    // InputStream
import java.io.InputStreamReader;                                                              // InputStreamReader
import static us.ko.is_ko;                                                                     // is_ko
import us.ko;                                                                                  // ko
import java.util.Map;                                                                          // Map
import static us.ko.ok;                                                                        // ok
import java.io.OutputStream;                                                                   // OutputStream
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import java.util.Scanner;                                                                      // Scanner
import java.util.TreeMap;                                                                      // TreeMap

@SuppressWarnings("serial")
public class data_t extends TreeMap<String, String> {

    static void log(final String line) {           //--strip
       CFG.log_wallet_trader("data_t: " + line);   //--strip
    }                                              //--strip

    public String find(String key) {
        return get(key);
    }

    public void emplace(String k, String v) {
        put(k, v);
    }

    public ko from(String s) {
        log("from string: " + s); //--strip
        InputStream is = new ByteArrayInputStream(s.getBytes());
        return from(is);
    }

    public ko from(InputStream is) {
        log("from stream"); //--strip
        clear();
        try (BufferedReader reader = new BufferedReader(new InputStreamReader(is))) {
            while (reader.ready()) {
                String line = reader.readLine();
                if (line.isEmpty()) continue;
                String[] kv = line.split(" ", 2);
                if (kv.length != 2) return new ko("KO 50045");
                if (kv[0].isEmpty()) {
                    clear();
                    return new ko("KO 66058");
                }
                put(kv[0], kv[1]);
            }
        }
        catch (Exception e) {
        }
        log("size " + size()); //--strip
        return ok;
    }

    public void dump(final String pfx, PrintStream os) {
        dump_(pfx, os);
    }

    public void dump_(final String pfx, PrintStream os) {
        for (Map.Entry<String, String> entry: entrySet()) {
            os.println(pfx + "'" + entry.getKey() + "': " + entry.getValue());
        }
    }

}

