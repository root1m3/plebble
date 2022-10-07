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
#include "bid2ask_t.h"

#include <us/test/node.h>
#include <us/test/dispatcher_t.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/data_t.h>
#include <us/wallet/trader/workflow/trader_protocol.h>

#include <us/trader/workflow/consumer/docs.h>
#include <us/trader/workflow/consumer/workflow_t.h>
#include <us/trader/workflow/consumer/basket.h>

#include "node_ask.h"
#include "network.h"

#define loglevel "test"
#define logclass "bid2ask_t"
#include <us/gov/logs.inc>
#include <us/test/assert.inc>

using c = us::test::bid2ask_t;

void c::test_0(node& bid, node& ask) {
    curtest(bid, ask, "trade start", __FILE__, __LINE__);
//    test_trade_start_dialog_a(bid, ask, trade_id, 0, 2, 5);
    test_trade_start_dialog_a(bid, ask, trade_id, 0, 2, 2);
}

void c::test_0_0(node& bid, node& ask) {
    curtest(bid, ask, "connect when online", __FILE__, __LINE__);
    bid.wallet_cli_dis->expected_code.emplace(us::gov::relay::pushman::push_ok, 1);
    bid.wallet_cli_dis->expected_code.check.emplace(us::gov::relay::pushman::push_ok,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            static int seq = 0;
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            Check_s_contains(payload, "Connecting...");
        }
    );

/*
    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 1);
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            int seq = 0;
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            Check_s_contains(payload, "state online");
        });
*/

    ask.wallet_cli_dis->expected_code.emplace(us::gov::relay::pushman::push_ok, 1);
    ask.wallet_cli_dis->expected_code.check.emplace(us::gov::relay::pushman::push_ok,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            static int seq = 0;
            assert(tid == ask.wallet_cli->cur);
            assert(tid == trade_id);
            Check_s_contains(payload, "Connecting...");
        }
    );

    exec_cur_trade(bid, "connect");
    exec_cur_trade(ask, "connect");
    wait(bid, ask);
}

void c::test_0_ping(node& n1, node& n2) {
    ostringstream testname;
    testname << n1.id << " pings, " << n2.id << " pongs.";
    ostringstream os2;
    curtest(n1, n2, testname.str(), __FILE__, __LINE__);
    int seq = 0;
    n1.wallet_cli_dis->expected_code.emplace(us::gov::relay::pushman::push_ok, 2);
    n1.wallet_cli_dis->expected_code.check.emplace(us::gov::relay::pushman::push_ok,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            cout << "seq " << seq << endl;
            assert(tid == n1.wallet_cli->cur);
            assert(tid == trade_id);
            if (seq == 0) {
                Check_s_contains(payload, "Sent ping.");
            }
            if (seq == 1) {
                Check_s_contains(payload, "Received Pong!. Roundtrip took ");
            }
            ++seq;
        }
    );
    exec_cur_trade(n1, "ping");
    wait(n1, n2);
}

void c::test_0_1(node& bid, node& ask) {
    curtest(bid, ask, "disconnect", __FILE__, __LINE__);
    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 1);
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            int seq = -1;
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            if (payload.find("state req_offline")!=string::npos) {
                cout << "Received seq 0" << endl;
                seq = 0;
            }
            if (payload.find("state gone_offline")!=string::npos) {
                cout << "Received seq 1" << endl;
                seq = 1;
            }
            if (payload.find("state offline")!=string::npos) {
                cout << "Received seq 2" << endl;
                seq = 2;
            }
            cout << "seq " << seq << endl;
            if (seq == 0) {
                Check_s_contains(payload, "created 16");
                Check_s_contains(payload, "activity 16");
                Check_s_contains(payload, "logfile ");
                Check_s_contains(payload, "test_l2/home/node_bid/123/wallet/trader/log/trade_16");
                Check_s_contains(payload, "state req_offline");
                Check_s_not_contains(payload, "online_age ");
                Check_s_contains(payload, "local_endpoint ");
                Check_s_contains(payload, "remote_endpoint ");
                Check_s_contains(payload, "remote_ip4 ");
                Check_s_contains(payload, "my_personality anonymous");
                Check_s_contains(payload, "peer_personality anonymous");
                Check_s_contains(payload, "peer_protocols 2");
                Check_s_contains(payload, "initiator Y");
                Check_s_contains(payload, "protocol not set");
                Check_s_contains(payload, "rf N");
            }
            else if (seq == 1) {
                Check_s_contains(payload, "state gone_offline");
            }
            else if (seq == 2) {
                Check_s_contains(payload, "state offline");
                Check_s_contains(payload, "initiator Y");
                Check_s_not_contains(payload, "remote_ip4 0.0.0.0:0");
            }
            else {
                assert(false);
            }
        });

    ask.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 2);
    ask.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            int seq = 0;
            assert(tid == ask.wallet_cli->cur);
            assert(tid == trade_id);
            cout << "payload " << payload << endl;
            cout << "seq " << seq << endl;
            if (seq == 0) {
                Check_s_contains(payload, "state gone_offline");
                Check_s_contains(payload, "created 16");
                Check_s_contains(payload, "activity 16");
                Check_s_contains(payload, "logfile ");
                Check_s_contains(payload, "test_l2/home/node_ask/123/wallet/trader/log/trade_16");
                Check_s_contains(payload, "local_endpoint ");
                Check_s_contains(payload, "remote_endpoint ");
                Check_s_contains(payload, "remote_ip4 ");
                Check_s_contains(payload, "my_personality anonymous");
                Check_s_contains(payload, "peer_personality anonymous");
                Check_s_contains(payload, "peer_protocols 2");
                Check_s_contains(payload, "initiator N");
                Check_s_contains(payload, "protocol not set");
                Check_s_contains(payload, "rf N");
            }
            else if (seq == 1) {
                Check_s_contains(payload, "state offline");
                Check_s_contains(payload, "initiator N");
                Check_s_contains(payload, "remote_ip4 0.0.0.0:0");
                Check_s_contains(payload, "created 16");
                Check_s_contains(payload, "activity 16");
                Check_s_contains(payload, "logfile ");
                Check_s_contains(payload, "test_l2/home/node_ask/123/wallet/trader/log/trade_16");
                Check_s_contains(payload, "local_endpoint ");
                Check_s_contains(payload, "remote_endpoint ");
                Check_s_contains(payload, "my_personality anonymous");
                Check_s_contains(payload, "peer_personality anonymous");
                Check_s_contains(payload, "peer_protocols 2");
                Check_s_contains(payload, "protocol not set");
                Check_s_contains(payload, "rf N");
            }
            else {
                assert(false);
            }
            seq++;
        });
    exec_cur_trade(bid, "disconnect");
    wait(bid, ask);
    cout << "checking current trade id " << bid.wallet_cli->cur << endl;
}

void c::test_0_1_1(node& bid, node& ask) {
    curtest(bid, ask, "disconnect when offline", __FILE__, __LINE__);
    bid.wallet_cli_dis->expected_code.emplace(us::gov::relay::pushman::push_ok, 1);
    bid.wallet_cli_dis->expected_code.check.emplace(us::gov::relay::pushman::push_ok,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            Check_s_contains(payload, "Already offline.");
        }
    );
    ask.wallet_cli_dis->expected_code.emplace(us::gov::relay::pushman::push_ok, 1);
    ask.wallet_cli_dis->expected_code.check.emplace(us::gov::relay::pushman::push_ok,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == ask.wallet_cli->cur);
            assert(tid == trade_id);
            Check_s_contains(payload, "Already offline.");
        }
    );
    exec_cur_trade(bid, "disconnect");
    exec_cur_trade(ask, "disconnect");
    wait(bid, ask);
}

