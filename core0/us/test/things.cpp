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
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <random>

#include <us/gov/config.h>
#include <us/gov/crypto/types.h>
#include <us/gov/types.h>
#include <us/gov/traders/wallet_address.h>
#include <us/gov/traders/app.h>

#include <us/wallet/wallet/local_api.h>
#include <us/wallet/trader/trader_protocol.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/chat_t.h>
#include <us/wallet/trader/bootstrap/a_t.h>
#include <us/wallet/trader/bootstrap/b_t.h>
#include <us/wallet/trader/kv.h>
#include <us/wallet/trader/endpoint_t.h>
#include <us/wallet/trader/bookmarks_t.h>
#include <us/wallet/trader/personality/proof_t.h>
#include <us/wallet/trader/libs_t.h>

#include "test_platform.h"

#define loglevel "test"
#define logclass "things"
#include "logs.inc"

#include "assert.inc"

namespace us::test {

using namespace std;
using namespace us;

struct things_t: us::test::test_platform {

    things_t(): us::test::test_platform(cout) {
    }


    void test_split_fqn(const string& sin, const pair<ko, pair<string, string>>& xpected) {
        auto r =us::gov::io::cfg0::split_fqn(sin);
        cout << "\nsplit_fqn '" << sin << "'\n";
        cout << "expected: " << (is_ko(xpected.first)?"KO":"OK") << " path='" << xpected.second.first << "' file='" << xpected.second.second << "'" << endl;
        cout << "got: " << (is_ko(r.first)?"KO":"OK") << " path='" << r.second.first << "' file='" << r.second.second << "'" << endl;
        if (xpected.first == ok) {
            assert(r.first == ok);
        }
        else {
            assert(is_ko(r.first));
        }
        assert(r.second.first == xpected.second.first);
        assert(r.second.second == xpected.second.second);
        cout << "PASS\n";
    }

    void test_split_fqn() {
        test_split_fqn("", make_pair("KO", make_pair("", "")));
        test_split_fqn("a", make_pair("KO", make_pair("", "")));
        test_split_fqn("a/", make_pair("KO", make_pair("", "")));
        test_split_fqn("/", make_pair(ok, make_pair("/", "")));
        test_split_fqn(" /", make_pair(ok, make_pair("/", "")));
        test_split_fqn(" / ", make_pair(ok, make_pair("/", "")));
        test_split_fqn("       / ", make_pair(ok, make_pair("/", "")));
        test_split_fqn("/ ", make_pair(ok, make_pair("/", "")));
        test_split_fqn("/a", make_pair(ok, make_pair("/", "a")));
        test_split_fqn("/a ", make_pair(ok, make_pair("/", "a")));
        test_split_fqn(" /a", make_pair(ok, make_pair("/", "a")));
        test_split_fqn("/a/b/c", make_pair(ok, make_pair("/a/b", "c")));
        test_split_fqn("/a/b/c ", make_pair(ok, make_pair("/a/b", "c")));
        test_split_fqn(" /a/b/c ", make_pair(ok, make_pair("/a/b", "c")));
        test_split_fqn("/a/b/ c ", make_pair("KO", make_pair("", "")));
        test_split_fqn("/a/ b/c ", make_pair("KO", make_pair("", "")));
        test_split_fqn("/ a/ b/c ", make_pair("KO", make_pair("", "")));
        test_split_fqn("/    a/ b/c ", make_pair("KO", make_pair("", "")));
        test_split_fqn("/a/b/c", make_pair(ok, make_pair("/a/b", "c")));
        test_split_fqn("/a/b/", make_pair(ok, make_pair("/a/b", "")));
        test_split_fqn("//a", make_pair(ok, make_pair("/", "a")));
        test_split_fqn(" ////a", make_pair(ok, make_pair("/", "a")));
        test_split_fqn("////a", make_pair(ok, make_pair("/", "a")));
        test_split_fqn("////a//b/c", make_pair(ok, make_pair("/a/b", "c")));
        test_split_fqn("////a//b/c/", make_pair(ok, make_pair("/a/b/c", "")));
        test_split_fqn("/a///b///c/d", make_pair(ok, make_pair("/a/b/c", "d")));
        test_split_fqn("/a///b///c//", make_pair(ok, make_pair("/a/b/c", "")));
        test_split_fqn("////a///b///c//", make_pair(ok, make_pair("/a/b/c", "")));
        test_split_fqn("////~*---xcn///\"$$///c//", make_pair(ok, make_pair("/~*---xcn/\"$$/c", "")));
        test_split_fqn("z////~*---xcn///\"$$///c//", make_pair("KO", make_pair("", "")));
        test_split_fqn("/z////~*---xcn///\"$$///c//", make_pair(ok, make_pair("/z/~*---xcn/\"$$/c", "")));
    }

    void test_datadiff() {
    };

    void test_path_fs(const string& sin, const string& xpected) {
        cout << "input:" << sin << " output:";
        string sout = us::gov::io::cfg0::rewrite_path(sin);
        cout << sout << endl;
        check(sout, xpected);
        sout=us::gov::io::cfg0::rewrite_path(sout);
        check(sout, xpected);
    }

    void test_path_fs() {
        test_path_fs("","");
        test_path_fs("/","");
        test_path_fs("/a/b/c","a/b/c");
        test_path_fs("/a/b/c/","a/b/c");
        test_path_fs("/a/b b/c/","a/bb/c");
        test_path_fs(" /a/b  b/c/ ","a/bb/c");
        test_path_fs(" /a////b  b/c/ ","a/bb/c");
        test_path_fs("/a/ b//c/d ","a/b/c/d");
        test_path_fs("a","a");
        test_path_fs("a/","a");
        test_path_fs("a//","a");
        test_path_fs("/a//","a");
        test_path_fs("//a//","a");
        test_path_fs("amz/AMZ/059","amz/AMZ/059");
        test_path_fs("a~z/A$Z/0++9/=","az/AZ/09");
        test_path_fs("𝕋𝕙𝕖 𝕢𝕦𝕚𝕔𝕜 𝕓𝕣𝕠𝕨𝕟 𝕗𝕠𝕩 𝕛𝕦𝕞𝕡𝕤 𝕠𝕧𝕖𝕣 𝕥𝕙𝕖 𝕝𝕒𝕫𝕪 𝕕𝕠𝕘/void","void");
        test_path_fs("パーティーへ行かないか","");
        test_path_fs("˙ɐnbᴉlɐ ɐuƃɐɯ ǝɹolop ʇǝ ǝɹoqɐl ʇn ʇunpᴉpᴉɔuᴉ ɹodɯǝʇ poɯsnᴉǝ op pǝs 'ʇᴉlǝ ƃuᴉɔsᴉdᴉpɐ ɹnʇǝʇɔǝsuoɔ 'ʇǝɯɐ ʇᴉs ɹolop ɯnsdᴉ ɯǝɹo˥00>","nbluolopoqlnunppuodposnoppslusdpnsuosolopnsdo00");
        test_path_fs("-$1.00","100");
    }

