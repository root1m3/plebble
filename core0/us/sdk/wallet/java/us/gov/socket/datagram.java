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
package us.gov.socket;
import java.util.ArrayList;                                                                    // ArrayList
import org.bouncycastle.util.Arrays;                                                           // Arrays
import us.gov.crypto.base58;                                                                   // base58
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import java.io.BufferedReader;                                                                 // BufferedReader
import java.nio.ByteBuffer;                                                                    // ByteBuffer
import java.nio.ByteOrder;                                                                     // ByteOrder
import us.CFG;                                                                                 // CFG
import java.io.DataInputStream;                                                                // DataInputStream
import java.io.DataOutputStream;                                                               // DataOutputStream
import java.security.GeneralSecurityException;                                                 // GeneralSecurityException
import us.gov.crypto.ripemd160.hash_t;                                                         // hash_t
import static us.gov.socket.types.*;                                                           // *
import us.stdint.*;                                                                            // *
import us.tuple.*;                                                                             // *
import java.io.InputStream;                                                                    // InputStream
import java.io.InputStreamReader;                                                              // InputStreamReader
import java.io.IOException;                                                                    // IOException
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import java.io.OutputStream;                                                                   // OutputStream
import us.pair;                                                                                // pair
import java.io.PrintWriter;                                                                    // PrintWriter
import java.security.PrivateKey;                                                               // PrivateKey
import java.security.PublicKey;                                                                // PublicKey
import us.gov.crypto.ripemd160;                                                                // ripemd160
import us.gov.crypto.sha256;                                                                   // sha256
import java.net.Socket;                                                                        // Socket
import us.gov.crypto.symmetric_encryption;                                                     // symmetric_encryption

public class datagram {
    static final int offset_channel = 0;
    static final int size_channel = 2;
    static final int offset_size = offset_channel+size_channel;
    static final int size_size = 4;
    static final int offset_service = offset_size+size_size;
    static final int size_service = 2;
    static final int offset_sequence = offset_service+size_service;
    static final int size_sequence = 2;

    public static final int h = size_channel + size_size + size_service + size_sequence;
    public static final int maxsize = CFG.MAX_SIZE_DATAGRAM;

    public static final ko KO_0001 = new ko("KO 0001 channel mismatch.");
    public static final ko KO_0002 = new ko("KO 0002 too big.");
    public static final ko KO_0003 = new ko("KO 0003 too small.");
    public static final ko KO_4038 = new ko("KO 5933 invalid size");
    public static final ko KO_4921 = new ko("KO 4921 ::recv<0");
    public static final ko KO_9021 = new ko("KO 9021 ::recv 0");
    public static final ko KO_20292 = new ko("KO 20292 Size would buffer overflow.");

    public static interface dispatcher_t {
        boolean dispatch(datagram d); //return true is datagram has been processed
    }

    static void log(final String line) {                            //--strip
        CFG.log_gov_socket("datagram: " + line);                    //--strip
    }                                                               //--strip

    public datagram(final channel_t channel, int mode) {
        switch(mode) {
            case 0: {  //used in encryption
                bytes = new byte[h];
                encode_channel(channel);
                //encode_size(sz);
                service = new svc_t(0);
                encode_service(service);
                encode_sequence(new seq_t(0));
            }
            break;
            case 1: { //used in decryption
            }
            break;
            case 2: { //used in recv
                bytes = new byte[h];
                dend = 0;
            }
            break;
        }
    }


    public datagram(final channel_t channel_, final svc_t service_, final seq_t sequence, int payload_sz) {
        assert channel_ != null;
        assert service_ != null;
        assert sequence != null;
        bytes = new byte[h + payload_sz];
        service = service_;
        encode_channel(channel_);
        encode_size(h + payload_sz);
        encode_service(service);
        encode_sequence(sequence);
        dend = bytes.length;
        log("new datagram svc" + service.value + " seq " + sequence.value + " sz " + bytes.length + " payload sz " + payload_sz + " ch " + decode_channel().value); //--strip
    }

