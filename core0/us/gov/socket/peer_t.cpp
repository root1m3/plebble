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
#include "peer_t.h"
#include <us/gov/config.h>
#include "daemon_t.h"
#include "rpc_daemon_t.h"

#define loglevel "gov/socket"
#define logclass "peer_t"
#include "logs.inc"

using namespace us::gov::socket;
using c = us::gov::socket::peer_t;

const char* c::KO_50143 = "KO 50143 Unexpected service number.";
const char* c::KO_20179 = "KO 20179 Ignored.";
const char* c::finished_reason_1 = "stalled datagram";
const char* c::finished_reason_2 = "not alive";

constexpr chrono::seconds c::stall;

thread_local string c::lasterror;

c::peer_t(rpc_daemon_t& d): b(d) {
}

c::peer_t(daemon_t& d, sock_t sock): b(d, sock) {
}

c::~peer_t() {
    delete curd;
}

void c::ping() {
    string pong;
    auto r = call_ping("ping", pong);
}

void c::on_peer_disconnected(const string& reason) {
    log("on_peer_disconnected", reason);
}

bool c::stalled_datagram() const {
    using namespace std::chrono;
    log("check stalled");
    lock_guard<mutex> lock(mx_curd);
    if (likely(curd == nullptr)) {
        log("no dgram created yet");
        return false;
    }
    log("dgram.curd.dend", curd->dend);
    auto d = system_clock::now() - activity_recv;
    if (d < stall) {
        log("not yet stalled", curd->dend, "sz", curd->size(), "dend", curd->dend, "svc", curd->service, "age", client::age(activity_recv));
        return false;
    }
    log("stalled_datagram", curd->dend, curd->size(), curd->dend, curd->service);
    return true;
}

pair<c::security_level_t, string> c::security_level() const {
    auto sl = b::security_level();
    if (sl.first != security_level_ok) {
        return sl;
    }
    assert(sl.first == security_level_ok);
    if (stalled_datagram()) {
        log ("stalled_datagram", "returning security_level_ban");
        log("RBF 2011 WARNING it should be ban, or disconned. caused disconnect then querying files");
        return make_pair(security_level_disconnect, finished_reason_1);
    }
    return sl;
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "socket::peer_t:\n";
}

void c::dump_all(const string& prefix, ostream& os) const {
    dump(prefix, os);
    b::dump_all(prefix, os);
}

void c::prepare_worker_recv() {
    daemon.recv_led().set();
    ++busyrecv;
    log("prepare_worker recv", "TAG661");
}

void c::worker_ended_recv() {
    --busyrecv;
    daemon.recv_led().reset();
    log("worker ended recv", "TAG661");
}

void c::process_work() { ///Entry point of a worker thread
    log("newtask", "TAG301", endpoint());
    datagram* d;
    {
        lock_guard<mutex> lock(mx_curd); //TODO check neccesity of this mutex
        d = curd;
        curd = nullptr;
    }
    log("cur dgram", d);
    seq_t seq = 0;
    pair<ko, datagram*> r = recv4x(d, static_cast<daemon_t&>(daemon).recv_led());
    if (is_ko(r.first)) {
        if (r.first == datagram::KO_9021) {
            #if CFG_COUNTERS == 1
                ++socket::client::counters.zero_read;
            #endif
            log("QUICK_RECONNECT issue traces here");
            disconnect(0, "");  /// tests ref.  search token QUICK_RECONNECT
        }
        else {
            if (unlikely(r.first == datagram::KO_0001)) {
                log("disconnect peer", r.first);
                auto ch = daemon.channel;
                if (r.second != nullptr) {
                    seq = r.second->decode_sequence();
                    ch = r.second->decode_channel();
                    log("dropping dgram. ch=", ch, "seq=", seq);
                    delete r.second;
                    r.second = nullptr;
                }
                #if CFG_COUNTERS == 1
                    ++socket::client::counters.disconnection_wrong_read; //TODO wrong channel
                #endif
                disconnectx(ch, seq, r.first);
            }
            else {
                log("disconnect peer", r.first);
                if (r.second != nullptr) {
                    log("dropping dgram");
                    seq = r.second->decode_sequence();
                    delete r.second;
                    r.second = nullptr;
                }
                #if CFG_COUNTERS == 1
                    ++socket::client::counters.disconnection_wrong_read;
                #endif
                disconnect(seq, r.first);
            }
        }
    }
    else if (r.second->completed()) {
        seq = r.second->decode_sequence();
        log("dgram reception completed", "decrypting");
        r = decrypt0(r.second);
        if (unlikely(is_ko(r.first))) {
            log("error", r.first);
            disconnect(seq, r.first);
        }
        else {
            datagram* d = r.second;
            log("complete dgram arrived", d, "sz", d->size(), "svc", d->service, endpoint());
            bool processed;
            try {
                log("dispatch job", endpoint());
                processed = process_work(d);
                log("dispatched job", endpoint());
            }
            catch(const exception& e) {
                log("worker-exception", "job uncontrollably failed, likely leaking d", endpoint(), e.what());
                processed = true;
            }
            if (unlikely(!processed)) {
                log("unhandled. handed over to daemon.");
                daemon.process_unhandled(*this, d);
            }
            else {
                log("work completed");
            }
        }
    }
    else {
        lock_guard<mutex> lock(mx_curd);
        assert(curd == nullptr);
        assert(r.second != nullptr);
        curd = r.second;
        log("partial dgram reception", curd->size(), "so far recvd", curd->dend);
    }
    worker_ended_recv();
    log("end service", "TAG301");
    static_cast<daemon_t&>(daemon).wakeup_handler();
}

