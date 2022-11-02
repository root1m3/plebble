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
#include <chrono>
#include <random>

#include <us/gov/config.h>
#include <us/gov/crypto/types.h>
#include <us/gov/types.h>
#include <us/gov/traders/wallet_address.h>
#include <us/gov/cash/accounts_t.h>
#include <us/gov/cash/account_t.h>
#include <us/gov/cash/t_t.h>
#include <us/gov/cash/f_t.h>
#include <us/gov/cash/ttx.h>
#include <us/gov/io/types.h>
#include <us/gov/io/seriable_vector.h>
#include <us/gov/dfs/fileattr_t.h>
#include <us/gov/engine/signed_data.h>
#include <us/gov/traders/app.h>

#include <us/wallet/wallet/algorithm.h>
#include <us/wallet/engine/wallet_connection_t.h>
#include <us/wallet/engine/ip4_endpoint_t.h>

#include "test_platform.h"

#define loglevel "test"
#define logclass "blobs"
#include "logs.inc"

#include "assert.inc"

namespace us::test {

using namespace std;
using namespace us;
using namespace us::gov;

struct blobs_t: us::test::test_platform {
    using b = us::test::test_platform;

    blobs_t(): rng(random_device{}()), b(cout) {
    }


    template<typename t>
    void test_blob_serializer(t v, int expected_sz) {
        cout << "blob_serializer " << +v << ' ' << " expected sz: " << expected_sz << endl;
        using blob_reader_t = us::gov::io::blob_reader_t;
        using blob_writer_t = us::gov::io::blob_writer_t;
        using blob_t = us::gov::io::blob_t;

        blob_t blob;
        blob_writer_t w(blob, blob_writer_t::blob_size(v));
        w.write(v);
        if (blob.size() != expected_sz) {
            cout << "blob.size() = " << blob.size() << endl;
        }
        assert(blob.size() == expected_sz);

        t v2;
        blob_reader_t r(blob);
        r.read(v2);
        if (v != v2) {
            cout << "read v = " << v2 << endl;
        }
        assert(v == v2);
    }

    void test_blob_serializer_sz(uint64_t v, int expected_sz) {
        cout << "blob_serializer " << +v << ' ' << " expected sz: " << expected_sz << endl;
        using blob_reader_t = us::gov::io::blob_reader_t;
        using blob_writer_t = us::gov::io::blob_writer_t;
        using blob_t = us::gov::io::blob_t;
        cout << "sizet_size(v)= " << blob_writer_t::sizet_size(v) << " expected " << expected_sz << endl;
        assert(blob_writer_t::sizet_size(v) == expected_sz);

        blob_t blob;
        blob_writer_t w(blob, blob_writer_t::sizet_size(v));
        w.write_sizet(v);
        if (blob.size() != expected_sz) {
            cout << "blob.size() = " << blob.size() << endl;
        }
        assert(blob.size() == expected_sz);

        uint64_t v2;
        blob_reader_t r(blob);
        r.read_sizet(v2);
        if (v != v2) {
            cout << "read v = " << v2 << endl;
        }
        assert(v == v2);
    }

    void test_blob_serializer(const hash_t& v) {
        using t = hash_t;
        int expected_sz = 20;
        cout << "blob_serializer " << v << ' ' << " expected sz: " << expected_sz << endl;
        using blob_reader_t = us::gov::io::blob_reader_t;
        using blob_writer_t = us::gov::io::blob_writer_t;
        using blob_t = us::gov::io::blob_t;

        blob_t blob;
        blob_writer_t w(blob, blob_writer_t::blob_size(v));
        w.write(v);
        if (blob.size() != expected_sz) {
            cout << "blob.size() = " << blob.size() << endl;
        }
        assert(blob.size() == expected_sz);

        t v2;
        blob_reader_t r(blob);
        r.read(v2);
        if (v != v2) {
            cout << "read v = " << v2 << endl;
        }
        assert(v == v2);
    }

