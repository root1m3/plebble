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
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <cassert>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <unordered_set>
#include <unistd.h>
#include <fstream>
#include <filesystem>
#include <random>
#include <limits>

#include <us/gov/config.h>
#include <us/gov/types.h>
#include <us/gov/crypto/ec.h>
#include <us/gov/crypto/ripemd160.h>
#include <us/gov/io/cfg0.h>
#include <us/gov/io/cfg1.h>
#include <us/gov/io/cfg.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/peer/peer_t.h>
#include <us/gov/engine/daemon_t.h>
#include <us/gov/engine/local_deltas_t.h>
#include <us/gov/engine/net_daemon_t.h>
#include <us/gov/engine/peer_t.h>
#include <us/gov/engine/db_t.h>
#include <us/gov/engine/auth/app.h>
#include <us/gov/engine/auth/local_delta.h>
#include <us/gov/engine/auth/collusion_control_t.h>
#include <us/gov/engine/auth/delta.h>
#include <us/gov/cash/tx_t.h>
#include <us/gov/cash/app.h>
#include <us/gov/cash/local_delta.h>
#include <us/gov/sys/app.h>
#include <us/gov/traders/app.h>
#include <us/gov/cli/rpc_daemon_t.h>
#include <us/gov/cli/rpc_peer_t.h>
#include <us/gov/cli/hmi.h>

#include <us/wallet/wallet/local_api.h>
#include <us/wallet/cli/hmi.h>
#include <us/wallet/tee.h>

#include "test_platform.h"
#include "encryption.h"
#include "main.h"
#include "network.h"
#include "w2w_t.h"
#include "node_bank.h"

#define loglevel "test"
#define logclass "main"
#include "logs.inc"

#include "assert.inc"

namespace us { namespace test {

using namespace std;
using namespace chrono;
using namespace us::gov;
using namespace us;
using sighash_t = us::gov::crypto::sha256::value_type;
using hash_t = us::gov::crypto::ripemd160::value_type;
using pub_t = us::gov::crypto::ec::keys::pub_t;
using priv_t = us::gov::crypto::ec::keys::priv_t;
using cash_t = us::gov::cash_t;
namespace fs = std::filesystem;

bool batch = false;


void check_accounts() {
    using accounts_t = us::gov::cash::accounts_t;
    using account_t = us::gov::cash::account_t;
    accounts_t o;
    account_t a;
    assert(a.locking_program == us::gov::cash::app::locking_program_id);
    o.emplace(hash_t(1), a);

    blob_t blob;
    o.write(blob);

    accounts_t o2;
    assert(o2.read(blob) == ok);

    assert(o2.size() == 1);
    assert(o2.begin()->first == hash_t(1));
    assert(o2.begin()->second.locking_program == us::gov::cash::app::locking_program_id);
}

struct tengine: us::gov::engine::daemon_t, us::test::test_platform {
    using b = us::gov::engine::daemon_t;
    using t = us::test::test_platform;
    using datagram = us::gov::socket::datagram;

    static constexpr int threadpool_size{1};
    static constexpr int edges{1};
    static constexpr int govport{22222};
    static constexpr const char* govaddr{"127.0.0.1"};

    tengine(const string& home, uint16_t port, ostream& os): b(123, crypto::ec::keys::generate(), home, port, port, edges, 1, threadpool_size, vector<pair<uint32_t, uint16_t>>(), home + "/status"), t(os) {
        sysop_allowed = true;
    }
    ~tengine() override {
    }

