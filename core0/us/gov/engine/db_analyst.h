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
#include "diff.h"
#include <list>
#include <vector>
#include <set>
#include <string>

namespace us { namespace gov { namespace engine {

    using namespace std;

    struct db_analyst {

        struct block: hash_t {
            block(const hash_t&t);
            ~block();
            block(const block&) = delete;
            block& operator = (const block&) = delete;
            string label(const hash_t& head) const;
            string content() const;
            void dump(int th, const hash_t& head, ostream&) const;
            void dump(const string& prefix, ostream&) const;
            void dot(ostream&) const;
            block* insert(block*);
            pair<size_t,size_t> rect() const;
            block* root();

        public:
            mutable size_t w;
            mutable size_t h;
            char type{' '};
            hash_t prevh;
            block* prev{0};
            std::map<hash_t, block*> forks;
        };

        struct chains_t {
            chains_t();
            ~chains_t();
            void dump(const hash_t& head, ostream&) const;
            vector<string> files(const string& home);
            void load(const string& home, const vector<string>& files);
            void load(const string& home);
            void dump(const string& prefix, ostream&) const;
            void dot(ostream& os) const;

            list<block*> roots;
        };

        db_analyst(const string& dir);
        void load_blocks();
        void load_head();
        void dump(ostream&) const;
        void dot(ostream&) const;
        static ko read_header_prev(const string& file, blob_reader_t::blob_header_t& header, hash_t& prev);
        static void print_chain(const string& home, const hash_t& tip, ostream&);
        static void print_app20db(const string& file, ostream&);
        static void print_app30db(const string& file, int detail, ostream&);

        string h;
        hash_t head{0};
        chains_t chains;
    };

}}}

