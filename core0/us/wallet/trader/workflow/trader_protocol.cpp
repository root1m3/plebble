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
#include <us/gov/io/cfg0.h>
#include <us/gov/socket/datagram.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/cert/doc0_t.h>

//#include "doc_t.h"
#include "trader_protocol.h"

#define loglevel "wallet/trader/workflow"
#define logclass "trader_protocol"
#include <us/gov/logs.inc>

using namespace std;
using namespace us::wallet;
using namespace us::wallet::trader::workflow;
using c = us::wallet::trader::workflow::trader_protocol;
using us::ko;
using us::gov::io::blob_t;

c::trader_protocol(business_t& bz): b(bz), _workflows(get_business()) {
}

c::~trader_protocol() {
}

bool c::requires_online(const string& cmd) const {
    if (b::requires_online(cmd)) return true;
    if (_workflows.requires_online(cmd)) return true;
    return false;
}

void c::help_online(const string& indent, ostream& os) const {
    b::help_online(indent, os);
    _workflows.help_online(indent, os);
}

void c::help_onoffline(const string& indent, ostream& os) const {
    b::help_onoffline(indent, os);
    twocol(indent, "show redirects", "Show redirects suggested by peer.", os);
    twocol(indent, "copyredirect <#ordinal>", "Copy bookmark from redirects. (see 'show redirects' command)", os);
    twocol(indent, "followredirect <#ordinal>", "Create a child trade on the same working dir. (see 'show redirects' command)", os);
    _workflows.help_onoffline(indent, os);
}

void c::help_show(const string& indent, ostream& os) const {
    b::help_show(indent, os);
    _workflows.help_show(indent, os);
}

void c::on_file_updated(const string& path, const string& name, ch_t& ch) {
    log("on_file_updated", path, name, ch.to_string());
    unique_lock lock(_local_params_mx, std::defer_lock);
    bool unlock{false};
    if(ch.closed()) {
        lock.lock();
        unlock = true;
    }
    _workflows.on_file_updated(path, name, ch);
    if (unlock) {
        lock.unlock();
    }
    log("end of on_file_updated", path, name, ch.to_string());
}

ko c::rehome_workflows(ch_t& ch) {
    assert(!ch.closed());
    return _workflows.rehome(datadir(_local_params), ch);
}

ko c::rehome(ch_t& ch) {
    auto r = b::rehome(ch);
    return rehome_workflows(ch);
}

ko c::on_attach(trader_t& tder_, ch_t& ch) {
    create_workflows(ch);
    auto r = b::on_attach(tder_, ch);
    if (is_ko(r)) {
        return r;
    }
    init_workflows(ch);
    _workflows.recompute_doctypes(ch);
    return ok;
}

