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
#include <vector>
#include <string>

#include <us/gov/config.h>
#include <us/gov/socket/client.h>

#define loglevel "test"
#define logclass "ip4"
#include "logs.inc"

#include "assert.inc"

namespace us { namespace test {

using namespace std;

void test_ip4(string ip,bool expected) {
    uint32_t x=us::gov::socket::client::ip4_encode(ip);
    bool b=us::gov::socket::client::is_wan_ip(x);
    if(b!=expected) {
        cout << "FAIL testing ip " << ip << " " << hex << x << endl;
        assert(false);
    }
}

void encode_decode(uint32_t enc,const string& ip) {
    auto e=us::gov::socket::client::ip4_encode(ip);
    auto d=us::gov::socket::client::ip4_decode(enc);
    auto e2=us::gov::socket::client::ip4_encode(d);
    auto d2=us::gov::socket::client::ip4_decode(e);
    assert(e==enc);
    assert(e==e2);
    assert(d==d2);
    assert(d==ip);
}

void encode_decode() {
    encode_decode(16777343,"127.0.0.1");
}

void test_ip4() {
    encode_decode();
    test_ip4("0.0.0.0",false); test_ip4("0.255.255.255",false);
    test_ip4("10.0.0.0",false); test_ip4("10.255.255.255",false);
    test_ip4("100.64.0.0",false); test_ip4("100.127.255.255",false);
    test_ip4("127.0.0.0",false); test_ip4("127.255.255.255",false);
    test_ip4("169.254.0.0",false); test_ip4("169.254.255.255",false);
    test_ip4("172.16.0.0",false); test_ip4("172.31.255.255",false);
    test_ip4("192.0.0.0",false); test_ip4("192.0.0.255",false);
    test_ip4("192.0.2.0",false); test_ip4("192.0.2.255",false);
    test_ip4("192.88.99.0",false); test_ip4("192.88.99.255",false);
    test_ip4("192.168.0.0",false); test_ip4("192.168.255.255",false);
    test_ip4("198.18.0.0",false); test_ip4("198.19.255.255",false);
    test_ip4("198.51.100.0",false); test_ip4("198.51.100.255",false);
    test_ip4("203.0.113.0",false); test_ip4("203.0.113.255",false);
    test_ip4("224.0.0.0",false); test_ip4("239.255.255.255",false);
    test_ip4("240.0.0.0",false); test_ip4("240.255.255.255",false);
    test_ip4("255.255.255.255",false);

    test_ip4("64.27.57.225",true); // multicast
    test_ip4("95.85.8.237",true); // multicast
    test_ip4("62.210.66.227",true); // multicast
    test_ip4("90.171.117.252",true); // reserved

    test_ip4("149.55.34.4",true);
    test_ip4("8.8.8.8",true);
    test_ip4("8.8.4.4",true);
    vector<string> valid{"212.47.228.216", "66.228.49.201", "137.116.225.142", "85.214.251.25", "67.186.224.85", "88.198.58.172", "62.75.216.13", "69.64.34.118", "79.160.221.140", "81.169.129.25", "98.143.152.14", "117.121.241.140", "128.30.96.44", "173.242.112.53", "184.154.36.82", "188.165.213.169", "204.9.55.71", "93.187.142.114", "91.210.181.21", "63.247.147.163", "199.115.228.181", "199.115.228.182", "91.121.205.50", "198.211.116.191", "162.243.120.138", "91.121.108.61", "185.122.57.203", "91.121.83.82 "};
    for (auto&i:valid) {
        test_ip4(i,true);
    }
}

}}

