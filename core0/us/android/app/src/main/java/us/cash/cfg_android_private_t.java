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
import java.util.ArrayList;                                                                    // ArrayList
import java.util.Arrays;                                                                       // Arrays
import us.gov.crypto.base58;                                                                   // base58
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import us.wallet.trader.bookmarks_t;                                                           // bookmarks_t
import us.wallet.trader.bookmark_t;                                                            // bookmark_t
import java.io.BufferedReader;                                                                 // BufferedReader
import us.gov.socket.busyled_t;                                                                // busyled_t
import java.io.ByteArrayOutputStream;                                                          // ByteArrayOutputStream
import android.graphics.drawable.ColorDrawable;                                                // ColorDrawable
import android.content.Context;                                                                // Context
import us.gov.socket.datagram;                                                                 // datagram
import us.gov.crypto.ec;                                                                       // ec
import android.os.Environment;                                                                 // Environment
import java.io.File;                                                                           // File
import java.io.FileInputStream;                                                                // FileInputStream
import java.io.FileOutputStream;                                                               // FileOutputStream
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.gov.id.types.*;                                                               // *
import static us.gov.io.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import java.io.InputStreamReader;                                                              // InputStreamReader
import java.io.IOException;                                                                    // IOException
import static us.ko.is_ko;                                                                     // is_ko
import java.security.KeyPair;                                                                  // KeyPair
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import java.security.PrivateKey;                                                               // PrivateKey
import us.wallet.trader.qr_t;                                                                  // qr_t
import java.nio.charset.StandardCharsets;                                                      // StandardCharsets
import java.lang.StringBuilder;                                                                // StringBuilder
import us.string;                                                                              // string
import java.io.UnsupportedEncodingException;                                                   // UnsupportedEncodingException

public class cfg_android_private_t extends us.gov.io.cfg1 {

    public static ko KO_97033 = new ko("KO 97033 Cannot write file.");
    public static ko KO_97034 = new ko("KO 97034 Cannot read file.");

    private static void log(final String line) {                      //--strip
        CFG.log_android("cfg_android_private_t: " + line);            //--strip
    }                                                                 //--strip

    public cfg_android_private_t(Context ctx_, PrivateKey privk, String home_) {
        super(privk, home_);
        if (!home.isEmpty()) {
            while (home.substring(0, 1).equals("/")) {
                home = home.substring(1);
            }
        }
        log("home is '" + home + "'"); //--strip
        ctx = ctx_;
        log("new cfg_android_private_t home " + home); //--strip
    }

    public cfg_android_private_t(Context ctx_, String privk_encoded, String home_) {
        super(privk_encoded, home_);
        if (!home.isEmpty()) {
            while (home.substring(0, 1).equals("/")) {
                home = home.substring(1);
            }
        }
        log("home is '" + home + "'"); //--strip
        ctx = ctx_;
        log("new cfg_android_private_t home " + home); //--strip
    }

    public cfg_android_private_t(Context ctx_, String privk_encoded) {
        super(privk_encoded, "");
        ctx = ctx_;
    }

    public cfg_android_private_t(Context ctx_) {
        ctx = ctx_;
    }

    private cfg_android_private_t(cfg_android_private_t other) {
        super(other);
        keys = ec.instance.generate_keypair();
        ctx = other.ctx;
    }

    public void delete_file(final String file) {
        String filename = filecode(home, file);
        log("delete file " + filename); //--strip
        ctx.deleteFile(filename);
        log("delete_file " + file + " " + filename); //--strip
    }

    public static void delete_file(Context ctx, final String home, final String file) {
        String filename = filecode(home, file);
        log("delete file " + filename); //--strip
        ctx.deleteFile(filename);
        log("delete_file " + file + " " + filename); //--strip
    }

    public File private_file(final String file) {
        String filename = filecode(home, file);
        log("private file " + filename); //--strip
        return ctx.getFileStreamPath(filename);
    }

