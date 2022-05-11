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
#include "trader_protocol.h"
#include <sstream>

#include <us/gov/io/cfg0.h>
#include <us/gov/io/cfg.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/socket/client.h>
#include <us/gov/cash/tx_t.h>
#include <us/gov/cash/ttx.h>
#include <us/gov/cash/map_tx.h>
#include <us/gov/cash/file_tx.h>
#include <us/gov/crypto/base58.h>
#include <us/gov/crypto/ripemd160.h>

#include <us/wallet/protocol.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/traders_t.h>
#include <us/wallet/engine/daemon_t.h>
#include <us/wallet/engine/peer_t.h>

#include "trader_t.h"

#define loglevel "wallet/trader"
#define logclass "trader_protocol"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::trader_protocol;

const char* c::WP_29101 = "WP 29101 Nothing done here."; //WayPoint
const char* c::KO_29100 = "KO 29100 Invalid command."; //Error

remote_params_t* c::remote_params_on_hold{nullptr};

c::trader_protocol(business_t& bz): business(bz), phome(bz.home) {
    log("constructor", phome);
    assert(!phome.empty());
    us::gov::io::cfg0::ensure_dir(phome);
    pphome = phome;
    remote_params = new remote_params_t();
    log("save params templates");
    save_params_template();
}

c::~trader_protocol() {
    delete remote_params;
}

#include "olog_t.inc"

template<typename... Args>
void c::olog(const Args&... args) const {
    ::ologx(*tder, args...);
}

ko c::attach(trader_t& trader, ch_t& ch) {
    assert(ch.closed());
    ch.open(_local_params, _local_params_mx);
    auto r = on_attach(trader, ch);
    ch.close();
    return r;
}

ko c::hold_remote_params(blob_t&& blob) {
    if (remote_params_on_hold != nullptr) {
        auto r = "KO 76095 Remote params on hold is already busy.";
        log(r);
        return r;
    }
    blob_reader_t reader(blob);
    remote_params_on_hold = new remote_params_t();
    auto r = reader.read(*remote_params_on_hold);
    if (is_ko(r)) {
        delete remote_params_on_hold;
        remote_params_on_hold = nullptr;
        return r;
    }
    return ok;
}

ko c::on_attach(trader_t& tder_, ch_t& ch) {
    log("on_attach", "trace-w8i");
    assert(!ch.closed());
    tder = &tder_;
    auto rp = remote_params; /// My preloaded remote_params (came from handshake), do as if they arrived with svc_params
    remote_params = nullptr;
    assert(pphome == phome);
    rehome_dir(phome, ch);
    assert(rp != nullptr);
    {
        auto r = on_remote_(rp, ch);
        if (is_ko(r)) {
            delete remote_params_on_hold;
            remote_params_on_hold = nullptr;
            return r;
        }
    }
    if (remote_params_on_hold != nullptr) {
        log("there are remote params on hold. (received before the protocol was created (slow computers))");
        rp = remote_params_on_hold;
        remote_params_on_hold = nullptr;
        {
            auto r = on_remote_(rp, ch);
            if (is_ko(r)) {
                return r;
            }
        }
    }
    return ok;
}

void c::schedule_push(uint16_t code) const {
    assert(tder->w != nullptr);
    tder->parent.schedule_push(get_push_datagram(code), *tder->w);
}

void c::data(const string& lang, ostream& os) const {
    log("generating push_data info");
    os << "pphome " << pphome << '\n';
    os << "ico " << (ico.empty() ? 'N' : 'Y') << '\n';
    os << "logo " << (image.empty() ? 'N' : 'Y') << '\n';
    {
        lock_guard<mutex> lock(_local_params_mx);
        os << "datadir " << datadir(_local_params) << '\n';
        _local_params.shared.dump("local__", os);
        _local_params.priv.dump("", os);
    }
    {
        lock_guard<mutex> lock(remote_params_mx);
        remote_params->dump("remote__", os);
    }
}

bool c::has_remote_params() const {
    lock_guard<mutex> lock(remote_params_mx);
    assert(remote_params != nullptr);
    return !remote_params->empty();
}

