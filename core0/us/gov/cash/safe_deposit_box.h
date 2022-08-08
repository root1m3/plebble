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
#include <set>
#include <vector>

#include <us/gov/io/seriable.h>
#include <us/gov/dfs/fileattr_t.h>

namespace us::gov::cash {

    using fileattr_t = us::gov::dfs::fileattr_t;
    struct tx_t;
    struct map_tx;
    struct file_tx;
    struct ttx;
    struct accounts_t;
    struct m_t;
    struct f_t;
    struct t_t;

    struct safe_deposit_box final: io::seriable {

        safe_deposit_box(const cash_t&);
        safe_deposit_box(const safe_deposit_box&);
        safe_deposit_box& operator = (const safe_deposit_box&);
        virtual ~safe_deposit_box();

        cash_t charge(const cash_t& tax); //charges tax plus storage costs
        cash_t maintenance_fee() const;
        void print_kv_b64(ostream&) const;
        hash_t file_hash(const string& path) const;
        vector<uint8_t> file_content(const hash_t& _arg_addr, const hash_t& _arg_content, const string& _arg_id) const;
        void merge(const safe_deposit_box&);
        void on_destroy(accounts_t& ledger);
        void hash_data_to_sign(crypto::ec::sigmsg_hasher_t&) const;
        void hash_data(crypto::ripemd160&) const;
        bool burn(const hash_t& token, const cash_t& amount, cash_t& fee);
        void add(const hash_t& token, const cash_t&);
        bool store(const string& key, const string& value);
        bool store(const string& path, const hash_t& key, uint32_t sz);
        void dump(ostream&) const;
        void diff_files(const safe_deposit_box& newer, vector<pair<hash_t, uint32_t>>& old_files, vector<pair<hash_t, uint32_t>>& new_files) const;
        void get_files(const hash_t&a, vector<pair<hash_t, pair<hash_t, fileattr_t>>>&) const;
        void get_files(vector<pair<hash_t, fileattr_t>>&) const;
        void get_files(vector<pair<hash_t, uint32_t>>&) const;
        void get_files(set<hash_t>&) const;
        void set_supply(const hash_t&token, const cash_t& supply);

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        cash_t value; //gas
        m_t* m{nullptr};
        f_t* f{nullptr};
        t_t* t{nullptr};
    };

}

