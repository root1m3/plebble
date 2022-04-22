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
#include "networking.h"

#include <fcntl.h>

#include <us/gov/config.h>
#include <us/gov/ko.h>
#include <us/gov/socket/peer_t.h>
#include <us/gov/socket/multipeer/server.h>
#include <us/gov/io/seriable.h>
#include <us/gov/socket/protocol.h>
#include <us/gov/socket/multipeer/handler_daemon_t.h>
#include <us/gov/cli/rpc_daemon_t.h>
#include <us/gov/engine/net.h>
#include <us/gov/dfs/peer_t.h>
#include <us/gov/engine/protocol.h>

#include "okev.h"

#define loglevel "test/net"
#define logclass "networking"
#include <us/gov/logs.inc>

#include "../assert.inc"

using namespace us;
using namespace us::test;
using c = us::test::networking;

constexpr chrono::milliseconds c::latency;


namespace {
    string logdir0;
    string homedir;

    struct rpc_cli: us::gov::socket::rpc_daemon_t {
        using b = us::gov::socket::rpc_daemon_t;

        struct peer_t: us::gov::socket::peer_t {
            using b = us::gov::socket::peer_t;
            using b::peer_t;

            virtual void on_peer_disconnected(const reason_t& reason)  override {
                b::on_peer_disconnected(reason);
                unique_lock<mutex> lock(mx);
                disconnect_reason = reason;
                cv.notify_all();
            }

            void wait_4_disconnect_report(networking* n, int secs) {
                tee("wait_4_disconnect_report ", secs, " secs");
                chrono::system_clock::time_point deadline = chrono::system_clock::now() + (secs == 0 ? c::latency : chrono::milliseconds(secs * 1000));
                unique_lock<mutex> lock(mx);
                if (!disconnect_reason.empty()) return;
                cv.wait_until(lock, deadline, [&] {
                        if (!disconnect_reason.empty()) return true;
                        return false;
                    });

                for (int i =0; i<10; ++i) {
                    if (disconnect_reason.empty()) {
                        tee("waking up backend");
                        n->wakeup_handler();
                        tee("max 2secs");
                        deadline += chrono::milliseconds(2000);
                        cv.wait_until(lock, deadline, [&] {
                                if (!disconnect_reason.empty()) return true;
                                return false;
                            });
                    }
                    else {
                        break;
                    }
                }
            }


            bool process_work(datagram* d) override {
                tee("recv DGRAM channel", d->decode_channel(), "svc", d->service);
                if (d->service < us::gov::protocol::socket_end) {
                    return b::process_work(d);
                }
                tee("Unexpected datagram", d->service);
                assert(false);
                delete d;
                return true;
            }

            mutex mx;
            condition_variable cv;

            string disconnect_reason;
        };

        rpc_cli(channel_t channel, shost_t shost, port_t port): b(channel, nullptr), shost(shost), port(port) {
            channel = datagram::all_channels;
        }

        ko connect() override {
            tee("rpc_daemon wants to connect to daemon");
//            return p->peer->connect0(govaddr, myport, true);
            shostport_t shostport = make_pair(shost, port);
            return get_peer().connect0(shostport, true);

        }

        virtual us::gov::socket::client* create_client() {
            return new peer_t(*this);
        }

        shost_t shost;
        port_t port;
    };

    void check_disconnect_report(networking* n, rpc_cli::peer_t* p, int secs, const char* expected_report) {
        p->wait_4_disconnect_report(n, secs);
        if (!p->disconnect_reason.empty()) {
            cout << "arrived disconnection report: " << p->disconnect_reason << endl;
        }
        assert(p->disconnect_reason == expected_report);
    }
}


c::networking(us::gov::crypto::ec::keys& k, uint16_t port, ostream&os): id(k), b(123, port, port, edges, (uint8_t)1, 2, vector<pair<uint32_t, uint16_t>>()), t(os), myport(port), load(this) {
    constructor();
}

c::networking(us::gov::crypto::ec::keys& k, uint16_t port, const vector<pair<uint32_t, uint16_t>>& sn, int workers, ostream&os): id(k), b(123, port, port, edges, (uint8_t)0, workers, sn), t(os), myport(port), load(this) {
    constructor();
}

void c::constructor() {
    myaddress = addr_from_ndx(myport - govport);
}

bool c::wait_rnd_before_start() const {
    return false;
}

ko c::start() {
    #if CFG_LOGS == 1
        b::logdir = logdir + "/net_daemon";
    #endif
    cout << "starting test::networking" << endl;
    return b::start();
}

