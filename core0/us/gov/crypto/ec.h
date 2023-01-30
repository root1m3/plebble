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
#pragma once
#include <secp256k1.h>
#include <iostream>
#include <vector>
#include <array>

#include <us/gov/config.h>
#include <us/gov/likely.h>

#include "ripemd160.h"
#include "sha256.h"

namespace us::gov::crypto {

    using namespace std;

    struct ec final {
        using sigmsg_hasher_t = sha256;
        using hasher_t = ripemd160;
        using hash_t = hasher_t::value_type;

        static const char* KO_96857;

        ec();
        ec(const ec&) = delete;
        ec& operator = (const ec&) = delete;
        ~ec();

        struct keys {
            struct pub_t final: secp256k1_pubkey {
                using b = secp256k1_pubkey;
                static constexpr size_t mem_size {sizeof(b)}; //64 bytes
                static_assert(mem_size == 64);
                static constexpr uint32_t ser_size{33};
                using hash_t = ec::hash_t;

                pub_t();
                pub_t(const pub_t&);
                pub_t(pub_t&&);
                explicit pub_t(const string& b58);
                explicit pub_t(const vector<unsigned char>& ser);

                const hash_t& hash() const;
                pub_t& operator = (const string&);
                pub_t& operator = (const pub_t&);
                pub_t& operator = (pub_t&&);
                hash_t compute_hash() const;
                string to_b58() const;
                bool set_b58(const string&);
                void write(unsigned char*) const;
                void read(const unsigned char*);

                bool operator == (const pub_t&) const;
                bool operator != (const pub_t&) const;
                bool operator == (const string& b58) const;
                bool operator != (const string& b58) const;
                inline void zero() { valid = false; }
                inline void set_valid() { valid = true; h = false; }

                uint8_t lsb() const { return *reinterpret_cast<const uint8_t*>(&data[mem_size - 1]); }

            public:
                static pub_t from_b58(const string&);
                static pub_t from_hex(const string&);
                void read_b58(const string&);

            public:
                void read_ser(const vector<unsigned char>&);
                bool to_vector(vector<unsigned char>&) const;
                size_t ser_sz() const;
                unsigned char* write_to(unsigned char*) const;
                const unsigned char* read_from(const unsigned char* beg, const unsigned char* end);

            public:
                bool valid{false};

            private:
                mutable bool h{false};
                mutable hash_t hash_cached;
            };

            static constexpr size_t priv_mem_size {32};
            struct priv_t final: array<uint8_t, priv_mem_size> {
                static constexpr uint32_t ser_size{priv_mem_size};

                priv_t();
                priv_t(priv_t&&);
                priv_t(const priv_t&);
                explicit priv_t(const string& b58);
                explicit priv_t(const char* b58);
                priv_t& operator = (const string& b58);
                priv_t& operator = (const priv_t&);
                priv_t& operator = (priv_t&&);
                void reset(const priv_t&);
                void reset(priv_t&&);
                string to_b58() const;
                static priv_t from_b58(const string&);
                bool set_b58(const string&);
                void zero();
                bool is_zero() const;
                bool is_not_zero() const;
                ko generate();
                ko verifyx() const;
                pub_t pub() const;
                int reset_if_distinct(const priv_t&); // 0 OK/KO equal; 1 OK distinct-valid; -1 KO distinct invalid input
                void write(unsigned char*) const;
                void read(const unsigned char*);

                bool operator == (const priv_t&) const;
                bool operator != (const priv_t&) const;
                bool operator == (const string& b58) const;
                bool operator != (const string& b58) const;
            };

            keys() {}
            keys(const keys&);
            keys(keys&&);
            explicit keys(const priv_t&);
            explicit keys(priv_t&&);
            void reset(); //reset invalid
            void reset(const priv_t&);
            void reset(priv_t&&);
            int reset_if_distinct(const priv_t&); // 0 OK/KO equal; 1 OK distinct-valid; -1 KO distinct invalid input
            ko verifyx() const;
            static keys generate();
            static ko verifyx(const priv_t&);
            static pub_t get_pubkey(const priv_t&);
            static priv_t generate_priv();

        public:
            priv_t priv;
            pub_t pub;
        };

