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
#include "protocol.h"

#include <fstream>

#include <us/gov/io/shell_args.h>
#include <us/gov/io/cfg0.h>
#include <us/gov/engine/daemon_t.h>
#include <us/gov/cli/rpc_peer_t.h>

#include <us/wallet/engine/daemon_t.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/traders_t.h>
#include <us/wallet/wallet/local_api.h>

#include "business.h"

#define loglevel "wallet/trader/r2r/a2a"
#define logclass "protocol"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::r2r::a2a::protocol;

const char* c::rolestr() const { return "a"; }
const char* c::peer_rolestr() const { return "a"; }
const char* c::get_name() const { return name; }

c::protocol(business_t& bz): b(bz) {
    log("created protocol_a2a_a");
}

c::~protocol() {
    log("destroyed protocol_a2a_a");
}

bool c::requires_online(const string& cmd) const {
    if (b::requires_online(cmd)) return true;
    return false;
}

void c::help_online(const string& indent, ostream& os) const {
    b::help_online(indent, os);
}

void c::help_onoffline(const string& indent, ostream& os) const {
    b::help_onoffline(indent, os);
}

void c::help_show(const string& indent, ostream& os) const {
    b::help_show(indent, os);
}

blob_t c::push_payload(uint16_t pc) const {
    log("push_payload. code", pc);
    assert((uint16_t)push_begin >= (uint16_t)trader_protocol::push_end);
    assert(pc < push_end);
    if (pc < push_begin) {
        return b::push_payload(pc);
    }
    assert(false);
    blob_t blob;
    return move(blob);
}

ko c::exec_offline(const string& cmd0, ch_t& ch) {
    {
        auto r = b::exec_offline(cmd0, ch);
        if (r != trader_protocol::WP_29101) return move(r);
    }
    istringstream is(cmd0);
    string cmd;
    is >> cmd;
    if (cmd == "show") {
        log("show");
        string cmd;
        is >> cmd;
        if (cmd == "info") {
            log("show info");
            return ok;
        }
    }
    auto r = WP_29101;
    log(r);
    return move(r);
}

ko c::exec_online(peer_t& peer, const string& cmd0, ch_t& ch) {
    log("exec_online", cmd0);
    {
        auto r = b::exec_online(peer, cmd0, ch);
        if (r != WP_29101) return move(r);
    }
    log("exec_online", "here", cmd0);
    auto r = WP_29101;
    log(r);
    return move(r);
}

void c::exec_help(const string& prefix , ostream& os) {
    b::exec_help(prefix + "basic ", os);
    os << prefix << "info\n";
}

ko c::exec(istream& is, wallet::local_api& w) {
    string cmd;
    is >> cmd;
    if (cmd == "info") {
        return w.push_OK("info TBD.");
    }
    if (cmd == "basic") {
        return b::exec(is, w);
    }
    auto r = "KO 10918 Invalid command";
    log(r);
    return r;
}

#include <us/gov/io/cfg0.h>

ko c::trading_msg(peer_t& peer, svc_t svc, blob_t&& blob) {
    log("trading_msg");
    static_assert( (uint16_t)svc_begin >= (uint16_t)trader_protocol::svc_end);
    assert(svc < svc_end);
    if (svc < svc_begin) {
        return b::trading_msg(peer, svc, move(blob));
    }
    return trader_protocol::KO_29100;
}

void c::dump(ostream& os) const {
    os << "a2a a";
}

void c::list_trades_bit(ostream& os) const {
    os << "a2a a ";
}

namespace { namespace i18n {

    struct r_en_t;
    struct r_es_t;

    struct r_t: unordered_map<uint32_t, const char*> {
        using b =  unordered_map<uint32_t, const char*>;

        using b::unordered_map;

        const char* resolve(uint32_t n) const {
            log("string-res. resolve", n);
            auto i = find(n);
            if (i == end()) return begin()->second;
            return i->second;
        }

        static r_t& resolver(const string& lang);
    };

    struct r_en_t: r_t {
        using b = r_t;

        r_en_t(): b({
            {0, "KO 30920 Use i18n package in Lower Layer."},
            {1, "Talk and act."}, {2, "Interact via chat."},

        }) {
            //log("constructor 'en' string resources with", size(), "entries. Entry #4 is", resolve(4));
        }

    };

    struct r_es_t: r_t {
        using b = r_t;

        r_es_t(): b({
            {0, "KO 30920"},
            {1, "Hablar y actuar."}, {2, "Interacciona via chat."},

        }) {
            //log("constructor 'es' string resources with", size(), "entries. Entry #4 is", resolve(4));
        }

    };

    r_en_t r_en;
    r_es_t r_es;

    r_t& r_t::resolver(const string& lang) {
        if (lang == "es") return r_es;
        return r_en;
    }

}}

uint32_t c::trade_state_() const {
    log("trade_state");
    return 1;
}

void c::judge(const string& lang) {
    auto st = trade_state_();
    if (st != _trade_state.first) {
        if (st == 0) {
            b::judge(lang);
            return;
        }
        auto t = i18n::r_t::resolver(lang);
        auto r = t.resolve(st);
        log("trade_state", st, r);
        _trade_state = make_pair(st, r);
        _user_hint = t.resolve(_trade_state.first + 1);
    }
}

c::factory_id_t c::factory_id() const {
    return protocol_selection_t("a2a", "a");
}

size_t c::blob_size() const {
    size_t sz = b::blob_size();
    return sz;
}

void c::to_blob(blob_writer_t& writer) const {
    b::to_blob(writer);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = b::from_blob(reader);
        if (is_ko(r)) {
            return r;
        }
    }
    return ok;
}