void c::test_0_1_2(node& bid, node& ask) {
    curtest(bid, ask, "quick re-connect from bid", __FILE__, __LINE__);
    cout << "=============TODO test without this delay==========" << endl;
    this_thread::sleep_for(1s);
    bool bug_quick_reconnect_expressed{false};
    bid.wallet_cli_dis->expected_code.emplace(us::gov::relay::pushman::push_ok, 1);
    bid.wallet_cli_dis->expected_code.check.emplace(us::gov::relay::pushman::push_ok,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            Check_s_contains(payload, "Connected!.");
        }
    );
    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 2);
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            int seq = 0;
            Check_s_contains(payload, "remote_ip4 ");
            Check_s_not_contains(payload, "ip4_resolver ");

            if (payload.find("state req_online")!=string::npos) {
                seq = 0;
            }
            else if (payload.find("state online")!=string::npos) {
                seq = 1;
            }
            else if (payload.find("state offline")!=string::npos) {
                seq = 2;
            }
            cout << "seq " << seq << endl;
            if (seq == 0) {
                Check_s_contains(payload, "state req_online");
            }
            else if (seq == 1) {
                Check_s_contains(payload, "state online");
            }
            else if (seq == 2) {
                Check_s_contains(payload, "state offline");
                bug_quick_reconnect_expressed = true;
                cout << "> Bug QUICK_RECONNECT did express on bid." << endl;
            }
            else {
                assert(false);
            }
        });
    ask.wallet_cli_dis->expected_code.add_minimum_occurrences(us::wallet::trader::trader_t::push_data, 1);
    ask.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == ask.wallet_cli->cur);
            assert(tid == trade_id);
            int seq = 0;
            Check_s_contains(payload, "remote_ip4 ");
            Check_s_not_contains(payload, "ip4_resolver ");

            if (payload.find("state offline")!=string::npos) {
                seq = 0;
            }
            else if (payload.find("state online")!=string::npos) {
                seq = 1;
            }

            // <== here is the thing   . connecting so quick after a shutdown produces this call to disconnect:
            // QUICK_RECONNECT gov/server/daemon.cpp
            // /   Goes offline misteriously if reconecting  quickly

            cout << "seq " << seq << endl;
            if (seq == 0) {
                Check_s_contains(payload, "state offline"); //We expec All bad. Unsuccesful reconnect. (too fast?)
                bug_quick_reconnect_expressed = true;
                cout << "> Bug QUICK_RECONNECT did express on ask." << endl;
            }
            else if (seq == 1) {
                Check_s_contains(payload, "state online");
                bug_quick_reconnect_expressed = false;
                cout << "> Bug QUICK_RECONNECT didn't express on ask." << endl;
                ask.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
            }
            else {
                assert(false);
            }
        });

    exec_cur_trade(bid, "connect");
    wait_no_clear(bid, ask);

    if (bug_quick_reconnect_expressed) {
        cout << "add a bit of delay while bid and ask nodes are still capturing" << endl;
        int b = 0;
        while(true) {
            cout << ++b << "s. state bid " << get_data(bid, "state") << " ask " << get_data(ask, "state") << endl;
            bid.wallet_cli_dis->expected_code.increase(us::wallet::trader::trader_t::push_data);
            ask.wallet_cli_dis->expected_code.increase_or_set_1_if_nonpos(us::wallet::trader::trader_t::push_data);
            exec_cur_trade(bid, "show data");
            exec_cur_trade(ask, "show data");
            wait_no_clear(bid, ask);
        }
        bid.wallet_cli_dis->expected_code.clear_all();
        ask.wallet_cli_dis->expected_code.clear_all();
        Check_data(bid, "state", "offline");
        Check_data(ask, "state", "offline");
    }
    else {
        Check_data(bid, "state", "online");
        Check_data(ask, "state", "online");
    }
    Fail();
}

void c::test_1(node& bid, node& ask) {
    curtest(bid, ask, "start bid2ask bid", __FILE__, __LINE__);
    test_trade_start_dialog_b(bid, ask, trade_id, 0, 2);
}

void c::test_3(node& bid, node& ask) {
    curtest(bid, ask, "bid.wallet_cli->api().list_trades()", __FILE__, __LINE__);

    string ans;
    auto r = bid.wallet_cli->rpc_daemon->get_peer().call_list_trades(ans);
    if (r != ok) {
        cout << r << endl;
        assert(false);
    }
    assert(!ans.empty());
    cout << ans << endl;
    wait(bid, ask);
}

void c::test_4(node& bid, node& ask) {
    curtest(bid, ask, "show data", __FILE__, __LINE__);

    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 1); //103
    exec_cur_trade(bid, "show data");

    wait(bid, ask);

    Check_data(bid, "logo", "N");
}

void c::test_5(node& bid, node& ask) {
    curtest(bid, ask, "ask.wallet_cli->api().list_trades()", __FILE__, __LINE__);

    string ans;
    auto r = ask.wallet_cli->rpc_daemon->get_peer().call_list_trades(ans); //exec("trade list");
    assert(r == ok);
    cout << "list_trades:\n" << ans << endl;
    string stid;
    {
        istringstream is(ans);
        is >> stid;
    }
    us::gov::crypto::ripemd160::value_type tid;
    {
        istringstream is(stid);
        is >> tid;
    }
    cout << "ask tid:\n" << tid << endl;
    cout << "bid tid:\n" << bid.wallet_cli->cur << endl;
    assert(tid == bid.wallet_cli->cur);
}

void c::test_6(node& bid, node& ask) {
    curtest(bid, ask, "trade show data", __FILE__, __LINE__);

    ask.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 1); //103
    us::gov::crypto::ripemd160::value_type& tid=bid.wallet_cli->cur;
    ostringstream cmd;
    cmd << "trade " << tid << " show data";
    assert(ask.wallet_cli->exec(cmd.str()) == ok);

    wait(bid, ask);

    Check_data(ask, "protocol", "bid2ask ask");
}

void c::test_7(node& bid, node& ask) {
    curtest(bid, ask, "request logo", __FILE__, __LINE__);

    bid.wallet_cli_dis->expected_code.emplace(us::gov::relay::pushman::push_ok, 1);
    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 1);
    cout << "\nrequest logo" << endl;
    exec_cur_trade(bid, "request logo");

    wait(bid, ask);

    cout << "check data" << endl;
    Check_data(bid, "logo", "Y");
}

void c::test_8(node& bid, node& ask) {
    curtest(bid, ask, "show logo", __FILE__, __LINE__);

    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_protocol::push_logo, 1);
    cout << "\nshow logo" << endl;
    exec_cur_trade(bid, "show logo");

    wait(bid, ask);

    cout << "check data" << endl;
    Check_data(bid, "logo", "Y");
}

void c::test_9(node& bid, node& ask) {
    curtest(bid, ask, "show data", __FILE__, __LINE__);

    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 1);
    ask.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 1);
    exec_cur_trade(bid, "show data");
    exec_cur_trade(ask, "show data");

    wait(bid, ask);

    Check_data(bid, "local__wf_cat", "N");
    Check_data(ask, "local__wf_cat", "Y");
}

void c::test_10(node& bid, node& ask) {
    curtest(bid, ask, "request cat", __FILE__, __LINE__);

    bid.wallet_cli_dis->expected_code.emplace(us::gov::relay::pushman::push_ok, 1);
    bid.wallet_cli_dis->expected_code.check.emplace(us::gov::relay::pushman::push_ok,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            Check_s_contains(payload, "Document requested. I expect it to be delivered...");
        }
    );

    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 1);
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            static int seq = 0;
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            Check_s_contains(payload, "created 1");
            Check_s_contains(payload, "activity 1");
            Check_s_contains(payload, "test_l2/home/node_bid/123/wallet/trader/log/trade_");
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
            Check_s_contains(payload, "logo Y");
            Check_s_contains(payload, "local__doctypes_consumer 288 290 292 ");
            Check_s_contains(payload, "local__doctypes_producer 291 ");
            Check_s_contains(payload, "local__wf_cat Y");
            Check_s_contains(payload, "local__wf_cat_expiry 1");
            Check_s_contains(payload, "local__wf_cat_ts 1");
            Check_s_contains(payload, "local__wf_inv N");
            Check_s_contains(payload, "local__wf_pay N");
            Check_s_contains(payload, "local__wf_rcpt N");
            Check_s_contains(payload, "chat_script 0");
            Check_s_contains(payload, "homemode me/peer");
            Check_s_contains(payload, "personality_sk ");
            Check_s_contains(payload, "remote__doctypes_consumer 291 ");
            Check_s_contains(payload, "remote__doctypes_producer 288 290 292 ");
            Check_s_contains(payload, "remote__wf_cat Y");
            Check_s_contains(payload, "remote__wf_cat_expiry 1");
            Check_s_contains(payload, "remote__wf_cat_ts 1");
            Check_s_contains(payload, "remote__wf_inv N");
            Check_s_contains(payload, "remote__wf_pay N");
            Check_s_contains(payload, "remote__wf_rcpt N");
            Check_s_contains(payload, "basket ");
            Check_s_contains(payload, "rf N");
            if (seq == 0) {
                Check_s_contains(payload, "peer_personality zzqCyAiEUz9mrw6EX1Rdehmovpv");
                Check_s_contains(payload, "peer_moniker Impact Shopping");
            }
            else if (seq == 1) {
                Fail();
                Check_s_contains(payload, "remote__doctypes_consumer 288 290 292 ");
                Check_s_contains(payload, "remote__doctypes_producer 291 ");
                Check_s_contains(payload, "remote__wf_cat Y");
                Check_s_contains(payload, "remote__wf_cat_expiry ");
                Check_s_contains(payload, "remote__wf_cat_ts 1");
                Check_s_contains(payload, "remote__wf_inv N");
                Check_s_contains(payload, "remote__wf_pay N");
                Check_s_contains(payload, "remote__wf_rcpt N");
            }
            else {
                Fail();
            }
            ++seq;
        }
    );

    ask.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 1);
    ask.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            static int seq = 0;
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            Check_s_contains(payload, "created 1");
            Check_s_contains(payload, "activity 1");
            Check_s_contains(payload, "bootstrapped_by ");
            Check_s_contains(payload, "logfile ");
            Check_s_contains(payload, "test_l2/home/node_ask/123/wallet/trader/log/trade_");
            Check_s_contains(payload, "state online");
            Check_s_contains(payload, "online_age 00:00:");
            Check_s_contains(payload, "remote_endpoint ");
            Check_s_contains(payload, "remote_ip4 ");
            Check_s_contains(payload, "local_endpoint ");
            Check_s_contains(payload, "my_personality zzqCyAiEUz9mrw6EX1Rdehmovpv");
            Check_s_contains(payload, "my_moniker Impact Shopping");
            Check_s_contains(payload, "peer_personality 11111111111111111111");
            Check_s_contains(payload, "peer_moniker anonymous");
            Check_s_contains(payload, "peer_protocols ");
            Check_s_contains(payload, "initiator N");
            Check_s_contains(payload, "protocol bid2ask ask");
            Check_s_contains(payload, "pphome ");
            Check_s_contains(payload, "test_l2/home/node_ask/123/wallet/trader/bid2ask/ask/zzqCyAiEUz9mrw6EX1Rdehmovpv/11111111111111111111");
            Check_s_contains(payload, "datadir ");
            Check_s_contains(payload, string("test_l2/home/node_ask/123/wallet/trader/trade_data/") + to_string(trade_id));
            Check_s_contains(payload, "logo N");
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
            Check_s_contains(payload, "chat_script 0");
            Check_s_contains(payload, "homemode me/peer");
            Check_s_contains(payload, "personality_moniker Impact Shopping");
            Check_s_contains(payload, "personality_sk ********");
            Check_s_contains(payload, "remote__doctypes_consumer 288 290 292 ");
            Check_s_contains(payload, "remote__doctypes_producer 291 ");
            Check_s_contains(payload, "remote__wf_inv N");
            Check_s_contains(payload, "remote__wf_inv_mode recv");
            Check_s_contains(payload, "remote__wf_pay N");
            Check_s_contains(payload, "remote__wf_pay_mode send");
            Check_s_contains(payload, "remote__wf_rcpt N");
            Check_s_contains(payload, "remote__wf_rcpt_mode recv");
            Check_s_contains(payload, "basket ");
            Check_s_contains(payload, "rf N");
            Check_s_contains(payload, "remote__wf_cat Y");
            Check_s_contains(payload, "remote__wf_cat_expiry 1");
            Check_s_contains(payload, "remote__wf_cat_mode recv");
            Check_s_contains(payload, "remote__wf_cat_ts 1");
        }
    );

    cout << "\nrequest cat" << endl;
    exec_cur_trade(bid, "request cat");

    wait(bid, ask);

    cout << "check data" << endl;
    Check_data(bid, "local__wf_cat", "Y");
}

