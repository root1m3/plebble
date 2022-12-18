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
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <set>
#include <map>
#include <cstring>
#include <cassert>
#include <filesystem>
#include <functional>

using namespace std;

void help(ostream& os) {
    os << "srctool [options] -a <header_file> <file>        Adds header\n";
    os << "srctool [options] -i <file>                      Removes header\n";
    os << "srctool [options] --patch <pfx> <envfile> <file> Replaces in file all tokens defined in env. tokens are in the form ##<pfx>token##, env in the form token=\"replacement\" \n";
    os << "Options:\n";
    os << "  --ext <extension>                              Treat file as if it had the given extension.\n";
    os << "  --dir                                          <file> is a directory, apply recursively.\n";
    os << "  --patch                                        Tokens mode. all other options ignored.\n";
    os << "  --dryrun                                       Write to srdout instead of rewriting files.\n";
    os << "    extension: sh                                File shall have bangshe on 1st line.\n";
}

string _ext;
bool _recursive{false};
bool _dryrun{false};

#define MAX_HDR_LINES 50

set<string> excludeset {
    "./gov/io/command.h",
    "./gov/crypto/base58.h",
    "./gov/crypto/base58.cpp",
    "./gov/crypto/endian_rw.h",
    "./android/app/src/main/java/us/cash/IntentResult.java",
    "./android/app/src/main/java/us/cash/IntentIntegrator.java",
    "./sdk/java/us/gov/crypto/base58.java",
    "./sdk/java/us/gov/crypto/linux_secure_random.java"
};


bool exclude(const string& f) {
//cout << "exclude? " << f << endl;
    return excludeset.find(f) != excludeset.end();
}

set<string> bashexts {
    "",
    "sh",
    "in"
};

set<string> ignoreexts {
    "xml",
};


string ext(const string& file) {
//cout << "file: " << file << endl;
    if (file == "makefile_distr") {
        return "makefile";
    }
    if (!_ext.empty()) {
        return _ext;
    }
    auto d = file.find_last_of('/');
    string filename;
    string ext;
    if (d == string::npos) {
        d = 0;
        filename = file;
    }
    else {
        filename = file.substr(d + 1);
    }
//cout << "  XXX0 " << filename << endl;
    auto i = filename.find_last_of('.');
    if (i == string::npos) {
        filename = filename.substr(0, i);
        ext = "script";
    }
    else {
        string fn = filename.substr(0, i);
        ext = filename.substr(i + 1);
        filename = fn;
//cout << "  XXX0.1 " << filename << " -- " << ext << endl;
        if (ext == "in") {
            auto i = filename.find_last_of('.');
            if (i == string::npos) {
                filename = filename.substr(0, i);
                ext = "script";
            }
            else {
                string fn = filename.substr(0, i);
                ext = filename.substr(i + 1);
                filename = fn;
//cout << "  XXX0.2 " << filename << " -- " << ext << endl;
            }
        }
    }
//cout << "XXX1 " << filename << " -- " << ext << endl;
    if (filename == "makefile") {
//cout << "  3-Return makefile\n";
        ext = "makefile";
    }
    else {
        if (bashexts.find(ext) != bashexts.end()) {
//cout << "  4-Return script\n";
            ext = "script";
        }
    }
//cout << "  5-Return " << ext << "\n";
    return ext;
}

string get_pfx(string& bangshe, const string& file) {
    bangshe = "";
    auto e = ext(file);
    string comment;
    if (e == "script") {
        ifstream isd(file);
        string line;
        getline(isd, line);
        if (line[0] == '#' && line[1] == '!') {
            comment = "#===-";
            bangshe = line;
        }
        else {
            //cerr << "KO 69855 file w/o ext hasn't got bangshe\n";
            return "";
        }
    }
    else if (e == "cpp") {
        comment = "//===-";
    }
    else if (e == "h") {
        comment = "//===-";
    }
    else if (e == "java") {
        comment = "//===-";
    }
    else if (e == "inc") {
        comment = "//===-";
    }
    else if (e == "makefile") {
        comment = "#===-";
    }
    else if (e == "gradle") {
        comment = "//===-";
    }
    else {
        return "";
    }
    return comment;
}