    void test_blob_serializer2(const string& v, int expected_sz) {
        using t = string;
        cout << "blob_serializer " << v.size() << ' ' << " expected sz: " << expected_sz << endl;
        using blob_reader_t = us::gov::io::blob_reader_t;
        using blob_writer_t = us::gov::io::blob_writer_t;
        using blob_t = us::gov::io::blob_t;

        blob_t blob;
        blob_writer_t w(blob, blob_writer_t::blob_size(v));
        w.write(v);
        if (blob.size() != expected_sz) {
            cout << "blob.size() = " << blob.size() << endl;
        }
        assert(blob.size() == expected_sz);

        t v2;
        blob_reader_t r(blob);
        r.read(v2);
        if (v != v2) {
            cout << "read v = " << v2 << endl;
        }
        assert(v == v2);
    }

    template<typename t>
    void test_blob_serializer_64() {
        test_blob_serializer((t) 0, 8);
        test_blob_serializer((t) 1, 8);
        test_blob_serializer((t) -1, 8);
        test_blob_serializer((t) 120, 8);
        test_blob_serializer((t) -120, 8);
        test_blob_serializer((t) 121, 8);
        test_blob_serializer((t) -121, 8);
        test_blob_serializer((t) 122, 8);
        test_blob_serializer((t) -122, 8);
        test_blob_serializer((t) 123, 8);
        test_blob_serializer((t) -123, 8);
        test_blob_serializer((t) 124, 8);
        test_blob_serializer((t) -124, 8);
        test_blob_serializer((t) 125, 8);
        test_blob_serializer((t) -125, 8);
        test_blob_serializer((t) 126, 8);
        test_blob_serializer((t) -126, 8);
        test_blob_serializer((t) 127, 8);
        test_blob_serializer((t) -127, 8);
        test_blob_serializer((t) 128, 8);
        test_blob_serializer((t) -128, 8);
        test_blob_serializer((t) 32766, 8);
        test_blob_serializer((t) -32766, 8);
        test_blob_serializer((t) 32767, 8);
        test_blob_serializer((t) -32767, 8);
        test_blob_serializer((t) 32768, 8);
        test_blob_serializer((t) -32768, 8);
        test_blob_serializer((t) 32769, 8);
        test_blob_serializer((t) -32769, 8);
        test_blob_serializer((t) 2147483647, 8);
        test_blob_serializer((t) -2147483647, 8);
        test_blob_serializer((t) 2147483648, 8);
        test_blob_serializer((t) -2147483648, 8);
        test_blob_serializer((t) numeric_limits<t>::max(), 8);
    }

    void test_blob_serializer() {
        cout << "uint8_t" << endl;
        test_blob_serializer((uint8_t) 0, 1);
        test_blob_serializer((uint8_t) 1, 1);
        test_blob_serializer((uint8_t) 0xfc, 1);
        test_blob_serializer((uint8_t) 0xfd, 1);
        test_blob_serializer((uint8_t) 0xfe, 1);
        test_blob_serializer((uint8_t) 0xff, 1);

        cout << "uint16_t" << endl;
        test_blob_serializer((uint16_t) 0, 2);
        test_blob_serializer((uint16_t) 1, 2);
        test_blob_serializer((uint16_t) 0xfc, 2);
        test_blob_serializer((uint16_t) 0xfd, 2);
        test_blob_serializer((uint16_t) 0xfe, 2);
        test_blob_serializer((uint16_t) 0xff, 2);

        cout << "uint32_t" << endl;
        test_blob_serializer((uint32_t) 0, 4);
        test_blob_serializer((uint32_t) 1, 4);
        test_blob_serializer((uint32_t) 0xfc, 4);
        test_blob_serializer((uint32_t) 0xfd, 4);
        test_blob_serializer((uint32_t) 0xfe, 4);
        test_blob_serializer((uint32_t) 0xff, 4);


        cout << "blob_size" << endl;
        test_blob_serializer_sz(0, 1);
        test_blob_serializer_sz(1, 1);
        test_blob_serializer_sz(0xfc, 1);
        test_blob_serializer_sz(0xfd, 3);
        test_blob_serializer_sz(0xfe, 3);
        test_blob_serializer_sz(0xff, 3);
        test_blob_serializer_sz(0xffff, 3);
        test_blob_serializer_sz(0x10000, 5);
        test_blob_serializer_sz(0xfffffffe, 5);
        test_blob_serializer_sz(0xffffffff, 5);
        test_blob_serializer_sz(0x100000000, 9);
        test_blob_serializer_sz(0xffffffffffffffff, 9);

        cout << "int64_t" << endl;
        test_blob_serializer_64<int64_t>();
        test_blob_serializer_64<uint64_t>();

        //test_blob_serializer((int64_t) numeric_limits<int64_t>::min(), 9);
    /*
        for (int64_t i = 2147483638; i < numeric_limits<int64_t>::max(); i+=1) {
            test_blob_serializer((int64_t) i, 5);
            test_blob_serializer((int64_t) -i, 5);
        }
    */
    //    test_blob_serializer((int64_t) -128, 3);

        test_blob_serializer(hash_t(0));
        test_blob_serializer(hash_t(23445));
        test_blob_serializer2("", 1);
        test_blob_serializer2("a", 2);
        test_blob_serializer2("aa", 3);
        test_blob_serializer2(string(500, 'a'), 503);
        test_blob_serializer2(string(50000, 'a'), 50003);
        test_blob_serializer2(string(500000, 'a'), 500005);

    }