    void acase() { //gov::io::blob_reader_t::current_version
        cout << "V7 diff sys app" << endl; // proc1 VX where X=prev char
        //proc1. leave this value of s, execute test to obtain the new value

         string s = /*v8*/ "EopTPw3SmupNkf6Dsaqx6zhnsM59cZVrjRMZhMm7rY8Fvgbpru9KYZF3rRTkaYJF9aTziwz8gJAsgu2GcMtq5pTztcLh1cqskznXPge5pP5jTVoQWXT9SiT42hAVUipesRPo7KUUCJmw6HMxhnP";
//       History:
//        string s = /*v7*/ "vbdLr3mC5fY6VzR7Edt86y5Uv4bJJkCuQKWPx1ZS1uhpLJpuCBKaT3NZv6c93CqAL2XhxLriHzKtr3HFcAd9VXCa2ccmVU2tZNSs3d7fE5g7P3DpLexxr5s7gsHyJY2sbiAe3biCbSregy6EhxxX";
//        Never released        string s = "6 ocu5W22cciHEZoDCmfmtvVc7Z1y1p2Y1uDZxQ5yEkND8 AN1rKvtaooWV4rY9FvXB8Rr6b2m9oSuiQz5wEGxtyuYAQMd7zvUSSVLDEwnRjDcMvuzDqmiMqqJWn4sF4xKDLWkwVAPLsmweD 1545565800000000000 3 1 2p5DodGLZg6wbYRnAHPn8CfARWpc 20 4 3gaEPErNicpAyBxqrpMRoXcZFou9 1822982076 16672 17888 8Pg6QViJtuxBuFpdXcdfzxGeDGr 2661973948 16672 17888 GutkKVYN1Q6n75XHdhtX4pvCWFi 953821266 16672 17888 4XbkvPxxMQjC3g9Z8o119H3oLeJ7 3961463889 16672 17888 30 1 2pCcSDYA1Zt5gY4UD25vRZnhmJ2G 11111111111111112UzHM 567 2 a b a2 b2 2 2pCcSDYA1Zt5gY4UD25vRZnhmJ2G / 101 cdCSmHMvyYemaAw5Sm3u7Aw7z75 / 202 0 123 ";
//        string s="5 ocu5W22cciHEZoDCmfmtvVc7Z1y1p2Y1uDZxQ5yEkND8 AN1rKvtaooWV4rY9FvXB8Rr6b2m9oSuiQz5wEGxtyuYAQMd7zvUSSVLDEwnRjDcMvuzDqmiMqqJWn4sF4xKDLWkwVAPLsmweD 1545565800000000000 3 1 2p5DodGLZg6wbYRnAHPn8CfARWpc 20 4 3gaEPErNicpAyBxqrpMRoXcZFou9 1822982076 16672 17888 8Pg6QViJtuxBuFpdXcdfzxGeDGr 2661973948 16672 17888 GutkKVYN1Q6n75XHdhtX4pvCWFi 953821266 16672 17888 4XbkvPxxMQjC3g9Z8o119H3oLeJ7 3961463889 16672 17888 0 30 1 2pCcSDYA1Zt5gY4UD25vRZnhmJ2G 11111111111111112UzHM 567 2 a b a2 b2 2 2pCcSDYA1Zt5gY4UD25vRZnhmJ2G / 101 cdCSmHMvyYemaAw5Sm3u7Aw7z75 / 202 0 123 ";
//        string s="4 ocu5W22cciHEZoDCmfmtvVc7Z1y1p2Y1uDZxQ5yEkND8 AN1rKvtaooWV4rY9FvXB8Rr6b2m9oSuiQz5wEGxtyuYAQMd7zvUSSVLDEwnRjDcMvuzDqmiMqqJWn4sF4xKDLWkwVAPLsmweD 1545565800000000000 3 1 2p5DodGLZg6wbYRnAHPn8CfARWpc 50 5866 0.8 0 258 16720 162 0.5 0 0.4 10 11 953821266 16672 2661973948 16672 1822982076 16672 3961463889 16672 1569376443 16672 3582345221 16672 3582345221 16672 3582345221 16672 3971129167 16672 1720009656 16672 1039151964 16672 1 6 400 0 0 0 0 404 0 0 0 0 408 0 0 0 0 800 12 428 15 536 804 15 2674 12 2148 808 12 1796 15 2254 20 4 3gaEPErNicpAyBxqrpMRoXcZFou9 1822982076 16672 17888 8Pg6QViJtuxBuFpdXcdfzxGeDGr 2661973948 16672 17888 GutkKVYN1Q6n75XHdhtX4pvCWFi 953821266 16672 17888 4XbkvPxxMQjC3g9Z8o119H3oLeJ7 3961463889 16672 17888 0 30 1 2pCcSDYA1Zt5gY4UD25vRZnhmJ2G 11111111111111112UzHM 567 2 a b a2 b2 2 2pCcSDYA1Zt5gY4UD25vRZnhmJ2G / 101 cdCSmHMvyYemaAw5Sm3u7Aw7z75 / 202 0 123 ";
//        string s="3 ocu5W22cciHEZoDCmfmtvVc7Z1y1p2Y1uDZxQ5yEkND8 AN1rKvtaooWV4rY9FvXB8Rr6b2m9oSuiQz5wEGxtyuYAQMd7zvUSSVLDEwnRjDcMvuzDqmiMqqJWn4sF4xKDLWkwVAPLsmweD 1545565800000000000 3 1 2p5DodGLZg6wbYRnAHPn8CfARWpc 50 5866 0.8 0 258 16720 162 0.5 0 0.4 10 11 953821266 16672 2661973948 16672 1822982076 16672 3961463889 16672 1569376443 16672 3582345221 16672 3582345221 16672 3582345221 16672 3971129167 16672 1720009656 16672 1039151964 16672 1 6 400 0 0 0 0 404 0 0 0 0 408 0 0 0 0 800 12 428 15 536 804 15 2674 12 2148 808 12 1796 15 2254 20 4 3gaEPErNicpAyBxqrpMRoXcZFou9 1822982076 16672 17888 8Pg6QViJtuxBuFpdXcdfzxGeDGr 2661973948 16672 17888 GutkKVYN1Q6n75XHdhtX4pvCWFi 953821266 16672 17888 4XbkvPxxMQjC3g9Z8o119H3oLeJ7 3961463889 16672 17888 0 30 1 2pCcSDYA1Zt5gY4UD25vRZnhmJ2G 11111111111111112UzHM 567 2 a b a2 b2 2 2pCcSDYA1Zt5gY4UD25vRZnhmJ2G 101 cdCSmHMvyYemaAw5Sm3u7Aw7z75 202 0 123 ";
//        string s="2 ocu5W22cciHEZoDCmfmtvVc7Z1y1p2Y1uDZxQ5yEkND8 AN1rKvtaooWV4rY9FvXB8Rr6b2m9oSuiQz5wEGxtyuYAQMd7zvUSSVLDEwnRjDcMvuzDqmiMqqJWn4sF4xKDLWkwVAPLsmweD 1545565800000000000 3 1 2p5DodGLZg6wbYRnAHPn8CfARWpc 50 5866 0.8 0 258 16720 162 0.5 0 0.4 10 11 953821266 16672 2661973948 16672 1822982076 16672 3961463889 16672 1569376443 16672 3582345221 16672 3582345221 16672 3582345221 16672 3971129167 16672 1720009656 16672 1039151964 16672 1 6 400 0 0 0 0 404 0 0 0 0 408 0 0 0 0 800 12 428 15 536 804 15 2674 12 2148 808 12 1796 15 2254 20 4 3gaEPErNicpAyBxqrpMRoXcZFou9 1822982076 16672 17888 8Pg6QViJtuxBuFpdXcdfzxGeDGr 2661973948 16672 17888 GutkKVYN1Q6n75XHdhtX4pvCWFi 953821266 16672 17888 4XbkvPxxMQjC3g9Z8o119H3oLeJ7 3961463889 16672 17888 0 30 1 2pCcSDYA1Zt5gY4UD25vRZnhmJ2G 11111111111111112UzHM 567 2 a b a2 b2 2 2pCcSDYA1Zt5gY4UD25vRZnhmJ2G 101 cdCSmHMvyYemaAw5Sm3u7Aw7z75 202 0 123 ";
//        string s="1 ocu5W22cciHEZoDCmfmtvVc7Z1y1p2Y1uDZxQ5yEkND8 AN1rKvtaooWV4rY9FvXB8Rr6b2m9oSuiQz5wEGxtyuYAQMd7zvUSSVLDEwnRjDcMvuzDqmiMqqJWn4sF4xKDLWkwVAPLsmweD 1545565800000000000 3 1 2p5DodGLZg6wbYRnAHPn8CfARWpc 50 5866 0.8 0 258 16720 162 0.5 0 0.4 10 11 953821266 16672 2661973948 16672 1822982076 16672 3961463889 16672 1569376443 16672 3582345221 16672 3582345221 16672 3582345221 16672 3971129167 16672 1720009656 16672 1039151964 16672 1 6 400 0 0 0 0 404 0 0 0 0 408 0 0 0 0 800 12 428 15 536 804 15 2674 12 2148 808 12 1796 15 2254 20 4 3gaEPErNicpAyBxqrpMRoXcZFou9 1822982076 16672 17888 8Pg6QViJtuxBuFpdXcdfzxGeDGr 2661973948 16672 17888 GutkKVYN1Q6n75XHdhtX4pvCWFi 953821266 16672 17888 4XbkvPxxMQjC3g9Z8o119H3oLeJ7 3961463889 16672 17888 0 30 1 2pCcSDYA1Zt5gY4UD25vRZnhmJ2G 11111111111111112UzHM 567 2 a b a2 b2 2 2pCcSDYA1Zt5gY4UD25vRZnhmJ2G 101 cdCSmHMvyYemaAw5Sm3u7Aw7z75 202 123 ";
//        string s="0 ocu5W22cciHEZoDCmfmtvVc7Z1y1p2Y1uDZxQ5yEkND8 AN1rKvtaooWV4rY9FvXB8Rr6b2m9oSuiQz5wEGxtyuYAQMd7zvUSSVLDEwnRjDcMvuzDqmiMqqJWn4sF4xKDLWkwVAPLsmweD 1545565800000000000 3 1 2p5DodGLZg6wbYRnAHPn8CfARWpc 6bfee5f17ef45e36c9d2bcac18f5278a71040190 5ZnmQCJyBmX1GiRkEcTbfkWypmR 50 5866 0.8 0 258 16720 6 400 0 0 0 0 404 0 0 0 0 408 0 0 0 0 800 12 428 15 536 804 15 2674 12 2148 808 12 1796 15 2254 162 0.5 0 0.4 10 11 953821266 2661973948 1822982076 3961463889 1569376443 3582345221 3582345221 3582345221 3971129167 1720009656 1039151964 1 20 4 3gaEPErNicpAyBxqrpMRoXcZFou9 1822982076 17888 8Pg6QViJtuxBuFpdXcdfzxGeDGr 2661973948 17888 GutkKVYN1Q6n75XHdhtX4pvCWFi 953821266 17888 4XbkvPxxMQjC3g9Z8o119H3oLeJ7 3961463889 17888 0 30 1 2pCcSDYA1Zt5gY4UD25vRZnhmJ2G 11111111111111112UzHM 567 2 a b a2 b2 2 2pCcSDYA1Zt5gY4UD25vRZnhmJ2G 101 cdCSmHMvyYemaAw5Sm3u7Aw7z75 202 123 ";
//        string s="1 ocu5W22cciHEZoDCmfmtvVc7Z1y1p2Y1uDZxQ5yEkND8 AN1rKvtaooWV4rY9FvXB8Rr6b2m9oSuiQz5wEGxtyuYAQMd7zvUSSVLDEwnRjDcMvuzDqmiMqqJWn4sF4xKDLWkwVAPLsmweD 1545565800000000000 3 1 2p5DodGLZg6wbYRnAHPn8CfARWpc 6bfee5f17ef45e36c9d2bcac18f5278a71040190 5ZnmQCJyBmX1GiRkEcTbfkWypmR 50 5866 0.8 0 258 16720 6 400 0 0 0 0 404 0 0 0 0 408 0 0 0 0 800 12 428 15 536 804 15 2674 12 2148 808 12 1796 15 2254 162 0.5 0 0.4 10 11 953821266 2661973948 1822982076 3961463889 1569376443 3582345221 3582345221 3582345221 3971129167 1720009656 1039151964 1 20 4 3gaEPErNicpAyBxqrpMRoXcZFou9 1822982076 17888 8Pg6QViJtuxBuFpdXcdfzxGeDGr 2661973948 17888 GutkKVYN1Q6n75XHdhtX4pvCWFi 953821266 17888 4XbkvPxxMQjC3g9Z8o119H3oLeJ7 3961463889 17888 0 30 0 0 ";
//        string s="0 ocu5W22cciHEZoDCmfmtvVc7Z1y1p2Y1uDZxQ5yEkND8 AN1rKvtaooWV4rY9FvXB8Rr6b2m9oSuiQz5wEGxtyuYAQMd7zvUSSVLDEwnRjDcMvuzDqmiMqqJWn4sF4xKDLWkwVAPLsmweD 1545565800000000000 3 1 2p5DodGLZg6wbYRnAHPn8CfARWpc 6bfee5f17ef45e36c9d2bcac18f5278a71040190 5ZnmQCJyBmX1GiRkEcTbfkWypmR 50 5866 0.8 0 258 16720 6 400 0 0 0 0 404 0 0 0 0 408 0 0 0 0 800 12 428 15 536 804 15 2674 12 2148 808 12 1796 15 2254 162 0.5 0 0.4 10 11 953821266 2661973948 1822982076 3961463889 1569376443 3582345221 3582345221 3582345221 3971129167 1720009656 1039151964 1 20 4 3gaEPErNicpAyBxqrpMRoXcZFou9 1822982076 17888 8Pg6QViJtuxBuFpdXcdfzxGeDGr 2661973948 17888 GutkKVYN1Q6n75XHdhtX4pvCWFi 953821266 17888 4XbkvPxxMQjC3g9Z8o119H3oLeJ7 3961463889 17888 0 30 0 0 ;

        auto g = new us::gov::engine::local_deltas_t();
        auto r = g->read(s);
        if (is_ko(r)) {
            cout << r << endl;
        }
        assert(is_ok(r));
        assert(g != nullptr);

        /* Code used to create the blob
        auto& r=dynamic_cast<us::gov::cash::app::local_delta&>(*g->find(us::gov::cash::app::id())->second);
        r.fees=123;
        us::gov::cash::hash_t addr("2pCcSDYA1Zt5gY4UD25vRZnhmJ2G");
        us::gov::cash::hash_t addr2("cdCSmHMvyYemaAw5Sm3u7Aw7z75");
        us::gov::cash::app::local_delta::safe_deposit_box box(567);
        check(box.value,(us::gov::cash::cash_t)567);
        { bool b=box.store("a", "b"); check(b,true); }
        { bool b=box.store("a2", "b2");  check(b,true); }
        { bool b=box.store(addr, 101);  check(b,true); }
        { bool b=box.store(addr2, 202);  check(b,true); }
        r.accounts.emplace(addr,us::gov::cash::app::local_delta::account_t(1,box));
        */

        blob_t blob;
        g->write(blob);
        auto s2 = us::gov::crypto::b58::encode(blob);

        assert(this == &db->sys_app->demon);
        if (s != s2) {
            cout << "acase V8:\n"; // proc1 VX, X=new char
            cout << "----------------------proc1 -1-------------------\n";
            cout << "PREV        string s = \"" << s << "\";\n";
            cout << "NEW         string s = \"" << s2 << "\";\n";
            cout << "-/--------------------proc1 -1-------------------\n";
            cout << "you updated gov::io::blob_reader_t::current_version to " << +gov::io::blob_reader_t::current_version << ", replace the previous definition of 's' above in this function.\n";
        }
        check(s2, s);
        delete g;
    }

