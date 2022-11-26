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
#include <unordered_map>
#include <map>
#include <mutex>
#include <chrono>

#include <us/gov/types.h>
#include <us/gov/id/api.h>

#include "device_t.h"
#include "types.h"

namespace us::wallet::engine {

    struct devices_t: unordered_map<hash_t, device_t> {

        using request_data_t = us::gov::id::request_data_t;

        static ko KO_30291;
        static ko KO_30292;

        struct attempts_t: unordered_map<pub_t, std::pair<time_point, uint32_t>> {
            static constexpr chrono::seconds forget{600s};
            void purge();
            void purge(const pub_t&);
            void add(const pub_t&);
            void dump(ostream&) const;
            void dump(vector<string>&) const;
            mutable mutex mx;
        };

        static char d_version; //file d serialization version
        devices_t(const string& home);
        void load();
        void save() const;
        ko authorize(const pub_t&, pin_t pin, request_data_t&); //returns subhome
        void dump(ostream&) const;
        ko device_pair(const pub_t&, string subhome, string name);
        pair<ko, string> device_unpair(const pub_t&);
        pair<ko, pin_t> device_prepair(pin_t pin, string subhome, string name);
        ko device_unprepair(pin_t pin);
        const device_t* get_device(const pub_t&) const;
        const attempts_t& get_attempts() const { return attempts; }
        void set_consume_pin(bool);
        bool get_consume_pin() const;
        inline const string& get_home() const { return home; }
        inline bool is_enabled__authorize_and_create_guest_wallet() const { return authorize_and_create_guest_wallet; }
        ko handle_conf(const string& key, const string& value, string& ans);
        ko valid_subhome(string subhome) const;

    private:
        ko load_();
        void save_() const;
        ko device_pair_(const pub_t&, string subhome, string name, bool save = true);
        pair<ko, string> device_unpair_(const pub_t&);
        pair<ko, pin_t> device_prepair_(pin_t, string subhome, string name);
        ko device_unprepair_(pin_t pin);

    private:
        string file;
        mutable mutex mx;
        string home;
        string msg;
        attempts_t attempts;

        unordered_map<pin_t, device_t> prepaired;
        bool authorize_and_create_guest_wallet{false};
        bool consume_pin{true};
        bool allow_pin0{false};
        bool random_pin{true};
    };

}

