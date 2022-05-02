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
#include <vector>
#include <string>
#include <map>
#include <mutex>
#include <us/gov/io/seriable_map.h>
#include <us/gov/io/seriable_vector.h>

namespace us { namespace wallet { namespace trader {

    using namespace std;
    using paragraph = string;

    struct paragraphs: us::gov::io::seriable_vector<paragraph> {
        using b = us::gov::io::seriable_vector<paragraph>;
        paragraphs() {}
        paragraphs(const paragraphs& other): b(other) {}
        paragraphs(initializer_list<paragraph> il): b(il) {}

        bool find(const string&) const;
    };

    struct chat_entry: paragraphs {
        using b = paragraphs;
        chat_entry();
        chat_entry(const chat_entry&);
        chat_entry(const paragraph&);
        chat_entry(const paragraphs&);
        void add(const paragraphs&);
        void add(const string&);
        bool is_blank() const;
        void dump(const string& pfx, ostream&) const;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        bool me;
    };

    struct chat_t: us::gov::io::seriable_map<ts_t, chat_entry> {
        using b = us::gov::io::seriable_map<ts_t, chat_entry>;
        using entry = chat_entry;
        using paragraph = us::wallet::trader::paragraph;
        using paragraphs = us::wallet::trader::paragraphs;

        chat_t() {}

        using b::size;
        using b::empty;
        bool is_empty() const;
        void emplacexx(ts_t, const chat_entry&);
        void emplace_errorxx(ts_t, const chat_entry&, const string& msg);
        bool clearxx();
        void dump(const string& pfx, ostream&) const;
        void dump(ostream&) const;
        void dump_(ostream&) const;
        bool empty_me_() const;
        uint8_t num_me_() const;
        bool empty_peer_() const;
        uint8_t num_peer_() const;
        static string formatts(uint64_t ts);
        string last_thing() const;
        string last_thing_me() const;
        const entry* last_entry() const;
        const entry* last_entry_me() const;
        bool need_reply() const;

    private:
        mutable mutex mx;
    };

}}}

