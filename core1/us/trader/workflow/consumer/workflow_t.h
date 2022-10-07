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

#include <us/gov/engine/signed_data.h>
#include <us/gov/relay/pushman.h>

#include <us/wallet/trader/workflow/workflow_t.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/wallet/local_api.h>
#include <us/wallet/trader/workflow/item_t.h>

#include <us/trader/workflow/types.h>

namespace us::wallet::trader {
    struct traders_t;
}

namespace us::trader::workflow::consumer {

    using item_factory_id_t = us::wallet::trader::workflow::item_t::factory_id_t;

    template<typename t, item_factory_id_t fid>
    struct item_t final: us::wallet::trader::workflow::item_t {
        using b = us::wallet::trader::workflow::item_t;
        void init(us::wallet::trader::workflow::workflow_t* parent) { b::init(parent, doc_type::name, doc_type::long_name); }
        using doc0_t = us::wallet::trader::workflow::doc0_t;
        using doc_type = t;
        doc0_t* create_doc() const override { return new doc_type(); }
        doc_type* get_doc() { return static_cast<doc_type*>(doc); }
        magic_t my_magic() const { return doc_type::magic; }
        item_factory_id_t factory_id() const override { return fid; }
    };

}

namespace us::trader::workflow::consumer {

    struct workflow_t: us::wallet::trader::workflow::workflow_t {
        using b = us::wallet::trader::workflow::workflow_t;
        using bitem = us::wallet::trader::workflow::item_t;
        using trader_t = us::wallet::trader::trader_t;
        using traders_t = us::wallet::trader::traders_t;
        using workflow_item_factories_t = bitem::factories_t;

    protected:
        workflow_t();

    public:
        ~workflow_t() override;

    public:
        template<typename t, factory_id_t fid> bitem* add(ch_t& ch) {
            auto i = find(t::name);
            if (i != end()) {
                return i->second;
            }
            auto p = new item_t<t, fid>();
            p->init(this);
            emplace(t::name, p);
            if (ch.closed()) return p;
            if (home.empty()) {
                p->set(ch);
            }
            else {
                p->load(ch);
            }
            return p;
        }

        template<typename t> bitem* remove(ch_t& ch) {
            auto i = find(t::name);
            if (i == end()) {
                return nullptr;
            }
            i->second->unset(ch);
            delete i->second;
            erase(i);
            return nullptr;
        }

        static void exec_help(const string& prefix, ostream&);
        static ko exec(istream&, traders_t&, wallet::wallet::local_api&);

    public:
        static constexpr item_factory_id_t cat_factory_id{1};
        static constexpr item_factory_id_t inv_factory_id{2};
        static constexpr item_factory_id_t pay_factory_id{3};
        static constexpr item_factory_id_t rcpt_factory_id{4};
        void register_factories(item_factories_t&) const override;

    public:
        bitem* enable_catalogue(bool, ch_t&);
        bitem* enable_invoice(bool, ch_t&);
        bitem* enable_payment(bool, ch_t&);
        bitem* enable_receipt(bool, ch_t&);
    };

}

