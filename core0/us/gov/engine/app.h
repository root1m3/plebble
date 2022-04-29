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
#include <vector>
#include <cassert>
#include <thread>
#include <chrono>
#include <mutex>
#include <unordered_map>

#include <us/gov/io/seriable.h>
#include <us/gov/types.h>

#include "evidence.h"

namespace us::gov::engine {

    struct peer_t;
    struct local_delta;
    struct delta;
    struct block;
    struct pow_t;
    struct daemon_t;
    struct block;

    struct app: io::seriable {

        struct local_delta: virtual io::seriable {
            virtual ~local_delta() {}
            static local_delta* create(appid_t id);
            static local_delta* create(appid_t appid, const blob_t&);
            static local_delta* create_prev(appid_t appid, const blob_t&);
            virtual void dump(const string& prefix, ostream&) const = 0;
            virtual void hash_data_to_sign(sigmsg_hasher_t&) const = 0;
            virtual void hash_data(hasher_t&) const = 0;
            virtual appid_t app_id() const = 0;
            hash_t compute_hash() const;
        };

        struct delta: virtual io::seriable {
            delta() {}
            virtual ~delta() {}
            static delta* create(appid_t id);
            static delta* create(const blob_t&);
            static delta* create(appid_t id, const blob_t&);
            virtual void dump(const string& prefix, ostream&) const = 0;
            virtual uint64_t merge(local_delta*);
            virtual void end_merge() = 0;
            uint64_t multiplicity{0};
        };

        app(daemon_t& e): demon(e) {}
        virtual ~app() {}
        virtual appid_t get_id() const = 0;
        virtual string get_name() const = 0;
        virtual void clear() = 0;
        virtual void dump(const string& prefix, int detail, ostream&) const = 0;
        virtual void import(const delta&, const pow_t&) = 0;
        virtual bool process(const evidence&) = 0;
        virtual local_delta* create_local_delta() = 0;
        virtual ko shell_command(istream&, ostream&);

    public:
        daemon_t& demon;
    };

}

