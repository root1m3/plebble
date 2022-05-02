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
#include "model.h"
#include <sstream>
#include <fstream>

#include "api_t.h"
#include "coder_cpp.h"
#include "coder_java.h"

using namespace std;
using namespace us::apitool;
using c = us::apitool::model;

c::model(const string& process): process(process) {
}

c::~model() {
    for (auto& i: *this) {
        delete i.first;
    }
}

int c::add(const string& name, int svc0) {
    auto a = api_t::load(process, name);
    emplace_back(make_pair(a, svc0));
    a->m = this;
    return a->svc_end(svc0);
}

int c::add_delegate(const string& name, int svc0) {
    auto a = api_t::load(process, name);
    emplace_back(make_pair(a, svc0));
    a->m = this;
    a->delegate = true;
    return a->svc_end(svc0);
}

