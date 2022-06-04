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
#include "r2r_t.h"
#include "node.h"
#include "dispatcher_t.h"
#include <us/gov/stacktrace.h>
#include <iostream>

#define loglevel "test"
#define logclass "r2r_t"
#include <us/gov/logs.inc>
#include "assert.inc"

////////////////////////////////////
//////Enable wait between steps when debugging failures or extending the tests.
//////If disabled tests go fast but failures can be originated on previous steps to those reported
#define ENABLE_WAIT
#define WAIT_TIME 1s
////////////////////////////////////

using c = us::test::r2r_t;
using namespace std;

int c::wait_from_seq{0};

namespace {

    void gfail(const char* file, int line, ostream& os) {
        print_stacktrace(os);
        os << "At " << file << " line " << line << endl;
        abort();
    }

    void gfail(const char* file, int line, bool b, ostream& os) {
        if (b) {
            ::gfail(file, line, os);
        }
    }

}

int c::testseq{0};

c::r2r_t(network& n): n(n), out(n.out) {
}

void c::fail_(const char* file, int line) {
    ::gfail(file, line, out);
}

void c::fail_(bool b, const char* file, int line) {
    ::gfail(file, line, b, out);
}

void c::wait(node& n1, node& n2) {
    n1.wallet_cli_dis->expected_code.wait();
    n2.wallet_cli_dis->expected_code.wait();
    #ifdef ENABLE_WAIT
        if (testseq >= wait_from_seq) {
            out << "seq " << testseq << " #########################comment out ENABLE_WAIT when all tests pass###########################################\n";
            auto wt = WAIT_TIME;
            out << "wait " << wt.count() << 's' << endl;
            this_thread::sleep_for(WAIT_TIME);
            out << "seq " << testseq << " #/#######################comment out ENABLE_WAIT when all tests pass###########################################\n";
        }
    #endif
}

void c::wait(node& n1, node& n2, uint64_t timeout_ms) {
    n1.wallet_cli_dis->expected_code.step_wait_ms = timeout_ms;
    n2.wallet_cli_dis->expected_code.step_wait_ms = timeout_ms;
    wait(n1, n2);
    n2.wallet_cli_dis->expected_code.default_step_wait();
    n1.wallet_cli_dis->expected_code.default_step_wait();
}

void c::wait_no_clear(node& n1, node& n2) {
    n1.wallet_cli_dis->expected_code.wait_no_clear();
    n2.wallet_cli_dis->expected_code.wait_no_clear();
    #ifdef ENABLE_WAIT
        if (testseq >= wait_from_seq) {
            out << "seq " << testseq << " #########################comment out ENABLE_WAIT when all tests pass###########################################\n";
            auto wt = WAIT_TIME;
            out << "wait " << wt.count() << 's' << endl;
            this_thread::sleep_for(WAIT_TIME);
            out << "seq " << testseq << " #/#######################comment out ENABLE_WAIT when all tests pass###########################################\n";
        }
    #endif
}

void c::wait_no_clear1(node& n1, node& n2) {
    n1.wallet_cli_dis->expected_code.wait_no_clear();
    n2.wallet_cli_dis->expected_code.wait();
    #ifdef ENABLE_WAIT
        if (testseq >= wait_from_seq) {
            out << "seq " << testseq << " #########################comment out ENABLE_WAIT when all tests pass###########################################\n";
            auto wt = WAIT_TIME;
            out << "wait " << wt.count() << 's' << endl;
            this_thread::sleep_for(WAIT_TIME);
            out << "seq " << testseq << " #/#######################comment out ENABLE_WAIT when all tests pass###########################################\n";
        }
    #endif
}