    void test_ev_ser_wallet_address() {
        cout << "gov::io::blob" << endl;
        test_blob_serializer();

        cout << "test unsigned" << endl;
        {
            hash_t x(0);
            us::gov::traders::wallet_address ev(x, 123, 456);
            ev.pubkey.zero();
            blob_t blob;
    //        auto sz = ev.blob_size();
    //        cout << sz << endl;
    //        unsigned char buffer[sz];
            ev.write(blob);
            us::gov::traders::wallet_address ev2(hash_t(0), 0, 0);
            {
                assert(is_ok(ev2.read(blob)));
            }
    //        auto p = ev2.read_from(buffer, buffer+sz);
    //        assert(p == buffer + sz);
    //        unsigned char buffer2[sz];
            blob_t blob2;
            ev2.write(blob2);
            assert(blob.size() == blob2.size());
            assert(memcmp(blob.data(), blob2.data(), blob.size())==0);
            assert(ev2.pport == ev.pport);
            assert(ev2.net_addr == ev.net_addr);
            assert(ev2.eid == ev.eid);
//            assert(ev2.expires == ev.expires);
            cout << "A:" << ev.pubkey << " B:" <<  ev2.pubkey << endl;
            assert(ev2.pubkey == ev.pubkey);
            assert(ev2.signature == ev.signature);
            ostringstream os;
            cout << "verif: " << ev.verify(os) << endl;
            cout << os.str() << endl;
            assert(os.str() == "KO 02101 empty signature.\n");
            assert(!ev.verify(cout));
            assert(!ev2.verify(cout));
        }
        cout << "\ntest well formed" << endl;
        {
            hash_t x(47638);
            us::gov::traders::wallet_address ev(x, 6542, 5903);
            assert(ev.app == us::gov::traders::app::id());
            assert(ev.eid == 0);
//            ev.expires = ev.ts + 10000000068859;
            auto k = crypto::ec::keys::generate();
            ev.sign(k);
            cout << "ev pubkey=" << ev.pubkey << endl;
    //        auto sz = ev.ser_sz();
    //        cout << sz << endl;
            blob_t blob;
    //        unsigned char buffer[sz];
            ev.write(blob);
            us::gov::traders::wallet_address ev2(hash_t(0), 0, 0);
            assert(is_ok(ev2.read(blob)));
            blob_t blob2;
            ev2.write(blob2);
            for (int i = 0; i < blob.size(); ++i) {
                //cout << "pos=" << i << ' ' << (int)buffer[i] << ' ' << (int)buffer2[i] << '\n';
                assert(blob[i] == blob2[i]);
            }
            assert(memcmp(blob.data(), blob2.data(), blob.size())==0);
            assert(ev2.pport == ev.pport);
            assert(ev2.pport == 5903);
            assert(ev2.net_addr == ev.net_addr);
            assert(ev2.net_addr == 6542);
            assert(ev2.eid == ev.eid);
            assert(ev2.eid == 0);
            assert(ev2.app == ev.app);
            assert(ev2.app == us::gov::traders::app::id());
//            assert(ev2.expires == ev.expires);
//            assert(ev2.expires == ev.ts + 10000000068859);
//            assert(ev2.expires == ev2.ts + 10000000068859);
            assert(ev2.pubkey == ev.pubkey);
            cout << ev.pubkey << endl;
            cout << ev2.pubkey << endl;
            cout << k.pub << endl;
            assert(ev2.pubkey == k.pub);
            assert(ev2.pkh == x);
            assert(ev2.signature == ev.signature);
            assert(ev.verify(cout));
            assert(ev2.verify(cout));

    //        cout << "ser bin: " << sz << " bytes" << endl;
    //        ostringstream os;
    //        ev.to_stream(os);
    //        cout << "ser os: " << os.str().size() << " bytes" << endl;
        }

        cout << "\ntest datagram" << endl;
        {
            hash_t x(47638);
            us::gov::traders::wallet_address ev(x, 6542, 5903);
            assert(ev.app == us::gov::traders::app::id());
            assert(ev.eid == 0);
//            ev.expires = ev.ts + 10000000068859;
            auto k = crypto::ec::keys::generate();
            ev.sign(k);
            cout << "ev pubkey=" << ev.pubkey << endl;
    //        auto sz = ev.ser_sz();
    //        cout << sz << endl;
            auto d = ev.get_datagram(0, 6);
    //        unsigned char buffer[sz];
            us::gov::traders::wallet_address ev2(hash_t(0), 0, 0);
            {
                auto r = ev2.read(*d);
                if (is_ko(r)) cout << r << endl;
                assert(is_ok(r));
            }
            auto d2 = ev2.get_datagram(0, 6);
            assert(d->size() == d2->size());
            for (int i = 0; i < d->size(); ++i) {
                //cout << "pos=" << i << ' ' << (int)buffer[i] << ' ' << (int)buffer2[i] << '\n';
                assert((*d)[i] == (*d2)[i]);
            }
            assert(memcmp(d->data(), d2->data(), d->size())==0);
            assert(ev2.pport == ev.pport);
            assert(ev2.pport == 5903);
            assert(ev2.net_addr == ev.net_addr);
            assert(ev2.net_addr == 6542);
            assert(ev2.eid == ev.eid);
            assert(ev2.eid == 0);
            assert(ev2.app == ev.app);
            assert(ev2.app == us::gov::traders::app::id());
///            assert(ev2.expires == ev.expires);
//            assert(ev2.expires == ev.ts + 10000000068859);
//            assert(ev2.expires == ev2.ts + 10000000068859);
            assert(ev2.pubkey == ev.pubkey);
            cout << ev.pubkey << endl;
            cout << ev2.pubkey << endl;
            cout << k.pub << endl;
            assert(ev2.pubkey == k.pub);
            assert(ev2.pkh == x);
            assert(ev2.signature == ev.signature);
            assert(ev.verify(cout));
            assert(ev2.verify(cout));
            delete d;
            delete d2;
    //        cout << "ser bin: " << sz << " bytes" << endl;
    //        ostringstream os;
    //        ev.to_stream(os);
    //        cout << "ser os: " << os.str().size() << " bytes" << endl;
        }
    }

