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
#include <vector>
#include <set>

#include <us/gov/types.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/engine/app.h>
#include <us/gov/engine/evidence.h>
#include <us/gov/engine/peer_t.h>

#include "tx_t.h"
#include "accounts_t.h"
#include "db_t.h"
#include "counters_t.h"
#include "locking_program_input_t.h"

namespace us { namespace gov { namespace cash {

    struct map_tx;
    struct file_tx;
    struct ttx;
    struct local_delta;
    struct delta;

    struct app final: engine::app {
        using b = engine::app;
        using evidence = us::gov::engine::evidence;
        using peer_t = us::gov::engine::peer_t;
        using datagram = us::gov::socket::datagram;
        using batch_t = accounts_t::batch_t;
        using local_delta = us::gov::cash::local_delta;
        using delta = us::gov::cash::delta;

        static constexpr const char* name = {"cash"};
        static constexpr appid_t id() { return 30; }
        static constexpr locking_program_t locking_program_id{1};
        static const char* KO_30928;

        static bool unlock(const hash_t& address, const locking_program_t&, const locking_program_input_t&, const tx_t&);
        static bool unlock(const hash_t& address, const locking_program_t&, const locking_program_input_t&, const map_tx&);
        static bool unlock(const hash_t& address, const locking_program_t&, const locking_program_input_t&, const file_tx&);
        static bool unlock(const hash_t& address, const locking_program_t&, const locking_program_input_t&, const ttx&);
        static pair<ko, evidence*> create_evidence(eid_t evid);

        static bool check_input(const hash_t& address, const tx_t&, const locking_program_input_t&);

    public:
        app(engine::daemon_t&);
        app(const app&) = delete;
        app& operator = (const app&) = delete;
        ~app() override;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public: // engine
        string get_name() const override { return name; }
        void clear() override;
        appid_t get_id() const override { return id(); }
        ko shell_command(istream&, ostream&) override;
        void import(const engine::app::delta&, const engine::pow_t&) override;
        void update_ledger(const delta& g, vector<pair<hash_t, uint32_t>>& old_files, vector<pair<hash_t, uint32_t>>& new_files);
        engine::app::local_delta* create_local_delta() override;
        bool account_state(const batch_t& batch, const hash_t& address, account_t&) const;
        bool process(const evidence&) override;
        bool process_tx(peer_t*, datagram*);

    public: /// tx_t
        bool process(const tx_t&, const tx::section_t&, batch_t&, cash_t& fee);
        bool process(const tx_t&, const tx::sections_t&, batch_t&, cash_t& fee);
        bool process(const tx_t&);
        void report_wallet(const tx_t& t, const string& feedback) const;
        static bool check_amounts(const tx_t&);

    public: /// map_t
        bool process(const map_tx&);

    public: /// file_t
        bool process(const file_tx&);
        void get_files(vector<pair<hash_t, uint32_t>>&) const;
        void get_files(vector<pair<hash_t, fileattr_t>>&) const;
        void get_files(set<hash_t>&) const;

    public: /// ttx_t
        bool process(const ttx&);

    public: /// cryptoeconomics
        cash_t charge_ledger();
        void tax_accounts(const cash_t& amount);
        cash_t pay_services() const;
        void process_profit(cash_t& profit);
        void pay_subsidy(const cash_t& amount, const engine::pow_t&);
        void dump(const string& prefix, int detail, ostream&) const override;

    public:
        db_t db;
        local_delta* pool{nullptr};
        mutex mx_pool;
        vector<pair<hash_t, uint32_t>> old_files;
        vector<pair<hash_t, uint32_t>> new_files;

        #if CFG_COUNTERS == 1
            using counters_t = us::gov::cash::counters_t;
            static counters_t counters;
        #endif
    };

}}}