void c::test_11(node& bid, node& ask) {
    curtest(bid, ask, "show cat", __FILE__, __LINE__);

    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::workflow::trader_protocol::push_workflow_item, 1);
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::workflow::trader_protocol::push_workflow_item,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            us::trader::workflow::consumer::item_t<us::trader::workflow::consumer::catalogue_t, 1> item;
            assert(is_ok(item.read(blob)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            assert(item.has_doc());
            ostringstream os;
            item.doc->write_pretty_en(os);
            auto s = os.str();
            cout << s << endl;
            Check_s_contains(s, "Expires:");
            Check_s_contains(s, "compact_form:");
            Check_s_contains(s, "pay_coin: ");
            Check_s_contains(s, "reward_coin: ");
            Check_s_contains(s, "gv9rkTQJ7Cyzep2dXfeVFWvSJac 10000 150");
            Check_s_contains(s, "3qRd49QNBqpH6XzPyug67fQBuufe 7510 230");
            Check_s_contains(s, "2rbPjmoN3TpZ3KeVQnA6EJsKEkhi 5050 17");
            Check_s_contains(s, "fields: product price reward");
            Check_s_contains(s, "lang: en");
        });

    exec_cur_trade(bid, "show cat");

    wait(bid, ask);
}

void c::test_12(node& bid, node& ask) {
    curtest(bid, ask, "show cat -p", __FILE__, __LINE__);

    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::workflow::trader_protocol::push_doc, 1); //103
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::workflow::trader_protocol::push_doc,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob){
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            cout << payload << endl;
            Check_s_contains(payload, "-- BEGIN OF DOCUMENT --------------------------------------------------------------");
            Check_s_contains(payload, "-- END OF DOCUMENT ----------------------------------------------------------------");
            Check_s_contains(payload, "Expires: ");
            Check_s_contains(payload, "compact_form: QTGMgnn8pXBGqHezNP4p3YJ5HJYQafr6uYxkQCAJqPBoWcbXZp9JL9PqW4vnk2WAo5fkLd1wRxkHx5h3pR1hs4RyZSpZQa49En7j2MVFAntih8PNF8G2RMJMTbkuho2brCS7BGajiA4Bzn6Y6rLw");
            Check_s_contains(payload, "lang: en");
            Check_s_contains(payload, "2rbPjmoN3TpZ3KeVQnA6EJsKEkhi 5050 17");
            Check_s_contains(payload, "3qRd49QNBqpH6XzPyug67fQBuufe 7510 230");
            Check_s_contains(payload, "2rbPjmoN3TpZ3KeVQnA6EJsKEkhi 5050 17");
        });

    exec_cur_trade(bid, "show cat -p");

    wait(bid, ask);

    {
        string basketstr = get_data(bid, "basket");
        cout << "basket " << basketstr << endl;

        us::trader::workflow::consumer::basket_t b;
        assert(is_ok(b.read(basketstr)));
        b.dump("basket>", cout);
        assert(b.empty());
        assert(b.serial == 0);
    }
    Check_data(bid, "logo", "Y");
    Check_data(bid, "remote_ip4", node::localip + ":22173");
}

//void c::test_13(node& bid, node& ask) {
//    curtest(bid, ask, "", __FILE__, __LINE__);
//}

void c::test_14(node& bid, node& ask) {
    curtest(bid, ask, "select >0", __FILE__, __LINE__);

    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 3);
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            static int seq = 0;
            cout << "seq " << seq << endl;
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            if (seq == 0) {
                if (payload.find("local__basket_serial 1") != string::npos) { //possible data coming before
                    ++seq;
                    bid.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
                else {
                    Check_s_contains(payload, "local__basket_serial 0");
                }
            }
            if (seq == 1) {
                Check_s_contains(payload, "created 1");
                Check_s_contains(payload, "activity 1");
                Check_s_contains(payload, "logfile ");
                Check_s_contains(payload, "test_l2/home/node_bid/123/wallet/trader/log/trade_1");
                Check_s_contains(payload, "state online");
                Check_s_contains(payload, "online_age 00:00:");
                Check_s_contains(payload, "remote_endpoint ");
                Check_s_contains(payload, "remote_ip4 ");
                Check_s_contains(payload, "local_endpoint ");
                Check_s_contains(payload, "my_personality 11111111111111111111");
                Check_s_contains(payload, "my_moniker anonymous");
                Check_s_contains(payload, "peer_personality zzqCyAiEUz9mrw6EX1Rdehmovpv");
                Check_s_contains(payload, "peer_moniker Impact Shopping");
                Check_s_contains(payload, "peer_protocols 2");
                Check_s_contains(payload, "initiator Y");
                Check_s_contains(payload, "protocol bid2ask bid");
                Check_s_contains(payload, "pphome ");
                Check_s_contains(payload, "test_l2/home/node_bid/123/wallet/trader/bid2ask/bid/11111111111111111111/zzqCyAiEUz9mrw6EX1Rdehmovpv");
                Check_s_contains(payload, "datadir ");
                Check_s_contains(payload, string("test_l2/home/node_bid/123/wallet/trader/trade_data/") + to_string(trade_id));
                Check_s_contains(payload, "logo Y");
                Check_s_contains(payload, "local__basket_serial 1");
                Check_s_contains(payload, "local__doctypes_consumer 288 290 292 ");
                Check_s_contains(payload, "local__doctypes_producer 291 ");
                Check_s_contains(payload, "local__wf_cat Y");
                Check_s_contains(payload, "local__wf_cat_expiry 1");
                Check_s_contains(payload, "local__wf_cat_ts 1");
                Check_s_contains(payload, "local__wf_inv N");
                Check_s_contains(payload, "local__wf_pay N");
                Check_s_contains(payload, "local__wf_rcpt N");
                Check_s_contains(payload, "chat_script 0");
                Check_s_contains(payload, "homemode me/peer");
                Check_s_contains(payload, "personality_moniker anonymous");
                Check_s_contains(payload, "personality_sk ");
                Check_s_contains(payload, "remote__basket_serial 0");
                Check_s_contains(payload, "remote__doctypes_consumer 291 ");
                Check_s_contains(payload, "remote__doctypes_producer 288 290 292 ");
                Check_s_contains(payload, "remote__wf_cat Y");
                Check_s_contains(payload, "remote__wf_cat_expiry 1");
                Check_s_contains(payload, "remote__wf_cat_ts 1");
                Check_s_contains(payload, "remote__wf_inv N");
                Check_s_contains(payload, "remote__wf_pay N");
                Check_s_contains(payload, "remote__wf_rcpt N");
                Check_s_contains(payload, "basket ");
            }
            if (seq == 2) {
                Check_s_contains(payload, "remote__basket_serial 1");
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
                Check_s_contains(payload, "peer_protocols 2");
                Check_s_contains(payload, "initiator N");
                Check_s_contains(payload, "protocol bid2ask ask");
                Check_s_contains(payload, "pphome");
                Check_s_contains(payload, "test_l2/home/node_ask/123/wallet/trader/bid2ask/ask/zzqCyAiEUz9mrw6EX1Rdehmovpv/11111111111111111111");
                Check_s_contains(payload, "datadir ");
                Check_s_contains(payload, string("test_l2/home/node_ask/123/wallet/trader/trade_data/") + to_string(trade_id));
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
                Check_s_contains(payload, "remote__doctypes_consumer 288 290 292 ");
                Check_s_contains(payload, "remote__doctypes_producer 291 ");
                Check_s_contains(payload, "remote__wf_cat Y");
                Check_s_contains(payload, "remote__wf_cat_expiry 1");
                Check_s_contains(payload, "remote__wf_cat_ts 1");
                Check_s_contains(payload, "remote__wf_inv N");
                Check_s_contains(payload, "remote__wf_pay N");
                Check_s_contains(payload, "remote__wf_rcpt N");
                Check_s_contains(payload, "basket ");
                Check_s_contains(payload, "rf N");
                Check_s_contains(payload, "remote__basket_serial 0");
            }
            if (seq == 1) {
                Check_s_contains(payload, "remote__basket_serial 1");
            }
            ++seq;
        }
    );

    exec_cur_trade(bid, "select 2rbPjmoN3TpZ3KeVQnA6EJsKEkhi 1");
    wait(bid, ask);

    {
        string basketstr = get_data(bid, "basket");
        cout << "basket bid " << basketstr << endl;

        us::trader::workflow::consumer::basket_t b;
        assert(is_ok(b.read(basketstr)));
        b.dump("basket bid>", cout);
        assert(b.size() == 1);
        assert(b.serial == 1);
        assert(b.begin()->first == hash_t("2rbPjmoN3TpZ3KeVQnA6EJsKEkhi"));
    }
    {
        string basketstr = get_data(ask, "basket");
        cout << "basket ask " << basketstr << endl;

        us::trader::workflow::consumer::basket_t b;
        assert(is_ok(b.read(basketstr)));
        b.dump("basket ask>", cout);
        assert(b.size() == 1);
        assert(b.serial == 1);
        assert(b.begin()->first == hash_t("2rbPjmoN3TpZ3KeVQnA6EJsKEkhi"));
    }
}

