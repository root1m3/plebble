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
#include <map>
#include <set>
#include <tuple>
#include <string>
#include <string_view>

#include "blob_reader_t.h"
#include "cfg0.h"
#include "types.h"

namespace us::gov::io {

    struct blob_writer_t {

        using version_t = blob_reader_t::version_t;
        using serid_t = blob_reader_t::serid_t;
        using blob_header_t = blob_reader_t::blob_header_t;

        struct writable {
        public:
            virtual ~writable() {}

        public:
            virtual size_t blob_size() const = 0;
            virtual void to_blob(blob_writer_t&) const = 0;
            virtual serid_t serial_id() const { return 0; }

        public:
            size_t tlv_size() const;
            void write(blob_t&) const;
            datagram* get_datagram(channel_t, svc_t svc, seq_t seq) const;
            void write(string& encoded) const;
            string encode() const;
            ko save(const string& filename) const;
        };

    public:
        blob_writer_t(blob_t& blob, const size_t& sz);

    private:
        blob_writer_t(datagram& datagram);

    public:
        constexpr static size_t header_size() { return sizeof(version_t) + sizeof(serid_t); }
        void write_header(const serid_t&);

        static size_t sizet_size(const uint64_t&);
        void write_sizet(const uint64_t&);

        template<typename t>
        static size_t blob_size(const t& o) {
            return o.blob_size();
        }

        template<typename t>
        void write(const t& o) {
            o.to_blob(*this);
        }

        template<typename t>
        static size_t blob_size(t* o) {
            if (o == nullptr) return 1;
            return 1 + o->blob_size();
        }

        template<typename t>
        void write(t* o) {
            uint8_t x = (o == nullptr ? 0 : 1);
            write(x);
            if (x == 1) {
                write(*o);
            }
        }


        template<typename t>
        static size_t blob_size(t* o, const factories_t<t>& f) {
            if (o == nullptr) return blob_size(t::null_instance);
            return blob_size(o->factory_id()) + o->blob_size();
        }

        template<typename t>
        void write(t* o, const factories_t<t>& f) {
//cout << "wcur=" << (uint64_t)(cur - blob.data()) << endl;
            typename t::factory_id_t x = (o == nullptr ? t::null_instance : o->factory_id());
            write(x);
            if (o == nullptr) {
                return;
            }
            write(*o);
        }

        static size_t blob_size(const char* s);
        void write(const char* s);

        static datagram* get_datagram(channel_t, svc_t svc, seq_t seq);
        static datagram* get_datagram(channel_t, svc_t svc, seq_t seq, const blob_t&);

        template<typename t>
        static datagram* get_datagram(channel_t channel, svc_t svc, seq_t seq, const t& o) {
            auto d = new datagram(channel, svc, seq, blob_size(o));
            blob_writer_t w(*d);
            w.write(o);
            return d;
        }

        static blob_t make_blob(const string& payload);

        //-------------------------

        template<typename t>
        static size_t blob_size(const vector<t>& o) {
            size_t sz = sizet_size(o.size());
            for (auto& i: o) {
                sz += blob_size(i);
            }
            return sz;
        }

        template<typename t>
        void write(const vector<t>& o) {
            write_sizet(o.size());
            for (auto& i: o) {
                write(i);
            }
        }

        template<typename t1, typename t2>
        static size_t blob_size(const pair<t1, t2>& o) {
            return blob_size(o.first) + blob_size(o.second);
        }

        template<typename t1, typename t2>
        void write(const pair<t1, t2>& o) {
            write(o.first);
            write(o.second);
        }


        template<typename t1, typename t2, typename t3>
        static size_t blob_size(const tuple<t1, t2, t3>& o) {
            return blob_size(get<0>(o)) + blob_size(get<1>(o)) + blob_size(get<2>(o));
        }

        template<typename t1, typename t2, typename t3>
        void write(const tuple<t1, t2, t3>& o) {
            write(get<0>(o));
            write(get<1>(o));
            write(get<2>(o));
        }

        template<typename k, typename v>
        static size_t blob_size(const map<k, v>& o) {
            size_t sz = sizet_size(o.size());
            for (auto& i: o) {
                sz += blob_size(i.first);
                sz += blob_size(i.second);
            }
            return sz;
        }

