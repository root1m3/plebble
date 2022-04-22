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
#include "test_platform.h"
#if defined(__GLIBC__)
 #include <execinfo.h>
#endif

#include <cxxabi.h>

#include <us/gov/config.h>

#define loglevel "test"
#define logclass "test_platform"
#include "logs.inc"

#include "assert.inc"

namespace us { namespace test {

using namespace std;

using c = us::test::test_platform;

c::test_platform(ostream& os): os(os) {
}

c::~test_platform() {
}

void c::print_stacktrace(ostream&out, unsigned int max_frames) {
    out << "stack trace" << endl;
    void* addrlist[max_frames+1];
    int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));
    if (addrlen == 0) {
    out << "  <empty, possibly corrupt>" << endl;
    return;
    }
    char** symbollist = backtrace_symbols(addrlist, addrlen);
    size_t funcnamesize = 256;
    char* funcname = (char*)malloc(funcnamesize);
    for (int i = 1; i < addrlen; i++) {
    char *begin_name = 0, *begin_offset = 0, *end_offset = 0;
    for (char *p = symbollist[i]; *p; ++p) {
        if (*p == '(')
        begin_name = p;
        else if (*p == '+')
        begin_offset = p;
        else if (*p == ')' && begin_offset) {
        end_offset = p;
        break;
        }
    }
    if (begin_name && begin_offset && end_offset && begin_name < begin_offset) {
        *begin_name++ = '\0';
        *begin_offset++ = '\0';
        *end_offset = '\0';
        int status;
        char* ret = abi::__cxa_demangle(begin_name, funcname, &funcnamesize, &status);
        if (status == 0) {
            funcname = ret;
            out << "  " << symbollist[i] << " : " << funcname << "+" << begin_offset << endl;
        }
        else {
            out << "  " << symbollist[i] << " : " << begin_name << "()+" << begin_offset <<  " " << status << endl;
        }
    }
    else {
        out << "  " << symbollist[i] << endl;
    }
    }
    free(funcname);
    free(symbollist);
}

void c::abort() {
    print_stacktrace(os);
    ::abort();
}

void c::fail(const string& reason) {
    os << "FAIL: " << reason << endl;
    print_stacktrace(os);
    ::abort();
}

}}

