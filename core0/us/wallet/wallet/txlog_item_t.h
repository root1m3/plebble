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
#include <memory>

#include <us/gov/cash/tx_t.h>
#include <us/gov/engine/track_status_t.h>

namespace us::wallet::wallet {

    using tx_t = us::gov::cash::tx_t;
    using track_t = ts_t;
    using trade_id_t = hash_t;

    enum wallet_evt_status_t: uint8_t {
        wevt_unknown,
        wevt_error,
        wevt_wait_rcpt_info,
        wevt_wait_signature,
        wevt_cancelled,
        wevt_delivered,

        wevt_num
    };
       
    static constexpr const char* wallet_evt_status_str[wevt_num] = {"unknown", "error", "wait_rcpt_info", "wait_signature", "cancelled", "delivered"};

    struct t1_t: us::gov::io::seriable {
        t1_t() {}
        t1_t(const cash_t& amount, const hash_t& coin);

    public:
        void dump(ostream&) const;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        hash_t coin;
        cash_t amount;
    };

    struct txlog_item_t {
        using gov_track_status_t = us::gov::engine::track_status_t;
        using gov_evt_status_t = us::gov::engine::evt_status_t;

    public:
        txlog_item_t(t1_t*, bool initiator, const trade_id_t&);

        bool req_pay() const { return !pay; }
        string title() const;

        void set_inv(tx_t*);
        void set_tx(tx_t*);

        void on_tx_tracking_status(const gov_track_status_t&, set<trade_id_t>& notify);

    public:
        bool initiator;
        string io_summary;
        wallet_evt_status_t wallet_evt_status{wevt_unknown};
        gov_evt_status_t gov_evt_status;
        string gov_evt_status_info;

        unique_ptr<t1_t> t1;
        unique_ptr<tx_t> inv;
        unique_ptr<tx_t> pay;

        trade_id_t trade_id;
    };

}

