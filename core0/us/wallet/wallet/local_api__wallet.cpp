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
#include "local_api.h"
#include <sstream>

#include <us/gov/socket/datagram.h>
#include <us/gov/socket/outbound/rendezvous_t.h>
#include <us/gov/cash/ttx.h>
#include <us/gov/cash/map_tx.h>
#include <us/gov/cash/file_tx.h>
#include <us/wallet/engine/daemon_t.h>
#include <us/wallet/wallet/algorithm.h>
#include <us/wallet/wallet/tx_make_p2pkh_input.h>
#include <us/wallet/trader/bookmarks_t.h>
#include <us/wallet/trader/traders_t.h>
#include <us/wallet/trader/businesses_t.h>

#include "types.h"

#define loglevel "wallet/wallet"
#define logclass "local_api__wallet"
#include <us/gov/logs.inc>

using namespace us::wallet::wallet;
using c = us::wallet::wallet::local_api;

//------------------apitool - API Spec defined @ us/api/generated/wallet/c++/wallet/hdlr_local-impl

ko c::handle_balance(uint16_t&& detail, string& data) {
    log("balance", detail);
    {
        ko r = refresh_data();
        if (unlikely(is_ko(r))) {
            return r;
        }
    }
    if (detail == 0) {
        auto r = w::balance();
        if (unlikely(is_ko(r.first))) {
            return r.first;
        }
        ostringstream os;
        r.second.box.dump(os);
        data = os.str();
    }
    else {
        ostringstream os;
        w::extended_balance(detail, os);
        data = os.str();
    }
    return ok;
}

ko c::handle_list(uint16_t&& detail, string& data) {
    log("list");
    ostringstream os;
    w::list(detail, os);
    data = os.str();
    return ok;
}

ko c::handle_recv_address(hash_t& addr) {
    log("recv_address");
    {
        auto r = refresh_data();
        if (unlikely(is_ko(r))) {
            return r;
        }
    }
    auto r = w::select_receiving_account();
    if (is_ko(r.first)) {
        return r.first;
    }
    addr = r.second;
    return ok;
}

ko c::handle_new_address(hash_t& addr) {
    log("new_address");
    addr = algorithm::new_address();
    if (unlikely(addr.is_zero())) {
        auto r = "KO 20919 Invalid new address.";
        log(r);
        return r;
    }
    return ok;
}

ko c::handle_add_address(priv_t&& priv, hash_t& addr) {
    log("add_address");
    addr = algorithm::add_address(priv);
    if (unlikely(addr.is_zero())) {
        auto r = "KO 20920 Invalid produced address";
        log(r);
        return r;
    }
    return ok;
}

ko c::handle_transfer(transfer_in_dst_t&& o_in, blob_t& blob_ev) {
    log("transfer");
    /// in:
    ///     hash_t addr;
    ///     cash_t amount;
    ///     hash_t coin;
    ///     uint8_t relay;

    {
        wallet::tx_make_p2pkh_input i;
        i.rcpt_addr = o_in.addr;
        i.amount = o_in.amount;
        i.token = o_in.coin;

        {
            ko r = refresh_data();
            if (unlikely(r != ok)) {
                return r;
            }
        }
        auto tx = algorithm::tx_make_p2pkh(i);
        if (is_ko(tx.first)) {
            assert(tx.second == nullptr);
            return tx.first;
        }
        assert(tx.second != nullptr);
        tx.second->write(blob_ev);
        log("resulting blob_ev sz", blob_ev.size());
        delete tx.second;
    }
    if (o_in.relay != 0) {
        blob_t ev = blob_ev;
        auto r = daemon.gov_rpc_daemon.get_peer().call_ev(move(ev));
        if (unlikely(is_ko(r))) {
            blob_ev.clear();
            return r;
        }
    }
    return ok;
}

ko c::handle_transfer_from(transfer_from_in_dst_t&& o_in, blob_t& blob_ev) {
    log("transfer_from");
    /// in:
    ///     hash_t src_addr;
    ///     hash_t rcpt_addr;
    ///     cash_t amount;
    ///     hash_t coin;
    ///     uint8_t relay;

    {
        wallet::tx_make_p2pkh_input i;
        i.src_addr = o_in.src_addr;
        i.rcpt_addr = o_in.rcpt_addr;
        i.amount = o_in.amount;
        i.token = o_in.coin;
        auto tx = tx_make_p2pkh(i);
        if (is_ko(tx.first)) {
            assert(tx.second == nullptr);
            return tx.first;
        }
        assert(tx.second != nullptr);
        tx.second->write(blob_ev);
        delete tx.second;
    }
    if (o_in.relay != 0) {
        blob_t ev = blob_ev;
        auto r = daemon.gov_rpc_daemon.get_peer().call_ev(move(ev));
        if (unlikely(is_ko(r))) {
            blob_ev.clear();
            return r;
        }
    }
    return ok;
}

