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
#include "server.h"
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <unordered_set>
#include <chrono>
#include <ctime>
#include <iostream>

#include <us/gov/likely.h>
#include <us/gov/socket/client.h>
#include <us/gov/socket/daemon_t.h>

#define loglevel "gov/socket/multipeer"
#define logclass "server"
#include <us/gov/socket/logs.inc>

using namespace us::gov::socket;
using c = us::gov::socket::multipeer::server;

constexpr chrono::seconds c::blacklist::bantime;

#if CFG_COUNTERS == 1
    c::counters_t c::counters;
#endif

c::server(daemon_t& daemon, port_t p): port(p), b(bind(&c::run_recv, this), bind(&c::onwakeup, this)), daemon(daemon) {
    log("server constructor - no dispatcher. listening port", p);
}

c::~server() {
    log("server destroyed");
    join();
}

sock_t c::make_socket(port_t port) {
    log("make_socket", port);
    sock_t sock;
    struct sockaddr_in name;
    sock = ::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (sock < 0) {
        cerr << "socket: server: could not create socket " << errno << " " << strerror(errno) << endl;
        return -1;
    }
    int optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    name.sin_family = AF_INET;
    name.sin_port = htons(port);
    name.sin_addr.s_addr = htonl (INADDR_ANY);
    if (::bind(sock, (struct sockaddr *) &name, sizeof (name)) < 0) {
        cerr << "port " << port << " is busy." << endl;
        return -1;
    }
    return sock;
}

bool c::banned_throttle(const string& addr) {
    log("WARNING throttle control is disabled"); // TODO
    return false;
    static unordered_set<string> throttle;
    auto x = throttle.find(addr);
    if (x == throttle.end()) {
        throttle.emplace(addr);
        return false;
    }
    else {
        return true;
    }
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "Hello from socker::server\n";
    os << prefix << "Listening socket: " << sock << '\n';
    os << prefix << "Listening TCP port: " << port << '\n';
    os << "recv:\n";
    #if CFG_COUNTERS == 1
        os << "bytes_received " << datagram::counters.bytes_received << '\n';
    #endif
}

#define ENABLE_BAN 0

void c::proc_evil(const vector<pair<host_t, string>>& e) {
    for (auto& i: e) {
        #if ENABLE_BAN == 1
            #ifdef NDEBUG
                Force fail - RBF blacklist not tested enough to go to production
            #endif
            ban.add(i.first, i.second);
            log("banned ", i.endpoint());
        #else
            log("RBF would BAN");
        #endif
        #if CFG_COUNTERS == 1
            ++counters.blacklisted_addr;
        #endif
    }
    static int n{0};
    if (++n == 1000) {
        n = 0;
        ban.purge();
    }
}

void c::onwakeup() {
    log("onwakeup");
    daemon.peers.feedlo();
}

