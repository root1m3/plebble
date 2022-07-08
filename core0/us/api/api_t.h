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
#include "apifun.h"

namespace us::apitool {

    using namespace std;
    struct model;

    struct api_t: vector<apifun> {
        string src;
        string name;
        vector<pair<string, bool>> v;

        void collect_in_specs(map<string, vector<pair<string, string>>>&) const;
        void collect_out_specs(map<string, vector<pair<string, string>>>&) const;

        static void warn_h(const string& line_comment_pfx, ostream&);
        void info(const string& line_comment_pfx, ostream&) const;
        static void warn_f(const string& line_comment_pfx, ostream&);
        static void templ_h(const string& line_comment_pfx, const string& filename, ostream&);
        static void templ_f(const string& line_comment_pfx, ostream&);
        static api_t* from_stream(istream&);
        static void feedback_load(const string& file);
        static api_t* load(const string& process, string file);
        vector<pair<string, bool>> compute_get_protocol_vector();
        int svc_end(int svc_begin) const;
        bool feedback_on_ko{false};
        model* m{nullptr};
        bool delegate{false}; // false: handler impl is at peer; true: handler at daemon
    };

}

