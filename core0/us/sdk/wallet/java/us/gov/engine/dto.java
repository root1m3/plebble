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
package us.gov.engine;
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import us.gov.socket.datagram;                                                                 // datagram
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.gov.socket.types.*;                                                           // *
import static us.ko.*;                                                                         // *
import static us.stdint.*;                                                                     // *
import us.ko;                                                                                  // ko
import us.gov.protocol;                                                                        // protocol
import java.security.PublicKey;                                                                // PublicKey
import us.gov.crypto.sha256;                                                                   // sha256
import us.string;                                                                              // string

public interface dto {

    //OFF#include <us/api/generated/gov/java/engine/hdlr_dto-hdr>
    //OFF#include <us/api/generated/gov/java/engine/cllr_dto-hdr>
    //OFF#include <us/api/generated/gov/java/engine_auth/hdlr_dto-hdr>
    //OFF#include <us/api/generated/gov/java/engine_auth/cllr_dto-hdr>
    //OFF#include <us/api/generated/gov/java/cash/hdlr_dto-hdr>
    //OFF#include <us/api/generated/gov/java/cash/cllr_dto-hdr>
    //#include <us/api/generated/gov/java/traders/hdlr_dto-hdr>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/gov/java/traders/hdlr_dto-hdr>
    // traders - master file: us/api/data/gov/traders


    /// lookup_wallet - OUT
    public static final class lookup_wallet_out_t extends blob_writer_t.writable {
        //source: coder_java.cpp::gen_dto_out_hdr (1)

        public lookup_wallet_out_t() {}
        public lookup_wallet_out_t(final uint32_t net_addr, final uint16_t port) {
            this.net_addr = net_addr;
            this.port = port;
        }

        @Override public int blob_size() {
            return blob_writer_t.blob_size(net_addr) +
                blob_writer_t.blob_size(port);
        }

        @Override public void to_blob(blob_writer_t writer) {
            writer.write(net_addr);
            writer.write(port);
        }

        public datagram get_datagram(final channel_t channel, final seq_t seq) {
            return super.get_datagram(channel, new svc_t(protocol.traders_lookup_wallet_response), seq);
        }

        public static datagram get_datagram(final channel_t channel, final seq_t seq, final uint32_t net_addr, final uint16_t port) {
            lookup_wallet_out_t o = new lookup_wallet_out_t(net_addr, port);
            return o.get_datagram(channel, seq);
        }

        public uint32_t net_addr;
        public uint16_t port;
    }

    /// random_wallet - OUT
    public static final class random_wallet_out_t extends blob_writer_t.writable {
        //source: coder_java.cpp::gen_dto_out_hdr (1)

        public random_wallet_out_t() {}
        public random_wallet_out_t(final hash_t addr, final uint32_t net_addr, final uint16_t port) {
            this.addr = addr;
            this.net_addr = net_addr;
            this.port = port;
        }

        @Override public int blob_size() {
            return blob_writer_t.blob_size(addr) +
                blob_writer_t.blob_size(net_addr) +
                blob_writer_t.blob_size(port);
        }

        @Override public void to_blob(blob_writer_t writer) {
            writer.write(addr);
            writer.write(net_addr);
            writer.write(port);
        }

        public datagram get_datagram(final channel_t channel, final seq_t seq) {
            return super.get_datagram(channel, new svc_t(protocol.traders_random_wallet_response), seq);
        }

        public static datagram get_datagram(final channel_t channel, final seq_t seq, final hash_t addr, final uint32_t net_addr, final uint16_t port) {
            random_wallet_out_t o = new random_wallet_out_t(addr, net_addr, port);
            return o.get_datagram(channel, seq);
        }

        public hash_t addr;
        public uint32_t net_addr;
        public uint16_t port;
    }
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.
    //#include <us/api/generated/gov/java/traders/cllr_dto-hdr>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/gov/java/traders/cllr_dto-hdr>
    // traders - master file: us/api/data/gov/traders


    public static final class lookup_wallet_out_dst_t extends blob_reader_t.readable {
        //source: coder_java.cpp::gen_dto_out_hdr (2)

        public lookup_wallet_out_dst_t() {}

        @Override public ko from_blob(blob_reader_t reader) {
            {
                ko r = reader.read(net_addr);
                if (ko.is_ko(r)) return r;
            }
            {
                ko r = reader.read(port);
                if (ko.is_ko(r)) return r;
            }
            return ok;
        }

        public uint32_t net_addr = new uint32_t();
        public uint16_t port = new uint16_t();
    }

    public static final class random_wallet_out_dst_t extends blob_reader_t.readable {
        //source: coder_java.cpp::gen_dto_out_hdr (2)

        public random_wallet_out_dst_t() {}

        @Override public ko from_blob(blob_reader_t reader) {
            {
                ko r = reader.read(addr);
                if (ko.is_ko(r)) return r;
            }
            {
                ko r = reader.read(net_addr);
                if (ko.is_ko(r)) return r;
            }
            {
                ko r = reader.read(port);
                if (ko.is_ko(r)) return r;
            }
            return ok;
        }

        public hash_t addr = new hash_t();
        public uint32_t net_addr = new uint32_t();
        public uint16_t port = new uint16_t();
    }
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.
    //OFF#include <us/api/generated/gov/java/sys/hdlr_dto-hdr>
    //OFF#include <us/api/generated/gov/java/sys/cllr_dto-hdr>

}

