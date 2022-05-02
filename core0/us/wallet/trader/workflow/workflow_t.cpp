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
#include "workflow_t.h"

#include <us/wallet/protocol.h>
#include <us/wallet/trader/business.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/traders_t.h>
#include <us/wallet/engine/daemon_t.h>

#include "trader_protocol.h"

#define loglevel "wallet/trader/workflow"
#define logclass "workflow_t"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::workflow::workflow_t;

c::workflow_t() {
}

c::~workflow_t() {
    lock_guard<mutex> lock(mx);
    for (auto& i: *this) delete i.second;
}

void c::on_file_updated(const string& path, const string& name, ch_t& ch) {
    log("on_file_updated", path, name, ch.to_string());
    lock_guard<mutex> lock(mx);
    auto i = find(name);
    if (i == end()) {
        log("WP 67853", name);
        return;
    }
    if (path != home) {
        log("WP 88574", path, home);
        return;
    }
    log("loading", name);
    i->second->load(ch);
    log("end of on_file_updated", path, name, ch.to_string());
}

ko c::load_all(ch_t& ch) {
    lock_guard<mutex> lock(mx);
    for (auto& i: *this) {
        log("loading", i.first);
        auto r = i.second->load(ch);
        if (is_ko(r)) {
            return move(r);
        }
    }
    return ok;
}

ko c::unload_all(ch_t& ch) {
    lock_guard<mutex> lock(mx);
    for (auto& i: *this) {
        log("unloading", i.first);
        auto r = i.second->unload(ch);
        if (is_ko(r)) {
            return move(r);
        }
    }
    return ok;
}

bool c::sig_reset(ostream& os) {
    lock_guard<mutex> lock(mx);
    bool ch = false;
    ostringstream x;
    x <<"forgotten ";
    for (auto& i: *this) {
        if (i.second->sig_reset(x)) {
            ch=true;
        }
    }
    if (ch) {
        os << x.str() << '\n';
    }
    return ch;
}

bool c::sig_hard_reset(ostream& os) {
    lock_guard<mutex> lock(mx);
    log("sig_hard_reset");
    bool b = false;
    string dir = datadir();
    ostringstream k;
    k << "Removed files: ";
    for (auto& i: *this) {
        if (i.second->sig_hard_reset(dir, k)) b = true;
    }
    if (b) {
        os << k.str() << '\n';
    }
    return b;
}

void c::sig_reload(ostream& os) {
}

void c::doctypes(doctypes_t& r) const {
    log("doctypes", size(), "workflow_items", "trace-w8i");
    lock_guard<mutex> lock(mx);
    for (auto& i: *this) {
        i.second->doctypes(r);
    }
}

void c::set(ch_t& ch) const {
    log("set");
    lock_guard<mutex> lock(mx);
    for (auto& i: *this) {
        log("  setting", i.first);
        i.second->set(ch);
    }
}

bool c::has_doc(const string& name) const {
    lock_guard<mutex> lock(mx);
    return find(name) != end();
}

ko c::push_(const_iterator i, bool compact) const {
    using datagram = us::gov::socket::datagram;
    if (i->second->doc == nullptr) {
        auto r = "KO 30947 Document not available.";
        log(r);
        return r;
    }
    auto& tder = trader();
    auto tid = tder.id;
    log("tid", tid);
    log("trade id", tder.id);
    datagram* d;
    if (compact) {
        blob_t blob;
        i->second->write(blob);
        d = peer_t::push_in_t(tid, workflow::trader_protocol::push_workflow_item, blob).get_datagram(tder.parent.daemon.channel, 0);
    }
    else {
        ostringstream os;
        i->second->doc->write_pretty(os);
        blob_t blob = io::blob_writer_t::make_blob(os.str());
        d = peer_t::push_in_t(tid, workflow::trader_protocol::push_doc, blob).get_datagram(tder.parent.daemon.channel, 0);
    }
    trader().schedule_push(d);
    return ok;
}

ko c::push(const string& name, bool compact) const {
    lock_guard<mutex> lock(mx);
    auto i = find(name);
    if (i == end()) {
        auto r = "KO 91824 Document doesn't exist in the current workflow.";
        log(r);
        return r;
    }
    return push_(i, compact);
}

ko c::send_to(peer_t& peer, const string& name) const {
    lock_guard<mutex> lock(mx);
    auto i = find(name);
    if (i == end()) {
        auto r = "KO 30948 Item not available.";
        log(r);
        return r;
    }
    return i->second->send_to(peer);
}

