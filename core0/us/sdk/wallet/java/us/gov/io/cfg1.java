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
import static us.gov.socket.types.channel_t;                                                   // channel_t
import java.nio.charset.Charset;                                                               // Charset
import us.gov.crypto.ec;                                                                       // ec
import java.io.File;                                                                           // File
import java.io.FileOutputStream;                                                               // FileOutputStream
import java.nio.file.Files;                                                                    // Files
import java.util.HashSet;                                                                      // HashSet
import java.io.IOException;                                                                    // IOException
import java.security.KeyPair;                                                                  // KeyPair
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import java.nio.file.Paths;                                                                    // Paths
import java.nio.file.attribute.PosixFilePermission;                                            // PosixFilePermission
import java.nio.file.attribute.PosixFilePermissions;                                           // PosixFilePermissions
import java.security.PrivateKey;                                                               // PrivateKey
import java.util.Set;                                                                          // Set

public class cfg1 extends cfg0 {

    private static void log(final String line) {            //--strip
        CFG.log_gov_io("cfg1: " + line);                    //--strip
    }                                                       //--strip

    public static final ko KO_30291 = new ko("KO 30291 invalid k file.");
    public static final ko KO_97832 = new ko("KO 97832 Secret key not found.");
    public static final ko KO_96857 = new ko("KO 96857 Invalid private key.");
    public static final ko KO_50493 = new ko("KO 50493 Cannot create home dir.");
    public static final ko KO_97032 = new ko("KO 97032 Cannot create k file.");
    public static final ko KO_60593 = new ko("KO 60593 Failure generating keys.");
    public static final ko KO_86059 = new ko("KO 86059 Unable to set posix permissions.");

    public cfg1(PrivateKey privk, String home_) {
        super(home_);
        keys = ec.instance.generate_keypair(privk);
        if (keys == null) {
            System.err.println("KO 60987 Invalid privkey");
            System.exit(1);
        }
    }

    public cfg1(String k_b58, String home_) {
        super(home_);
        if (k_b58 != null && !k_b58.isEmpty()) {
            keys = ec.instance.generate_keypair(k_b58);
        }
        else {
            keys = ec.instance.generate_keypair();
        }
        if (keys == null) {
            System.err.println("KO 60987 Invalid privkey");
            System.exit(1);
        }
    }

    public cfg1() {
        super("");
        keys = ec.instance.generate_keypair();
        if (keys == null) {
            System.err.println("KO 60988 Invalid privkey");
            System.exit(1);
        }
    }

    public cfg1(cfg1 other) {
        super(other);
        keys = other.keys;
    }

    public static String k_filename() {
        return "k";
    }

    public static String k_file(String home) {
        return abs_file(home, k_filename());
    }

    public static ko write_k(String home, PrivateKey priv) {
        log("write_k " + home);  //--strip
        if (!ensure_dir(home)) {
            log(KO_50493.msg + " " + home); //--strip
            return KO_50493;
        }
        String filename = k_filename();
        log("filename " + home + " " + filename); //--strip
        try {
            File file = new File(home, filename);
            if (!file.exists()) {
	            file.createNewFile();
            }
            String content = ec.instance.to_b58(ec.instance.generate_keypair().getPrivate());
            FileOutputStream os = new FileOutputStream(file);
            os.write(content.getBytes());
            os.write('\n');
            os.flush();
            os.close();
        }
        catch (Exception e) {
            log(KO_97032.msg + " " + e.getMessage()); //--strip
            return KO_97032;
        }
        try {
            File file = new File(home, filename);
            Set<PosixFilePermission> perms = new HashSet<>();
            perms.add(PosixFilePermission.OWNER_READ);
            perms.add(PosixFilePermission.OWNER_WRITE);
            Files.setPosixFilePermissions(file.toPath(), perms);
        }
        catch (Exception e) {
            log(KO_86059.msg + " " + e.getMessage() + ". (Ignored)"); //--strip
        }
        return ok;
    }

    public static String read_file(String path, Charset encoding) throws IOException {
        byte[] encoded = Files.readAllBytes(Paths.get(path));
        return new String(encoded, encoding);
    }

    public static pair<ko, PrivateKey> load_sk(String home) {
        log("load_sk " + home); //--strip
        try {
            String keyfile = k_file(home);
            log("keyfile " + keyfile); //--strip
            String content = read_file(keyfile, Charset.defaultCharset()).trim();
            PrivateKey privateKey = ec.instance.get_private_key(base58.decode(content));
            return new pair<ko, PrivateKey>(ok, privateKey);
        }
        catch(Exception e) {
            log(KO_96857.msg + " " + e.getMessage()); //--strip
            return new pair<ko, PrivateKey>(KO_96857, null);
        }
    }

    public static pair<ko, cfg1> load(final channel_t channel, String home, boolean gen) {
        log("load " + home + " " + gen); //--strip
        String keyfile = k_file(home);
        if (!file_exists(keyfile)) {
            log("keyfile " + keyfile + " not found."); //--strip
            if (!gen) {
                return new pair<ko, cfg1>(KO_97832, null);
            }
            log("generating new keys at " + home); //--strip
            KeyPair k;
            try {
                k = ec.instance.generate_keypair();
            }
            catch (Exception e) {
                log(KO_60593.msg + " " + e.getMessage()); //--strip
                return new pair<ko, cfg1>(KO_60593, null);
            }
            ko r = write_k(home, k.getPrivate());
            if (ko.is_ko(r)) {
                log(r.msg); //--strip
                return new pair<ko, cfg1>(r, null);
            }
        }
        pair<ko, cfg0> x = cfg0.load(home);
        if (ko.is_ko(x.first)) {
            return new pair<ko, cfg1>(x.first, null);
        }
        pair<ko, PrivateKey> pk = load_sk(home);
        if (ko.is_ko(pk.first)) {
            return new pair<ko, cfg1>(pk.first, null);
        }
        return new pair<ko, cfg1>(ok, new cfg1(pk.second, x.second.home));
    }

    public KeyPair keys;
}