ko c::handle_tx_charge_pay(tx_charge_pay_in_dst_t&& o_in, blob_t& blob_ev) {
    log("tx_charge_pay");
    /// in:
    ///     hash_t addr;
    ///     cash_t samount;
    ///     hash_t scoin;
    ///     cash_t ramount;
    ///     hash_t rcoin;

    auto tx = algorithm::tx_charge_pay(o_in.addr, o_in.ramount, o_in.rcoin, o_in.samount, o_in.scoin);
    if (unlikely(is_ko(tx.first))) {
        assert(tx.second == nullptr);
        return tx.first;
    }
    tx.second->write(blob_ev);
    delete tx.second;
    return ok;
}

ko c::handle_invoice_verify(invoice_verify_in_dst_t&& o_in, string& err) {
    log("invoice_verify");
    /// in:
    ///     blob_t blob_ev;
    ///     cash_t oamount;
    ///     hash_t ocoin;
    ///     cash_t iamount;
    ///     hash_t icoin;

    tx_t* tx;
    {
        auto r = tx_from_blob(o_in.blob_ev, tx);
        if (is_ko(r)) {
            return r;
        }
    }
    auto r = algorithm::invoice_verify(*tx, o_in.oamount, o_in.ocoin, o_in.iamount, o_in.icoin);
    delete tx;
    if (is_ko(r)) {
        err = r;
    }
    else {
        err = "OK Invoice verified correctly.";
    }
    return ok;
}

ko c::handle_payment_verify(payment_verify_in_dst_t&& o_in, string& err) {
    log("payment_verify");
    /// in:
    ///     blob_t blob_ev;
    ///     cash_t oamount;
    ///     hash_t ocoin;
    ///     cash_t iamount;
    ///     hash_t icoin;

    tx_t* tx;
    {
        auto r = tx_from_blob(o_in.blob_ev, tx);
        if (is_ko(r)) {
            return r;
        }
    }
    auto r = algorithm::payment_verify(*tx, o_in.oamount, o_in.ocoin, o_in.iamount, o_in.icoin);
    delete tx;
    if (is_ko(r)) {
        err = r;
    }
    else {
        err = "OK Payment verified correctly.";
    }
    return ok;
}

ko c::handle_tx_pay(tx_pay_in_dst_t&& o_in, blob_t& blob_ev2) {
    log("tx_pay");
    /// in:
    ///     blob_t blob_ev;
    ///     uint8_t relay;

    tx_t* ptx;
    {
        auto r = tx_from_blob(o_in.blob_ev, ptx);
        if (is_ko(r)) {
            return r;
        }
    }
    auto& tx = *ptx;
    {
        auto r = refresh_data();
        if (unlikely(is_ko(r))) {
            delete ptx;
            return r;
        }
    }
    {
        auto r = algorithm::tx_pay(tx);
        if (is_ko(r)) {
            delete ptx;
            return move(r);
        }
    }
    tx.write(blob_ev2);
    if (o_in.relay != 0) {
        blob_t ev = blob_ev2;
        auto r = daemon.gov_rpc_daemon.get_peer().call_ev(move(ev));
        if (unlikely(is_ko(r))) {
            blob_ev2.clear();
            return r;
        }
    }
    return ok;
}

ko c::handle_tx_new(blob_t& blob_ev) {
    log("tx_new");
    tx_t tx;
    tx.write(blob_ev);
    return ok;
}

ko c::handle_tx_add_section(tx_add_section_in_dst_t&& o_in, blob_t& blob_ev) {
    log("tx_add_section");
    /// in:
    ///     blob_t blob_ev;
    ///     hash_t coin;

    tx_t tx;
    auto r = tx.read(o_in.blob_ev);
    if (is_ko(r)) {
        return r;
    }
    tx.sections.emplace_back(tx_t::section_t(o_in.coin));
    tx.write(blob_ev);
    return ok;
}

ko c::handle_tx_add_input(tx_add_input_in_dst_t&& o_in, blob_t& blob_ev) {
    log("tx_add_input");
    /// in:
    ///     blob_t blob_ev;
    ///     uint16_t section;
    ///     hash_t addr;
    ///     cash_t amount;

    tx_t tx;
    {
        auto r = tx.read(o_in.blob_ev);
        if (is_ko(r)) {
            return r;
        }
    }
    if (o_in.section >= tx.sections.size()) {
        auto r = "KO 76946 Section not found.";
        log(r);
        return r;
    }
    if (o_in.addr.is_zero()) {
        auto r = "KO 20116 Invalid address.";
        log(r);
        return r;
    }
    if (o_in.amount < 1) {
        auto r = "KO 76920 Invalid amount.";
        log(r);
        return r;
    }
    tx.sections[o_in.section].add_input(o_in.addr, o_in.amount);
    tx.write(blob_ev);
    return ok;
}

