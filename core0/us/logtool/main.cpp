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
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <iomanip>
#include <chrono>

using namespace std;
using namespace std::chrono;

using tt = void*;

void help(ostream& os) {
    os << "logtool timeline <dir>\n";
    os << "logtool timediff <time0> <time1>\n";
}

void trim(tt t0, tt t1, string srcfile) {
    string dstfile;
    auto n = srcfile.find_last_of("/");
    if (n == string::npos) {
        dstfile = srcfile;
    }
    else {
        dstfile = srcfile.substr(n+1);
    }
    ifstream is(srcfile);
    while(!is.fail()) {
        string s;
        getline(is,s);
        istringstream lis(s);
        tt t;
        lis >> t;
        if (t>=t1) break;
        if (t>=t0) {
            cout << s << endl;
        }
    }
}

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

void files(vector<string>& r, const string& home) {
    for(auto& p: fs::directory_iterator(home)) {
        if (is_directory(p.path())) {
            files(r,p.path());
            continue;
        }
        if (!is_regular_file(p.path())) continue;
        r.push_back(p.path());
    }
}

struct cal_t:multimap<tt,string> {
    void dump(ostream& os) const {
        for (auto&i:*this) {
            os << i.first << ' ' << i.second << endl;
        }
    }
};

void timeline(cal_t& cal, const string& dirX, const string& file) {
    auto p=file.find_last_of('/');
    string fn;
    if (p==string::npos) {
        fn=file;
    }
    else {
        fn=file.substr(p+1);
    }
    auto q=fn.find_last_of('_');
    if (q==string::npos || q==0) {
        return;
    }
    string threadname=fn.substr(0,q);
    ifstream is(file);
    while(!is.fail()) {
        string s;
        getline(is,s);
        istringstream lis(s);
        tt t;
        lis >> t;
        string rem;
        getline(lis,rem);
        if (!rem.empty()) {
            cal.emplace(t,file+": "+threadname+rem);
        }
    }

}

void timeline(string dir) {
    vector<string> f;
    files(f,dir);
    cal_t cal;
    for (auto&i:f) {
        timeline(cal,dir,i);
    }
    cal.dump(cout);
}

namespace {

    template<typename T2, typename T1>
    inline T2 lexical_cast(const T1 &in) {
        T2 out;
        std::stringstream ss;
        ss << in;
        ss >> std::hex >> out;
        return out;
    }

    template <typename Container, typename Fun>
    void tuple_for_each(const Container& c, Fun fun) {
        for (auto& e : c)
            fun(std::get<0>(e), std::get<1>(e), std::get<2>(e));
    }

    string duration_str(chrono::nanoseconds time) {
        using namespace std::chrono;
        using T = std::tuple<nanoseconds, int, const char *>;
        constexpr T formats[] = {
            T{hours(1), 2, ""},
            T{minutes(1), 2, ":"},
            T{seconds(1), 2, ":"},
            T{milliseconds(1), 3, "."},
        };
        std::ostringstream o;
        tuple_for_each(formats, [&time, &o](auto denominator, auto width, auto separator) {
            o << separator << std::setw(width) << std::setfill('0') << (time / denominator);
            time = time % denominator;
        });
        return o.str();
    }

}

void timediff(const string& st0, const string& st1) {
    uint64_t t0 = lexical_cast<uint64_t>(st0);
    uint64_t t1 = lexical_cast<uint64_t>(st1);
    cout << "time diff\n" << t1-t0 << " nsec\n" << duration_str(nanoseconds(t1-t0)) <<  endl;
}

int main(int argc, char**argv) {
    if (argc<2) {
        help(cerr);
        exit(1);
    }
    int n = 1;
    string cmd = argv[n++];
    if (cmd == "timeline") {
        if (argc < 2) {
            help(cerr);
            exit(1);
        }
        string dir = argv[2];
        timeline(dir);

    }
    else if (cmd == "timediff") {
        if (argc<3) {
            help(cerr);
            exit(1);
        }
        string t0 = argv[2];
        string t1 = argv[3];
        timediff(t0, t1);

    }
    else {
        help(cerr);
        exit(1);
    }

}