bool c::requires_online(const string& cmd) const {
    if (cmd == "request" ||
        cmd == "send" ||
        cmd == "say_hello" ) return true;
    return false;
}

void c::help_online(const string& indent, ostream& os) const {
    twocol(indent, "end", "Finishes current protocol", os);
    twocol(indent, "request logo", "Request peer's logo", os);
    twocol(indent, "request ico", "Request peer's icon", os);
    twocol(indent, "send <file>", "Send over a file", os);
    twocol(indent, "send shared_params [mute]", "Send shared parameters. [without feedback]", os);
}

void c::help_onoffline(const string& indent, ostream& os) const {
}

void c::help_show(const string& indent, ostream& os) const {
    twocol(indent, "params", "Show params.", os);
    twocol(indent, "ico", "peer's icon", os);
    twocol(indent, "logo", "peer's logo", os);
}

void c::twocol(const string& prefix, const string_view& l, const string_view& r, ostream& os) {
    static constexpr string_view e;
    twocol(prefix, l, r, e, os);
}

namespace {
    vector<string> fit(const string_view& s, int w, bool left) {
        vector<string> lines;
        auto x = s;
        while(true) {
            if (x.empty()) break;
            auto s1 = x.substr(0, w);
            auto s2 = x.size() > w ? x.substr(w) : "";
            lines.push_back(string(s1));
            x = s2;
        }
        if (left) {
            for (auto& i: lines) {
                i = i + string(w - i.size(), ' ');
            }
        }
        else {
            for (auto& i: lines) {
                i = string(w - i.size(), ' ') + i;
            }
        }
        return move(lines);
    }
}

void c::twocol(const string& prefix, const string_view& l, const string_view& r, const string_view& r2, ostream& os) {
    static const int w{80};
    vector<string> l1 = fit(l, w, true);
    vector<string> l2;
    {
        ostringstream os2;
        os2 << r << '.' << r2;
        l2 = fit(os2.str(), w, true);
    }
    auto l1i = l1.begin();
    auto l2i = l2.begin();
    while(true) {
        if (l2i == l2.end() && l1i == l1.end()) break;
        if (l1i == l1.begin()) {
            os << prefix;
        }
        else {
            os << string(prefix.size(), ' ');
        }
        if (l1i != l1.end()) {
             if (l1i != l1.begin()) os << "    ";
             os << *l1i;
             ++l1i;
        }
        else {
            os << string(w, ' ');
        }
        os << "  ";
        if (l2i != l2.end()) {
             if (l2i != l2.begin()) os << "    ";
             os << *l2i;
             ++l2i;
        }
        os << '\n';
    }
}

void c::on_finish() {
}

void c::on_file_updated(const string& path, const string& name) {
    log("on_file_updated", path, name);
    ch_t ch(_local_params, _local_params_mx);
    on_file_updated(path, name, ch);
    ch.close();
    log("on_file_updated", path, name, "ch:", ch.to_string());
    tder->update_peer(move(ch));
}

void c::on_file_updated(const string& path, const string& name, ch_t& ch) {
}

const hash_t& c::tid() const {
    return tder->id;
}

ko c::rehome_apply(ch_t& ch) {
    log("rehome_apply.", pphome);
    auto r = rehome(ch);
    if (is_ko(r)) {
        return move(r);
    }
    log("changed?", ch.to_string(), "pphome", pphome);
    return ok;
}

protocol_selection_t c::opposite_protocol_selection() const {
    return protocol_selection_t(get_name(), peer_rolestr());
}

protocol_selection_t c::protocol_selection() const {
    return protocol_selection_t(get_name(), rolestr());
}

string c::datadir(const local_params_t&) const {
    assert(!pphome.empty());
    ostringstream os;

    os << static_cast<business_t&>(business).r2rhome << "/trade_data/" << tder->datasubdir;
    string dir = os.str();
    us::gov::io::cfg0::ensure_dir(dir);
    log("datadir", dir);
    return dir;
}

void c::save_params_template() {
    log("save_params_template", phome);
    local_params_t lp;
    underride_default_params(lp);
    us::gov::io::cfg0::ensure_dir(phome);
    lp.priv.saveX(phome + "/params_private.template");
    lp.shared.saveX(phome + "/params_shared.template");
}

