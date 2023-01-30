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
#include <cassert>
#include <utility>
#include <functional>
	
#include <us/gov/ko.h>
#include <us/gov/likely.h>

namespace us::gov::io {

    using namespace std;

    template<typename t>
    struct factory_t {
        using value_type = t;
        factory_t() {}
        virtual ~factory_t() {}
        virtual pair<ko, value_type*> create() const { return make_pair("KO 71012 factory not implemented.", nullptr); }
        virtual pair<ko, value_type*> create() { return const_cast<const factory_t<t>&>(*this).create(); }
    };


    template<typename t>
    struct factories_t: map<typename t::factory_id_t, factory_t<t>*> {
        using b = map<typename t::factory_id_t, factory_t<t>*>;

        factories_t() {
        }

        ~factories_t() {
//cout << "LEAK XXSZ " << this << ' ' << b::size() << endl;
            for (auto& i: *this) {
//cout << "LEAK XXXX " << i.second << endl;
                delete i.second;
            }
        }

        ko register_factory(const typename t::factory_id_t& id, factory_t<t>* f) {
            auto i = b::find(id);
            if (i != b::end()) {
                auto r = "KO 40193 Factory already registered.";
                //log(r, id);
                cerr << "factory_id_t " << id << " " << r << endl;
                assert(false);
                return r;
            }
//cout << "LEAK REG " << this << ' ' << f << endl;
            b::emplace(id, f);
            return ok;
        }

        ko unregister_factory(const typename t::factory_id_t& id) {
            auto i = b::find(id);
            if (i == b::end()) {
                cerr << "factory_id_t " << id << " not found" << endl;
                return "KO 40192 Factory not found.";
            }
            delete i->second;
            b::erase(i);
            return ok;
        }

        pair<ko, t*> create(const typename t::factory_id_t& id) const {
            auto i = b::find(id);
            if (unlikely(i == b::end())) {
                cerr << "factory_id_t " << id << " not found" << endl;
                assert(false); //--strip
                return make_pair("KO 69243 factory not found.", nullptr);
            }
            return i->second->create();
        }

        template<typename init_t>
        pair<ko, t*> create(const typename t::factory_id_t& id, init_t&& init) const {
            auto i = b::find(id);
            if (unlikely(i == b::end())) {
                cerr << "factory_id_t " << id << " not found" << endl;
                assert(false); //--strip
                return make_pair("KO 69243 factory not found.", nullptr);
            }
            auto o = i->second->create();
            assert(o.first == ok);
            init(*o.second);
            return o;
        }

    };

}

