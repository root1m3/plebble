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
#include "w2w_t.h"

#include <us/wallet/engine/daemon_t.h>

#include "node.h"
#include "dispatcher_t.h"
#include "network.h"

#define loglevel "test"
#define logclass "w2w_t"

#include <us/gov/logs.inc>
#include "assert.inc"

using c = us::test::w2w_t;

c::track_t c::init_transfer(node& sender, node& rcpt) {
    //transfer gas
    track_t track{0};
    sender.wallet_cli_dis->expected_code.emplace(us::wallet::wallet::local_api::push_txlog, 2);
    sender.wallet_cli_dis->expected_code.check.emplace(us::wallet::wallet::local_api::push_txlog,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            assert(tid == trade_id);
            us::wallet::wallet::index_t txnd;
            assert(is_ok(blob_reader_t::parse(blob, txnd)));
            static int seq = 0;
            ostringstream pfx;
            pfx << "sender-txlog-index-#" << seq << "> ";
            txnd.dump(pfx.str(), cout);
            assert(txnd.size() == 1);
            if (seq == 0) {
                track = txnd.begin()->first;
                cout << "track " << track;
                assert(txnd.begin()->second.wallet_track_status == us::wallet::wallet::wts_wait_rcpt_info);
            }
            else if (seq == 1) {
                assert(txnd.begin()->first == track);
                assert(txnd.begin()->second.wallet_track_status == us::wallet::wallet::wts_wait_signature);
                seq = -1;
            }
            ++seq;
        }
    );

    rcpt.wallet_cli_dis->expected_code.emplace(us::wallet::wallet::local_api::push_txlog, 1);
    rcpt.wallet_cli_dis->expected_code.check.emplace(us::wallet::wallet::local_api::push_txlog,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            assert(tid == trade_id);
            us::wallet::wallet::index_t txnd;
            assert(is_ok(blob_reader_t::parse(blob, txnd)));
            txnd.dump("rcpt-txlog-index-#0> ", cout);
            assert(txnd.size() == 1);
            assert(txnd.begin()->first == track);
            assert(txnd.begin()->second.wallet_track_status == us::wallet::wallet::wts_wait_signature);
        }
    );

    auto r = sender.wallet_cli->rpc_daemon->get_peer().call_exec_trade(us::wallet::cli::rpc_peer_t::exec_trade_in_t(trade_id, "transfer 100 gas"));
    if (is_ko(r)) {
        cout << sender.wallet_cli->rewrite(r) << '\n';
        assert(false);
        exit(1);
    }
    wait(sender, rcpt);
    assert(track != 0);
    return track;
}

void c::cancel_transfer_by_sender1(node& sender, node& rcpt, track_t track) {
    assert(track != 0);
    sender.wallet_cli_dis->expected_code.emplace(us::wallet::wallet::local_api::push_txlog, 1);
    sender.wallet_cli_dis->expected_code.check.emplace(us::wallet::wallet::local_api::push_txlog,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            assert(tid == trade_id);
            us::wallet::wallet::index_t txnd;
            assert(is_ok(blob_reader_t::parse(blob, txnd)));
            txnd.dump("sender-txlog-index-#0> ", cout);
            assert(txnd.size() == 1);
            assert(txnd.begin()->first == track);
            assert(txnd.begin()->second.wallet_track_status == us::wallet::wallet::wts_cancelled);
        }
    );

    rcpt.wallet_cli_dis->expected_code.emplace(us::wallet::wallet::local_api::push_txlog, 1);
    rcpt.wallet_cli_dis->expected_code.check.emplace(us::wallet::wallet::local_api::push_txlog,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            assert(tid == trade_id);
            us::wallet::wallet::index_t txnd;
            assert(is_ok(blob_reader_t::parse(blob, txnd)));
            txnd.dump("rcpt-txlog-index-#0> ", cout);
            assert(txnd.size() == 1);
            assert(txnd.begin()->first == track);
            assert(txnd.begin()->second.wallet_track_status == us::wallet::wallet::wts_cancelled);
        }
    );

    ostringstream cmd;
    cmd << "cancel " << track;
    auto r = sender.wallet_cli->rpc_daemon->get_peer().call_exec_trade(us::wallet::cli::rpc_peer_t::exec_trade_in_t(trade_id, cmd.str()));
    if (is_ko(r)) {
        cout << sender.wallet_cli->rewrite(r) << '\n';
        assert(false);
        exit(1);
    }
    wait(sender, rcpt);
}

