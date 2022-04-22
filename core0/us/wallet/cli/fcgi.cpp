//===-                           P L E B B L E
//===-                         https://plebble.us
//===-
//===-              Copyright (C) 2017-2022 root1m3@plebble.us
//===-
//===-                      GNU GENERAL PUBLIC LICENSE
//===-                       Version 3, 29 June 2007
//===-
//===-    This program is free software: you can redistribute it and/or modify
//===-    it under the terms of the AGPLv3 License as published by the Free
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
#include <us/gov/config.h>
#if CFG_FCGI == 1

#include "fcgi.h"
#include <unordered_map>

using namespace us::wallet::cli;
using c = us::wallet::cli::fcgi_t;

int c::count_reqs{0};
us::wallet::engine::daemon_t* c::api{nullptr};

template<typename T>
std::basic_string<T> uri_decode(const std::basic_string<T>& in) {
    std::basic_string<T> out;
    out.clear();
    out.reserve(in.size());
    for (std::size_t i = 0; i < in.size(); ++i) {
        if (in[i] == L'%') {
            if (i + 3 <= in.size()) {
                int value;
                std::basic_istringstream<T> is(in.substr(i + 1, 2));
                if (is >> std::hex >> value) {
                    out += static_cast<T>(value);
                    i += 2;
                }
                else {
                    out.clear();
                    return out;
                }
            }
            else {
                out.clear();
                return out;
            }
        }
        else if (in[i] == L'+') {
            out += L' ';
        }
        else {
            out += in[i];
        }
    }
    return move(out);
}

void c::help(ostream& os) const {
        static const string url="https://127.0.0.1/api";
        os << "Content-Type: text/html; charset=utf-8" << endl << endl;
        os << "<pre>" << endl;
        os << "us-wallet functions" << endl;
        os << endl;

    os << "Wallet" << endl;
//#include <us/api/apitool_generated__links_wallet_fcgi_query_strings>
    os << endl;
    os << "Pairing" << endl;
//#include <us/api/apitool_generated__links_pairing_fcgi_query_strings>
    os << endl;
        os << endl;
        os << "</pre>" << endl;
}

namespace {

    pair<string,string> split(const string& s, char c) {
        for (int i = 0; i < s.size(); ++i) {
            if (s[i] == c) {
                return make_pair(s.substr(0, i) ,s.substr(i + 1));
            }
        }
        return make_pair(s, "");
    }

    vector<string> split2(const string& s, char c) {
        vector<string> v;
        int previ = 0;
        for (int i = 0; i < s.size(); ++i) {
            if (s[i] == c) {
                v.push_back(s.substr(previ, i - previ));
                previ = i + 1;
            }
        }
        if (previ<s.size()) v.push_back(s.substr(previ));
        return v;
    }


    struct args_t: unordered_map<string, string> {
    /*
        const string& get(const string& param) const;
        template<typename T>
        T get(const string& param) {
            auto i=find(param);
            if (i==end()) {
                return T();
            }
            return convert<T>(i->second);
        }
        template<typename T>
        T get(const string& param, const T& default_value) {
            auto i = find(param);
            if (i == end()) {
                return default_value;
            }
            return convert<T>(i->second);
        }
    */
    };

    args_t parse_uri(const string& uri) {
        args_t m;
        auto i = uri.find('?');
        if (i == string::npos) return m;
        auto p = split2(&uri[i + 1], '&'); //vector<str> "para=value"
        for (auto& i: p) {
            auto v = split(i, '='); //pair<str,str>
            m.emplace(v);
        }
        return m;
    }

}

void c::draw_home() {
        out << "Content-Type: text/html; charset=utf-8\n\n";
        out <<
"<!DOCTYPE html>\n"
"<html>"
    "<head>"
        "<meta charset='utf-8' />"
        "<title>fastcgi++: Hello World</title>"
    "</head>"
    "<body>"
        "<form method=\"GET\" action=\"/api\">"
            "<table>"
            "<tr>"
                "<td>request:<td>"
                "<td><textarea rows=\"20\" cols=\"160\" name=\"cmd\" value=\"\"></textarea><td>"
            "</tr>"
            "<tr>"
                "<td colspan=\"2\"><input type=\"submit\" value=\"submit\"></input><td>"
            "</tr>"
            "</table>"
        "</form>"
    "</body>"
"</html>";
}

bool c::response() {
    ++count_reqs;

    if (environment().requestMethod == Fastcgipp::Http::RequestMethod::GET) {
        draw_home();
        return true;
    }
    {
    out << "Content-Type: text/plain; charset=utf-8" << endl << endl;
    ostringstream os;
    os << environment().requestMethod << endl;
    return true;
    }

    auto uri = uri_decode(environment().requestUri);




    auto args = parse_uri(uri);
    string cmd; // = args.get<string>("cmd");
    ostringstream os;
///////    TODO
    if (cmd == "balance") {
        //string detailed = args.get<string>("detailed");
        //bool b = detailed == "1";
        //api->balance(b, os);
        os << "Balance RESPONSE" << endl;
    }
    string r = os.str();
    if (!r.empty()) {
        out << "Content-Type: text/plain; charset=utf-8" << endl << endl;
        out << r << endl;
    }
    else {
        help(out);
        out << "environment().requestUri " << environment().requestUri << '\n';
        out << "environment().requestMethod " << environment().requestMethod << '\n'; //http://www.nongnu.org/fastcgipp/doc/2.1/a00120.html#aa3ab66b5e611c1192bb544662abfd50c
        out << "environment().contentType " << environment().contentType << '\n';
        out << "environment().gets " << environment().gets.size() << '\n';
        out << "environment().posts " << environment().posts.size() << '\n';
    }

    return true;
}


#endif