bool c::set_personality_from_params(ch_t& ch) {
    string per = ch.local_params->priv.get("personality_sk", "");
    string per_moniker = ch.local_params->priv.get("personality_moniker", "");
    int r = 0;
    log("set_personality", per, per_moniker);
    if (!per.empty()) {
        r = tder->set_personality(per, per_moniker);
        if (r != 0) {
            log("set_personality.", tder->my_personality.id, tder->my_personality.moniker, r);
            ch.personality_changed = true;
        }
        else {
            log("personality didnt change");
        }
        return r != 0;
    }
    if (!per_moniker.empty()) {
        log("set moniker.", tder->my_personality.moniker);
        bool mch = tder->my_personality.moniker != per_moniker;
        if (mch) {
            ch.personality_changed = true;
            tder->my_personality.moniker = per_moniker;
        }
    }
    return r != 0;
}

bool c::rehome_dir(const string& dir, ch_t& ch) { //changed personality
    log("rehome_dir", dir);
    us::gov::io::cfg0::ensure_dir(dir);
    ch.priv_params_changed |= underride_default_params(*ch.local_params);
    ch.local_params->load(dir, ch);
    return set_personality_from_params(ch);
}

ko c::rehome(ch_t& ch) { //pdir -> pphome. Produces ppdir from pdir
    assert(!pphome.empty());
    log("rehome. Starting ch", ch.to_string(), "pphome", pphome);
    bool modeis_mepeer = ch.local_params->priv.get("homemode", "") == "me/peer";
    log("modeis_mepeer", modeis_mepeer);
    static constexpr int max_jumps = 100;
    int jumps = 0;
    while(true) {
        hash_t first_pers;
        hash_t second_pers;
        if (modeis_mepeer) {
            first_pers = tder->my_personality.id;
            second_pers = tder->peer_personality.id;
            log("personalities: me ", first_pers, "/ peer", second_pers);
        }
        else {
            first_pers = tder->peer_personality.id;
            second_pers = tder->my_personality.id;
            log("personalities: peer ", first_pers, "/ me", second_pers);
        }
        ostringstream dir;
        dir << phome << '/' << first_pers;
        bool jump = false;
        for (int i = 0; i < 2; ++i) {
            if (i == 1) {
                dir << '/' << second_pers;
            }
            if (rehome_dir(dir.str(), ch)) {
                jump = true;
                break;
            }
        }
        if (jump) {
            if (++jumps < max_jumps) {
                continue;
            }
            auto r = "KO 30293 Cycle has been stopped.";
            log(r);
        }
        pphome = dir.str();
        break;
    }
    log("rehome. Leaving ch", ch.to_string(), "pphome", pphome, "jumps", jumps);
    return ok;
}

void c::dump(ostream& os) const {
}

void c::to_stream(ostream& os) const {
}

c* c::from_stream(istream& is) {
    return nullptr;
}

ko c::exec_offline(const string& cmd0, ch_t& ch) {
    istringstream is(cmd0);
    string cmd;
    is >> cmd;
    if (cmd == "show") {
        string cmd;
        is >> cmd;
        log("show", cmd);
        if (cmd == "logo") {
            log("OK the logo will be pushed");
            //ch.no_need_to_update_devices();
            schedule_push(push_logo);
            return ok;
        }
        if (cmd == "ico") {
            log("OK the ico will be pushed");
            //ch.no_need_to_update_devices();
            schedule_push(push_ico);
            return ok;
        }
        if (cmd == "params") {
            log("OK the logo will be pushed");
            //ch.no_need_to_update_devices();
            schedule_push(push_params);
            return ok;
        }
    }
    auto r = WP_29101;
    return move(r);
}

us::wallet::wallet::local_api& c::w() {
    assert(tder->w != nullptr);
    return *tder->w;
}

const us::wallet::wallet::local_api& c::w() const {
    assert(tder->w != nullptr);
    return *tder->w;
}

string c::get_lang() const {
    return "en";
}