void c::start_load(const string& logdir) {
    test_load_logdir = logdir;
    assert(load::start() == ok);
}

void c::stop() {
    load::stop();
    b::stop();
}

void c::join() {
    load::join();
    b::join();
}

void c::join_load() {
    load::join();
}

void c::set_load_function(function<void ()> r) {
    load::set_run_function(r);
}

string c::addr_from_ndx(int ndx) {
    ostringstream os;
    os << "127.0.0.";
    os << 1 + ndx;
    return os.str();
}

us::gov::socket::peer_t* c::create_client(int sock) {
    return new peer_t(*this, sock);
}

void c::relay_evidence(const datagram &d, peer_t* exclude) {
    int n = send(0, exclude, d);
    log("broadcast_evidence", "orig", n);
}

const keys_t& c::get_keys() const {
    return id;
}

size_t c::clients_size() {
    lock_guard<mutex>(daemon.peers.mx);
    return daemon.peers.size();
}

void c::check_clients_size(size_t expected) {
    size_t sz;
    {
        lock_guard<mutex>(daemon.peers.mx);
        sz = daemon.peers.size();
    }
    cout << "clients sz = " << sz << " expected " << expected << endl;
    check(sz, expected);
}

us::gov::socket::rpc_daemon_t* c::connect2backend() {
    auto* p = new rpc_cli(123, govaddr, myport); //create_client(0);
    cout << "Connecting to backend " << govaddr << ":" << myport << endl;

    ko r = p->start(); //p->peer->connect0(govaddr, myport, true); //was false
    if (is_ko(r)) {
        fail(r);
    }
    assert(is_ok(p->wait_ready(chrono::system_clock::now() + chrono::seconds(3))));
    cout << "connected" << endl;
    this_thread::sleep_for(latency);
    return p;
}

int c::ban_counter() const {
    return static_cast<const us::gov::socket::multipeer::server&>(*this).counters.blacklisted_addr;
}

void c::reset_ban() {
    static_cast<us::gov::socket::multipeer::server&>(*this).counters.blacklisted_addr = 0;
    ban.clear();
}

void c::reset_clients() {
    for (auto& i: daemon.peers) i.second->disconnect(0, "");
    this_thread::sleep_for(latency);
}

void c::reset_dgram_counters() {
    datagram::counters.infraheader_datagram=
    datagram::counters.wrong_channel=
    datagram::counters.wrong_size=
    datagram::counters.valid_dgram_fragment1 = 0;
}

void c::check_ban_count(int expected) {
    log("ban counter", ban_counter(), "expected", expected);
    check(ban_counter(), expected);
    check(ban.size(), (size_t)expected);
}

void c::check_initial_state() {
    //check_idle_threads(threadpool_size);
    check_ban_count(0);
    check_clients_size(0);
}

void c::connect_disconnect() {
    auto sz = clients_size();
    cout << "sz " << sz << endl;
    auto p = connect2backend();
    cout << "sz " << sz << endl;
    assert(p->peer != nullptr);
    this_thread::sleep_for(latency);
    check_clients_size(sz + 1);
    p->stop();
    p->join();
    delete p;
    this_thread::sleep_for(latency);
    check_clients_size(sz);
    check_initial_state();
}

/*
//cracker attepts to consume the resource
void c::connect_disconnect2() { // A cracker connects and disconnects
    auto sz = clients_size();
    auto p = connect2backend();
    check_clients_size(sz + 1);
    delete p;
    this_thread::sleep_for(latency);
    check_initial_state();
}
*/

void c::connect_banned() {
    log("check banned client can't connect again");
    auto sz = clients_size();
    auto p = connect2backend();
    this_thread::sleep_for(latency);
    check_clients_size(sz);
    delete p;
}

void c::bore_server() {
    log("bore_server");
    this_thread::sleep_for(us::gov::peer::peer_t::auth_window + latency);
    //this_thread::sleep_for(1s); //--UTWWTL
    wakeup_handler();
//        clients.read_sockets();
    this_thread::sleep_for(latency);
}