    void test_trim(const string& s0, const string& expected) {
        string s=s0;
        us::gov::io::cfg0::trim(s);
        cout << "trim '" << s0 << "' gave '" << s << "'" << endl;
        assert(s==expected);
    }
    /*
    void test_endpoint(const string&s, us::ko eko, const string& eh, uint16_t ep) {
        test_endpoint(s, (const char*)eko, eh, ep);
    }
    */

    template<typename t>
    t test_serialization_blob(const t& g) {
        using blob_t = us::wallet::trader::blob_t;

        cout << "\n    test serialization blob" << endl;
        cout << "        write" << endl;
        g.dump("    g> ", cout);
        blob_t b;
        g.write(b);
        cout << g.tlv_size() << " " << b.size() << endl;
        assert(g.tlv_size() == b.size());
        auto s = hash_t::compute(b);
        cout << "      s===>" << s << "<=== " << b.size() << endl;
        assert(b.size() > 0);
        auto g2 = g;
        g2.dump("    g2> ", cout);
        blob_t b2;
        g2.write(b2);
        assert(g2.tlv_size() == b.size());
        assert(g2.tlv_size() == b2.size());
        auto s2 = hash_t::compute(b2);
        cout << "      s2==>" << s2 << "<===" << endl;

        assert(!s.empty());
        assert(s == s2);

        t g3;
        cout << "        read" << endl;
        auto r = g3.read(b);
        if (is_ko(r)) {
            cout << r << endl;
        }
        assert(is_ok(r));
        g3.dump("    g3> ", cout);
        blob_t b3;
        g3.write(b3);
        auto s3 = hash_t::compute(b3);
        cout << "      s3==>" << s3 << "<===" << endl;
        assert(s3 == s);
        return g3;
    }

    template<typename t>
    t test_serialization(const t& g) {
        cout << "\ntest serialization - serid " << (g.serial_id() == 0 ? '-' : (char)g.serial_id()) << endl;
        blob_t blob;
        g.write(blob);
        auto s = us::gov::crypto::b58::encode(blob);
        cout << "  s===>" << s << "<===" << endl;
        auto g2 = g;
        blob_t blob2;
        g2.write(blob2);
        auto s2 = us::gov::crypto::b58::encode(blob2);
        cout << "  s2==>" << s2 << "<===" << endl;

        assert(!s.empty());
        assert(s == s2);

        t g3;
        {
            auto r = g3.read2(s);
            if (is_ko(r.first)) {
                cout << r.first << endl;
                assert(false);
            }
        }
    //    istringstream is(s);
    //    assert(is_ok(g3.from_stream(is)));

        blob_t blob3;
    //    ostringstream os3;
    //    g3.to_stream(os3);
        g3.write(blob3);
        auto s3 = us::gov::crypto::b58::encode(blob3);
      //    auto s3= os3.str();
        cout << "  s3==>" << s3 << "<===" << endl;
        assert(s3 == s);
        return g3;
    }

    template<typename t>
    void test_serialization_eq(const t& g) {
        t r =  test_serialization(g);
        assert(g == r);
    }


    template<typename t>
    t test_serialization_op(const t& g) {
        cout << "\ntest serialization" << endl;
        ostringstream os;
        os << g;
        auto s = os.str();
        cout << "  s===>" << s << "<===" << endl;
        auto g2 = g;
        ostringstream os2;
        os2 << g2;
        auto s2 = os2.str();
        cout << "  s2==>" << s2 << "<===" << endl;

        assert(!s.empty());
        assert(s==s2);

        t g3;
        istringstream is(s);
        is >> g3;
        assert(!is.fail());

        ostringstream os3;
        os3 << g3;
        auto s3= os3.str();
        cout << "  s3==>" << s3 << "<===" << endl;
        assert(s3 == s);
        return g3;
    }

    template<typename t>
    void test_serialization_op_eq(const t& g) {
        t r =  test_serialization_op(g);
        assert(g == r);
    }


    void test_endpoint(const string&s, us::ko eko, const string& eh, uint16_t ep) {
        using us::gov::socket::client;
        cout << "checking '" << s << "'" << endl;
        pair<us::ko, pair<string, uint16_t>> r=client::parse_endpoint(s);
        cout << "got " << (r.first==us::ok?"ok":r.first) << " '" << r.second.first << "' " << r.second.second << "'" << endl;
        assert(r.first==eko);
        assert(r.second.first==eh);
        assert(r.second.second==ep);
    }

    void test_endpoint() {
        using us::gov::socket::client;
        using us::ok;
        test_endpoint("", client::KO_27190, "", 0);
        test_endpoint("         ", client::KO_27190, "", 0);
        test_endpoint(":", client::KO_27190,"", 0);
        test_endpoint("    :", client::KO_27190,"", 0);
        test_endpoint("a:", client::KO_27191,"", 0);
        test_endpoint("a:a", client::KO_27191,"", 0);
        test_endpoint("a:0", client::KO_27191,"", 0);
        test_endpoint("a: 0 ", client::KO_27191,"", 0);
        test_endpoint("a:1", ok, "a", 1);
        test_endpoint("a: 1", ok, "a", 1);
        test_endpoint("a:1 ", ok, "a", 1);
        test_endpoint("a:1", ok, "a", 1);
        test_endpoint("example.com:16671", ok, "example.com", 16671);
        test_endpoint("    example.com:16671", ok, "example.com", 16671);
        test_endpoint("example.com    :16671", ok, "example.com", 16671);
        test_endpoint("    example.com    :16671", ok, "example.com", 16671);
        test_endpoint("    exa   mple.com    :16671", ok, "exa   mple.com", 16671);
    }


