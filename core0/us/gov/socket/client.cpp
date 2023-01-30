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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <string>
#include <chrono>
#include <sstream>
#include <thread>
#include <iomanip>
#include <tuple>

#include <us/gov/config.h>
#include <us/gov/likely.h>
#include <us/gov/config.h>
#include <us/gov/likely.h>
#include <us/gov/io/cfg0.h>

#include "daemon0_t.h"
#include "client.h"

#define loglevel "gov/socket"
#define logclass "client"
#include "logs.inc"

using namespace us::gov::socket;
using c = us::gov::socket::client;

const char* c::KO_7190 = "KO 7190 Nothing received";
const char* c::KO_60541 = "KO 60541 Invalid address.";
const char* c::KO_58961 = "KO 58961 Invalid port.";
const char* c::KO_10100 = "KO 10100 Unreachable host.";
const char* c::KO_10210 = "KO 10210 Not implemented.";
const char* c::KO_27190 = "KO 27190 Invalid endpoint port.";
const char* c::KO_27191 = "KO 27191 Invalid endpoint host.";

struct timeval c::timeout{3, 0};

#if CFG_COUNTERS == 1
    c::counters_t c::counters;
#endif

c::client(daemon0_t& daemon): sock(-1), daemon(daemon) {
    log("contructor-outbound", this, sock);
    using namespace chrono;
    activity.store(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    since = activity_recv = chrono::system_clock::now();
}

c::client(daemon0_t& daemon, int sock): sock(sock), daemon(daemon) {
    using namespace chrono;
    assert(sock != 0);
    if (sock != -1) {
        hostport = raddress();
        log("contructor-inbound", sock, endpoint());
    }
    else {
        log("contructor-outbound");
    }
    activity.store(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    since = activity_recv = chrono::system_clock::now();
}

c::~client() {
    assert(sock != 0);
    if (sock != -1) {
        log("close socket", sock);
        ::shutdown(sock, SHUT_RDWR);
        ::close(sock);
    }
    log("client destructor", sock, endpoint());
}

void c::register_reason(channel_t channel, seq_t seq, const reason_t& reason) {
    if (get<2>(finished_reason).empty()) {
        finished_reason = make_tuple(channel, seq, reason);
        log("adding finishing reason", reason, "seq", seq);
    }
    else {
        log("preserving existing finishing reason", get<2>(finished_reason), "seq", get<1>(finished_reason));
    }
}

void c::disconnectx(channel_t channel, seq_t seq, const reason_t& reason) {
    log("disconnect", endpoint(), "fd", sock, "chan", channel);
    register_reason(channel, seq, reason);
    daemon.detach(*this);
    on_I_disconnected(reason);
}

void c::disconnect(seq_t seq, const reason_t& reason) {
    log("disconnect");
    disconnectx(daemon.channel, seq, reason);
}

pair<ko, datagram*> c::decrypt0(datagram* d) const {
    log("WARNING: Not decrypting. Encryption not supported.");
    return make_pair(ok, d); //assumes the input datagram is not encrypted;
}

pair<ko, datagram*> c::encrypt0(datagram* d) const { //delete only if the returned pointer is not d
    log("WARNING: Not encrypting. Encryption not supported.");
    assert(daemon.api_v != 0);
    return make_pair(ok, d); //no encryption;
}

/*
svc_t c::translate_svc(svc_t svc, bool inbound) const {
    log("WARNING: Not using any API versioning translator.");
    assert(daemon.api_v != 0);
    return svc;
}
*/

void c::set_finish() {
    log("set_finish", sock, endpoint());
    finished.store(true);
    if (unlikely(sock == -1)) return;
    log("close socket", sock);
    ::shutdown(sock, SHUT_RDWR);
    ::close(sock);
    sock = -1;
}

void c::set_finished() {
    log("set finished");
    finished.store(true);
}

void c::set_finishing_reason(channel_t peer_channel, seq_t seq, const reason_t& reason) {
    log("adding finishing reason", reason, "seq", seq);
    if (reason.empty()) {
        log("no given reason");
        return;
    }
    finished_reason = make_tuple(peer_channel, seq, reason);
}

bool c::is_wan_ip(host_t x) {
    if (unlikely(x == 0xFFFFFFFF)) return false; //255.255.255.255/32
    x &= 0x00FFFFFF;
    if (unlikely(x == 0x000000C0)) return false; //192.0.0.0/24
    if (unlikely(x == 0x000200C0)) return false; //192.0.2.0/24
    if (unlikely(x == 0x006358C0)) return false; //192.88.99.0/24
    if (unlikely(x == 0x006433C6)) return false; //198.51.100.0/24
    if (unlikely(x == 0x007100CB)) return false; //203.0.113.0/24
    x &= 0x0000FFFF;
    if (unlikely(x == 0x0000FEA9)) return false; //169.254.0.0/16
    if (likely(x == 0x0000A8C0)) return false; //192.168.0.0/16
    if (unlikely((x & 0x0000FEFF) == 0x000012C6)) return false; //198.18.0.0/15
    if (unlikely((x & 0x0000F0FF) == 0x000010AC)) return false; //172.16.0.0/12
    if (unlikely((x & 0x0000C0FF) == 0x00004064)) return false; //100.64.0.0/10
    x &= 0x000000FF;
    if (unlikely(x == 0x00000000)) return false; //0.0.0.0/8
    if (likely(x == 0x0000000A)) return false; //10.0.0.0/8
    if (likely(x == 0x0000007F)) return false; //127.*
    x &= 0x000000F0;
    if (unlikely(x == 0x000000E0)) return false; //224.0.0.0/4 //Multicast
    if (unlikely(x == 0x000000F0)) return false; //240.0.0.0/4 //reserved
    return true;
}

bool c::is_valid_ip(host_t addr, channel_t channel) {
    log("is_valid_ip", addr, channel);
    if (channel == 0) {
        return is_wan_ip(addr);
    }
    return addr != 0;
}

string c::ip4_decode(host_t addr0) {
    in_addr addr;
    addr.s_addr = addr0;
    return inet_ntoa(addr);
}

host_t c::ip4_encode(const shost_t& shost) {
    log("ip4_encode", shost);
    in_addr addr;
    host_t h;
    if (inet_aton(shost.c_str(), &addr) == 0) {
        h = 0;
    }
    else {
        h = addr.s_addr;
        log("ip4_encode. resolved IP4 ", shost, h);
    }
    if (unlikely(h == 0)) {
        hostent *hostinfo;
        hostinfo = ::gethostbyname(shost.c_str());
        if (hostinfo == 0) {
            #if CFG_COUNTERS == 1
                ++counters.unknown_hostname;
            #endif
            log("ip4_encode. unresolved name", shost);
            return 0;
        }
        sockaddr_in name;
        name.sin_addr = *(in_addr*)hostinfo->h_addr;
        h = name.sin_addr.s_addr;
        log("ip4_encode. resolved name", shost, h);
    }
    log("ip4_encode returns", h);
    return h;
}

hostport_t c::ip4_encode(const shostport_t& shostport) {
    return hostport_t(ip4_encode(shostport.first), shostport.second);
}

string c::endpoint(host_t a, port_t p) {
    ostringstream os;
    os << ip4_decode(a) << ":" << p;
    return os.str();
}

string c::endpoint(const hostport_t& ip) {
    return endpoint(ip.first, ip.second);
}

string c::endpoint() const {
    return endpoint(hostport);
}

pair<ko, shostport_t> c::parse_endpoint(const string& ipport) {
    auto i = ipport.find(':');
    shost_t h;
    port_t p;
    if (i == string::npos) {
        h = ipport;
        us::gov::io::cfg0::trim(h);
        if (h.empty()) {
            log(KO_27190, 2);
            return make_pair(KO_27190, make_pair("", 0));
        }
        p = 0;
    }
    else {
        h = ipport.substr(0, i);
        us::gov::io::cfg0::trim(h);
        if (h.empty()) {
            log(KO_27190, 2);
            return make_pair(KO_27190, make_pair("", 0));
        }
        if (ipport.size() == i + 1) {
            log(KO_27191, 1);
            return make_pair(KO_27191, make_pair("", 0));
        }
        istringstream is(ipport.substr(i + 1));
        is >> p;
        if (is.fail() || p == 0) {
            log(KO_27191, 2);
            return make_pair(KO_27191, make_pair("", 0));
        }
    }
    return make_pair(ok, make_pair(h, p));
}

hostport_t c::raddress() const {
    struct sockaddr_storage addr;
    socklen_t len = sizeof addr;
    int a = getpeername(sock, (struct sockaddr*)&addr, &len);
    if (unlikely(a != 0)) return make_pair(0, 0);
    if (likely(addr.ss_family == AF_INET)) {
        struct sockaddr_in *s = (struct sockaddr_in*) &addr;
        auto o = ntohs(s->sin_port);
        return make_pair(s->sin_addr.s_addr, o);
    }
    return make_pair(0, 0);
}

namespace {
    template <typename Container, typename Fun>
    void tuple_for_each(const Container& c, Fun fun) {
        for (auto& e : c)
            fun(std::get<0>(e), std::get<1>(e), std::get<2>(e));
    }

    string duration_str(chrono::milliseconds time) {
        using namespace std::chrono;
        using T = std::tuple<milliseconds, int, const char *>;
        constexpr T formats[] = {
            T{hours(1), 2, ""},
            T{minutes(1), 2, ":"},
            T{seconds(1), 2, ":"},
        };
        std::ostringstream o;
        tuple_for_each(formats, [&time, &o](auto denominator, auto width, auto separator) {
            o << separator << std::setw(width) << std::setfill('0') << (time / denominator);
            time = time % denominator;
        });
        return o.str();
    }
}

string c::age(const time_point& time) {
    using namespace chrono;
    return duration_str(duration_cast<milliseconds>(system_clock::now() - time));
}

string c::age(ts_ms_t msts) {
    using namespace chrono;
    return duration_str(milliseconds(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - msts));
}

string c::refdata() const {
    ostringstream os;
    os << "fd" << sock << " " << endpoint();
    return os.str();
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "socket::client: fd " << sock << " inet_addr " << endpoint() << " age " << age(since) << " idle " << age(activity.load()) << "idle(recv) " << age(activity_recv) << '\n';
}

void c::dump_all(const string& prefix, ostream& os) const {
    dump(prefix, os);
}

void c::disable_recv_timeout() {
    log("WP 5149 disabling timeout");
    struct timeval timeout{0, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);
}

ko c::init_sock2(const hostport_t& hostport, bool block) {
    log("init_sock", "block=", block);
    sock = ::socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        auto r = "KO 10580 Socket initialization error.";
        log(r, errno, strerror(errno));
        return r;
    }
    log("is non-blocking?", fcntl(sock, F_GETFL, 0) & O_NONBLOCK);
    sockaddr_in skaddr;
    skaddr.sin_family = AF_INET;
    skaddr.sin_port = htons(hostport.second);
    skaddr.sin_addr.s_addr = hostport.first;
    log("::connect", endpoint(hostport));
    int r = ::connect(sock, (struct sockaddr*) &skaddr, sizeof(skaddr));
    log("/ ::connect", endpoint(hostport), r);
    if (unlikely(r < 0)) {
        log(KO_10100, "close socket-initsock. connect returned", r, endpoint(hostport));
        ::close(sock);
        sock = -1;
        return KO_10100;
    }
    if (block) {
        log("init_sock", "set timeout");
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO | SO_SNDTIMEO, (const char*)& timeout, sizeof timeout);
    }
    else {
        log("init_sock", "set non-block");
        fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);
    }
    return ok;
}

