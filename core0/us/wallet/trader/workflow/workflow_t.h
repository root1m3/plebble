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
#include <string>
#include <map>

#include <us/gov/engine/signed_data.h>
#include <us/wallet/engine/peer_t.h>
#include <us/wallet/trader/kv.h>
#include <us/wallet/trader/ch_t.h>

#include "item_t.h"

namespace us { namespace wallet { namespace trader {

    struct trader_t;
    struct trader_protocol;

}}}

namespace us { namespace wallet { namespace trader { namespace workflow {

    struct workflows_t;

    struct workflow_t: map<string, item_t*> {
        using b = map<string, item_t*>;

        using peer_t = engine::peer_t;
        using trader_t = us::wallet::trader::trader_t;
        using trader_protocol = us::wallet::trader::trader_protocol;
        using doctypes_t = item_t::doctypes_t;
        using ch_t = trader::ch_t;

        workflow_t();
        virtual ~workflow_t();

        const string& datadir() const { return home; }
        virtual trader_t& trader() const = 0;
        bool requires_online(const string& cmd) const;
        ko exec_online(peer_t&, const string& cmd, ch_t&);
        ko exec_offline(const string& cmd, ch_t&);
        void help_online(const string& indent, ostream&) const;
        void help_onoffline(const string& indent, ostream&) const;
        void help_show(const string& indent, ostream&) const;
        void on_file_updated(const string& path, const string& name, ch_t&);
        ko load_all(ch_t&);
        ko unload_all(ch_t&);
        ko save(const string& name);
        //ko load(const string& name, istream&, ch_t&);
        void set(ch_t&) const;
        bool sig_hard_reset(ostream&);
        bool sig_reset(ostream&);
        void sig_reload(ostream&);
        bool has_doc(const string& name) const;
        ko rehome(const string& dir, ch_t&);
        void doctypes(doctypes_t&) const; //consumer, producer
        ko send_to(peer_t&, const string& name) const;
        ko send_request_to(peer_t&, const string& name) const;
        ko push(const string& name, bool compact) const;
        ko push_(const_iterator, bool compact) const;

    public:
        workflows_t* parent{nullptr};
        string home;
        mutable mutex mx;
    };

}}}}



