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
#include <us/gov/id/peer_t.h>
#include <us/gov/crypto/symmetric_encryption.h>
#include <us/gov/socket/peer_t.h>

#include "types.h"
#include "api.h"

namespace us::gov::id {

    struct daemon_t;
    struct rpc_daemon_t;

    struct peer_t: base_ns::peer_t, api {
        using b = base_ns::peer_t;

        static const char* KO_6017;

        enum stage_t: uint8_t {
            anonymous = 0,
            verified,
            verified_fail,
            num_stages
        };
        constexpr static array<const char*, num_stages> stagestr = {"anonymous", "verified", "verified_fail"};

        using role_t = us::gov::id::role_t;
        constexpr static array<const char*, num_roles> rolestr = {"peer", "sysop", "device"};

    public:
        peer_t(daemon_t&, sock_t sock);
        using b::peer_t;
        virtual ~peer_t();

    public:
        virtual ko connect(const  hostport_t&, pport_t, pin_t, role_t, const request_data_t& request_data, bool block);
                ko connect(const shostport_t&, pport_t, pin_t, role_t, const request_data_t& request_data, bool block);
        virtual ko verification_completed(pport_t, pin_t, request_data_t&);
        virtual void upgrade_software();
        const keys_t& get_keys() const;
        virtual void dump_all(const string& prefix, ostream&) const override;
        void dump(const string& prefix, ostream&) const;
        bool process_work(datagram*) override;
        static string to_string(const vector<unsigned char>& data);
        bool verification_is_fine() const { return get_stage_peer() == verified; }
        ko turn_on_encryption();
        ko wait_auth() const;
        pair<ko, datagram*> encrypt0(datagram*) const override;
        pair<ko, datagram*> decrypt0(datagram*) const override;
        using b::disconnect;
        void disconnectx(channel_t, seq_t, const reason_t&) override;
        stage_t get_stage_peer() const;
        void set_stage_peer(stage_t);

        inline bool is_role_peer() const { return role == role_t::role_peer; }
        inline bool is_role_sysop() const { return role == role_t::role_sysop; }
        inline bool is_role_device() const { return role == role_t::role_device; }

        struct handshake_t final {
            using sigmsg_hasher_t = crypto::ec::sigmsg_hasher_t;
            using sigmsg_hash_t = sigmsg_hasher_t::value_type;

            handshake_t(api_v_t, role_t, pport_t, pin_t, const request_data_t&);
            handshake_t();
            handshake_t(sigmsg_hash_t&&);


            role_t parse_role() const;
            version_fingerprint_t parse_version_fingerprint() const;
            api_v_t parse_api_v() const;
            pport_t parse_pport() const;
            pin_t parse_pin() const;
            void dump(const string& pfx, ostream&) const;

            sigmsg_hash_t msg;
            request_data_t request_data;
        };

        struct handshakes_t final {

            handshakes_t(api_v_t, role_t, pport_t, pin_t, const request_data_t&);
            handshakes_t(sigmsg_hash_t&&);
            handshakes_t();
            ~handshakes_t();

            handshake_t* me{nullptr};
            handshake_t* peer{nullptr};
        };

        #ifdef has_us_gov_id_api

            virtual ko initiate_dialogue(role_t, pport_t, pin_t, const request_data_t&);

            #include <us/api/generated/gov/c++/id/hdlr_override>
            #include <us/api/generated/gov/c++/id/hdlr_svc_handler-hdr>
            #include <us/api/generated/gov/c++/id/cllr_override>

        #endif

    public:
        #if CFG_COUNTERS == 1
            struct counters_t {
               uint32_t successful_verifications{0};
               uint32_t failed_verifications{0};
               uint32_t signals_upgrade_software{0};
               void dump(ostream&) const;
            };
            static counters_t counters;
        #endif

        virtual void verification_result(request_data_t&&) = 0;
        static bool am_I_older(version_fingerprint_t peer);
        static bool am_I_older(version_fingerprint_t me, version_fingerprint_t peer);

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

}

