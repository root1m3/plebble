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
#include <us/gov/ko.h>
#include <us/gov/likely.h>

namespace us::gov::io {
    using namespace std;

    template<typename t>
    struct factory_t {
        factory_t(const typename t::factory_id_t& id): id(id) {
        }
        virtual ~factory_t() {}

    public:
        virtual t* create() = 0;

    public:
        typename t::factory_id_t id;
    };


    template<typename t>
    struct factories_t: map<typename t::factory_id_t, factory_t<t>*> {
        using b = map<typename t::factory_id_t, factory_t<t>*>;

        factories_t() {
        }

        ~factories_t() {
            for (auto& i: *this) {
                delete i.second;
            }
        }

        ko register_factory(factory_t<t>* f) {
            auto i = b::find(f->id);
            if (i == b::end()) {
                return "KO 40193 Factory already registered.";
            }
            emplace(f->id, f);
            return ok;
        }

        t* create(const typename t::factory_id_t& id) const {
            auto i = b::find(id);
            if (unlikely(i == b::end())) {
                return nullptr;
            }
            return i->second->create();
        }

    };

}