    using accounts_t = us::gov::cash::accounts_t;
    using account_t = us::gov::cash::account_t;

    void test_ev_ser_cash_accounts(accounts_t& o) {
        blob_t blob;
        o.write(blob);
        accounts_t o2;
        assert(is_ok(o2.read(blob)));
        blob_t blob2;
        o2.write(blob2);
        accounts_t o3;
        assert(is_ok(o3.read(blob2)));
        assert(o.size() == o3.size());
        check(blob, blob2);
        cout << blob.size() << endl;;
    }

    void test_ev_ser_cash_accounts() {
        {
            accounts_t o;
            test_ev_ser_cash_accounts(o);
        }
        {
            accounts_t o;
            {
                account_t a;
                a.locking_program = 4;
                a.box.value = 889;
                {
                    using t_t = us::gov::cash::t_t;
                    a.box.t = new t_t();
                }
                o.emplace(hash_t(6059), a);
            }
            test_ev_ser_cash_accounts(o);
        }
        {
            accounts_t o;
            {
                account_t a;
                a.locking_program = 4;
                a.box.value = 889;
                {
                    using t_t = us::gov::cash::t_t;
                    a.box.t = new t_t();
                    a.box.t->emplace(hash_t(234756), 6489);
                }
                o.emplace(hash_t(6059), a);
            }
            {
                account_t a;
                a.locking_program = 2;
                a.box.value = 819;
                {
                    using t_t = us::gov::cash::t_t;
                    using f_t = us::gov::cash::f_t;
                    a.box.t = new t_t();
                    a.box.t->emplace(hash_t(217766), 54489);
                    a.box.f = new f_t();
                    a.box.f->emplace(hash_t(3839), us::gov::dfs::fileattr_t("gdsgfsdg", 35656));
                }
                o.emplace(hash_t(2059), a);
            }
            test_ev_ser_cash_accounts(o);
        }
    }

