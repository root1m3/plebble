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
#include "protocol_selection_t.h"
#include <cstring>

#define loglevel "wallet/trader"
#define logclass "protocol_selection_t"
#include <us/gov/logs.inc>

using c = us::wallet::trader::protocol_selection_t;
using namespace us::wallet;
using namespace us::wallet::trader;
using namespace std;
using us::ko;

c::protocol_selection_t() {
}

c::protocol_selection_t(string&& p, string&& r): b(move(p), move(r)) {
}

c::protocol_selection_t(const string& p, const string& r): b(p, r) {
}

c::protocol_selection_t(protocol_selection_t&& other): b(move(other)) {
}

c::protocol_selection_t(const protocol_selection_t& other): b(other) {
}

c& c::operator = (const c& other) {
    first = other.first;
    second = other.second;
    return *this;
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(first) + blob_writer_t::blob_size(second);
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(first);
    writer.write(second);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(first);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = reader.read(second);
        if (is_ko(r)) {
            return r;
        }
    }
    return ok;
}

void c::dump(const string& prefix, ostream&) const {
}

void c::to_streamX(const string& o, ostream& os) {
    if (o.empty()) {
        os << "- ";
    }
    else {
        os << o << ' ';
    }
}

string c::to_string() const {
    ostringstream os;
    to_streamX(os);
    return os.str();
}

void c::to_streamX(ostream& os) const {
    to_streamX(first, os);
    to_streamX(second, os);
}

ko c::from_streamX(istream& is, string& o) {
    is >> o;
    if (is.fail()) {
        auto r = "KO 70144";
        log(r);
        return r;
    }
    if (o == "-") o.clear();
    return ok;
}

ko c::from_streamX(istream& is) {
    if (is.eof()) {
        first = second = "";
        return ok;
    }
    {
        auto r = from_streamX(is, first);
        if (is_ko(r)) {
            auto r = "KO 56784";
            log(r);
            return r;
        }
    }
    {
        auto r = from_streamX(is, second);
        if (is_ko(r)) {
            auto r = "KO 56785";
            log(r);
            return r;
        }
    }
    return ok;
}