void c::test_15(node& bid, node& ask) {
    curtest(bid, ask, "select <0", __FILE__, __LINE__);

    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 3); //one per peer
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            static int seq = 0;
            cout << "seq " << seq << endl;
            if (seq == 0) {
                if (payload.find("local__basket_serial 2") != string::npos) { //possible data coming before
                    ++seq;
                    bid.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
            }
            if (seq == 1) {
                if (payload.find("remote__basket_serial 2") != string::npos) { //possible data coming before
                    ++seq;
                    bid.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
                else {
                    Check_s_contains(payload, "remote__basket_serial 1");
                }
            }
            if (seq == 2) {
                Check_s_contains(payload, "created 1");
                Check_s_contains(payload, "activity 1");
                Check_s_contains(payload, "logfile ");
                Check_s_contains(payload, "test_l2/home/node_bid/123/wallet/trader/log/trade_1");
                Check_s_contains(payload, "state online");
                Check_s_contains(payload, "online_age 00:00:");
                Check_s_contains(payload, "remote_endpoint ");
                Check_s_contains(payload, "remote_ip4 ");
                Check_s_contains(payload, "local_endpoint ");
                Check_s_contains(payload, "my_personality 11111111111111111111");
                Check_s_contains(payload, "my_moniker anonymous");
                Check_s_contains(payload, "peer_personality zzqCyAiEUz9mrw6EX1Rdehmovpv");
                Check_s_contains(payload, "peer_moniker Impact Shopping");
                Check_s_contains(payload, "peer_protocols 2");
                Check_s_contains(payload, "initiator Y");
                Check_s_contains(payload, "protocol bid2ask bid");
                Check_s_contains(payload, "pphome ");
                Check_s_contains(payload, "test_l2/home/node_bid/123/wallet/trader/bid2ask/bid/11111111111111111111/zzqCyAiEUz9mrw6EX1Rdehmovpv");
                Check_s_contains(payload, "datadir ");
                Check_s_contains(payload, string("test_l2/home/node_bid/123/wallet/trader/trade_data/") + to_string(trade_id));
                Check_s_contains(payload, "logo Y");
                Check_s_contains(payload, "local__doctypes_consumer 288 290 292 ");
                Check_s_contains(payload, "local__doctypes_producer 291 ");
                Check_s_contains(payload, "local__wf_cat Y");
                Check_s_contains(payload, "local__wf_cat_expiry 1");
                Check_s_contains(payload, "local__wf_cat_ts 1");
                Check_s_contains(payload, "local__wf_inv N");
                Check_s_contains(payload, "local__wf_pay N");
                Check_s_contains(payload, "local__wf_rcpt N");
                Check_s_contains(payload, "chat_script 0");
                Check_s_contains(payload, "homemode me/peer");
                Check_s_contains(payload, "personality_moniker anonymous");
                Check_s_contains(payload, "personality_sk ");
                Check_s_contains(payload, "remote__doctypes_consumer 291 ");
                Check_s_contains(payload, "remote__doctypes_producer 288 290 292 ");
                Check_s_contains(payload, "remote__wf_cat Y");
                Check_s_contains(payload, "remote__wf_cat_expiry 1");
                Check_s_contains(payload, "remote__wf_cat_ts 1");
                Check_s_contains(payload, "remote__wf_inv N");
                Check_s_contains(payload, "remote__wf_pay N");
                Check_s_contains(payload, "remote__wf_rcpt N");
                Check_s_contains(payload, "basket ");
                Check_s_contains(payload, "remote__basket_serial 2");
                Check_s_contains(payload, "local__basket_serial 2");
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
                Check_s_contains(payload, "peer_protocols 2");
                Check_s_contains(payload, "initiator N");
                Check_s_contains(payload, "protocol bid2ask ask");
                Check_s_contains(payload, "pphome");
                Check_s_contains(payload, "test_l2/home/node_ask/123/wallet/trader/bid2ask/ask/zzqCyAiEUz9mrw6EX1Rdehmovpv/11111111111111111111");
                Check_s_contains(payload, "datadir ");
                Check_s_contains(payload, string("test_l2/home/node_ask/123/wallet/trader/trade_data/") + to_string(trade_id));
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
                Check_s_contains(payload, "remote__doctypes_consumer 288 290 292 ");
                Check_s_contains(payload, "remote__doctypes_producer 291 ");
                Check_s_contains(payload, "remote__wf_cat Y");
                Check_s_contains(payload, "remote__wf_cat_expiry 1");
                Check_s_contains(payload, "remote__wf_cat_ts 1");
                Check_s_contains(payload, "remote__wf_inv N");
                Check_s_contains(payload, "remote__wf_pay N");
                Check_s_contains(payload, "remote__wf_rcpt N");
                Check_s_contains(payload, "rf N");
                Check_s_contains(payload, "basket ");
                Check_s_contains(payload, "local__basket_serial 2");
                Check_s_contains(payload, "remote__basket_serial 1");
            }
            if (seq == 1) {
                Check_s_contains(payload, "remote__basket_serial 2");
            }
            ++seq;
        }
    );
    exec_cur_trade(bid, "select 2rbPjmoN3TpZ3KeVQnA6EJsKEkhi -1");

    wait(bid, ask);

    {
        string basketstr = get_data(bid, "basket");
        cout << "basket bid " << basketstr << endl;

        us::trader::workflow::consumer::basket_t b;
        assert(is_ok(b.read(basketstr)));
        b.dump("basket bid>", cout);
        assert(b.size() == 0);
        assert(b.serial == 2);
    }
    {
        string basketstr = get_data(ask, "basket");
        cout << "basket ask " << basketstr << endl;

        us::trader::workflow::consumer::basket_t b;
        assert(is_ok(b.read(basketstr)));
        b.dump("basket ask>", cout);
        assert(b.size() == 0);
        assert(b.serial == 2);
    }
}

