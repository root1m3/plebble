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
#include <fstream>
#include <unordered_map>
#include <us/gov/config.h>
#include <us/gov/types.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/sys/install_script.h>
#include <us/gov/engine/auth/acl_tx.h>
#include <us/gov/cash/sigcode_t.h>
#include <us/gov/cash/tx_t.h>
#include <us/gov/cash/app.h>
#include <us/gov/cash/locking_program_input_t.h>

namespace us::wallet::wallet {

    using namespace us::gov;

    struct tx_make_p2pkh_input;

    struct algorithm: unordered_map<hash_t, keys_t> {

        using addr_t = hash_t;
        using account_t = us::gov::cash::account_t;
        using accounts_t = us::gov::cash::accounts_t;
        using tx_t = us::gov::cash::tx_t;
        using ttx_t = us::gov::cash::ttx;
        using map_tx_t = us::gov::cash::map_tx;
        using file_tx_t = us::gov::cash::file_tx;
        using sigcode_t = us::gov::cash::sigcode_t;
        using locking_program_t = us::gov::locking_program_t;
        using locking_program_input_t = us::gov::cash::locking_program_input_t;

        #ifdef CFG_PERMISSIONED_NETWORK
        using acl_tx_t = us::gov::engine::acl_tx;
        #endif

        using install_script_t = us::gov::sys::install_script;

        static const char *KO_9372;

        struct io_account_t: us::gov::cash::account_t {
            using b = us::gov::cash::account_t;
            io_account_t(const hash_t& address,const b& acc, const cash_t& io_amount);
            void dump(const string& prefix, int detail, ostream&) const;
            hash_t address;
            cash_t io_amount;
        };

        struct io_accounts_t: vector<io_account_t> {
            cash_t get_io_amount() const;
            void dump(const string& prefix, int detail, ostream&) const;
        };

        struct affected_t: vector<string> {
            affected_t();

        public:
            void add_pay(const cash_t& to_pay, const hash_t& coin);
            void add_charge(const cash_t& to_charge, const hash_t& coin);
            string to_string(string&& prefix) const;

        };

        algorithm(const string& keys_file);
        virtual ~algorithm();

        bool file_exists() const;
        bool load();
        bool save() const;
        hash_t new_address();
        hash_t add_address(const priv_t& key);
        const keys_t* get_keys(const hash_t& address) const;
        pair<ko, account_t> balance() const;
        void dump_balances(ostream&) const;
        void extended_balance(int detail, ostream&) const;
        void accounts_with_gas(ostream&) const;
        using asa_t = string;
        io_accounts_t select_sources(const asa_t&, const cash_t& amount, const hash_t& token) const;
        io_accounts_t select_sources_algS0(const cash_t& amount, const hash_t& token) const;
        io_accounts_t select_destinations(const asa_t&, const cash_t& amount, const hash_t& token) const;
        io_accounts_t select_destinations_algR0(const cash_t& amount, const hash_t& token) const;
        io_accounts_t select_source(const hash_t& src, const cash_t& amount, const hash_t& token) const;
        static locking_program_input_t create_input(const sigmsg_hash_t&, const priv_t&);
        locking_program_input_t generate_locking_program_input(const ttx_t& t, const locking_program_t&) const;
        locking_program_input_t generate_locking_program_input(const map_tx_t& t, const locking_program_t&) const;
        locking_program_input_t generate_locking_program_input(const file_tx_t& t, const locking_program_t&) const;
        locking_program_input_t generate_locking_program_input(const tx_t& t, const sigcode_t&, const hash_t& address, const locking_program_t&) const;
        pair<bool, locking_program_t> get_locking_program(const hash_t& addr) const;
        pair<ko, tx_t*> tx_charge(const hash_t& addr, const cash_t& ramount, const hash_t& rtoken) const;
        pair<ko, tx_t*> tx_charge_pay(const hash_t& addr, const cash_t& ramount, const hash_t& rtoken, const cash_t& samount, const hash_t& stoken) const;
        ko tx_pay(tx_t&) const;
        ko tx_pay(const asa_t& asa_pay, const asa_t& asa_charge, tx_t& tx, affected_t& affected) const;
        void hook_charge_pay(string tx) const;
        void on_charge_pay(const tx_t&) const;
        void sync(ostream&);
        pair<ko, hash_t> select_receiving_account() const;
        pair<ko, tx_t*> tx_make_p2pkh(const tx_make_p2pkh_input&);
        pair<ko, ttx_t*> set_supply(const hash_t& addr, const cash_t& supply);
        ko tx_sign(tx_t& tx, const sigcode_t& sigcode) const;
        pair<ko, tx_t*> tx_sign(const string& txb58, const sigcode_t&);
        pair<ko, map_tx_t*> map_tx_sign(const string& txb58);
        pair<ko, file_tx_t*> file_tx_sign(const string& txb58);
        pair<ko, install_script_t*> tx_make_sys_patch_os(const priv_t&, const string& script);
        pair<ko, map_tx_t*> map_tx_create(const hash_t& addr, const string& key, const string& value);
        pair<ko, file_tx_t*> file_tx_create(const hash_t& addr, const string& path, const hash_t& digest, const size_t& size = 0);
        static ko tx_from_blob(const blob_t&, tx_t*&);
        void dump(ostream& os) const;
        void list(uint16_t showpriv, ostream&) const;
        ko invoice_verify(const tx_t&, const cash_t& oamount, const hash_t& otoken, const cash_t& iamount, const hash_t& itoken) const;
        ko payment_verify(const tx_t&, const cash_t& oamount, const hash_t& otoken, const cash_t& iamount, const hash_t& itoken) const;

        #ifdef CFG_PERMISSIONED_NETWORK
        pair<ko, acl_tx_t*> tx_make_nodes_acl(const hash_t& addr, bool allow, const priv_t& priv);
        #endif

    public:
        string keys_filename;
        accounts_t* data;
        mutable mutex mx;
        mutable bool need_save{false};
    };

}

