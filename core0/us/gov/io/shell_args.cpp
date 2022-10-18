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
#include "shell_args.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <string>

#include <us/gov/config.h>

#include "cfg0.h"

//#define loglevel "io"
//#define logclass "shell_args"
//#include "logs.inc"

// don't log here, it's used before initializing the logging subsystem

using namespace us::gov::io;
using namespace std;
using c = us::gov::io::shell_args;

c::shell_args(int argc, char** argv): argc(argc), argv(argv), del(false) {
}

c::shell_args(const shell_args& other): argc(other.argc), del(other.del), n(other.n) {
    if (del) {
        argv = new char*[other.argc];
        for (int i = 0; i < other.argc; ++i) {
            argv[i] = new char[strlen(other.argv[i]) + 1];
            strcpy(argv[i], other.argv[i]);
        }
    }
    else {
        argv = other.argv;
    }
}

shell_args& c::operator = (const shell_args& other) {
    if (del) {
        for (int i = 0; i < argc; ++i) {
            delete [] argv[i];
        }
        delete [] argv;
    }
    if (other.del) {
        argv = new char*[other.argc];
        for (int i = 0; i < other.argc; ++i) {
            argv[i] = new char[strlen(other.argv[i]) + 1];
            strcpy(argv[i], other.argv[i]);
        }
    }
    else {
        argv = other.argv;
    }
    argc = other.argc;
    n = other.n;
    del = other.del;
    return *this;
}

c::shell_args(const string& args): del(true) {
    vector<string> v;
    istringstream is(args);
    while (is.good()) {
        string w;
        is >> w;
        if (w.empty()) continue;
        v.push_back(w);
    }
    argc = v.size() + 1;
    argv = new char*[argc];
    int j = 0;
    argv[j] = new char[1];
    argv[j][0] = '\0';
    for (auto& i: v) {
        ++j;
        argv[j] = new char[i.size() + 1];
        strcpy(argv[j], i.c_str());
    }
}

c::~shell_args() {
    if (del) {
        for (int i = 0; i < argc; ++i) {
            delete [] argv[i];
        }
        delete [] argv;
    }
}

bool c::is_root_token(string i) {
    transform(i.begin(), i.end(), i.begin(), [](unsigned char ch){ return tolower(ch); });
    if (i == LGAS) return true; //water
    if (i == "w") return true;
    if (i == "h2o") return true;
    if (i == "agua") return true;
    if (i == "gas") return true;
    return false;
}

hash_t c::next_token(istream& is) {
    string s;
    is >> s;
    if (is_root_token(s)) return hash_t(0);
    return convert<hash_t>(s);
}

hash_t c::next_token() {
    if (n >= argc) {
        return hash_t(0);
    }
    string i = argv[n++];
    if (is_root_token(i)) return hash_t(0);
    return convert<hash_t>(i);
}

string c::next_line() {
    ostringstream os;
    while(n < argc) {
        os << argv[n++] << ' ';
    }
    string r = os.str();
    cfg0::trim(r);
    return r;
}

int c::args_left() const {
    return argc - n;
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "argc: " << argc << '\n';
    os << prefix << "argv: ";
    int m = 0;
    while(m < argc) {
        os << argv[m++] << ' ';
    }
    os << '\n';
    os << prefix << "n: " << n << '\n';
    os << prefix << "del: " << del << '\n';
}

void c::dump(ostream& os) const {
    dump("", os);
}

