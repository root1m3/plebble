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
#include "section_t.h"

#define loglevel "gov/cash/tx"
#define logclass "section_t"
#include <us/gov/logs.inc>

using namespace std;
using namespace us::gov;
using namespace us::gov::cash;
using c = us::gov::cash::tx::section_t;
using us::ko;

c::section_t() {
}

c::section_t(const hash_t& token): token(token) {
}

c::section_t(const section_t& other): token(other.token), inputs(other.inputs), outputs(other.outputs) {
}

void c::add_input(const hash_t& addr, const cash_t& amount) {
    inputs.emplace_back(input_t(addr, amount));
}

void c::add_input(const hash_t& addr, const cash_t& amount, const locking_program_input_t& locking_program_input) {
    inputs.emplace_back(input_t(addr, amount, locking_program_input));
}

void c::add_output(const hash_t& addr, const cash_t& amount) {
    outputs.emplace_back(output_t(addr, amount));
}

void c::write_sigmsg(sigmsg_hasher_t& h, const sigcode_section_t& sc) const {
    h.write(token);
    inputs.write_sigmsg(h, sc);
    outputs.write_sigmsg(h, sc);
}

void c::write_pretty(const string& prefix, ostream& os) const {
    inputs.write_pretty(prefix, os);
    outputs.write_pretty(prefix, os);
}

cash_t c::required_input() const {
    cash_t sum_inputs{0};
    cash_t sum_outputs{0};
    for (auto& i: inputs) {
        sum_inputs += i.amount;
    }
    for (auto& i: outputs) {
        sum_outputs += i.amount;
    }
    if (sum_outputs < sum_inputs) return 0;
    return sum_outputs - sum_inputs;
}

cash_t c::required_output() const {
    cash_t sum_inputs{0};
    cash_t sum_outputs{0};
    for (auto& i: inputs) {
        sum_inputs += i.amount;
    }
    for (auto& i: outputs) {
        sum_outputs += i.amount;
    }
    if (sum_outputs > sum_inputs) return 0;
    return sum_inputs - sum_outputs;
}

bool c::check_amounts() const {
    cash_t sum_inputs{0};
    cash_t sum_outputs{0};
    for (auto& i: inputs) {
        if (i.amount <= 0) return false;
        sum_inputs += i.amount;
    }
    for (auto& i: outputs) {
        if (i.amount <= 0) return false;
        sum_outputs += i.amount;
    }
    return sum_inputs == sum_outputs && sum_inputs > 0;
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(token) + blob_writer_t::blob_size(inputs) + blob_writer_t::blob_size(outputs);
 }

void c::to_blob(blob_writer_t& writer) const {
    writer.write(token);
    writer.write(inputs);
    writer.write(outputs);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(token);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(inputs);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(outputs);
        if (is_ko(r)) return r;
    }
    return ok;
}