ko c::send_request_to(peer_t& peer, const string& name) const {
    lock_guard<mutex> lock(mx);
    auto i = find(name);
    if (i == end()) {
        auto r = "KO 30948 Item not available.";
        log(r);
        return r;
    }
    return i->second->send_request_to(peer);
}

ko c::save(const string& name) {
    lock_guard<mutex> lock(mx);
    auto wfi = find(name);
    if (wfi == end()) {
        auto r = "KO 71001 Invalid name.";
        log(r);
        return r;
    }
    return wfi->second->save();
}

void c::help_online(const string& indent, ostream& os) const {
    using fmt = trader_protocol;
    lock_guard<mutex> lock(mx);
    for_each(begin(), end(), [&](const auto& i) {
        if (i.second->mode == item_t::mode_send) {
            ostringstream x;
            x << "send " << i.second->name;
            fmt::twocol(indent, x.str(), string("Send over ") + i.second->long_name, os);
        }
        else {
            ostringstream x;
            x << "request " << i.second->name;
            fmt::twocol(indent, x.str(), string("Request ") + i.second->long_name, os);
        }
    });
}

void c::help_onoffline(const string& indent, ostream& os) const {
    using fmt = trader_protocol;
    lock_guard<mutex> lock(mx);
    string pfx = indent + "  \" ";
    for_each(begin(), end(), [&](const auto& i) {
        os << indent << i.second->name << " (" << i.second->long_name << ")\n";
        i.second->help_onoffline(pfx, os);
    });
}

void c::help_show(const string& indent, ostream& os) const {
    using fmt = trader_protocol;
    lock_guard<mutex> lock(mx);
    for_each(begin(), end(), [&](const auto& i) {
        fmt::twocol(indent, i.second->name + " [-p]", "Show " + i.second->long_name + " [pretty]", os);
    });
}

ko c::rehome(const string& dir, ch_t& ch) {
    log("rehome", home, "=>", dir);
    assert(!dir.empty());
    if (!home.empty()) {
        auto r = unload_all(ch);
        if (is_ko(r)) {
            return r;
        }
    }
    home = dir;
    log("home=", home);
    {
        auto r = load_all(ch);
        if (is_ko(r)) {
            return r;
        }
    }
    return ok;
}

ko c::exec_offline(const string& cmd0, ch_t& ch) {
    log("exec_offline", cmd0);
    istringstream is(cmd0);
    string cmd;
    is >> cmd;
    lock_guard<mutex> lock(mx);
    if (cmd == "show") {
        string cmd;
        is >> cmd;
        log("exec_online show", cmd);
        auto i = find(cmd);
        if (i == end()) {
            auto r = us::wallet::trader::trader_protocol::WP_29101;
            log(r);
            return move(r);
        }
        string pretty;
        is >> pretty;
        if (pretty == "-p") {
            return push_(i, false);
        }
        else if (pretty.empty()) {
            return push_(i, true);
        }
        else {
            auto r = "KO 22712 Invalid option.";
            log(r);
            return move(r);
        }
    }
    auto i = find(cmd);
    if (i == end()) {
        auto r = trader::trader_protocol::WP_29101;
        log(r);
        return move(r);
    }
    cmd.clear();
    getline(is, cmd);
    return i->second->exec_offline(home, cmd, ch);
}

bool c::requires_online(const string& cmd) const {
    if (cmd == "send") return true;
    if (cmd == "request") return true;
    return false;
}

ko c::exec_online(peer_t& peer, const string& cmd0, ch_t& ch) {
    log("exec_online", cmd0);
    istringstream is(cmd0);
    string cmd;
    is >> cmd;
    lock_guard<mutex> lock(mx);
    if (cmd == "send") {
        log("send");
        string cmd;
        is >> cmd;
        auto i = find(cmd);
        if (i == end()) {
            auto r = us::wallet::trader::trader_protocol::WP_29101;
            log(r);
            return move(r);
        }
        auto r = i->second->send_to(peer);
        if (is_ko(r)) {
            return move(r);
        }
        return trader().push_OK("Document has been sent to your peer.");
    }
    if (cmd == "request") {
        log("request");
        string cmd;
        is >> cmd;
        auto i = find(cmd);
        if (i == end()) {
            auto r = us::wallet::trader::trader_protocol::WP_29101;
            log(r);
            return move(r);
        }
        auto r = i->second->send_request_to(peer);
        if (is_ko(r)) {
            return move(r);
        }
        return trader().push_OK("Document requested. I expect it to be delivered...");
    }
    auto r = trader::trader_protocol::WP_29101;
    log(r);
    return move(r);
}

