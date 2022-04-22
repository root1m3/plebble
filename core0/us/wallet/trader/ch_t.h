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

#include <string>
#include <vector>
#include <us/gov/ko.h>
#include <mutex>

namespace us { namespace wallet { namespace trader {

    using namespace std;

    struct local_params_t;

    struct ch_t {

        ch_t(int);
        ch_t(local_params_t&, mutex&);
        ~ch_t();

        bool all_changed() const { return shared_params_changed && personality_changed; }
        bool changed() const { return shared_params_changed || personality_changed; }
        string to_string() const;
        ch_t& operator |= (const ch_t& other);
        bool any() const { return priv_params_changed || shared_params_changed || personality_changed; }
        bool need_update_devices() const;
        void to_stream(ostream& os) const;
        ko from_stream(istream& is);
        void always_update_devices_();
        void no_need_to_update_devices_() { if (update_devices < 2) update_devices = 0; }
        void file_updated(const string& path, const string& file);
        void open(local_params_t&, mutex&);
        void close();
        bool closed() const { return local_params == nullptr; }
    public:
        bool priv_params_changed{false};
        bool shared_params_changed{false};
        bool personality_changed{false};
        vector<pair<string, string>> updated_files;

        local_params_t* local_params;
        unique_lock<mutex>* lock;
    private:
        int update_devices{1};
        //0 never update devices, even if params are sent to peer
        //1 update devices only if shared params are sent to peer
        //2 always update devices, even if no params are sent to peer
    };

}}}

