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
#include "dispatcher_t.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <iomanip>

#include <us/gov/socket/datagram.h>
#include <us/gov/relay/protocol.h>
#include <us/gov/stacktrace.h>
#include <us/gov/log/thread_logger.h>
#include <us/gov/io/blob_reader_t.h>

#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/workflow/trader_protocol.h>

#define loglevel "trader/test"
#define logclass "dispatcher_t"
#include <us/gov/logs.inc>
#include "assert.inc"

//#define STEP_WAIT_MS 20000

using namespace std;
using us::gov::socket::datagram;
using c = us::test::dispatcher_t;

atomic<int> c::dgram_count = 0;
atomic<int> c::dgram_count_test = 0;

namespace {
    void fail(ostream& os) {
        assert(false);
    }

    void fail(bool b, ostream& os) {
        if (b) {
            ::fail(os);
        }
    }
}

c::dispatcher_t(const string& id, walletx_t& i, ostream& os): id(id), b(i), expected_code(id, os), out(os) {
}

string c::codename(uint16_t code) {
    switch (code) {
        case us::gov::relay::pushman::push_ko: return "push_ko";
        case us::gov::relay::pushman::push_ok: return "push_ok";
        using trader_t = us::wallet::trader::trader_t;
        case trader_t::push_log: return "push_log";
        case trader_t::push_data: return "push_data";
        case trader_t::push_peer_personality: return "push_peer_personality";
        case trader_t::push_chat: return "push_chat";
        case trader_t::push_roles_mine: return "push_roles_mine";
        case trader_t::push_roles_peer: return "push_roles_peer";
        case trader_t::push_roles: return "push_roles";
        case trader_t::push_trade: return "push_trade";
        case trader_t::push_killed: return "push_killed";
        case trader_t::push_help: return "push_help";
        case trader_t::push_local_functions: return "push_local_functions";
        case trader_t::push_remote_functions: return "push_remote_functions";
        case us::wallet::trader::workflow::trader_protocol::push_workflow_item: return "push_workflow_item";
        case us::wallet::trader::workflow::trader_protocol::push_doc: return "push_doc";
        case trader_t::protocol::push_logo: return "push_logo";
        case us::wallet::wallet::local_api::push_txlog: return "push_txlog";
    }
    ostringstream os;
    os << "unknown code " << code;
    return os.str();
}

void c::print(const string& id, const hash_t& tid, uint16_t code, const vector<uint8_t>& s, ostream& os) {
    os << id << "> push tid " << tid << " code " << code << " (" << codename(code) << ") " << s.size() << " bytes\n";
}

bool c::dispatch(us::gov::socket::datagram* d0) {
    ++dgram_count;
    log("dispatch", dgram_count.load());
    if (m.scr.mute || muted) {
        log("m.scr.mute || muted", m.scr.mute, muted);
        return b::dispatch(d0);
    }
    log("HMI", dgram_count.load());
    auto d = new datagram(*d0);
    b::dispatch(d0); //program outputs to scr
    ++dgram_count_test;
    log("test dispatcher", dgram_count_test.load(), dgram_count.load());

    static mutex scrmx;
    lock_guard<mutex> lock(scrmx);
    out << "\n== " << id << " =============================== test-dgram#" << dgram_count_test.load() << "; dgram#" << dgram_count.load() << " svc " << d->service << ' ' << endl;
    switch (d->service) {
        case us::gov::protocol::relay_push: {
            us::wallet::cli::rpc_peer_t::push_in_dst_t o_in;
            {
                us::gov::io::blob_reader_t reader(*d);
                auto r = reader.read(o_in);
                if (is_ko(r)) {
                    delete d;
                    out << "KO 66985 Parse" << endl;
                    fail(out);
                }
            }
            print(id, o_in.tid, o_in.code, o_in.blob, out);
            if (o_in.code == us::wallet::trader::trader_t::push_data) {
                string payload;
                assert(is_ok(us::gov::io::blob_reader_t::parse(o_in.blob, payload)));
                out << "===========DATA summary for node " << id << '\n';
                bool okadd = true;
                if (!data_seq.add(payload)) {
                    okadd = false;
                }
                data_seq.dump(out);
                if (!okadd) {
                    out << "WARNING: DUPLICATE DATA. data arrived at node " << id << " producing empty diff\n";
                    ++dupl_data_count;
                }
                out << "=========/=DATA summary" << endl;
                if (ignore_dupl_data) {
                    if (!okadd) {
                        out << "ignore_dupl_data: dropping it away from tests.\n";
                        break;
                    }
                }
            }
            expected_code.arrived(o_in.tid, o_in.code, o_in.blob);
        }
        break;
        default:
            if (expected_code.enabled) {
                out << "TEST - KO 66293 Expected " << expected_code.size() << " push Codes (svc " << us::gov::protocol::relay_push << "). Got instead svc " << d->service << '\n';
                fail(out);
            }
    }
    out << "=/ " << id << " =============================== test-dgram#" << dgram_count_test.load() << "; dgram#" << dgram_count.load() << endl;
    delete d;
    return true;
}

