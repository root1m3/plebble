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
#ifdef __FreeBSD__
#include <sys/socket.h>
#endif

#include <us/gov/likely.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/socket/client.h>

#include "caller_daemon_t.h"

#define loglevel "gov/socket/outbound"
#define logclass "send_queue_t"
#include <us/gov/socket/logs.inc>

using namespace us::gov::socket;
using c = us::gov::socket::send_queue_t;

c::send_queue_t(): t(bind(&c::run_send, this), bind(&c::onwakeup, this)) {
    log("new sendq at mem ", this);
}

c::~send_queue_t() {
    log("sendq destroyed", this);
    clear();
}

void c::onwakeup() {
    log("onwakeup");
    cv.notify_all();
}

void c::clear() {
    unique_lock<mutex> lock(mx);
    ongoing.clear();
    while (!empty()) {
        qi* i = top();
        pop();
        delete i;
    }
}

void c::dump(const string& prefix, ostream& os) const {
    unique_lock<mutex> lock(mx);
    os << prefix << "active " << is_active() << '\n';
    os << prefix << "wmh " << wmh << '\n';
    os << prefix << "schunk " << schunk << '\n';
    os << prefix << "queued " << size() << '\n';
    #if CFG_COUNTERS == 1
        os << prefix << "dgrams_sent " << sent << '\n';
        os << prefix << "dgrams_dropped " << dropped << '\n';
        os << prefix << "bytes_sent " << bytes_sent << '\n';
    #endif
}

size_t c::get_size() const {
    unique_lock<mutex> lock(mx);
    return size();
}

ko c::send(datagram* d, client* peer, uint16_t pri) {
    log("admission pri", pri, "sz", d->size(), "seq", d->decode_sequence(), "recipient", peer->endpoint(), "dgram svc field (to be overwritten)", d->service, "svc", d->decode_service());
    assert(d->completed());
    log("overwriting service field with pri", pri);
    d->service = pri;
    unique_lock<mutex> lock(mx);
    if (unlikely(size() >= wmh)) {
        delete d;
        auto r = "KO 30221 Queue reached its max capacity. Datagram discarded.";
        log(r);
        return r;
    }
    auto i = new qi(d, peer);
    if (!attic.add_(i)) {
        push(i);
    }
    task_wakeup();
    return ok;
}

attic_t::~attic_t() {
    for (auto& i: clindex) {
        for (auto& j: i.second) {
            delete j;
        }
    }
}

bool attic_t::add_(qi* i) {
    auto n = clindex.find(i->second);
    if (n == clindex.end()) return false;
    n->second.emplace_back(i);
    return true;
}

void attic_t::add_(vector<qi*>&& i) {
    using namespace std::chrono;
    assert(!i.empty());
    client* x = (*i.begin())->second;
    assert(clindex.find(x) == clindex.end());
    clindex.emplace(x, i);
    emplace(duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count(), x);
}

int64_t attic_t::rem_time() const {
    using namespace std::chrono;
    assert(!empty());
    return min_age - (duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count() - begin()->first);
}

void c::resume_clients_() {
    using namespace std::chrono;
    if (likely(attic.empty())) return;
    log("retrieve clients on-hold. attic sz=", attic.size());
    uint64_t now = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
    auto i = attic.begin();
    while (i != attic.end()) {
        if ((now - i->first) < attic_t::min_age) {
            log("client needs more holding time.", i->second->endpoint());
            break;
        }
        client* x = i->second;
        auto v = attic.clindex.find(x);
        assert(v != attic.clindex.end());
        log("client's jobs returning to queue.", x->endpoint(), ';', v->second.size(), "jobs.");
        for (auto& n: v->second) {
            if (n->first->in_progress()) {
                log("queuing ongoing job for this cli. dend=", n->first->dend, '/', n->first->size());
                ongoing.set(n);
            }
            else {
                log("queueing not-started-yet job.");
            }
            push(n);
        }
        attic.clindex.erase(x);
        i = attic.erase(i);
    }
}

void c::hold(client* c) {
    log("attic_store client", c->endpoint(), "initial size", size());
    unique_lock<mutex> lock(mx);
    vector<qi*> v;
    vector<qi*> v2;
    v.reserve(size());
    while(!empty()) {
        qi* x = top();
        pop();
        if (x->second == c) {
            log("moving datagram to attic", x->first, "progress", x->first->dend, '/', x->first->size());
            ongoing.reset(x);
            v2.emplace_back(x);
        }
        else {
            v.emplace_back(x);
        }
    }
    for (auto& i: v) {
        push(i);
    }
    attic.add_(move(v2));
}

qi* c::next() {
    unique_lock<mutex> lock(mx);
    return next_();
}

qi* c::next_() {
    resume_clients_();
    while (true) {
        if(empty()) {
            log("empty");
            reset_wait();
            return nullptr;
        }
        qi* i = top();
        if (i->first->size() == i->first->dend) {
            log("finished. comm completed send. dend=", i->first->dend, "svc=", i->first->service, "hash=", i->first->compute_hash());
            pop();
            ongoing.reset(i);
            delete i;
            continue;
        }
        log("dgam cli", i->second, "pri", i->first->service, "progress", i->first->dend, '/', i->first->size());
        //avoid starting a new dgram for a client that has another ongoing
        qi* r = replace(i); //same i (if no other qi has been started for this client), othersise the unfinished qi
        if (r != i) { //--strip
            log("dgam cli", i->second, "pri", i->first->service, "progress", i->first->dend, '/', i->first->size()); //--strip
        } //--strip
        return r;
    }
}