void c::run_recv() {
    #if CFG_LOGS == 1
        ostringstream os;
        os << "traffic-inbound-multipeer-port_" << port;
        log_start(logdir, os.str());
    #endif
    assert(EAGAIN == EWOULDBLOCK); //assuming they are the same value for this POSIX impl. POSIX does not require these value have the same value.
    log("recv thread init");
    busyled.set();
    fd_set read_fd_set;
    int i;
    struct sockaddr_in clientname;
    unsigned int size;
    assert(port);
    sock = make_socket(port);
    if (sock < 0) {
        auto r = "KO 89143 socket failure";
        log(r);
        assert(false);
        return;
    }
    if (::listen(sock, 4) < 0) {
        close(sock);
        sock = -1;
        log("end", "listen failure");
        return;
    }
    auto r = daemon.peers.init(port);
    if (unlikely(is_ko(r))) {
        close(sock);
        sock = -1;
        log(r);
        cerr << r << ". Failed connecting the loopback client.\n";
        log("end", "loopback failure");
        return;
    }
    sock_t loopback;
    size = sizeof(clientname);
    loopback = ::accept4(sock, (struct sockaddr *) &clientname, &size, SOCK_NONBLOCK);
    if (loopback < 0) {
        log("KO 3067 ::accept loopback failed");
        close(sock);
        log("end", "loopback failure in accept");
        assert(false);
        return;
    }
    char discard[4];
    vector<sock_t> honest;
    vector<pair<host_t, string>> evil;
    auto maxs0 = sock > loopback ? sock : loopback;
    #if CFG_CONNECTION_LOG == 1
        ostringstream filename_connlog;
        filename_connlog << LOGDIR << "/connection_log_" << this_thread::get_id();
        log("Writting new connections to", filename_connlog.str());
        ofstream conlog(filename_connlog.str());
    #endif
    log("main recv loop");
    while (isup()) {
        FD_ZERO(&read_fd_set);
        FD_SET(sock, &read_fd_set);
        FD_SET(loopback, &read_fd_set);
        log("update");
        #if CFG_COUNTERS == 1
            ++counters.select_cycles;
        #endif
        daemon.peers.update(honest, evil);
        proc_evil(evil);
        auto maxs = maxs0;
        for (auto& i: honest) {
            assert(i != 0);
            if (i > maxs) maxs = i;
            FD_SET(i, &read_fd_set);
        }
        log("select");
        busyled.reset();
        sock_t r = ::select(maxs + 1, &read_fd_set, 0, 0, 0);
        busyled.set();
        if (unlikely(isdown())) {
            log("terminated");
            break;
        }
        log(r, maxs);
        if (likely(r > 0)) {
            if (FD_ISSET(sock, &read_fd_set)) {
                log("accept", sock);
                sock_t nnew;
                size = sizeof(clientname);
                nnew = ::accept4(sock, (struct sockaddr *) &clientname, &size, SOCK_NONBLOCK);
                if (likely(nnew >= 0)) {
                    #if CFG_COUNTERS == 1
                       ++counters.new_connections;
                    #endif
                    daemon.new_peer(nnew);
                }
                else {
                    switch (errno) {
                        case EMFILE:
                            log("KO 00024 Too many files.", errno, strerror(errno));
                            break;
                        default:
                            log("KO 7092 error in accept.", errno, strerror(errno));
                    }
                }
            }
            log("honest sz ", honest.size());
            {
                unique_lock<mutex> lock(daemon.peers.mx);
                for (sock_t i: honest) {
                    if (!FD_ISSET (i, &read_fd_set)) continue;
                    auto cl = daemon.peers.find(i);
                    if (unlikely(cl == daemon.peers.end())) {
                        log("data arrived for an unknown fd ", i);
                        #if CFG_COUNTERS == 1
                            ++counters.data_arrived_unmapped_fd;
                        #endif
                        continue;
                    }
                    log("work out incoming data", cl->second->endpoint());
                    receive_and_process(cl->second);
                }
            }
            if (FD_ISSET(loopback, &read_fd_set)) {
                log("flush loopback");
                ::recv(loopback, &discard, 4, 0);
            }
        }
        else {
            #if CFG_COUNTERS == 1
               ++counters.select_errors;
            #endif
            if (errno == EINTR) {
                #if CFG_COUNTERS == 1
                    ++counters.select_EINTR;
                #endif
                log("EINTR");
            }
            else if (errno == EBADF) {
                #if CFG_COUNTERS == 1
                    ++counters.select_EBADF;
                #endif
                log("Bad file descriptor", maxs);
            }
            else {
               log("uncaught condition in select", errno, strerror(errno));
            }
            continue;
        }
    }

    close(sock);
    close(loopback);
    sock = -1;
    loopback = -1;
    log("disconnect-loopback");
    daemon.peers.cleanup();
    busyled.reset();
    log("end");
}

void c::blacklist::dump(ostream& os) const {
    for (auto& i: *this) {
        auto n = chrono::system_clock::to_time_t(i.second.first);
        os << client::ip4_decode(i.first) << ' ' << n << ' ' << i.second.second << '\n';
    }
}

void c::blacklist::add(host_t addr, const string& reason) {
    log("banned", client::ip4_decode(addr), reason);
    emplace(addr, make_pair(chrono::system_clock::now(), reason));
}

void c::blacklist::purge() {
    auto n = chrono::system_clock::now();
    for (auto i = begin(); i != end(); ) {
        if (i->second.first - n < bantime) {
            ++i;
            continue;
        }
        else {
            i = erase(i);
        }
    }
}

#if CFG_COUNTERS == 1
    void c::counters_t::dump(ostream& os) const {
        os << "select_cycles " << select_cycles << '\n';
        os << "select_errors " << select_errors << '\n';
        os << "select_EINTR " << select_EINTR << '\n';
        os << "select_EBADF " << select_EBADF << '\n';
        os << "new_connections " << new_connections << '\n';
        os << "closed_banned " << closed_banned << '\n';
        os << "data_arrived_unmapped_fd " << data_arrived_unmapped_fd << '\n';
        os << "blacklisted_addr " << blacklisted_addr << '\n';
        os << "disconnection_normal " << disconnection_normal << '\n';
        os << "disconnection_unknown_service " << disconnection_unknown_service << '\n';
    }
#endif

