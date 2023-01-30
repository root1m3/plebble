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
#include "doc0_t.h"
#include <us/gov/crypto/base58.h>
#include <us/wallet/trader/params_t.h>
#include <chrono>

#define loglevel "wallet/trader/cert"
#define logclass "doc0"
#include <us/gov/logs.inc>

using namespace us::wallet::trader::cert;
using c = us::wallet::trader::cert::doc0_t;

c::doc0_t() {
}

c::~doc0_t() {
}

void c::set(const string& key_prefix, ch_t& ch) const {
    ostringstream os;
    os << key_prefix << "_ts";
    ch.shared_params_changed |= ch.local_params->shared.set(os.str(), get_ts());
}

void c::unset(const string& key_prefix, ch_t& ch) const {
    ostringstream os;
    os << key_prefix << "_ts";
    ch.shared_params_changed |= ch.local_params->shared.unset(os.str());
}

string c::lang() const {
    return params.get("lang", "en");
}

bool c::verify(ostream&) const {
    log("doc0_t verify ok");
    return true;
}

bool c::verify() const {
    ostream nullos(0);
    return verify(nullos);
}

bool c::tamper(const string& search, const string& replace) { /// replaces only the first occurrence. returns false if no matches found.
    string text = as_string();
    auto i = text.find(search);
    if (i == string::npos) {
        return false;
    }
    text.replace(i, search.size(), replace);
    buf = vector<uint8_t>(text.begin(), text.end());
    return true;
}

void c::write_pretty(ostream& os) const {
    string lang = params.get("lang", "en");
    if (lang == "es") {
        os << begin_of_document_es << '\n';
        os << title_es() << '\n'; //"catalogue.\n";
        os << "Fecha documento: ~" << us::gov::engine::evidence::formatts(ts) << " UTC\n";
        write_pretty_es(os);
        os << end_of_document_es << '\n';
    }
    else {
        os << begin_of_document << '\n';
        os << title() << '\n'; //"catalogue.\n";
        os << "Document date: ~" << us::gov::engine::evidence::formatts(ts) << " UTC\n";
        write_pretty_en(os);
        os << end_of_document << '\n';
    }
}

string c::as_string() const {
    return string(buf.begin(), buf.end());
}

namespace {
    void tabdump(const string& pfx, const string& txt, ostream& os) {
        string s = txt;
        char* b = const_cast<char*>(s.c_str());
        size_t sz = s.size();
        size_t p = 0;
        while (p < sz) {
            size_t cr = p;
            while(cr < sz && b[cr] != '\n') ++cr;
            b[cr] = '\0';
            os << pfx << &b[p] << '\n';
            p = cr + 1;
        }
    }
}

void c::write_pretty_en(ostream& os) const {
    if (!params.empty()) {
        os << '\n';
        os << "Parameters:\n";
        params.write_pretty("  ", os);
    }
    if (buf.empty()) return;
    if (type == type_string) {
        string text = as_string();
        os << '\n';
        os << "Content:\n";
        tabdump("  ", text, os);
    }
    else {
        os << "Content: " << typestr[type] << " " << buf.size() << " bytes.";
    }
    os << '\n';
}

void c::write_pretty_es(ostream& os) const {
    if (!params.empty()) {
        os << '\n';
        os << "Parametros:\n";
        params.write_pretty("  ", os);
    }
    if (buf.empty()) return;
    if (type == type_string) {
        string text = as_string();
        os << '\n';
        os << "Contenido:\n";
        tabdump("  ", text, os);
    }
    else {
        os << "Contenido: " << typestr[type] << " " << buf.size() << " bytes.";
    }
    os << '\n';
}

void c::hash_data_to_sign(sigmsg_hasher_t& h) const {
    hashmsg(h);
}

void c::hash_data(hasher_t& h) const {
    hashmsg(h);
}

