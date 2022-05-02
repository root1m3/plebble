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
package us.gov.io;
import us.gov.crypto.base58;                                                                   // base58
import us.CFG;                                                                                 // CFG
import us.gov.crypto.ec;                                                                       // ec
import us.gov.crypto.ripemd160.hash_t;                                                         // hash_t
import static us.gov.cash.types.*;                                                             // *
import us.stdint.*;                                                                            // *
import java.io.InputStream;                                                                    // InputStream
import java.io.PrintStream;                                                                    // PrintStream
import java.security.PrivateKey;                                                               // PrivateKey
import java.security.PublicKey;                                                                // PublicKey

public class shell_args {
    public static void log(final String line) {     //--strip
        CFG.log_gov_io("shell_args: " + line);      //--strip
    }                                               //--strip

    public shell_args(String[] args0) {
        args = new String[args0.length + 1];
        int i = 1;
        for (String s: args0) {
            args[i++] = s;
        }
        args[0] = CFG.PLATFORM + "-walletj";
    }

    public shell_args(shell_args other) {
        args = other.args;
        n = other.n;
    }

    public shell_args(String args0) {
        if (args0.trim().isEmpty()) {
            args = new String[1];
        }
        else {
            String[] args1 = args0.split("\\s");
            args = new String[args1.length + 1];
            int i = 1;
            for (String s: args1) {
                args[i++] = s;
            }
        }
        args[0] = CFG.PLATFORM + "-walletj";
    }

    public hash_t next_token() {
        if (n >= args.length) {
            return null;
        }
        String i = args[n++];
        if (i.equals(CFG.LGAS) || i.equals(CFG.UGAS)) i = "11111111111111111111";
        return hash_t.decode(i);
    }

    public hash_t next_hash() {
        if (n >= args.length) {
            return null;
        }
        String i = args[n++];
        return hash_t.decode(i);
    }

    public PrivateKey next_priv() {
        if (n >= args.length) {
            return null;
        }
        try {
            String i = args[n++];
            byte[] k = base58.decode(i);
            return ec.instance.get_private_key(k);
        }
        catch (Exception e) {
            return null;
        }
    }

    public PublicKey next_pub() {
        if (n >= args.length) {
            return null;
        }
        try {
            String i = args[n++];
            byte[] k = base58.decode(i);
            return ec.instance.get_public_key(k);
        }
        catch (Exception e) {
            return null;
        }
    }

    public byte[] next_priv_raw() {
        if (n >= args.length) {
            return null;
        }
        String i = args[n++];
        return base58.decode(i);
    }

    public byte[] next_pub_raw() {
        if (n >= args.length) {
            return null;
        }
        String i = args[n++];
        return base58.decode(i);
    }

    public cash_t next_cash(cash_t default_value) {
        if (n >= args.length) {
            return default_value;
        }
        try {
            return new cash_t(Long.parseLong(args[n++]));
        }
        catch (Exception e) {
            return default_value;
        }
    }

    public cash_t next_cash() {
        return next_cash(new cash_t(0L));
    }

    public String next_string(String default_value) {
        if (n >= args.length) {
            return default_value;
        }
        return args[n++];
    }

    public String next_string() {
        return next_string("");
    }

    public uint16_t next_uint16(uint16_t default_value) {
        if (n >= args.length) {
            return default_value;
        }
        try {
            return new uint16_t(Integer.parseInt(args[n++]));
        }
        catch (Exception e) {
            return default_value;
        }
    }

    public uint16_t next_uint16() {
        return next_uint16(new uint16_t(0));
    }

    public uint32_t next_uint32(uint32_t default_value) {
        if (n >= args.length) {
            return default_value;
        }
        try {
            return new uint32_t(Long.parseLong(args[n++]));
        }
        catch (Exception e) {
            return default_value;
        }
    }

    public uint32_t next_uint32() {
        return next_uint32(new uint32_t(0L));
    }

    public int64_t next_int64(int64_t default_value) {
        if (n >= args.length) {
            return default_value;
        }
        try {
            return new int64_t(Long.parseLong(args[n++]));
        }
        catch (Exception e) {
            return default_value;
        }
    }

    public int64_t next_int64() {
        return next_int64(new int64_t(0L));
    }

    public uint64_t next_uint64(uint64_t default_value) {
        if (n >= args.length) {
            return default_value;
        }
        try {
            return new uint64_t(Long.parseLong(args[n++]));
        }
        catch (Exception e) {
            return default_value;
        }
    }

    public uint64_t next_uint64() {
        return next_uint64(new uint64_t(0L));
    }

    public String next_line() {
        String line = "";
        while(n<args.length) {
            line += args[n++] + ' ';
        }
        return line.trim();
    }

    public int args_left() {
        return args.length - n;
    }

    public void dump(String prefix, PrintStream os) {
        os.println(prefix + "argc: " + args.length);
        os.print(prefix + "argv: ");
        int m = 0;
        while(m < args.length) {
            os.print(args[m++] + ' ');
        }
        os.println();
        os.println(prefix + "n: " + n);
    }

    public void dump(PrintStream os) {
        dump("", os);
    }

    public String[] args;
    public int n = 1;
}

