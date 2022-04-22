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
#include <unordered_map>
#include <us/gov/likely.h>

namespace us { namespace gov { namespace engine {

    template<typename d>
    struct average_merger {
        void merge(d& v1, const d& v2) const {
            v1 += v2;
        }
        void end_merge(d& v, const uint64_t& multiplicity) const {
            double m = multiplicity;
            v /= m;
        }
    };

    template<typename d>
    struct majority_merger {
        void merge(const d& v) {
            auto i = m.find(v);
            if (i != m.end()) {
                ++i->second;
                return;
            }
            m.emplace(v, 1);
        }

        void end_merge(d& v) {
            const d* x = nullptr;
            uint64_t z = 0;
            for (auto& i: m) {
                if (i.second > z) {
                    z = i.second;
                    x = &i.first;
                }
            }
            if (likely(z != 0)) {
                v = *x;
            }
            else {
                v = 0;
            }
            m.clear();
        }
        unordered_map<d, uint64_t> m;
    };

}}}