        template<typename k, typename v>
        void write(const map<k, v>& o) {
            write_sizet(o.size());
            for (auto& i: o) {
                write(i.first);
                write(i.second);
            }
        }

        template<typename k, typename v>
        static size_t blob_size(const unordered_map<k, v>& o) {
            size_t sz = sizet_size(o.size());
            for (auto& i: o) {
                sz += blob_size(i.first);
                sz += blob_size(i.second);
            }
            return sz;
        }

        template<typename k, typename v>
        void write(const unordered_map<k, v>& o) {
            write_sizet(o.size());
            for (auto& i: o) {
                write(i.first);
                write(i.second);
            }
        }

        template<typename k>
        static size_t blob_size(const set<k>& o) {
            size_t sz = sizet_size(o.size());
            for (auto& i: o) {
                sz += blob_size(i);
            }
            return sz;
        }

        template<typename k>
        void write(const set<k>& o) {
            write_sizet(o.size());
            for (auto& i: o) {
                write(i);
            }
        }

        blob_t& blob;
        uint8_t* cur;
    };

    template<> inline datagram* blob_writer_t::get_datagram(channel_t channel, svc_t svc, seq_t seq, const blob_writer_t::writable& o) {
        return o.get_datagram(channel, svc, seq);
    }

    template<> constexpr size_t blob_writer_t::blob_size(const bool&) { return sizeof(uint8_t); }
    template<> void blob_writer_t::write(const bool&);

    template<> constexpr size_t blob_writer_t::blob_size(const uint8_t&) { return sizeof(uint8_t); }
    template<> void blob_writer_t::write(const uint8_t&);

    template<> constexpr size_t blob_writer_t::blob_size(const uint16_t&) { return sizeof(uint16_t); }
    template<> void blob_writer_t::write(const uint16_t&);

    template<> constexpr size_t blob_writer_t::blob_size(const uint32_t&) { return sizeof(uint32_t); }
    template<> void blob_writer_t::write(const uint32_t&);

    template<> constexpr size_t blob_writer_t::blob_size(const uint64_t&) { return sizeof(uint64_t); }
    template<> void blob_writer_t::write(const uint64_t&);

    template<> constexpr size_t blob_writer_t::blob_size(const int64_t&) { return sizeof(int64_t); }
    template<> void blob_writer_t::write(const int64_t&);

    template<> constexpr size_t blob_writer_t::blob_size(const int32_t&) { return sizeof(int32_t); }
    template<> void blob_writer_t::write(const int32_t&);

    template<> size_t blob_writer_t::blob_size(const string&);
    template<> void blob_writer_t::write(const string&);

    template<> size_t blob_writer_t::blob_size(const string_view&);
    template<> void blob_writer_t::write(const string_view&);

    template<> size_t blob_writer_t::blob_size(const blob_t&);
    template<> void blob_writer_t::write(const blob_t&);

    template<> constexpr size_t blob_writer_t::blob_size(const hash_t&) { return hasher_t::output_size; }
    template<> void blob_writer_t::write(const hash_t&);

    template<> constexpr size_t blob_writer_t::blob_size(const sigmsg_hash_t&) { return sigmsg_hasher_t::output_size; }
    template<> void blob_writer_t::write(const sigmsg_hash_t&);

    template<> constexpr size_t blob_writer_t::blob_size(const pub_t&) { return pub_t::ser_size; }
    template<> void blob_writer_t::write(const pub_t&);

    template<> constexpr size_t blob_writer_t::blob_size(const priv_t&) { return priv_t::ser_size; }
    template<> void blob_writer_t::write(const priv_t&);

    template<> constexpr size_t blob_writer_t::blob_size(const keys& o) { return blob_size(o.priv) + blob_size(o.pub); }
    template<> void blob_writer_t::write(const keys&);

    template<> size_t blob_writer_t::blob_size(const sig_t&);
    template<> void blob_writer_t::write(const sig_t&);

    template<> constexpr size_t blob_writer_t::blob_size(const time_point&) { return sizeof(uint64_t); }
    template<> void blob_writer_t::write(const time_point&);

    template<> constexpr size_t blob_writer_t::blob_size(const char&) { return sizeof(char); }
    template<> void blob_writer_t::write(const char&);

    inline size_t blob_writer_t::blob_size(const char* s) { return blob_size(string_view(s)); }
    inline void blob_writer_t::write(const char* s) { write(string_view(s)); }

}

