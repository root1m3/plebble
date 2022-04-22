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
package us.gov.id;
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import us.gov.socket.datagram;                                                                 // datagram
import static us.gov.crypto.types.*;                                                           // *
import static us.gov.io.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import static us.ko.*;                                                                         // *
import us.ko;                                                                                  // ko
import us.gov.protocol;                                                                        // protocol
import java.security.PublicKey;                                                                // PublicKey
import us.gov.crypto.sha256;                                                                   // sha256

public interface dto {

    //#include <us/api/generated/java/gov/id/cllr_dto-hdr>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/gov/id/cllr_dto-hdr>
    // id - master file: us/api/data/gov/id


    /// peer_challenge - IN
    public static final class peer_challenge_in_t extends blob_writer_t.writable {
        //source: coder_java.cpp::gen_dto_in_hdr (1)

        public peer_challenge_in_t(final sha256.hash_t msg, final pub_t pub, final sig_t sig, final sig_der_t sig_der) {
            this.msg = msg;
            this.pub = pub;
            this.sig = sig;
            this.sig_der = sig_der;
        }

        @Override public int blob_size() {
            return blob_writer_t.blob_size(msg) +
                blob_writer_t.blob_size(pub) +
                blob_writer_t.blob_size(sig) +
                blob_writer_t.blob_size(sig_der);
        }

        @Override public void to_blob(blob_writer_t writer) {
            writer.write(msg);
            writer.write(pub);
            writer.write(sig);
            writer.write(sig_der);
        }

        public datagram get_datagram(final channel_t channel, final seq_t seq) {
            return super.get_datagram(channel, new svc_t(protocol.id_peer_challenge), seq);
        }

        public static datagram get_datagram(final channel_t channel, final seq_t seq, final sha256.hash_t msg, final pub_t pub, final sig_t sig, final sig_der_t sig_der) {
            peer_challenge_in_t o = new peer_challenge_in_t(msg, pub, sig, sig_der);
            return o.get_datagram(channel, seq);
        }

        final sha256.hash_t msg;
        final pub_t pub;
        final sig_t sig;
        final sig_der_t sig_der;
    }

    /// challenge_response - IN
    public static final class challenge_response_in_t extends blob_writer_t.writable {
        //source: coder_java.cpp::gen_dto_in_hdr (1)

        public challenge_response_in_t(final pub_t pub, final sig_t sig, final sig_der_t sig_der) {
            this.pub = pub;
            this.sig = sig;
            this.sig_der = sig_der;
        }

        @Override public int blob_size() {
            return blob_writer_t.blob_size(pub) +
                blob_writer_t.blob_size(sig) +
                blob_writer_t.blob_size(sig_der);
        }

        @Override public void to_blob(blob_writer_t writer) {
            writer.write(pub);
            writer.write(sig);
            writer.write(sig_der);
        }

        public datagram get_datagram(final channel_t channel, final seq_t seq) {
            return super.get_datagram(channel, new svc_t(protocol.id_challenge_response), seq);
        }

        public static datagram get_datagram(final channel_t channel, final seq_t seq, final pub_t pub, final sig_t sig, final sig_der_t sig_der) {
            challenge_response_in_t o = new challenge_response_in_t(pub, sig, sig_der);
            return o.get_datagram(channel, seq);
        }

        final pub_t pub;
        final sig_t sig;
        final sig_der_t sig_der;
    }
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.

    //#include <us/api/generated/java/gov/id/hdlr_dto-hdr>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/gov/id/hdlr_dto-hdr>
    // id - master file: us/api/data/gov/id


    public static final class peer_challenge_in_dst_t extends blob_reader_t.readable {
        //source: coder_java.cpp::gen_dto_in_hdr (2)

        public peer_challenge_in_dst_t() {}


        @Override public ko from_blob(blob_reader_t reader) {
            {
                ko r = reader.read(msg);
                if (ko.is_ko(r)) return r;
            }
            {
                ko r = reader.read(pub);
                if (ko.is_ko(r)) return r;
            }
            {
                ko r = reader.read(sig);
                if (ko.is_ko(r)) return r;
            }
            {
                ko r = reader.read(sig_der);
                if (ko.is_ko(r)) return r;
            }
            return ok;
        }

        public sha256.hash_t msg = new sha256.hash_t();
        public pub_t pub = new pub_t();
        public sig_t sig = new sig_t();
        public sig_der_t sig_der = new sig_der_t();
    }

    public static final class challenge_response_in_dst_t extends blob_reader_t.readable {
        //source: coder_java.cpp::gen_dto_in_hdr (2)

        public challenge_response_in_dst_t() {}


        @Override public ko from_blob(blob_reader_t reader) {
            {
                ko r = reader.read(pub);
                if (ko.is_ko(r)) return r;
            }
            {
                ko r = reader.read(sig);
                if (ko.is_ko(r)) return r;
            }
            {
                ko r = reader.read(sig_der);
                if (ko.is_ko(r)) return r;
            }
            return ok;
        }

        public pub_t pub = new pub_t();
        public sig_t sig = new sig_t();
        public sig_der_t sig_der = new sig_der_t();
    }
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.

}

