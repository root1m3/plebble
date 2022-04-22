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
#include "handler_daemon_t.h"
#include <sstream>

#include <us/gov/socket/rpc_daemon_t.h>
#include <us/gov/socket/types.h>
#include <us/gov/config.h>

#define loglevel "gov/socket/inbound/single"
#define logclass "handler_daemon_t"
#include <us/gov/socket/logs.inc>

using namespace us::gov::socket;
using c = us::gov::socket::single::handler_daemon_t;

const char* c::KO_40032 = "KO 40032 Not connected.";

#if CFG_COUNTERS == 1
    c::counters_t c::counters;
#endif

c::handler_daemon_t(rpc_daemon_t& daemon, dispatcher_t* dispatcher):
        b(bind(&c::run_recv, this),
        bind(&c::onwakeup, this)),
        daemon(daemon),
        dispatcher(dispatcher) {
}

ko c::connect() {
    return daemon.connect();
}

void c::stop() {
    log("stop");
    b::stop();
    log("stopping thread: recv");
    cv_recv.notify_all();
    cv_connected.notify_all();
    log("stopping");
}

void c::wait(const time_point& deadline) {
    using namespace chrono;
    auto now = system_clock::now();
    if (now >= deadline) {
        return;
    }
    log("throttle control: waiting for", duration_cast<milliseconds>(deadline - system_clock::now()).count(), "ms before trying again...");
    unique_lock<mutex> lock(mx_connect);
    cv_connected.wait_until(lock, deadline, [&]{ return system_clock::now() >= deadline || isdown(); });
}

bool c::has_peer() const {
    return daemon.peer != nullptr; 
}

peer_t& c::get_peer() {
    assert(daemon.peer != nullptr);
    return *daemon.peer;
}

const peer_t& c::get_peer() const {
    assert(daemon.peer != nullptr);
    return *daemon.peer;
}

bool c::is_connected() const {
    if (unlikely(!is_active())) return false;
    if (unlikely(!has_peer())) return false;
    return get_peer().connected();
}

ko c::wait_connected() {
    if (is_connected()) {
        return ok;
    }
    mutex mxw;
    unique_lock lock(mxw);
    cv_connected.wait(lock, [&] { return is_connected() || isdown(); });
    return is_connected() ? ok : KO_40032;
}

ko c::wait_connection(bool initiate) {
    log("wait_connection", "initiate=", initiate);
    if (initiate) {
        if (is_connected()) {
            log("already connected");
            return ok;
        }
        auto birthline = last_try_connect + chrono::milliseconds(ms);
        wait(birthline);
        last_try_connect = chrono::system_clock::now();
        log("connect");
        return connect();
    }
    log("wait_connected");
    return wait_connected();
}

void c::run_recv() {
    #if CFG_LOGS == 1
        log_start(logdir, "traffic-inbound-single");
    #endif
    log("run_recv");
    last_try_connect = chrono::system_clock::now() - chrono::milliseconds(ms);
    busyled.set();
    while (isup()) {
        {
            ko r = wait_connection(connect_for_recv);
            if (is_ko(r)) {
                if (stop_on_disconnection) {
                    log("stopping");
                    stop();
                    break;
                }
                continue;
            }
        }
        if (unlikely(isdown())) {
            break;
        }
        pair<ko, datagram*> r = get_peer().recv6(busyled);
        if (unlikely(isdown())) {
            delete r.second;
            break;
        }
        if (unlikely(is_ko(r.first))) {
            log("disconnect", r.first, "errno", errno, strerror(errno));
            if (r.first == datagram::KO_9021) {
                assert(r.second == nullptr);
                #if CFG_COUNTERS == 1
                    ++socket::client::counters.disconnection_wrong_read;
                #endif
                get_peer().disconnect(0, "");
            }
            else if (r.first == datagram::KO_0001) {
                #if CFG_COUNTERS == 1
                    ++socket::client::counters.disconnection_wrong_read; //TODO wrong channel
                #endif
                assert(r.second != nullptr);
                auto ch = r.second->decode_channel();
                delete r.second;
                log("dgram arrived for wrong channel", ch, daemon.channel);
                ostringstream msg;
                msg << "wrong channel " << ch << ". This is channel " << daemon.channel;
                get_peer().disconnectx(ch, 0, msg.str());
            }
            else {
                #if CFG_COUNTERS == 1
                    ++socket::client::counters.zero_read;
                #endif
                get_peer().disconnect(0, r.first);
            }
            continue;
        }
        log("process_work");
        assert(r.second != nullptr);
        if (!get_peer().process_work(r.second)) {
            process_unhandled(get_peer(), r.second);
        }
    }
    busyled.reset();
    log("end");
}

void c::onwakeup() {
    log("don't know (yet) how to force a wake up on ::recv");
}

void c::set_busy_handler(busyled_t::handler_t* h) {
    busyled.set_handler(h);
}

bool c::process_unhandled(client& peer, datagram* d) {
    log("process_unhandled", "svc", d->service);
    if (dispatcher != nullptr) {
        log("handed over to dispatcher", "svc", d->service);
        if (dispatcher->dispatch(d)) {
            log("dispatcher consumed the datagram");
            return true;
        }
    }
    log("backend sent an unknown service", d->service);
    delete d;
    return true;
}

#if CFG_COUNTERS == 1
    void c::counters_t::dump(ostream& os) const {
        os << "disconnection_normal " << disconnection_normal << '\n';
        os << "disconnection_unknown_service " << disconnection_unknown_service << '\n';
    }
#endif