void c::test_16(node& bid, node& ask) {
    curtest(bid, ask, "2 x select", __FILE__, __LINE__);

    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 2);
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            static int seq = 0;
            cout << "seq " << seq << endl;
            if (seq == 0) {
                Check_s_contains(payload, "created 1");
                Check_s_contains(payload, "activity 1");
                Check_s_contains(payload, "logfile ");
                Check_s_contains(payload, "test_l2/home/node_bid/123/wallet/trader/log/trade_1");
                Check_s_contains(payload, "state online");
                Check_s_contains(payload, "online_age 00:00:");
                Check_s_contains(payload, "remote_endpoint ");
                Check_s_contains(payload, "remote_ip4 ");
                Check_s_contains(payload, "local_endpoint ");
                Check_s_contains(payload, "my_personality 11111111111111111111");
                Check_s_contains(payload, "my_moniker anonymous");
                Check_s_contains(payload, "peer_personality zzqCyAiEUz9mrw6EX1Rdehmovpv");
                Check_s_contains(payload, "peer_moniker Impact Shopping");
                Check_s_contains(payload, "peer_protocols 2");
                Check_s_contains(payload, "initiator Y");
                Check_s_contains(payload, "protocol bid2ask bid");
                Check_s_contains(payload, "pphome ");
                Check_s_contains(payload, "test_l2/home/node_bid/123/wallet/trader/bid2ask/bid/11111111111111111111/zzqCyAiEUz9mrw6EX1Rdehmovpv");
                Check_s_contains(payload, "datadir ");
                Check_s_contains(payload, string("test_l2/home/node_bid/123/wallet/trader/trade_data/") + to_string(trade_id));
                Check_s_contains(payload, "logo Y");
                Check_s_contains(payload, "local__doctypes_consumer 288 290 292 ");
                Check_s_contains(payload, "local__doctypes_producer 291 ");
                Check_s_contains(payload, "local__wf_cat Y");
                Check_s_contains(payload, "local__wf_cat_expiry 1");
                Check_s_contains(payload, "local__wf_cat_ts 1");
                Check_s_contains(payload, "local__wf_inv N");
                Check_s_contains(payload, "local__wf_pay N");
                Check_s_contains(payload, "local__wf_rcpt N");
                Check_s_contains(payload, "chat_script 0");
                Check_s_contains(payload, "homemode me/peer");
                Check_s_contains(payload, "personality_moniker anonymous");
                Check_s_contains(payload, "personality_sk ");
                Check_s_contains(payload, "remote__doctypes_consumer 291 ");
                Check_s_contains(payload, "remote__doctypes_producer 288 290 292 ");
                Check_s_contains(payload, "remote__wf_cat Y");
                Check_s_contains(payload, "remote__wf_cat_expiry 1");
                Check_s_contains(payload, "remote__wf_cat_ts 1");
                Check_s_contains(payload, "remote__wf_inv N");
                Check_s_contains(payload, "remote__wf_pay N");
                Check_s_contains(payload, "remote__wf_rcpt N");
                Check_s_contains(payload, "basket ");
                Check_s_contains(payload, "local__basket_serial 3");
                Check_s_contains(payload, "remote__basket_serial 2");
            }
            if (seq == 1) {
                Check_s_contains(payload, "basket ");
                Check_s_contains(payload, "local__basket_serial 3");
                Check_s_contains(payload, "remote__basket_serial 3");
            }
            if (seq == 2) {
                Check_s_contains(payload, "local__basket_serial 4");
                Check_s_contains(payload, "remote__basket_serial 3");
            }
            if (seq == 3) {
                Check_s_contains(payload, "local__basket_serial 4");
                Check_s_contains(payload, "remote__basket_serial 4");
            }
            ++seq;
        }
    );

    ask.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 3);
    ask.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            static int seq = 0;
            cout << "seq " << seq << endl;
            if (seq == 0) {
                if (payload.find("local__basket_serial 3") != string::npos) {
                    ++seq;
                    ask.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
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
                Check_s_contains(payload, "peer_protocols 2");
                Check_s_contains(payload, "initiator N");
                Check_s_contains(payload, "protocol bid2ask ask");
                Check_s_contains(payload, "pphome");
                Check_s_contains(payload, "test_l2/home/node_ask/123/wallet/trader/bid2ask/ask/zzqCyAiEUz9mrw6EX1Rdehmovpv/11111111111111111111");
                Check_s_contains(payload, "datadir ");
                Check_s_contains(payload, string("test_l2/home/node_ask/123/wallet/trader/trade_data/") + to_string(trade_id));
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
                Check_s_contains(payload, "remote__doctypes_consumer 288 290 292 ");
                Check_s_contains(payload, "remote__doctypes_producer 291 ");
                Check_s_contains(payload, "remote__wf_cat Y");
                Check_s_contains(payload, "remote__wf_cat_expiry 1");
                Check_s_contains(payload, "remote__wf_cat_ts 1");
                Check_s_contains(payload, "remote__wf_inv N");
                Check_s_contains(payload, "remote__wf_pay N");
                Check_s_contains(payload, "remote__wf_rcpt N");
                Check_s_contains(payload, "rf N");
                Check_s_contains(payload, "basket ");
                Check_s_contains(payload, "local__basket_serial 3");
                Check_s_contains(payload, "remote__basket_serial 2");
            }
            if (seq == 2) {
                Check_s_contains(payload, "local__basket_serial 3");
                Check_s_contains(payload, "remote__basket_serial 3");
            }
            if (seq == 3) {
                Check_s_contains(payload, "local__basket_serial 4");
                Check_s_contains(payload, "remote__basket_serial 3");
            }
            if (seq == 4) {
                Check_s_contains(payload, "local__basket_serial 4");
                Check_s_contains(payload, "remote__basket_serial 4");
            }
            ++seq;
        }
    );

    exec_cur_trade(bid, "select 3qRd49QNBqpH6XzPyug67fQBuufe 3");
    wait(bid, ask);

    log("adding another product more");

    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 2);
    ask.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 2);

    exec_cur_trade(bid, "select 2rbPjmoN3TpZ3KeVQnA6EJsKEkhi 3");
    wait(bid, ask);

    {
        string basketstr = get_data(bid, "basket");
        cout << "basket bid " << basketstr << endl;

        us::trader::workflow::consumer::basket_t b;
        assert(is_ok(b.read(basketstr)));
        b.dump("basket bid>", cout);
        assert(b.size() == 2);
        assert(b.serial == 4);
        {
            auto i = b.find(hash_t("2rbPjmoN3TpZ3KeVQnA6EJsKEkhi"));
            assert(i != b.end());
            assert(i->second.first = 3);
            assert(i->second.second.first = 5050);
            assert(i->second.second.second = 17);
        }
        {
            auto i = b.find(hash_t("3qRd49QNBqpH6XzPyug67fQBuufe"));
            assert(i != b.end());
            assert(i->second.first = 3);
            assert(i->second.second.first = 7510);
            assert(i->second.second.second = 230);
        }
    }
    {
        string basketstr = get_data(ask, "basket");
        cout << "basket ask " << basketstr << endl;

        us::trader::workflow::consumer::basket_t b;
        assert(is_ok(b.read(basketstr)));
        b.dump("basket ask>", cout);
        assert(b.size() == 2);
        assert(b.serial == 4);
        {
            auto i = b.find(hash_t("2rbPjmoN3TpZ3KeVQnA6EJsKEkhi"));
            assert(i != b.end());
            assert(i->second.first = 3);
            assert(i->second.second.first = 5050);
            assert(i->second.second.second = 17);
        }
        {
            auto i = b.find(hash_t("3qRd49QNBqpH6XzPyug67fQBuufe"));
            assert(i != b.end());
            assert(i->second.first = 3);
            assert(i->second.second.first = 7510);
            assert(i->second.second.second = 230);
        }
    }

}

void c::test_17(node& bid, node& ask) {
    curtest(bid, ask, "request inv", __FILE__, __LINE__);

    Check_data(bid, "local__wf_inv", "N");
    Check_data(ask, "local__wf_inv", "N");
    bid.wallet_cli_dis->expected_code.emplace(us::gov::relay::pushman::push_ok, 1);
    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 3);
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            static int seq =0;
            if (seq == 0) {
                if (payload.find("local__wf_inv Y") != string::npos) {
                    ++seq;
                    bid.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
                else {
                    Check_s_contains(payload, "rf N");
                    Check_s_contains(payload, "local__wf_inv N");
                    Check_s_contains(payload, "local__wf_inv_mode recv");
                    Check_s_contains(payload, "remote__wf_inv Y");
                    Check_s_contains(payload, "remote__wf_inv_mode send");
                    Check_s_contains(payload, "remote__wf_inv_ts 1");
                }
            }
            if (seq == 1) {
                Check_s_contains(payload, "local__wf_inv Y");
                Check_s_contains(payload, "local__wf_inv_mode recv");
                Check_s_contains(payload, "local__wf_inv_ts 1");

                if (payload.find("remote__wf_inv Y") != string::npos) {
                    ++seq;
                    bid.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
                else {
                    Check_s_contains(payload, "remote__wf_inv N");
                }

            }
            if (seq == 2) {
                Check_s_contains(payload, "local__wf_inv Y");
                Check_s_contains(payload, "local__wf_inv_mode recv");
                Check_s_contains(payload, "local__wf_inv_ts 1");

                Check_s_contains(payload, "remote__wf_inv Y");
                Check_s_contains(payload, "remote__wf_inv_mode send");
                Check_s_contains(payload, "remote__wf_inv_ts 1");

            }
            ++seq;
        }
    );

    ask.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 2);
    ask.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid==ask.wallet_cli->cur);
            assert(tid==trade_id);
            static int seq =0;
            assert(tid==bid.wallet_cli->cur);
            assert(tid==trade_id);
            Check_s_contains(payload, "rf N");
            if (seq == 0) {
                Check_s_contains(payload, "remote__wf_inv N");
                Check_s_contains(payload, "remote__wf_inv_mode recv");
                Check_s_contains(payload, "local__wf_inv Y");
                Check_s_contains(payload, "local__wf_inv_mode send");
                Check_s_contains(payload, "local__wf_inv_ts 1");
            }
            else if (seq == 1) {
                Check_s_contains(payload, "remote__wf_inv Y");
                Check_s_contains(payload, "remote__wf_inv_mode recv");
                Check_s_contains(payload, "remote__wf_inv_ts 1");
                Check_s_contains(payload, "local__wf_inv Y");
                Check_s_contains(payload, "local__wf_inv_mode send");
                Check_s_contains(payload, "local__wf_inv_ts 1");

            }
            else {
                Fail();
            }
            ++seq;
        }
    );

    exec_cur_trade(bid, "request inv");
    wait(bid, ask);

    Check_data(bid, "local__wf_inv", "Y");
    Check_data(ask, "local__wf_inv", "Y");
    Check_entry_exists(bid, "local__wf_inv_ts");
}