        #if CFG_COUNTERS == 1
            struct counters_t {
                uint32_t unable_serialize_pubkey{0};
                uint32_t invalid_b58_pubkey{0};
                uint32_t invalid_ser_pubkey{0};
                uint32_t error_signing{0};
                uint32_t error_serializing_signature{0};
                uint32_t signatures_created{0};
                uint32_t error_creating_shared_secret{0};
                uint32_t signature_verification_der_parse_error{0};
                uint32_t signature_verification_pass{0};
                uint32_t signature_verification_fail{0};
                uint32_t privkey_verification_pass{0};
                uint32_t privkey_verification_fail{0};
                uint32_t error_priv_2_pub{0};
                void dump(ostream&) const;
            };
            static counters_t counters;
        #endif

        static constexpr uint32_t bit_size() { return 256; }

        struct sig_t: secp256k1_ecdsa_signature {
            static constexpr uint32_t mem_size{sizeof(secp256k1_ecdsa_signature)};
            static_assert(mem_size == 64);

            sig_t() { zero(); }
            sig_t(int) { zero(); }

            bool operator == (const sig_t& other) const;

            void write(unsigned char*) const;
            void read(const unsigned char*);
            string to_b58() const;
            bool set_b58(const string&);
            void from_b58(const string& b58);
            void zero();
            bool is_zero() const;
            bool is_not_zero() const;

        }; // signature DER

        using sig_der_t = vector<uint8_t>;

        static sig_t sig_from_b58(const string& b58);
        sig_der_t sig_encode_der(const sig_t& sig) const;
        string sig_encode_der_b58(const sig_t& sig) const;
        sig_t sig_from_der(const sig_der_t&) const;
        sig_t sig_from_der(const string& der_b58) const;
        sig_t sig_from_b58_prev(const string& b58) const;
        ko sign(const keys::priv_t&, const hash_t&, sig_t&) const;
        ko sign(const keys::priv_t&, const string&, sig_t&) const;
        ko sign(const keys::priv_t&, const vector<uint8_t>&, sig_t&) const;
        ko sign(const keys::priv_t&, const sigmsg_hasher_t::value_type&, sig_t&) const;
        bool verify(const keys::pub_t&, const hash_t&, const sig_t&) const;
        bool verify(const keys::pub_t&, const string&, const sig_t&) const;
        bool verify(const keys::pub_t&, const vector<uint8_t>&, const sig_t&) const;
        bool verify(const keys::pub_t&, const sigmsg_hasher_t::value_type&, const sig_t&) const;
        bool verify(const keys::pub_t&, const sigmsg_hasher_t::value_type&, const sig_t&, ostream& err) const;
        bool verify_not_normalized(const keys::pub_t&, const string&, const sig_t&) const;
        bool verify_not_normalized(const keys::pub_t&, const vector<uint8_t>&, const sig_t&) const;
        bool verify_not_normalized(const keys::pub_t&, const sigmsg_hasher_t::value_type&, const sig_t&) const;
        ko generate_shared_key(unsigned char *, size_t, const keys::priv_t&, const keys::pub_t&);

        static ec instance;
        secp256k1_context *ctx;
    };

    inline ostream& operator << (ostream& os, const ec::keys::pub_t& pub) {
        os << pub.to_b58();
        return os;
    }

    istream& operator >> (istream& is, ec::keys::pub_t& k);

    inline ostream& operator << (ostream& os, const ec::keys::priv_t& sk) {
        os << sk.to_b58();
        return os;
    }

    inline istream& operator >> (istream& is, ec::keys::priv_t& sk) {
        string s;
        is >> s;
        if (!sk.set_b58(s)) {
            is.setstate(ios_base::failbit);
            sk.zero();
        }
        return is;
    }

    inline ostream& operator << (ostream& os, const ec::sig_t& sig) {
        os << sig.to_b58();
        return os;
    }

    inline istream& operator >> (istream& is, ec::sig_t& sig) {
        string s;
        is >> s;
        if (!sig.set_b58(s)) {
            is.setstate(ios_base::failbit);
            sig.zero();
        }
        return is;
    }

}

namespace std {
    template <>
    struct hash<us::gov::crypto::ec::keys::pub_t> {
        size_t operator () (const us::gov::crypto::ec::keys::pub_t&) const;
    };
}