    void serialization_local_deltas() {
        acase();
        string prev;
        {
            auto* ld = create_local_deltas(0);
            blob_t out;
            ld->write(out);
            prev = us::gov::crypto::b58::encode(out);
            cout << "----------------------proc1 -2-------------------\n";
            cout << "V8 serialization_local_deltas. Line for future V9\n";
            cout << "        prev=/" << "*v8*" << "/\"" << prev << "\";\n"; //proc1. VX, X=new vchar
            cout << "-/--------------------proc1 -2-------------------\n";
            delete ld;
        }

        //uncomment on v9
        //prev=/*v8*/"75grLJGBJemo382RE318pJRkkny6iLGduSRGgP7ez9aJusFao5nLQBCMRSDSrPekk2QFyY7mfUkoaZScWafQQoZrTVEqdCW75SCga8sd64ZpAFHd4ngis4SuSZerujjoGZCqmqamQ9uLRTbrjGMFQReQUy2GCYv4uCxnKNtC5giZjCChZQBnkamRao8tZKNZ7Tu";
        prev=/*v7*/"Qc4fRa6F55ohCjrtYCKpxpT7gy7XxqKGQ14BBZRHc5aJEYRzxPa75PENmfnJ2Tb2AWBhEXgoYFsZEC1VrhAvYqcFsoECYdyXH5QQ48Vja7NdZidMBHLh9WxfP6AWzRFKgn4nwa4h1j8TCtA9trRdogM12k61k9UZ5uhk77xr23FrFvmJXBkFnXL3Mn5FkgHpjHm9";

        //prev=/*v6*/"TqgkQxoSWtYCJ3Hbn1tGq63upPMd2zU8bgZGzdqx1jJxFsNRrHqHrMbgaVddAdyP3njjNTRE8fTQSrSuCmK7jtNizES7afuowQZMZBdkkjbMJK1R1QLF3o1cYcZ92VvmB4jy9yGNCrS7AmEKSDfhipyQP4DYxAd8TciNRLT2HJPZ2y3PGWxeNdkAWvNNqAb2Qz95";
        //prev="5 d4yp7kjDKiQN12jbbp4saFotg4eAFJvmjXSHjr8Hz1DM 381yXZRuczk7rgy6MqVZYFHXYHk4ridp3Tv7iBe2uurFfCLqDPH1wmdrctCXtd7qJ93X34MEZbcJEmm5RxmYHyc5cS5P8qVk 0 3 1 tDKRyCNBzMgKQRe12ETU6wMkq7u 20 0 0 30 0 0 ";
        //prev="4 ovECWdFFZDjkc3rCmp2bbnGifWK3b9PsAoxbfG5MqXG7 AN1rKvszW4AKKf33nMzR6RYVssKyo9QqdJ6PqbwixYPP25e28KMWqFWgZt18wRXBnAzdvuMigBaXuomEWhnK5sYd25XbtdsRT 0 3 1 zAFwd5FAHPdTPRw66jBV2i2Vcxo 56 6000 1.26 15046 1828 1729619 5 0 0 0 1 0 0 20 400 0 0 0 0 404 0 0 0 0 408 0 0 0 0 800 0 0 0 0 804 0 0 0 0 808 0 0 0 0 1600 0 0 0 0 1604 0 0 0 0 2002 0 0 0 0 2010 0 0 0 0 2400 0 0 0 0 2404 0 0 0 0 2408 0 0 0 0 2412 0 0 0 0 2416 0 0 0 0 2422 0 0 0 0 2430 0 0 0 0 2438 0 0 0 0 2445 0 0 0 0 2450 0 0 0 0 20 0 0 30 0 0 ";
        //prev="3 29he43ZqvXKkeMskj86aAxouNzJ6jKsaVBd1q1VK2ptte AN1rKvtDjfpV6gYfaeLpgTFbyxVpvkyaZQV4iWM6rh2rAh5wB3Hdhb9nYUYeQydWqPjVHWCHowrYUx2KqPs8vnSkp4hMHopGe 0 3 1 z6ACBXViF1JF3rxFTnz9i6tAEZn 56 6000 1.14 15046 1868 1729622 2 0 0 0 1 0 0 20 400 0 0 0 0 404 0 0 0 0 408 0 0 0 0 800 0 0 0 0 804 0 0 0 0 808 0 0 0 0 1600 0 0 0 0 1604 0 0 0 0 2002 0 0 0 0 2010 0 0 0 0 2400 0 0 0 0 2404 0 0 0 0 2408 0 0 0 0 2412 0 0 0 0 2416 0 0 0 0 2422 0 0 0 0 2430 0 0 0 0 2438 0 0 0 0 2445 0 0 0 0 2450 0 0 0 0 20 0 0 30 0 0 ";
        //prev="2 e4itM5QhQHBnEWFHPCdDjRStHVuiUh1PmzADxJhXNmMN AN1rKvtdweFejZeQHHiQbrrtAsJQDqTVyEct3htgMLoF26QgiDdrVXewgEqTHYb8dvjXY1nYfgfqPDLSqnA7mJssULkP6oFE6 0 3 1 3gB2B2VNFxZ8i8McLK3DoyZoSR8Y 52 6186 0.23 8065 526 57384 3 0.1 0 3.0914e-41 1 0 0 15 400 0 0 0 0 404 0 0 0 0 408 0 0 0 0 800 0 0 0 0 804 0 0 0 0 808 0 0 0 0 2002 0 0 0 0 2010 0 0 0 0 2400 0 0 0 0 2404 0 0 0 0 2408 0 0 0 0 2412 0 0 0 0 2416 0 0 0 0 2422 0 0 0 0 2430 0 0 0 0 20 0 0 30 0 0 ";
        //prev="1 z2NfEAsAE1tYQ3pshjdZd4CGYL2ZSxb6PiEZknXR1mY1 AN1rKvtUetynmJtZqqRpS5jhc9c8Y2EgDN1ZxDw1kzjwpQAoVZ294VgcNpjDD4fv9PDRmFewD8VZynz9UTAyNeYtf9DERgfxT 0 3 1 2R7SAm2T2z5FHVZmQM1Y9ifvYhQ3 52 6185 1.52 4549 124 85715 32.6 0.1 1.1 2 1 0 0 15 400 0 0 0 0 404 0 0 0 0 408 0 0 0 0 800 0 0 0 0 804 0 0 0 0 808 0 0 0 0 2002 0 0 0 0 2010 0 0 0 0 2400 0 0 0 0 2404 0 0 0 0 2408 0 0 0 0 2412 0 0 0 0 2416 0 0 0 0 2422 0 0 0 0 2430 0 0 0 0 20 0 0 30 0 0 ";
        //proc1 - Obtain new value of prev executing tests before the change
        //prev="0 sYGkCGUBtr6vqgb3ApjjpyPL5g8NYVaUDK3UYPhRgXYn 381yXZCKwJTDVYV4ejL8Y8foPmD5foqwBzF7Eo41B3qiCd9B8NZ93XxVq7vnCrmJ7Zdk3DeqqGFRVgFY31FXEW6KqYtE7rsB 0 3 1 3dgQhcL4dhmQudnnUWLiEL762UTJ a419aee9cf7a9e86a04079f5bcbf01d8eca4ab57 3ghau5rkkFbMkCe8QCifmsx9YUSF 52 6185 5.7 3271 694 138442 15 400 0 0 0 0 404 0 0 0 0 408 0 0 0 0 800 0 0 0 0 804 0 0 0 0 808 0 0 0 0 2002 0 0 0 0 2010 0 0 0 0 2400 0 0 0 0 2404 0 0 0 0 2408 0 0 0 0 2412 0 0 0 0 2416 0 0 0 0 2422 0 0 0 0 2430 0 0 0 0 0 0 0 0.1 1 0 0 20 0 0 30 0 0 ";
        //prev="1 bknE2yRAsMYYqi8Mpz2VDVL2BQpoacTJhztuNH1vVbUu 381yXZWAVnJy3bvkHp9cHnQWa6VoLq35QT4SJQKz4YhmoR4upryrJ3zECtHZsM9Vu262KJTGbFRx4MaNWwTVp6K61H7WmT2K 0 3 1 4LYmm8RJQYQAaiWnU574Q23sXREJ 04bb394d88519cf336fbf0cc2d0896c54eed646e 3HEj7hnJhT7PbiaUjDQj6TLNx5Hm 52 6185 1.41 1237 435 145533 6 400 0 0 0 0 404 0 0 0 0 408 0 0 0 0 800 0 0 0 0 804 0 0 0 0 808 0 0 0 0 0 0.1 0 0 1 0 1 20 0 0 30 0 0 ";
        //prev="0 gyJV3HWobi5naUegrKxPcDcyRAz8PURfHpRjiAQoo5HG 381yXZUQgCUPT8M1XxA3Ss8MVYtfHTkT6HmmZ3GKp6v8tgDZkTU6XJukLFKUpYJh7RyahQB8SFkJFrdcyC3ZpPJsPSc1icSV 0 3 1 2oDPP3nWogQ2dQd7wDY7XLNGKH7k 826e25eded05b5af1b4a658e93a3903ba9214bfe 2fXeh8zDNJrKbjWAjLjZ5uRSsSEQ 52 6185 1.09 461 369 147314 6 400 0 0 0 0 404 0 0 0 0 408 0 0 0 0 800 0 0 0 0 804 0 0 0 0 808 0 0 0 0 0 0.1 0 0 1 0 1 20 0 0 30 0 0 ";
        //prev="4 25A1KZ8wmLssH1eHcYqcnrSrMnKnAqWeEbRMCHo2nyWK3 AN1rKvszcWZmxkhZgc1LC2bVPGs1qQnBD2mgKNL3DmPFps54wpZ2RyGPvbciX72bi2FJBcVieUsz1tgqgonJLLfHEXGUJbHyC 0 3 1 4Htbk7pZkG2nNtD1Cdgxq6vfMkqS f0b2a5cb8ad5409194f6fd105f2ebd5c88cbb8a3 2DrCcbyA9JPe9aTTCVcoFap61m5t 52 6185 1.16 3095 509 147883 6 400 0 0 0 0 404 0 0 0 0 408 0 0 0 0 800 0 0 0 0 804 0 0 0 0 808 0 0 0 0 0 0.1 0 2.41624e+08 1 0 1 20 0 0 30 0 0 ";
        //prev="3 yQXZpmn3ybnqnDt145TpPsAY2U7hAbTZPHsxUnyGmQTT AN1rKvthvXpojE9EWi2N7oaqKGq95nr8dWg2BrKUMQ6S8FtwZ2zzJzk6qGVXTfN4gCBEupNyEoe3tjCtf4fCZRGb5ckDAUDoT 0 3 1 2hVXqqpap7GThRWcHPkRSLTDMkCD 012d59ee3ccd0a9243c207bec4f0dd750a010f8d 2N3687UUKxYk3457EasUpi2pyZXd 52 6185 0.98 4042 367 148143 6 400 0 0 0 0 404 0 0 0 0 408 0 0 0 0 800 0 0 0 0 804 0 0 0 0 808 0 0 0 0 0 0.1 0 4.5765e-41 1 0 20 0 0 30 0 0 ";
        os << "prev: " << prev << endl;

        auto g = new us::gov::engine::local_deltas_t();
        auto r = g->read(prev);
        if (is_ko(r)) {
            delete g;
            assert(false);
        }
        assert(g != nullptr);

        blob_t out;
        g->write(out);
        string cur = us::gov::crypto::b58::encode(out);
        os << "cur: " << cur << endl;
        auto g2 = new engine::local_deltas_t();
        assert(g2->read(out) == ok);

        blob_t out2;
        g2->write(out2);
        string inp2 = us::gov::crypto::b58::encode(out2);
        os << "cur: " << inp2 << endl;

        auto g3 = new engine::local_deltas_t();
        assert(g3->read(out2) == ok);

        engine::diff* d = new engine::diff();
        if (!d->add(g)) {
            assert(false);
        }
        if (d->add(g2)) {
            assert(false);
        }
        if (d->add(g3)) {
            assert(false);
        }
        delete d;
    }

