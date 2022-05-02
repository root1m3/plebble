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
#include <us/gov/types.h>
#include <us/gov/engine/evidence.h>
#include <us/gov/engine/app.h>
#include <us/gov/peer/nodes_t.h>
#include <us/gov/peer/account_t.h>

#include "db_t.h"

namespace us { namespace gov { namespace peer {
    struct nodes_t;
}}}

namespace us { namespace gov { namespace engine {
    struct daemon_t;
}}}

namespace us { namespace gov { namespace traders {

    struct wallet_address;
    struct local_delta;
    struct delta;
    using wallets_t = peer::nodes_t;
    using account_t = peer::account_t;

    struct app: engine::app {
        using b = engine::app;
        using evidence = engine::evidence;
        using peer_t = engine::peer_t;

        static const char* KO_78101, *KO_73291, *KO_37628;

        static constexpr const char* name = {"traders"};
        static constexpr appid_t id() { return 40; }

        app(engine::daemon_t&);
        app(const app&) = delete;
        app& operator = (const app&) = delete;
        ~app() override;

        string get_name() const override { return name; }
        appid_t get_id() const override { return id(); }
        ko shell_command(istream&, ostream&) override;
        void import(const wallets_t& online);
        void import(const engine::app::delta&, const engine::pow_t&) override;
        engine::app::local_delta* create_local_delta() override;
        static pair<ko, evidence*> create_evidence(eid_t evid);
        bool process(const evidence&) override;
        void process(const wallet_address&);
        void clear() override;
        void dump(const string& prefix, int detail, ostream&) const override;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        db_t db;
        traders::local_delta* pool{nullptr};
        mutex mx_pool;

    };

}}}