bool read_header(const string& filename, const string& pfx, vector<unsigned char>& buf) {
    ifstream is(filename);
    ostringstream os;
    int nl = 0;
    while (true) {
        string line;
        getline(is, line);
        if (!is.good()) break;
        os << pfx << line << '\n';
        ++nl;
    }
    if (nl >= MAX_HDR_LINES) {
        cerr << "Header size must be < " << MAX_HDR_LINES << " lines\n";
        return false;
    }
    string s = os.str();
    buf.resize(s.size());
    memcpy(buf.data(), s.data(), s.size());
    return true;
}

bool read_file(const string& filename, vector<unsigned char>& buf) {
    ifstream is(filename, ios::binary | ios::ate);
    if (!is) {
        return false;
    }
    ifstream::pos_type pos = is.tellg();
    buf.resize(pos + 1l);
    buf[0] = 10;
    is.seekg(0, ios::beg);
    is.read((char*)&buf[1], pos);
//cout << "file size " << buf.size() << endl;
    return !is.fail();
}

bool write_file(const string& filename, size_t from, const vector<unsigned char>& buf) {
//    ofstream os(filename + ".check", ios::binary);
    if (_dryrun) {
        cout.write((char*)&buf[from], buf.size() - from);
        return true;
    }
    ofstream os(filename , ios::binary);
    if (!os) {
        return false;
    }
    os.write((char*)&buf[from], buf.size() - from);
//cout << "file size " << buf.size() << endl;
    return !os.fail();
}

bool fileinfo(const string& file, vector<uint8_t>& content, string& comment, size_t& hdr_start) {
    if (!read_file(file, content)) {
        cerr << "KO 69853 Error reading file\n";
        return false;
    }
    hdr_start = 0;
    auto e = ext(file);
//cout << "e " << e << endl;
    if (e == "script") {
        if (content.size() < 2) {
            //cerr << "Empty file no ext\n";
            return false;
        }
        if (content[1] == '#' && content[2] == '!') {
            comment = "#===-";
            auto i = content.begin();
            auto e = content.end();
            uint8_t lf = 10;
            ++i;
            i = find(i, e, lf);
            if (i == content.end()) {
                hdr_start = content.size() - 1;
                return true;
            }
            hdr_start = distance(content.begin(), i);
            return true;
        }
        //cerr << "KO 69855 file w/o ext hasn't got bangshe\n";
        return false;
    }
    else if (e == "cpp") {
        comment = "//===-";
        return true;
    }
    else if (e == "h") {
        comment = "//===-";
        return true;
    }
    else if (e == "java") {
        comment = "//===-";
        return true;
    }
    else if (e == "inc") {
        comment = "//===-";
        return true;
    }
    else if (e == "makefile") {
        comment = "#===-";
        return true;
    }
    else if (e == "gradle") {
        comment = "//===-";
        return true;
    }
    //cerr << "KO 69855 ext not recognized\n";
    return false;
}

map<string, vector<uint8_t>> hdrs;

const vector<uint8_t>& get_header(const string& hfile, const string& pfx) {
    auto i = hdrs.find(pfx);
    if (i == hdrs.end()) {
        vector<uint8_t> header;
        if (!read_header(hfile, pfx, header)) {
            cerr << "KO 69853 Error reading header file\n";
            exit(1);
        }
        i = hdrs.emplace(pfx, move(header)).first;
    }
    return i->second;
}

int add_header(const string& bangshe, const vector<uint8_t>& header, const string& file) {
    ifstream is(file, ios::binary | ios::ate);
    if (!is) {
        return 1;
    }

    ifstream::pos_type pos = is.tellg();
    vector<uint8_t> buf;
    buf.resize((size_t)pos + header.size());
    is.seekg(0, ios::beg);
    size_t p = 0;
    if (!bangshe.empty()) {
        is.read((char*)&buf[p], bangshe.size() + 1);
        p += bangshe.size() + 1;
    }
    size_t r = (size_t)pos - p; 
    memcpy(buf.data() + p, header.data(), header.size());
    p += header.size();
    is.read((char*)&buf[p], r);
//cout << "file size " << buf.size() << endl;
    if (is.fail()) return 1;

    if (!write_file(file, 0, buf)) {
        return 1;
    }

    return 0;
}

int add_header(const string& hfile, const string& file) {
    string bangshe;
    string pfx = get_pfx(bangshe, file);
    if (pfx.empty()) {
        return 1;
    }
    const auto& header = get_header(hfile, pfx);
    return add_header(bangshe, header, file);
}