    void serialization_diff_prev(const string& prev) {
        os << "serialization_diff" << endl;
        os << "prev: " << prev.size() << endl;
        us::gov::engine::diff g;
        auto r = g.load(prev);
        assert(is_ok(r));
        blob_t cur;
        {
            g.write(cur);
            os << "cur: " << cur.size() << " " << (100.0*(double)cur.size()/(double)prev.size()) << '%' <<  endl;;
        }

        auto g2 = new us::gov::engine::diff();
        assert(g2->read(cur)==ok);
        blob_t inp2;
        {
            g2->write(inp2);
        }
        os << "inp2: " << inp2.size() << endl;

        auto g3 = new us::gov::engine::diff();
        assert(g3->read(inp2) == ok);
        delete g2;
        delete g3;
    }

    void serialization_db_prev(const string& prev) {
        os << "db" << endl;
        vector<uint8_t> content;
        assert(is_ok(us::gov::io::read_file_(prev, content)));
        {
            os << "prev: " << content.size() << endl;
            auto h = hasher_t::digest(content);
            assert(is_ok(load_db(prev)));
        }
        blob_t cur;
        {
            db->write(cur);
        }
        os << "cur: " << cur.size() << " " << (100.0*(double)cur.size()/(double)prev.size()) << '%' <<  endl;
        assert(is_ok(load_db(cur)));

        blob_t cur2;
        {
            db->write(cur2);
        }
        os << "cur2: " << cur2.size() << endl;
        assert(is_ok(load_db(cur2)));
    }

