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
#include "daemon_t.h"

#include <chrono>
#include <sstream>
#include <fstream>
#include <filesystem>

#include <us/gov/io/cfg0.h>

#include "protocol.h"
#include "peer_t.h"

#define loglevel "gov/dfs"
#define logclass "daemon_t"
#include "logs.inc"

using namespace us::gov::dfs;
using c = us::gov::dfs::daemon_t;
namespace fs = std::filesystem;

us::gov::dfs::mezzanine::mezzanine(daemon_t* d): d(d), b(bind(&c::run, d), bind(&c::on_dfs_wakeup, d)) {
}

c::daemon_t(channel_t channel): t(this), b(channel) {
}

c::daemon_t(channel_t channel, port_t port, pport_t pport, uint8_t edges, uint8_t devices, int workers, const vector<hostport_t>& sn): b(channel, port, pport, edges, devices, workers), t(this) {
    set_seed_nodes(sn);
}

c::~daemon_t() {
}

void c::on_dfs_wakeup() {
    cv.notify_all();
}

us::ko c::start() {
    log("start");
    ko r = b::start();
    if (unlikely(r != ok)) {
        return r;
    }
    r = t::start();
    if (unlikely(r != ok)) {
        b::stop();
        return r;
    }
    log("started dfs daemon");
    return r;
}

us::ko c::wait_ready(const time_point& deadline) const {
    ko r = b::wait_ready(deadline);
    if (unlikely(r != ok)) {
        return r;
    }
    return t::wait_ready(deadline);
}

ko c::wait_ready(int seconds_deadline) const {
    return wait_ready(chrono::system_clock::now() + chrono::seconds(seconds_deadline));
}

void c::stop() {
    log("stop");
    b::stop();
    log("stopping dfs daemon");
    t::stop();
}

void c::join() {
    b::join();
    log("waiting for dfs daemon");
    t::join();
    log("stopped");
}

bool c::check_dfs_permission(const hash_t& addr) const {
    return true;
}

void c::save(const string& hash, const vector<uint8_t>& data, int propagate) {
    assert(false);
}

string c::load(const string& hash) {
    assert(false);
    return "Not implemented";
}

string c::resolve_filename(const string& filename) {
    string res;
    int max_length = filename.size() / 2 + filename.size();
    res.reserve(max_length);
    for (int i = 0; i < filename.size(); i++) {
        if((i & 1) == 0) {
            res += "/";
        }
        res += filename[i];
    }
    if (res[res.size() - 1] == '/') {
        res.end() - 1;
    }
    return &res[1];
}

void c::wait() {
    if (t::reset_wait()) {
        return;
    }
    log("wait 60s");
    mutex mxw;
    unique_lock<mutex> lock(mxw);
    cv.wait_for(lock, 60s, [&] { return t::resumed(); });
    t::reset_wait();
}

void c::run() {
    #if CFG_LOGS == 1
        log_start(logdir, "dfs");
    #endif
    while(t::isup()) {
        vector<hash_t> v;
        {
            unique_lock<mutex> lock(mx);
            for (auto i = index.begin(); i != index.end(); ) {
                ostringstream file;
                file << fsroot << '/' << i->first;
                if (io::cfg_filesystem::file_exists(file.str())) {
                    i = index.erase(i);
                    continue;
                }
                v.emplace_back(i->first);
                ++i;
            }
        }
        seq_t seq = 0;
        for (auto& i: v) {
            log("send file request to clique", i);
            clique_send(2, 0, blob_writer_t::get_datagram(channel, protocol::dfs_file_request, seq, i));
        }
        wait();
    }
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "Greets from dfs::daemon\n";
    unique_lock<mutex> lock(mx);
    os << prefix << "waiting for " << index.size() << " files:\n";
    for (auto& i: index) {
        os << prefix << ' ' << i.first << ' ' << i.second << '\n';
    }
}

void c::index_set(const vector<pair<hash_t, uint32_t>>& hv) {
    log("index_set", hv.size());
    unique_lock<mutex> lock(mx);
    index.clear();
    for (const auto& h: hv) {
        ostringstream hf;
        hf << fsroot << '/' << h.first;
        uint32_t fsz = ::us::gov::io::cfg_filesystem::file_size(hf.str());
        if (fsz == h.second) {
            log("file already exists with the right size", h.second);
            continue;
        }
        log("file does not exist with the right size", fsz, '/', h.second, "added to index");
        index.emplace(h.first, h.second);
    }
    if (!index.empty()) {
        lock.unlock();
        t::reset_resume();
        cv.notify_all();
    }
}

void c::index_add(const hash_t& h, uint32_t a) {
    ostringstream hf;
    hf << fsroot << '/' << h;
    unique_lock<mutex> lock(mx);
    uint32_t fsz = ::us::gov::io::cfg_filesystem::file_size(hf.str());
    if (fsz == a) {
        log("file", h, "already exist with the right size", a);
        return;
    }
    log("file does not exist with the right size", fsz, '/', a, "added to index");
    index.emplace(h, a);
    lock.unlock();
    t::reset_resume();
    cv.notify_one();
}

size_t c::rm_files(const set<hash_t>& exclude) {
    size_t n = 0;
    for(auto& p: fs::directory_iterator(fsroot)) {
        if (!is_regular_file(p.path())) continue;
        istringstream is(p.path().filename());
        hash_t h;
        is >> h;
        if (exclude.find(h) != exclude.end()) continue;
        ::remove(p.path().c_str());
        ++n;
    }
    return n;
}

void c::rm_file(const hash_t&h) {
    ostringstream hs;
    hs << h;
    ostringstream hf;
    hf << fsroot << '/' << hs.str();
    if (!::us::gov::io::cfg_filesystem::file_exists(hf.str())) return;
    ::remove(hf.str().c_str());
}

void c::index_rm(const hash_t&h) {
    rm_file(h);
    unique_lock<mutex> lock(mx);
    auto i = index.find(h);
    if (i == index.end()) return;
    index.erase(i);
}

void c::index_clear() {
    unique_lock<mutex> lock(mx);
    index.clear();
}