c::expected_code_t::expected_code_t(const string& id, ostream& os): id(id), out(os) {
    set_default_step_wait();
}

void c::expected_code_t::abort_tests() {
    cerr << "dispatcher::expected_code_t abort_tests" << endl;
    clear_all();
    cv.notify_all();
}

void c::expected_code_t::arrived(const hash_t& h, uint16_t code, const vector<uint8_t>& s) {
    if (!enabled) {
        log("not enabled");
        return;
    }
    check_payload(h, code, s, out);
    arrived(code);
}

void c::expected_code_t::add_exact_occurrences(uint16_t code, int numo) {
    unique_lock<mutex> lock(mx);
    emplace(code, numo);
}

void c::expected_code_t::add_minimum_occurrences(uint16_t code, int numo) {
    unique_lock<mutex> lock(mx);
    emplace(code, numo);
    marked_minimum.emplace(code);
}

void c::expected_code_t::decrement(uint16_t code) {
    unique_lock<mutex> lock(mx);
    auto i = find(code);
    assert(i != end());
    --i->second;
    assert(i->second >= 0);
}

void c::expected_code_t::increase(uint16_t code) {
    unique_lock<mutex> lock(mx);
    auto i = find(code);
    assert(i != end());
    ++i->second;
    assert(i->second >= 0);
}

void c::expected_code_t::increase_or_set_1_if_nonpos(uint16_t code) {
    unique_lock<mutex> lock(mx);
    auto i = find(code);
    assert(i != end());
    ++i->second;
    if (i->second < 1) i->second = 1;
}

bool c::expected_code_t::all_empty() const {
    unique_lock<mutex> lock(mx);
    auto b = empty() && check.empty() && marked_minimum.empty();
    return b;
}

void c::expected_code_t::clear_all() {
    unique_lock<mutex> lock(mx);
    clear_all_();
}

void c::expected_code_t::clear_all_() {
    clear();
    check.clear();
    marked_minimum.clear();
    zero_arrivals_is_good = false;
}

void c::expected_code_t::emplace(uint16_t a, int b) {
    assert(b > 0);
    auto i = find(a);
    if (i == end()) {
        b::emplace(a, b);
        return;
    }
    //assert(i->second == 0);
    i->second += b;
}

void c::expected_code_t::check_not_expecting_() const { //assume caller takes mx
    assert(enabled);
    if (num_expected_() == 0) {
        out << ":::: "; reftest(out);
        out << ":::: All expected codes arrived for " << id << ". PASS." << endl;
        return;
    }
    dump(out);
    if (!zero_arrivals_is_good) {
        out << "!::: ############# Expected more codes to arrive!!!" << endl;
        fail(out);
    }
    else {
        out << ":::: ############# Expected more codes to arrive, but they didn't and this condition is ok (zero_arrivals_is_good=true)." << endl;
    }
}

void c::expected_code_t::dump(ostream& os) const {
    print_stacktrace(os);
    string pfx = ":::: ";
    os << pfx; reftest(os);
    os << pfx << "  At node " << id << endl;
    os << pfx << "    " << size() << " codes" << endl;
    os << pfx << "    " << marked_minimum.size() << " codes marked 'minimum arrivals'" << endl;
    os << pfx << "    " << check.size() << " payload checkers." << endl;
    for (auto& m: *this) {
        bool marked = marked_minimum.find(m.first) != marked_minimum.end();
        bool checker = check.find(m.first) != check.end();
        os << pfx << "    Expecting code: " << m.first << "(" << dispatcher_t::codename(m.first) << ") " << m.second << " times. " << (marked ? " (minimum number of occurrences)" : " (exact number of occurrences)") << (checker ? " (with checker)" : " (without checker)") << "\n";
    }
}