ko c::handle_tx_add_output(tx_add_output_in_dst_t&& o_in, blob_t& blob_ev) {
    log("tx_add_output");
    /// in:
    ///     blob_t blob_ev;
    ///     uint16_t section;
    ///     hash_t addr;
    ///     cash_t amount;

    tx_t tx;
    {
        auto r = tx.read(o_in.blob_ev);
        if (is_ko(r)) {
            return r;
        }
    }
    if (o_in.section >= tx.sections.size()) {
        auto r = "KO 65029 Section not found.";
        log(r);
        return r;
    }
    if (o_in.addr.is_zero()) {
        auto r = "KO 30214 Invalid address.";
        log(r);
        return r;
    }
    if (o_in.amount < 1) {
        auto r = "KO 76026 Invalid amount.";
        log(r);
        return r;
    }
    tx.sections[o_in.section].add_output(o_in.addr, o_in.amount);
    tx.write(blob_ev);
    return ok;
}

ko c::handle_tx_sign(tx_sign_in_dst_t&& o_in, blob_t& blob_ev) {
    log("tx_sign");
    /// in:
    ///     blob_t blob_ev;
    ///     blob_t blob_sigcode;

    using sigcode_t = us::gov::cash::sigcode_t;
    sigcode_t sigcode;
    {
        auto r = sigcode.read(o_in.blob_sigcode);
        if (is_ko(r)) {
            return r;
        }
    }
    tx_t tx;
    {
        auto r = tx.read(o_in.blob_ev);
        if (is_ko(r)) {
            return r;
        }
    }
    auto r = algorithm::tx_sign(tx, sigcode);
    if (unlikely(is_ko(r))) {
        return r;
    }
    tx.write(blob_ev);
    return ok;
}

ko c::handle_tx_send(blob_t&& blob_ev, string& info) {
    log("tx_send");
    using evidence = us::gov::engine::evidence;
    pair<ko, evidence*> r = evidence::from_blob(blob_ev);
    {
        if (is_ko(r.first)) {
            assert(r.second == nullptr);
            return r.first;
        }
    }
    if (!r.second->check_amounts()) {
        delete r.second;
        auto e = "KO 84308 Invalid amounts.";
        log(e);
        return e;
    }
    delete r.second;
    {
        auto r = daemon.gov_rpc_daemon.get_peer().call_ev(move(blob_ev));
        if (unlikely(is_ko(r))) {
            return r;
        }
    }
    info = "Transaction successfully relayed.";
    return ok;
}

ko c::handle_tx_decode(blob_t&& blob_ev, string& tx_pretty) {
    log("tx_decode");
    using evidence = us::gov::engine::evidence;
    pair<ko, evidence*> r = evidence::from_blob(blob_ev);
    {
        if (is_ko(r.first)) {
            assert(r.second == nullptr);
            return r.first;
        }
    }
    ostringstream os;
    if (lang == "es") {
        r.second->write_pretty_es(os);
    }
    else {
        r.second->write_pretty_en(os);
    }
    tx_pretty = os.str();
    delete r.second;
    return ok;
}

ko c::handle_tx_check(blob_t&& blob_ev, string& data) {
    log("tx_check");
    using evidence = us::gov::engine::evidence;

    pair<ko, evidence*> r = evidence::from_blob(blob_ev);
    {
        if (is_ko(r.first)) {
            assert(r.second == nullptr);
            return r.first;
        }
    }
    if (!r.second->check_amounts()) {
        data = "Invalid amounts.";
        delete r.second;
        return ok;
    }
    ostringstream os;
    r.second->verify(os);
    data = os.str();
    delete r.second;
    return ok;
}

ko c::handle_set_supply(set_supply_in_dst_t&& o_in, blob_t& blob_ev) {
    log("set_supply");
    /// in:
    ///     hash_t addr;
    ///     cash_t amount;
    ///     uint8_t relay;

    {
        auto r = refresh_data();
        if (unlikely(is_ko(r))) {
            return r;
        }
    }
    auto tx = algorithm::set_supply(o_in.addr, o_in.amount);
    if (is_ko(tx.first)) {
        assert(tx.second == nullptr);
        return tx.first;
    }
    tx.second->write(blob_ev);
    delete tx.second;
    if (o_in.relay != 0) {
        auto ev = blob_ev;
        auto r = daemon.gov_rpc_daemon.get_peer().call_ev(move(ev));
        if (unlikely(is_ko(r))) {
            blob_ev.clear();
            return r;
        }
    }
    return ok;
}

