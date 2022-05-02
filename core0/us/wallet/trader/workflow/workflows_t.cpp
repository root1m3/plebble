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
#include "workflows_t.h"

#include <us/gov/socket/datagram.h>
#include <us/wallet/trader/trader_t.h>

#include "doc_t.h"

#define loglevel "wallet/trader/workflow"
#define logclass "workflows_t"
#include <us/gov/logs.inc>

using namespace us::wallet::trader::workflow;
using c = us::wallet::trader::workflow::workflows_t;

c::~workflows_t() {
    for (auto& i: *this) delete i;
}

bool c::requires_online(const string& cmd) const {
    for (auto& i: *this) {
        if (i->requires_online(cmd)) return true;
    }
    return false;
}

void c::on_file_updated(const string& path, const string& name, ch_t& ch) {
    log("on_file_updated", name, path);
    assert(!ch.closed());
    for (auto& i: *this) {
        i->on_file_updated(path, name, ch);
    }
}

pair<workflow_t*, item_t*> c::find(const string& name) const {
    for (auto& i: *this) {
        auto wfi = i->find(name);
        if (wfi != i->end()) {
            return make_pair(i, wfi->second);
        }
    }
    return make_pair(nullptr, nullptr);
}

tuple<workflow_t*, item_t*, doc0_t*> c::read_item(const blob_t& blob) const {
    log("read_item. blob sz", blob.size());
    string name;
    us::gov::io::blob_reader_t reader(blob);
    {
        auto r = reader.read(name);
        if (is_ko(r)) {
            return make_tuple(nullptr, nullptr, nullptr);
        }
    }
    log("read_item", name);
    auto i = find(name);
    if (i.second == nullptr) {
        return make_tuple(nullptr, nullptr, nullptr);
    }
    auto r = i.second->doc_from_blob(reader);
    if (is_ko(r.first)) {
        assert(r.second == nullptr);
        return make_tuple(nullptr, nullptr, nullptr);
    }
    log("returning doc", r.second);
    return make_tuple(i.first, i.second, r.second);
}


ko c::rehome(const string& dir, ch_t& ch) {
    log("set workflows home", dir);
    assert(!dir.empty());
    if (home == dir) {
        return ok;
    }
    home = dir;
    for (auto& i: *this) {
        auto r = i->rehome(home, ch);
        if (is_ko(r)) {
            return r;
        }
    }
    return ok;
}

void c::recompute_doctypes(ch_t& ch) {
    log("recompute doctypes", size(), "workflows", "trace-w8i");
    workflow_t::doctypes_t v;
    for (auto& i: *this) {
        i->doctypes(v);
    }
    log("set_params_workflow", v.first.size(), v.second.size(), "trace-w8i");
    v.set(ch);
}

void c::add(workflow_t* wf, ch_t& ch) {
    log("add workflow. home", home);
    assert(wf != nullptr);
    assert(wf->parent == nullptr);
    wf->parent = this;
    wf->home = home;
    emplace_back(wf);
    log("set_params_new_workflow");
    if (!home.empty()) {
        wf->load_all(ch);
    }
    recompute_doctypes(ch);
}

void c::help_online(const string& indent, ostream& os) const {
    for (auto& i: *this) {
        i->help_online(indent, os);
    }
}

void c::help_onoffline(const string& indent, ostream& os) const {
    for (auto& i: *this) {
        i->help_onoffline(indent, os);
    }
}

void c::help_show(const string& indent, ostream& os) const {
    for (auto& i: *this) {
        i->help_show(indent, os);
    }
}

ko c::exec_offline(const string& cmd, ch_t& ch) {
    log("exec_offline");
    for (auto& i: *this) {
        auto r = i->exec_offline(cmd, ch);
        if (r == trader::trader_protocol::WP_29101) {
            continue;
        }
        if (is_ko(r)) {
            return move(r);
        }
        return move(r);
    }
    auto r = trader::trader_protocol::WP_29101;
    log(r);
    return move(r);
}

ko c::exec_online(peer_t& peer, const string& cmd, ch_t& ch) {
    log("exec_online");
    for (auto& i: *this) {
        auto r = i->exec_online(peer, cmd, ch);
        if (is_ok(r)) {
            return r;
        }
        if (r != trader::trader_protocol::WP_29101) return r;
    }
    auto r = trader::trader_protocol::WP_29101;
    log(r);
    return move(r);
}

bool c::sig_reset(ostream& os) {
    bool ch{false};
    for (auto& wf: *this) {
        ch |= wf->sig_reset(os);
    }
    return ch;
}

void c::sig_reload(ostream& os) {
    for (auto& wf: *this) {
        wf->sig_reload(os);
    }
}

bool c::sig_hard_reset(ostream& os) {
    bool ch{false};
    for (auto& wf: *this) {
        ch |= wf->sig_hard_reset(os);
    }
    return ch;
}

void c::doctypes(doctypes_t& r) const {
    for (auto& wf: *this) {
        wf->doctypes(r);
    }
}

