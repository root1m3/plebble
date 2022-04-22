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
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

bool read_file(const string& filename, vector<unsigned char>& buf) {
    ifstream is(filename, ios::binary|ios::ate);
    ifstream::pos_type pos = is.tellg();
    buf.resize(pos);
    is.seekg(0, ios::beg);
    is.read((char*)&buf[0], pos);
    return !is.fail();
}


int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "removes file header" << endl;
        cout << "file oldheader_file" << endl;
        return 1;
    }
    string file = argv[1];
    string header = argv[2];
    vector<unsigned char> oldheader;
    vector<unsigned char> content;
    if (!read_file(file, content) ) {
        cerr << "error reading file" << endl;
        return 1;
    }
    if (!read_file(header, oldheader) ) {
        cerr << "error reading header" << endl;
        return 1;
    }
    auto i = content.begin();
    auto h = oldheader.begin();
    while(true) {
        if (i == content.end()) break;
        if (h == oldheader.end()) break;
        if (*i != *h) break;
        ++i;
        ++h;
        continue;
    }
    ofstream os(file, ios::binary | ios::out);
    os.write((const char*)&*i, content.size() - distance(content.begin(), i));
    return 0;
}