ko c::handle_store_kv(store_kv_in_dst_t&& o_in, blob_t& blob_ev) {
    log("store_kv");
    /// in:
    ///     hash_t addr;
    ///     string key;
    ///     string value;
    ///     uint8_t relay;

    {
        auto r = refresh_data();
        if (unlikely(is_ko(r))) {
            return r;
        }
    }
    auto tx = algorithm::map_tx_create(o_in.addr, o_in.key, o_in.value);
    if (is_ko(tx.first)) {
        assert(tx.second == nullptr);
        return tx.first;
    }
    tx.second->write(blob_ev);
    delete tx.second;
    if (o_in.relay != 0) {
        auto ev = blob_ev;
        auto r = daemon.gov_rpc_daemon.get_peer().call_ev(move(ev));
        if (unlikely(is_ko(r))) {
            blob_ev.clear();
            return r;
        }
    }
    return ok;
}

ko c::handle_rm_kv(rm_kv_in_dst_t&& o_in, blob_t& blob_ev) {
    log("rm_kv");
    /// in:
    ///     hash_t addr;
    ///     string key;
    ///     uint8_t relay;

    store_kv_in_dst_t o;
    o.addr = o_in.addr;
    o.key = o_in.key;
    o.relay = o_in.relay;
    return handle_store_kv(move(o), blob_ev);
}

ko c::handle_store_file(store_file_in_dst_t&& o_in, store_file_out_t& o_out) {
    log("store_file");
    /// in:
    ///     hash_t addr;
    ///     string path;
    ///     blob_t content;
    ///     uint8_t relay;

    /// out:
    ///    blob_t blob_ev;
    ///    string path;
    ///    hash_t digest;

    if (!us::gov::socket::datagram::validate_payload(o_in.content)) {
        auto r = "KO 12146 File size exceed datagram limit.";
        log(r);
        return r;
    }
    if (o_in.content.empty()) {
        auto r = "KO 12146 Empty file.";
        log(r);
        return r;
    }
    {
        ko r = refresh_data();
        if (unlikely(is_ko(r))) {
            return r;
        }
    }
    size_t sz = o_in.content.size();
    auto hash = us::gov::crypto::ripemd160::digest(o_in.content);
    auto tx = algorithm::file_tx_create(o_in.addr, o_in.path, hash, sz);
    if (is_ko(tx.first)) {
        assert(tx.second == nullptr);
        return tx.first;
    }
    tx.second->write(o_out.blob_ev);
    o_out.path = tx.second->path;
    o_out.digest = tx.second->digest;
    delete tx.second;
    if (o_in.relay != 0) {
        string error;
        auto r = daemon.gov_rpc_daemon.get_peer().call_share_file(daemon_t::gov_rpc_daemon_t::peer_type::share_file_in_t(o_out.blob_ev, o_in.content), error);
        if (unlikely(is_ko(r))) {
            return r;
        }
        if (!error.empty()) {
            auto r = "KO 80799 Error relaying evidence";
            log(r, error);
            return r;
        }
    }
    return ok;
}

ko c::handle_rm_file(rm_file_in_dst_t&& o_in, blob_t& blob_ev) {
    log("rm_file");
    /// in:
    ///     hash_t addr;
    ///     string path;
    ///     hash_t digest;
    ///     uint8_t relay;

    {
        auto r = refresh_data();
        if (unlikely(is_ko(r))) {
            return r;
        }
    }
    pair<ko, file_tx_t*> tx = algorithm::file_tx_create(o_in.addr, o_in.path, o_in.digest, 0);
    if (is_ko(tx.first)) {
        return tx.first;
    }
    tx.second->write(blob_ev);
    if (o_in.relay != 0) {
        string error;
        auto r = daemon.gov_rpc_daemon.get_peer().call_share_file(daemon_t::gov_rpc_daemon_t::peer_type::share_file_in_t(blob_ev, vector<uint8_t>()), error);
        if (unlikely(is_ko(r))) {
            return r;
        }
        if (!error.empty()) {
            auto r = "KO 80799 Error relaying evidence";
            log(r, error);
            return r;
        }
    }
    return ok;
}

