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
#include <vector>
#include <string>
#include <map>

#include <us/gov/engine/signed_data.h>
#include <us/gov/io/seriable.h>
#include <us/gov/io/factory.h>

#include <us/wallet/engine/peer_t.h>
#include <us/wallet/trader/kv.h>
#include <us/wallet/trader/ch_t.h>

#include "item_t.h"

namespace us::wallet::trader {
    struct trader_t;
    struct trader_protocol;
}

namespace us::wallet::trader::workflow {

    struct workflows_t;

    struct workflow_t: map<string, item_t*>, us::gov::io::seriable {
        using b = map<string, item_t*>;
        using peer_t = engine::peer_t;
        using trader_t = us::wallet::trader::trader_t;
        using trader_protocol = us::wallet::trader::trader_protocol;
        using doctypes_t = item_t::doctypes_t;
        using ch_t = trader::ch_t;

    protected: //create instances using workflow_t::factories.create(); 
        workflow_t();

    public:
        virtual ~workflow_t();
        virtual void init(workflows_t& parent_, ch_t&);

    public:
        using factories_t = us::gov::io::factories_t<workflow_t>;
        using factory_t = us::gov::io::factory_t<workflow_t>;
        using factory_id_t = uint8_t;

        static factory_id_t null_instance;
        virtual factory_id_t factory_id() const = 0;

    public:
        const string& datadir() const { return home; }
        bool requires_online(const string& cmd) const;
        ko exec_online(trader_t&, peer_t&, const string& cmd, ch_t&);
        ko exec_offline(trader_t&, const string& cmd, ch_t&);
        void help_online(const string& indent, ostream&) const;
        void help_onoffline(const string& indent, ostream&) const;
        void help_show(const string& indent, ostream&) const;
        void on_file_updated(const string& path, const string& name, ch_t&);
        ko load_all(ch_t&);
        ko unload_all(ch_t&);
        ko save(const string& name);
        void set(ch_t&) const;
        bool sig_hard_reset(ostream&);
        bool sig_reset(ostream&);
        void sig_reload(ostream&);
        bool has_doc(const string& name) const;
        ko rehome(const string& dir, ch_t&);
        void doctypes(doctypes_t&) const; //consumer, producer
        ko send_to(trader_t&, peer_t&, const string& name) const;
        ko send_request_to(trader_t&, peer_t&, const string& name) const;

    private:
        ko push_(trader_t&, const_iterator, bool compact) const;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        using item_factories_t = item_t::factories_t;
        using item_factory_t = item_t::factory_t;
        using item_factory_id_t = item_t::factory_id_t;

        virtual void register_factories(item_factories_t&) const = 0;

    public:
        item_factories_t item_factories;

    public:
        workflows_t* parent{nullptr};
        string home;
        mutable mutex mx;
    };

}

