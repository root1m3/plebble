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
#include "expiry_doc_t.h"
#include <us/gov/crypto/base58.h>
#include <us/wallet/trader/params_t.h>
#include <chrono>

#define loglevel "wallet/trader/workflow"
#define logclass "expiry_doc"
#include <us/gov/logs.inc>

using c = us::wallet::trader::workflow::expiry_doc_t;
using namespace us::wallet::trader::workflow;
using us::ko;

c::expiry_doc_t() {
}

c::~expiry_doc_t() {
}

ko c::init(const options& o) {
    log("init expiry doc", o.xhours);
    if (o.xhours == 0) {
        return "KO 50069 Document has expired on creation.";
    }
    auto r = b::init(o);
    if (is_ko(r)) return r;
    expiry = chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now().time_since_epoch()).count() + o.xhours * 3600e9;
    log("init expiry", expiry);
    return ok;
}

ko c::options::parse(const string& option, istream& is) {
    if (option == "-x") {
        is >> xhours;
        if (is.fail()) {
            auto r = "KO 81224 expires.";
            log(r);
            return r;
        }
        if (xhours < 0) {
            auto r = "KO 81289 invalid expiry hour.";
            log(r);
            return r;
        }
        return ok;
    }
    return b::options::parse(option, is);
}

void c::options::exec_help(const string& prefix, ostream& os) {
    os << prefix << "-x  Expiry hours.\n";
    b::options::exec_help(prefix, os);
}

void c::options::dump(const string& pfx, ostream& os) const {
    os << pfx << "xhours " << xhours << '\n';
    b::options::dump(pfx, os);
}

void c::write_pretty_en(ostream& os) const {
    os << "Expires: ~" << us::gov::engine::evidence::formatts(expiry) << " UTC\n";
    b::write_pretty_en(os);
}

void c::write_pretty_es(ostream& os) const {
    os << "Caduca: ~" << us::gov::engine::evidence::formatts(expiry) << " UTC\n";
    b::write_pretty_es(os);
}

bool c::verify(ostream& os) const {
    log("workflow_expiry_doc_t verify");
    auto now = chrono::duration_cast<chrono::nanoseconds> (chrono::system_clock::now().time_since_epoch()).count();
    if (now > expiry) {
        os << "expiry time.\n";
        log("failed verification.", "expiry time", expiry, "now", now);
        return false;
    }
    log("workflow_expiry_doc_t verify ok");
    return true;
}

void c::hash_data_to_sign(sigmsg_hasher_t& h) const {
    b::hash_data_to_sign(h);
    h.write(expiry);
}

void c::hash_data(hasher_t& h) const {
    b::hash_data(h);
    h.write(expiry);
}

void c::set(const string& key_prefix, ch_t& ch) const {
    b::set(key_prefix, ch);
    ostringstream os;
    os << key_prefix << "_expiry";
    ch.shared_params_changed |= ch.local_params->shared.set(os.str(), expiry);
}

void c::unset(const string& key_prefix, ch_t& ch) const {
    b::unset(key_prefix, ch);
    ostringstream os;
    os << key_prefix << "_expiry";
    ch.shared_params_changed |= ch.local_params->shared.unset(os.str());
}

size_t c::blob_size() const {
    return b::blob_size() + blob_writer_t::blob_size(expiry);
}

void c::to_blob(blob_writer_t& writer) const {
    b::to_blob(writer);
    writer.write(expiry);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = b::from_blob(reader);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(expiry);
        if (is_ko(r)) return r;
    }
    return ok;
}


