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
package us.gov.crypto;
import us.CFG;                                                                                 // CFG
import java.io.*;                                                                              // *
import java.security.*;                                                                        // *

 /*
 * Copyright 2013 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * A SecureRandom implementation that is able to override the standard JVM provided implementation, and which simply
 * serves random numbers by reading /dev/urandom. That is, it delegates to the kernel on UNIX systems and is unusable on
 * other platforms. Attempts to manually set the seed are ignored. There is no difference between seed bytes and
 * non-seed bytes, they are all from the same source.
 */
@SuppressWarnings("serial")
public class linux_secure_random extends SecureRandomSpi {

    static void log(final String line) {                                      //--strip
        CFG.log_gov_crypto("linux_secure_random: " + line);                   //--strip
    }                                                                         //--strip

    private static final FileInputStream urandom;

    @SuppressWarnings({"serial", "deprecation"})
    public static class provider extends Provider {

        public provider() {
            super("linux_secure_random", 1.0, "A Linux specific random number provider that uses /dev/urandom");
            put("SecureRandom.linux_secure_random", linux_secure_random.class.getName());
        }
    }

    static {
        log("Adding RNG provider /dev/urandom."); //--strip
        try {
            File file = new File("/dev/urandom");
            urandom = new FileInputStream(file);
            if (urandom.read() == -1) {
                throw new RuntimeException("/dev/urandom not readable?");
            }
            // Now override the default SecureRandom implementation with this one.
            int position = Security.insertProviderAt(new provider(), 1);
            log("Added RNG provider /dev/urandom."); //--strip
        }
        catch (FileNotFoundException e) { // Should never happen.
            log("/dev/urandom does not appear to exist or is not openable"); //--strip
            throw new RuntimeException(e);
        }
        catch (IOException e) {
            log("/dev/urandom does not appear to be readable"); //--strip
            throw new RuntimeException(e);
        }
    }

    private final DataInputStream dis;

    public linux_secure_random() {
        // DataInputStream is not thread safe, so each random object has its own.
        dis = new DataInputStream(urandom);
    }

    @Override
    protected void engineSetSeed(byte[] bytes) {
        // Ignore.
    }

    @Override
    protected void engineNextBytes(byte[] bytes) {
        try {
            log("Reading random bytes " + bytes.length); //--strip
            dis.readFully(bytes); // This will block until all the bytes can be read.
        }
        catch (IOException e) {
            throw new RuntimeException(e); // Fatal error. Do not attempt to recover from this.
        }
    }

    @Override
    protected byte[] engineGenerateSeed(int i) {
        byte[] bits = new byte[i];
        engineNextBytes(bits);
        return bits;
    }
}