/*
thread* c::recv_datagram(const int& sock, const int& timeout_seconds, datagram*& dest) {
    using namespace std::chrono;
    thread* t = new thread([&] {
        log_start(logdir, "recv_datagram");


        auto t0 = chrono::system_clock::now();

        cout << "recv_datagram with timeout " << timeout_seconds << " secs" << endl;
        datagram* d = new datagram(1);
        struct timeval timeout;
        timeout.tv_sec = timeout_seconds;
        timeout.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

        ko r = d->recvfrom(sock);
        if (r != ok) {
            cout << (const char*) r << endl;
            cout << "a datagram protocol::us_gov_finished was supposed to be delivered by the backend" << endl;
            assert(false);
        }
        if (!d->completed()) {
            delete d;
            d=nullptr;
        }
        dest = d;
        auto t = chrono::system_clock::now();
        if (d == nullptr) {
            cout << "recv_datagram returned 0 after " << duration_cast<milliseconds>(t-t0).count() << " millisecs" << endl;
        }
        else {
            cout << "recv_datagram returned dgram after " << duration_cast<milliseconds>(t-t0).count() << " millisecs. svc " << d->decode_service() << " dend " << d->dend << endl;
        }
    });
    this_thread::yield();

    return t;

}
*/

/*
------recv5
new 4 recv dgram
------/ recv5 8 9
sec level------------------------
curd 0x7fe388000f70
curd 8/9
0 seconds from dgram creation.
123 123
b
us::gov::socket::datagram::counters.wrong_channel 0


 causing stall. sleep for 3100 ms


sec level------------------------
curd 0x7fe388000f70
curd 8/9
1 seconds from dgram creation.


 sleep 1s more



 read
Fail. Expected 1, got 0
*/


void c::cause_stall() {
    log("wait to cause stall");
    wakeup_handler();
//        clients.read_sockets();
//cout << " causing stall. sleep for " << duration_cast<milliseconds>(us::gov::socket::peer_t::stall+latency).count() << " ms" << endl;
    this_thread::sleep_for(us::gov::socket::peer_t::stall);

    //this_thread::sleep_for(latency);

//        this_thread::sleep_for(us::gov::socket::peer_t::stall);
//       this_thread::sleep_for(1s);
//       this_thread::sleep_for(1s);
//       this_thread::sleep_for(1s);
    //this_thread::sleep_for(latency);
    //this_thread::sleep_for(latency);
    //this_thread::sleep_for(latency);
//cout << " read sockets" << endl;
    wakeup_handler();
//        clients.read_sockets();
    this_thread::sleep_for(latency);
//       this_thread::sleep_for(1s);
/*
for (int i=0;i<6; ++i) {
    this_thread::sleep_for(1s);
cout << "wait1s " << i << "/60" << endl;
    clients.read_sockets();
}
*/
//        this_thread::sleep_for(latency);
}

void c::connect_quiet() {
    log("connect quiet");
    auto sz = clients_size();
    auto bc = ban_counter();
    auto p = connect2backend();
    check_clients_size(sz + 1);
    bore_server();

    p->stop();
    //p->join();  //destructor calls join
    delete p;

    check_clients_size(sz);
    check_ban_count(bc);
    check_initial_state();
}

void c::connect_from_invalid_net_address() { //localhost is invalid in channel 0
    auto sz = clients_size();
    auto p = connect2backend();
    this_thread::sleep_for(latency);
    check_clients_size(sz);
    check_initial_state();
    delete p;
}

/*
    using namespace std::chrono;
    log_info(cout);

    datagram *d = 0;
    thread* th = recv_datagram(p->sock, secs + 1, d);

    cout << "sleep " << latency.count() << " millisecs" << endl;
    this_thread::sleep_for(latency);

    if (secs > 0) {
        cout << "sleep " << secs << " secs" << endl;
        this_thread::sleep_for(seconds(secs));
    }
    cout << "wakeup" << endl;
    wakeup();
    this_thread::yield();


    th->join();
    delete th;

    if (d != nullptr) cout << "recv didn't return a datagram" << endl;
    assert(d != nullptr);
    string reason;
    {
        us::gov::io::blob_reader_t r(*d);
        assert(is_ok(r.read(reason))); //:: = d->parse_string();
    }
    cout << "==>received reason: \"" << reason;

    cout << "\"; expected: \"" << expected_report << "\" --> ";
    if (reason == expected_report) cout << "OK";
    else cout << "KO";
    cout << endl;

    assert(reason == expected_report);
    delete d;
//}
*/