int remove_header(const string& file) {
//cout << file << endl;
    vector<uint8_t> content;
    string comment;
    size_t hdr_start;
    if (!fileinfo(file, content, comment, hdr_start)) {
        return 1;
    }

    //cout << ext(file) << endl;
    //cout << "hdr_start " << hdr_start << endl;
    //cout << "comment " << comment << endl;
    auto n = content.begin();
    advance(n, hdr_start);
    if (n == content.end()) return 0;
//cout << *n << " " << hdr_start << " " << content.size() << endl;
    if (hdr_start < content.size() - 1) { // empty file
        assert(*n == 10);
    }

    vector<uint8_t> s;
    s.resize(1 + comment.size());
    s[0] = 10;
//    s.reserve(1 + comment.size());
    memcpy(s.data() + 1, comment.data(), comment.size());
//    copy(comment.begin(), comment.end(), back_inserter(s));

    
    while(true) {
        //++n;
        auto i = search(n, content.end(), s.begin(), s.end());
        if (i == content.end()) {
            //n = search(n, content.end(), s.begin(), s.begin() + 1); //position at next lf
            //++n;
            //cout << "pattern found at offset " << distance(content.begin(), n) << endl;  
            break;
        }
        int nl = count(content.begin(), i, '\n');
        if (nl > MAX_HDR_LINES) {
            //if i > MAX_HDR_LINES, knowing header < MAX_HDR_LINES, this match belongs to actual body.
            break;
        }
        n = search(i + 1, content.end(), s.begin(), s.begin() + 1);
        //n = i;
    }
//cout << hdr_start << endl;
//cout << content.size() << endl;
    auto b0 = content.begin() + (distance(content.begin(), n) - hdr_start + 1);
    if (b0 == content.end()) return 0;
    auto b = b0;
//cout << distance(content.begin(), n) << endl;
//cout << distance(b, n) << endl;
    size_t i = 1;
    if (distance(b, n) > 0) {
        memmove(&*b, &content[1], distance(b, n));
    }
/*
        while (b != n) {
    cout << i << endl;
            *b = content[i];
            ++b;
            ++i;
        }
*/
    //}
    if (!write_file(file, distance(content.begin(), b0), content)) {
        cerr << "KO 78699 Coult not write file.\n";
        exit(1);
    }
    //cout << "body offset = " << distance(content.begin(), b0) << endl;
    //cout << "body offset = " << distance(content.begin(), n) << endl;
    //int mm=0;
    //while (n++ != content.end()) {
    //    cout << *n;
    //    if (++mm == 30) break;
    //}
    return 0;
}

vector<string> collect_files(const string& dir) {
    namespace fs = std::filesystem;
    vector<string> r;
    r.reserve(10000);
    for (auto& p: fs::recursive_directory_iterator(dir)) {
        if (!is_regular_file(p.path())) continue;
        auto s = p.path().generic_string();
        if (exclude(s)) continue;
        r.push_back(s);
    }
    return move(r);
}

int add_header_dir(const string& hfile, const string& dir) {
    vector<string> files = collect_files(dir);
    int w = 0;
    int b = 0;
    for (auto& file: files) {
        if (++w % 10 == 0) {
            ++b;
            cout << '.'; cout.flush();
            if (b % 60 == 0) cout << '\n';
        }

        add_header(hfile, file);
    }
    cout << '\n';
    return 0;
}

int remove_header_dir(const string& dir) {
    vector<string> files = collect_files(dir);
    int w = 0;
    int b = 0;
    for (auto& file: files) {
        if (++w % 10 == 0) {
            ++b;
            cout << '.'; cout.flush();
            if (b % 60 == 0) cout << '\n';
        }
        remove_header(file);
    }
    cout << '\n';
    return 0;
}

bool is_white(const char* p) {
    auto c = *p;
    if (c <= ' ') return true;
    return false;
}

bool is_identifier(const char* p) {
    auto c = *p;
    if (c >= '0' && c <= '9') return true;
    if (c >= 'A' && c <= 'Z') return true;
    if (c >= 'a' && c <= 'z') return true;
    if (c == '_') return true;
    if (c == '-') return true;
    return false;
}

void advance_to_identifier(const char*& p, const char* e) {
    while (p != e) {
//        cout << "advance_to_identifier p " << (void*)p << " [" << (char)*p << "] e " << (void*)e << endl;
        if (!is_white(p)) break;
        ++p;
    }
//    cout << "found p " << (void*)p << " [" << (char)*p << "] e " << (void*)e << endl;
    return;
}

