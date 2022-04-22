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
#include <us/gov/config.h>
#include <us/gov/bgtask.h>
#include "calendar.h"

namespace us { namespace gov { namespace engine {

    struct evidence_processor: bgtask {
        using t = bgtask;

        evidence_processor();
        ~evidence_processor();

        void process_evidences(uint64_t maxts);
        void run();
        void wait();
        void wait_new_cycle();
        void onwakeup();
        virtual void on_block_closure(uint64_t) = 0;
        virtual void exec_evidence(const evidence&) = 0;
        virtual bool ready_for_block_opening(uint64_t) const = 0;

        calendar_t calendar;

    private:
        mutable condition_variable cv;

    #if CFG_LOGS == 1
    public:
        string logdir;
    #endif
    };

}}}

