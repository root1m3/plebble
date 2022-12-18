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
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <filesystem>

#include <us/gov/sys/app.h>
#include <us/gov/engine/diff.h>

#include "grid_analyst.h"

using c = us::gov::engine::grid_analyst;
using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;

c::grid_analyst(const string& home, bool disabled___) {
}

vector<string> c::files(const string& home) {
    vector<string> r;
    for(auto& p: fs::recursive_directory_iterator(home)) {
        if (!is_regular_file(p.path())) continue;
        r.push_back(p.path());
    }
    return r;
}