void c::options::exec_help(const string& prefix, ostream& os) {
    os << prefix << "-itxt  file for text.\n";
    os << prefix << "-ikv  key-value file.\n";
    os << prefix << "-bkv  key-value blob file.\n";
    os << prefix << "-kv key value.\n";
    os << prefix << "-p  doc refers to personality.\n";
    os << prefix << "-lang  en/es.\n";
}

void c::options::dump(const string& pfx, ostream& os) const {
    os << pfx << "txt " << text.size() << " bytes.\n";
    os << pfx << "kv " << kv.size() << " entries.\n";
}

ko c::options::parse(const string& opt, istream& is) {
    log("options::parse", opt);
    if (opt == "-itxt") {
        string file;
        is >> file;
        log("-itxt option.", file);
        if (file.empty()) {
            auto r = "KO 30122 txt input filename";
            log(r);
            return r;
        }
        if (!us::gov::io::cfg0::file_exists(file)) {
            auto r = "KO 50113 txt input file does not exist.";
            log(r);
            return r;
        }
        return us::gov::io::read_text_file_(file, text);
    }
    if (opt == "-ikv" ) {
        string file;
        is >> file;
        log("-ikv option.", file);
        if (file.empty()) {
            auto r = "KO 30123 kv input filename";
            log(r);
            return r;
        }
        if (!us::gov::io::cfg0::file_exists(file)) {
            auto r = "KO 50112 kv input file does not exist.";
            log(r);
            return r;
        }
        if (!kv.loadX(file)) {
            auto r = "KO 65992 error loading input file.";
            log(r);
            return r;
        }
        return ok;
    }
    if (opt == "-bkv" ) {
        string file;
        is >> file;
        log("-bkv option.", file);
        if (file.empty()) {
            auto r = "KO 30123 kv input filename";
            log(r);
            return r;
        }
        if (!us::gov::io::cfg0::file_exists(file)) {
            auto r = "KO 50112 kv input file does not exist.";
            log(r);
            return r;
        }
        return kv.load(file);
    }
    if (opt == "-kv" ) {
        string key;
        is >> key;
        if (key.empty()) {
            auto r = "KO 30133 key.";
            log(r);
            return r;
        }
        string value;
        is >> value;
        if (value.empty()) {
            auto r = "KO 30143 Value.";;
            log(r);
            return r;
        }
        kv.set(key, value);
        return ok;
    }
    if (opt == "-p") {
        hash_t refer_personality{0}; //anonymous
        is >> refer_personality;
        if (is.fail()) {
            auto r = "KO 30125 personality";
            log(r);
            return r;
        }
        if (!refer_personality.is_zero() ) {
            kv.set("refer_personality", refer_personality);
        }
        return ok;
    }
    if (opt == "-lang") {
        string lang;
        is >> lang;
        if (is.fail()) {
            auto r = "KO 81229 lang.";
            log(r);
            return r;
        }
        if (lang != "en") {
            if (lang != "es") {
                auto r = "KO 81289 only en/es lang supported.";
                log(r);
                return r;
            }
            kv.set("lang", lang);
        }
        return ok;
    }
    return KO_31998;
}

ko c::options::parse_cmdline(istream& is) {
    log("parse_cmdline is");
    string opt;
    is >> opt;
    while(!opt.empty() && opt[0] == '-') {
        auto r = parse(opt, is);
        if (is_ko(r)) {
            return r;
        }
        opt.clear();
        is >> opt;
    }
    return ok;
}

ko c::init(const options& o) {
    params = o.kv;
    buf = vector<uint8_t>(o.text.begin(), o.text.end());
    ts = chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now().time_since_epoch()).count();
    return ok;
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(ts) + blob_writer_t::blob_size(params) + blob_writer_t::blob_size(buf) + sizeof(uint8_t);
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(ts);
    writer.write(params);
    writer.write(buf);
    writer.write((uint8_t) type);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(ts);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(params);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(buf);
        if (is_ko(r)) return r;
    }
    {
        uint8_t v;
        auto r = reader.read(v);
        if (is_ko(r)) return r;
        type = (type_t)v;
    }
    return ok;
}

