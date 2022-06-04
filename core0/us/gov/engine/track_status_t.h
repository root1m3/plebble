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
#include <cassert>
#include <set>
#include <us/gov/io/seriable.h>
#include "types.h"

namespace us::gov::engine {

    enum evt_status_t: uint8_t { //values coupled in fragment_w2w.java (android app)
        evt_unknown,
        evt_error,
        evt_wait_arrival,
        evt_calendar,
        evt_mempool,
        evt_craftblock,
        evt_consensus,
        evt_settled,
        evt_untracked,

        evt_num
    };

    static constexpr const char* evt_status_str[evt_num] = {"unknown", "error", "wait_arrival", "calendar", "mempool", "craftblock", "consensus", "settled", "untracked"};

    struct track_status_t: us::gov::io::seriable {

        track_status_t() {}
        track_status_t(int e): st(evt_untracked) {
            assert(e == 0);
        }

        track_status_t(const ts_t& from, const ts_t& to, const evt_status_t& st, const string& info): from(from), to(to), st(st), info(info) { assert(info.empty()); }
        track_status_t(const ts_t& from, const ts_t& to, const evt_status_t& st): from(from), to(to), st(st) {}
        track_status_t(const ts_t& ts, const evt_status_t& st): from(ts), to(ts + 1), st(st) {}
        track_status_t(const ts_t& ts, const evt_status_t& st, const string& info): from(ts), to(ts + 1), st(st), info(info) { assert(info.empty()); }

        void reset(const ts_t& from, const ts_t& to, const evt_status_t&, const string& info);
        void reset(const ts_t& from, const ts_t& to, const evt_status_t&);
        void reset(const ts_t&, const evt_status_t&);
        void reset(const ts_t&, const evt_status_t&, const string& info);

    public:
        void dump(ostream&) const;
        bool range() const { return to != from + 1; }

    public: //serialization
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        ts_t from;
        ts_t to;
        evt_status_t st;
        string info;
    };

}