    using blob_reader_t = us::gov::io::blob_reader_t;

    void blocks_v7(const string& datadir) { //proc1 - blocks_vX, X=prev char
        vector<string> r;
        for(auto& p: fs::directory_iterator(datadir)) {
            if (!is_regular_file(p.path())) continue;
            if (p.path().filename() == "head") continue;
            r.push_back(p.path().filename());
        }
        for (auto& i: r) {
            cout << "##################################### " << i << ' ';
            string file = datadir + "/" + i;
            blob_reader_t::blob_header_t hdr;
            ko r = blob_reader_t::read_header(file, hdr);
            assert(is_ok(r));
            if (hdr.serid == 'D') {
                serialization_diff_prev(file);
            }
            else if (hdr.serid == 'S') {
                serialization_db_prev(file);
            }
            else {
                cout << "v " << +hdr.version << endl;
                cout << "ser " << +hdr.serid << ' ' << hdr.serid << endl;
                assert(false);
            }
        }
    }

    void serialization(const string& datadir) {
        os << "local_deltas" << endl;
        serialization_local_deltas();
        blocks_v7(datadir + "/blocks_v7"); //proc1 - blocks_vX where X=previous char streams_version
    }

    void dashboard() {
        #if CFG_COUNTERS == 1
            ostringstream os;
            write_status(os);
            check(os.str().empty(), false);
        #endif
    }

