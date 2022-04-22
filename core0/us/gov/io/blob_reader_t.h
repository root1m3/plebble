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
#pragma once
#include <cassert>
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <limits>
#include <unordered_map>
#include <tuple>

#include <us/gov/config.h>
#include <us/gov/likely.h>
#include <us/gov/types.h>
#include <us/gov/crypto/types.h>
#include <us/gov/socket/types.h>

#include "types.h"

#ifdef DEBUG
   struct assert_is_root_namespace;
   static_assert(std::is_same<assert_is_root_namespace, ::assert_is_root_namespace>::value, "Not root namespace. Check includes.");
#endif

namespace us { namespace gov { namespace io {

    using namespace us::gov;

    struct blob_reader_t {
        using version_t = uint8_t;
        using serid_t = uint8_t;

        static constexpr version_t current_version{CFG_BLOB_VERSION};
        static constexpr uint64_t max_sizet_containers{numeric_limits<uint16_t>::max()};
        static const char* KO_67217, *KO_60499, *KO_60498, *KO_75643;

        struct blob_header_t {
            version_t version;
            serid_t serid;
        };

        struct readable {
            virtual ~readable() {}

            virtual ko from_blob(blob_reader_t&) = 0;
            virtual serid_t serial_id() const { return 0; }

            ko read(const blob_t&);
            ko read(const datagram&);
            ko read(const string& blob_b58);
            ko load(const string& filename);
            pair<ko, blob_header_t> read1(const blob_t&);
            pair<ko, blob_header_t> read1(const string& blob_b58);
            pair<ko, pair<blob_header_t, hash_t>> read2(const blob_t&);
            pair<ko, pair<blob_header_t, hash_t>> read2(const string& blob_b58);
            pair<ko, blob_header_t> load1(const string& filename);
            pair<ko, pair<blob_header_t, hash_t>> load2(const string& filename);
            pair<ko, blob_t> load3(const string& filename);
            static pair<ko, readable*> load(const string& filename, function<readable*(const serid_t&)>);

        };

        blob_reader_t(const blob_t&);
        blob_reader_t(const datagram&);

        template<typename t>
        static ko readD(const datagram& d, t& o) {
            blob_reader_t reader(d);
            return reader.read(o);
        }

        ko read_sizet(uint64_t&);

        template<typename t>
        ko read(t& o) {
            return o.from_blob(*this);
        }

        template<typename t>
        ko read(vector<t>& o) {
            o.clear();
            uint64_t sz;
            auto r = read_sizet(sz);
            if (is_ko(r)) return r;
            if (unlikely(sz > max_sizet_containers)) return KO_75643;
            o.resize(sz);
            for (auto& i: o) {
                auto r = read(i);
                if (is_ko(r)) return r;
            }
            return ok;
        }

        template<typename t1, typename t2>
        ko read(pair<t1, t2>& o) {
            {
                auto r = read(o.first);
                if (is_ko(r)) return r;
            }
            {
                auto r = read(o.second);
                if (is_ko(r)) return r;
            }
            return ok;
        }

        template<typename t1, typename t2, typename t3>
        ko read(tuple<t1, t2, t3>& o) {
            {
                auto r = read(get<0>(o));
                if (is_ko(r)) return r;
            }
            {
                auto r = read(get<1>(o));
                if (is_ko(r)) return r;
            }
            {
                auto r = read(get<2>(o));
                if (is_ko(r)) return r;
            }
            return ok;
        }

        template<typename k, typename v>
        ko read(map<k, v>& o) {
            o.clear();
            uint64_t sz;
            auto r = read_sizet(sz);
            if (is_ko(r)) return r;
            if (unlikely(sz > max_sizet_containers)) return KO_75643;
            for (uint64_t i = 0; i < sz; ++i) {
                k a;
                {
                    auto r = read(a);
                    if (is_ko(r)) return r;
                }
                v b;
                {
                    auto r = read(b);
                    if (is_ko(r)) return r;
                }
                o.emplace(move(a), move(b));
            }
            return ok;
        }

        template<typename k, typename v>
        ko read(unordered_map<k, v>& o) {
            o.clear();
            uint64_t sz;
            auto r = read_sizet(sz);
            if (is_ko(r)) return r;
            if (unlikely(sz > max_sizet_containers)) return KO_75643;
            for (uint64_t i = 0; i < sz; ++i) {
                k a;
                {
                    auto r = read(a);
                    if (is_ko(r)) return r;
                }
                v b;
                {
                    auto r = read(b);
                    if (is_ko(r)) return r;
                }
                o.emplace(move(a), move(b));
            }
            return ok;
        }

        ko read_header(serid_t);
        ko read_header();
        static ko read_header(const string& file, blob_header_t&);

        template<typename t>
        static ko parse(const blob_t& blob, t& o) {
            blob_reader_t reader(blob);
            return reader.read(o);
        }

        uint8_t peek() const;

        const blob_t& blob;
        const uint8_t* cur;
        const uint8_t* end;
        blob_header_t header;

    };

    template<> inline ko blob_reader_t::readD(const datagram& d, blob_reader_t::readable& o) { return o.read(d); }
    template<> ko blob_reader_t::readD(const datagram&, blob_t&);

    template<> ko blob_reader_t::read(bool&);
    template<> ko blob_reader_t::read(uint64_t&);
    template<> ko blob_reader_t::read(int64_t&);
    template<> ko blob_reader_t::read(int32_t&);
    template<> ko blob_reader_t::read(uint8_t&);
    template<> ko blob_reader_t::read(uint16_t&);
    template<> ko blob_reader_t::read(uint32_t&);
    template<> ko blob_reader_t::read(string&);
    template<> ko blob_reader_t::read(blob_t&);
    template<> ko blob_reader_t::read(hash_t&);
    template<> ko blob_reader_t::read(sigmsg_hash_t&);
    template<> ko blob_reader_t::read(pub_t&);
    template<> ko blob_reader_t::read(priv_t&);
    template<> ko blob_reader_t::read(sig_t&);
    template<> ko blob_reader_t::read(time_point&);
    template<> ko blob_reader_t::read(char&);

}}}