string extract_identifier(const char*& p, const char* e) {
//cout << "extract_identifier p " << (void*)p << " e " << (void*)e << endl;
    advance_to_identifier(p, e);
    char context = ' ';
    auto p0 = p;
    char prev = ' ';
    while (p != e) {
        char c = *p;
//        cout << "iteration " << (void*)p << " [" << (char)*p << "] e " << (void*)e << endl;
        if (c == '#' && prev != '\\') {
//cout << "ctx#IN\n";
            context='#';
            break;
        }
        if (c == '\"' && prev != '\\') {
            if (context == ' ') {
//cout << "ctx\"IN\n";
                context = '\"';
                ++p;
                p0 = p;
                continue;
            }
            if (context == '\"') {
//cout << "ctx\"OUT\n";
                context = ' ';
                break;
            }
        }
        if (c == '\'' && prev != '\\') {
            if (context == ' ') {
//cout << "ctx\'IN\n";
                context = '\'';
                ++p;
                p0 = p;
                continue;
            }
            if (context == '\'') {
//cout << "ctx\'OUT\n";
                context = ' ';
                break;
            }
        }
        if (context == ' ' && c != '\\') {
            if (!is_identifier(p)) {
//cout << "found a non-identifier char.\n";
                break;
            }
        }
        prev = c;
        ++p;
    }
    if (p == p0) {
        return "";
    }
    string ans = string(p0, p - p0);
//cout << "ans = " << ans << endl;
    return ans;
}

void advance_after(const char*& p, const char* e, char f) {
//cout << "advance_after " << endl;
    while (p != e) {
        if (*p == f) {
//cout << "found char " << f << endl;
            ++p;
            break;
        }
        ++p;
    }
}

pair<string, string> extract_key_value(const string& line) {
//cout << "extract_key_value line: " << line << endl;
    const char* p = line.c_str();
    const char* e = p + line.size();
    pair <string, string> r;
    r.first = extract_identifier(p, e);
//cout << "key: " << r.first << endl;
    if (r.first.empty()) {
        return make_pair("", "");
    }
    advance_after(p, e, '=');
    r.second = extract_identifier(p, e);
//cout << "value: " << r.second << endl;
    return r;
}

map<string, string> load_vars(const string& file) {
    map<string, string> vars;
    ifstream is(file);
    while (is.good()) {
        string line;
        getline(is, line);
        pair<string, string> r = extract_key_value(line);
//cout << "key=" << r.first << " value=" << r.second << '\n';
        if (r.first.empty()) {
            continue;
        }
        auto i = vars.find(r.first);
        if (i == vars.end()) {
            vars.emplace(r.first, r.second);
        }
        else {
            i->second = r.second;
        }
    }

/*
    cout << "----------------vars----------------:\n";
    cout << "load:\n";
    for (auto &i: vars) {
        cout << "key: " << i.first << " value: " << i.second << '\n';
    }
    cout << "-/--------------vars----------------:\n";
*/

    return vars;
}

string replace_tokens0(const string varpfx, const string& line) {
    string token = "##srctool_header##";
    auto i = line.find(token);
    if (i == string::npos) {
        return line;
    }
    string pfx = line.substr(0, i);
    string sfx = line.substr(i + token.size());
//cout << "varpfx = " << varpfx << endl;
    ostringstream os;
    os << pfx << "-----------------------------------" << sfx << '\n';
    os << pfx << "---- DO NOT EDIT ------------------" << sfx << '\n';
    os << pfx << "-----------------------------------" << sfx << '\n';
    os << pfx << "-- file generated from             " << sfx << '\n';
    os << pfx << "--   ##" << varpfx << "_SRC##\n";
    os << pfx << "-- by                              " << sfx << '\n';
    os << pfx << "--   ##" << varpfx << "_BY##\n";
    os << pfx << "-----------------------------------" << sfx << '\n';
    os << '\n';
    return os.str();
}