    using signed_data0 = us::gov::engine::signed_data0;

    void test_signed_data(signed_data0& o) {
        blob_t blob;
        o.write(blob);
        signed_data0 o2;
        assert(is_ok(o2.read(blob)));
        blob_t blob2;
        o2.write(blob2);
        signed_data0 o3;
        assert(is_ok(o3.read(blob2)));
        assert(o.pubkey == o3.pubkey);
        assert(o.signature == o3.signature);
        check(blob, blob2);
        cout << blob.size() << endl;;
    }

    void test_signed_data() {
        tee("test_signed_data");
        {
            signed_data0 o;
            test_signed_data(o);
        }
        auto k = us::gov::crypto::ec::keys::generate();
        {
            signed_data0 o;
            {
                o.pubkey = k.pub;
            }
            test_signed_data(o);
        }
        {
            signed_data0 o;
            {
                o.pubkey = k.pub;
                o.signature.data[16] = 'A';
            }
            test_signed_data(o);
        }
        {
            string sigder = "381yXYmWWMTYPsDYLYmk6cNKa5tEWVrnHmRs9HH5dJ4NdGmC9si9Ds1KG2aSLTa6hzeedVnsrUywcopXH3Nde7ELUSexR2QX";
            auto sig = us::gov::crypto::ec::instance.sig_from_der(sigder);
            auto der = us::gov::crypto::ec::instance.sig_encode_der_b58(sig);
            assert(sigder == der);

            signed_data0 o;
            {
                o.pubkey = k.pub;
                o.signature = sig;
            }
            test_signed_data(o);
        }
    }

    void test_bin_ser() {
        test_ev_ser_wallet_address();
        test_ev_ser_cash_accounts();
        test_signed_data();
    }

    void test_datagram_parse(int mode_) {
        //mode = mode_;
        /*
        cout << "test_datagram_parse. mode=" << mode << endl;
        {
          cout << "X dgram parse. payload=v{hu4u2}" << endl;
          vector<tuple<hash_t, uint32_t, uint16_t>> i_in = create_instance<vector<tuple<hash_t, uint32_t, uint16_t>>>();
          us::gov::socket::datagram d(1, 0,    i_in);
          vector<tuple<hash_t, uint32_t, uint16_t>> i_out;
          assert(d.parse(i_out));
          check_x(i_in, i_out);
        }
        */
        //#include <us/api/apitool_generated__c++__datagram_tests_impl>
    }

    using wallet_connection_t = us::wallet::engine::wallet_connection_t;
    using serid_t = us::gov::io::blob_reader_t::serid_t;
    using ip4_endpoint_t = us::wallet::engine::ip4_endpoint_t;
    using blob_writer_t = us::gov::io::blob_writer_t;
    using blob_reader_t = us::gov::io::blob_reader_t;
    using blob_header_t = us::gov::io::blob_writer_t::blob_header_t;

    struct wrapper_wallet_connection_t: wallet_connection_t {
        using b = wallet_connection_t;

        wrapper_wallet_connection_t(): wallet_connection_t("nm1", ip4_endpoint_t((shost_t)"192.167.66.3", (port_t)18782, (channel_t)0)) {
            ts = 52;
            addr = "addr_1";
            subhome = "subhome_1";
            ssid = "ssid_1";
            
        }

        size_t blob_size() const override {
            return b::blob_size();
        }

        void to_blob(blob_writer_t& w) const override {
            b::to_blob(w);
        }

        ko from_blob(blob_reader_t& reader) override {
            {
                auto r = b::from_blob(reader);
                if (is_ko(r)) return r;
            }
            return ok;
        }

    };

    struct container_wc_t: us::gov::io::seriable_vector<wrapper_wallet_connection_t> {
        using b = us::gov::io::seriable_vector<wrapper_wallet_connection_t>;

        using b::seriable_vector;

        serid_t serial_id() const override { return 'X'; }

    };

    string container_of_5_wrapper_wallet_connection_a50_curblob;