    public pair<ko, datagram> encrypt(symmetric_encryption se) {
        log("encrypt"); //--strip
        datagram d = new datagram(decode_channel(), 0);
        pair<ko, byte[]> r = se.encrypt(bytes);
        if (ko.is_ko(r.first)) {
            return new pair<ko, datagram>(r.first, null);
        }
        d.bytes = Arrays.concatenate(d.bytes, r.second);
        d.dend = d.bytes.length;
        d.encode_size(d.dend);
        return new pair<ko, datagram>(ok, d);
    }

    public pair<ko, datagram> decrypt(channel_t channel, symmetric_encryption se) {
        if (service.value != 0) {
            ko r = new ko("KO 81920 svc should be 0 for encrypted dgram.");
            log("decrypt dgram " + r.msg + " " + service.value); //--strip
            return new pair<ko, datagram>(r, null);
        }
        log("decrypt dgram " + bytes.length); //--strip
        if (bytes.length < h) {
            ko r = new ko("KO 58948 Invalid dgram.");
            log("decrypt dgram " + r.msg + " " + service.value); //--strip
            return new pair<ko, datagram>(r, null);
        }
        datagram d = new datagram(channel, 1);
        {
            pair<ko, byte[]> r = se.decrypt(bytes, h, bytes.length - h);
            if (ko.is_ko(r.first)) {
                return new pair<ko, datagram>(r.first, null);
            }
            d.bytes = r.second;
        }
        d.dend = d.bytes.length;
        if (d.bytes.length < h) {
            ko r = new ko("KO 25994 Invalid decrypted size.");
            log(r.msg); //--strip
            return new pair<ko, datagram>(r, null);
        }
        channel_t ch = d.decode_channel();
        if (ch.value != channel.value) {
            if (channel.value != all_channels.value) { //accepting any channel
                log(KO_0001.msg + " evil1 enc dgram_ch:" + ch.value + " sys:" + channel.value); //--strip
                return new pair<ko, datagram>(KO_0001, null);
            }
            log("enc dgram arrived targeting ALL channels."); //--strip
        }
        else {
            log("enc dgram arrived targeting my channel. " + ch.value);  //--strip
        }

        int sz = d.decode_size();
        if (sz != d.bytes.length) {
            log(KO_4038.msg + "DGRAM DECRYPT KO 6"); //--strip
            return new pair<ko, datagram>(KO_4038, null);
        }
        if (sz < h) {
            log(KO_4038.msg + "DGRAM DECRYPT KO 7"); //--strip
            return new pair<ko, datagram>(KO_4038, null);
        }
        d.service = d.decode_service();
        return new pair<ko, datagram>(ok, d);
    }

    public boolean validate_payload(byte[] payload) {
        return h + blob_writer_t.blob_size(payload) < maxsize;
    }

    public boolean in_progress() { return dend > 0 && dend < size(); }

    public void encode_channel(channel_t channel) {
        assert bytes.length >= offset_channel + 2;
        log("writing dgram channel " + channel.value); //--strip
        channel.write(bytes, offset_channel);
    }

    public channel_t decode_channel() {
        assert(bytes.length >= 2);
        channel_t o = new channel_t();
        o.read(bytes, offset_channel);
        return o;
    }

    public void encode_size(int sz) {
        log("encoding size=" + sz); //--strip
        assert(bytes.length > offset_size + 4);
        new uint32_t(sz).write(bytes, offset_size);
    }

    public int decode_size() {
        assert bytes.length >= 4;
        return (int)uint32_t.from(bytes, offset_size).value;
    }

    public int size() {
        return bytes.length;
    }

    public void encode_service(svc_t svc) {
        assert bytes.length >= offset_service + 2;
        svc.write(bytes, offset_service);
    }

    public svc_t decode_service() {
        assert bytes.length >= h;
        svc_t svc = new svc_t();
        svc.read(bytes, offset_service);
        return svc;
    }

