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
#include <us/gov/types.h>
#include <us/gov/engine/evidence.h>
#include <us/gov/engine/app.h>
#include "db_t.h"

namespace us { namespace gov { namespace engine {
    struct daemon_t;
    struct peer_t;
}}}

namespace us { namespace gov { namespace sys {

    struct install_script_response;
    struct install_script;

    struct app: engine::app {
        using b = engine::app;
        using peer_t = engine::peer_t;
        using evidence = engine::evidence;

        static pubkey_t automatic_updates_subscription;

        static constexpr const char* name = {"sys"};
        static constexpr appid_t id() { return 1; }

        app(engine::daemon_t&);
        app(const app&) = delete;
        app& operator = (const app&) = delete;
        ~app() override;

        engine::app::local_delta* create_local_delta() override;
        static pair<ko, evidence*> create_evidence(eid_t evid);
        string get_name() const override { return name; }
        void import(const engine::app::delta&, const engine::pow_t&) override;
        bool process(const evidence&) override;
        bool process(const install_script&);
        bool process(const install_script_response&);
        void clear() override;
        void dump_policies(ostream&) const;
        appid_t get_id() const override { return id(); }
        ko shell_command(istream&, ostream&) override;
        void maintenance(int code, const hash_t& jobid);
        void send_response(const string& response, const hash_t& job);
        string exec(const string& cmd) const;
        void dump(const string& prefix, int detail, ostream&) const override;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        string src_hash;
        string us_cash_android_hash;

        db_t db;
    };

}}}