void c::test_18(node& bid, node& ask) {
    curtest(bid, ask, "show inv -p", __FILE__, __LINE__);

    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::workflow::trader_protocol::push_doc, 1);
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::workflow::trader_protocol::push_doc,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid==bid.wallet_cli->cur);
            assert(tid==trade_id);
            Check_s_contains(payload, "-- BEGIN OF DOCUMENT --------------------------------------------------------------");
            Check_s_contains(payload, "-- END OF DOCUMENT ----------------------------------------------------------------");
            Check_s_not_contains(payload, "refer_personality: 11111111111111111111"); //anon personality doesn't show up in the doc
            Check_s_contains(payload, "Invoice");
            Check_s_contains(payload, "Document date: ");
            Check_s_contains(payload, " UTC");
            Check_s_contains(payload, "instructions: ");
            Check_s_contains(payload, "lang: en");
            Check_s_contains(payload, "tx: ");
            Check_s_contains(payload, "type: cash");
            Check_s_contains(payload, "processor: 30 0");
            Check_s_contains(payload, "exec time: ");
            Check_s_contains(payload, "section #0. coin");
            Check_s_contains(payload, "section #1. coin");
            Check_s_contains(payload, "unlock with: ");
            Check_s_contains(payload, "---transaction---------------");
            Check_s_contains(payload, "-/-transaction---------------");
            Check_s_contains(payload, "Transaction is incomplete. section 0. No inputs");
            Check_s_contains(payload, "section 1 input 0. Valid.");
            Check_s_contains(payload, "section 1. No outputs");
            Check_s_contains(payload, "Signed by: zzqCyAiEUz9mrw6EX1Rdehmovpv");
            Check_s_contains(payload, "Public key: 21DCFrG7K1NSJKTeiGNhTvnH4y5DZr4MDmSVHf8sLBcto");
            Check_s_contains(payload, "Signature: ");
            Check_s_contains(payload, "Cryptographic verification SUCCEED. Data is legitimate.");
            Check_s_contains(payload, "amount: 37680");
            Check_s_contains(payload, "withdraw: 741");
        });

    exec_cur_trade(bid, "show inv -p");
    wait(bid, ask);
}

void c::test_19(node& bid, node& ask) {
    curtest(bid, ask, "inv_pay", __FILE__, __LINE__);

    Check_data(bid, "local__wf_pay", "N");
    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 6);
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            static int seq = 0;
            cout << "seq " << seq << endl;

            Check_s_contains(payload, "created 1");
            Check_s_contains(payload, "activity 1");
            Check_s_contains(payload, "logfile ");
            Check_s_contains(payload, "test_l2/home/node_bid/123/wallet/trader/log/trade_1");
            Check_s_contains(payload, "state online");
            Check_s_contains(payload, "online_age 00:00:");
            Check_s_contains(payload, "remote_endpoint ");
            Check_s_contains(payload, "remote_ip4 ");
            Check_s_contains(payload, "local_endpoint ");
            Check_s_contains(payload, "my_personality 11111111111111111111");
            Check_s_contains(payload, "my_moniker anonymous");
            Check_s_contains(payload, "peer_personality zzqCyAiEUz9mrw6EX1Rdehmovpv");
            Check_s_contains(payload, "peer_moniker Impact Shopping");
            Check_s_contains(payload, "peer_protocols 2");
            Check_s_contains(payload, "initiator Y");
            Check_s_contains(payload, "protocol bid2ask bid");
            Check_s_contains(payload, "pphome ");
            Check_s_contains(payload, "test_l2/home/node_bid/123/wallet/trader/bid2ask/bid/11111111111111111111/zzqCyAiEUz9mrw6EX1Rdehmovpv");
            Check_s_contains(payload, "datadir ");
            Check_s_contains(payload, string("test_l2/home/node_bid/123/wallet/trader/trade_data/") + to_string(trade_id));
            Check_s_contains(payload, "logo Y");
            Check_s_contains(payload, "local__doctypes_consumer 288 290 292 ");
            Check_s_contains(payload, "local__doctypes_producer 291 ");
            Check_s_contains(payload, "local__wf_cat Y");
            Check_s_contains(payload, "local__wf_cat_expiry 1");
            Check_s_contains(payload, "local__wf_cat_ts 1");
            Check_s_contains(payload, "local__wf_inv Y");
            Check_s_contains(payload, "local__wf_inv_ts 1");
            Check_s_contains(payload, "local__wf_pay Y");
            Check_s_contains(payload, "local__wf_pay_ts 1");
            Check_s_contains(payload, "chat_script 0");
            Check_s_contains(payload, "homemode me/peer");
            Check_s_contains(payload, "personality_moniker anonymous");
            Check_s_contains(payload, "personality_sk ");
            Check_s_contains(payload, "remote__doctypes_consumer 291 ");
            Check_s_contains(payload, "remote__doctypes_producer 288 290 292 ");
            Check_s_contains(payload, "remote__wf_cat Y");
            Check_s_contains(payload, "remote__wf_cat_expiry 1");
            Check_s_contains(payload, "remote__wf_cat_ts 1");
            Check_s_contains(payload, "remote__wf_inv Y");
            Check_s_contains(payload, "remote__wf_inv_ts 1");
            {
                string basketstr = get_data(bid, "basket");
                Check_s_contains(payload, string("basket ") + basketstr);
                cout << "basket bid " << basketstr << endl;

                us::trader::workflow::consumer::basket_t b;
                assert(is_ok(b.read(basketstr)));
                b.dump("basket bid>", cout);
                assert(b.size() == 2);
                assert(b.serial == 4);
                {
                    auto i = b.find(hash_t("2rbPjmoN3TpZ3KeVQnA6EJsKEkhi"));
                    assert(i != b.end());
                    assert(i->second.first = 3);
                    assert(i->second.second.first = 5050);
                    assert(i->second.second.second = 17);
                }
                {
                    auto i = b.find(hash_t("3qRd49QNBqpH6XzPyug67fQBuufe"));
                    assert(i != b.end());
                    assert(i->second.first = 3);
                    assert(i->second.second.first = 7510);
                    assert(i->second.second.second = 230);
                }
            }
            if (seq == 0) {
                Check_s_contains(payload, "local__wf_pay Y");
                Check_s_contains(payload, "local__wf_pay_mode send");
                Check_s_contains(payload, "local__wf_pay_ts 1");
                Check_s_contains(payload, "remote__wf_pay N");
                Check_s_contains(payload, "remote__wf_pay_mode recv");
                Check_s_contains(payload, "remote__wf_rcpt N");
                Check_s_contains(payload, "remote__wf_rcpt_mode send");
                if (payload.find("local__wf_rcpt Y") != string::npos) {
                    seq++;
                    bid.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
                else {
                    Check_s_contains(payload, "local__wf_rcpt N");
                    Check_s_contains(payload, "local__wf_rcpt_mode recv");
                }
            }
            if (seq == 1) {
                Check_s_contains(payload, "local__wf_pay Y");
                Check_s_contains(payload, "local__wf_pay_mode send");
                Check_s_contains(payload, "local__wf_pay_ts 1");
                Check_s_contains(payload, "local__wf_rcpt");
                if (payload.find("remote__wf_pay Y") != string::npos) {
                    ++seq;
                    bid.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
                else {
                    Check_s_contains(payload, "remote__wf_pay N");
                    Check_s_contains(payload, "remote__wf_pay_mode recv");
                    Check_s_contains(payload, "remote__wf_rcpt N");
                    Check_s_contains(payload, "remote__wf_rcpt_mode send");
                }
            }
            if (seq == 2) {
                if (payload.find("remote__wf_pay Y") != string::npos) {
                    ++seq;
                    bid.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
                else {
                    Check_s_contains(payload, "remote__wf_pay N");
                    Check_s_contains(payload, "remote__wf_pay_mode recv");
                }
            }
            if (seq == 3) {
                Check_s_contains(payload, "remote__wf_pay Y");
                Check_s_contains(payload, "remote__wf_pay_mode recv");
                if (payload.find("remote__wf_rcpt Y") != string::npos) {
                    ++seq;
                    bid.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
                else {
                    Check_s_contains(payload, "remote__wf_rcpt N");
                    Check_s_contains(payload, "remote__wf_rcpt_mode send");
                }
            }
            if (seq == 4) {
                Check_s_contains(payload, "remote__wf_pay Y");
                Check_s_contains(payload, "remote__wf_rcpt Y");
                Check_s_contains(payload, "remote__wf_rcpt_mode send");
                Check_s_contains(payload, "remote__wf_rcpt_ts 1");
                if (payload.find("local__wf_rcpt Y") != string::npos) {
                    ++seq;
                    bid.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
                else {
                    Check_s_contains(payload, "local__wf_rcpt N");
                }
            }
            if (seq == 5) {
                Check_s_contains(payload, "local__wf_pay Y");
                Check_s_contains(payload, "local__wf_pay_mode send");
                Check_s_contains(payload, "local__wf_pay_ts 1");
                Check_s_contains(payload, "remote__wf_pay Y");
                Check_s_contains(payload, "remote__wf_pay_mode recv");
                Check_s_contains(payload, "remote__wf_pay Y");
                Check_s_contains(payload, "remote__wf_rcpt Y");
                Check_s_contains(payload, "remote__wf_rcpt_mode send");
                Check_s_contains(payload, "remote__wf_rcpt_ts 1");
                Check_s_contains(payload, "local__wf_rcpt Y");
                Check_s_contains(payload, "local__wf_rcpt_mode recv");
                Check_s_contains(payload, "local__wf_rcpt_ts 1");
            }
            ++seq;
        }
    );

    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_chat, 1);
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_chat,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            us::wallet::trader::chat_t chat;
            assert(is_ok(blob_reader_t::parse(blob, chat)));
            chat.dump("chat bid>", cout);
            cout << "last thing: " << chat.last_thing() << endl;
            cout << "last thing me: " << chat.last_thing_me() << endl;
            Check_s_eq(chat.last_thing(), "Thank you for shopping at Impact Shopping.");
            Check_s_eq(chat.last_thing_me(), "");
        });

    ask.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_chat, 1);
    ask.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_chat,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            assert(tid==ask.wallet_cli->cur);
            assert(tid==trade_id);
            us::wallet::trader::chat_t chat;
            assert(is_ok(blob_reader_t::parse(blob, chat)));
            chat.dump("chat ask>", cout);
            cout << "last thing: " << chat.last_thing() << endl;
            cout << "last thing me: " << chat.last_thing_me() << endl;
            Check_s_eq(chat.last_thing(), "");
            Check_s_eq(chat.last_thing_me(), "Thank you for shopping at Impact Shopping.");
        });

    ask.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 4);
    ask.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            static int seq = 0;
            cout << "seq " << seq << endl;
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
            Check_s_contains(payload, "peer_protocols 2");
            Check_s_contains(payload, "initiator N");
            Check_s_contains(payload, "protocol bid2ask ask");
            Check_s_contains(payload, "pphome");
            Check_s_contains(payload, "test_l2/home/node_ask/123/wallet/trader/bid2ask/ask/zzqCyAiEUz9mrw6EX1Rdehmovpv/11111111111111111111");
            Check_s_contains(payload, "datadir ");
            Check_s_contains(payload, string("test_l2/home/node_ask/123/wallet/trader/trade_data/") + to_string(trade_id));
            Check_s_contains(payload, "logo N");
            Check_s_contains(payload, "local__doctypes_consumer 291 ");
            Check_s_contains(payload, "local__doctypes_producer 288 290 292 ");
            Check_s_contains(payload, "local__wf_cat Y");
            Check_s_contains(payload, "local__wf_cat_expiry 1");
            Check_s_contains(payload, "local__wf_cat_ts 1");
            Check_s_contains(payload, "local__wf_inv Y");
            Check_s_contains(payload, "local__wf_inv_ts 1");
            Check_s_contains(payload, "chat_script 0");
            Check_s_contains(payload, "homemode me/peer");
            Check_s_contains(payload, "personality_sk ********");
            Check_s_contains(payload, "personality_moniker Impact Shopping");
            Check_s_contains(payload, "remote__doctypes_consumer 288 290 292 ");
            Check_s_contains(payload, "remote__doctypes_producer 291 ");
            Check_s_contains(payload, "remote__wf_cat Y");
            Check_s_contains(payload, "remote__wf_cat_expiry 1");
            Check_s_contains(payload, "remote__wf_cat_ts 1");
            Check_s_contains(payload, "remote__wf_inv Y");
            Check_s_contains(payload, "remote__wf_inv_ts 1");
            Check_s_contains(payload, "rf N");
            Check_s_contains(payload, "basket ");
            if (seq == 0) {
                Check_s_contains(payload, "local__basket_serial 4");
                Check_s_contains(payload, "local__doctypes_consumer 291 ");
                Check_s_contains(payload, "local__doctypes_producer 288 290 292 ");
                Check_s_contains(payload, "local__wf_cat Y");
                Check_s_contains(payload, "local__wf_cat_expiry 1");
                Check_s_contains(payload, "local__wf_cat_mode send");
                Check_s_contains(payload, "local__wf_cat_ts 1");
                Check_s_contains(payload, "local__wf_inv Y");
                Check_s_contains(payload, "local__wf_inv_mode send");
                Check_s_contains(payload, "local__wf_inv_ts 1");
                Check_s_contains(payload, "remote__basket_serial 4");
                Check_s_contains(payload, "remote__doctypes_consumer 288 290 292 ");
                Check_s_contains(payload, "remote__doctypes_producer 291 ");
                Check_s_contains(payload, "remote__wf_cat Y");
                Check_s_contains(payload, "remote__wf_cat_expiry 1");
                Check_s_contains(payload, "remote__wf_cat_mode recv");
                Check_s_contains(payload, "remote__wf_cat_ts 1");
                Check_s_contains(payload, "remote__wf_inv Y");
                Check_s_contains(payload, "remote__wf_inv_mode recv");
                Check_s_contains(payload, "remote__wf_inv_ts 1");
                if (payload.find("remote__wf_pay Y") != string::npos) { //possible data coming before
                    ++seq;
                    ask.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
                else {
                    Check_s_contains(payload, "remote__wf_pay N");
                    Check_s_contains(payload, "remote__wf_pay_mode send");
                    Check_s_contains(payload, "remote__wf_rcpt N");
                    Check_s_contains(payload, "remote__wf_rcpt_mode recv");
                }
            }
            if (seq == 1) {
                Check_s_contains(payload, "remote__wf_pay Y");
                Check_s_contains(payload, "remote__wf_pay_mode send");
                Check_s_contains(payload, "remote__wf_pay_ts 1");
                Check_s_contains(payload, "remote__wf_rcpt N");
                Check_s_contains(payload, "remote__wf_rcpt_mode recv");
                if (payload.find("local__wf_pay Y") != string::npos) { //possible data coming before
                    ++seq;
                    ask.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
                else {
                    Check_s_contains(payload, "local__wf_pay N");
                    Check_s_contains(payload, "local__wf_rcpt Y");
                    Check_s_contains(payload, "local__wf_rcpt_mode send");
                    Check_s_contains(payload, "local__wf_rcpt_ts 1");
                }

            }
            if (seq == 2) {
                Check_s_contains(payload, "local__wf_pay Y");
                Check_s_contains(payload, "local__wf_pay_mode recv");
                Check_s_contains(payload, "local__wf_pay_ts 1");
                Check_s_contains(payload, "local__wf_rcpt Y");
                Check_s_contains(payload, "local__wf_rcpt_mode send");
                Check_s_contains(payload, "local__wf_rcpt_ts 1");
                if (payload.find("remote__wf_rcpt Y") != string::npos) { //possible data coming before
                    ++seq;
                    ask.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                }
                else {
                    Check_s_contains(payload, "remote__wf_rcpt N");
                }
            }
            if (seq == 3) {
                Check_s_contains(payload, "local__wf_pay Y");
                Check_s_contains(payload, "local__wf_pay_mode recv");
                Check_s_contains(payload, "local__wf_pay_ts 1");
                Check_s_contains(payload, "local__wf_rcpt Y");
                Check_s_contains(payload, "local__wf_rcpt_mode send");
                Check_s_contains(payload, "local__wf_rcpt_ts 1");
                Check_s_contains(payload, "remote__wf_rcpt Y");
                Check_s_contains(payload, "remote__wf_rcpt_mode recv");
                Check_s_contains(payload, "remote__wf_rcpt_ts 1");
            }
            ++seq;
        }
    );

    exec_cur_trade(bid, "inv_pay Ref: HIJK");
    wait(bid, ask);
    Check_data(bid, "local__wf_pay", "Y");
}

