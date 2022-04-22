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
#include "algorithm.h"

#include <sys/stat.h>

#include <us/gov/config.h>
#include <us/gov/io/cfg.h>
#include <us/gov/cash/ttx.h>
#include <us/gov/cash/map_tx.h>
#include <us/gov/cash/file_tx.h>

#include <us/wallet/protocol.h>

#include "tx_make_p2pkh_input.h"

#define loglevel "wallet/wallet"
#define logclass "algorithm"
#include <us/gov/logs.inc>

using namespace us::wallet::wallet;
using c = us::wallet::wallet::algorithm;

const char* c::KO_9372 = "KO 9372 No data.";

c::algorithm(const string& keys_filename): data(nullptr), keys_filename(keys_filename) {
    log("constructor wallet::algorithm", "keys_filename", keys_filename);
    load();
}

c::~algorithm() {
    save();
    delete data;
}

bool c::file_exists() const {
    return us::gov::io::cfg::file_exists(keys_filename);
}

void c::sync(ostream& os) {
    clear();
    load();
}

bool c::load() {
    log("load");
    if (!us::gov::io::cfg::file_exists(keys_filename)) return false;
    ifstream f(keys_filename);
    while(f.good()) {
        string pkb58;
        f >> pkb58;
        if (pkb58.empty()) continue;
        auto pk = priv_t::from_b58(pkb58);
        if (is_ko(keys_t::verifyx(pk))) {
            continue;
        }
        keys_t k(pk);
        hash_t h = k.pub.compute_hash();
        emplace(h, move(k));
    }
    return true;
}

bool c::save() const {
    log("save");
    if (!need_save) return true;
    {
        ofstream f(keys_filename);
        for (auto& i: *this) {
            f << i.second.priv << ' ';
        }
    }
    chmod(keys_filename.c_str(), S_IRUSR | S_IWUSR);
    need_save = false;
    return true;
}

hash_t c::new_address() {
    keys_t k = keys_t::generate();
    auto h = k.pub.compute_hash();
    emplace(h, move(k));
    need_save = true;
    save();
    return move(h);
}

hash_t c::add_address(const keys_t::priv_t& key) {
    log("add_address");
    if (is_ko(keys_t::verifyx(key))) {
        return hash_t(0);
    }
    keys_t k(key);
    auto h = k.pub.compute_hash();
    if (emplace(h, move(k)).second) {
        need_save = true;
        save();
    }
    return move(h);
}

const keys_t* c::get_keys(const hash_t& address) const {
    auto i = find(address);
    if (i == end()) return nullptr;
    return &i->second;
}

pair<ko, c::account_t> c::balance() const {
    lock_guard<mutex> lock(mx);
    if (data == nullptr) {
        return make_pair(KO_9372, account_t());
    }
    return make_pair(ok, data->merge());
}

std::pair<ko, hash_t> c::select_receiving_account() const { //the account with biggest gas
    log("select_receiving_account");
    lock_guard<mutex> lock(mx);
    if (data == nullptr) {
        auto r = "KO 9121 Data is null.";
        log(r);
        return make_pair(r, hash_t());
    }
    cash_t maxv = 0;
    std::pair<ko, hash_t> ans{nullptr, hash_t()};
    for (auto& i: *data) {
        if (i.second.box.value > maxv) {
            ans.second = i.first;
            maxv = i.second.box.value;
        }
    }
    return ans;
}

void c::dump_balances(ostream& os) const {
    lock_guard<mutex> lock(mx);
    if (data == nullptr) return;
    cash_t b = 0;
    os << "[address] [locking_program] [balance]\n";
    for (auto& i: *data) {
        b += i.second.box.value;
        os << i.first << ' ' << i.second.locking_program << ' ' << i.second.box.value << '\n';
    }
    os << "total balance: " << b << '\n';
}

void c::extended_balance(int detail, ostream& os) const {
    lock_guard<mutex> lock(mx);
    if (data == nullptr) {
        return;
    }
    for (auto& i: *data) {
        i.second.dump("", i.first, detail, os);
    }
}