    void test_calendar() {
        struct testev: us::gov::engine::evidence {
            testev(): us::gov::engine::evidence(0, 0) {
            }
            string name() const override { return "name"; }
        };
        us::gov::engine::calendar_t cal;
        testev* ev1=new testev();
        testev* ev2=new testev();
        ev1->ts=ev2->ts;
        assert(cal.scheduleX(ev1)==us::ok);
        assert(cal.scheduleX(ev2)==us::gov::engine::calendar_t::WP_50451);
        delete ev2;
        assert(cal.size() == 1);
    }

    void test_cash() {
        auto cashapp = static_cast<us::gov::cash::app*>(db->apps.find(us::gov::cash::app::id())->second);
        cashapp->db.clear();

        //Private key: 3Fy1YB6FfFXxsyZa2TfGHewR3qrT4kxwwaQAgsydyw6Q
        //Public key: 29AGBUwDQuuyqVojfbGcjSzBrf8Z3RuAMGNWSktgtfkf5
        //Address: cdCSmHMvyYemaAw5Sm3u7Aw7z75
        hash_t src_addr("cdCSmHMvyYemaAw5Sm3u7Aw7z75");

        us::gov::crypto::ec::keys::priv_t src_sk("3Fy1YB6FfFXxsyZa2TfGHewR3qrT4kxwwaQAgsydyw6Q");
        cashapp->db.add_(src_addr, 100);
        cashapp->db.dump("", 2, os);

        check(cashapp->pool->accounts.empty(), true);

        //Private key: AnRr1ogvR3AVhHp5KpJHi1C5noqhLCSu6C3zNq4djahD
        //Public key: p11pVH4BugKcowjxZteae3wJNynuhC3Wt3B3QdyLCVbf
        //Address: 2pCcSDYA1Zt5gY4UD25vRZnhmJ2G
        hash_t dst_addr("2pCcSDYA1Zt5gY4UD25vRZnhmJ2G");

        {
        cash::tx_t t;
        bool b = t.check_amounts();
        check(b, false);
        }
        {
        cash::tx_t t;
        auto& s = t.add_section(hash_t(0));
        s.add_input(src_addr, 18);
        bool b = t.check_amounts();
        check(b, false);
        }
        cout << "3" << endl;
        {
        cash::tx_t t;
        auto& s = t.add_section(hash_t(0));
        s.add_output(dst_addr, 18);
        bool b = t.check_amounts();
        check(b, false);
        }
        {
        cash::tx_t t;
        auto& s = t.add_section(hash_t(0));
        s.add_input(src_addr, 17);
        s.add_output(dst_addr, 18);
        bool b = t.check_amounts();
        check(b, false);
        }
        cout << "5" << endl;
        {
        cash::tx_t t;
        auto& s = t.add_section(hash_t(0));
        s.add_input(src_addr, 18);
        s.add_output(dst_addr, 17);
        bool b = t.check_amounts();
        check(b, false);
        }

        cout << "6" << endl;

        cash::tx_t t;
        auto& s = t.add_section(hash_t(0));
        s.add_input(src_addr, 18);
        s.add_output(dst_addr, 18);
        bool b = t.check_amounts();
        check(b, true);

        t.write_pretty_en(cout);

        auto sigcode = t.make_sigcode_all();
        assert(sigcode.size() == 1);
        assert(sigcode[0].inputs.size() == 1);
        assert(sigcode[0].outputs.size() == 1);
        sigcode.dump(cout);
        cout << "--" << endl;
        crypto::ec::sigmsg_hasher_t::value_type h = t.get_hash(sigcode);
        cout << "TX hash: " << h << endl;
        s.inputs.begin()->locking_program_input = us::wallet::wallet::algorithm::create_input(h, src_sk);
        s.inputs.begin()->locking_program_input.sigcode = sigcode;
        os << "locking_program_input " << s.inputs.begin()->locking_program_input.encode() << endl;
        s.inputs.begin()->locking_program_input.dump(cout);

        cout << "TX before processing:" << endl;
        t.write_pretty_en(cout);

        cashapp->process(t);

        check(cashapp->pool->accounts.size(), (size_t)2);

        auto pe1 = cashapp->pool->accounts.find(src_addr);
        assert(pe1 != cashapp->pool->accounts.end());
        cout << "src acc " << pe1->second.box.value << endl;
        check(pe1->second.box.value, (cash_t)100-18-1);
        auto pe2 = cashapp->pool->accounts.find(dst_addr);
        assert(pe2 != cashapp->pool->accounts.end());
        cout << "dst acc " << pe2->second.box.value << endl;
        check(pe2->second.box.value, (cash_t)18);
        auto ld = create_local_deltas(0);
        engine::diff* d = new engine::diff();
        if (!d->add(ld)) {
            assert(false);
        }
        d->close();

        os << "-------bfore import" << endl;
        cashapp->db.dump("", 2, os);

        import(*d,d->hash());
        delete d;

        os << "-------after import" << endl;
        cashapp->db.dump("", 2, os);

        check(cashapp->db.cash_in_circulation(), 100 + cashapp->db.max_subsidy - 1 - 2); //-1 for the fees, -2 for maintenance charger of two accounts

    }

