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
#pragma once
#include <string>
#include <vector>
#include <us/gov/config.h>
#include "types.h"

namespace us { namespace gov { namespace io {

    using namespace std;

    ko read_file_(const string& filename, vector<uint8_t>&);
    ko read_file_(istream& is, vector<unsigned char>&);
    ko read_text_file_(const string& filename, string&);
    ko write_file_(const vector<uint8_t>&, const string& filename);

    ko system_command(const string& command, string& result);

    struct cfg0 {

        static const char* KO_96857, *KO_60534;

        static void check_platform();
        static bool is_big_endian();
        static bool mkdir(const string& d);
        static bool file_exists(const string& f);
        static uint32_t file_size(const string& f);
        static bool ensure_dir(const string& d);
        static string rewrite_path(const string&);
        static bool dir_exists(const string& d);
        static string abs_file(const string& home, const string& fn);
        static void mkdir_tree(string sub, string dir);
        static pair<ko, cfg0> load(const string& home);
        static void trim(string&);
        static void to_identifier(string&);
        static string directory_of_file(const string& file);
        static bool ensure_writable(const string& file);
        //static string file_content(const string& file);
        static pair<ko, pair<string, string>> split_fqn(string fqn);

        cfg0(const string& home);
        cfg0(const cfg0& other);
        virtual ~cfg0();

        string home;
    };

    using cfg_filesystem = cfg0;

}}}