ko c::connect0(const hostport_t& hostport, bool block) {
    log("connect0", endpoint(hostport), "block?=", block);
    if (unlikely(hostport.first == 0)) {
        log(KO_60541);
        on_connect(KO_60541);
        return KO_60541;
    }
    if (unlikely(hostport.second == 0)) {
        log(KO_58961);
        on_connect(KO_58961);
        return KO_58961;
    }
    finished.store(false);
    finished_reason = make_tuple(daemon.channel, 0, string());
    ko r = init_sock2(hostport, block);
    if (is_ko(r)) {
        log("connect-outbound-FAILED", sock, endpoint(hostport));
        on_connect(r);
        return r;
    }
    this->hostport = hostport;
    log("reset ts activity = now", 2);
    using namespace chrono;
    since = system_clock::now();
    activity.store(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    log("connect-outbound", sock, endpoint(hostport));
    on_connect(ok);
    return ok;
}

ko c::connect0(const shostport_t& shostport, bool block) {
    log("connect0", shostport.first, "block?=", block);
    return connect0(ip4_encode(shostport), block);
}

void c::on_connect(ko err) {
    log("on_connect", err == ok ? "ok" : err);
    if (is_ok(err)) {
        activity_recv = chrono::system_clock::now();
    }
    daemon.on_connect(*this, err);
}

pair<ko, datagram*> c::recv4x(datagram* d, busyled_t& busyled) {
    log("recv4");
    pair<ko, datagram*> r;
    if (d != nullptr) {
        log("continuing with existing datagram", d->dend);
        r.second = d;
    }
    else {
        log("starting new datagram");
        r.second = new datagram(daemon.channel, 2);
    }
    #if CFG_COUNTERS == 1
        ++counters.attempt_read_fragment;
    #endif
    auto prevdend = r.second->dend;
    log("before: dend=", prevdend);
    ko ans = r.second->recvfrom(sock, daemon.channel, busyled);
    if (unlikely(is_ko(ans))) {
        log("recvfrom answer:", ans);
        r.first = ans;
        if (r.first != datagram::KO_0001) {
            delete r.second;
            r.second = nullptr;
        }
        return move(r);
    }
    assert(r.second != nullptr);
    if (prevdend < r.second->dend) {
        log("reset ts activity=now", 1, "dend:", prevdend, "->", r.second->dend);
        using namespace chrono;
        auto t = system_clock::now();
        activity.store(duration_cast<milliseconds>(t.time_since_epoch()).count());
        activity_recv = t;
    }
    log("after: dend=", r.second->dend);
    return move(r);
}

pair<ko, datagram*> c::recv6(busyled_t& busyled) {
    log("recv6");
    pair<ko, datagram*> r;
    r.second = nullptr;
    while(true) {
        r = recv4x(r.second, busyled);
        if (unlikely(is_ko(r.first))) {
            break;
        }
        assert(r.second != nullptr);
        if (r.second->completed()) {
            r = decrypt0(r.second);
            if (unlikely(is_ko(r.first))) {
                if (r.first != datagram::KO_0001) { //--strip
                    assert(r.second == nullptr); //--strip
                } //--strip
                return r;
            }
            assert(daemon.api_v != 0); //top peer instance must set api_v
            if (unlikely(peer_api_v != daemon.api_v)) { 
                if (peer_api_v < daemon.api_v || peer_api_v == 255) {
                    translate_svc(r.second->service, true);
                }
            }
            break;
        }
    }
    log("/recv6");
    return r;
}

#if CFG_COUNTERS == 1
    void c::counters_t::dump(ostream& os) const {
        os << "attempt_read_fragment " << attempt_read_fragment << '\n';
        os << "disconnection_wrong_read " << socket::client::counters.disconnection_wrong_read << '\n';
        os << "zero_read " << socket::client::counters.zero_read << '\n';
        os << "unknown_hostname " << unknown_hostname << '\n';
        os << "connection_refused " << connection_refused << '\n';
        os << "busy_outbound_pipe " << busy_outbound_pipe << '\n';
    }
#endif


