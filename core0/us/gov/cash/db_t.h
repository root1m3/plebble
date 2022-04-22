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
#include <mutex>

namespace us { namespace gov { namespace cash {

    struct accounts_t;

    struct db_t final: io::seriable {
        db_t();
        db_t(const db_t&) = delete;
        db_t& operator = (const db_t&) = delete;
        db_t(db_t&&);
        ~db_t();
        void clear();

    public:
        bool add_(const hash_t&, const cash_t& amount);
        bool withdraw_(const hash_t& k, const cash_t& amount);
        cash_t budget() const;
        void tax_accounts(const cash_t& amount);
        cash_t cash_in_circulation() const;

    public:
        void dump(const string& prefix, int detail, ostream&) const;
        ko print_data(const hash_t& addr, ostream&) const;
        void print_kv_b64(const hash_t& addr, ostream&) const;

    public:
        void list_files(const hash_t& addr, const string& path, ostream&) const;
        hash_t file_hash(const hash_t& addr, const string& path) const;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        accounts_t* accounts{nullptr};
        cash_t max_subsidy;
        mutable mutex mx;
    };

}}}

