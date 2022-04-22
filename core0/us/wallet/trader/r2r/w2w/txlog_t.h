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
#include <map>
#include <vector>
#include <mutex>

#include <us/gov/cash/tx_t.h>
#include <us/gov/socket/types.h>
#include <us/gov/io/seriable_map.h>
#include <us/gov/io/seriable_vector.h>
#include "txlog_item_t.h"

namespace us { namespace wallet { namespace trader { namespace r2r { namespace w2w {

    struct protocol;

    struct index_item_t: virtual us::gov::io::seriable {

        index_item_t() {}
        index_item_t(const track_t& track, const string& label, uint16_t state): track(track), label(label), state(state) {}

    public: //serialization
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;


    public:
        track_t track;
        string label;
        uint16_t state;

    };

    struct index_t: us::gov::io::seriable_vector<pair<track_t, index_item_t>> {
        void dump(const string& prefix, ostream&) const;
    };

    struct txlog_t: map<track_t, txlog_item_t> {
        using b = map<track_t, txlog_item_t>;

        txlog_t(): p(nullptr) {}
        txlog_t(protocol& p): p(&p) {}

        track_t store_new_invoice(const string& io_summary, uint16_t state_id, const string& state, tx_t*);
        track_t store_recv_invoice(const string& io_summary, uint16_t state_id, const string& state, tx_t*);
        track_t store_new_tx(const track_t&, const string& io_summary, uint16_t state_id, const string& state, tx_t*);

        index_t index() const;
        ko cancel(const track_t&);
        pair<ko, tx_t*> get_invoice(const track_t&) const; //returns a modifiable copy of the invoice that must be deleted by the caller

        void dump(const string& prefix, ostream&) const;

        mutable mutex mx;
        protocol* p;
    };

}}}}}