ko c::send1(datagram* d) {
    if (unlikely(!daemon.is_active())) {
        auto r = "KO 73101 Daemon is inactive.";
        log(r);
        return r;
    }
    return daemon.send1(*this, d);
}

std::pair<ko, datagram*> c::sendrecv(datagram* d, string& remote_error) {
    log("sendrecv");
    return daemon.sendrecv(*this, d, remote_error);
}

void c::process_ok_work(datagram* d) {
    log("process_ok_work", d->service);
    daemon.process_ok_work(*this, d);
}

void c::process_ko_work(channel_t channel, seq_t seq, ko r) {
    log("process_ko_work. seq", seq, channel, r);
    daemon.process_ko_work(*this, channel, seq, r);
}

void c::set_finished() {
    log("set finished");
    #if CFG_REPORT_REASON == 1
        if (!is_finished()) {
            auto& rsn = get<2>(finished_reason);
            if (!rsn.empty()) {
                log("reporting gov_socket_finished with reason", rsn.empty() ? "N/A" : rsn);
                auto d = blob_writer_t::get_datagram(get<0>(finished_reason), protocol::socket_finished, 0, rsn);
                log("svc socket_finished goes for channel", d->decode_channel());
                ko r = send1(d);
                log(r == ok ? "success" : r);
                log("flushing send_queue");
                daemon.wait_empty();
                log("flushed send_queue");
            }
        }
    #else
        log("this server doesn't report the reason:", get<2>(finished_reason));
    #endif
    b::set_finished();
}

bool c::process_work(datagram* d) {
    using namespace us::gov::protocol;
    assert(d != nullptr);
    assert(d->service < protocol::socket_end);
    #ifdef has_us_gov_socket_api
        switch(d->service) {
            #include <us/api/generated/gov/c++/socket/hdlr_svc-router>
        }
    #endif
    return false;
}

#ifdef has_us_gov_socket_api

#include <us/api/generated/gov/c++/socket/hdlr_svc_handler-impl>

//------------------apitool - API Spec defined @ us/api/generated/gov/c++/socket/hdlr_local-impl

ko c::handle_error(datagram* dgram, string&& msg) {
    log("error", "svc", dgram->service, "sz", dgram->size(), msg);
    daemon.rendezvous.arrived(dgram);
    return ok;
}

ko c::handle_ping(string&& msg, string& ans) {
    log("ping");
    ans = "pong";
    return ok;
}

ko c::handle_finished(string&& reason) {
    log("finished", reason);
    on_peer_disconnected(reason);
    return ok;
}

//-/----------------apitool - End of API implementation.


#include <us/api/generated/gov/c++/socket/cllr_rpc-impl>

#endif