    public void encode_sequence(seq_t seq) {
        assert(bytes.length >= offset_sequence + 2);
        seq.write(bytes, offset_sequence);
    }

    public seq_t decode_sequence() {
        assert bytes.length >= h;
        seq_t seq = new seq_t();
        seq.read(bytes, offset_sequence);
        return seq;
    }

    public boolean completed() {
        return dend == bytes.length && bytes.length > 0;
    }

    public static int timeout = 3000;

    ko recvfrom(Socket s, channel_t channel, busyled_t busyled) {
        log("recvfrom. ch " + channel.value); //--strip
        log("recvfrom timeout is " + timeout); //--strip
        try {
            log("recvfrom socket t/o: " + s.getSoTimeout()); //--strip
            if (dend < h) {
                log("READ-A"); //--strip
                busyled.reset();
                int nread = s.getInputStream().read(bytes, dend, h - dend);
                busyled.set();
                log("/READ-A"); //--strip
                if (nread < 0) {
                    log(KO_4921.msg+" nread < 0; (" + nread + ")."); //--strip
                    return KO_4921;
                }
                dend += nread;
                if (dend < h) {
                    log("dend is < h " + dend); //--strip
                    return ok;
                }
                channel_t ch = decode_channel();
                if (ch.value != channel.value) {
                    if (channel.value != all_channels.value) { //accepting any channel
                        log(KO_0001.msg + " evil1 dgram_ch:" + ch.value + " sys:" + channel.value); //--strip
                        return KO_0001; //"evil. channel mismatch " + ch + "!=" + system_channel + "(system channel)";
                    }
                    log("enc dgram arrived targeting ALL channels."); //--strip
                }
                else {
                    log("enc dgram arrived targeting my channel. " + ch.value);  //--strip
                }
                int sz = decode_size();
                if (sz > maxsize) {
                    log("evil2"); //--strip
                    return KO_0002; //"evil. too big";
                }
                if (sz < h) {
                    log("evil3"); //--strip
                    return KO_0003; //"evil. too small";
                }
                byte[] m = new byte[sz];
                System.arraycopy(bytes, 0, m, 0, h);
                bytes = m;
                service = decode_service();
                if (dend == sz) {
                    log("return Completed OK dend=" + dend); //--strip
                    return ok;
                }
                log("Received header for dgram sz=" + sz + " svc=" + service.value);  //--strip
            }
            log("READ-B " + (bytes.length - dend)); //--strip
            busyled.reset();
            int nread = s.getInputStream().read(bytes, dend, bytes.length - dend);  //-1 if there is no more data because the end of the stream has been reached. https://docs.oracle.com/javase/7/docs/api/java/io/InputStream.html#read()
            busyled.set();
            log("/READ-B"); //--strip
            if (nread < 0) {
                log(KO_4921.msg + " nread < 0; (" + nread + ")."); //--strip
                return KO_4921;
            }
            dend += nread;
            log("return OK read chunk " + nread + " dend=" + dend); //--strip
            return ok;
        }
        catch(java.net.SocketTimeoutException e) {
            log("KO 6912 - Timeout Exception " + e.getMessage()); //--strip
            return new ko("KO 6912 " + e.getMessage());
        }
        catch(IOException e) {
            log("KO 6251 - IOException " + e.getMessage()); //--strip
            return new ko("KO 6251 " + e.getMessage());
        }
        catch(Exception e) {
            log("KO 6651 - Exception " + e.getMessage()); //--strip
            return new ko("KO 6651 " + e.getMessage());
        }
    }

    public hash_t compute_hash() {
        return ripemd160.hash(bytes);
    }

    public hash_t compute_hash_payload() {
        byte[] ed = java.util.Arrays.copyOfRange(bytes, h, bytes.length-h);
        return ripemd160.hash(ed);
    }

    public byte[] bytes;
    //public static channel_t system_channel = CFG.CHANNEL;
    public static channel_t all_channels = new channel_t(65535);
    public svc_t service;
    public int dend;
}