    public ko write_file(final String file, byte[] content) {
        try {
            File f = private_file(file);
            FileOutputStream os = new FileOutputStream(f);
            log("Writing sz=" + content.length); //--strip
            os.write(content);
            os.flush();
            os.close();
            return ok;
        }
        catch (Exception e) {
            log("Ex. " + e.getMessage()); //--strip
            return new ko("KO 89540 ");
        }
    }

    public ko write_file(final String file, final String content) {
        return write_file(file, content.getBytes());
    }

    public static ko write_file(Context ctx, String home, String name, byte[] content) {
        String filename = filecode(home, name);
        try {
            File f = ctx.getFileStreamPath(filename);
            FileOutputStream os = new FileOutputStream(f);
            log("Writing sz=" + content.length); //--strip
            os.write(content);
            os.flush();
            os.close();
            return ok;
        }
        catch (Exception e) {
            log("Ex. " + e.getMessage()); //--strip
            return new ko("KO 89540 ");
        }
    }

    public static ko write_file_string(Context ctx, final String home, final String name, final String content) {
        return write_file(ctx, home, name, content.getBytes());
    }

    public static ko write_k(Context ctx, String home, PrivateKey priv) {
        String filename = k_filename();
        log("write_k as " + filename);  //--strip
        return write_file(ctx, home, filename, us.gov.crypto.ec.instance.to_b58(priv).getBytes());
    }

    public static String read_file_string(Context ctx, final String home, final String file) {
        String filename = filecode(home, file);
        log("read_file_string " + home + ' ' + file + " filename " + filename); //--strip
        byte[] content = read_file(ctx, home, file);
        if (content == null) return "";
        return new String(content, StandardCharsets.UTF_8);
    }

    public String read_file_string(final String file) {
        byte[] content = read_file2(file);
        if (content == null) return "";
        return new String(content, StandardCharsets.UTF_8);
    }

    public static byte[] read_file(Context ctx, final String home, final String file) {
        String filename = filecode(home, file);
        log("read_file " + home + ' ' + file + " filename " + filename); //--strip
        try {
            FileInputStream is = ctx.openFileInput(filename);
            long sz = is.getChannel().size();
            byte content[] = new byte[(int)sz];
            is.read(content);
            is.close();
            log("returning " + content.length + " bytes"); //--strip
            return content;
        }
        catch (Exception e) {
            log(KO_97034.msg + " " + file + ". " + e.getMessage()); //--strip
            return null;
        }
    }

    public byte[] read_file2(String file) {
        String filename = filecode(home, file);
        log("read_file2 " + filename); //--strip
        try {
            FileInputStream is = ctx.openFileInput(filename);
            long sz = is.getChannel().size();
            byte content[] = new byte[(int)sz];
            is.read(content);
            is.close();
            log("returning " + content.length + " bytes"); //--strip
            return content;
        }
        catch (Exception e) {
            log(KO_97034.msg + " " + file + ". " + e.getMessage()); //--strip
            return null;
        }
    }

    public static String fnpath(String path) {
        String r = path.replaceAll("/", "_").trim();
        log(path + " => " + r); //--strip
        return r; 
    }

    static String filecode(final String home, final String file) {
        if (home.isEmpty()) {
            return fnpath(file);
        }
        return fnpath(home + '/' + file);
    }

    public boolean file_exists2(final String file) {
        return file_exists2(ctx, home, file);
    }

    public static boolean file_exists2(Context ctx, final String home, final String file) {
        log("file_exists2 home " + home + " file " + file); //--strip
        String filename = filecode(home, file);
        log("filename " + filename); //--strip
        String[] f = ctx.fileList();
        log("ctx.fileList: "); //--strip
        boolean e = false;
        for (int i = 0; i < f.length; ++i) {
            log("    File: " + f[i]); //--strip
            if (filename.equals(f[i])) {
                e = true;
                log("           (^found)"); //--strip
            }
        }
        return e;
    }

    Context ctx;
}

