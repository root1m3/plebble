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

#include "collusion_control_t.h"
#include "db_t.h"
#include "maskcoord_t.h"
#include "counters_t.h"

namespace test {
    struct auth_app;
}

namespace us::gov::engine {
    struct daemon_t;
    struct acl_tx;
}

namespace us::gov::engine::auth {

    using nodes_t = peer::nodes_t;
    using account_t = peer::account_t;

    struct local_delta;
    struct delta;
    struct node_address_tx;

    struct app: engine::app {
        using b = engine::app;
        friend struct ::test::auth_app;

        static constexpr const char* name = {"auth"};
        static constexpr appid_t id() { return 20; }

    #if CFG_TEST == 1
        static size_t cfg_shard_size;
    #else
        static constexpr size_t cfg_shard_size{CFG_SHARD_SIZE};
    #endif

        static const char* KO_78101, *KO_73291, *KO_31092;

        #ifdef CFG_PERMISSIONED_NETWORK
            static pub_t node_master_pubkey;
        #endif

        #if CFG_COUNTERS == 1
            static counters_t counters;
        #endif

    public:
        app(engine::daemon_t&);
        app(const app&) = delete;
        app& operator = (const app&) = delete;
        ~app() override;

    public:
        string get_name() const override { return name; }
        appid_t get_id() const override { return id(); }
        ko shell_command(istream&, ostream&) override;
        void import(const nodes_t& online);
        void import(const engine::app::delta&, const engine::pow_t&) override;
        engine::app::local_delta* create_local_delta() override;
        static pair<ko, evidence*> create_evidence(eid_t evid);
        bool process(const evidence&) override;
        void process(const node_address_tx&);
        bool report_node(const hash_t& pkh);
        void clear() override;
        void basic_auth_completed(peer_t* p, pport_t rpport);
        peer_t::stage_t my_stage() const;
        inline bool is_node() const { return my_stage() == peer_t::node; }
        const keys& get_keys() const;

    public:
        void dump(const string& prefix, int detail, ostream&) const override;

    public:
        #ifdef CFG_PERMISSIONLESS_NETWORK
            void add_growth_transactions(unsigned int seed);
            void layoff(nodes_t& n, uint16_t cut);
            void layoff();
            void shard();
        #endif

        #ifdef CFG_PERMISSIONED_NETWORK
            bool account_state(const hash_t& pubkeyh, host_t&, port_t&, peer_t::stage_t& stage) const;
            bool account_state(const nodes_t&, const hash_t& pubkeyh, host_t&, port_t&) const;
            bool account_state(const auth::local_delta&, const hash_t& pubkeyh, host_t&, port_t&, peer_t::stage_t& stage) const;
            bool process(const acl_tx&);
        #endif

    private:
        size_t blob_size() const final override;
        void to_blob(blob_writer_t&) const final override;

    public:
        ko from_blob(blob_reader_t&) final override;

    public:
        double growth{0.01};
        int min_growth{1};
        uint16_t layoff_days{LAYOFF_DAYS};
        collusion_control_t collusion_control;
        const pub_t& node_pubkey;
        db_t db;
        engine::auth::local_delta* pool{nullptr};
        mutex mx_pool;
        mutable peer_t::stage_t cache_my_stage{peer_t::unknown};

        maskcoord_t maskcoord;

    private:
        mutable unique_lock<mutex> lock_nodes;
        mutable unique_lock<mutex> lock_hall;
    };

}

