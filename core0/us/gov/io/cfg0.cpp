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
#include "cfg0.h"
//#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cassert>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iostream>

#include <us/gov/stacktrace.h>
#include "types.h"


#define loglevel "gov/io"
#define logclass "cfg0"
#include "logs.inc"

using namespace us::gov::io;
using c = us::gov::io::cfg0;

const char* c::KO_60534 = "KO 60534 Cannot create home dir.";
const char* c::KO_84012 = "KO 84012 Likely the file doesn't exist in the local filesystem.";

c::cfg0(const string& home): home(home) {
}

c::cfg0(const cfg0& other): home(other.home) {
}

c::~cfg0() {
}

bool c::is_big_endian() {
    union {
        uint16_t i;
        char c[2];
    } bint = {0x0102};
    return bint.c[0] == 1;
}

void c::check_platform() {
    if (is_big_endian()) {
        auto r = "KO 07200 Only compatible with little-endian systems.";
        log(r, "Exiting");
        cerr << "Apologies, this program cannot run on big-endian systems." << endl;
        exit(1);
    }
}

pair<ko, pair<string, string>> c::split_fqn(string fqn) {
    trim(fqn);
    size_t pos = fqn.find(' ');
    if (pos != string::npos) {
        auto r = "KO 50182 input contains spaces.";
        log(r, fqn);
        return make_pair(r, make_pair("", ""));
    }
    if (fqn.empty()) {
        auto r = "KO 50183 Empty input.";
        log(r, fqn);
        return make_pair(r, make_pair("", ""));
    }
    if (*fqn.begin() != '/') {
        auto r ="KO 50182 Not an absolute path.";
        log(r, fqn);
        return make_pair(r, make_pair("", ""));
    }
    char cstr[fqn.size() + 1];
    char* p = fqn.data();
    char* tp = cstr;
    *tp = *p;
    ++tp;
    ++p;
    pos = 0;
    size_t j = 1;
    for (size_t i = 1; i < fqn.size(); ++i, ++p) {
        if (*(tp - 1) == '/' && *p == '/') continue;
        *tp = *p;
        if (*tp == '/') pos = j;
        ++tp;
        ++j;
    }
    if (*(tp - 1) == '/' && pos > 0) --tp; /// remove last slash
    *tp = '\0';
    fqn = cstr;
    string path = fqn.substr(0, pos);
    if (path.empty()) path = "/";
    ++pos;
    if (pos >= fqn.size()) {
        return make_pair(ok, make_pair(path, ""));
    }
    string name = fqn.substr(pos);
    return make_pair(ok, make_pair(path, name));
}

