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
import us.CFG;                                                                                 // CFG
import java.io.File;                                                                           // File
import java.nio.file.Files;                                                                    // Files
import static us.gov.io.types.*;                                                               // *
import java.io.IOException;                                                                    // IOException
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import java.nio.file.Paths;                                                                    // Paths
import java.nio.charset.StandardCharsets;                                                      // StandardCharsets

public class cfg0 {

    static private void log(final String line) {    //--strip
        CFG.log_gov_io("cfg0: " + line);            //--strip
    }                                               //--strip

    public cfg0(String home_) {
        home = home_;
    }

    public cfg0(cfg0 other) {
        home = other.home;
    }

    public static void check_platform() {
    }

    public static String abs_file(String home, String fn) {
        return home + "/" + fn;
    }

    public static boolean dir_exists(String d) {
        File f = new File(d);
        if (f.exists() && f.isDirectory()) {
           return true;
        }
        return false;
    }

    public static boolean file_exists(final String s) {
        File f = new File(s);
        if (f.exists() && !f.isDirectory()) {
           return true;
        }
        return false;
    }

    public static boolean ensure_dir(String d) {
        try {
            Files.createDirectories(Paths.get(d));
        }
        catch (IOException e) {
            System.err.println("KO 60983 Creating directory "+d);
            return false;
        }
        return true;
    }

    public static final ko KO_60534 = new ko("KO 60534 Cannot create home dir.");

    public static pair<ko, cfg0> load(String home) {
        check_platform();
        if (!ensure_dir(home)) {
             log(KO_60534.msg + " " + home); //--strip
             return new pair<ko, cfg0>(KO_60534, null);
        }
        return new pair<ko, cfg0>(ok, new cfg0(home));
    }

    public static String rewrite_path(String d) {
        String v = "";
        boolean newp = false;
        int i = 0;
        while (i < d.length()) {
            char c = d.charAt(i);
            if (c == '/') {
                newp = true;
                ++i;
                continue;
            }
            boolean ok = false;
            if (c >= 'a' && c <= 'z')
                ok = true;
            else if (c >= 'A' && c <= 'Z')
                ok = true;
            else if (c >= '0' && c <= '9')
                ok = true;
            if (!ok) {
                ++i; //ignored char
                continue;
            }
            if (newp) {
                newp = false;
                if (!v.isEmpty()) {
                    v += '/';
                }
            }
            v += c;
            ++i;
        }
        return v;
    }

    public static byte[] read_file(String filename) {
        try {
            File f = new File(filename);
            byte[] content = Files.readAllBytes(f.toPath());
            return content;
        }
        catch (Exception e) {
            return null;
        }
    }

    public static String read_text_file(String filename) {
        String content;
        try {
            content = new String(Files.readAllBytes(Paths.get(filename)));
        }
        catch (IOException e) {
            content = null;
        }
        return content;
    }

    public static ko read_file_(final String filename, blob_t blob) {
        blob.value = read_file(filename);
        if (blob.value == null) {
            ko r = new ko("KO 84010 Likely the file doesn't exist in the local filesystem.");
            log(r.msg); //--strip
            return r;
        }
        return ok;
    }

    public static ko read_file_(final String filename, bin_t bin) {
        bin.value = read_file(filename);
        if (bin.value == null) {
            ko r = new ko("KO 84011 Likely the file doesn't exist in the local filesystem.");
            log(r.msg); //--strip
            return r;
        }
        return ok;
    }

    public static ko write_file_(final byte[] content, final String filename) {
        try {
            Files.write(Paths.get(filename), content);
            return ok;
        }
        catch (IOException e) {
            e.printStackTrace();
        }
        ko r = new ko("KO 65994 Error writing file");
        log(r.msg); //--strip
        return r;
    }

    public String home;
}