c::io_accounts_t c::select_destinations_algR0(const cash_t& amount, const hash_t& token) const {
    io_accounts_t ans;
    lock_guard<mutex> lock(mx);
    if (data == nullptr) return ans;
    if (data->empty()) return ans;
    vector<pair<accounts_t::const_iterator, cash_t>> v;
    v.reserve(data->size());
    for (accounts_t::const_iterator i = data->begin(); i != data->end(); ++i) {
        cash_t val = i->second.get_value(token);
        if (val > 0) {
            v.emplace_back(make_pair(i, val));
        }
    }
    if (v.empty()) {
        for (accounts_t::const_iterator i = data->begin(); i != data->end(); ++i) {
            cash_t val = i->second.get_value(hash_t(0));
            if (val > 0) {
                v.emplace_back(make_pair(i, val));
            }
        }
        sort(v.begin(), v.end(), [](const pair<accounts_t::const_iterator, cash_t>& v1, const pair<accounts_t::const_iterator, cash_t>& v2) { return v1.second > v2.second; });
        for (auto i: v) i.second=0;
    }
    else {
        sort(v.begin(), v.end(), [](const pair<accounts_t::const_iterator, cash_t>& v1, const pair<accounts_t::const_iterator, cash_t>& v2) { return v1.second > v2.second; });
    }
    const auto& i = *v.begin(); //receive at wealthiest account
    ans.emplace_back(io_account_t(i.first->first, i.first->second, amount));
    return move(ans);
}

c::io_accounts_t c::select_sources(const asa_t& asa, const cash_t& amount, const hash_t& token) const {
    if (asa == "algS0") {
        return select_sources_algS0(amount, token);
    }
    return io_accounts_t();
}

c::io_accounts_t c::select_destinations(const asa_t& asa, const cash_t& amount, const hash_t& token) const {
    if (asa == "algR0") {
        return select_destinations_algR0(amount, token);
    }
    return io_accounts_t();
}

c::io_accounts_t c::select_sources_algS0(const cash_t& amount, const hash_t& token) const {
    log("select_sources", amount, token);
    assert(data != nullptr);
    io_accounts_t ans;
    vector<pair<accounts_t::const_iterator, cash_t>> v;
    lock_guard<mutex> lock(mx);
    v.reserve(data->size());
    for (accounts_t::const_iterator i = data->begin(); i != data->end(); ++i) {
        cash_t val = i->second.get_value(token);
        if (val > 0) {
            v.emplace_back(make_pair(i, val));
        }
    }
    log("found entries", v.size());
    sort(v.begin(), v.end(), [](const pair<accounts_t::const_iterator, cash_t>& v1, const pair<accounts_t::const_iterator, cash_t>& v2) { return v1.second < v2.second; });
    cash_t remaining = amount;
    for (auto& i: v) {
        if (remaining == 0) break;
        auto available = i.first->second.input_eligible(token);
        log("other account", i.first->first, token, available);
        if (available == 0) {
            log("skipped non-eligible account");
            continue;
        }
        if (available <= remaining) {
            ans.emplace_back(io_account_t(i.first->first, i.first->second, available));
            remaining -= available;
            log("taken it all. remaining", remaining);
        }
        else {
            ans.emplace_back(io_account_t(i.first->first, i.first->second, remaining));
            remaining = 0;
            log("taken the remainder.");
            break;
        }
    }
    return move(ans);
}

c::io_accounts_t c::select_source(const hash_t& src, const cash_t& amount, const hash_t& token) const {
    lock_guard<mutex> lock(mx);
    log("select_source", src, amount, token);
    io_accounts_t ans;
    auto i = data->find(src);
    if (i == data->end()) {
        log("account not found in data", src);
        return ans;
    }
    cash_t remaining = amount;
    auto available = i->second.input_eligible2(token);
    if (available == 0) {
        log("skipped non-eligible account");
        return ans;
    }
    log("available amount in account", token, available);
    if (available < remaining) {
        log("insufficient funds");
        return ans;
    }
    else {
        ans.emplace_back(io_account_t(i->first, i->second, remaining));
        remaining = 0;
        log("taken the remainder.");
    }
    return move(ans);
}

void c::dump(ostream& os) const {
    os << "[private Key] [public key] [address]\n";
    int n = 0;
    lock_guard<mutex> lock(mx);
    for (auto& i: *this) {
        os << '#' << n++ << ": " << i.second.priv << ' ' << i.second.pub << ' ' << i.first << '\n';
    }
}

