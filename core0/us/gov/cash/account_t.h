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
#include <thread>
#include <chrono>
#include <cassert>
#include <map>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <map>

#include <us/gov/config.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/crypto/crypto.h>
#include <us/gov/io/seriable.h>
#include <us/gov/dfs/fileattr_t.h>
//#include <us/gov/engine/diff.h>
#include <us/gov/engine/app.h>
#include <us/gov/engine/evidence.h>

#include "tx_t.h"
#include "safe_deposit_box.h"

namespace us::gov::cash {

    struct accounts_t;

    struct account_t: io::seriable {
        using box_t = safe_deposit_box;
        using locking_program_t = uint8_t;

        account_t();
        account_t(const locking_program_t&, const box_t&);
        account_t(locking_program_t&&, box_t&&);

        void on_destroy(accounts_t& ledger);
        void merge(const account_t&);
        void dump(const string& prefix, const hash_t& addr, int detail, ostream&) const;
        void list_files(const string& path, ostream&) const;
        void print_data(const hash_t& addr, ostream&) const;
        void hash_data_to_sign(sigmsg_hasher_t&) const;
        void hash_data(hasher_t&) const;
        cash_t get_value(const hash_t& token) const;
        cash_t input_eligible(const hash_t& coin) const; //not eligible if account contains data, or not enough gas
        cash_t input_eligible2(const hash_t& coin) const; //not eligible only if not enough gas
        hash_t file_hash(const string& path) const;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        locking_program_t locking_program;
        box_t box;
    };

}