ko c::handle_search(search_in_dst_t&& o_in, string& result) {
    log("search");
    /// in:
    ///     hash_t addr;
    ///     string pattern;

    /*
        log("search");
        os << get_endpoint().pubkey.hash() << endl;
        auto d=new datagram(gov::protocol::gov_engine_search_kv,0,addr,words);
        auto r=sendrecv(d);
        if (!r.first.empty()) {
            assert(r.second==0);
            return;
        }
        assert(r.second!=0);
        if (unlikely(r.second->service!=gov::protocol::gov_engine_search_kv+(1<<2))) {
            os << "KO 8401 - unexpected service " << r.second->service << endl;
            delete r.second;
            return;
        }
        auto r2=r.second->parse_string();
        os << r2 << endl;
        delete r.second;
    */

    auto r = "KO 76994 Disabled function.";
    log(r);
    return r;
}

ko c::handle_file(hash_t&& digest, vector<uint8_t>& content) {
    log("file");
    return daemon.gov_rpc_daemon.get_peer().call_file_request(digest, content);
}

ko c::handle_exec(string&& cmd0) {
    log("exec", cmd0, "local_api is", this);
    auto r = exec(cmd0);
    if (is_ko(r)) {
        push_KO(r);
    }
    return ok;
}

ko c::handle_get_subhome(string& subhome_) {
    log("get_subhome");
    ostringstream os;
    os << '/' << subhome;
    subhome_ = os.str();
    return ok;
}

ko c::handle_net_info(net_info_out_t& o_out) {
    log("net_info");
    /// out:
    ///    hash_t wallet_address;
    ///    string subhome;
    ///    vector<tuple<hash_t, uint32_t, uint16_t>> seeds;

    auto r = daemon.gov_rpc_daemon.get_peer().call_nodes(o_out.seeds);
    if (unlikely(is_ko(r))) {
        return r;
    }
    o_out.wallet_address = daemon.id.pub.hash();
    o_out.subhome = subhome;
    return ok;
}

ko c::handle_compilance_report(compilance_report_in_dst_t&& o_in, string& data) {
    log("compilance_report");
    /// in:
    ///     string jurisdiction;
    ///     string date_from;
    ///     string date_to;

    auto r = "KO 70699 Not implemented.";
    log(r);
    data = r;
    return r;
}

ko c::handle_priv_key(priv_t&& priv, priv_key_out_t& o_out) {
    log("priv_key");
    /// out:
    ///    pub_t pub;
    ///    hash_t addr;

    {
        auto r = gov::crypto::ec::keys::verifyx(priv);
        if (is_ko(r)) {
            return r;
        }
    }
    o_out.pub = gov::crypto::ec::keys::get_pubkey(priv);
    if (!o_out.pub.valid) {
        auto r = "KO 40049 Invalid secret key.";
        log(r);
        return r;
    }
    o_out.addr = o_out.pub.compute_hash();
    return ok;
}

ko c::handle_pub_key(pub_t&& pub, pub_key_out_t& o_out) {
    log("pub_key");
    /// out:
    ///    pub_t pub;
    ///    hash_t addr;

    if (!pub.valid) {
        auto r = "KO 40249 Invalid public key.";
        log(r);
        return r;
    }
    o_out.pub = pub;
    o_out.addr = pub.compute_hash();
    return ok;
}

ko c::handle_gen_keys(gen_keys_out_t& o_out) {
    log("gen_keys");
    /// out:
    ///    priv_t priv;
    ///    pub_t pub;
    ///    hash_t addr;

    auto k = gov::crypto::ec::keys::generate();
    o_out.priv = k.priv;
    o_out.pub = k.pub;
    o_out.addr = k.pub.compute_hash();
    return ok;
}

ko c::handle_sign(sign_in_dst_t&& o_in, sign_out_t& o_out) {
    log("sign");
    /// in:
    ///     vector<uint8_t> msg;
    ///     priv_t priv;

    /// out:
    ///    pub_t pub;
    ///    blob_t sig;

    gov::crypto::ec::keys k(o_in.priv);
    auto r = gov::crypto::ec::instance.sign(o_in.priv, o_in.msg, o_out.sig);
    if (is_ko(r)) {
        return r;
    }
    o_out.pub = k.pub;
    return ok;
}

ko c::handle_verify(verify_in_dst_t&& o_in, uint8_t& result) {
    log("verify");
    /// in:
    ///     vector<uint8_t> msg;
    ///     pub_t pub;
    ///     blob_t sig;

    result = gov::crypto::ec::instance.verify_not_normalized(o_in.pub, o_in.msg, o_in.sig) ? 1 : 0;
    return ok;
}