void c::list(uint16_t showpriv, ostream& os) const {
    os << "#: ";
    if (showpriv > 0) {
        os << "[private Key] ";
    }
    os << "[public key] [address]\n";
    int n = 0;
    lock_guard<mutex> lock(mx);
    if (showpriv > 0) {
        for (auto& i: *this) {
            os << '#' << n++ << ": " << i.second.priv << ' ' << i.second.pub << ' ' << i.first << '\n';
        }
    }
    else {
        for (auto& i: *this) {
            os << '#' << n++ << ": " << i.second.pub << ' ' << i.first << '\n';
        }
    }
    os << size() << " keys\n";
}

c::io_account_t::io_account_t(const hash_t& address, const b& acc, const cash_t& io_amount): b(acc), address(address), io_amount(io_amount) {
}

void c::io_account_t::dump(const string& prefix, int detail, ostream& os) const {
    b::dump(prefix, address, detail, os);
    os << prefix << " io amount: " << io_amount << '\n';
}

cash_t c::io_accounts_t::get_io_amount() const {
    cash_t w = 0;
    for (auto& i: *this) {
        w += i.io_amount;
    }
    return move(w);
}

void c::io_accounts_t::dump(const string& prefix, int detail, ostream& os) const {
    string pfx = prefix + "    ";
    for (auto& i: *this) {
        i.dump(pfx, detail, os);
    }
    os << prefix << "total i/o: " << get_io_amount() << '\n';
}

c::locking_program_input_t c::create_input(const sigmsg_hash_t& h, const privkey_t& sk) {
    locking_program_input_t o;
    ko r = us::gov::crypto::ec::instance.sign(sk, h, o.sig);
    if (unlikely(is_ko(r))) {
        o.sig.zero();
        return move(o);
    }
    o.pubkey = keys::get_pubkey(sk);
    o.sigcode.clear();
    return move(o);
}

c::locking_program_input_t c::generate_locking_program_input(const ttx_t& t, const locking_program_t& locking_program) const {
    if (unlikely(locking_program == 0)) {
        return locking_program_input_t();
    }
    if (locking_program == us::gov::cash::app::locking_program_id) {
        const keys_t* k = get_keys(t.address);
        if (k == nullptr) return locking_program_input_t();
        return create_input(t.get_hash(), k->priv);
    }
    return locking_program_input_t();
}

c::locking_program_input_t c::generate_locking_program_input(const map_tx_t& t, const locking_program_t& locking_program) const {
    if (unlikely(locking_program == 0)) {
        return locking_program_input_t();
    }
    if (locking_program == us::gov::cash::app::locking_program_id) {
        const keys_t* k = get_keys(t.address);
        if (k == nullptr) return locking_program_input_t();
        return create_input(t.get_hash(), k->priv);
    }
    return locking_program_input_t();
}

c::locking_program_input_t c::generate_locking_program_input(const file_tx_t& t, const locking_program_t& locking_program) const {
    if (unlikely(locking_program == 0)) {
        return locking_program_input_t();
    }
    if (locking_program == us::gov::cash::app::locking_program_id) {
        const keys_t* k = get_keys(t.address);
        if (k == nullptr) return locking_program_input_t();
        return create_input(t.get_hash(), k->priv);
    }
    return locking_program_input_t();
}

c::locking_program_input_t c::generate_locking_program_input(const tx_t& t, const sigcode_t& sigcode, const hash_t& address, const locking_program_t& locking_program) const {
    log("generate_locking_program_input");
    if (unlikely(locking_program == 0)) {
        return locking_program_input_t();
    }
    if (locking_program == us::gov::cash::app::locking_program_id) {
        const keys_t* k = get_keys(address);
        if (k == nullptr) {
            log("keys not found for address", address);
            return locking_program_input_t();
        }
        auto o = create_input(t.get_hash(sigcode), k->priv);
        o.sigcode = sigcode;
        return move(o);
    }
    return locking_program_input_t();
}