    void test_bookmarks() {
        using bookmarks_t = us::wallet::trader::bookmarks_t;
        using bookmark_t = us::wallet::trader::bookmark_t;
        using bookmark_info_t = us::wallet::trader::bookmark_info_t;
        using protocol_selection_t = us::wallet::trader::protocol_selection_t;
        using qr_t = us::wallet::trader::qr_t;
        {
            cout << "protocol_selection 1" << endl;
            protocol_selection_t b;
            test_serialization_blob(b);
        }
        {
            cout << "protocol_selection 2" << endl;
            protocol_selection_t b("pat2ai", "pat");
            test_serialization_blob(b);
        }
        {
            cout << "bookmark_info 1" << endl;
            bookmark_info_t b;
            test_serialization_blob(b);
        }
        {
            cout << "bookmark_info 2" << endl;
            bookmark_info_t b("  book mark 1 ", {0,1,2,3,4,5,6,7,8,9});
            test_serialization_blob(b);
        }
        {
            vector<uint8_t> ico;
            auto r = us::gov::io::read_file_("../test/img/ico.png", ico);
            assert(is_ok(r));
            assert(ico.size() == 12239);
            cout << "bookmark_info 3" << endl;
            bookmark_info_t b("  book mark 1 ", move(ico));
            test_serialization_blob(b);
        }
        {
            cout << "endpoint 1" << endl;
            endpoint_t b;
            test_serialization_blob(b);
        }
        {
            cout << "endpoint 2" << endl;
            endpoint_t b("16 4B3wEjGcfDco3mPhsensQpcqX8Cm.subhome");
            b.dump("> ", cout);
            test_serialization_blob(b);
        }
        {
            cout << "qr 1" << endl;
            qr_t b;
            test_serialization_blob(b);
        }
        {
            cout << "qr 2" << endl;
            qr_t b(endpoint_t("0 4B3wEjGcfDco3mPhsensQpcqX8Cm.subhome"), protocol_selection_t("pat2ai", "pat"));
            test_serialization_blob(b);
        }
        {
            cout << "bookmarks 1" << endl;
            bookmarks_t b;
            test_serialization_blob(b);
        }
        {
            cout << "bookmarks 2" << endl;
            bookmarks_t b;
            assert(b.add("bm 2", bookmark_t(qr_t(endpoint_t("0 4B3wEjGcfDco3mPhsensQpcqX8Cm.subhome"), protocol_selection_t("pat2ai", "pat")), bookmark_info_t("  book mark 1 ", {0,1,2,3,4,5,6,7,8,9}))) == ok);
            test_serialization_blob(b);
        //exit(0);
        }
        {
            cout << "bookmarks 3" << endl;
            vector<uint8_t> ico;
            auto r = us::gov::io::read_file_("../test/img/ico.png", ico);
            assert(is_ok(r));
            assert(ico.size() == 12239);
            bookmarks_t b;
            assert(b.add("bm 3", bookmark_t(qr_t(endpoint_t("0 4B3wEjGcfDco3mPhsensQpcqX8Cm.subhome"), protocol_selection_t("pat2ai", "pat")), bookmark_info_t("  book mark 1 ", move(ico)))) == ok);
            assert(b.begin()->second.ico.size() ==  12239);
            test_serialization_blob(b);
        //exit(0);
        }
    }

    void test_trader_endpoint() {
        using us::wallet::trader::trader_t;
        using endpoint_t = us::wallet::trader::endpoint_t;
        {
            string ip = "185.157.222.106:5556";
            cout << "ip=" << ip << endl;
            auto hash = endpoint_t::encode_ip4(ip);
            cout << "hash=" << hash << endl;
            string ip2 = endpoint_t::decode_ip4_string(hash);
            cout << "ip2=" << ip2 << endl;
            assert(ip == ip2);
        }
        {
            auto x = endpoint_t("4B3wEjGcfDco3mPhsensQpcqX8Cm");
            assert(!x.is_ip4());
            assert(x.wloc.empty());
            test_serialization(x);
        }
        {
            auto x = endpoint_t("4B3wEjGcfDco3mPhsensQpcqX8Cm.subhome");
            assert(!x.is_ip4());
            assert(x.wloc=="subhome");
            test_serialization(x);
        }
        {
            auto x = endpoint_t("0 4B3wEjGcfDco3mPhsensQpcqX8Cm.subhome");
            assert(!x.is_ip4());
            assert(x.wloc == "subhome");
            test_serialization(x);
        }
        {
            auto x = endpoint_t("6 4B3wEjGcfDco3mPhsensQpcqX8Cm.subhome");
            assert(!x.is_ip4());
            assert(x.wloc == "subhome");
            test_serialization(x);
        }
        {
            string ip = "185.157.222.106:1234";
            auto x = endpoint_t(ip);
            assert(x.is_ip4());
            string ip2 = x.decode_ip4_string();
            assert(ip == ip2);
            assert(x.wloc.empty());
            test_serialization(x);
        }
        {
            string ip="185.157.222.106:1234.subhome";
            auto x = endpoint_t(ip);
            assert(x.is_ip4());
            string ip2 = x.decode_ip4_string();
            assert(ip2 == "185.157.222.106:1234");
            assert(x.wloc == "subhome");
            test_serialization(x);
        }
        {
            string ip = "185.157.222.subhome";
            auto x = endpoint_t(ip);
            assert(!x.is_ip4());
            assert(x.wloc == "");
            assert(x.pkh.is_zero());
            test_serialization(x);
        }
        {
            string ip = "";
            auto x = endpoint_t(ip);
            assert(!x.is_ip4());
            assert(x.wloc == "");
            assert(x.pkh.is_zero());
            test_serialization(x);
        }
        {
            string ip=".";
            auto x = endpoint_t(ip);
            assert(!x.is_ip4());
            assert(x.wloc == "");
            assert(x.pkh.is_zero());
            test_serialization(x);
        }
        {
            string ip="...";
            auto x = endpoint_t(ip);
            assert(!x.is_ip4());
            assert(x.wloc == "");
            assert(x.pkh.is_zero());
            test_serialization(x);
        }
        {
            string ip = "....";
            auto x = endpoint_t(ip);
            assert(!x.is_ip4());
            assert(x.wloc == "");
            assert(x.pkh.is_zero());
            test_serialization(x);
        }
        {
            string ip = ".....";
            auto x = endpoint_t(ip);
            assert(!x.is_ip4());
            cout << x.wloc << endl;
            assert(x.wloc == "");
            assert(x.pkh.is_zero());
            test_serialization(x);
        }
    }

