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

#include <us/gov/io/seriable.h>
#include <us/gov/cash/locking_program_input_t.h>

namespace us::gov::cash::tx {

    struct input_t final: io::seriable {

        input_t();
        input_t(const hash_t& address, const cash_t& amount);
        input_t(const hash_t& address, const cash_t& amount, const locking_program_input_t&);
        input_t(const input_t&);

    public:
        void write_sigmsg(sigmsg_hasher_t&) const;
        void write_pretty(const string& prefix, ostream&) const;
        inline uint64_t uniq() const { return *reinterpret_cast<const uint64_t*>(&locking_program_input.sig); }

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        hash_t address;
        cash_t amount;
        locking_program_input_t locking_program_input;
    };

}