ko c::handle_encrypt(encrypt_in_dst_t&& o_in, vector<uint8_t>& msg_scrambled) {
    log("encrypt");
    /// in:
    ///     vector<uint8_t> msg;
    ///     priv_t src_priv;
    ///     pub_t rcpt_pub;

    gov::crypto::symmetric_encryption s_e;
    {
        auto r = s_e.init(o_in.src_priv, o_in.rcpt_pub);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = s_e.encrypt(o_in.msg, msg_scrambled, 0);
        if (is_ko(r)) {
            return r;
        }
    }
    return ok;
}

ko c::handle_decrypt(decrypt_in_dst_t&& o_in, vector<uint8_t>& msg) {
    log("decrypt");
    /// in:
    ///     vector<uint8_t> msg_scrambled;
    ///     pub_t src_pub;
    ///     priv_t rcpt_priv;

    gov::crypto::symmetric_encryption s_e;
    {
        auto r = s_e.init(o_in.rcpt_priv, o_in.src_pub);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = s_e.decrypt(o_in.msg_scrambled, msg);
        if (is_ko(r)) {
            return r;
        }
    }
    return ok;
}

ko c::handle_patch_os(patch_os_in_dst_t&& o_in, patch_os_out_t& o_out) {
    log("patch_os");
    /// in:
    ///     string script;
    ///     priv_t groot_key;
    ///     uint8_t relay;

    /// out:
    ///    blob_t blob_ev;
    ///    hash_t job;

    auto tx = w::tx_make_sys_patch_os(o_in.groot_key, o_in.script);
    if (is_ko(tx.first)) {
        return tx.first;
    }
    o_out.job = tx.second->hash_id();
    tx.second->write(o_out.blob_ev);
    delete tx.second;
    if (o_in.relay != 0) {
        blob_t ev = o_out.blob_ev;
        auto r = daemon.gov_rpc_daemon.get_peer().call_ev(move(ev));
        if (unlikely(is_ko(r))) {
            o_out.blob_ev.clear();
            return r;
        }
    }
    return ok;
}
/*
ko c::handle_track(track_in_dst_t&& o_in, track_status_t& track_status) {
    log("track");
    /// in:
    ///     uint64_t ts;
    ///     hash_t src;

    log("track");
    string progress;
    return daemon.gov_rpc_daemon.get_peer().call_track(o_in.ts, track_status);
}

ko c::handle_track_response(track_status_t&& track_status) {
    log("track_response");
    
    return ok; //Ignore
}

ko c::handle_track_pay(track_pay_in_dst_t&& o_in, track_pay_out_t& o_out) {
    log("track_pay");
    /// in:
    ///     blob_t blob_tx;
    ///     hash_t src;

    /// out:
    ///    blob_t blob_tx;
    ///    track_status_t track_status;

    tx_t tx;
    {
        auto r = tx.read(o_in.blob_tx);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = track_pay("algS0", "algR0", tx, o_in.src, o_out.track_status);
        if (is_ko(r.first)) {
            return r.first;
        }
    }
    tx.write(o_out.blob_tx);
    return ok;
}

ko c::handle_track_relay(track_relay_in_dst_t&& o_in, track_status_t& track_status) {
    log("track_relay");
    /// in:
    ///     blob_t blob_tx;
    ///     hash_t src;

    log("track_relay");
    {
        using evidence = us::gov::engine::evidence;
        pair<ko, evidence*> r = evidence::from_blob(o_in.blob_tx);
        if (is_ko(r.first)) {
            assert(r.first == nullptr);
            return r.first;
        }
        if (!r.second->check_amounts()) {
            delete r.second;
            auto e = "KO 84306 Input and output amounts must match.";
            log(e);
            return e;
        }
        delete r.second;
    }
    return daemon.gov_rpc_daemon.get_peer().call_ev_track(o_in.blob_tx, track_status);
}
*/
ko c::handle_list_protocols(string& data) {
    log("list_protocols");
    ostringstream os;
    businesses.api_list_protocols(os);
//    daemon.traders.api_list_protocols(os);
    data = os.str();
    log("list_protocols response:", data);
    return ok;
}

ko c::handle_trade(trade_in_dst_t&& o_in, hash_t& tid) {
    log("trade", "local_api is", this);
    /// in:
    ///     hash_t parent_trade;
    ///     string datasubdir;
    ///     blob_t qr;

    trader::qr_t qr;
    {
        auto r = qr.read(o_in.qr);
        if (is_ko(r)) {
            return r;
        }
    }
    auto r = businesses.initiate(o_in.parent_trade, o_in.datasubdir, move(qr));
    if (is_ko(r.first)) {
        return r.first;
    }
    tid = r.second;
    return ok;
}