    void test_trim() {
        test_trim("","");
        test_trim(" ","");
        test_trim("          ","");
        test_trim("\n         ","");
        test_trim("        \n","");
        test_trim("\t  \n      \n","");
        test_trim(" A", "A");
        test_trim("  A", "A");
        test_trim("\nA", "A");
        test_trim("\n A", "A");
        test_trim("\n    A", "A");
        test_trim("  \n    A", "A");
        test_trim("\t  \n    A", "A");
        test_trim("\t  \n    A ", "A");
        test_trim("\t  \n    A\n", "A");
        test_trim("\t  \n    A\t\n", "A");
        test_trim("\t  \n    A\t\n ", "A");
        test_trim("\t  \n    A\t\n B", "A\t\n B");
        test_trim("\t  \n    A\t\n B ", "A\t\n B");
        test_trim("\t  \n    A\t\n B      ", "A\t\n B");
        test_trim("\t  \n    A\t\n B      \t\n", "A\t\n B");
        test_trim("\t  \n    A\t\n B      \t\n", "A\t\n B");
        test_trim("\t  \n    A   ", "A");
        test_trim("A   ", "A");
        test_trim("A  \n ", "A");
        test_trim("A  \n B", "A  \n B");
        test_trim("\n  A  \n B \n", "A  \n B");

        test_trim(" AA", "AA");
        test_trim("  AA", "AA");
        test_trim("\nAA", "AA");
        test_trim("\n AA", "AA");
        test_trim("\n    AA", "AA");
        test_trim("  \n    AA", "AA");
        test_trim("\t  \n    AA", "AA");
        test_trim("\t  \n    AA ", "AA");
        test_trim("\t  \n    AA\n", "AA");
        test_trim("\t  \n    AA\t\n", "AA");
        test_trim("\t  \n    AA\t\n ", "AA");
        test_trim("\t  \n    AA\t\n BB", "AA\t\n BB");
        test_trim("\t  \n    AA\t\n BB ", "AA\t\n BB");
        test_trim("\t  \n    AA\t\n BB      ", "AA\t\n BB");
        test_trim("\t  \n    AA\t\n BB      \t\n", "AA\t\n BB");
        test_trim("\t  \n    AA\t\n BB      \t\n", "AA\t\n BB");
        test_trim("\t  \n    AA   ", "AA");
        test_trim("AA   ", "AA");
        test_trim("AA  \n ", "AA");
        test_trim("AA  \n BB", "AA  \n BB");
        test_trim("\n  AA  \n BB \n", "AA  \n BB");

    }

    using chat_entry = us::wallet::trader::chat_entry;
    using paragraphs = us::wallet::trader::paragraphs;

    void test_chat(const chat_entry& e, const chat_entry& e2) {
        e.dump(" e>", cout);
        e2.dump("e2>", cout);
        check(e.size(), e2.size());
        check(e.me, e2.me);

    //    ostringstream os;
    //    e.to_stream(os);
        cout << e.size() << endl;
        e.dump("entry: ", cout);
        blob_t blob;
        e.write(blob);
        cout << blob.size() << ' ' << e.blob_size() << endl;
        cout << "=> " << e.size() << ' ' << us::gov::io::blob_writer_t::sizet_size(e.size()) << endl;
        chat_entry o;
        {
            auto r = o.read(blob);
            if (is_ko(r)) {
                cout << r << endl;
                assert(false);
            }
        }
    //    istringstream is(os.str());
    //    auto r = us::wallet::trader::chat_entry::from_stream(is);

        blob_t blob2;
        o.write(blob2);

    //    ostringstream os2;
    //    r.to_stream(os2);
    //    cout << os2.str() << endl;
        cout << blob.size() << ' ' << blob2.size() << endl;
        check(blob, blob2);

        blob_t blob3;
        e2.write(blob3);
    //    ostringstream os3;
    //    e2.to_stream(os3);
    //    cout << os3.str() << endl;
    //    check(os2.str(),os3.str());
        check(blob2, blob3);
    }

    void test_chat(const us::wallet::trader::chat_entry& e) {
        us::wallet::trader::chat_entry e2(e);
        test_chat(e, e2);
    }

    void test_chat() {
        {
            cout << "empty constructor" << endl;
            chat_entry e;
            test_chat(e);
        }
        {
            cout << "string constructor" << endl;
            chat_entry e("Hi");
            test_chat(e);
        }
        {
            cout << "vector<string> constructor" << endl;
            chat_entry e(paragraphs{"Hi", "How're u doing", "Great thanks", "Bye"});
            test_chat(e);
        }
    }

    using personality_t = us::wallet::trader::personality::personality_t;
    using raw_personality_t = us::wallet::trader::personality::raw_personality_t;
    using keys = us::gov::crypto::ec::keys;
    using priv_t = keys::priv_t;
    using pub_t = keys::pub_t;

    void test_priv_zero() {
        priv_t k;
        assert(is_ok(k.generate()));
        assert(!k.is_zero());
        k.zero();
        assert(k.is_zero());
        assert(!k.is_not_zero());
    }

    void test_priv_reset() {
        priv_t inv;
        assert(inv.is_zero());
        priv_t val;
        assert(is_ok(val.generate()));
        assert(val.is_not_zero());
        {
        auto cp = val;
        assert(val == cp);
        val.reset(val);
        assert(val == cp);
        }
        {
        auto cp{val};
        val.reset(val);
        assert(val == cp);
        }
        {
        auto cp{val};
        cp.reset(inv);
        assert(cp == inv);
        }
        {
        auto cp{inv};
        cp.reset(val);
        assert(cp == val);
        }
    }

    void test_priv_reset_if_distinct() {
        {
        priv_t inv;
        priv_t val;
        assert(is_ok(val.generate()));
        assert(inv.is_zero());
        assert(val.is_not_zero());
        assert(val.reset_if_distinct(inv) == -1);
        assert(val == inv);
        }
        {
        priv_t inv;
        priv_t val;
        assert(is_ok(val.generate()));
        assert(inv.is_zero());
        assert(val.is_not_zero());
        assert(inv.reset_if_distinct(val) == 1);
        assert(val == inv);
        }
        {
        priv_t inv;
        priv_t val;
        assert(is_ok(val.generate()));
        assert(inv.is_zero());
        assert(val.is_not_zero());
        assert(inv.reset_if_distinct(inv) == 0);
        assert(inv == inv);
        }
        {
        priv_t inv;
        priv_t val;
        assert(is_ok(val.generate()));
        assert(inv.is_zero());
        assert(val.is_not_zero());
        assert(val.reset_if_distinct(val) == 0);
        assert(val == val);
        }
        {
        priv_t val;
        priv_t val2;
        assert(is_ok(val.generate()));
        assert(is_ok(val2.generate()));
        assert(val.is_not_zero());
        assert(val2.is_not_zero());
        assert(val.reset_if_distinct(val2) == 1);
        assert(val == val2);
        assert(val.reset_if_distinct(val2) == 0);
        assert(val == val2);
        }
    }

    void test_priv_b58() {
        auto sk = keys::generate_priv();
        string s = sk.to_b58();
        cout << s << endl;
        assert(sk.is_not_zero());

        {
            auto b58 = "F8NUWTiGyjD6JKghxBhtf2f9y9MKet7Ffwfn9gK2WL6m";
            priv_t k = priv_t(b58);
            priv_t k2(b58);
            priv_t k3{b58};
            priv_t k4 = priv_t::from_b58(b58);
            assert(k == k2);
            assert(k == k3);
            assert(k == k4);
            auto s = k.to_b58();
            assert(b58 == s);

            assert(k == b58);
            assert(!(k != b58));
            assert(k != "ggg");
            assert(!(k == "ggg"));
        }

    }

