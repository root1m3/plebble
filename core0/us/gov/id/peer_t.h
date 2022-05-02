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
#include <mutex>
#include <condition_variable>
#include <us/gov/config.h>
#include <us/gov/crypto/types.h>
#include <us/gov/crypto/symmetric_encryption.h>
#include <us/gov/socket/peer_t.h>
#include "types.h"
#include "api.h"

namespace us { namespace gov { namespace id {

    struct daemon_t;
    struct rpc_daemon_t;

    struct peer_t: base_ns::peer_t, api {
        using b = base_ns::peer_t;

        static const char* KO_6017;

        enum stage_t {
            anonymous = 0,
            verified,
            verified_fail,
            num_stages
        };
        constexpr static array<const char*, num_stages> stagestr = {"anonymous", "verified", "verified_fail"};

        enum role_t {
            role_peer = 0,
            role_sysop = 1,
            role_device = 2,
            num_roles
        };
        constexpr static array<const char*, num_roles> rolestr = {"peer", "sysop", "device"};

        peer_t(daemon_t&, sock_t sock);
        using b::peer_t;
        virtual ~peer_t();

        virtual ko connect(const hostport_t&, pport_t, pin_t, role_t, bool block);
        ko connect(const shostport_t&, pport_t, pin_t, role_t, bool block);
        virtual void verification_completed(pport_t, pin_t) {}
        const keys_t& get_keys() const;
        virtual void dump_all(const string& prefix, ostream&) const override;
        void dump(const string& prefix, ostream&) const;
        bool process_work(datagram*) override;
        static string to_string(const vector<unsigned char>& data);
        bool verification_is_fine() const { return get_stage_peer() == verified; }
        string short_version() const;
        ko turn_on_encryption();
        ko wait_auth() const;
        pair<ko, datagram*> encrypt0(datagram*) const override;
        pair<ko, datagram*> decrypt0(datagram*) const override;
        using b::disconnect;
        void disconnectx(channel_t, seq_t, const reason_t&) override;
        stage_t get_stage_peer() const;
        void set_stage_peer(stage_t);

        inline bool is_role_peer() const { return role == role_peer; }
        inline bool is_role_sysop() const { return role == role_sysop; }
        inline bool is_role_device() const { return role == role_device; }

        struct handshake_t {
            using sigmsg_hasher_t = crypto::ec::sigmsg_hasher_t;
            using sigmsg_hash_t = sigmsg_hasher_t::value_type;

            handshake_t(role_t role, pport_t pport, pin_t pin);
            handshake_t();

            role_t parse_role() const;
            version_fingerprint_t parse_version_fingerprint() const;
            pport_t parse_pport() const;
            pin_t parse_pin() const;
            void dump(const string& pfx, ostream&) const;

            sigmsg_hash_t msg;
        };

        struct handshakes_t {

            handshakes_t(role_t, pport_t, pin_t);
            handshakes_t();
            ~handshakes_t();

            handshake_t* me{nullptr};
            handshake_t* peer{nullptr};
        };

        #ifdef has_us_gov_id_api

            virtual ko initiate_dialogue(role_t, pport_t, pin_t);

            #include <us/api/generated/c++/gov/id/hdlr_override>
            #include <us/api/generated/c++/gov/id/hdlr_svc_handler-hdr>
            #include <us/api/generated/c++/gov/id/cllr_override>

        #endif

    public:
        stage_t stage_peer{anonymous};
        handshakes_t* handshakes{nullptr};
        role_t role{role_peer};
        pub_t pubkey;
        version_fingerprint_t version_fingerprint{0};
        mutable mutex mx_auth;
        mutable condition_variable cv_auth;
        mutable crypto::symmetric_encryption* se{nullptr};
        thread_local static bool inbound_traffic__was_encrypted;
    };

}}}

