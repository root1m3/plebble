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
#include <vector>
#include <set>
#include <us/gov/io/seriable_map.h>
#include <us/gov/dfs/fileattr_t.h>

namespace us { namespace gov { namespace cash {

    using fileattr_t = us::gov::dfs::fileattr_t;

    struct f_t final: io::seriable_map<hash_t, fileattr_t> {
        f_t();
        f_t(const f_t&);

        f_t& operator = (const f_t&) = delete;
        void dump(const string& prefix, ostream&) const;
        void hash_data_to_sign(sigmsg_hasher_t&) const;
        void hash_data(crypto::ripemd160&) const;
        uint32_t total_mib() const;
        void list_files(const string& path, ostream&) const;
        void get_files(vector<pair<hash_t, fileattr_t>>&) const;
        void get_files(vector<pair<hash_t, uint32_t>>&) const;
        void get_files(const hash_t&, vector<pair<hash_t, pair<hash_t, fileattr_t>>>&) const;
        void get_files(set<hash_t>&) const;
        hash_t file_hash(const string& path) const;
        void diff(const f_t& newer, vector<pair<hash_t, uint32_t>>& old_files, vector<pair<hash_t, uint32_t>>& new_files) const;
        void merge(const f_t&);
        void delete_path(const string&);
        void del_dup();
    };

}}}