    void test_priv() {
        test_priv_zero();
        test_priv_reset();
        test_priv_reset_if_distinct();
        test_priv_b58();
        {
        priv_t k;
        test_serialization_op_eq(k);
        }
        {
        priv_t k;
        k.generate();
        test_serialization_op_eq(k);
        }

    }

    void test_pub() {
        {
        keys k;
        assert(!k.pub.valid);
        test_serialization_op_eq(k.pub);
        }
        {
        keys k = keys::generate();
        assert(k.pub.valid);
        test_serialization_op_eq(k.pub);
        }

    }

    void test_keys() {
        priv_t inv;
        keys k(inv);
        keys ksinv(inv);

        priv_t val;
        assert(is_ok(val.generate()));
        keys ksval(val);

        priv_t val2;
        assert(is_ok(val2.generate()));

        {
        keys ks(inv);
        assert(ks.priv.is_zero());
        assert(!ks.pub.valid);
        }
        {
        keys ks(val);
        assert(ks.priv.is_not_zero());
        assert(ks.pub.valid);
        auto r = ks.reset_if_distinct(val);
        assert(r == 0);
        assert(ks.priv == ksval.priv);
        assert(ks.pub == ksval.pub);
        }
        {
        keys cp = ksinv;
        assert(ksinv.priv == cp.priv);
        assert(ksinv.pub == cp.pub);
        }
        {
        keys cp = ksval;
        assert(ksval.priv == cp.priv);
        assert(ksval.pub == cp.pub);
        }
        {
        auto cp = ksval;
        auto r = cp.reset_if_distinct(val);
        assert(r == 0);
        }
        {
        auto cp = ksval;
        auto r = cp.reset_if_distinct(inv);
        assert(r == -1);
        }
        {
        auto cp = ksinv;
        auto r = cp.reset_if_distinct(inv);
        assert(r == 0);
        }
        {
        auto cp = ksinv;
        auto r = cp.reset_if_distinct(val);
        assert(r == 1);
        }

        assert(is_ok(ksval.verifyx()));
        ksval.reset();
        assert(is_ko(ksval.verifyx()));
        ksval.reset(val2);
        assert(is_ok(ksval.verifyx()));

        {
            priv_t val;
            assert(is_ok(val.generate()));
            cout << val << endl;
            keys kval(val);
            cout << val << endl;
            cout << kval.priv << endl;
            assert(val == kval.priv);
            assert(is_ok(kval.verifyx()));

            assert(keys::get_pubkey(val)==kval.pub);
            assert(is_ok(keys::verifyx(val)));

            priv_t inv;
            assert(is_ko(keys::verifyx(inv)));
        }
        {
            priv_t val;
            assert(is_ok(val.generate()));
            keys kval(val);
            assert(val == kval.priv);
            assert(is_ok(kval.verifyx()));

            keys okeys(move(kval));
            assert(val == okeys.priv);
            assert(is_ok(okeys.verifyx()));
        }

        {
            priv_t val;
            assert(is_ok(val.generate()));
            priv_t valc(val);
            keys kval(move(val));
            assert(valc == kval.priv);
            assert(is_ok(kval.verifyx()));
        }
    }

    void test_trader_personality_defconst() {
        personality_t p;
        cout << "1" << endl;
        assert(!p.k.pub.valid);
        cout << "2" << endl;
        assert(p.moniker=="anonymous");
        cout << "3" << endl;
        assert(p.is_anonymous());
        cout << "4" << endl;
        assert(p.k.priv.is_zero());
        assert(p.id.is_zero());
    }

    void test_trader_personality_const_invsk() {
        priv_t k;
        assert(k.is_zero());

        personality_t p(k, "abc");
        assert(!p.k.pub.valid);
        assert(p.moniker=="abc");
        assert(p.is_anonymous());
        assert(p.k.priv.is_zero());
        assert(p.id.is_zero());
    }

    void test_trader_personality_const_oksk() {
        priv_t k;
        assert(is_ok(k.generate()));
        assert(k.is_not_zero());

        personality_t p(k, "abc");
        assert(p.k.pub.valid);
        assert(p.moniker=="abc");
        assert(!p.is_anonymous());
        assert(p.k.priv.is_not_zero());
        assert(p.k.priv == k);
        assert(p.id.is_not_zero());
    }

    void test_trader_personality_const_ksigndocs() {
        priv_t k;
        assert(is_ok(k.generate()));
        assert(k.is_not_zero());

        {
        personality_t p(k, "abc");
        assert(p.k.priv.is_not_zero());
        assert (p.k_sign_docs() == p.k.priv);
        }

        {
        priv_t k2;
        assert(k2.is_zero());
        personality_t p(k2, "abc");
        assert(p.k.priv.is_zero());
        auto ksign = p.k_sign_docs();
        assert(ksign != p.k.priv);
        assert(ksign.is_not_zero());
        }
    }


    void test_trader_personality_reset_if_distinct() {
        priv_t k;
        assert(is_ok(k.generate()));
        assert(k.is_not_zero());
        auto pubk = keys::get_pubkey(k);
        auto h = pubk.hash();

        priv_t keq{k};
        assert(k == keq);

        priv_t kinv;
        assert(kinv.is_zero());

        {
        personality_t p(k, "abc");
        auto r = p.reset_if_distinct(keq, "abc2");
        assert(r == 0);
        assert(p.moniker=="abc2");
        assert(p.id==h);
        }
        {
        personality_t p(k, "abc");
        assert(!p.is_anonymous());
        auto r = p.reset_if_distinct(kinv, "abc2");
        cout << r << endl;
        assert(r == -1);
        assert(p.moniker=="abc2");
        assert(p.is_anonymous());
        assert(p.id==hash_t(0));
        }
        {
        personality_t p(kinv, "abc");
        assert(p.is_anonymous());
        assert(p.id==hash_t(0));
        auto r = p.reset_if_distinct(k, "abc2");
        assert(r == 1);
        assert(p.moniker=="abc2");
        assert(!p.is_anonymous());
        assert(p.id==h);

        r = p.reset_if_distinct(k, "abc2");
        assert(r == 0);
        assert(p.moniker=="abc2");
        assert(!p.is_anonymous());
        assert(p.id==h);

        r = p.reset_if_distinct(k, "abc");
        assert(r == 0);
        assert(p.moniker=="abc");
        assert(!p.is_anonymous());
        assert(p.id==h);
        }
    }

