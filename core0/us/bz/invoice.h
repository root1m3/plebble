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
#ifndef USGOV_daea0acf65d626f8ec4a1a0528dc8e48df41cc5384b22487a2f1b799465c871b
#define USGOV_daea0acf65d626f8ec4a1a0528dc8e48df41cc5384b22487a2f1b799465c871b

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <us/gov/crypto/base58.h>

namespace us {
namespace bz {

using namespace std;

struct invoice {
    invoice();
    string close();
    void to_stream(ostream&os) const;
    string from_stream(istream&is);
    void dump(ostream&os) const;
    static bool init(const string& home);
    void replace_token(string& s, const string& token, const string& value) const;
    bool save_pdf(const string& outfile) const;
    bool generate_pdf() const;
    bool generate_svg() const;

    public:
    string issue_date;
    string due_date;
    string client_ref;
    string cliaddr1;
    string cliaddr2;
    string cliaddr3;
    string concept0;
    double subtotal;
    double total_vat;
    string bzname;

    private:
    size_t id{0};
    double total;
    bool closed{false};

    public:
    static size_t prev_id;
    static size_t base_id;
    static string homedir;

};

}}

#endif

