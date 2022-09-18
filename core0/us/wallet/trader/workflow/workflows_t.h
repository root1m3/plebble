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
#include <tuple>
#include <us/gov/io/seriable.h>
#include <us/wallet/trader/params_t.h>
#include "workflow_t.h"

namespace us::wallet::trader::workflow {
    using namespace std;

    struct workflows_t final: private vector<workflow_t*>, public us::gov::io::seriable {
        using doctypes_t = workflow_t::doctypes_t;
        using peer_t = workflow_t::peer_t;
        using ch_t = trader::ch_t;

        ~workflows_t();

    public:
        pair<workflow_t*, item_t*> find(const string& name) const;
        tuple<workflow_t*, item_t*, doc0_t*> read_item(const blob_t&) const;
        bool requires_online(const string& cmd) const;
        void help_online(const string& indent, ostream&) const;
        void help_onoffline(const string& indent, ostream&) const;
        void help_show(const string& indent, ostream&) const;
        void sig_reload(ostream&);
        void add(workflow_t*, ch_t&);
        bool sig_reset(ostream&);
        bool sig_hard_reset(ostream&);
        void on_file_updated(const string& path, const string& name, ch_t&);
        void recompute_doctypes(ch_t&);
        ko exec_offline(const string& cmd, ch_t&);
        ko exec_online(peer_t&, const string& cmd, ch_t&);
        ko rehome(const string& home, ch_t&);
        void doctypes(doctypes_t&) const; //consumer, producer
        ko update_item(const blob_t&, ch_t&);

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        string home;
    };

}