    void test_trader_personality_reset_if_distinct_b58() {
        tee("test_trader_personality_reset_if_distinct_b58");
        priv_t k;
        assert(is_ok(k.generate()));
        assert(k.is_not_zero());
        auto pubk = keys::get_pubkey(k);
        auto h = pubk.hash();
        string kb58 = k.to_b58();

        priv_t keq{k};
        assert(k == keq);
        string keqb58 = keq.to_b58();

        priv_t kinv;
        assert(kinv.is_zero());
        string kinvb58 = kinv.to_b58();

        {
        personality_t p(k, "abc");
        auto r = p.reset_if_distinct(keqb58, "abc2");
        assert(r == 0);
        assert(p.moniker=="abc2");
        assert(p.id==h);
        }
        {
        personality_t p(k, "abc");
        assert(!p.is_anonymous());
        auto r = p.reset_if_distinct(kinvb58, "abc2");
        cout << r << endl;
        assert(r == -1);
        assert(p.moniker=="abc2");
        assert(p.is_anonymous());
        assert(p.id==hash_t(0));
        }
        {
        personality_t p(kinv, "abc");
        assert(p.is_anonymous());
        assert(p.id==hash_t(0));
        auto r = p.reset_if_distinct(kb58, "abc2");
        assert(r == 1);
        assert(p.moniker=="abc2");
        assert(!p.is_anonymous());
        assert(p.id==h);

        r = p.reset_if_distinct(kb58, "abc2");
        assert(r == 0);
        assert(p.moniker=="abc2");
        assert(!p.is_anonymous());
        assert(p.id==h);

        r = p.reset_if_distinct(kb58, "abc");
        assert(r == 0);
        assert(p.moniker=="abc");
        assert(!p.is_anonymous());
        assert(p.id==h);
        }

        auto g = personality_t::generate("me");
        assert(!g.is_anonymous());
        assert(g.moniker=="me");
    }

    void test_trader_personality_serialization() {
        tee("test_trader_personality_serialization");
        {
        raw_personality_t g;
        test_serialization(g);
        }
        {
        auto g = personality_t::generate("me");
        assert(!g.is_anonymous());
        assert(g.moniker == "me");
        auto ch = personality_t::gen_challenge();
        auto proof = g.gen_proof(ch);
        tee("test_serialization", "proof");
        test_serialization(proof);
        }
    }

    using proof_t = us::wallet::trader::personality::proof_t;


    void test_proof() {  ///I want to very other's proof of identity
        tee("proof-1");
        {
            proof_t pi;
            test_serialization(pi);
        }

        //ME
        auto challenge = personality_t::gen_challenge();
        cout << "challenge " << challenge << endl;
        //send challenge to peer

        //PEER
        personality_t peer = personality_t::generate("peer");
        assert(!peer.is_anonymous());
        assert(peer.moniker == "peer");

        auto pi = peer.gen_proof(challenge);
        assert(pi.id==peer.id);
        assert(pi.moniker=="peer");

        //send proof to me

        //ME

        //should verify
        assert(pi.verify(challenge));

    //    test_proof_serialization(pi);
        tee("proof-2");
        test_serialization(pi);

        tee("proof-2.7");
        //should no verify with onother challenge
        {
            auto ch2 = challenge;
            *reinterpret_cast<uint8_t*>(&ch2[0])^=1;
            assert(!pi.verify(ch2));
        }

        tee("proof-2.8");
        {
            //raw for light storage, discards signature and pubkey
            auto r = pi.raw();
            assert(r.id == pi.id);
            assert(r.moniker == pi.moniker);
            test_serialization(r);
        }

        tee("proof-2.9");
        {
            //copy
            auto cp = pi;
            assert(cp.verify(challenge));
            //tamper copy
            *reinterpret_cast<uint8_t*>(&cp.id)^=1;
            //should not verify
            assert(!cp.verify(challenge));
        }
        tee("proof-3");
        {
            //constr copy
            auto cp(pi);
            assert(cp.verify(challenge));
            //tamper copy
            *reinterpret_cast<uint8_t*>(&cp.id)^=1;
            //should not verify
            assert(!cp.verify(challenge));
        }

        //tamper original
        *reinterpret_cast<uint8_t*>(&pi.id)^=1;
        //should not verify
        assert(!pi.verify(challenge));
        tee("proof-4");
    }


    void test_trader_personality() {
        tee("test_trader_personality");

        test_trader_personality_defconst();
        test_trader_personality_const_invsk();
        test_trader_personality_const_oksk();
        test_trader_personality_const_ksigndocs();
        test_trader_personality_reset_if_distinct();
        test_trader_personality_reset_if_distinct_b58();
        test_trader_personality_serialization();
        tee("test_proof");
        test_proof();
    }

    //using handshake0_t = us::wallet::trader::bootstrap::handshake0_t;
    using handshake_t = us::wallet::trader::bootstrap::handshake_t;
    using a1_t = us::wallet::trader::bootstrap::a1_t;
    using a2_t = us::wallet::trader::bootstrap::a2_t;
    using a3_t = us::wallet::trader::bootstrap::a3_t;
    using b1_t = us::wallet::trader::bootstrap::b1_t;
    using b2_t = us::wallet::trader::bootstrap::b2_t;
    using b3_t = us::wallet::trader::bootstrap::b3_t;

    using params_t = us::wallet::trader::kv;
    using protocols_t = us::wallet::trader::bootstrap::protocols_t;
    using endpoint_t = us::wallet::trader::endpoint_t;
    using protocol_selection_t = us::wallet::trader::protocol_selection_t;

    protocols_t testset_protocols() {
        protocols_t x;
        x.push_back({"pat2ai", "pat"});
        x.push_back({"pat2phy", "pat"});
        x.push_back({"bid2ask", "bid"});
        x.push_back({"pat2slt", "pat"});
        return x;
    }

    protocols_t testset_protocols2() {
        protocols_t x;
        x.push_back({"bid2ask", "ask"});
        return x;
    }

    params_t testset_params() {
        params_t x;
        x.set("initA", "valueA");
        x.set("initB", 12);
        x.set("initC", "valueC");
        return x;
    }

    params_t testset_params2() {
        params_t x;
        x.set("folp1", "BobSponge");
        x.set("folp2", 12);
        x.set("folp3", "valueC++");
        return x;
    }

    a1_t testset_a1() {
        endpoint_t ep("0 192.168.1.1:44000");  //my endpoint
        protocols_t prots = testset_protocols(); //my speakable protocols
        //params_t sh_params = testset_params(); //my local shared params
        a1_t x(move(ep), "", move(prots), personality_t::gen_challenge());
        return x;
    }

    a1_t testset_a1_2() {
        endpoint_t ep("0 192.168.1.1:44000");  //my endpoint
        protocols_t prots = testset_protocols(); //my speakable protocols
        //params_t sh_params = testset_params(); //my local shared params
        a1_t x(move(ep), "fart", move(prots), personality_t::gen_challenge());
        return x;
    }