void c::send_bytes(size_t howmany, uint32_t decl_sz, uint16_t channel, uint16_t svc, bool slowly) {
    log("send_bytes", "howmany", howmany, "decl_sz", decl_sz, "channel", channel, "svc", svc);
    cout << "==>send_bytes. howmany " << howmany << " decl_sz " << decl_sz << " channel " << channel << " svc " << svc << endl;
    char* w = new char[howmany];
    {
        int fd = open("/dev/urandom", O_RDONLY);
        read(fd, w, howmany);
        close(fd);
        log("buffer filled with random garbage is ready");
    }
//        memset(w,0,howmany);

    //log("send_bytes",howmany,decl_sz,channel);
    //log("system channel",datagram::system_channel);
    //cout << "send " << howmany << " bytes, encoded sz=" << decl_sz << " channel " << channel << endl;
    auto sz = clients_size();
    auto bc = ban_counter();
    auto pd = connect2backend();
    auto p = static_cast<rpc_cli::peer_t*>(pd->peer);
    check_clients_size(sz + 1);
    check(us::gov::socket::datagram::counters.infraheader_datagram, (uint32_t)0);
    assert(datagram::size_channel == 2);
    if (howmany >= datagram::size_channel) {
        log("encoding channel (", channel, ")");
        w[datagram::offset_channel] = channel & 0xff;
        w[datagram::offset_channel + 1] = channel >> 8 & 0xff;
    }
    assert(datagram::size_size == 4);
    if (howmany>=datagram::offset_size + 4) {
        log("encoding decl_sz (", decl_sz , ")");
        w[datagram::offset_size] = decl_sz & 0xff;
        w[datagram::offset_size + 1] = decl_sz >> 8 & 0xff;
        w[datagram::offset_size + 2] = decl_sz >> 16 & 0xff;
        w[datagram::offset_size + 3] = decl_sz >> 24 & 0xff;
    }
    assert(datagram::size_service == 2);
    if (howmany>=datagram::offset_size + 4) {
        w[datagram::offset_service] = svc & 0xff;
        w[datagram::offset_service + 1] = svc >> 8 & 0xff;
    }

    log("datagram::counters.wrong_channel counter is ",us::gov::socket::datagram::counters.wrong_channel);
    int tot = 0;
    int ln = 0;
    assert(EAGAIN == EWOULDBLOCK);
    if (slowly) {
        while(tot < howmany) {// 1 sec per chunk
            log("write fd", p->sock, "tot sofar", tot, "remaning", howmany - tot);
            cout << "write fd " << p->sock << " tot sofar " << tot << " remaning " << (howmany - tot) << endl;
            int b = 1;
            if (tot < datagram::h) {
                if (howmany > datagram::h) {
                    ln = ::write(p->sock, w + tot, datagram::h - tot);
                }
                else {
                    ln = ::write(p->sock, w + tot, 1);
                }
            }
            else {
                ln = ::write(p->sock, w + tot, 1);
            }
            if (ln < 0) {
                if (errno == EAGAIN) {
                    static int times = 0;
                    log("Eagain", times);
                    if (times++ == 100) {
                        assert(false);
                        break;
                    }
                    this_thread::sleep_for(12ms);
                    continue;
                }
                else {
                    log("error",errno,strerror(errno));
                    cout << "errno= " << errno << " " << strerror(errno) << endl; //server disconnects if channel mismatch, or encoded size <h
                    break;
                }
            }
            tot += ln;
            this_thread::sleep_for(1s);
        }
    }
    else {
        while (tot < howmany) {
            log("write fd", p->sock, "tot sofar", tot, "remaning", howmany - tot);
            cout << "write fd " << p->sock << " tot sofar " << tot << " remaning " << (howmany - tot) << endl;
            ln = ::write(p->sock, w + tot, howmany - tot);
            if (ln < 0) {
                if (errno == EAGAIN) {
                    static int times = 0;
                    log("Eagain",times);
                    if (times++ == 100) {
                        assert(false);
                        break;
                    }
                    this_thread::sleep_for(12ms);
                    continue;
                }
                else {
                    log("error", errno, strerror(errno));
                    cout << "errno= " << errno << " " << strerror(errno) << endl; //server disconnects if channel mismatch, or encoded size <h
                    break;
                }
            }
            tot += ln;
        }
    }
    log("bytes written", tot, "out of", howmany);
    cout << "bytes written " << tot << " out of " << howmany << endl;

    string expected;
    if (howmany == 0) {
        expected = "disconnect auth-window";
    }
    else {
        if (howmany < datagram::h) {
            expected = "disconnect stall";
        }
        else {
            if (channel != 123) {
                expected = "disconnect channel";
            }
            else {
                if (decl_sz > datagram::maxsize) {
                    expected = "disconnect too_big";
                }
                else if (decl_sz < datagram::h) {
                    expected = "disconnect too_small";
                }
                else {
                    if (decl_sz > howmany) {
                        expected = "disconnect stall";
                    }
                    else {
                        if (slowly) { //goes at 1 byte per second
                            if (howmany >= datagram::h) {
                                if ((howmany - datagram::h) > us::gov::peer::peer_t::auth_window.count()) {
                                    expected = "disconnect auth-window";
                                }
                                else {
                                    if (svc == us::gov::protocol::socket_error) {
                                        expected = "malformed";
                                    }
                                    else {
                                        expected = "service";
                                    }
                                }
                            }
                            else {
                                if (howmany > us::gov::peer::peer_t::auth_window.count()) {
                                    expected="disconnect auth-window";
                                }
                                else {
                                    if (svc == us::gov::protocol::socket_error) {
                                        expected = "malformed";
                                    }
                                    else {
                                        expected = "service";
                                    }

                                }
                            }
                        }
                        else {
                            if (svc == us::gov::protocol::socket_error) {
                                expected = "malformed";
                            }
                            else {
                                expected = "service";
                            }
                        }
//        cout << "SUCESSS MAN. TIME PORTRAIT" << endl;
//assert(false);
/*
                        if (howmany<datagram::h) {
                            expected="stall";
                        }
                        else {
                            if (decl_sz<datagram::h) {
                                expected="disconnect";
                            }
                            else if (decl_sz>howmany) {
                                expected="stall";
                            }
                        }
*/
                    }
                }
            }
        }
    }
    //cout << "expected=" << expected << endl;
    log("expected", expected);

//        int tx_secs=0;
//      if (howmany>10000000) {
//        tx_secs=howmany/10000000;
  //  }
    int tx_secs = howmany / 10000000;
    cout << "using rx_secs=" << tx_secs << endl;

    tee("trace XX72", expected);

    if (expected == "disconnect auth-window") {
        check_disconnect_report(this, p, slowly ? 0 : tx_secs + us::gov::peer::peer_t::auth_window.count(), us::gov::peer::peer_t::finished_reason_1);
    }
    else if (expected == "disconnect stall") {
        check_disconnect_report(this, p, slowly ? 0 : tx_secs+us::gov::socket::peer_t::stall.count(), us::gov::socket::peer_t::finished_reason_1);
    }
    else if (expected == "disconnect channel") {
        check_disconnect_report(this, p, slowly ? 0 : tx_secs, us::gov::socket::datagram::KO_0001);
    }
    else if (expected == "disconnect too_big") {
        check_disconnect_report(this, p, slowly ? 0 : tx_secs, us::gov::socket::datagram::KO_0002);
    }
    else if (expected == "disconnect too_small") {
        check_disconnect_report(this, p, slowly ? 0 : tx_secs, us::gov::socket::datagram::KO_0003);
    }
    else if (expected == "not_impl") {
        check_disconnect_report(this, p, slowly ? 0 : tx_secs, us::gov::socket::peer_t::KO_10210);
    }
    else if (expected == "malformed") {
        //check_disconnect_report(p,slowly?0:tx_secs, us::gov::socket::peer_t::KO_46392);
    }
    else if (expected == "service") {
        check_disconnect_report(this, p, slowly ? 0 : tx_secs, us::gov::socket::multipeer::handler_daemon_t::KO_42093);
    }
    else {
        cout << "expected " << expected << endl;
        assert(false);
    }
    log("reset test");
    delete [] w;
    pd->stop();
    pd->join();
    delete pd;
    reset_ban();
    reset_dgram_counters();
    reset_clients();
    check_initial_state();

return;

exit(1);
cout << "wait 2 secs" << endl;
this_thread::sleep_for(2s);
cout << "exit" << endl;

    //p->stop();
    //p->join();
exit(1);
/*

    if (expected=="stall") {
        cout << "waiting for " << duration_cast<seconds>(us::gov::socket::peer_t::stall).count() << " secs" << endl;
        this_thread::sleep_for(latency);
1.- recibida la conexion?
        check_clients_size(sz+1); //HX7T56            check_clients_size(sz);


        this_thread::sleep_for(us::gov::socket::peer_t::stall);

esta stall?
1.- recibida la conexion
2.- timeot, si no has mandado el postit entonces, el postit esta stall

esta conectado
        assert(!p->connected());
        this_thread::sleep_for(500ms);
        assert(!p->connected());

    }
*/

return;

//        bool causes_disconnection=false;
/*
    if (channel==datagram::system_channel && decl_sz>=us::gov::socket::datagram::h) {
//          log("",tot,howmany);
        causes_disconnection=true;
    }
*/
//        wakeup();
//        clients.read_sockets();

//        this_thread::sleep_for(latency);
/*
if (tot>decl_sz) {
    log("tot>decl_sz","backend should disconnect");
    causes_disconnection=true;
}

if (!causes_disconnection) {
    log("so far the connection shouln't be dropped");
    check(tot==howmany,true);
}
*/

    if (howmany<us::gov::socket::datagram::h) {
        log("howmany<us::gov::socket::datagram::h");
        //check_clients_size(sz+1);
        cause_stall();
        cause_stall();
        check_clients_size(sz); //HX7T56            check_clients_size(sz);
        check_ban_count(bc); // HX7T56  //check_ban_count(bc+1);
        check(us::gov::socket::datagram::counters.infraheader_datagram, (uint32_t)1);
    }
    else {
        log("howmany>=us::gov::socket::datagram::h");
        check(us::gov::socket::datagram::counters.infraheader_datagram, (uint32_t)0);
        if (channel != 123) {
            log("channel!=datagram::system_channel");
            check(us::gov::socket::datagram::counters.wrong_channel, (uint32_t)1);
            check_clients_size(sz);
            check_ban_count(bc + 1);
            connect_banned();
        }
        else {
            log("channel==datagram::system_channel");
            check(us::gov::socket::datagram::counters.wrong_channel,(uint32_t)0);
            if (decl_sz<us::gov::socket::datagram::h || decl_sz>us::gov::socket::datagram::maxsize) {
                log("decl_sz<us::gov::socket::datagram::h || decl_sz>us::gov::socket::datagram::maxsize","true",decl_sz<us::gov::socket::datagram::h, decl_sz>us::gov::socket::datagram::maxsize, decl_sz, us::gov::socket::datagram::h, us::gov::socket::datagram::maxsize);
                check(us::gov::socket::datagram::counters.wrong_size,(uint32_t)1);
                check_clients_size(sz);
                check_ban_count(bc+1);
            }
            else {
                log("decl_sz<us::gov::socket::datagram::h || decl_sz>us::gov::socket::datagram::maxsize","false",decl_sz<us::gov::socket::datagram::h, decl_sz>us::gov::socket::datagram::maxsize, decl_sz, us::gov::socket::datagram::h, us::gov::socket::datagram::maxsize);
                if (howmany<decl_sz) {
                    log("howmany<decl_sz",howmany,decl_sz);
                    cause_stall();
//                        check_ban_count(bc+1);
                    check_ban_count(bc); // HX7T56  //check_ban_count(bc+1);
//                          check_clients_size(sz); //HX7T56 check_clients_size(sz);
                }
                else {
                    log("howmany>=decl_sz", howmany, decl_sz);
                    static unordered_set<uint16_t> valid_service_id{
                        us::gov::protocol::socket_error,
                        us::gov::protocol::socket_ping,
                        us::gov::protocol::id_request,
                        us::gov::protocol::id_peer_challenge,
                        us::gov::protocol::id_challenge_response
                    };
                    uint16_t svc=w[datagram::offset_service];
                    svc |= w[datagram::offset_service + 1] << 8;
                    if (valid_service_id.find(svc) == valid_service_id.end()) {
                        log("invalid service",svc);
                        bore_server();
                        check_ban_count(bc + 1);
                        check_clients_size(sz);
                    }
                    else {
                        log("valid service", svc);
                        check(tot == howmany, true);
                        bore_server();
                        check_ban_count(bc);
                        check_clients_size(sz + 1);
                    }
                }
            }
        }
    }
    log("reset test");
    delete [] w;
    delete p;
    reset_ban();
    reset_dgram_counters();
    reset_clients();
    check_initial_state();
}

