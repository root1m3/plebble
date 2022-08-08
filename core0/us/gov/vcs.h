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
#pragma once
#include <string>
#include <sstream>

namespace us::vcs {

    using namespace std;

    extern string src; 
    extern string devjob;
    extern string devjobtag;
    extern string brand;
    extern string branch;
    extern string codehash;
    extern string cfghash;
    extern string hashname;
    extern string version_name;
    extern string build_date;

    static inline void version(ostream& os) {
        os << version_name << ' ' << codehash << ' ' << build_date;
    }

    static inline void name_date(ostream& os) {
        os << version_name << ' ' << build_date;
    }

    static inline string apkfilename() {
        ostringstream os;
        os << brand << "-wallet_android_" << branch << '_' << hashname << ".apk";
        return os.str();
    }

    static inline string sdk_name() {
        ostringstream os;
        os << brand << "-wallet-sdk_java_" << branch;
        return os.str();
    }

    static string version() {
        ostringstream os;
        version(os);
        return os.str();
    }

    static string name_date() {
        ostringstream os;
        name_date(os);
        return os.str();
    }

}