void c::cancel_transfer_by_sender2(node& sender, node& rcpt, track_t track) {
    assert(track != 0);
    sender.wallet_cli_dis->expected_code.emplace(us::wallet::wallet::local_api::push_txlog, 1);
    sender.wallet_cli_dis->expected_code.check.emplace(us::wallet::wallet::local_api::push_txlog,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            assert(tid == trade_id);
            us::wallet::wallet::index_t txnd;
            assert(is_ok(blob_reader_t::parse(blob, txnd)));
            static int seq = 0;
            ostringstream pfx;
            pfx << "sender-txlog-index-#" << seq << "> ";
            txnd.dump(pfx.str(), cout);
            assert(txnd.size() == 0);
        }
    );

    rcpt.wallet_cli_dis->expected_code.emplace(us::wallet::wallet::local_api::push_txlog, 1);
    rcpt.wallet_cli_dis->expected_code.check.emplace(us::wallet::wallet::local_api::push_txlog,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            assert(tid == trade_id);
            us::wallet::wallet::index_t txnd;
            assert(is_ok(blob_reader_t::parse(blob, txnd)));
            txnd.dump("rcpt-txlog-index-#0> ", cout);
            assert(txnd.size() == 0);
        }
    );

    ostringstream cmd;
    cmd << "cancel " << track;
    auto r = sender.wallet_cli->rpc_daemon->get_peer().call_exec_trade(us::wallet::cli::rpc_peer_t::exec_trade_in_t(trade_id, cmd.str()));
    if (is_ko(r)) {
        cout << sender.wallet_cli->rewrite(r) << '\n';
        assert(false);
        exit(1);
    }
    wait(sender, rcpt);
}

void c::confirm_transfer(node& sender, node& rcpt, track_t track) {
    //transfer gas
    sender.wallet_cli_dis->expected_code.emplace(us::wallet::wallet::local_api::push_txlog, 1);
    sender.wallet_cli_dis->expected_code.check.emplace(us::wallet::wallet::local_api::push_txlog,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            assert(tid == trade_id);
            us::wallet::wallet::index_t txnd;
            assert(is_ok(blob_reader_t::parse(blob, txnd)));
            static int seq = 0;
            ostringstream pfx;
            pfx << "sender-txlog-index-#" << seq << "> ";
            txnd.dump(pfx.str(), cout);
            assert(txnd.size() == 1);
            if (seq == 0) {
                track = txnd.begin()->first;
                cout << "track " << track;
                assert(txnd.begin()->second.wallet_track_status == us::wallet::wallet::wts_wait_rcpt_info);
            }
            else if (seq == 1) {
                assert(txnd.begin()->first == track);
                assert(txnd.begin()->second.wallet_track_status == us::wallet::wallet::wts_wait_signature);
                seq = -1;
            }
            ++seq;
        }
    );

    rcpt.wallet_cli_dis->expected_code.emplace(us::wallet::wallet::local_api::push_txlog, 1);
    rcpt.wallet_cli_dis->expected_code.check.emplace(us::wallet::wallet::local_api::push_txlog,
        [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
            assert(tid == trade_id);
            us::wallet::wallet::index_t txnd;
            assert(is_ok(blob_reader_t::parse(blob, txnd)));
            txnd.dump("rcpt-txlog-index-#0> ", cout);
            assert(txnd.size() == 1);
            assert(txnd.begin()->first == track);
            assert(txnd.begin()->second.wallet_track_status == us::wallet::wallet::wts_wait_signature);
        }
    );

    ostringstream cmd;
    cmd << "pay " << track;
    auto r = sender.wallet_cli->rpc_daemon->get_peer().call_exec_trade(us::wallet::cli::rpc_peer_t::exec_trade_in_t(trade_id, cmd.str()));
    if (is_ko(r)) {
        cout << sender.wallet_cli->rewrite(r) << '\n';
        assert(false);
        exit(1);
    }
    wait(sender, rcpt);
}