void c::test_immediate_disconnect() {
}

void c::fuzzy_datagram_fast() {
    tee("fuzzy_datagram_fast");
   // datagram::system_channel=1;
//send 8 bytes, encoded sz=9 channel 123

//        send_bytes(20000001,20000001,datagram::system_channel);
//        send_bytes(19999999,19999999,datagram::system_channel);
//        send_bytes(1,0,datagram::system_channel);
//        send_bytes(19999999,0,datagram::system_channel);
//        send_bytes(1,99999999,datagram::system_channel);
//        send_bytes(19999999,19999999,datagram::system_channel);
//        send_bytes(8,20000000,datagram::system_channel);
    channel_t chan = 123;
    tee("chan", chan);
    assert(chan != 0);

    auto infrasizes = initializer_list<uint32_t>{1, datagram::h - 1};
    auto sizes = initializer_list<uint32_t>{datagram::h, datagram::h + 1, datagram::maxsize - 1, datagram::maxsize, datagram::maxsize + 1};


    tee("==> sending nothing after connecting");
    send_bytes(0, 0, chan, 0);

    tee("==> sending bytes smaller than the header ");
    for (uint32_t real_sz: infrasizes) {
        cout << "==+> sending bytes smaller than the header. sz=" << real_sz << endl;
        send_bytes(real_sz, 0, 0, 0);
    }

    tee("==> sending headers to a different channel (0)");
//        cout << "different declared sizes" << endl;
    for (uint32_t decl_sz: sizes) {
        cout << "==+> sending headers to a different channel (0) - size" << datagram::h << "decl_sz" << decl_sz << endl;
        send_bytes(datagram::h, decl_sz, 0, 0);
    }
//ok till here. it might be unstable though, some runs don't work


    tee("==> sending headers to the right channel (", chan, ")");
    for (uint32_t decl_sz: sizes) { //
        if (decl_sz != datagram::h) {
            tee("==+> sending headers to the right channel (", chan, ") - size", datagram::h, "decl_sz", decl_sz);
            send_bytes(datagram::h, decl_sz, chan, 0);

        }
    }

    tee("==> sending decrypted datagrams (svc=socket_error) to the right channel (", chan , ")");
    uint16_t svc = us::gov::protocol::socket_error;
    for (uint32_t real_sz: sizes) {
        for (uint32_t decl_sz: sizes) {
            tee("==+> sending decrypted datagrams ((svc=gov_socket_error) to the right channel (", chan, ")");
            send_bytes(real_sz, decl_sz, chan, svc);
        }
    }

    tee("==> sending decrypted datagrams (svc unhandled) to the right channel (", chan ,")");
    svc = numeric_limits<uint16_t>::max();
    for (uint32_t real_sz: sizes) {
        for (uint32_t decl_sz: sizes) {
            tee("==+> sending decrypted datagrams (svc unhandled) to the right channel (", chan, ")");
            send_bytes(real_sz, decl_sz, chan, svc);
        }
    }
}