    void test_wallet_connection() {
        container_wc_t container_wc(5, wrapper_wallet_connection_t());
        blob_t blob;
        container_of_5_wrapper_wallet_connection_a50_curblob = container_wc.encode();        
        cout << "container_of_5_wrapper_wallet_connection_a50_curblob=" << container_of_5_wrapper_wallet_connection_a50_curblob << endl;
    }

    void wrapper_test_wallet_connection2() {
        string container_of_5_wrapper_wallet_connection_t_a49_blob = "24HGz9KDSkRWJvUzGCe2fJkngCb8v7XfmMMjnLtWXds1YaL5RtMcjTxoCZCGQKwsvCt7YJKRBDTq3k9aYoXzRvqMx4aModzMpjTqZduRBVbFbh9vyNsTC9jVLtER2Rod1WExpYC4My4ikxGMvrz7ZTtwDgoNAh9eTnPR1P8uqGLvErrNEXgDSbzHMEhgDv19Ckd9myzgXuDmRXMBxbr4p8obcwKGMDPR5ALCcdBvgWehGPqvhFZmXYwtrtedJ7zaJ7HB3AXLMtS2Fi9ZbeBc5orcBFycV7275nNcV1h";
        string container_of_5_wrapper_wallet_connection_t_a50_blob = "3JxT8NUbu3u9xYXNVCwQi1kqTuCJyiAQBc8h3kTvBTKVz9w898JiC1HAyB4eHaZiYQnjzAGCpWmATosNReVAeKATNz4MDdGEKUFcVANCpnu1V8Y2dHKFApuzxnKqJweYJ758hrRijLxKNtPkaKuW2r9b4HDHAtHHerUTaKsHDCBpAozpZ43taHWvtC8kBcGgVwonmT8FhoXCaiEsWWQJWkvStCtarrEbYAL9Q6jyVUDnLw3hDRa2JoFVdpZvwfa7mn1sKBrDUvnEFTRWKa2j1rVrhgiAHToYNQsW9bLCdVoQywCfYRNchKu25eKV1puU8kexK1uuf5bcp5qH6VgVFQQEQikydSeamY4621DnwUZiCF";
        cout << "container_of_5_wrapper_wallet_connection_t_a49_blob=" << container_of_5_wrapper_wallet_connection_t_a49_blob << endl;
        cout << "container_of_5_wrapper_wallet_connection_t_a50_blob=" << container_of_5_wrapper_wallet_connection_t_a50_blob << endl;
        assert(container_of_5_wrapper_wallet_connection_t_a50_blob == container_of_5_wrapper_wallet_connection_a50_curblob);
        container_wc_t cw;
        assert(is_ko(cw.read(container_of_5_wrapper_wallet_connection_t_a49_blob)));

        string a49h = blob_writer_t::add_header(blob_header_t{blob_reader_t::current_version - 1, 'X'}, container_of_5_wrapper_wallet_connection_t_a49_blob);
        cout << "a49h = {blob_reader_t::current_version - 1}'X' + container_of_5_wrapper_wallet_connection_t_a49_blob = " << a49h << endl;
        ko r = cw.read(a49h);
        if (is_ko(r)) {
            tee(r);
        }
        assert(is_ok(r));
        assert(cw.size() == 5);

        assert(is_ok(cw.read(container_of_5_wrapper_wallet_connection_t_a50_blob)));
        assert(cw.size() == 5);
    }

    void wrapper_test_wallet_connection() {
        string wallet_connection_t_a49_blob="XvkfoSNzekbT3FwZdtp99Mg1aqRhkJ3EmvvMm66pddv9aBHdonSAqfD";
        wallet_connection_t wc;
        assert(is_ko(wc.read(wallet_connection_t_a49_blob)));
    }

    void convert_fromv9(const string& blobb58_v9) {
        string a49h = blob_writer_t::add_header(blob_header_t{blob_reader_t::current_version - 1, 'X'}, blobb58_v9);
        cout << "--- convert-str95------------" << endl;
        cout << "input: " << blobb58_v9 << endl;
        cout << "a49h = {blob_reader_t::current_version - 1}'X' + blobb58_v9 = " << a49h << endl;

        struct wc_t: wallet_connection_t {
            serid_t serial_id() const override { return 'X'; }
        };

        wc_t wc;
        ko r = wc.read(a49h);
        if (is_ko(r)) {
            tee(r);
        }
        assert(is_ok(r));
        wc.subhome = "new";

        wallet_connection_t wc2(wc); //serialization with no header
        cout << "a50= " << wc2.encode() << endl;
        cout << "-/- convert------------------" << endl;
    }

