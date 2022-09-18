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
#include "item_t.h"

#include <fstream>

#include <us/gov/socket/datagram.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/protocol.h>
#include <us/wallet/trader/business.h>

#include "workflow_t.h"
#include "trader_protocol.h"
#include "workflows_t.h"

#define loglevel "wallet/trader/workflow"
#define logclass "item_t"
#include <us/gov/logs.inc>

using namespace us::wallet::trader::workflow;
using c = us::wallet::trader::workflow::item_t;

us::gov::io::factories_t<c> c::factories;

c::item_t(workflow_t* parent, const string& name, const string& long_name): parent(parent), name(name), long_name(long_name) {
}

c::~item_t() {
    delete doc;
}

void c::set(ch_t& ch) const {
    ostringstream os;
    os << "wf_" << name;
    string n = os.str();
    log("set", n);
    ch.shared_params_changed |= ch.local_params->shared.set(n, (doc != nullptr ? "Y" : "N"));
    {
        ostringstream os2;
        os2 << n << "_mode";
        ch.shared_params_changed |= ch.local_params->shared.set(os2.str(), modestr[mode]);
    }
    if (doc == nullptr) {
        auto d = create_doc();
        d->unset(n, ch);
        delete d;
        return;
    }
    doc->set(n, ch);
}

void c::unset(ch_t& ch) const {
    ostringstream os;
    os << "wf_" << name;
    string n = os.str();
    log("unset", n);
    ch.shared_params_changed |= ch.local_params->shared.unset(n);
    {
        ostringstream os2;
        os << n << "_mode";
        ch.shared_params_changed |= ch.local_params->shared.unset(os2.str());
    }
    if (doc == nullptr) {
        auto d = create_doc();
        d->unset(n, ch);
        delete d;
        return;
    }
    doc->unset(n, ch);
}

void c::replace_doc(doc0_t* d, ch_t& ch) {
    log("replace_doc", this, d);
    delete doc;
    doc = d;
    save();
    set(ch);
    ch.file_updated(parent->home, name);
}

void c::doctypes(doctypes_t& v) const { //consumer, producer
    magic_t m = my_magic();
    log("doctypes", name, ", my magic is ", m, ", my mode is", modestr[mode], "trace-w8i");
    if (mode == mode_send) {
        v.second.emplace_back(m);
    }
    else {
        v.first.emplace_back(m);
    }
}

void c::doctypes_t::set(ch_t& ch) const {
    log("setting doctypes");
    assert(!ch.closed());
    bool r{false};
    {
        ostringstream os;
        for (auto& i: first) { //consumer
            os << i << ' ';
        }
        log("set doctypes_consumer", os.str());
        ch.shared_params_changed |= ch.local_params->shared.set("doctypes_consumer", os.str());
    }
    {
        ostringstream os;
        for (auto& i: second) { //producer
            os << i << ' ';
        }
        log("set doctypes_producer", os.str());
        ch.shared_params_changed |= ch.local_params->shared.set("doctypes_producer", os.str());
    }
}

void c::set_mode(mode_t m, ch_t& ch) {
    log("set_mode", name, modestr[m], " <- ", modestr[mode], "trace-w8i");
    if (m == mode) return;
    mode = m;
    ostringstream os;
    os << "wf_" << name << "_mode";
    ch.shared_params_changed |= ch.local_params->shared.set(os.str(), modestr[mode]);
    parent->parent->recompute_doctypes(ch);
}

ko c::load(const string& filename, ch_t& ch) {
    log("load", filename);
    auto r = b::load(filename);
    set(ch);
    return r;
}

ko c::unload(ch_t& ch) {
    log("unload", filename());
    if (doc == nullptr) {
        return ok;
    }
    delete doc;
    doc = nullptr;
    set(ch);
    return ok;
}

string c::filename() const {
    ostringstream os;
    assert(!parent->home.empty());
    os << parent->home << '/' << name;
    log("filename", os.str());
    return os.str();
}

ko c::save() const {
    log("save");
    if (parent->home.empty()) {
        auto r = "KO 49300 Parnt home is empty";
        log(r);
        return r;
    }
    return b::save(filename());
}

ko c::tamper(const string& w0, const string& w1) {
    log("tamper");
    if (doc == nullptr) {
        auto r = "KO 69968 Nothing to tamper.";
        log(r);
        return r;
    }
    if (!doc->tamper(w0, w1)) {
        auto r = "KO 33022 Word not found in doc.";
        log(r);
        return r;
    }
    save();
    return ok;
}

bool c::verify(ostream& os) const {
    log("verify");
    if (doc == nullptr) {
        log("doc not available");
        return false;
    }
    return doc->verify(os);
}

bool c::verify() const {
    log("verify");
    if (doc == nullptr) {
        log("doc not available");
        return false;
    }
    return doc->verify();
}

ko c::send_to(peer_t& peer) const {
    log("send_to");
    if (doc == nullptr) {
        auto r = "KO 30918 Document not available.";
        log(r);
        return r;
    }
    if (mode != mode_send) {
        auto r = "KO 30421 Document not expected to be sent.";
        log(r);
        return r;
    }
    blob_t blob;
    write(blob);
    return peer.call_trading_msg(peer_t::trading_msg_in_t(parent->trader().id, workflow::trader_protocol::svc_workflow_item, blob));
}

