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
#pragma once

#include <vector>
#include <string>
#include <functional>

namespace conch {
    using namespace std;

    struct flagdef {
        char short_name;
        string name;
        bool optional;
        bool req_input;
        string value;
        string desc;
        bool overriden{false};

        bool operator == (const string&) const;
        bool operator == (const flagdef&) const;
        void dump(const string& pfx, ostream& os) const;
        void dump_value(const string& pfx, ostream& os) const;
    };

    struct cmddef {
        string name;
        string desc;
        bool operator == (const string&) const;
        bool operator == (const cmddef&) const;
        void dump(const string& pfx, ostream& os) const;
    };
}

namespace std {
    template <>
    struct hash<conch::flagdef> {
        size_t operator()(const conch::flagdef&) const;
    };

    template <>
    struct hash<conch::cmddef> {
        size_t operator()(const conch::cmddef&) const;
    };
}

namespace conch {
    struct params: vector<flagdef> {
        using b=vector<flagdef>;
        using b::vector;
        params(b&&);
        params(const params&, istream&);
        flagdef* lookup(const string&);
        const string& get(const string&) const;
        const string& get(char) const;
        bool is_set(char flag) const;
        bool is_set(const string& flag) const;
        bool any_set() const;
        bool has(char flag) const;
        void set_optional(char flag);
        void set_mandatory(char flag);
        void dump(const string& prefix, ostream&) const;
        void dump_values(const string& prefix, ostream&) const;
        void dump_set(const string& prefix, ostream&) const;
        bool check_req(ostream&) const;
        string ko;
    };

    struct section: private vector<pair<cmddef, section*>> {
        using b=vector<pair<cmddef, section*>>;
        struct v_t:vector<pair<string, params*>> {
            params* lookup(const vector<string>& cmdpath);
        };
        section();
        section(params&&);
        section(function<bool(const params&, ostream&)>);
        void constructor();
        virtual ~section();
        void help(const v_t&) const;
        void help(const v_t&, v_t::const_iterator) const;
        bool fillv(const string& mycmd, v_t&, istream&) const;
        bool exec(const v_t&, v_t::const_iterator) const;
        virtual void rewrite(v_t&) const {}
        bool exec(istream&) const;
        const section* root() const;
        const section* lookup(const string&) const;
        const section* lookup(const v_t&, v_t::const_iterator) const;
        section& add_section(cmddef&&, section*);
        string scope() const;
        static string fmt_field(const string&, int w);
        void set_handler(function<bool(const params&, ostream&)>);
        void add(flagdef&&);
        v_t::const_iterator check_req(const v_t&, ostream&) const;
        section* root();
        static void print_error(const string& msg);

        string name;
        string desc;
        static ostream*pos;
        static ostream*peos;
        static string version;
        const params pdef;
        function<bool(const params&, ostream&)> handler;
        bool skip_handler{true};
        section* parent{nullptr};
    };
}

