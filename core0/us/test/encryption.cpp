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
#include <cassert>
#include <vector>

#include <us/gov/config.h>
#include <us/gov/crypto/symmetric_encryption.h>

#include "encryption.h"

#define loglevel "test"
#define logclass "encryption"
#include "logs.inc"

#include "assert.inc"

namespace us { namespace test {

using namespace std;
using us::gov::crypto::symmetric_encryption;
using us::ok;
using us::is_ok;
using us::is_ko;
using us::ko;

void check_ok(ko r) {
    if (r!=ok) cout << (const char*) r << endl;
    assert(r==ok);
}

void test_symmetric_encryption() {
    //test that encryption then decryption will retrieve original plaintext.
    test_encrypt_decrypt("encrypt this");
    test_encrypt_decrypt("");
    test_encrypt_decrypt(",./;'#[]-=123456DFJLSKDFJERUEIUR  \n rtr");
    test_encrypt_decrypt("0");
    test_encrypt_decrypt("𝕋𝕙𝕖 𝕢𝕦𝕚𝕔𝕜 𝕓𝕣𝕠𝕨𝕟 𝕗𝕠𝕩 𝕛𝕦𝕞𝕡𝕤 𝕠𝕧𝕖𝕣 𝕥𝕙𝕖 𝕝𝕒𝕫𝕪 𝕕𝕠𝕘");
    test_encrypt_decrypt("パーティーへ行かないか");
    test_encrypt_decrypt("˙ɐnbᴉlɐ ɐuƃɐɯ ǝɹolop ʇǝ ǝɹoqɐl ʇn ʇunpᴉpᴉɔuᴉ ɹodɯǝʇ poɯsnᴉǝ op pǝs 'ʇᴉlǝ ƃuᴉɔsᴉdᴉpɐ ɹnʇǝʇɔǝsuoɔ 'ʇǝɯɐ ʇᴉs ɹolop ɯnsdᴉ ɯǝɹo˥00˙Ɩ$-");
    test_encrypt_decrypt("null");
    test_encrypt_decrypt("-$1.00");

    //test that message can't be decoded with the wrong key.
    keys a = keys::generate();
    keys b = keys::generate();
    keys c = keys::generate();
    cout << "wrong key test" << endl;
    test_encrypt_decrypt("encrypt this", a.priv, a.pub, c.priv, b.pub, false);

    cout << "same key test" << endl;
    test_encrypt_decrypt("encrypt this", a.priv, a.pub, a.priv, a.pub, true);
}

void test_encrypt_decrypt(string plaintext_string) {
    keys a = keys::generate();
    keys b = keys::generate();
    test_encrypt_decrypt(plaintext_string,a.priv,a.pub,b.priv,b.pub,true);
}

void test_encrypt_decrypt(string plaintext_string,const keys::priv_t& priv_a, const keys::pub_t& pub_a, const keys::priv_t& priv_b, const keys::pub_t& pub_b, bool expected_ok) {
    cout << "plaintext size " << plaintext_string.size() << endl;
    vector<unsigned char> plaintext(plaintext_string.begin(),plaintext_string.end());
    symmetric_encryption se_a;
    assert(is_ok(se_a.init(priv_a, pub_b)));
//    cout << "instantiated symenc" << endl;
    vector<unsigned char> ciphertext;
    assert(se_a.encrypt(plaintext, ciphertext, 0)==ok);
    vector<unsigned char> ciphertext_2;
    assert(se_a.encrypt(plaintext, ciphertext_2, 0)==ok);
    assert(ciphertext!=ciphertext_2); //encrypted text doesn't repeat

    cout << "encrypted vector " << ciphertext.size() << " bytes" << endl;

    symmetric_encryption se_b;
    assert(is_ok(se_b.init(priv_b,pub_a)));
    vector<unsigned char> decodedtext;
    auto r=se_b.decrypt(ciphertext, decodedtext);
    if (expected_ok) {
        check_ok(r);
    }
    else {
        assert(is_ko(r));
    }
    cout << "decrypted vector " <<decodedtext.size() << " bytes" << endl;
    assert(plaintext==decodedtext?expected_ok:!expected_ok);
}

using namespace chrono;
/*
void alpha_28_comp(int a28, int cur, ostream& os) {
    int diff=a28-cur;
    os << 100.0*(1.0-((double)cur/(double)a28)) << "% better";
}
*/

//void test_perf_encrypt(int msgsz, int iters, int alpha_28_ref) {
void test_perf_encrypt(int msgsz, int iters) {
    cout << "symmetric encryption perf test. msg sz=" << msgsz << " iters=" << iters << "... "; cout.flush();
    keys a = keys::generate();
    keys b = keys::generate();
    symmetric_encryption se_a;
    assert(is_ok(se_a.init(a.priv, b.pub)));
    vector<unsigned char> plaintext(msgsz, 'x');
    assert(plaintext.size() == msgsz);
    vector<unsigned char> ciphertext;
    //ciphertext.reserve(plaintext.size() * 2);
    auto t0 = chrono::system_clock::now();
    se_a.encrypt(plaintext, ciphertext, 0);
    for (int i = 0; i< iters; ++i) {
        se_a.encrypt(plaintext, ciphertext, 0);
    }
    auto t1 = chrono::system_clock::now();
    auto t = duration_cast<milliseconds>(t1 - t0).count();
    cout << t << " ms. ";
    //alpha_28_comp(alpha_28_ref, t, cout);
    cout << '\n';
}

#define include_heavy 0

void test_perf_encrypt() {
  bool prev = us::dbg::thread_logger::instance.disabled;

  tee("enc ratio");
  for (int sz = 0; sz < 100; ++sz) {
      test_perf_encrypt(sz, 1);
  }
  for (int sz = 100; sz < 1000; sz += 10) {
      test_perf_encrypt(sz, 1);
  }
  for (int sz = 1000; sz < 10000; sz += 100) {
      test_perf_encrypt(sz, 1);
  }
  for (int sz = 10000; sz < 100000; sz += 1000) {
      test_perf_encrypt(sz, 1);
  }
  for (int sz = 100000; sz < 1000000; sz += 10000) {
      test_perf_encrypt(sz, 1);
  }

  tee("perf");
  us::dbg::thread_logger::instance.disabled = true;

  test_perf_encrypt(10, 10000);
  #if include_heavy == 1
      test_perf_encrypt(10, 100000);
      test_perf_encrypt(10, 1000000);
  #endif
  test_perf_encrypt(10000, 10000);
  #if include_heavy == 1
      test_perf_encrypt(10000, 100000);
      test_perf_encrypt(10000, 1000000);
  #endif
  test_perf_encrypt(100000, 10000);
  #if include_heavy == 1
      test_perf_encrypt(100000, 100000);
      test_perf_encrypt(100000, 1000000);
  #endif
  us::dbg::thread_logger::instance.disabled = prev;
}

}}