ko c::handle_list_trades(string& data) {
    log("list_trades");
    ostringstream os;
    traders.list_trades(os);
    data = os.str();
    return ok;
}

ko c::handle_kill_trade(hash_t&& tid, string& data) {
    log("kill_trade");
    data = "killing trade in background...";
    return traders.exec(tid, "kill");
}

ko c::handle_exec_trade(exec_trade_in_dst_t&& o_in) {
    log("exec_trade", "local_api is", this);
    /// in:
    ///     hash_t tid;
    ///     string cmd;

    return traders.exec(o_in.tid, o_in.cmd);
    //lf.exec_help(prefix + "loc ", os); //--add_help_items
    //w.businesses.exec_help(prefix, os); //--api_list

}

ko c::handle_qr(bookmarks_t& bookmarks) {
    log("qr");
    published_bookmarks(bookmarks);
//    daemon.traders.published_bookmarks(*this, bookmarks);
    return ok;
}

ko c::handle_bookmark_add(bookmark_add_in_dst_t&& o_in, string& ans) {
    log("bookmark_add");
    /// in:
    ///     string name;
    ///     bookmark_t bookmark;

    ans = "OK";
    return bookmarks.add(o_in.name, move(o_in.bookmark));
}

ko c::handle_bookmark_delete(string&& key, string& ans) {
    log("bookmark_delete");
    auto r = bookmarks.remove(key);
    if (is_ko(r)) {
        ans = r;
        return r;
    }
    ans = "ok";
    return r;
}

ko c::handle_bookmark_list(bookmarks_t& ans_bookmarks) {
    log("bookmark_list");
    ostringstream os;
    os << daemon.home << "/trader/" << "brand_bookmarks";
    ans_bookmarks.load(os.str());
    {
        lock_guard<mutex> lock(bookmarks.mx);
        ans_bookmarks += bookmarks;
    }
    return ok;
}

ko c::handle_timeseries_list(string& ans) {
    log("timeseries_list");

    ostringstream os;
    os << "find " << home << "/accounts/ -type f -name \"params\" | sed \"s~.*" << home << "/\\(.*\\)/timeseries/params~\\1~\"";
    return us::gov::io::system_command(os.str(), ans);
}

ko c::handle_timeseries_show(timeseries_show_in_dst_t&& o_in, string& ans) {
    log("timeseries_show");
    /// in:
    ///     hash_t addr;
    ///     uint64_t ts;

    if (o_in.addr.is_zero()) {
        return "KO 84120 Invalid address.";
    }
    string tshome;
    string params_file;
    {
        ostringstream os;
        os << home << "/accounts/" << o_in.addr << "/timeseries";
        tshome = os.str();
        params_file = tshome + "/params";
    }
    if (!us::gov::io::cfg0::file_exists(params_file)) {
        auto r = "KO 40398 The account hasn't got timeseries.";
        log(r);
        return r;
    }
    if (o_in.ts == 0) {
        //list files
        ostringstream os;
        os << "cd " << tshome << "; find . -type f -maxdepth 1";
        return us::gov::io::system_command(os.str(), ans);
    }
    vector<uint8_t> v;
    {
        ostringstream os;
        os << tshome << '/' << o_in.ts;
        auto r = us::gov::io::read_file_(os.str(), v);
        if (is_ko(r)) {
            return r;
        }
    }
    ans = us::gov::crypto::b58::encode(v);
    return ok;
}

ko c::handle_timeseries_new(timeseries_new_in_dst_t&& o_in, string& ans) {
    log("timeseries_new");
    /// in:
    ///     hash_t addr;
    ///     uint8_t monotonic;
    if (o_in.addr.is_zero()) {
        return "KO 84120 Invalid address.";
    }
    string tshome;
    string params_file;
    {
        ostringstream os;
        os << home << "/accounts/" << o_in.addr << "/timeseries";
        tshome = os.str();
        us::gov::io::cfg0::ensure_dir(tshome);
        params_file = tshome + "/params";
    }
    if (us::gov::io::cfg0::file_exists(params_file)) {
        auto r = "KO 56984 timeseries already exist in the specified account.";
        log(r);
        return r;
    }
    {
        ofstream os(params_file, ios::trunc);
        os << (o_in.monotonic == 0 ? "free" : "monotonic") << '\n';
    }
    ostringstream os;
    os << "New " << (o_in.monotonic == 0 ? "free" : "monotonic") << " timeseries created in address " << o_in.addr << '\n';
    ans = os.str();
    return ok;
}