string replace_tokens(const string varpfx, const string& line, function<string(const string&)> f) {
    const char* p = line.c_str();
    const char* e = p + line.size();
//cout << endl;
//cout << endl;
//cout << "replace_tokens: " << line << endl;
//cout << "varpfx = " << varpfx << endl;

    ostringstream os;
    int mode = 0;
    ostringstream buf;
    ostringstream value;
    while (p != e) {
        while (p != e && mode == 0) {
            if (p + 2 < e) {
                if (strncmp(p, "##", 2) == 0) {
//cout << "mode0 -> mode1. " << p << endl;
                    buf = ostringstream();
                    buf << "##";
                    p += 2;
                    mode = 1;
                    break;
                }
            }
            os << (char)*p;
            ++p;
            
        }
        while (p != e && mode == 1) {
            if (p + varpfx.size() > e) {
                os << buf.str();
                mode = 0;
//cout << "mode1 -> mode0. " << p << endl;
                break;
            }
            if (!varpfx.empty()) {
                if (strncmp(p, varpfx.c_str(), varpfx.size()) == 0) {
                    buf << varpfx;
                    p += varpfx.size();
                }
                else {
                    os << buf.str();                    
                    mode = 0;
//cout << "mode1 -> mode0. " << p << endl;
                    break;
                }
            }
            value = ostringstream();
            mode = 2;
//cout << "mode1 -> mode2. " << p << endl;
            break;
        }
        while (p != e && mode == 2) {
            if (p + 2 > e) {
//                cerr << "KO 50359 overflow" << '\n';
                os << buf.str();
                mode = 0;
//cout << "mode2 -> mode0. " << p << endl;
                break;
            }
            if (strncmp(p, "##", 2) == 0) {
                ostringstream token;
                token << varpfx << value.str();
                auto r = f(value.str());
                os << r;
//        cout << ":::::::::: " << token.str() << " -> " << r << endl;
//                auto i = tokens.find(token.str());
//                if (i != tokens.end()) {
//                    os << i->second;
//                }
                p += 2;
                mode = 0;
//cout << "mode2 -> mode0. " << p << endl;
                break;
            }
            buf << (char)*p;
            value << (char)*p;
            ++p;
        }
    }
    return os.str();
}

int patch(const string& prefix, const string& envfile, const string& file) { //file= output file, file assumes file.in to exist
    auto vars = load_vars(envfile);    
    ostringstream nm;
    nm << file << ".in";
    ifstream is(nm.str());
    if (!is.good()) {
        cerr << "KO 50499 " << file << '\n';
        return 1;
    }
    ofstream os(file);

    auto f = [&](const string& key) -> string {
//cout << "find key " << key << endl;
        auto i = vars.find(key);
        if (i == vars.end()) {
            return "";
        }
        return i->second;
    };

    while (is.good()) {
        string line;
        getline(is, line);
        string oline = line;
//cout << " - " << line << endl;
        oline = replace_tokens0(prefix, oline);
//cout << " > " << oline << endl;
        oline = replace_tokens(prefix, oline, f);
        os << oline << '\n';
//cout << ">> " << oline << endl;
    }    
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        help(cerr);
        return 1;
    }
    int p = 1;
    while(true) {
        string arg = argv[p++];
        if (arg == "--ext") {
            if (argc <= p) {
                help(cerr);
                return 1;
            }
            _ext = argv[p++];
            if (_recursive) {
                cerr << "KO 76998 --ext and --dir cannot be used together.\n";
                return 1;
            }
            if (_ext == "sh") _ext = "script";
            if (argc <= p) {
                help(cerr);
                return 1;
            }
            continue;
        }
        if (arg == "--dir") {
            if (argc <= p) {
                help(cerr);
                return 1;
            }
            if (!_ext.empty()) {
                cerr << "KO 76997 --ext and --dir cannot be used together.\n";
                return 1;
            }
            _recursive = true;
            continue;
        }
        if (arg == "--patch") {
            if (argc <= p) {
                help(cerr);
                return 1;
            }
            string prefix = argv[p++];
            if (argc <= p) {
                help(cerr);
                return 1;
            }
            string envfile = argv[p++];
            if (argc <= p) {
                help(cerr);
                return 1;
            }
            string ofile = argv[p++];
            auto r = patch(prefix, envfile, ofile);
            return r;
        }
        if (arg == "--dryrun") {
            _dryrun = true;
            continue;
        }
        if (arg == "-i") {
            if (argc < p) {
                help(cerr);
                return 1;
            }
            if (_recursive) {
                return remove_header_dir(argv[p]);
            }
            else {
                return remove_header(argv[p]);
            }
        }
        if (arg == "-a") {
            if (argc < p + 1) {
                help(cerr);
                return 1;
            }
            if (_recursive) {
                return add_header_dir(argv[p], argv[p + 1]);
            }
            else {
                return add_header(argv[p], argv[p + 1]);
            }
        }
        break;
    }

    help(cout);
    cerr << "KO 76094 invalid input\n";
    return 1;
}

