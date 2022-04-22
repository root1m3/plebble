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
#include <string>
#include <sstream>

#include <us/gov/crypto/types.h>

#include "convert.h"
#include "types.h"

namespace us { namespace gov { namespace io {

    struct shell_args final {

        shell_args(int argc, char** argv);
        shell_args(const string& args); //Copies *must* not live longer than the original
        shell_args(const shell_args&);
        ~shell_args();

        shell_args& operator = (const shell_args&);

        template<typename T>
        T test_next() {
            if (n >= argc) {
                return T();
            }
            string i = argv[n];
            return convert<T>(i);
        }

        template<typename T>
        T next() {
            if (n >= argc) {
                return T();
            }
            string i = argv[n++];
            return convert<T>(i);
        }

        template<typename T>
        T next(const T& default_value) {
            if (n >= argc) {
                return default_value;
            }
            string i = argv[n++];
            return convert<T>(i);
        }

        void rew() {
            if (n > 1) --n;
        }

        static hash_t next_token(istream&);
        hash_t next_token();
        string next_line();
        int args_left() const;
        void dump(const string& prefix, ostream&) const;
        void dump(ostream&) const;

        bool del;
        int argc;
        char** argv;
        int n{1};
    };

}}}