void c::test_20(node& bid, node& ask) {
    curtest(bid, ask, "show pay -p", __FILE__, __LINE__);

    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::workflow::trader_protocol::push_doc, 1);
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::workflow::trader_protocol::push_doc,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid==bid.wallet_cli->cur);
            assert(tid==trade_id);
            Check_s_contains(payload, "-- BEGIN OF DOCUMENT --------------------------------------------------------------");
            Check_s_contains(payload, "-- END OF DOCUMENT ----------------------------------------------------------------");
            Check_s_not_contains(payload, "refer_personality: 11111111111111111111"); //anon personality doesn't show up in the doc
            Check_s_contains(payload, "Payment");
            Check_s_contains(payload, "lang: en");
            Check_s_contains(payload, "message: Ref: HIJK");
            Check_s_contains(payload, "pay_amount: 37680");
            Check_s_contains(payload, "reward_amount: 741");
            Check_s_contains(payload, "pay_coin: ");
            Check_s_contains(payload, "reward_coin: ");
            Check_s_contains(payload, "basket_hash: ");
            Check_s_contains(payload, "tx: ");
            Check_s_contains(payload, "type: cash");
            Check_s_contains(payload, "processor: 30 0");
            Check_s_contains(payload, "exec time: ");
            Check_s_contains(payload, "section #0. coin");
            Check_s_contains(payload, "section #1. coin");
            Check_s_contains(payload, "unlock with: ");
            Check_s_contains(payload, "---transaction---------------");
            Check_s_contains(payload, "-/-transaction---------------");
            Check_s_contains(payload, "Transaction is complete.");
            Check_s_contains(payload, "Signed by: ");
            Check_s_contains(payload, "Public key: ");
            Check_s_contains(payload, "Signature: ");
            Check_s_contains(payload, "Cryptographic verification SUCCEED. Data is legitimate.");
            Check_s_contains(payload, "amount: 37680");
            Check_s_contains(payload, "withdraw: 741");
            Check_s_contains(payload, "amount: 741");
            Check_s_contains(payload, "withdraw: 37680");
        });

    exec_cur_trade(bid, "show pay -p");
    wait(bid, ask);
}

void c::test_21(node& bid, node& ask) {
    curtest(bid, ask, "inv_pay 2nd", __FILE__, __LINE__);

    bid.wallet_cli_dis->expected_code.emplace(us::gov::relay::pushman::push_ko, 1);
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::workflow::trader_protocol::push_doc,
    [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
        string payload;
        assert(is_ok(blob_reader_t::parse(blob, payload)));
        assert(tid==bid.wallet_cli->cur);
        assert(tid==trade_id);
        Check_s_contains(payload, "KO 40219 Payment has been already completed.");
    });

    exec_cur_trade(bid, "inv_pay");
    wait(bid, ask);
}

