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
#include <vector>
#include <us/gov/ko.h>
#include <us/gov/io/seriable.h>

namespace us::wallet::trader {

    using namespace us::gov;

    struct protocol_selection_t: pair<string, string>, gov::io::seriable {
        using b = pair<string, string>;

        protocol_selection_t();
        protocol_selection_t(string&&, string&&);
        protocol_selection_t(const string&, const string&);
        protocol_selection_t(protocol_selection_t&&);
        protocol_selection_t(const protocol_selection_t&);

        protocol_selection_t& operator = (const protocol_selection_t&);
        bool operator < (const protocol_selection_t& other) const;

        void dump(const string& prefix, ostream&) const;

    public: //serialization blob
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        static protocol_selection_t from_string2(const string&);
        string to_string2() const;

    public: //serialization string
        string to_string() const;
        static void to_streamX(const string&, ostream&);
        static ko from_streamX(istream&, string&);
        void to_streamX(ostream&) const;
        ko from_streamX(istream&);

    };

}

