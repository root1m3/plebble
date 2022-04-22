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
// stacktrace.h (c) 2008, Timo Bingmann from http://idlebox.net/
// published under the WTFPL v2.0
// Print a demangled stack backtrace of the caller function to FILE* out.
#include <iostream>

#if 0
    /// /////////////////////////// Usage: /////////////////////////////////////////////
#include <us/gov/stacktrace.h>
print_stacktrace(cout);
    /// /////////////////////////// ////// /////////////////////////////////////////////
#endif

#if defined(__GLIBC__)
    #include <stdio.h>
    #include <stdlib.h>
    #include <execinfo.h>
    #include <cxxabi.h>

    static inline void print_stacktrace_release(std::ostream& out, unsigned int max_frames = 10) {
        out << "stack trace:" << std::endl;

        // storage array for stack trace address data
        void* addrlist[max_frames+1];

        // retrieve current stack addresses
        int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

        if (addrlen == 0) {
            out << "  <empty, possibly corrupt>" << std::endl;
            return;
        }

        // resolve addresses into strings containing "filename(function+address)",
        // this array must be free()-ed
        char** symbollist = backtrace_symbols(addrlist, addrlen);

        // allocate string which will be filled with the demangled function name
        size_t funcnamesize = 256;
        char* funcname = (char*)malloc(funcnamesize);

        // iterate over the returned symbol lines. skip the first, it is the
        // address of this function.
        for (int i = 1; i < addrlen; i++) {
        char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

        // find parentheses and +address offset surrounding the mangled name:
        // ./module(function+0x15c) [0x8048a6d]
        for (char *p = symbollist[i]; *p; ++p)
        {
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
                out << "  " << symbollist[i] << " : " << funcname << "+" << begin_offset << std::endl;
            }
            else {
                out << "  " << symbollist[i] << " : " << begin_name << "()+" << begin_offset << std::endl;
            }
        }
        else {
            out << "  " << symbollist[i] << std::endl;
        }
        }

        free(funcname);
        free(symbollist);
    }

    #ifdef DEBUG
        static inline void print_stacktrace(std::ostream& out, unsigned int max_frames = 10) {
                print_stacktrace_release(out, max_frames);
        }
    #else
        #define print_stacktrace (void)sizeof
    #endif
#else
    #define print_stacktrace (void)sizeof
#endif