void c::test_22(node& bid, node& ask) {
    curtest(bid, ask, "select after pay", __FILE__, __LINE__);

    bid.wallet_cli_dis->expected_code.emplace(us::gov::relay::pushman::push_ko, 1);
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::workflow::trader_protocol::push_doc,
    [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
        string payload;
        assert(is_ok(blob_reader_t::parse(blob, payload)));
        assert(tid == bid.wallet_cli->cur);
        assert(tid == trade_id);
        Check_s_contains(payload, "KO 40219 Payment has been already completed. The basket cannot be changed.");
    });

    exec_cur_trade(bid, "select 3qRd49QNBqpH6XzPyug67fQBuufe 1");
    wait(bid, ask);
}

void c::test_23(node& bid, node& ask) {
    curtest(bid, ask, "show rcpt -p", __FILE__, __LINE__);

    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::workflow::trader_protocol::push_doc, 1);
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::workflow::trader_protocol::push_doc,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            Check_s_contains(payload, "-- BEGIN OF DOCUMENT --------------------------------------------------------------");
            Check_s_contains(payload, "-- END OF DOCUMENT ----------------------------------------------------------------");
            Check_s_not_contains(payload, "refer_personality: 11111111111111111111"); //anon personality doesn't show up in the doc
            Check_s_contains(payload, "Receipt");
            Check_s_contains(payload, "lang: en");
            Check_s_contains(payload, "tx: ");
            Check_s_contains(payload, "Transaction successfully relayed.");
            Check_s_contains(payload, "Signed by: zzqCyAiEUz9mrw6EX1Rdehmovpv");
            Check_s_contains(payload, "Public key: 21DCFrG7K1NSJKTeiGNhTvnH4y5DZr4MDmSVHf8sLBcto");
            Check_s_contains(payload, "Signature: ");
            Check_s_contains(payload, "Cryptographic verification SUCCEED. Data is legitimate.");
            Check_s_contains(payload, "2rbPjmoN3TpZ3KeVQnA6EJsKEkhi 3 5050 17");
            Check_s_contains(payload, "3qRd49QNBqpH6XzPyug67fQBuufe 3 7510 230");
            Check_s_contains(payload, "status: PAID");
            Check_s_contains(payload, "basket_hash: ");
            Check_s_contains(payload, "fields: product units price/unit reward/unit");
            Check_s_contains(payload, "message: Ref: HIJK");
            Check_s_contains(payload, "pay_amount: 37680");
            Check_s_contains(payload, "pay_coin: ");
            Check_s_contains(payload, "reward_amount: 741");
            Check_s_contains(payload, "reward_coin: ");
            Check_s_contains(payload, "trade_id: ");
        });

    exec_cur_trade(bid, "show rcpt -p");
    wait(bid, ask);
}

void c::check_local_remote(node& n, const string& var) {
    assert(get_data(n, string("local__") + var) == get_data(n, string("remote__") + var));
}

void c::check_wf_data(node& n) {
    Check_data(n, "local__wf_cat", "Y");
    Check_entry_exists(n, "local__wf_cat_ts");
    Check_entry_exists(n, "local__wf_cat_expiry");
    Check_data(n, "local__wf_inv", "Y");
    Check_entry_exists(n, "local__wf_inv_ts");
    Check_data(n, "local__wf_pay", "Y");
    Check_entry_exists(n, "local__wf_pay_ts");
    Check_data(n, "local__wf_rcpt", "Y");
    Check_entry_exists(n, "local__wf_rcpt_ts");

    Check_data(n, "remote__wf_cat", "Y");
    Check_entry_exists(n, "remote__wf_cat_ts");
    Check_entry_exists(n, "remote__wf_cat_expiry");
    Check_data(n, "remote__wf_inv", "Y");
    Check_entry_exists(n, "remote__wf_inv_ts");
    Check_data(n, "remote__wf_pay", "Y");
    Check_entry_exists(n, "remote__wf_pay_ts");
    Check_data(n, "remote__wf_rcpt", "Y");
    Check_entry_exists(n, "remote__wf_rcpt_ts");

    check_local_remote(n, "wf_cat_ts");
    check_local_remote(n, "wf_cat_expiry");
    check_local_remote(n, "wf_inv_ts");
    check_local_remote(n, "wf_pay_ts");
    check_local_remote(n, "wf_rcpt_ts");
}

void c::check_data_fields(node& n) {
    Check_entry_exists(n, "created");
    Check_entry_exists(n, "activity");
    Check_entry_exists(n, "logfile");
    Check_entry_exists(n, "state");
    Check_entry_exists(n, "peer_personality");
    Check_entry_exists(n, "online_age");
    Check_entry_exists(n, "rf");
    Check_entry_exists(n, "basket");
    Check_entry_exists(n, "personality_sk");
    Check_entry_exists(n, "homemode");
    Check_entry_exists(n, "chat_script");
    Check_entry_exists(n, "logo");
    Check_entry_exists(n, "datadir");
    Check_entry_exists(n, "pphome");
    Check_entry_exists(n, "protocol");
    Check_entry_exists(n, "initiator");
    Check_entry_exists(n, "peer_protocols");
    Check_entry_exists(n, "my_personality");
    Check_entry_exists(n, "remote_ip4");
    Check_entry_exists(n, "remote_endpoint");
    Check_entry_exists(n, "local_endpoint");
    Check_entry_exists(n, "remote__doctypes_consumer");
    Check_entry_exists(n, "remote__doctypes_producer");
    Check_entry_exists(n, "local__doctypes_consumer");
    Check_entry_exists(n, "local__doctypes_producer");
}

void c::test_24(node& bid, node& ask) {
    curtest(bid, ask, "bid show data", __FILE__, __LINE__);

    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 1);
    exec_cur_trade(bid, "show data");

    wait(bid, ask);

    check_wf_data(bid);
    Check_data(bid, "initiator", "Y");
    check_data_fields(bid);
}

void c::test_25(node& bid, node& ask) {
    curtest(bid, ask, "ask - show data", __FILE__, __LINE__);

    ask.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 1); //103
    exec_cur_trade(ask, "show data");

    wait(bid, ask);

    check_wf_data(ask);
    Check_data(ask, "initiator", "N");
    check_data_fields(ask);
}

void c::test_26(node& bid, node& ask) {
    curtest(bid, ask, "world", __FILE__, __LINE__);
    cout << "check world" << endl;
    vector<hash_t> v;
    auto r = bid.wallet_cli->rpc_daemon->get_peer().call_world(v);
    if (r != ok) {
        cout << r << endl;
        assert(false);
    }
    cout << "num wallets in world: " << v.size() << endl;
}

void c::test_end(node& bid, node& ask) {
    curtest(bid, ask, "disconnect", __FILE__, __LINE__);

    bid.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 1);
    bid.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            Check_s_contains(payload, "state offline");
        }
    );

    ask.wallet_cli_dis->expected_code.emplace(us::gov::relay::pushman::push_ok, 1);
    ask.wallet_cli_dis->expected_code.check.emplace(us::gov::relay::pushman::push_ok,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            Check_s_contains(payload, "Going offline.");
        }
    );

    ask.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 1);
    ask.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            string payload;
            assert(is_ok(blob_reader_t::parse(blob, payload)));
            assert(tid == bid.wallet_cli->cur);
            assert(tid == trade_id);
            Check_s_contains(payload, "state offline");
        }
    );

    exec_cur_trade(ask, "disconnect");
    wait(bid, ask);
}

void c::test(node& bid, node& ask) {
    using hash_t = us::gov::crypto::ripemd160::value_type;

    cout << "start trade with impact shopping" << endl;
    cout << "bid node: ";
    bid.gov_cli->exec("id");
    cout << "ask node: ";
    ask.gov_cli->exec("id");
    cout << "ask node wallet port: " << ask.wport << endl;
    cout << "bid current trade id " << bid.wallet_cli->cur << endl;
    cout << "ask current trade id " << ask.wallet_cli->cur << endl;
    assert(bid.wallet_cli->cur.is_zero());
    assert(ask.wallet_cli->cur.is_zero());

    auto reward_coin = dynamic_cast<node_ask&>(ask).reward_coin;
    auto addr = dynamic_cast<node_ask&>(ask).addr;
    assert(reward_coin.is_not_zero());
    assert(addr.is_not_zero());

    test_0(bid, ask); // initiate trade

    test_0_0(bid, ask); // connect when online
    test_0_ping(bid, ask);
    test_0_ping(ask, bid);
    test_1(bid, ask);
    test_3(bid, ask);
    test_4(bid, ask);
    test_5(bid, ask);
    test_6(bid, ask);
    test_7(bid, ask);
    test_8(bid, ask);
    test_9(bid, ask);
    test_10(bid, ask);
    test_11(bid, ask);
    test_12(bid, ask);
    test_restartw(bid, ask);
    test_14(bid, ask);
    test_15(bid, ask);
    test_16(bid, ask);
    test_17(bid, ask);
    test_18(bid, ask);
    test_19(bid, ask);
    test_20(bid, ask);
    test_21(bid, ask);
    test_22(bid, ask);
    test_23(bid, ask);
    test_24(bid, ask);
    test_25(bid, ask);
    test_26(bid, ask);
    test_end(bid, ask);
}

void c::run() {
    auto& n1 = *n.find("bid")->second;
    auto& n2 = *n.find("ask")->second;
    test_r2r_cfg(n1, n2, [&](node& n1, node& n2) { test(n1, n2); }, trade_id);
}

