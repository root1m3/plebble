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
import android.Manifest;                                                                       // Manifest
import static us.ko.ok;                                                                        // ok
import android.content.pm.PackageManager;                                                      // PackageManager
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import java.security.PrivateKey;                                                               // PrivateKey
import us.wallet.trader.qr_t;                                                                  // qr_t
import java.nio.charset.StandardCharsets;                                                      // StandardCharsets
import java.lang.StringBuilder;                                                                // StringBuilder
import us.string;                                                                              // string
import java.io.UnsupportedEncodingException;                                                   // UnsupportedEncodingException

public class cfg_android_public_t extends us.gov.io.cfg0 {

    public static ko KO_97033 = new ko("KO 97033 Cannot write file.");
    public static ko KO_97034 = new ko("KO 97034 Cannot read file.");

    private static void log(final String line) {                    //--strip
        CFG.log_android("cfg_android_public_t: " + line);           //--strip
    }                                                               //--strip

    public cfg_android_public_t(app a_, String home_) {
        super(home_);
//        log("ctx.getDataDir " + ctx.getDataDirFile().getCanonicalPath()); //--strip
//        filepfx = fnpath(home);
        a = a_;
        log("new cfg_android_public_t home " + home); //--strip
        //log("Private storage  " + a.getApplicationContext().getFilesDir().getPath()); //--strip
    }

    private cfg_android_public_t(cfg_android_public_t other) {
        super(other);
        a = other.a;
//        filepfx = other.filepfx;
    }

    static class downloads_dir_lock_t extends Object {}
    static public downloads_dir_lock_t downloads_dir_lock_t = new downloads_dir_lock_t();

    public File get_download_file(String file) {
        log("file " + file); //--strip
        log("Environment.getExternalStorageDirectory()=" + Environment.getExternalStorageDirectory()); //--strip
        File f = new File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS) + "/" + CFG.blob_id, file);
        try {
            log("canonicalpath: " + f.getCanonicalPath()); //--strip
            File parent = f.getParentFile();
            if (parent != null && !parent.exists() && !parent.mkdirs()) {
                ko r = new ko("KO 76905 Cannot create dir.");
                log(r.msg + " " + parent.getCanonicalPath()); //--strip
                return null;                    
            }
            log("parent.exists()=" + parent.exists() + " " + parent.getCanonicalPath()); //--strip
            return f;
        }
        catch (Exception e) {
            log("Ex. " + e.getMessage()); //--strip
        }
        return null;
    }

    public boolean is_external_storage_writable() {
        return Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED);
    }

    public boolean is_external_storage_readable() { // Checks if a volume containing external storage is available to at least read.
         return Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED) || Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED_READ_ONLY);
    }

    public File public_file(String file) {
        try {
            return get_download_file(file);
        }
        catch (Exception e) {
            return null;
        }
    }

    public boolean public_file_exists(String file) {
        try {
            File f = get_download_file(file);
            if (f == null) return false;
            return f.exists();
        }
        catch (Exception e) {
            return false;
        }
    }

    public void delete_public_file(String file) {
        try {
            File f = get_download_file(file);
            if (f != null) f.delete();
        }
        catch (Exception e) {
        }
    }

    public ko write_public_file(String file, byte[] content) {
        log("write_public_file " + file + " sz:" + content.length); //--strip
        try {
            File f = get_download_file(file.trim());
            log("A1 + " + f.getCanonicalPath()); //--strip
            if (!f.createNewFile()) {
                log("createNewFile didn't create the file"); //--strip
            }
            log("A2"); //--strip
            FileOutputStream os = new FileOutputStream(f);
            log("A3"); //--strip
            os.write(content);
            log("A4"); //--strip
            log("Writing public file " + file + " sz=" + content.length); //--strip
            os.flush();
            os.close();
            log("A5"); //--strip
        }
        catch (Exception e) {
            log(KO_97033.msg + ". " + file + ". " + e.getMessage()); //--strip
            return KO_97033;
        }
        return ok;
    }

/*
    public static String fnpath(String path) {
        log("encoding dir in the filename, because android SDK sucks with directory trees."); //--strip
        return path.replaceAll('/','_').trim(); 
    }
*/
/*
    public static pair<ko, cfg_android_public_t> load(Context ctx, String home) {
        return new pair<ko, cfg_android_public_t>(ok, new cfg_android_public_t(ctx, home));
    }
*/
    app a;
//    String filepfx = null;
}

