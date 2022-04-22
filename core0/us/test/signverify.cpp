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
#include <iostream>
#include <cassert>
#include <vector>

#include <us/gov/config.h>
#include <us/gov/crypto/ec.h>
#include <us/gov/crypto/base58.h>

#define loglevel "test"
#define logclass "signverify"
#include "logs.inc"

#include "assert.inc"

namespace us { namespace test {

using namespace std;

using namespace us::gov::crypto;
using us::is_ok;

void test_signverify(const string& text, const ec::keys::priv_t& priv_a, const ec::keys::pub_t& pub_a, bool expected) {
    cout << "test_signverify ";
    ec::sigmsg_hasher_t hasher;
    hasher.write(text);
    ec::sigmsg_hasher_t::value_type h;
    hasher.finalize(h);

    us::gov::crypto::ec::sig_t sig;
    assert(is_ok(ec::instance.sign(priv_a, text, sig)));
    bool r = ec::instance.verify_not_normalized(pub_a, h, sig);

    assert(r == expected);
    cout << "OK" << endl;
}

void test_signverify(const string& text) {
    ec::keys k = ec::keys::generate();
    ec::keys k2 = ec::keys::generate();
    test_signverify(text,k.priv,k.pub,true);
    test_signverify(text,k.priv,k2.pub,false);
}
/*
void test_logoutput(const string& msg_b58, const string& pubk_b58, const string& sig_der_b58) {
    cout << "test_logoutput " << msg_b58 << " " << pubk_b58 << " " << sig_der_b58 << endl;
    vector<unsigned char> signature_der=b58::decode(sig_der_b58);
    ec::keys::pub_t pub(b58::decode(pubk_b58));
    auto msg=b58::decode_string(msg_b58);
    bool r=ec::instance.verify_not_normalized(pub, msg, signature_der);
    assert(r);
    cout << "OK" << endl;
}
*/
/*
void test_logoutput(const string& logline) {
    istringstream is(logline);
    string ign;
    string msg;
    string pubk;
    string sig;
    is >> ign;
    assert(ign=="msgh");
    is >> msg;
    is >> ign;
    assert(ign=="pubk");
    is >> pubk;
    is >> ign;
    assert(ign=="sig-der-b58");
    is >> sig;
    test_logoutput(msg,pubk,sig);
}
*/
void test_signverify() {
    test_signverify("sign this");
    test_signverify("");
    test_signverify(",./;'#[]-=123456DFJLSKDFJERUEIUR  \n rtr");
    test_signverify("0");
    test_signverify("𝕋𝕙𝕖 𝕢𝕦𝕚𝕔𝕜 𝕓𝕣𝕠𝕨𝕟 𝕗𝕠𝕩 𝕛𝕦𝕞𝕡𝕤 𝕠𝕧𝕖𝕣 𝕥𝕙𝕖 𝕝𝕒𝕫𝕪 𝕕𝕠𝕘");
    test_signverify("パーティーへ行かないか");
    test_signverify("˙ɐnbᴉlɐ ɐuƃɐɯ ǝɹolop ʇǝ ǝɹoqɐl ʇn ʇunpᴉpᴉɔuᴉ ɹodɯǝʇ poɯsnᴉǝ op pǝs 'ʇᴉlǝ ƃuᴉɔsᴉdᴉpɐ ɹnʇǝʇɔǝsuoɔ 'ʇǝɯɐ ʇᴉs ɹolop ɯnsdᴉ ɯǝɹo˥00˙Ɩ$-");
    test_signverify("null");
    test_signverify("-$1.00");


//    test_logoutput("msgh 4WqqdNVQX2Gr8AYP1WDS3DWemMQjfes4rJr1daKeUs2i pubk dK5hXxepMdWj2z7ztwzxPWhciyQ2hTk2BnuXzY1PGkD5 sig-der-b58 381yXZJss3LJaTnXEaTHSCyWyNGcLtVnErjs1U3srW3MXyrQSc8CB6hx1Y8YUPuiQ6PY9QYWRhtFiCkcj4BHZzyod21Juh6C");
//    test_logoutput("msgh 7k8zBT76pcuUUfGNB1fgW6b8GJUApBzyARV9vF5WC6gR pubk dK5hXxepMdWj2z7ztwzxPWhciyQ2hTk2BnuXzY1PGkD5 sig-der-b58 381yXZC4f3qKY3QcQUWC2RcrAkywRJBhnE5UFzYsrCWrWAUC3eGjrFfap1Lm3W2NdRzCHGxuZ6XL19BExhjqckqgwAK4ziB3");

/*
    //test that message can't be decoded with the wrong key.
    keys a = keys::generate();
    keys b = keys::generate();
    keys c = keys::generate();
cout << "wrong key test" << endl;
    test_signverify("encrypt this", a.priv, a.pub, c.priv, b.pub,false);

cout << "same key test" << endl;
    test_signverify("encrypt this", a.priv, a.pub, a.priv, a.pub, true);

*/
}

}}