    void test() {
        test_bin_ser();
        test_datagram_parse(0);
        test_datagram_parse(1);
        test_datagram_parse(2);
        test_wallet_connection();
        wrapper_test_wallet_connection();
        wrapper_test_wallet_connection2();
        /*
        convert_fromv9("Sk8adPorUZdLvsqufDQdnCp2r2KDABwmcKXwDYKJrmsQEEbSxe5jxmdUjz9BKLMoCetgS7my");
        convert_fromv9("6bJY8RxitpYoDA3AmpxpXhpWQvPqgYPP3XAGMWMez7aGWMzA4iiWs22xgfMQd76BEBEAwjm");
        convert_fromv9("4HuXw4HAXDiqGmyhVdDN5S1X4cQk4ejM98cXXQDrffYc3wScHDHBRHmzQQLdjwHpU87");
        */
    }

    mt19937_64 rng;
    int mode{0};

};

void test_sigcode0() {

    using sigcode_t = us::gov::cash::sigcode_t;
    sigcode_t sc;
    blob_t blob0;
    sc.write(blob0);
    cout << blob0.size() << endl;

    sigcode_t sc1;
    assert(is_ok(sc1.read(blob0)));
}

void test_locking_program_input() {

    using locking_program_input_t = us::gov::cash::locking_program_input_t;
    locking_program_input_t o;
    blob_t blob0;
    o.write(blob0);
    cout << blob0.size() << endl;

    locking_program_input_t o1;
    assert(is_ok(o1.read(blob0)));
}


void test_tx() {
    us::wallet::wallet::algorithm w("");
    auto a = w.new_address();


    {
    auto tx = w.set_supply(a, 5647);
    assert(is_ko(tx.first));
    cout << tx.first << endl;
    assert(string(tx.first) == "KO 32001 A funded account must exist");
    }
    w.data = new us::gov::cash::accounts_t();
    w.data->emplace(a, us::gov::cash::account_t());

    auto tx = w.set_supply(a, 5647);


    blob_t blob0;
    tx.second->write(blob0);

    cout << "blob size" << blob0.size() << endl;
    using evidence = us::gov::engine::evidence;
    pair<ko, evidence*> r = evidence::from_blob(blob0);
    {
        if (is_ko(r.first)) {
            cout << r.first << endl;
            assert(r.second == nullptr);
            assert(false);
        }
    }
    r.second->write_pretty_en(cout);
    delete r.second;
}

void test_ev_blob() {
    test_sigcode0();
    test_locking_program_input();
    test_tx();
}

#include <us/gov/relay/rpc_peer_t.h>

void test_push(const string& s) {
    tee("test push dgram write/read");
    datagram* d;
    hash_t tid(4673);
    {
        blob_t blob;
        {
        us::gov::io::blob_writer_t writer(blob, us::gov::io::blob_writer_t::blob_size(s));
        writer.write(s);
        }
        assert(blob.size() == s.size() + us::gov::io::blob_writer_t::sizet_size(s.size()));

        us::gov::relay::rpc_peer_t::push_in_t o(tid, 82, blob);
        d = o.get_datagram(0, 0);
    }

    us::gov::relay::rpc_peer_t::push_in_dst_t o_in;
    us::gov::io::blob_reader_t reader(*d);
    auto r = reader.read(o_in);
    if (is_ko(r)) {
        cout << r << '\n';
        assert(false);
    }
    assert(o_in.blob.size() == s.size() + us::gov::io::blob_writer_t::sizet_size(s.size()));
    assert(o_in.tid == tid);
    assert(o_in.code == 82);

    string s2;
    assert(is_ok(us::gov::io::blob_reader_t::parse(o_in.blob, s2)));
    assert(s2 == s);
    delete d;
}

void test_push() {
    test_push("");
    test_push("abracadabra-poo");
    test_push("POLLAFRITA");
}

void test_blobs() {
    {
        blobs_t blobs;
        blobs.test();
    }
    test_ev_blob();
    test_push();
}

}