ko c::exec_offline(const string& cmd0, ch_t& ch) {
    string lang = "en";
    {
        istringstream is(cmd0);
        string cmd;
        is >> cmd;
        if (cmd == "show") {
            string cmd;
            is >> cmd;
            if (cmd == "redirects") {
                //ch.no_need_to_update_devices();
                schedule_push(push_redirects);
                return ok;
            }
        }
        if (cmd == "copyredirect") {
            int i = -1;
            is >> i;
            log("copyredirect", i);
            --i;
            string name;
            bookmark_t bm;
            {
                lock_guard<mutex> lock(redirects_mx);
                auto it = redirects.begin();
                if (i < 0) {
                    auto r = "KO 76904 Invalid Ordinal. [check 'show redirects']";
                    log(r, i);
                    return r;
                }
                advance(it, i);
                if (it == redirects.end()) {
                    auto r = "KO 76905 Invalid Ordinal. [check 'show redirects']";
                    log(r, i);
                    return r;
                }
                name = it->first;
                bm = it->second;
            }
            auto r = tder->create_bookmark(name, bm);
            if (is_ko(r)) return r;
            schedule_push(us::wallet::trader::trader_t::push_bookmarks);
            return tder->push_OK("Bookmark added.");
        }
        if (cmd == "followredirect") {
            int i = -1;
            is >> i;
            log("followredirect", i);
            --i;
            string name;
            bookmark_t bm;
            {
                lock_guard<mutex> lock(redirects_mx);
                auto it = redirects.begin();
                if (i < 0) {
                    auto r = "KO 76904 Invalid Ordinal. [check 'show redirects']";
                    log(r, i);
                    return r;
                }
                advance(it, i);
                if (it == redirects.end()) {
                    auto r = "KO 76905 Invalid Ordinal. [check 'show redirects']";
                    log(r, i);
                    return r;
                }
                name = it->first;
                bm = it->second;
            }
            pair<ko, hash_t> child_trade = tder->initiate(move(bm.qr));
            if (is_ko(child_trade.first)) return child_trade.first;
            {
                ostringstream os;
                os << "Trade forks from " << tder->id << ". Child trade is " << child_trade.second;
                return tder->push_OK(os.str());
            }
        }
    }

    {
        auto r = _workflows.exec_offline(*tder, cmd0, ch);
        if (is_ok(r)) {
            return move(r);
        }
        if (r != WP_29101) {
            return move(r);
        }
    }
    {
        auto r = b::exec_offline(cmd0, ch);
        if (is_ok(r)) {
            return move(r);
        }
        if (r != WP_29101) {
            return move(r);
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
        if (r != WP_29101) {
            return move(r);
        }
    }
    {
        auto r = _workflows.exec_online(*tder, peer, cmd0, ch);
        if (r != WP_29101) {
            if (is_ok(r)) { //callbacks: on_send_item
                istringstream is(cmd0);
                string cmd;
                is >> cmd;
                if (cmd == "send") {
                    string what;
                    is >> what;
                    on_send_item(what); //app, ...
                }
            }
            return move(r);
        }
    }
    auto r = WP_29101;
    log(r);
    return move(r);
}

bool c::sig_reset(ostream& os) {
    bool ch = _workflows.sig_reset(os);
    ostringstream x;
    x <<"forgotten ";
    {
        lock_guard<mutex> lock(redirects_mx);
        if (!redirects.empty()) {
            redirects.clear();
            x << "redirects ";
            ch = true;
        }
    }
    if (ch) {
        os << x.str() << '\n';
    }
    return ch;
}

bool c::sig_hard_reset(ostream& os) {
    bool ch = _workflows.sig_hard_reset(os);
    return ch;
}

void c::sig_reload(ostream& os) {
}

bool c::on_signal(int sig, ostream& os) {
    b::on_signal(sig, os);
    log("on_signal", sig);
    switch (sig) {
    case 0: //RESET
        return sig_reset(os);
    case 1: //HARD RESET
        return sig_hard_reset(os);
    case 2: //RELOAD
        sig_reload(os);
        return false;
    }
    return false;
}

ko c::on_receive(peer_t& peer, item_t& itm, doc0_t* doc, ch_t& ch) {
    log("on_receive doc");
    itm.replace_doc(doc, ch);
    return ok;
}

ko c::workflow_item_requested(item_t& i, peer_t&, ch_t&) {
    log("workflow_item_requested", i.name);
    return ok;
}

ko c::trading_msg(peer_t& peer, svc_t svc, blob_t&& blob) {
    log("trading_msg", "svc", svc, "payload size", blob.size(), "bytes");
    assert(svc < svc_end);
    if (svc < svc_begin) {
        return b::trading_msg(peer, svc, move(blob));
    }
    assert (svc >= svc_begin);
    switch(svc) {
        case svc_redirects: {
            log("received redirects", blob.size(), "bytes");
            ch_t ch(_local_params, _local_params_mx);
            {
                unique_lock<mutex> lock(redirects_mx);
                auto r = redirects.read(blob);
                if (is_ko(r)) {
                    redirects.clear();
                }
                ch.shared_params_changed |= ch.local_params->shared.set("redirects", (redirects.empty() ? "N" : "Y"));
                log("redirects size-3", redirects.size());
                #ifdef DEBUG
                for (auto& i: redirects) {
                    log("redirects size-content", i.second.qr.protocol_selection.first);
                }
                #endif
            }
            if (ch.shared_params_changed) {
                tder->update_peer(peer, move(ch));
                tder->resume_chat(peer);
            }
            else {
                ch.close();
            }
            return ok;
        }
        case svc_workflow_item_request: {
            log("Received doc request.");
            string name;
            {
                blob_reader_t reader(blob);
                auto r = reader.read(name);
                if (is_ko(r)) {
                    log(r);
                    return ok;
                }
            }
            auto wf = _workflows.find(name);
            if (wf.second == nullptr) {
                log("Received doc request but no workflow is handling the doc type.");
                return ok;
            }
            {
                ch_t ch(_local_params, _local_params_mx);
                {
                    lock_guard<mutex> lock2(wf.first->mx);
                    auto r = workflow_item_requested(*wf.second, peer, ch);
                    if (is_ko(r)) {
                        return r;
                    }
                }
                tder->update_peer(peer, move(ch));
            }
            {
                lock_guard<mutex> lock2(wf.first->mx);
                auto r = wf.second->send_to(*tder, peer);
                if (is_ko(r)) {
                    log(r);
                    return ok;
                }
            }
            return ok;
        }
        case svc_workflow_item: {
            log("received svc_workflow_item");
            auto wf = _workflows.read_item(blob);
            if (get<1>(wf) == nullptr) {
                auto r = "KO 92201 Received doc but no workflow is handling this doc type.";
                log(r);
                return r;
            }
            log("doc from blob", get<2>(wf));
            ch_t ch(_local_params, _local_params_mx);
            {
                unique_lock<mutex> lock(get<0>(wf)->mx);
                auto r = on_receive(peer, *get<1>(wf), get<2>(wf), ch);
                if (is_ko(r)) {
                    return r;
                }
            }
            if (ch.shared_params_changed) {
                tder->update_peer(peer, move(ch));
                tder->resume_chat(peer);
            }
            else {
                ch.close();
            }
            return ok;
        }
    }
    return KO_29100;
}

blob_t c::push_payload(uint16_t pc) const {
    log("pushing to devices. code", pc);
    static_assert((uint16_t)push_begin >= (uint16_t)b::push_end);
    assert(pc < push_end);
    if (pc >= push_begin) {
        blob_t blob;
        switch (pc) {
            case push_redirects: {
                lock_guard<mutex> lock(redirects_mx);
                redirects.write(blob);
            }
            break;
        }
        return blob;
    }
    return b::push_payload(pc);
}

size_t c::blob_size() const {
    size_t sz = b::blob_size() + 
        blob_writer_t::blob_size(_workflows) +
        blob_writer_t::blob_size(redirects) +
        blob_writer_t::blob_size(_trade_state.first) +
        blob_writer_t::blob_size(_trade_state.second) +
        blob_writer_t::blob_size(_user_hint);
    log("blob_size", sz);
    return sz;
}

void c::to_blob(blob_writer_t& writer) const {
    b::to_blob(writer);
    log("to_blob", "cur", (uint64_t)(writer.cur - writer.blob.data()));
    writer.write(_workflows); 
    writer.write(redirects);
    writer.write(_trade_state.first);
    writer.write(_trade_state.second);
    writer.write(_user_hint);
}

ko c::from_blob(blob_reader_t& reader) {
    log("from_blob", "cur", (uint64_t)(reader.cur - reader.blob.data()));
    {
        auto r = b::from_blob(reader);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = reader.read(_workflows);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = reader.read(redirects);
        if (is_ko(r)) {
            return r;
        }
    }    
    {
        auto r = reader.read(_trade_state.first);
        if (is_ko(r)) {
            return r;
        }
    }    
    {
        auto r = reader.read(_trade_state.second);
        if (is_ko(r)) {
            return r;
        }
    }    
    {
        auto r = reader.read(_user_hint);
        if (is_ko(r)) {
            return r;
        }
    }    
    return ok;
}

