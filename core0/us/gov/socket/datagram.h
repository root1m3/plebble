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

#include <string>
#include <vector>
#include <chrono>
#include <atomic>
#include <limits>
#include <unordered_map>

#include <us/gov/crypto/types.h>
#include "types.h"

#ifdef DEBUG
//#define DGRAM_CATCH_DELETE
#endif
#ifdef DEBUG
//#define FIND_DGRAM_DOUBLE_DELETE
#endif

namespace us::gov::crypto {

    struct symmetric_encryption;

}

namespace us::gov::socket {

    struct busyled_t;

    struct datagram final: vector<uint8_t> {
        using b = vector<uint8_t>;

        static const char *KO_0001, *KO_0002, *KO_0003, *KO_4038, *KO_4921, *KO_9021, *KO_20293, *KO_20292;

        struct dispatcher_t {
            virtual ~dispatcher_t() {}
            virtual bool dispatch(datagram*); //return true is datagram has been processed
        };

        #if CFG_COUNTERS == 1
            struct iodata {
                iodata() {}
                struct data {
                    void dump(ostream&) const;
                    void to_stream(ostream&) const;
                    atomic<uint32_t> num{0};
                    atomic<uint32_t> bytes{0};
                };
                void dump(ostream&) const;
                void to_stream(ostream&) const;
                data sent;
                data received;
            };

            struct counters_t: unordered_map<uint16_t, iodata*> {
                counters_t();
                ~counters_t();
                uint32_t wrong_channel{0};
                uint32_t closed_connection{0};
                uint32_t peer_timeout{0};
                uint32_t peer_ended{0};
                uint32_t infraheader_datagram{0};
                uint32_t wrong_size{};
                uint32_t valid_dgram_fragment1{0};
                uint32_t valid_dgram_fragment2{0};
                uint64_t bytes_received{0};
                void update(const datagram& d, bool send);
                void dump(ostream&) const;
                void dump_bw(const chrono::system_clock::time_point&, ostream&) const;
                void to_stream_io(ostream&) const;
                void clean();
                pair<uint32_t, uint32_t> totals() const;
            };
            static counters_t counters;
        #endif

        static constexpr size_t offset_channel{0};
        static constexpr size_t size_channel{sizeof(channel_t)};
        static constexpr size_t offset_size{offset_channel + size_channel};
        static constexpr size_t size_size{sizeof(uint32_t)};
        static constexpr size_t offset_service{offset_size + size_size};
        static constexpr size_t size_service{sizeof(svc_t)};
        static constexpr size_t offset_sequence{offset_size + size_size + size_service};
        static constexpr size_t size_sequence{sizeof(seq_t)};
        static constexpr size_t h{size_channel + size_size + size_service + size_sequence};
        static constexpr size_t maxsize{CFG_MAX_SIZE_DATAGRAM};

        static constexpr svc_t svc_encrypted{0};

    public:
        datagram(channel_t channel, svc_t service, seq_t sequence, uint32_t payload_size);
        datagram(channel_t channel, uint16_t mode);
        datagram(const datagram&);

        ~datagram();

    public:
        bool completed() const;
        hash_t compute_hash() const;
        hash_t compute_hash_payload() const;
        ko recvfrom(int sock, channel_t, busyled_t&);

        pair<ko, datagram*> encrypt(crypto::symmetric_encryption&) const;
        pair<ko, datagram*> decrypt(channel_t channel, crypto::symmetric_encryption&) const;

        void reset_service(svc_t);

        void dump(ostream&) const;
        uint32_t decode_size() const;
        svc_t decode_service() const;
        seq_t decode_sequence() const;
        void encode_size(uint32_t);
        void encode_service(svc_t);
        void encode_sequence(seq_t);
        void encode_channel(channel_t);
        channel_t decode_channel() const;

        static bool validate_payload(const vector<uint8_t>&);

        inline bool in_progress() const { return dend > 0 && dend < size(); }

        #ifdef DGRAM_CATCH_DELETE
            static void _begin_protect_delete(const datagram*);
            static void _end_protect_delete(const datagram*);
            struct _protect_delete_raii {
                _protect_delete_raii(const datagram* d): d(d) {
                    datagram::_begin_protect_delete(d);
                }
                ~_protect_delete_raii() {
                    datagram::_end_protect_delete(d);
                }
                const datagram* d;
            };

        #endif
        #ifdef FIND_DGRAM_DOUBLE_DELETE
            uint64_t _serial;
            static atomic<uint64_t> _next_serial;
        #endif

        static constexpr channel_t all_channels{numeric_limits<channel_t>::max()};
        svc_t service;
        uint32_t dend;
    };

}

