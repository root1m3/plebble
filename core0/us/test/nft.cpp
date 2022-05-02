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
/*
#include <iostream>
#include <vector>

using namespace std;

struct da {
    da(const string& file) {
        bin=load(file);
        hash=hash_t::compute(bin);
    }

    vector<uint8_t> bin;
    hash_t hash;
};

struct collection: vector<da*> {
    collection() {}
    ~collection() {
        for (auto&i:*this) delete i;
    }

    bool add_file(const string& file) {
        emplace_back(new da(file));
    }

    bool add_dir(const string& dir) {
        bool r{false};
        for f in dir
            if (!add_file(f)) return false;
        return r;
    }

};


int main_nft() {
    collection nft;
    nft.add("nft0");
}
*/