ko c::send_request_to(peer_t& peer) const {
    if (mode != mode_recv) {
        auto r = "KO 30931 Document not expected to be received.";
        log(r);
        return r;
    }
    log("ordering doc");
    blob_t blob;
    {
        blob_writer_t w(blob, blob_writer_t::blob_size(name));
        w.write(name);
    }
    return peer.call_trading_msg(peer_t::trading_msg_in_t(parent->trader().id, workflow::trader_protocol::svc_workflow_item_request, blob));
}

bool c::sig_reset(ostream& os) {
    if (doc == nullptr) return false;
    delete doc;
    doc = nullptr;
    os << name << ' ';
    return true;
}

bool c::sig_hard_reset(const string& dir, ostream& os) {
    string fqn = dir + '/' + name;
    if (us::gov::io::cfg0::file_exists(fqn)) {
        ::remove(fqn.c_str());
        os << name << ' ';
        return true;
    }
    return false;
}

void c::help_onoffline(const string& indent, ostream& os) const {
    using fmt = trader_protocol;
    fmt::twocol(indent, "save <filename>", string("Save doc ") + long_name + " to disk", os);
    fmt::twocol(indent, "load <filename>", string("Load doc ") + long_name + " from disk", os);
    fmt::twocol(indent, "isave <filename>", string("Save item ") + long_name + " to disk", os);
    fmt::twocol(indent, "iload <filename>", string("Load item ") + long_name + " from disk", os);
    fmt::twocol(indent, "tamper <w0> <w1>", "Change (maliciously) the document", "[Academic/Demo purposes]", os);
}

ko c::exec_offline(const string& home, const string& cmd0, ch_t& ch) {
    istringstream is(cmd0);
    string cmd;
    is >> cmd;
    if (cmd == "isave") {
        string fname;
        is >> fname;
        if (fname.empty()) {
            fname = filename();
        }
        auto r = b::save(fname);
        if (is_ko(r)) {
            return r;
        }
        ostringstream os;
        os << "Item saved as " << fname;
        return parent->trader().push_OK(os.str());
    }
    if (cmd == "iload") {
        string fname;
        is >> fname;
        if (fname.empty()) {
            fname = filename();
        }
        auto r = load(fname, ch);
        if (is_ko(r)) {
            return move(r);
        }
        ostringstream os;
        os << "Item loaded from file " << fname;
        return parent->trader().push_OK(os.str());
    }
    if (cmd == "save") {
        string fname;
        is >> fname;
        if (fname.empty()) {
            fname = filename();
        }
        if (doc == nullptr) {
            auto r = "KO 40393 doc unavailable";
            log(r);
            return r;
        }
        auto r = doc->save(fname);
        if (is_ko(r)) {
            return r;
        }
        ostringstream os;
        os << "Document saved as " << fname;
        return parent->trader().push_OK(os.str());
    }
    if (cmd == "load") {
        string fname;
        is >> fname;
        if (fname.empty()) {
            fname = filename();
        }
        auto* d = create_doc();
        assert(d != nullptr);
        auto r = d->load(fname);
        if (is_ko(r)) {
            delete d;
            return move(r);
        }
        replace_doc(d, ch);
        ostringstream os;
        os << "Document loaded from file " << fname;
        return parent->trader().push_OK(os.str());
    }
    if (cmd == "tamper") {
        string w0, w1;
        is >> w0;
        is >> w1;
        if (is.fail()) {
            auto r = "KO 40339 Required 2 words.";
            log(r);
            return move(r);
        }
        auto r = tamper(w0, w1);
        if (is_ko(r)) {
            return r;
        }
        return parent->trader().push_OK("Document has been modified.");
    }
    auto r = trader::trader_protocol::WP_29101; //"WP 40392 Invalid workflow-item command";
    log(r);
    return move(r);
}

void c::api_list(const string& scope, multimap<string, pair<string, string>>& items) const {
}

ko c::api_exec(const string& scope, const string& fn, const string& args, ostream& os) {
    return business_t::KO_50100;
}

pair<ko, doc0_t*> c::doc_from_blob(blob_reader_t& reader) const {
    uint8_t hasdoc;
    {
        auto r = reader.read(hasdoc);
        if (is_ko(r)) return make_pair(r, nullptr);
    }
    if (hasdoc == 0) {
        log("blob brough null doc");
        return make_pair(ok, nullptr);
    }
    auto* d = create_doc();
    assert(d != nullptr);
    {
        auto r = reader.read(*d);
        if (is_ko(r)) {
            delete d;
            return make_pair(r, nullptr);
        }
    }
    ostream nullos(0);
    if (!d->verify(nullos)) {
        auto r = "KO 33201 Document doesn't verify.";
        log(r);
        delete d;
        return make_pair(r, nullptr);
    }
    return make_pair(ok, d);
}

size_t c::blob_size() const {
    size_t sz = blob_writer_t::blob_size(name) + 1;
    if (doc != nullptr) sz += blob_writer_t::blob_size(*doc);
    return sz;
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(name);
    uint8_t hasdoc = doc == nullptr ? 0 : 1;
    writer.write(hasdoc);
    if (doc != nullptr) writer.write(*doc);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        if (name.empty()) {
            auto r = reader.read(name);
            if (is_ko(r)) return r;
        }
        else {
            string n;
            auto r = reader.read(n);
            if (is_ko(r)) return r;
            if (n != name) {
                auto r = "KO 40329 Workflow item name mismatch";
                log(r, n, name);
                return r;
            }
        }
    }
    {
        auto r = doc_from_blob(reader);
        if (is_ko(r.first)) {
            assert(r.second == nullptr);
            return r.first;
        }
        assert(r.second != nullptr);
        delete doc;
        doc = r.second;
    }
    return ok;
}