pair<bool, c::locking_program_t> c::get_locking_program(const hash_t& addr) const {
    lock_guard<mutex> lock(mx);
    if (data == nullptr) {
        return make_pair(false, 0);
    }
    auto b = data->find(addr);
    if(unlikely(b == data->end())) {
        return make_pair(false, 0);
    }
    return make_pair(true, b->second.locking_program);
}

pair<ko, c::file_tx_t*> c::file_tx_sign(const string& txb58) {
    pair<ko, c::file_tx_t*> ret;
    ret.second = new file_tx_t();
    ret.first = ret.second->read(txb58);
    if (unlikely(is_ko(ret.first))) {
        delete ret.second;
        ret.second = nullptr;
        return move(ret);
    }
    file_tx_t& tx = *ret.second;
    auto plp = get_locking_program(tx.address);
    if (!plp.first) {
        delete ret.second;
        ret.second = nullptr;
        return ret;
    }
    locking_program_t locking_program = plp.second;
    if (!us::gov::cash::app::unlock(tx.address, locking_program, tx.locking_program_input, tx)) {
        tx.locking_program_input = move(generate_locking_program_input(tx, locking_program));
        if (!us::gov::cash::app::unlock(tx.address, locking_program, tx.locking_program_input, tx)) {
            tx.locking_program_input = locking_program_input_t();
        }
    }
    return move(ret);
}

ko c::tx_sign(tx_t& tx, const sigcode_t& sigcode) const {
    locking_program_t locking_program = us::gov::cash::app::locking_program_id;
    for (auto& s: tx.sections) {
        for (auto& i: s.inputs) {
            if (i.locking_program_input.sig.is_not_zero()) {
                log("input already signed");
                continue;
            }
            i.locking_program_input = move(generate_locking_program_input(tx, sigcode, i.address, locking_program));
            i.locking_program_input.sigcode = sigcode;
            #ifdef DEBUG
                if (!us::gov::cash::app::unlock(i.address, locking_program, i.locking_program_input, tx)) {
                    auto r = "KO 11008 Unlock test failed";
                    log(r);
                    assert(false);
                    i.locking_program_input = locking_program_input_t();
                }
            #endif
        }
    }
    return ok;
}

#ifdef CFG_PERMISSIONED_NETWORK

#include <us/gov/engine/auth/acl_tx.h>

pair<ko, c::acl_tx_t*> c::tx_make_nodes_acl(const hash_t& addr, bool allow, const priv_t& priv) {
    log("tx_make_nodes_acl", addr, allow);
    pair<ko, acl_tx_t*> ret;
    ret.first = ok;
    ret.second = new acl_tx_t();
    ret.second->addr = addr;
    ret.second->allow = allow;
    ret.second->sign(priv);
    ostream nullos(0);
    if (!ret.second->verify(nullos)) {
        delete ret.second;
        ret.second = nullptr;
        ret.first = "KO 72920 Invalid supplied key.";
        log(ret.first);
    }
    return ret;
}

#endif

pair<ko, c::install_script_t*> c::tx_make_sys_patch_os(const priv_t& priv, const string& script) {
    pair<ko, install_script_t*> ret;
    ret.first = ok;
    ret.second = new install_script_t();
    ret.second->script = script;
    ret.second->sign(priv);
    return ret;
}

pair<ko, c::ttx_t*> c::set_supply(const hash_t& addr, const cash_t& supply) {
    pair<ko, ttx_t*> ret;
    ret.second = new ttx_t();
    ret.second->address = addr;
    ret.second->token = addr;
    ret.second->supply = supply;
    auto plp = get_locking_program(ret.second->address);
    if (!plp.first) {
        delete ret.second;
        ret.second = nullptr;
        ret.first = "KO 32001 A funded account must exist";
        log(ret.first);
        return move(ret);
    }
    ret.second->locking_program_input = move(generate_locking_program_input(*ret.second, plp.second));
    ret.first = ok;
    return ret;
}

pair<ko, c::map_tx_t*> c::map_tx_create(const hash_t& addr, const string& key, const string& value) {
    auto t = new map_tx_t();
    t->address = addr;
    t->key = key;
    t->value = value;
    auto plp = get_locking_program(t->address);
    if (!plp.first) {
        delete t;
        auto r = "KO 41021 Required to input an existing funded account.";
        log(r);
        return make_pair(r, nullptr);
    }
    t->locking_program_input = move(generate_locking_program_input(*t, plp.second));
    return make_pair(ok, t);
}