void c::test(node& w1, node& w2) {
    using hash_t = us::gov::crypto::ripemd160::value_type;
    cout << "w1 gov: ";
    w1.gov_cli->exec("id");
    cout << "w2 gov: ";
    w2.gov_cli->exec("id");
    cout << "w1 wallet: ";
    w1.wallet_cli->exec("id");
    cout << "w1 wallet: ";
    w2.wallet_cli->exec("id");
    cout << "w1 node wallet port: " << w1.wport << endl;
    cout << "w2 node wallet port: " << w2.wport << endl;

    cout << "w2 QR's" << endl;
//    w2.wallet_cli->exec("trade qr");

    using bookmarks_t = us::wallet::trader::bookmarks_t;
    {
        bookmarks_t b;
        auto r = w1.wallet_cli->rpc_daemon->get_peer().call_qr(b);
        if (is_ko(r)) {
            cout << r << '\n';
            assert(false);
            exit(1);
        }
        b.dump("", cout);

        auto i = b.find_protocol_role("w2w", "w");
        assert(i != b.end());
        auto& qr = i->second.qr;
        qr.dump("w1 bank qr: ", cout);

        assert(w1.wallet_cli_dis != nullptr);
        assert(w2.wallet_cli_dis != nullptr);
        w2.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_trade, 1);
        w2.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 3);
        w2.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
            [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
                string payload;
                assert(is_ok(blob_reader_t::parse(blob, payload)));
                static int seq = 0;
                cout << "seq " << seq << endl;
//                Check_s_contains(payload, "initiator Y");
                if (seq == 0 && payload.find("state online") != string::npos) {
                    w2.wallet_cli_dis->expected_code.decrement(us::wallet::trader::trader_t::push_data);
                    ++seq;
                }
                if (seq == 0) {
                    Check_s_contains(payload, "state connected");
                    Check_s_contains(payload, "protocol not set");
                }
                else if (seq == 1) {
                    Check_s_contains(payload, "state online");
                    Check_s_contains(payload, "protocol w2w w");
                }
                else if (seq == 2) {
                    cout << "EXTRA DATA arrived. REVIEW why" << endl;
                }
                else {
                    Fail();
                }
                ++seq;
                
            }
        );

        w1.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_trade, 1);
        w1.wallet_cli_dis->expected_code.emplace(us::wallet::trader::trader_t::push_data, 2);
        w1.wallet_cli_dis->expected_code.check.emplace(us::wallet::trader::trader_t::push_data,
            [&](const hash_t& tid, uint16_t code, const vector<uint8_t>& blob) {
                string payload;
                assert(is_ok(blob_reader_t::parse(blob, payload)));
                static int seq = 0;
                cout << "seq " << seq << endl;
//                cout << payload << endl;
                if (seq == 0) {
                    Check_s_contains(payload, "initiator N");
                    Check_s_contains(payload, "protocol w2w w");
                }
                else if (seq == 1) {
                    cout << "------------" << endl;
                }
                else if (seq == 2) {
                    cout << "------------" << endl;
                    Fail();
                }
                else {
                    Fail();
                }
                ++seq;
                
            }
        );

        log("start trade on w2 -> w1");
        {
            string datasubdir;
            blob_t blob;
            qr.write(blob);
            auto r = w2.wallet_cli->rpc_daemon->get_peer().call_trade(us::wallet::cli::rpc_peer_t::trade_in_t(hash_t(0), datasubdir, blob), trade_id);
            if (is_ko(r)) {
                auto s = w2.wallet_cli->rpc_daemon->get_peer().lasterror;
                if (s.empty()) {
                    cout << r << '\n';
                }
                else {
                    cout << s << '\n';
                }
                assert(false);
                exit(1);
            }
        }
        wait(w1, w2);
    }
    cout << "trade id: " << trade_id << '\n';
    w1.wallet->daemon->traders.dump("traders w1> ", cout);
    cout << endl;
    w2.wallet->daemon->traders.dump("traders w2> ", cout);
    cout << endl;

    {
        auto track = init_transfer(w2, w1);
        cout << "Transfer " << track << " waiting for sender confirmation" << endl;

        cancel_transfer_by_sender1(w2, w1, track);
        cancel_transfer_by_sender2(w2, w1, track);
    }
    {
        cout << "Init new transfer" << endl;
        auto track = init_transfer(w2, w1);
        cout << "Transfer " << track << " waiting for sender confirmation" << endl;
        cout << "Confirming transfer" << endl;
        confirm_transfer(w2, w1, track);
    }
cout << "HERE" << endl;
exit(1);
}

void c::run() {
    assert(n.find("bank1") != n.end());
    assert(n.find("bank2") != n.end());
    auto& n1 = *n.find("bank1")->second;
    auto& n2 = *n.find("bank2")->second;
    test_r2r_cfg(n1, n2, [&](node& n1, node& n2) { test(n1, n2); }, trade_id);
}

