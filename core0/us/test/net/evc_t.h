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
#include <us/gov/bgtask.h>
#include <us/gov/crypto/types.h>
#include <us/gov/engine/net.h>

namespace us { namespace test {

    struct pld {
        pld(const hash_t& h, int numnodes): h(h) { a.resize(numnodes, 0); }
        hash_t h;
        vector<int> a;
    };

    struct evc_t: map<chrono::system_clock::time_point, pld> {
        evc_t(int numnodes);
        void wait_empty();
        void purge_();
        void newev( hash_t h, int srcport);
        void recv(const hash_t&h, int ndx);
        //void recvu(const hash_t&h, int ndx);
        void dump(ostream& os) const;

        int numnodes{0};
        int completed{0};
        int total{0};
        mutable mutex mx;
    };

    struct evsseen_t: unordered_set<hash_t> {
        mutex mx;
    };

}}