int c::expected_code_t::num_expected_() const {
    int num_expected = 0;
    for (auto& xx: *this) {
        assert(xx.second >= 0);
        //if (xx.second > 0) num_expected += xx.second;
        num_expected += xx.second;
    }
    return num_expected;
}

void c::expected_code_t::arrived(uint16_t code) {
//    assert(num_expected_()>=0);
    log("arrived", code);
    unique_lock<mutex> lock(mx);
    bool mmin = marked_minimum.find(code) != marked_minimum.end();
    auto i = find(code);
    bool f = false;
    if (i == end()) {
        f = true;
    }
    else if (i->second == 0) {
        f = true;
    }
    else {
        --i->second;
    }
    if (f) {
        ostringstream os;
        dump(os);
        os << "!::: UNEXPECTED code arrived at node " << id << ". Code " << code << " (" << dispatcher_t::codename(code) << ")\n";
        log(os.str());
        out << os.str() << endl;
        fail(out);
    }
    if (num_expected_() == 0) {
        ostringstream os;
        reftest(os);
        os << ":::: All expected codes arrived at node " << id << endl;
        log(os.str());
        out << os.str() << "\n";
        cv.notify_all();
    }
}

void c::abort_tests() {
    expected_code.abort_tests();
}

using hash_t = us::gov::crypto::ripemd160::value_type;
bool save_seq = true;
map<hash_t, int> tidnum;

namespace {
#if CFG_LOGS == 0
    #undef log_dir
    string logdir() {
        ostringstream os;
        os << "logs/us-trader_" << getpid() << '/' << "test_r2r";
        return os.str();
    }
#else
    string logdir() {
        return us::dbg::thread_logger::instance.get_logdir();
    }
#endif
}

c::data_seq_t::data_seq_t() {
    reserve(5);
}

bool c::data_seq_t::add(const string& s) {
    data_t d;
    assert(is_ok(d.from(s)));
    if (!empty()) {
        auto diff = d.get_diff(*rbegin());
        diff.filter("data_generated");
        diff.filter("activity");
        diff.filter("online_age");
        if (diff.empty()) {
            return false;
        }
    }
    emplace_back(d);
    return true;
}

void c::data_seq_t::dump(ostream& os) const {
    int seq = 0;
    const data_t* prev = nullptr;
    for (auto& i: *this) {
        os << "    seq# " << seq << '\n';
        if (seq == 0) {
            data_t e;
            auto d = i.get_diff(e);
            d.dump("        ", os);
        }
        else {
            auto d = i.get_diff(*prev);
            d.dump("        ", os);
        }
        os << '\n';
        ++seq;
        prev = &i;
    }
}

void c::expected_code_t::check_payload(const hash_t& tid, uint16_t code, const vector<uint8_t>& s, ostream& os) {
    auto i = check.find(code);
    if (i == check.end()) return;
    reftest(os);
    os << '\n';
    if (save_seq) {
        int tidn;
        {
            auto i = tidnum.find(tid);
            if (i == tidnum.end()) {
                int n = tidnum.size();
                i = tidnum.emplace(tid, n).first;
            }
            tidn = i->second;
        }
        ostringstream fn;
        fn << logdir() << "/payloads_process_" << getpid();
        us::gov::io::cfg0::ensure_dir(fn.str());
        fn << "/" << std::setfill('0') << std::setw(4) << dgram_count_test << "_tid" << tidn << "_" << id << "_" << code << "_" << dispatcher_t::codename(code);
        ofstream ofs(fn.str());
        ofs.write((const char*)s.data(), s.size());
    }
    for (auto& x: i->second) {
        x(tid, code, s);
    }
}

void c::expected_code_t::wait_no_clear() {
    unique_lock<mutex> lock(mx);
    wait_no_clear_(lock);
}

void c::expected_code_t::wait_no_clear_(unique_lock<mutex>& lock) {
    if (!enabled) return;
    std::chrono::milliseconds s{step_wait_ms};
    if (num_expected_() == 0) return; //Nothing to wait for
    cv.wait_for(lock, s, [&](){ return num_expected_() == 0; } );
    check_not_expecting_();
}

void c::expected_code_t::wait() {
    unique_lock<mutex> lock(mx);
    wait_no_clear_(lock);
    clear_all_();
}

void c::expected_code_t::check_t::emplace(uint16_t a, function<void(const hash_t&, uint16_t code, const vector<uint8_t>&)> b) {
    auto i = find(a);
    if (i == end()) {
        b::emplace(a, vector{b});
        return;
    }
    i->second.emplace_back(b);
}