    void print(const vector<tuple<hash_t,uint32_t,uint16_t>>& v) {
        cout << "sz=" << v.size() << endl;
        if (v.empty()) return;
        auto i=v.begin();
        cout << get<0>(*i) << ' ' << get<1>(*i) << ' ' << get<2>(*i) << endl;
    }

    void check_x(const vector<tuple<hash_t,uint32_t,uint16_t>>& v, const vector<tuple<hash_t,uint32_t,uint16_t>>& expected) {
        if (v.size()!=expected.size()) {
            os << "XFail. vector sizes differ. Expected " << expected.size() << ", got " << v.size() << '\n';
            abort();
        }
        vector<tuple<hash_t,uint32_t,uint16_t>>::const_iterator i=v.begin();
        vector<tuple<hash_t,uint32_t,uint16_t>>::const_iterator j=expected.begin();
        while(i!=v.end()) {
            cout << get<0>(*i) << ' ' << get<1>(*i) << ' ' << get<2>(*i) << '\n';
            cout << get<0>(*j) << ' ' << get<1>(*j) << ' ' << get<2>(*j) << '\n';
            cout << '\n';
            ++i;
            ++j;
        }
        if (v==expected) return;
        os << "XFail. content of vectors of size " << v.size() << " differ.\n";
        abort();
    }

    void test_app_factory() {
        cout << "test delta factory\n";
        {
        cout << "LD auth::app" << endl;
        auto p = us::gov::engine::app::local_delta::create(us::gov::engine::auth::app::id());
        assert(p != nullptr);
        delete p;
        }
        {
        cout << "LD cash::app" << endl;
        auto p = us::gov::engine::app::local_delta::create(us::gov::cash::app::id());
        assert(p != nullptr);
        delete p;
        }
        {
        cout << "LD sys::app" << endl;
        auto p = us::gov::engine::app::local_delta::create(us::gov::sys::app::id());
        assert(p != nullptr);
        delete p;
        }
        {
        cout << "LD traders::app" << endl;
        auto p = us::gov::engine::app::local_delta::create(us::gov::traders::app::id());
        assert(p != nullptr);
        delete p;
        }

        {
        cout << "D auth::app" << endl;
        auto p = us::gov::engine::app::delta::create(us::gov::engine::auth::app::id());
        assert(p != nullptr);
        delete p;
        }
        {
        cout << "D cash::app" << endl;
        auto p = us::gov::engine::app::delta::create(us::gov::cash::app::id());
        assert(p != nullptr);
        delete p;
        }
        {
        cout << "D sys::app" << endl;
        auto p = us::gov::engine::app::delta::create(us::gov::sys::app::id());
        assert(p != nullptr);
        delete p;
        }
        {
        cout << "D traders::app" << endl;
        auto p = us::gov::engine::app::delta::create(us::gov::traders::app::id());
        assert(p != nullptr);
        delete p;
        }
    }

    void self_test(const string& datadir) {
        cout << endl << "dashboard" << endl;
        dashboard();
        cout << endl << "serialization" << endl;
        serialization(datadir);
        cout << endl << "cash" << endl;
        test_cash();
        test_calendar();
        test_app_factory();
    }

};

using us::gov::io::cfg;
using us::gov::io::cfg0;
using us::gov::io::cfg1;
using us::ok;

int test_db_analyst(string sets);
int main_calendar();
void test_peerd();
void test_server();
void test_ip4();
void test_auth();
void test_syncd();

#if CFG_LOGS == 0
    #undef log_dir
    namespace {
        string log_dir() {
            ostringstream os;
            os << "logs/us-test_" << getpid();
            return os.str();
        }
    }
#endif
void test_engine_daemon(const string& datadir) {
    using us::gov::io::cfg_daemon;
    string homedir = log_dir() + "/home/test_engine_daemon";
    auto conf = cfg_daemon::load(123, homedir + "/gov", true);
    assert(conf.first == ok);
    assert(conf.second != nullptr);
    {
        tengine e(homedir, tengine::govport, cout);
        #if CFG_LOGS == 1
            e.logdir = log_dir() + "/self_test";
        #endif
        e.self_test(datadir);
    }
    delete conf.second;
}

void test_tee(string teststr) {
    using namespace us::wallet;
    ostringstream os1;
    ostringstream os2;
    teestream tee(os1,os2);
    tee << teststr;
    assert(os1.str()==teststr);
    assert(os2.str()==teststr);
}

void test_tee() {
    test_tee("test");
    ostringstream os;
    os << "test" << endl;
    test_tee(os.str());
}

void test_signverify();
void test_node0();
void test_blobs();
void test_things();

void sig_handler(int s) {
    cout << "------------------\n";
    cout << "main: received signal " << s << endl;
    log_info(cout);
    cout << "------------------\n";
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGPIPE, SIG_DFL);
    raise(s);
}

#if CFG_COUNTERS == 0
    Error. configure with counters.
    bin/configure ... --with-counters
#endif

struct network_c0: network {
    using b = network;

