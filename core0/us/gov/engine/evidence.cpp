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
#include "evidence.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <us/gov/crypto/base58.h>
#include <us/gov/cash/app.h>
#include <us/gov/sys/app.h>
#include <us/gov/engine/auth/app.h>
#include <us/gov/sys/install_script.h>
#include <us/gov/traders/app.h>

#include "types.h"

#define loglevel "gov/engine"
#define logclass "evidence"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::evidence;
using namespace std::chrono;

c::evidence(appid_t app, eid_t eid0): ts(duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count()), app(app), eid(eid0) {
    log("created evidence", this);
}

c::evidence(const evidence& other): ts(other.ts), app(other.app), eid(other.eid) {
}

c::~evidence() {
    log("destroyed evidence", this);
}

void c::update_ts(const ts_t& nsecs_in_future) {
    ts = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count() + nsecs_in_future;
}

void c::hash_data_to_sign(crypto::ec::sigmsg_hasher_t& h) const {
    h.write(ts);
    h.write(app);
    h.write(eid);
}

void c::hash_data(hasher_t& h) const {
    h.write(ts);
    h.write(app);
    h.write(eid);
}

hash_t c::hash_id() const {
    hasher_t hasher;
    hash_data(hasher);
    hash_t hash;
    hasher.finalize(hash);
    return hash;
}

void c::write_sigmsg(sigmsg_hasher_t& h, bool include_ts) const {
    if (include_ts) h.write(ts);
    h.write(app);
    h.write(eid);
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(ts) +
        blob_writer_t::blob_size(app) +
        blob_writer_t::blob_size(eid);
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(ts);
    writer.write(app);
    writer.write(eid);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(ts);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(app);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(eid);
        if (is_ko(r)) return r;
    }
    return ok;
}

pair<ko, c*> c::create(appid_t appid, eid_t evid) {
    log("creating evidence from header info", +appid, +evid);
    if (appid == cash::app::id()) return cash::app::create_evidence(evid);
    if (appid == engine::auth::app::id()) return engine::auth::app::create_evidence(evid);
    if (appid == traders::app::id()) return traders::app::create_evidence(evid);
    if (appid == sys::app::id()) return sys::app::create_evidence(evid);
    auto r = "KO 14930 Invalid app.";
    log(r, +appid);
    return make_pair(r, nullptr);
}

pair<ko, pair<appid_t, eid_t>> c::extract_instance_idX(io::blob_reader_t& reader) {
    log("extract_instance_id");
    pair<ko, pair<appid_t, eid_t>> ret = make_pair(ok, make_pair(0, 0));
    auto r = reader.read_header();
    if (is_ko(r)) {
        ret.first = r;
        return move(ret);
    }
    if (unlikely(reader.header.serid != serid)) {
        auto r = "KO 30299 Invalid blob type. Expected evidence.";
        log(r);
        ret.first = r;
        return move(ret);
    }
    {
        ts_t ts;
        ret.first = reader.read(ts);
        if (is_ko(ret.first)) return move(ret);
        log("skipping field ts", ts);
    }
    {
        ret.first = reader.read(ret.second.first);
        if (is_ko(ret.first)) return move(ret);
        log("field appid", +ret.second.first);
    }
    {
        ret.first = reader.read(ret.second.second);
        if (is_ko(ret.first)) return move(ret);
        log("field eid", +ret.second.second);
    }
    return move(ret);
}

pair<ko, c*> c::from_blob(const blob_t& blob) {
    log("from blob", blob.size());
    c* ev;
    {
        pair<ko, pair<appid_t, eid_t>> x;
        {
            io::blob_reader_t reader(blob);
            x = extract_instance_idX(reader);
            if (is_ko(x.first)) {
                return make_pair(x.first, nullptr);
            }
        }
        auto r = create(x.second.first, x.second.second);
        if (unlikely(is_ko(r.first))) {
            return move(r);
        }
        ev = r.second;
    }
    {
        auto r = ev->read(blob);
        if (unlikely(is_ko(r))) {
            delete ev;
            return make_pair(r, nullptr);
        }
    }
    return make_pair(ok, ev);
}

pair<ko, c*> c::from_b58(const string& blob_b58) {
    blob_t blob = crypto::b58::decode(blob_b58);
    return from_blob(blob);
}

datagram* c::get_datagram(channel_t channel, seq_t seq) const {
    return s::get_datagram(channel, protocol::engine_ev, seq);
}

string c::formatts(ts_t ts) {
    time_t t = ts / 1e9;
    struct tm lt;
    (void) localtime_r(&t, &lt);
    char res[64];
    if (strftime(res, sizeof(res), "%a %b %d %Y %T", &lt) == 0) {
        return "?";
    }
    return res;
}

void c::write_pretty(const string& lang, ostream& os) const {
    if (lang == "es") {
        write_pretty_es(os);
    }
    else {
        write_pretty_en(os);
    }
}

void c::write_pretty_en(ostream& os) const {
    os << "exec time: " << ts << " ~" << formatts(ts) << '\n';
    os << "processor: " << +app << " " << +eid << '\n';
}

void c::write_pretty_es(ostream& os) const {
    os << "tiempo ejecucion: " << ts << " ~" << formatts(ts) << '\n';
    os << "procesador: " << +app << " " << +eid << '\n';
}

bool c::verify(ostream& os) const {
    return true;
}

