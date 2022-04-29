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
#include "coder_tex.h"
#include "api_t.h"
#include <sstream>
#include <cassert>
#include <fstream>

using namespace std;
using namespace us::apitool;
using c = us::apitool::tex::coder;

void c::generate() const {
/*
    ostringstream fn;
    write_file_prefix(fn);
    fn << "datagram_counters";
    string file = fn.str();
    feedback(fn.str());
    ofstream include_os(file);
    api_t::warn_h(include_os);
    for (auto& i: m) {
        do_gov_daemon_api(*i.first, i.second, include_os);
    }
    api_t::warn_f(include_os);
*/
}