void c::fuzzy_datagram_slow() {
    tee("fuzzy_datagram_slow");
    auto prev = us::gov::peer::peer_t::auth_window;
    us::gov::peer::peer_t::auth_window = 20s;

    channel_t chan = 123;
    tee("chan", chan);

    tee("auth window:", us::gov::peer::peer_t::auth_window.count(), "secs");
    assert(us::gov::peer::peer_t::auth_window.count() > 1);

    {
        tee("==> slowly sending header");
        auto sz = datagram::h;
        send_bytes(sz, sz, chan, us::gov::protocol::socket_error, true);
    }
    {
        tee("==> slowly sending decrypted datagram (svc!=0) transmission lasting less than auth_window");
        cout << "h+9 bytes out in 9 seconds (< auth_window (" << us::gov::peer::peer_t::auth_window.count() << "s))" << endl;
        auto sz = us::gov::peer::peer_t::auth_window.count() - 1;
        send_bytes(sz, sz, chan, us::gov::protocol::socket_error, true);
    }
    {
        tee("==> slowly sending decrypted datagram (svc!=0) transmission lasting more than auth_window");
        cout << "h+11 bytes out in 11 seconds (> auth_window (" << us::gov::peer::peer_t::auth_window.count() << "s))" << endl;
        auto sz = us::gov::peer::peer_t::auth_window.count() + 1;
        send_bytes(sz, sz, chan, us::gov::protocol::socket_error, true);
    }
    us::gov::peer::peer_t::auth_window = prev;
}