void c::wait_no_clear2(node& n1, node& n2) {
    n1.wallet_cli_dis->expected_code.wait();
    n2.wallet_cli_dis->expected_code.wait_no_clear();
    #ifdef ENABLE_WAIT
        if (testseq >= wait_from_seq) {
            out << "seq " << testseq << " #########################comment out ENABLE_WAIT when all tests pass###########################################\n";
            auto wt = WAIT_TIME;
            out << "wait " << wt.count() << 's' << endl;
            this_thread::sleep_for(WAIT_TIME);
            out << "seq " << testseq << " #/#######################comment out ENABLE_WAIT when all tests pass###########################################\n";
        }
    #endif
}

void c::curtest_cont(node& n1, node& n2, const string& title, const char*file, int line) {
    out << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
    out << "\n= " << testseq << " === " << title << " ========================================================================\n";
    string fil = file;
    auto f = [=](ostream& os) {
        os << "ref: testseq " << testseq << " '" << title << "' @ " << file << ":" << line << '\n';
    };
    n1.wallet_cli_dis->expected_code.reftest = f;
    n2.wallet_cli_dis->expected_code.reftest = f;

    n1.wallet_cli_dis->data_seq.clear();
    n2.wallet_cli_dis->data_seq.clear();

    ++testseq;
}

void c::curtest(node& n1, node& n2, const string& title, const char*file, int line) {
    curtest_cont(n1, n2, title, file, line);
    Failb(!n1.wallet_cli_dis->expected_code.all_empty());
    Failb(!n2.wallet_cli_dis->expected_code.all_empty());
}

void c::test_r2r_cfg(node& n1, node& n2, function<void(node&, node&)> foo, hash_t& tid) {
    banner(n1, n2, out);
    this_thread::sleep_for(1s);

    auto o1 = n1.wallet_cli->interactive;
    auto o2 = n1.wallet_cli->p.names;
    auto o3 = n1.wallet_cli_dis->expected_code.enabled;
    auto o4 = n2.wallet_cli->interactive;
    auto o5 = n2.wallet_cli->p.names;
    auto o6 = n2.wallet_cli_dis->expected_code.enabled;
    assert(!o3);
    assert(!o6);

    n1.wallet_cli->interactive = false;
    n1.wallet_cli->p.names = true;
    n1.wallet_cli_dis->expected_code.enabled = true;
    n2.wallet_cli->interactive = false;
    n2.wallet_cli->p.names = true;
    n2.wallet_cli_dis->expected_code.enabled = true;

    assert(n1.wallet_cli_dis->expected_code.all_empty());
    assert(n2.wallet_cli_dis->expected_code.all_empty());

    foo(n1, n2);

    n1.wallet_cli->interactive = o1;
    n1.wallet_cli->p.names = o2;
    n1.wallet_cli_dis->expected_code.enabled = o3;
    n2.wallet_cli->interactive = o4;
    n2.wallet_cli->p.names = o5;
    n2.wallet_cli_dis->expected_code.enabled = o6;

    assert(n1.wallet_cli_dis->expected_code.all_empty());
    assert(n2.wallet_cli_dis->expected_code.all_empty());

    //reset cur trade
    assert(n1.wallet_cli->cur.is_not_zero());
    assert(n2.wallet_cli->cur.is_not_zero());
    assert(n1.wallet_cli->cur == tid);
    assert(n2.wallet_cli->cur == tid);
    exec_cur_trade(n1, "wallet");
    exec_cur_trade(n2, "wallet");
    assert(n1.wallet_cli->cur.is_zero());
    assert(n2.wallet_cli->cur.is_zero());

}

void c::banner(node& a, node& b, ostream& os) {
    a.banner(os);
    b.banner(os);
}

void c::check_entry_exists_(node& n, const string& key, const char* file, int line) {
    out << "checking if entry exists " << key << endl;
    lock_guard<mutex> lock(n.wallet_cli->data.mx);
    auto i=n.wallet_cli->data.find(key);
    if (i==n.wallet_cli->data.end()) {
        fail_(file, line);
    }
    out << "got " << i->second << endl;
    if (i->first != "personality_sk") {
        fail_(i->second.empty(), file, line);
    }
}