pair<ko, c::file_tx_t*> c::file_tx_create(const hash_t& addr, const string& path, const hash_t& digest, const size_t& size) {
    auto t = new file_tx_t();
    t->address = addr;
    t->digest = digest;
    t->size = size;
    t->path = us::gov::io::cfg0::rewrite_path(path);
    auto plp = get_locking_program(t->address);
    if (!plp.first) {
        delete t;
        auto r = "KO 41021 Required to input an existing funded account.";
        log(r);
        return make_pair(r, nullptr);
    }
    t->locking_program_input = move(generate_locking_program_input(*t, plp.second));
    return make_pair(ok, t);
}

pair<ko, c::tx_t*> c::tx_make_p2pkh(const tx_make_p2pkh_input& i) {
    pair<ko, tx_t*> ret;
    ret.second = new tx_t();
    tx_t& t = *ret.second;
    io_accounts_t input_accounts;
    if (i.src_addr.is_zero()) {
        asa_t asa = "algS0";
        input_accounts = select_sources(asa, i.amount, i.token);
    }
    else {
        input_accounts = select_source(i.src_addr, i.amount, i.token);
    }
    if (input_accounts.empty()) {
        delete ret.second;
        ret.first = "KO 65026 Insufficient balance.";
        ret.second = nullptr;
        log(ret.first);
        return move(ret);
    }
    if (input_accounts.get_io_amount() != i.amount) {
        delete ret.second;
        ret.first = "KO 76023 Inconsistency on amounts.";
        ret.second = nullptr;
        log(ret.first);
        return move(ret);
    }
    auto& s = t.add_section(i.token);
    s.inputs.reserve(input_accounts.size());
    for (auto& i: input_accounts) {
        s.add_input(i.address, i.io_amount);
    }
    s.add_output(i.rcpt_addr, i.amount);
    if (!t.check_amounts()) {
        delete ret.second;
        ret.first = "KO 20887 Invalid amounts.";
        ret.second = nullptr;
        log(ret.first);
        return move(ret);
    }
    auto sigcode = t.get_sigcode_all();
    for (auto& i: s.inputs) {
        i.locking_program_input = move(generate_locking_program_input(t, sigcode, i.address, cash::app::locking_program_id));
        i.locking_program_input.sigcode = sigcode;
    }
    ret.first = ok;
    return move(ret);
}

void c::hook_charge_pay(string tx) const {
}

void c::on_charge_pay(const tx_t& t) const {
    log("WP 60095 on_charge_pay");
return;
    ostringstream os;
    os << t ;
    thread work(&c::hook_charge_pay,this,os.str());
    work.detach();
}

pair<ko, c::tx_t*> c::tx_charge(const hash_t& addr, const cash_t& ramount, const hash_t& rtoken) const {
    return tx_charge_pay(addr, ramount, rtoken, 0, hash_t(0));
}

