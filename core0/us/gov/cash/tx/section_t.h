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
#include <us/gov/io/seriable.h>
#include <us/gov/cash/locking_program_input_t.h>
#include "section_t.h"
#include "inputs_t.h"
#include "outputs_t.h"

namespace us { namespace gov { namespace cash { namespace tx {

    struct section_t final: io::seriable {

        section_t();
        section_t(const hash_t& token);
        section_t(const section_t&);
        void add_input(const hash_t& addr, const cash_t& amount);
        void add_input(const hash_t& addr, const cash_t& amount, const locking_program_input_t&);
        void add_output(const hash_t& addr, const cash_t& amount);
        void write_sigmsg(sigmsg_hasher_t&, const sigcode_section_t&) const;
        void write_pretty(const string& prefix, ostream&) const;
        cash_t required_input() const;
        cash_t required_output() const;
        bool check_amounts() const;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        hash_t token;
        inputs_t inputs;
        outputs_t outputs;
    };

}}}}