bool c::mkdir(const string& d) {
    int e = ::mkdir(d.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    //if (e == -1) cerr << errno << " " << strerror(errno) << endl;
    return e != -1;
}

bool c::file_exists(const string& f) {
    struct stat s;
    if (stat(f.c_str(), &s) == 0) return S_ISREG(s.st_mode);
    return false;
}

uint32_t c::file_size(const string& f) {
    struct stat stat_buf;
    int rc = stat(f.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

void c::mkdir_tree(string sub, string dir) {
    if (sub.length() == 0) return;
    int i = 0;
    for (; i < sub.length(); i++) {
        dir += sub[i];
        if (sub[i] == '/')
        break;
    }
    ::mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if ((i + 1) < sub.length()) mkdir_tree(sub.substr(i + 1), dir);
}

bool c::ensure_dir(const string& d) {
    if (!dir_exists(d)) {
        mkdir_tree(d, "");
        if (!dir_exists(d)) {
            cerr << "KO 60983 Creating directory " << d << endl;
            print_stacktrace(cout);
            assert(false);
            return false;
        }
    }
    return true;
}

bool c::dir_exists(const string& d) {
    struct stat s;
    if (stat(d.c_str(), &s) == 0) return S_ISDIR(s.st_mode);
    return false;
}

string c::abs_file(const string& home, const string& fn) {
    return home + "/" + fn;
}

string c::directory_of_file(const string& file) {
    auto n = file.rfind('/');
    if (n == string::npos) return "";
    return file.substr(0, n);
}

bool c::ensure_writable(const string& file) {
    string dir = directory_of_file(file);
    if (dir.empty()) return true;
    return ensure_dir(dir);
}

pair<us::ko, c> c::load(const string& home) {
    check_platform();
    if (!ensure_dir(home)) {
        log(KO_60534, home);
        return make_pair(KO_60534, c(""));
    }
    return make_pair(ok, c(home));
}

ko us::gov::io::write_file_(const vector<uint8_t>& v, const string& filename) {
    ofstream os(filename, fstream::out | fstream::binary);
    if (!os) {
        auto r = "KO 65994 Error writing file";
        log(r);
        return r;
    }
    os.write((const char*)v.data(), v.size());
    if (!os.good()) {
        auto r = "KO 65991 Error writing file";
        log(r);
        return r;
    }
    return ok;
}

ko us::gov::io::read_file_(const string& filename, vector<unsigned char>& buf) {
    ifstream is(filename, ios::binary | ios::ate);
    if (is.fail()) {
        log(cfg0::KO_84012, filename);
        return cfg0::KO_84012;
    }
    return read_file_(is, buf);
}

ko us::gov::io::read_file_(istream& is, vector<unsigned char>& buf) {
    ifstream::pos_type pos = is.tellg();
    try {
        log("size file", pos);
        buf.resize(pos);
    }
    catch (exception e) {
        auto r = "KO 84032";
        log(r);
        return r;
    }
    is.seekg(0, ios::beg);
    is.read((char*)buf.data(), pos);
    log("is.fail?", is.fail());
    if (is.fail()) {
        auto r = "KO 65998";
        log(r);
        return r;
    }
    return ok;
}

ko us::gov::io::read_text_file_(const string& filename, string& dest) {
    ifstream is(filename, ios::ate);
    if (is.fail()) {
        auto r = "KO 65998 File cannot be opened.";
        log(r);
        return r;
    }
    ifstream::pos_type pos = is.tellg();
    try {
        dest.resize(pos);
    }
    catch(exception e) {
        auto r = "KO 54098";
        log(r);
        return r;
    }
    is.seekg(0, ios::beg);
    is.read((char*)dest.data(), pos);
    if (is.fail()) {
        auto r = "KO 65997";
        log(r);
        return r;
    }
    return ok;
}

string c::rewrite_path(const string& d) {
    vector<char> v;
    v.reserve(d.size());
    auto i = d.begin();
    bool newp = false;
    while(i != d.end()) {
        if(*i == '/') {
            newp = true;
            ++i;
            continue;
        }
        bool ok = false;
        if (*i >= 'a' && *i <= 'z')
            ok = true;
        else if (*i >= 'A' && *i <= 'Z')
            ok = true;
        else if (*i >= '0' && *i <= '9')
            ok = true;
        if (!ok) {
            ++i; //ignored char
            continue;
        }
        if (newp) {
            newp = false;
            if (!v.empty()) {
                v.emplace_back('/');
            }
        }
        v.emplace_back(*i);
        ++i;
    }
    return string(v.data(), v.size());
}

void c::trim(string& s) {
    if (s.empty()) return;
    char* b = s.data();
    char* e = b + s.size();
    while(b < e) {
        if (*b > ' ') break;
        ++b;
    }
    --e;
    while(b <= e) {
        if (*e > ' ') break;
        --e;
    }
    ++e;
    s = string(b, e-b);
}

void c::to_identifier(string& s) {
    trim(s);
    char* b = s.data();
    char* e = b + s.size();
    while(b < e) {
        if (*b <= ' ') *b = '_';
        ++b;
    }
}

#include "command.h"

ko us::gov::io::system_command(const string& command, string& result) {
//cout << "system_command " << command << endl;
    raymii::CommandResult ans = raymii::Command::exec(command);
    if (ans.exitstatus != 0) {
//cout << "Exitstatus: " << ans.exitstatus << endl;
//cout << "Result: " << ans.output << endl;
        auto r = "KO 80795 Command finished with error.";
//cout << r << endl;
        log(r);
        return r;
    }
//cout << "Exitstatus: " << ans.exitstatus << endl;
//cout << "Result: " << ans.output << endl;
    result = ans.output;
    return ok;
}


