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
#include <vector>
#include <sstream>
#include <cassert>
#include <fstream>
#include <functional>
#include <unordered_map>
#include <map>
#include <set>
#include <unordered_set>
#include "config.h"
#include "model.h"
#include "coder_java.h"
#include "coder_cpp.h"
#include "coder_tex.h"

using namespace std;
using namespace us::apitool;

void help(ostream& os) {
    os << "apitool - help\n";
    os << "apitool [no arguments]  Generates api for all supported languages.\n";
}

#include "coder_cpp.h"
#include "coder_java.h"

int main(int argc, char** argv) {
    if (argc == 2) {
        string cmd = argv[1];
        if (cmd == "-h") {
            help(cout);
            return 0;
        }
        else {
            cout << "invalid command " << cmd << endl;
            help(cout);
            return 0;
        }
    }

    coder::feedback_enabled = true;

    int svc0 = 1;
    {
        model m("gov");
        m.reserve(11);
        svc0 = m.add("socket", svc0);
        svc0 = m.add("id", svc0);
        svc0 = m.add("auth", svc0);
        svc0 = m.add("peer", svc0);
        svc0 = m.add("relay", svc0);
        svc0 = m.add("dfs", svc0);
        svc0 = m.add("engine", svc0);
        svc0 = m.add("cash", svc0);
        svc0 = m.add("engine_auth", svc0);
        svc0 = m.add("traders", svc0);
        svc0 = m.add("sys", svc0);
        cpp::coder(m).generate();
        java::coder(m).generate();
        //tex::coder(m).generate();
    }
    {
        model m("wallet");
        m.reserve(4);
        svc0 = m.add("engine", svc0);
        svc0 = m.add("pairing", svc0);
        svc0 = m.add("r2r", svc0);
        svc0 = m.add_delegate("wallet", svc0);
        cpp::coder(m).generate();
        java::coder(m).generate();
        //tex::coder(m).generate();
    }
    if (coder::feedback_enabled) cout << '\n';
    return 0;
}