void c::fuzzy_datagram() {
    fuzzy_datagram_fast();
    fuzzy_datagram_slow();
}

void c::self_test() {
    check_initial_state();

    cout << "==========connect_disconnect" << endl;
    connect_disconnect();

//    cout << "connect_disconnect2" << endl;
//    connect_disconnect2();

    cout << "==========connect_quiet" << endl;
    connect_quiet();

/*
    check_idle_threads(threadpool_size);
    connect_disconnect();
    connect_disconnect2();
    connect_quiet();
*/
    //connect_from_invalid_net_address();
    cout << "==========fuzzy tests" << endl;
    fuzzy_datagram();

}

bool c::is_loading() const {
    return load::is_active();
}

void c::test_load(int num_dgrams, chrono::milliseconds& timebetweendgrams) {
    cout << "load started with num_dgrams " << num_dgrams << " timebetweendgrams " << timebetweendgrams.count() << " ms" << endl;
    log_start(test_load_logdir, "main");
    log("started wallet cli");
    log("sending evidences");
    auto k = us::gov::crypto::ec::keys::generate();


    struct cli_t: us::gov::cli::rpc_daemon_t {
        using b = us::gov::cli::rpc_daemon_t;
        using b::rpc_daemon_t;

        void on_peer_disconnected(const string& reason) override {
            b::on_peer_disconnected(reason);
            cout << reason << endl;
            assert(false);
        }
    };


    shostport_t shostport = make_pair(myaddress, myport);

    cli_t cli(123, k, shostport, peer_t::role_device, nullptr);
    #if CFG_LOGS == 1
        cli.logdir = logdir;
    #endif
    assert(cli.start() == ok);
    assert(is_ok(cli.wait_ready(3)));
    assert(cli.sendq_active());
//    log("trace CVS72w3");
    assert(cli.peer->daemon.is_active());
    int counter = 0;
    while(load::isup() && counter < num_dgrams) {
        datagram* d;
        hash_t h;
        {
            okev ev;
            ev.src_port = myport;
            h = ev.hash_id();
            log("okev +", h);
            d = ev.get_datagram(123, 0);
 //           cout << "send okev dgram sz = " << d->size() << endl;
            {
                okev ev2;
                //us::gov::io::blob_reader_t reader(*d);
                assert(is_ok(ev2.read(*d)));
            }

        }
        if (timebetweendgrams.count() > 0) {
            this_thread::sleep_for(timebetweendgrams);
        }
        if (load::isdown()) break;
        evc->newev(h, myport - govport);
//        tee("sent another", evsent_succ , h);
//    log("trace CVS72w2");
        assert(cli.peer->daemon.is_active());
        ko r = cli.peer->send1(d);
        if (r != ok) {
            cout << r << endl;
        }
        assert(r == ok);
        ++counter;
        ++evsent_succ;
        log("send_datagram.", myport);
    }
    cout << "end sending. waiting for evc be flushed" << endl;
    evc->wait_empty();
    cout << "waiting for 4 secs to stop servers" << endl;
    this_thread::sleep_for(4s);
    cout << "stopping loader" << endl;
    cli.stop();
    cli.join();
}