ko c::exec_online(peer_t& peer, const string& cmd0, ch_t& ch) {
    istringstream is(cmd0);
    assert(tder->w != nullptr);
    string cmd;
    is >> cmd;
    if (cmd == "request") {
        string cmd;
        is >> cmd;
        if (cmd == "logo") {
            olog("exec", cmd0, cmd);
            auto r = peer.call_trading_msg(peer_t::trading_msg_in_t(tid(), svc_logo_request, blob_t()));
            if (is_ko(r)) {
                return move(r);
            }
            //ch.no_need_to_update_devices();
            return tder->parent.push_OK(tid(), "Logo requested...", *tder->w);
        }
        if (cmd == "ico") {
            olog("exec", cmd0, cmd);
            auto r = peer.call_trading_msg(peer_t::trading_msg_in_t(tid(), svc_ico_request, blob_t()));
            if (is_ko(r)) {
                return move(r);
            }
            //ch.no_need_to_update_devices();
            return tder->parent.push_OK(tid(), "Icon requested...", *tder->w);
        }
    }
    if (cmd == "send") {
        string cmd;
        is >> cmd;
        if (cmd == "shared_params") {
            ch.shared_params_changed = true;
            ch.no_need_to_update_devices_();
            string mute;
            is >> mute;
            if (mute == "mute") {
                return ok;
            }
            return tder->parent.push_OK(tid(), "Sent shared_params.", *tder->w);
        }
    }
    auto r = WP_29101;
    return move(r);
}

bool c::sig_reset(ostream& os) {
    bool ch = false;
    ostringstream x;
    x <<"forgotten ";
    {
        lock_guard<mutex> lock(assets_mx);
        if (!image.empty()) {
            image.clear();
            x << "logo ";
            ch = true;
        }
        if (!ico.empty()) {
            ico.clear();
            x << "ico ";
            ch = true;
        }
    }
    if (ch) {
        os << x.str() << '\n';
    }
    return ch;
}

bool c::sig_hard_reset(ostream& os) {
    return false;
}

void c::sig_reload(ostream& os) {
}