void c::check_data_(node& n, const string& key, const string& expected, const char* file, int line) {
    out << "checking key " << key << " at node " << n.id << ". expected " << expected << endl;
    lock_guard<mutex> lock(n.wallet_cli->data.mx);
    auto i=n.wallet_cli->data.find(key);
    if (i==n.wallet_cli->data.end()) {
        n.wallet_cli->data.dump_(n.id+"> ", out);
        out << "key not found. " << key << endl;
        fail_(file, line);
    }
    out << "got " << i->second << endl;
    fail_(i->second!=expected, file, line);
}

string c::get_data(node& n, const string& key) {
    lock_guard<mutex> lock(n.wallet_cli->data.mx);
    auto i=n.wallet_cli->data.find(key);
    if (i==n.wallet_cli->data.end()) {
        return "";
    }
    return i->second;
}

void c::check_data_null(node& n, const string& key) {
    out << "checking key " << key << " at node " << n.id << ". not expected." << endl;
    lock_guard<mutex> lock(n.wallet_cli->data.mx);
    auto i=n.wallet_cli->data.find(key);
    if (i==n.wallet_cli->data.end()) {
        return;
    }
    out << "got " << i->second << endl;
    Fail();
}

void c::exec_cur_trade(node& n, const string& command_line) {
    out << "exec_cur_trade " << command_line << endl;
    log("exec_curtrade", command_line);
    assert(n.wallet_cli->cur.is_not_zero());
    ostringstream cmd;
    cmd << "trade " << n.wallet_cli->cur << " " << command_line;
    out << cmd.str() << '\n';
    n.wallet_cli->exec(cmd.str());
}

void c::test_trade_start_dialog_a(node& bid, node& ask, hash_t& trade_id, int n1_num_protocols_seq0, int n1_num_protocols_seq1, int n2_num_protocols_seq0) {
    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_trade, 1);
    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 4);
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            static int seq = 0;
            cout << "seq " << seq << endl;

            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            Check_s_contains(payload, "created 16");
            Check_s_contains(payload, "activity 16");
            Check_s_contains(payload, "logfile ");
            Check_s_contains(payload, "test_r2r/home/node_");
            Check_s_contains(payload, "local_endpoint ");
            Check_s_contains(payload, "remote_endpoint ");
            Check_s_contains(payload, "remote_ip4 ");
            Check_s_contains(payload, "my_personality 11111111111111111111");
            Check_s_contains(payload, "my_moniker anonymous");
            Check_s_contains(payload, "peer_personality 11111111111111111111");
            Check_s_contains(payload, "peer_moniker anonymous");
            Check_s_contains(payload, "initiator Y");
            Check_s_contains(payload, "protocol not set");
            Check_s_contains(payload, "rf N");

            if (seq == 0) {
                if (payload.find("state online") != string::npos) {
                    seq++;
                    bid.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
                Check_s_contains(payload, "state connected");
                Check_s_contains(payload, "remote_endpoint 123 111111111");
                Check_s_contains(payload, "peer_moniker anonymous");
                Check_s_contains(payload, "peer_personality 11111111111111111111");
            }
            if (seq == 1) {
                if (payload.find("state online") != string::npos) {
                    seq++;
                    bid.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
            }
            if (seq == 2) {
                Check_s_contains(payload, "state online");
                ostringstream os;
                os << "remote_endpoint 123 " << ask.wallet_pkh;
                cout << os.str() << endl;
                if (payload.find(os.str()) != string::npos) {
                    seq++;
                    bid.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
                else {
                    Check_s_contains(payload, "remote_endpoint 123 1111");
                }
            }
            if (seq == 3) {
                Check_s_contains(payload, "state online");
                ostringstream os;
                os << "remote_endpoint 123 " << ask.wallet_pkh;
                cout << os.str() << endl;
                Check_s_contains(payload, os.str());
                seq = -1;
            }
            seq++;
        });
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_trade,
    [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
        cout << "new tid " << tid << endl;
        assert(tid.is_not_zero());
        trade_id = tid;
    });

    ask.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_trade, 1);
    ask.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 2);
    ask.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            static int seq = 0;
            cout << "seq " << seq << endl;
            cout << payload << endl;
            assert(tid == ask.wallet_cli->cur);
            assert(tid == trade_id);
            string peer_prots;
            {
                ostringstream os;
                os << "peer_protocols " << n2_num_protocols_seq0;
                peer_prots = os.str();
            }
            if (seq == 0) {
                Check_s_contains(payload, "created 16");
                Check_s_contains(payload, "activity 16");
                Check_s_contains(payload, "logfile ");
                Check_s_contains(payload, "test_r2r/home/node_");
                Check_s_contains(payload, "state online");
                Check_s_contains(payload, "online_age 00:00:0");
                Check_s_contains(payload, "local_endpoint ");
                Check_s_contains(payload, "initiator N");
                Check_s_contains(payload, "rf N");
                {
                    ostringstream os;
                    os << "remote_endpoint 123 " << bid.wallet_pkh;
                    cout << os.str() << endl;
                    Check_s_contains(payload, os.str());
                }
                Check_s_contains(payload, "remote_ip4 ");
                Check_s_contains(payload, "my_personality 11111111111111111111");
                Check_s_contains(payload, "my_moniker anonymous");
                Check_s_contains(payload, "peer_personality 11111111111111111111");
                Check_s_contains(payload, "peer_moniker anonymous");
                if (payload.find(string("peer_protocols ") + peer_prots) == string::npos) { //possible data coming before
                    ++seq;
                    ask.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
                else {
                    Check_s_contains(payload, "peer_protocols 0");
                }
            }
            if (seq == 1) {
                Check_s_contains(payload, peer_prots);
                Check_s_contains(payload, "protocol not set");
                seq = -1;
            }
            seq++;
        });
    log("trace S23ww");
    ostringstream cmd;
    cmd << "trade start 123 " << node::localip << ":" << ask.wport;
    cout << "exec " << cmd.str() << endl;
    bid.wallet_cli->exec(cmd.str());
    wait(bid, ask);
}