    a2_t testset_a2(const personality_t& bob, const a1_t& a1) {
        endpoint_t ep("192.168.1.2:44000.mycustodialhome");  //my endpoint
        protocols_t prots = testset_protocols2(); //my speakable protocols
    //    params_t sh_params = testset_params2(); //my local shared params
        auto proof = bob.gen_proof(a1.challenge);
        a2_t x(move(ep), move(prots), move(proof), personality_t::gen_challenge());
        return x;
    }

    a3_t testset_a3(const personality_t& alice, const a1_t& a1, const a2_t& a2) {
        auto proof = alice.gen_proof(a2.challenge);
        a3_t x(move(proof));
        return x;
    }

    b1_t testset_b1() {
        string prot = "bid2ask";
        string role = "ask";
        params_t sh_params = testset_params(); //my local shared params
        b1_t x(protocol_selection_t(move(prot), move(role)), sh_params);
        return x;
    }

    b2_t testset_b2(const personality_t& bob, const b1_t& b1, const a1_t& a1) {
        params_t sh_params = testset_params2(); //my local shared params
        auto proof = bob.gen_proof(a1.challenge);
        b2_t x(move(proof), move(sh_params));
        return x;
    }

    b3_t testset_b3() {
        params_t sh_params = testset_params(); //my local shared params
        b3_t x(move(sh_params));
        return x;
    }

    /*

    void test_trade_bootstrap_handshake0() {
        {
            handshake0_t h0;
            test_serialization(h0);
        }

    }

    void test_trade_bootstrap_handshake() {
        handshake_t h;
        test_serialization(h);
    }
    */
    personality_t alice = personality_t::generate("alice");
    personality_t bob = personality_t::generate("bob");

    void test_trade_bootstrap_a() {
        {
        a1_t a1;
        test_serialization(a1);
        a2_t a2;
        test_serialization(a2);
        a3_t a3;
        test_serialization(a2);
        }
        {
        a1_t a1 = testset_a1();
        test_serialization(a1);
        a2_t a2 = testset_a2(bob, a1);
        test_serialization(a2);
        a3_t a3 = testset_a3(alice, a1, a2);
        test_serialization(a3);
        }
    }

    void test_trade_bootstrap_b() {
        {
        b1_t b1;
        test_serialization(b1);
        b2_t b2;
        test_serialization(b2);
        b3_t b3;
        test_serialization(b2);
        }
        {
        a1_t a1 = testset_a1_2();
        test_serialization(a1);
        a2_t a2 = testset_a2(bob, a1);
        b1_t b1 = testset_b1();
        test_serialization(b1);
        b2_t b2 = testset_b2(bob, b1, a1);
        test_serialization(b2);
        b3_t b3 = testset_b3();
        test_serialization(b3);
        }
    }
    /*
    void test_trade_bootstrap_hs_1b() {
        hs_1b_t hs_1b;
        test_serialization(hs_1b);
    }
    */

    tuple<a1_t, a2_t, a3_t> test_trade_bootstrap_3way_handshake_a() { //--no protocol initiated

        //================================================================
        //A// 1.- Initiator(Alice) - 1a
            a1_t a1 = testset_a1();
            {
                blob_t msg;
                a1.write(msg);
                cout << "1.- Alice sends: " << msg.size() << endl;
            }

        //B// 2.- Bob(follower) - receives handshake, generates step 2 and sends over;
            a2_t a2 = testset_a2(bob, a1);
            {
                blob_t msg;
                a2.write(msg);
                cout << "2.- Bob sends: " << msg.size() << endl;
            }

        //A// 3.- Alice (initiator) - receives a2, generates a3 using a1 and a2 and sends over;
            {
                //verify bob's personality
                auto my_challenge = a1.challenge; //I Alice created a2
                assert(a2.personality_proof.verify(my_challenge));
                //ok. get the raw identity
                auto bob_id = a2.personality_proof.raw();
                bob_id.dump(cout);
                assert(bob_id.moniker == "bob");
            }

            //let's give them our proof using the challenge sent by bob
            a3_t a3 = testset_a3(alice, a1, a2);
            {
                blob_t msg;
                a3.write(msg);
                cout << "3.- Alice sends: " << msg.size() << endl;
            }

        //B// 4.- Bob receives a3 checks Alice's Id

        {
            //verify alice's personality
            auto my_challenge = a2.challenge; //I Bob created a2
            assert(a3.personality_proof.verify(my_challenge));
            //ok. get the raw identity
            auto alice_identity = a3.personality_proof.raw();
            alice_identity.dump(cout);
            assert(alice_identity.moniker=="alice");
        }

        /// done -----------
        assert(a3.personality_proof.id == alice.id);
        assert(a3.personality_proof.moniker == alice.moniker);
        assert(a3.personality_proof.raw().id == alice.id);
        assert(a3.personality_proof.raw().moniker == alice.moniker);
        assert(a2.personality_proof.id == bob.id);
        assert(a2.personality_proof.moniker == bob.moniker);
        assert(a2.personality_proof.raw().id == bob.id);
        assert(a2.personality_proof.raw().moniker == bob.moniker);

        return make_tuple(a1, a2, a3);

    }

    void test_trade_bootstrap_3way_handshake_b(tuple<a1_t, a2_t, a3_t>&& a) { //--no protocol initiated

        //================================================================
        //B// 1.- Alice - 1a
            b1_t b1 = testset_b1();
            {
                blob_t msg;
                b1.write(msg);
                cout << "1.- Alice sends: " << msg.size() << endl;
            }

        //B// 2.- Bob - instanciate protocol resulting in update on personality and params
            b2_t b2 = testset_b2(bob, b1, get<0>(a));
            {
                blob_t msg;
                b2.write(msg);
                cout << "2.- Bob sends: " << msg.size() << endl;
            }

        //B// 3.- Alice - receives b2, resultin on update on params
            b3_t b3 = testset_b3();
            {
                blob_t msg;
                b3.write(msg);
                cout << "3.- Alice sends: " << msg.size() << endl;
            }

    }

    void test_trade_bootstrap() {
    //    test_trade_bootstrap_handshake0();
    //    test_trade_bootstrap_handshake();
    //    test_trade_bootstrap_hs_1b();
        test_trade_bootstrap_a();
        test_trade_bootstrap_b();

        auto r = test_trade_bootstrap_3way_handshake_a();
        test_trade_bootstrap_3way_handshake_b(move(r));

    }