bool c::on_signal(int sig, ostream& os) {
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

ko c::trading_msg(peer_t& peer, svc_t svc, blob_t&& blob) {
    log("trading_msg", "svc", svc, "payload size", blob.size(), "bytes");
    assert (svc >= svc_begin);
    assert (svc < svc_end);
    switch (svc) {
        case svc_logo: {
            log("received logo image", blob.size(), "bytes");
            {
                lock_guard<mutex> lock(assets_mx);
                image = move(blob);
            }
            schedule_push(trader_t::push_data);
            return ok;
        }
        case svc_ico: {
            log("received ico image", blob.size(), "bytes");
            {
                lock_guard<mutex> lock(assets_mx);
                ico = move(blob);
            }
            schedule_push(trader_t::push_data);
            return ok;
        }
        case svc_ico_request: {
            log("svc_ico_request. Sending ico.");
            return peer.call_trading_msg(peer_t::trading_msg_in_t(tid(), svc_ico, business.ico));
        }
        case svc_logo_request: {
            string fn = phome + "/logo.png";
            blob_t v;
            log("svc_logo_request. Sending file", fn);
            auto r = us::gov::io::read_file_(fn, v);
            if (is_ko(r)) {
                return r;
            }
            return peer.call_trading_msg(peer_t::trading_msg_in_t(tid(), svc_logo, v));
        }
        case svc_params: {
            log("Received svc_params.");
            blob_reader_t reader(blob);
            ch_t ch(_local_params, _local_params_mx);
            {
                auto r = on_svc_params_(reader, ch);
                if (is_ko(r)) {
                    return move(r);
                }
            }
            return tder->update_peer(peer, move(ch));
        }
    }
    return KO_29100;
}

c::params_t c::shared_params() const {
    log("copying shared_params");
    lock_guard<mutex> lock(_local_params_mx);
    return _local_params.shared;
}

ko c::update_peer_(peer_t& peer, ch_t&& ch) const {
    assert(ch.closed());
    if (!ch.shared_params_changed) return ok;
    blob_t blob;
    {
        lock_guard<mutex> lock(_local_params_mx);
        blob_writer_t w(blob, blob_writer_t::blob_size(_local_params.shared));
        w.write(_local_params.shared);
    }
    log("send_shared_params");
    return peer.call_trading_msg(peer_t::trading_msg_in_t(tid(), svc_params, blob));
}

ko c::on_remote(const personality::proof_t::raw_t& x, ch_t& ch) {
    assert(ch.closed());
    ch.open(_local_params, _local_params_mx);
    auto r = on_remote_(x, ch);
    ch.close();
    return r;
}

ko c::on_remote(params_t* x, ch_t& ch) {
    log("on_remote");
    assert(ch.closed());
    ch.open(_local_params, _local_params_mx);
    auto r = on_remote_(x, ch);
    ch.close();
    return r;
}

ko c::on_remote_(const personality::proof_t::raw_t&, ch_t& ch) {
    assert(!ch.closed());
    log("on_remote_", "Display devices will be refreshed.");
    auto r = rehome_apply(ch);
    if (is_ko(r)) {
        return move(r);
    }
    return move(r);
}

ko c::on_remote_(remote_params_t *rp, ch_t& ch) {
    assert(!ch.closed());
    assert(rp != nullptr);
    log("on_remote_params");
    {
        lock_guard<mutex> lock(remote_params_mx);
        if (remote_params != nullptr) {
            if (*rp == *remote_params) {
                log("peer sent the same param set");
                delete rp;
                return ok;
            }
        }
        delete remote_params;
        remote_params = rp;
        logdump("RP> ", *remote_params);
    }
    ch.always_update_devices_();
    log("setting ch always_update_devices", ch.to_string());
    return rehome_apply(ch);
}

ko c::on_svc_params(blob_reader_t& reader, ch_t& ch) {
    assert(ch.closed());
    ch.open(_local_params, _local_params_mx);
    auto r = on_svc_params_(reader, ch);
    ch.close();
    return r;
}

ko c::on_svc_params_(blob_reader_t& reader, ch_t& ch) {
    assert(!ch.closed());
    remote_params_t* rp = new remote_params_t();
    auto r = reader.read(*rp);
    if (is_ko(r)) {
        delete rp;
        return r;
    }
    return on_remote_(rp, ch);
}

datagram* c::get_push_datagram(uint16_t pc) const {
    return peer_t::push_in_t(tder->id, pc, push_payload(pc)).get_datagram(tder->parent.daemon.channel, 0);
}

blob_t c::push_payload(uint16_t pc) const {
    log("push_payload", pc);
    static_assert((uint16_t)push_begin >= (uint16_t)trader_t::push_end);
    assert(pc < push_end);
    if (pc >= push_begin) {
        switch(pc) {
            case push_logo: {
                log("push remote logo");
                lock_guard<mutex> lock(assets_mx);
                return image;
            }
            case push_ico: {
                log("push remote ico");
                lock_guard<mutex> lock(assets_mx);
                return ico;
            }
            case push_params: {
                log("push params");
                blob_t blob;
                {
                    lock_guard<mutex> lock(_local_params_mx);
                    blob_writer_t w(blob, blob_writer_t::blob_size(_local_params));
                    w.write(_local_params);
                }
                return move(blob);
            }
            break;
            default:
                return blob_t();
        }
    }
    assert(pc < push_begin);
    return tder->push_payload(pc, get_lang());
}

void c::exec_help(const string& prefix , ostream& os) {
    os << prefix << "info\n";
}

ko c::exec(istream& is, traders_t& traders, wallet::local_api& w) {
    string cmd;
    is >> cmd;
    if (cmd == "info") {
        return traders.push_OK("I can't figure this out.", w);
    }
    auto r = "KO 10918 Invalid command";
    log(r);
    return r;
}

bool c::underride_default_params(local_params_t& lp) const { //returns if priv_params changed
    log("underride_default_params");
    bool r{false};
    r |= lp.priv.underride("homemode", "me/peer");
    r |= lp.priv.underride("personality_sk", "");
    r |= lp.priv.underride("personality_moniker", "anonymous");
    r |= lp.priv.underride("chat_script", "0");
    return r;
}

chat_t::entry c::AI_chat(const chat_t&, peer_t&) {
    chat_t::entry ans;
    return ans;
}

