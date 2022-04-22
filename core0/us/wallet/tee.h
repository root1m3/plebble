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
#include <iostream>

namespace us{ namespace wallet {

    using namespace std;

    template <typename char_type, typename traits = std::char_traits<char_type> >
    class basic_teebuf: public std::basic_streambuf<char_type, traits> {
    public:
        typedef typename traits::int_type int_type;
        basic_teebuf(std::basic_streambuf<char_type, traits> * sb1, std::basic_streambuf<char_type, traits> * sb2):sb1(sb1),sb2(sb2) {}

    private:
        virtual int sync() {
            int const r1 = sb1->pubsync();
            int const r2 = sb2->pubsync();
            return r1 == 0 && r2 == 0 ? 0 : -1;
        }

        virtual int_type overflow(int_type c) {
            int_type const eof = traits::eof();
            if (traits::eq_int_type(c, eof)) {
                return traits::not_eof(c);
            }
            else {
                char_type const ch = traits::to_char_type(c);
                int_type const r1 = sb1->sputc(ch);
                int_type const r2 = sb2->sputc(ch);
                return traits::eq_int_type(r1, eof) || traits::eq_int_type(r2, eof) ? eof : c;
            }
        }

    private:
        std::basic_streambuf<char_type, traits> * sb1;
        std::basic_streambuf<char_type, traits> * sb2;
    };

    typedef basic_teebuf<char> teebuf;

    class teestream: public ostream {
    public:
        teestream(std::ostream & o1, std::ostream & o2): std::ostream(&tbuf), tbuf(o1.rdbuf(), o2.rdbuf()) {}
    private:
        teebuf tbuf;
    };


}}