//=_==============


bool c::recv_ev(const okev& e) {
    auto h = e.hash_id(); //d->compute_hash();
//        tee("arrived_evidence", h, "recv_evs", static_cast<networking&>(daemon).recv_evs);
    {
        lock_guard<mutex> lock(evsseen.mx);
        if (evsseen.find(h) != evsseen.end()) {
            ++dupevs;
            return false;
        }
        evsseen.emplace(h);
        ++uniqevs;
    }
    evc->recv(h, myport - govport);
    return true;
}

bool c::peer_t::process_work(datagram* d) {
    if (d->service < us::gov::protocol::dfs_end) {
        return b::process_work(d);
    }
    if (d->service == us::gov::protocol::engine_ev) {
        //cout << "recvd okev dgram sz = " << d->size() << endl;
        okev e;
        assert(is_ok(e.read(*d)));
        //++static_cast<networking&>(daemon).recv_evs;
        if (static_cast<networking&>(daemon).recv_ev(e)) {
            static_cast<networking&>(daemon).relay_evidence(*d, this);
            delete d;
        }
        else {
            delete d;
        }
        return true;
    }

    return false;
}

void c::peer_t::verification_completed(pport_t rpport, pin_t pin) {
    //log("verification_completed",rpport);
    b::verification_completed(rpport, pin);
    if (!verification_is_fine()) {
        //log("verification_not_fine->disconnect",ip4_decode(address),port);
        disconnect(0, "");
        return;
    }

    if (!static_cast<us::gov::peer::daemon_t&>(daemon).grid.add(*this, true)) {
        //log ("disconnect-grid doesn't accept", client::ip4_decode(address), port, sock);
        disconnect(0, "");
        return;
    }

    //log("added to grid");
}

bool c::peer_t::authorize(const pub_t& p, pin_t pin) {
    cout << "Authorizing incoming peer pub " << p << " pin " << pin << endl;
    return true;
//    return b::authorize(p, pport);
}