pair<ko, c::tx_t*> c::tx_charge_pay(const hash_t& addr, const cash_t& ramount, const hash_t& rtoken, const cash_t& samount, const hash_t& stoken) const {
    log("tx_charge_pay().", addr, ramount, rtoken, samount, stoken);
    using sigcode_section_t = us::gov::cash::sigcode_section_t;
    using section_t = us::gov::cash::tx::section_t;
    pair<ko, tx_t*> ans;
    ans.first = ok;
    if (rtoken == stoken) {
        if (samount > 0 && ramount > 0) {
            ans.first = "KO 60590 Same token.";
            ans.second = nullptr;
            log(ans.first);
            return move(ans);
        }
    }
    if (addr.is_zero()) {
        ans.first="KO 40399 Invalid address.";
        ans.second = nullptr;
        log(ans.first);
        return move(ans);
    }
    if (ramount < 0 || samount < 0) {
        ans.first = "KO 40391 Invalid amount.";
        ans.second = nullptr;
        log(ans.first);
        return move(ans);
    }
    log("new tx");
    ans.second = new tx_t();
    tx_t& tx = *ans.second;
    tx.sections.clear();
    if (ramount > 0) {
        log("add section", rtoken, ramount);
        tx.sections.emplace_back(section_t(rtoken));
        tx.sections[0].add_output(addr, ramount);
    }
    if (samount > 0) {
        log("add section", stoken, samount);
        tx.sections.emplace_back(section_t(stoken));
        tx.sections[1].add_input(addr, samount);
        sigcode_t sc;
        sc.ts = false;
        sc.emplace_back(sigcode_section_t());
        sc.rbegin()->outputs.push_back(0);
        sc.emplace_back(sigcode_section_t());
        sc.rbegin()->inputs.push_back(0);
        const keys_t* k = get_keys(addr);
        if (k == nullptr) {
            ans.first = "KO 30294 I don't know the secret key.";
            ans.second = nullptr;
            log(ans.first);
            return move(ans);
        }
        tx.sections[1].inputs[0].locking_program_input = create_input(tx.get_hash(sc), k->priv);
        tx.sections[1].inputs[0].locking_program_input.sigcode = sc;
    }
    if (tx.sections.empty()) {
        delete ans.second;
        ans.first = "KO 40321 Invalid amount.";
        ans.second = nullptr;
        log(ans.first);
        return move(ans);
    }
    log("transaction:");
    logdump(*ans.second);
    log("calling on_charge_pay");
    on_charge_pay(*ans.second);
    log("tx_charge_pay finished");
    return move(ans);
}

ko c::tx_from_blob(const blob_t& blob_tx, tx_t*& tx) {
    auto r = us::gov::engine::evidence::from_blob(blob_tx);
    if (is_ko(r.first)) {
        return r.first;
    }
    tx = dynamic_cast<tx_t*>(r.second);
    if (tx == nullptr) {
        auto r2 = "KO 65094 Not a tx_t evidence.";
        log(r2);
        delete r.second;
        return r2;
    }
    return ok;
}

ko c::invoice_verify(const tx_t& tx, const cash_t& oamount, const hash_t& otoken, const cash_t& iamount, const hash_t& itoken) const {
    if (oamount == 0) {
        auto r = "KO 21019 expected output amount is zero.";
        log(r);
        return r;
    }
    int expected_sections = 2;
    if (iamount == 0) {
        expected_sections = 1;
    }
    log("expected_sections", expected_sections);
    if (tx.sections.size() != expected_sections) {
        auto r = "KO 21018 Unexpected number of sections.";
        log(r);
        return r;
    }
    auto i = tx.sections.find(otoken);
    if (i == tx.sections.end()) {
        auto r = "KO 29017 pay token not found.";
        log(r);
        return r;
    }
    if (i->inputs.total() != 0) {
        auto r = "KO 29016 pay token already has inputs.";
        log(r);
        return r;
    }
    if (i->outputs.total() != oamount) {
        auto r = "KO 29015 pay token amount donesn't match.";
        log(r);
        return r;
    }
    if (expected_sections == 2) {
        log("reward section");
        auto i = tx.sections.find(itoken);
        if (i == tx.sections.end()) {
            auto r = "KO 29014 reward token not found.";
            log(r);
            return r;
        }
        if (i->outputs.total() != 0) {
            auto r = "KO 29013 reward token already has outputs.";
            log(r);
            return r;
        }
        if (i->inputs.total() != iamount) {
            auto r = "KO 29012 reward token amount doesn't match.";
            log(r);
            return r;
        }
    }
    log("invoice verified ok");
    return ok;
}