    network_c0(const string& homedir, const string& logdir, const string& vardir, ostream& os): b(homedir, logdir, vardir, "test-c0_r2r_stage1", os) {
        add_node("bank1", new node_bank("bank1", homedir, logdir, vardir, 22072, 22073));
        add_node("bank2", new node_bank("bank2", homedir, logdir, vardir, 22172, 22173));
    }

    ~network_c0() {
    }

    void stage1_configure() override {
        cout << "configuring bank nodes" << endl;
        dynamic_cast<node_bank*>(find("bank1")->second)->create_bank("w2w/bank1");
        dynamic_cast<node_bank*>(find("bank2")->second)->create_bank("w2w/bank2");
        find("bank1")->second->register_wallet();
        find("bank2")->second->register_wallet();
    }

    void stage1_ff_configure() override {
        cout << "configuring ask node" << endl;
        assert(dynamic_cast<node_bank*>(find("bank1")->second)->load_data("w2w/bank1"));

        cout << "configuring pat node" << endl;
        assert(dynamic_cast<node_bank*>(find("bank2")->second)->load_data("w2w/bank2"));
    }

};

void test_r2r(const string& homedir, const string& logdir, const string& vardir) {
    us::gov::engine::auth::collusion_control_t::max_nodes_per_ip = 255;
    ostream& os = cout;

    network_c0 n(homedir, logdir, vardir, os);

    cout << "stage1" << endl;
    n.stage1();

    us::test::r2r_t::wait_from_seq = 0;

    {
      w2w_t o(n);
      o.run();
    }


/*
    {
        bid2ask_t o(n);
        o.run();
    }
*/

    auto b = n.bookmarks();
    b.dump("bookmarks>", cout);

    if (!batch) {
        n.menu();
    }

    /*
    cout << "=software updates=====================================================================" << endl;
    //software updates account
    {
        auto a=bid.wallet_cli->api().new_address();
        assert(a.first==ok);
        cout << "software updates account: " << a.second << ". @ bid wallet" << endl;

        {
        cout << "list addresses:" << endl;
        auto l=bid.wallet_cli->api().list(1);
        assert(l.first==ok);
        cout << l.second << endl;
        }

        gov::engine::evidence* ev;
        {
            auto am=1000000;
            auto t=bid.wallet_cli->api().transfer(a.second, am, hash_t(0));
            if (t.first!=ok) cout << t.first << endl;
            assert(t.first==ok);
            cout << "transfer " << am << " to address " << a.second << endl;
            cout << t.second << endl;
            ev=gov::engine::evidence::from_b58(t.second);
        }
        uint64_t ts=ev->ts;

        wait_settlement(*bid.gov_cli, ts);
        delete ev;

        cout << "balance account " << a.second << ":" << endl;
        auto t=bid.wallet_cli->api().balance(1);
        if (t.first!=ok) {
            cout << t.first << endl;
        }
        else {
            cout << t.second << endl;
        }

    }
*/
    cout << "ended" << endl;
}

#if CFG_LOGS == 0
#undef log_dir
namespace {
    string log_dir() {
        ostringstream os;
        os << "logs/us-trader_" << getpid() << '/' << "test_r2r";
        return os.str();
    }
}
#endif

void test_nodes_main() {
    string logdir0 = "test_r2r";
    log_start(logdir0, "main");
    tee("=================test_nodes========================");

    string homedir = log_dir() + "/home";
    string vardir = log_dir() + "/var";

    network::test();

    test_r2r(homedir, logdir0, vardir);
    tee("=========== end testing nodes ==============");
}

void test_nodes() {
    tee("=================test_nodes========================");
    log("starting thread");
    thread th(&test_nodes_main); //start new log file
    th.join();
    tee("=========== end testing nodes ==============");
}


void test_perf_encrypt();
bool valgrind = false;
bool l1_tests = true;
bool l2_tests = false;

void test_l1() {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    cout << "auth..." << endl;
    test_auth();

    cout << "blobs..." << endl;
    test_blobs();

    cout << "things..." << endl;
    test_things();

    cout << "signverify..." << endl;
    test_signverify();

    cout << "valgrind=" << valgrind << endl;
    cout << "symmetric encryption..." << endl;
    test_symmetric_encryption();

    if (!valgrind) {
        cout << "symmetric encryption performance..." << endl;
        test_perf_encrypt();
    }

    cout << "engine daemon..." << endl;
//    test_engine_daemon(string(argv[1]));
    test_engine_daemon("../test");

    cout << "peerd..." << endl;
    test_peerd();

    cout << "node0..." << endl;
    test_node0();

    cout << "syncd..." << endl;
    test_syncd();

    cout << "tee..." << endl;
    test_tee();

    cout << "accounts..." << endl;
    check_accounts();

    cout << "ip4..." << endl;
    test_ip4();

    cout << "calendar..." << endl;
    main_calendar();

    cout << "auth..." << endl;
    test_auth();

//x    test_server();
//    test_db_analyst(string(argv[1])+"/db_analyst");
}

void help() {
    cout << "--batch        Unattended/not interactive" << endl;
    cout << "--only-r2r     Skip L1 tests." << endl;
    cout << "--only-l1      Skip L2 tests." << endl;
    cout << "--valgrind     Omit heavy tests." << endl;
}

int core0_main(int argc, char** argv) {

    us::gov::io::shell_args args(argc, argv);

    while(true) {
        string command = args.next<string>();
        if (command == "--batch") {
            batch = true;
        }
        else if (command == "--only-r2r") {
            l1_tests = false;
        }
        else if (command == "--only-l1") {
            l2_tests = false;
        }
        else if (command == "--valgrind") {
            valgrind = true;
        }
        else if (command.empty()) {
            break;
        }
        else {
            help();
            cout << "Invalid command/option." << endl;
            return 1;
        }
    }

    if (l1_tests) {
        test_l1();
    }

    log_info(cout);
    cout << endl;

//    if (argc < 2) {
//        cerr << "datadir" << endl;
//        return 1;
//    }

//  #if CFG_LOGS == 1
//      us::dbg::thread_logger::instance.disabled=true;
//  #endif

//  us::dbg::task::teecout=true;
//    log_pstart("us-test");
//    log_start("", "main");


    if (l2_tests) {
        cout << "r2r..." << endl;
        test_nodes();
    }

    us::gov::cli::hmi::process_cleanup();
    us::wallet::cli::hmi::process_cleanup();
    tee("TEST SUCCEED");

    return 0;
}


}}

