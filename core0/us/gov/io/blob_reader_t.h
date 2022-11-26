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
#include <cassert>
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <set>
#include <limits>
#include <unordered_map>
#include <tuple>
#include <functional>
#include <utility>
#include <type_traits>

#include <us/gov/config.h>
#include <us/gov/likely.h>
#include <us/gov/types.h>
#include <us/gov/crypto/types.h>
#include <us/gov/socket/types.h>

#include "factory.h"
#include "types.h"
#include "readable.h"

namespace us::gov::io {

    using namespace us::gov;

    struct blob_reader_t;

     struct blob_reader_t {
        using readable = us::gov::io::readable;
        using version_t = readable::version_t;
        using serid_t = readable::serid_t;
        using blob_header_t = readable::blob_header_t;

        static constexpr version_t current_version{CFG_BLOB_VERSION};
        static constexpr uint64_t max_sizet_containers{numeric_limits<uint16_t>::max()};

        static const char* KO_67217, *KO_60498, *KO_75643;

        blob_reader_t(const blob_t&);
        blob_reader_t(const datagram&);

/*
        static ko readD(const datagram& d, readable& o) { return o.read(d); }

        template<typename t>
        static ko readD(const datagram& d, t& o) {
            static_assert(!std::is_convertible<t*, readable*>::value, "KO 77888 template specialization for readable should have been instantiated instead.");
            
            blob_reader_t reader(d);
            return reader.read(o);
        }
*/

        ko read_sizet(uint64_t&);

        template<typename t>
        ko read(t& o) {
            return o.from_blob(*this);
        }

        template<typename t>
        ko read(t*& o) {
            if (o != nullptr) {
                delete o;
            }
            {
                uint8_t x;
                auto r = read(x);
                if (is_ko(r)) return r;
                if (x == 0) {
                    o = nullptr;
                    return ok;
                }
            }
            o = new t();
            auto r = read(*o);
            if (is_ko(r)) {
                delete o;
                o = nullptr;
                return r;
            }
            return ok;
        }

        template<typename t>
        ko read(t*& o, const factory_t<t>& f) {
            if (o != nullptr) {
                delete o;
            }
            uint8_t factory_id;
            {
                auto r = read(factory_id);
                if (is_ko(r)) return r;
                if (factory_id == 0) {
                    o = nullptr;
                    return ok;
                }
            }
            {
                auto p = f.create();
                if (is_ko(p.first)) {
                    return p.first;
                }
                o = p.second;
            }
            if (unlikely(o == nullptr)) {
                return "KO 65028 Invalid factory id";
            }
            auto r = read(*o);
            if (is_ko(r)) {
                delete o;
                o = nullptr;
                return r;
            }
            return ok;
        }

        template<typename t>
        ko read(t*& o, const factories_t<t>& f) {
            if (o != nullptr) {
                delete o;
            }
            typename t::factory_id_t factory_id;
            {
                auto r = read(factory_id);
                if (is_ko(r)) return r;
                if (factory_id == t::null_instance) {
                    o = nullptr;
                    return ok;
                }
            }
            {
                auto p = f.create(factory_id);
                if (is_ko(p.first)) {
                    return p.first;
                }
                o = p.second;
            }
            if (unlikely(o == nullptr)) {
                return "KO 65028 Invalid factory id";
            }
            auto r = read(*o);
            if (is_ko(r)) {
                delete o;
                o = nullptr;
                return r;
            }
            return ok;
        }

        template<typename t, typename init_t>
        ko read(t*& o, const factories_t<t>& f, init_t&& init) {
            if (o != nullptr) {
                delete o;
            }
            typename t::factory_id_t factory_id;
            {
                auto r = read(factory_id);
                if (is_ko(r)) return r;
                if (factory_id == t::null_instance) {
                    o = nullptr;
                    return ok;
                }
            }
            {
                auto p = f.create(factory_id, forward<init_t>(init));
                if (is_ko(p.first)) {
                    return p.first;
                }
                o = p.second;
            }
            if (unlikely(o == nullptr)) {
                return "KO 65028 Invalid factory id";
            }
            auto r = read(*o);
            if (is_ko(r)) {
                delete o;
                o = nullptr;
                return r;
            }
            return ok;
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

        template<typename k>
        ko read(set<k>& o) {
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
                o.emplace(move(a));
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

//    template<> ko blob_reader_t::readD(const datagram&, readable&);
//    template<> ko blob_reader_t::readD(const datagram&, blob_t&);

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
    template<> ko blob_reader_t::read(keys&);
    template<> ko blob_reader_t::read(sig_t&);
    template<> ko blob_reader_t::read(time_point&);
    template<> ko blob_reader_t::read(char&);

}