ko c::payment_verify(const tx_t& tx, const cash_t& oamount, const hash_t& otoken, const cash_t& iamount, const hash_t& itoken) const {
    if (oamount == 0) {
        auto r = "KO 21119 expected output amount is zero.";
        log(r);
        return r;
    }
    int expected_sections = 2;
    if (iamount == 0) {
        expected_sections = 1;
    }
    log("expected_sections", expected_sections);
    if (tx.sections.size() != expected_sections) {
        auto r = "KO 21118 Unexpected number of sections.";
        log(r);
        return r;
    }
    auto i = tx.sections.find(otoken);
    if (i == tx.sections.end()) {
        auto r = "KO 29117 pay token not found.";
        log(r);
        return r;
    }
    if (i->inputs.total() == 0) {
        auto r = "KO 29116 pay token has no inputs.";
        log(r);
        return r;
    }
    if (i->outputs.total() != oamount) {
        auto r = "KO 29115 pay token amount donesn't match.";
        log(r);
        return r;
    }
    if (expected_sections == 2) {
        log("reward section");
        auto i = tx.sections.find(itoken);
        if (i == tx.sections.end()) {
            auto r = "KO 29114 reward token not found.";
            log(r);
            return r;
        }
        if (i->outputs.total() == 0) {
            auto r = "KO 29113 reward token has no outputs.";
            log(r);
            return r;
        }
        if (i->inputs.total() != iamount) {
            auto r = "KO 29112 reward token amount doesn't match.";
            log(r);
            return r;
        }
    }
    ostringstream v;
    if (!tx.verify(v)) {
        log(v.str());
        auto r = "KO 29111 Transaction doesn't verify.";
        log(r);
        return r;
    }
    log("payment verified ok");
    return ok;
}

ko c::tx_pay(tx_t& tx) const {
    affected_t affected;
    return tx_pay("algS0", "algR0", tx, affected);
}

ko c::tx_pay(const asa_t& asa_pay, const asa_t& asa_charge, tx_t& tx, affected_t& affected) const {
    log("tx_pay", asa_pay, asa_charge);
    vector<pair<int, int>> its;
    int sidx = 0;
    for (auto& s: tx.sections) {
        cash_t to_pay = s.required_input();
        log("tx_pay - section coin ", s.token, "to_pay", to_pay);
        if (to_pay > 0) {
            io_accounts_t input_accounts = select_sources(asa_pay, to_pay, s.token);
            if (input_accounts.empty()) {
                auto r = "KO 29114 Insufficient balance (0)";
                log(r);
                return r;
            }
            if (input_accounts.get_io_amount() != to_pay) {
                auto r = "KO 30119 Insufficient balance.";
                log(r);
                return r;
            }
            affected.add_pay(to_pay, s.token);
            s.inputs.reserve(input_accounts.size());
            int iidx = s.inputs.size();
            for (auto& i: input_accounts) {
                s.add_input(i.address, i.io_amount);
                its.push_back(make_pair(sidx, iidx));
                ++iidx;
            }
        }
        cash_t to_charge = s.required_output();
        log("tx_pay - section coin ", s.token, "to_charge", to_charge);
        if (to_charge > 0) {
            io_accounts_t output_accounts = select_destinations(asa_charge, to_charge, s.token);
            if (output_accounts.empty()) {
                auto r = "KO 32001 No available accounts.";
                log(r);
                return r;
            }
            if(output_accounts.get_io_amount() != to_charge) {
                auto r = "KO 20101 Inconsistency on amounts.";
                log(r);
                return r;
            }
            affected.add_charge(to_charge, s.token);
            s.outputs.reserve(output_accounts.size());
            for (auto& i: output_accounts) {
                s.add_output(i.address, i.io_amount);
            }
        }
        ++sidx;
    }
    if (!tx.check_amounts()) {
        auto r = "KO 30261 Failed check. Input amounts must match output amounts.";
        log(r);
        return r;
    }
    tx.update_ts(0);
    auto sigcode = tx.get_sigcode_all();
    log("using sigcode_all");
    for (auto& n: its) {
        auto& s = tx.sections[n.first];
        auto& i = s.inputs[n.second];
        i.locking_program_input = move(generate_locking_program_input(tx, sigcode, i.address, cash::app::locking_program_id));
        i.locking_program_input.sigcode = sigcode;
    }
    log("tx_pay SUCCESS", "affected", affected.to_string());
    return ok;
}

c::affected_t::affected_t() {
}

string c::affected_t::to_string() const {
    return os.str();
}

void c::affected_t::add_pay(const cash_t& to_pay, const hash_t& coin) {
    os << '-' << to_pay << ' ';
    if (coin.is_zero()) os << LGAS;
    else os << coin;
    os << '\n';
}

void c::affected_t::add_charge(const cash_t& to_charge, const hash_t& coin) {
    os << '+' << to_charge << ' ';
    if (coin.is_zero()) os << LGAS;
    else os << coin;
    os << '\n';
}