    void test_older_version_algorithm(version_fingerprint_t me, version_fingerprint_t peer, bool ev) {
        auto r = us::gov::id::peer_t::am_I_older(me, peer);
        assert(r == ev);
    }
    // [0-86)
    // [86-119)
    // [119-256)
    void test_older_version_algorithm() {
        version_fingerprint_t me;
        version_fingerprint_t peer;
        test_older_version_algorithm(0, 0, false);
        test_older_version_algorithm(0, 1, true);
        test_older_version_algorithm(0, 85, true);
        test_older_version_algorithm(0, 86, true);
        test_older_version_algorithm(0, 87, true);
        test_older_version_algorithm(0, 118, true);
        test_older_version_algorithm(0, 119, false);
        test_older_version_algorithm(0, 120, false);

        test_older_version_algorithm(1, 0, false);
        test_older_version_algorithm(1, 1, false);
        test_older_version_algorithm(1, 85, true);
        test_older_version_algorithm(1, 86, true);
        test_older_version_algorithm(1, 87, true);
        test_older_version_algorithm(1, 118, true);
        test_older_version_algorithm(1, 119, false);
        test_older_version_algorithm(1, 120, false);
        
        test_older_version_algorithm(85, 0, false);
        test_older_version_algorithm(85, 1, false);
        test_older_version_algorithm(85, 85, false);
        test_older_version_algorithm(85, 86, true);
        test_older_version_algorithm(85, 87, true);
        test_older_version_algorithm(85, 118, true);
        test_older_version_algorithm(85, 119, false);
        test_older_version_algorithm(85, 120, false);
        test_older_version_algorithm(85, 254, false);
        test_older_version_algorithm(85, 255, false);

        test_older_version_algorithm(86, 0, false);
        test_older_version_algorithm(86, 1, false);
        test_older_version_algorithm(86, 85, false);
        test_older_version_algorithm(86, 86, false);
        test_older_version_algorithm(86, 87, true);
        test_older_version_algorithm(86, 118, true);
        test_older_version_algorithm(86, 119, true);
        test_older_version_algorithm(86, 120, true);
        test_older_version_algorithm(86, 254, true);
        test_older_version_algorithm(86, 255, true);

        test_older_version_algorithm(118, 0, false);
        test_older_version_algorithm(118, 1, false);
        test_older_version_algorithm(118, 85, false);
        test_older_version_algorithm(118, 86, false);
        test_older_version_algorithm(118, 87, false);
        test_older_version_algorithm(118, 118, false);
        test_older_version_algorithm(118, 119, true);
        test_older_version_algorithm(118, 120, true);
        test_older_version_algorithm(118, 254, true);
        test_older_version_algorithm(118, 255, true);

        test_older_version_algorithm(119, 0, true);
        test_older_version_algorithm(119, 1, true);
        test_older_version_algorithm(119, 85, true);
        test_older_version_algorithm(119, 86, false);
        test_older_version_algorithm(119, 87, false);
        test_older_version_algorithm(119, 118, false);
        test_older_version_algorithm(119, 119, false);
        test_older_version_algorithm(119, 120, true);
        test_older_version_algorithm(119, 254, true);
        test_older_version_algorithm(119, 255, true);

        test_older_version_algorithm(0, 1, true);
        test_older_version_algorithm(1, 0, false);
        test_older_version_algorithm(1, 255, false);
        test_older_version_algorithm(1, 254, false);
        test_older_version_algorithm(10, 254, false);

    //cout << "==========================" << endl;
    //assert(false);
    }

    void test_ripemd160() {
        hash_t z(0);
        assert(z.is_zero());
        assert(!z.is_not_zero());
        hash_t one(1);
        assert(!one.is_zero());
        assert(one.is_not_zero());
        assert(z != one);
        assert(!(z == one));
        auto copy = z;
        assert(copy == z);
        auto copy2 = one;
        assert(copy2 == one);
        ostringstream os;
        os << z << ' ' << one;
        cout << os.str() << endl;
        assert(os.str() == "11111111111111111111 11111111111111112UzHM");
    }

    void test_ec() {
        test_priv();
        test_pub();
        test_keys();
    }

    void test_pointers() { //Morello board
        struct type_t { int x; };
        auto i11 = new type_t();
        auto i12 = i11;
        auto i21 = new type_t();
        auto i22 = i21;
        assert(i11 == i12);
        assert(i11 != i21);
        assert(i11 != i22);
        assert(i12 != i21);
        assert(i12 != i22);
        assert(i21 == i22);
        assert(i21 != i11);
        assert(i21 != i12);
        assert(i22 != i11);
        assert(i22 != i12);
        delete i11;
        delete i21;
    }

    void test_extract_protocol_selection(const string& filename) {
        using libs_t = us::wallet::trader::libs_t;
        auto r = libs_t::extract_protocol_selection(filename);
        assert(!r.is_set());
    }

    void test_extract_protocol_selection(const string& filename, protocol_selection_t&& expect) {
        tee(filename, expect.to_string2());
        using libs_t = us::wallet::trader::libs_t;
        auto r = libs_t::extract_protocol_selection(filename);
        tee("got:", r.to_string2(), "expected:", expect.to_string2());
        assert(r.is_set() == expect.is_set());
        assert(r == expect);
    }

    void test_extract_protocol_selection() {
        test_extract_protocol_selection("");
        test_extract_protocol_selection("so");
        test_extract_protocol_selection(".so");
        test_extract_protocol_selection(".so.so");
        test_extract_protocol_selection("--");
        test_extract_protocol_selection("--.so");
        test_extract_protocol_selection("- -.so");
        test_extract_protocol_selection(" - -.so");
        test_extract_protocol_selection(" - -");
        test_extract_protocol_selection(" --");
        test_extract_protocol_selection("lib--.");
        test_extract_protocol_selection("lib--.so");
        test_extract_protocol_selection("lib-p-.so");
        test_extract_protocol_selection("lib-p-r.so", protocol_selection_t("p", "r"));
        test_extract_protocol_selection("libp-r.so");
        test_extract_protocol_selection("libxxxtrader-protocol-role.so", protocol_selection_t("protocol", "role"));
//assert(false);
    }

    void self_test() {
        cout << "pointers (classic/fat)" << endl;
        test_pointers();
        cout << "trim" << endl;
        test_trim();
        cout << "path_fs" << endl;
        test_path_fs();
        cout << "split_fqn" << endl;
        test_split_fqn();
        cout << "datadiff" << endl;
        test_datadiff();
        cout << "hash_t" << endl;
        test_ripemd160();
        cout << "ec" << endl;
        test_ec();
        cout << "client::endpoint" << endl;
        test_endpoint();
        cout << "trader::endpoint" << endl;
        test_trader_endpoint();
        cout << "bookmarks" << endl;
        test_bookmarks();
        cout << "trader::personality" << endl;
        test_trader_personality();
        cout << "trader::bootstrap" << endl;
        test_trade_bootstrap();
        cout << "older_version_algorithm" << endl;
        test_older_version_algorithm();
    //cout << "================================= OK ================================" << endl;
    //assert(false);
        cout << "chat" << endl;
        test_chat();
        cout << "extract_protocol_selection" << endl;
        test_extract_protocol_selection();
        
    }

};

void test_things() {
    things_t things;
    things.self_test();
}

}