qi* c::replace(qi* x) {
    assert(x != nullptr);
    auto i = ongoing.find(x->second);
    if (i != ongoing.end()) {
        if (i->second != x) {  //--strip
            log("replaced"); //--strip
        }                    //--strip
        if (i->second->first->dend == i->second->first->size()) {
            log("not replacing with a finished dgram"); //--strip
            ongoing.erase(i);
            return x;
        }
        return i->second;
    }
    return x;
}

void c::run_send() {
    #if CFG_LOGS == 1
        log_start(logdir, "traffic-outbound");
    #endif
    log("ethernet datagram estimated optimal size", schunk);
    while(isup()) {
        log("take next");
        qi* d = next();
        if (d == nullptr) {
            log("sleep");
            cv_empty.notify_all();
            wait();
            log("awaken");
            continue;
        }
        size_t rem = d->first->size()-d->first->dend;
        size_t sz = rem < 2 * schunk ? rem : schunk;
        log("dgram pri", d->first->service, (d->first->decode_service() == 0 ? "encrypted" : "public"), "remaining", rem, "dgram progress", d->first->dend, "/", d->first->size());
        if (unlikely(d->second->sendref.load() < 0)) { //was: !d->second->sendref.load()<0   ???
            log("client dying", d->second, "dgram progress was", d->first->dend, "/", d->first->size());
            d->first->dend = d->first->size();
            continue;
        }
        if (unlikely(d->second->sock < 0)) {
            log("sock? fd=", d->second->sock,". dgram progress was", d->first->dend, "/", d->first->size());
            d->first->dend = d->first->size();
            continue;
        }
        busyled.set();
        int n = ::send(d->second->sock, d->first->data() + d->first->dend, sz, MSG_NOSIGNAL);
        busyled.reset();
        if (likely(n >= 0)) {
            if (d->errcount > 0) {     //--strip
                log("Recovering after", d->errcount, "consecutive errors."); //--strip
            } //--strip
            d->errcount = 0;
//            d->skip = 0;
            log("::send fd ", d->second->sock, "sz", sz, "returned", n);
            d->first->dend += n;
            log("sent", n, "to", d->second->endpoint(), "progress", d->first->dend, "/", d->first->size(), '[', (100.0*((float)d->first->dend)/((float)(d->first->size()))), "%]");
            #if CFG_COUNTERS == 1
                ++sent;
                bytes_sent += n;
            #endif
            if (d->first->in_progress()) {
                ongoing.set(d);
            }
            else {
                ongoing.reset(d);
            }
            continue;
        }
        else {
            log("::send fd ", d->second->sock, "sz", sz, "returned", n, errno, strerror(errno));
        }
        //issues
        #if CFG_COUNTERS == 1
            ++dropped;
        #endif
        log("info constants: EINPROGRESS=", EINPROGRESS, "EAGAIN=", EAGAIN);
        log("errno", errno, strerror(errno), "errcount count", d->errcount);
        switch(errno) {
            case EINPROGRESS:
            case EAGAIN:
                log("temp error");
                hold(d->second);
                if (++d->errcount >= 255) {
                    log("disconnect after errcount maxed.");
                    d->second->disconnect(0, "");
                }
                continue;
            case EBADF: //9 Bad file descriptor
            case EPIPE: //32 Broken pipe
                log("EBADF(9) EPIPE(32) handler", errno);
                log("disconnect");
                d->second->disconnect(0, "");
            default:
                log("def handler", errno);
                d->first->dend = d->first->size(); //finish with this dgram
                log("KO 93219 errno ", errno, strerror(errno));
                continue;
        }
    }
    log("end");
}

bool c::wait_empty() {
    log("wait_empty_");
    unique_lock<mutex> lock(mx);
    log("wait_empty_mx");
    if (empty()) return true;
    log("wait_empty_ sz", size());
    cv_empty.wait_for(lock, 3s, [&]{ return empty() || isdown(); });
    log("wait_empty_/_ final sz", size());
    if (!empty()) {
        auto r = "KO 20951 Couldn't empty the queue";
        log(r);
        return false;
    }
    return true;
}

void c::wait() {
    if (reset_wait()) {
        return;
    }
    unique_lock<mutex> lock(mx);
    if (attic.empty()) {
        log("wait till signal");
        cv.wait(lock, [&]{ return resumed(); });
    }
    else {
        int64_t rt = attic.rem_time();
        if (rt > 0) {
            log("wait for ", rt, "ns");
            cv.wait_for(lock, std::chrono::nanoseconds(rt), [&]{ return resumed(); });
        }
    }
    reset_wait();
}

void c::set_busy_handler(busyled_t::handler_t* hsend) {
    busyled.set_handler(hsend);
}

void c::ongoing_t::set(qi* x) {
    auto i = find(x->second);
    if (i != end()) {
        assert(x->first == i->second->first);
        return;
    }
    log("client", x->second, "has an ongoing unfinished item", x);
    emplace(x->second, x);
}

qi* c::ongoing_t::reset(qi* x) {
    auto i = find(x->second);
    if (i == end()) return nullptr;
    erase(i);
    return x;
}

qi::qi(datagram* d, client* peer) {
    first = d;
    second = peer;
    log("new qi. dgram sz", d->size(), "pri", d->service, "cli", peer, "sendref", peer->sendref.load());
    d->dend = 0;
    ++peer->sendref;
}

qi::~qi() {
    delete first;
    --second->sendref;
}

bool priority_service::operator () (qi*& lhs, qi*& rhs) const {
    return lhs->first->service > rhs->first->service;
}