ko c::handle_timeseries_add(timeseries_add_in_dst_t&& o_in, string& ans) {
    log("timeseries_add");
    /// in:
    ///     hash_t addr;
    ///     vector<uint8_t> payload;

    if (o_in.addr.is_zero()) {
        return "KO 84120 Invalid address.";
    }
    string tshome;
    string params_file;
    {
        ostringstream os;
        os << home << "/accounts/" << o_in.addr << "/timeseries";
        tshome = os.str();
        us::gov::io::cfg0::ensure_dir(tshome);
        params_file = tshome + "/params";
    }
    if (!us::gov::io::cfg0::file_exists(params_file)) {
        auto r = "KO 56984 no timeseries found in the specified account.";
        log(r);
        return r;
    }
    using namespace std::chrono;
    uint64_t now = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
    ostringstream fname;
    fname << tshome << '/' << now;
    auto r = us::gov::io::write_file_(o_in.payload, fname.str());
    if (is_ko(r)) {
        return r;
    }
    ostringstream os;
    os << "Event registered as " << now << '.' << o_in.addr << '\n';
    ans = os.str();
    return ok;
}

ko c::handle_r2r_index_hdr(protocols_t& protocols) {
    log("r2r_index_hdr");
    return businesses.handle_r2r_index_hdr(protocols);
}

ko c::handle_r2r_bookmarks(protocol_selection_t&& protocol_selection, bookmarks_t& bookmarks) {
    log("r2r_bookmarks");
    return businesses.handle_r2r_bookmarks(move(protocol_selection), bookmarks);
}

ko c::handle_r2r_index(bookmark_index_t& bookmark_index) {
    log("r2r_index");
    return businesses.handle_r2r_index(bookmark_index);
}

//-/----------------apitool - End of API implementation.

//------------------apitool - API Spec defined @ us/api/generated/wallet/c++/wallet/hdlr_local-impl

///################################################################################################################################################################

#ifdef CFG_PERMISSIONED_NETWORK

ko c::handle_nodes_allow(nodes_allow_in_dst_t&& o_in, blob_t& blob_ev) {
    log("nodes_allow");
    /// in:
    ///     hash_t addr;
    ///     priv_t priv;
    ///     uint8_t relay;

    auto tx = algorithm::tx_make_nodes_acl(o_in.addr, true, o_in.priv);
    if (is_ko(tx.first)) {
        return tx.first;
    }
    tx.second->write(blob_ev);
    delete tx.second;
    if (o_in.relay != 0) {
        blob_t ev = blob_ev;
        auto r = daemon.gov_rpc_daemon.get_peer().call_ev(move(ev));
        if (unlikely(is_ko(r))) {
            blob_ev.clear();
            return r;
        }
    }
    return ok;
}

ko c::handle_nodes_deny(nodes_deny_in_dst_t&& o_in, blob_t& blob_ev) {
    log("nodes_deny");
    /// in:
    ///     hash_t addr;
    ///     priv_t priv;
    ///     uint8_t relay;

    auto tx = algorithm::tx_make_nodes_acl(o_in.addr, false, o_in.priv);
    if (is_ko(tx.first)) {
        return tx.first;
    }
    tx.second->write(blob_ev);
    delete tx.second;
    if (o_in.relay != 0) {
        blob_t ev = blob_ev;
        auto r = daemon.gov_rpc_daemon.get_peer().call_ev(move(ev));
        if (unlikely(is_ko(r))) {
            blob_ev.clear();
            return r;
        }
    }
    return ok;
}

#endif

pair<ko, c::affected_t> c::track_pay(const asa_t& asa_pay, const asa_t& asa_charge, tx_t& tx, const hash_t& tid, track_status_t& track_status) {
    log("track_pay");
    pair<ko, affected_t> ret;
    {
        auto r = refresh_data();
        if (unlikely(is_ko(r))) {
            ret.first = r;
            return ret;
        }
    }
    {
        auto r = tx_pay("algS0", "algR0", tx, ret.second);
        if (is_ko(r)) {
            ret.first = r;
            return move(ret);
        }
    }
    blob_t blob;
    tx.write(blob);
    ret.first = daemon.gov_rpc_daemon.get_peer().call_ev_track(blob, track_status);
    return move(ret);
}

pair<ko, c::affected_t> c::analyze_pay(const asa_t& asa_pay, const asa_t& asa_charge, const tx_t& tx0) {
    tx_t tx(tx0);
    string title;
    pair<ko, affected_t> ret;
    {
        auto r = refresh_data();
        if (unlikely(is_ko(r))) {
            ret.first = r;
            return ret;
        }
    }
    {
        auto r = tx_pay(asa_pay, asa_charge, tx, ret.second);
        if (is_ko(r)) {
            ret.first = r;
            return move(ret);
        }
    }
    ret.first = ok;
    return move(ret);
}

