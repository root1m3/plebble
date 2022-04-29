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
#include <unordered_set>
#include <us/gov/bgtask.h>
#include <us/gov/config.h>

namespace us::gov::socket {

    struct client;

}

namespace us::gov::socket::multipeer {

    struct gc_t final: unordered_set<client*>, wbgtask {
        using t = wbgtask;

        gc_t();
        ~gc_t();

        void add_(client*);
        void dump(ostream&) const;
        void clear();
        void task_init() override;
        void task() override;
        void task_cleanup() override;
        void collect() override;
        void onwakeup();

    public:
        mutable mutex mx;
        condition_variable cv;

        #if CFG_LOGS == 1
            string logdir;
        #endif
    };

}

