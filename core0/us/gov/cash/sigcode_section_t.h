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
#include <us/gov/io/seriable.h>
#include <vector>

namespace us { namespace gov { namespace cash {

    struct sigcode_section_t final: io::seriable {

        struct side_t: vector<uint8_t> {
            void dump(const string& prefix, ostream&) const;
            void dump_line(ostream&) const;
        };

        sigcode_section_t() {}
        sigcode_section_t(const sigcode_section_t& other): inputs(other.inputs), outputs(other.outputs) {}
        sigcode_section_t& operator = (const sigcode_section_t& other) {
            inputs = other.inputs;
            outputs = other.outputs;
            return *this;
        }
        ~sigcode_section_t() override {}

        void dump(const string& prefix, ostream&) const;
        void dump_line(ostream&) const;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        side_t inputs;
        side_t outputs;
    };

}}}