string c::to_string(const hash_t& v) {
    ostringstream os;
    os << v;
    return os.str();
}

void c::test_trade_start_dialog_b(node& bid, node& ask, hash_t& trade_id, int num_protocols0, int num_protocols1) {
    log("test_trade_start_dialog_b");
    bid.wallet_cli_dis->expected_code.emplace(us::gov::relay::pushman::push_ok, 1);
    bid.wallet_cli_dis->expected_code.check.emplace(us::gov::relay::pushman::push_ok,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            Check_s_contains(payload, "Waiting for peer...");
        }
    );

    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 3);
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            static int seq = 0;
            cout << "seq " << seq << endl;
            if (seq == 0) {
                if (payload.find("state online") != string::npos) { //possible data coming before
                    ++seq;
                    bid.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
                else {
                    Check_s_contains(payload, "state offline");
                }
            }
            if (seq == 1) {
                Check_s_contains(payload, "created 1");
                Check_s_contains(payload, "activity 1");
                Check_s_contains(payload, "/123/wallet/trader/log/trade_");
                Check_s_contains(payload, "state online");
                Check_s_contains(payload, "online_age 00:00:");
                Check_s_contains(payload, "local_endpoint ");
                Check_s_contains(payload, "remote_endpoint ");
                Check_s_contains(payload, "remote_ip4 ");
                Check_s_contains(payload, "my_personality 11111111111111111111");
                Check_s_contains(payload, "my_moniker anonymous");
                Check_s_contains(payload, "peer_protocols 2");
                Check_s_contains(payload, "initiator Y");
                Check_s_contains(payload, "protocol bid2ask bid");
                Check_s_contains(payload, "logo N");
                Check_s_contains(payload, "local__doctypes_consumer 288 290 292 ");
                Check_s_contains(payload, "local__doctypes_producer 291 ");
                Check_s_contains(payload, "local__wf_cat N");
                Check_s_contains(payload, "local__wf_inv N");
                Check_s_contains(payload, "local__wf_pay N");
                Check_s_contains(payload, "local__wf_rcpt N");
                Check_s_contains(payload, "chat_script 0");
                Check_s_contains(payload, "homemode me/peer");
                Check_s_contains(payload, "personality_sk ");
                Check_s_contains(payload, "basket ");
                Check_s_contains(payload, "chat_script 0");
                Check_s_contains(payload, "rf N");
                Check_s_contains(payload, "peer_personality 11111111111111111111");
                Check_s_contains(payload, "peer_moniker anonymous");
                Check_s_contains(payload, "pphome");
                Check_s_contains(payload, "/123/wallet/trader/bid2ask/bid/11111111111111111111/11111111111111111111");
                Check_s_contains(payload, "datadir ");
                Check_s_contains(payload, string("/123/wallet/trader/trade_data/") + to_string(trade_id));
                if (payload.find("remote__doctypes_consumer ") != string::npos) { //possible data coming before
                    ++seq;
                    bid.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
                else {
                    Check_s_not_contains(payload, "remote__doctypes_consumer ");
                    Check_s_not_contains(payload, "remote__doctypes_producer ");
                    Check_s_not_contains(payload, "remote__wf_cat ");
                    Check_s_not_contains(payload, "remote__wf_inv ");
                    Check_s_not_contains(payload, "remote__wf_pay ");
                    Check_s_not_contains(payload, "remote__wf_rcpt ");
                }
            }
            if (seq == 2) {
                Check_s_contains(payload, "peer_personality zzqCyAiEUz9mrw6EX1Rdehmovpv");
                Check_s_contains(payload, "peer_moniker Impact Shopping");
                Check_s_contains(payload, "pphome");
                Check_s_contains(payload, "/123/wallet/trader/bid2ask/bid/11111111111111111111/zzqCyAiEUz9mrw6EX1Rdehmovpv");
                Check_s_contains(payload, "datadir ");
                Check_s_contains(payload, string("/123/wallet/trader/trade_data/") + to_string(trade_id));
                Check_s_contains(payload, "remote__doctypes_consumer 291 ");
                Check_s_contains(payload, "remote__doctypes_producer 288 290 292 ");
                Check_s_contains(payload, "remote__wf_cat Y");
                Check_s_contains(payload, "remote__wf_cat_expiry ");
                Check_s_contains(payload, "remote__wf_cat_ts 1");
                Check_s_contains(payload, "remote__wf_inv N");
                Check_s_contains(payload, "remote__wf_pay N");
                Check_s_contains(payload, "remote__wf_rcpt N");
                seq = -1;
            }
            ++seq;
        }
    );

    ask.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 2);
    ask.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            static int seq = 0;
            cout << "seq " << seq << endl;
            if (seq == 0) {
                if (payload.find("state online") != string::npos) {
                    ++seq;
                    ask.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
                else {
                    Check_s_contains(payload, "state offline");
                }
            }
            if (seq == 1) {
                Check_s_contains(payload, "created 1");
                Check_s_contains(payload, "activity 1");
                Check_s_contains(payload, "state online");
                Check_s_contains(payload, "online_age 00:00:");
                Check_s_contains(payload, "local_endpoint ");
                Check_s_contains(payload, "remote_endpoint ");
                Check_s_contains(payload, "remote_ip4 ");
                Check_s_contains(payload, "my_personality zzqCyAiEUz9mrw6EX1Rdehmovpv");
                Check_s_contains(payload, "my_moniker Impact Shopping");
                Check_s_contains(payload, "peer_personality 11111111111111111111");
                Check_s_contains(payload, "peer_moniker anonymous");
                Check_s_contains(payload, "peer_protocols ");
                Check_s_contains(payload, "initiator N");
                Check_s_contains(payload, "protocol bid2ask ask");
                Check_s_contains(payload, "pphome");
                Check_s_contains(payload, "test_r2r/home/node_ask/123/wallet/trader/bid2ask/ask/zzqCyAiEUz9mrw6EX1Rdehmovpv/11111111111111111111");
                Check_s_contains(payload, "datadir ");
                Check_s_contains(payload, string("test_r2r/home/node_ask/123/wallet/trader/trade_data/") + to_string(trade_id));
                Check_s_contains(payload, "logo N");
                Check_s_contains(payload, "local__doctypes_consumer 291 ");
                Check_s_contains(payload, "local__doctypes_producer 288 290 292 ");
                Check_s_contains(payload, "local__wf_cat Y");
                Check_s_contains(payload, "local__wf_cat_expiry 1");
                Check_s_contains(payload, "local__wf_cat_ts 1");
                Check_s_contains(payload, "local__wf_inv N");
                Check_s_contains(payload, "local__wf_pay N");
                Check_s_contains(payload, "local__wf_rcpt N");
                Check_s_contains(payload, "chat_script 0");
                Check_s_contains(payload, "homemode me/peer");
                Check_s_contains(payload, "personality_sk ********");
                Check_s_contains(payload, "personality_moniker Impact Shopping");
                Check_s_contains(payload, "basket ");
                Check_s_contains(payload, "rf N");

                Check_s_contains(payload, "local__basket_serial 0");
                Check_s_contains(payload, "local__doctypes_consumer 291 ");
                Check_s_contains(payload, "local__doctypes_producer 288 290 292 ");
                Check_s_contains(payload, "local__wf_cat Y");
                Check_s_contains(payload, "local__wf_cat_expiry 1");
                Check_s_contains(payload, "local__wf_cat_mode send");
                Check_s_contains(payload, "local__wf_cat_ts 1");
                Check_s_contains(payload, "local__wf_inv N");
                Check_s_contains(payload, "local__wf_inv_mode send");
                Check_s_contains(payload, "local__wf_pay N");
                Check_s_contains(payload, "local__wf_pay_mode recv");
                Check_s_contains(payload, "local__wf_rcpt N");
                Check_s_contains(payload, "local__wf_rcpt_mode send");

                Check_s_contains(payload, "remote__basket_serial 0");
                Check_s_contains(payload, "remote__doctypes_consumer 288 290 292 ");
                Check_s_contains(payload, "remote__doctypes_producer 291 ");
                Check_s_contains(payload, "remote__wf_cat N");
                Check_s_contains(payload, "remote__wf_cat_mode recv");
                Check_s_contains(payload, "remote__wf_inv N");
                Check_s_contains(payload, "remote__wf_inv_mode recv");
                Check_s_contains(payload, "remote__wf_pay N");
                Check_s_contains(payload, "remote__wf_pay_mode send");
                Check_s_contains(payload, "remote__wf_rcpt N");
                Check_s_contains(payload, "remote__wf_rcpt_mode recv");
                Check_s_not_contains(payload, "remote__wf_cat_expiry");
                Check_s_not_contains(payload, "remote__wf_cat_ts");
                seq = -1;
            }
            ++seq;
        }
    );

    ostringstream cmd;
    assert(!bid.wallet_cli->cur.is_zero());
    cout << "tid: " << bid.wallet_cli->cur << endl;
    cmd << "start bid2ask ask";
    cout << cmd.str() << endl;
    exec_cur_trade(bid, cmd.str());

    wait(bid, ask);

    Check_data(bid, "protocol", "bid2ask bid");
    Check_data(ask, "protocol", "bid2ask ask");

    {
        auto s1=get_data(bid, "local_endpoint");
        auto s2=get_data(ask, "remote_endpoint");
        cout << "bid local_ep " << s1 << "; ask remote_ep " << s2 << endl;
        assert(s1 == s2);
    }
    {
        auto s1=get_data(bid, "remote_endpoint");
        auto s2=get_data(ask, "local_endpoint");
        cout << "bid remote_ep " << s1 << "; ask local_ep " << s2 << endl;
        assert(s1 == s2);
    }
}

