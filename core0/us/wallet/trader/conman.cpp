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
#include "conman.h"
#include <sstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <tuple>

#include <us/gov/stacktrace.h>
#include <us/gov/io/cfg.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/socket/client.h>
#include <us/gov/peer/peer_t.h>
#include <us/gov/cash/tx_t.h>
#include <us/gov/cash/ttx.h>
#include <us/gov/cash/map_tx.h>
#include <us/gov/cash/file_tx.h>

#include <us/wallet/engine/peer_t.h>
#include <us/wallet/engine/daemon_t.h>

#include "traders_t.h"

#define loglevel "wallet/trader"
#define logclass "conman"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::conman;

constexpr const char* c::statestr[c::num_states];

#include "olog_t.inc"

template<typename... Args>
void c::olog(const Args&... args) const {
    ::ologx(*this, args...);
}

c::conman(engine::daemon_t& daemon): daemon(daemon), t(bind(&c::run, this), bind(&c::onwakeup, this)) {
    log("constructor");
}

c::~conman() {
    log("destructor", "WPX48hf", this);
}

c::state_t c::initiate_connection() {
    using client = us::gov::socket::client;
    assert(state == state_req_online);
    state_t st = state_req_online;
    if (remote_ip.first == 0) {
        update_ip();
    }
    if (is_ko(ko_ip_resolution)) {
        assert(remote_ip.first == 0);
        st = state_connect_failed;
        log("returning ko-ip-resolution ko", ko_ip_resolution, "new state", statestr[st]);
        return st;
    }
    assert(ko_ip_resolution == ok);
    assert(remote_ip.first != 0);
    //try with the current ip (or the renewed one in case it was 0)
    olog("Connecting to remote trader", client::endpoint(remote_ip));
    function pre_attach = [&](us::gov::peer::peer_t* p) {
            log("pre-attach");
            assert(p != nullptr);
            olog("temp state transition", statestr[st], "->", statestr[state_connected]);
            st = state_connected;
        };
    function pre_connect = [&](us::gov::peer::peer_t* p) {
            log("pre-connect");
            static_cast<peer_t*>(p)->post_auth = [&, p](uint16_t rpport) {
                    log("post-auth");
                    peer_t& o = static_cast<peer_t&>(*p);
                    log("calling online", "TRACE 8c");
                    online(o);
                    log("announcing rpport", rpport);
                    o.announce(rpport);
                };
        };
    log("grid_connect", "TRACE 8c");
    daemon.grid_connect(remote_ip, pre_connect, pre_attach);
    if (st != state_connected) {
        olog("Renewing IP");
        update_ip();
        if (is_ok(ko_ip_resolution)) {
            daemon.grid_connect(remote_ip, pre_connect, pre_attach);
            if (st != state_connected) {
                olog(ko_ip_resolution, us::gov::socket::client::endpoint(remote_ip));
                st = state_connect_failed;
            }
        }
    }
    log("returning ko-ip-resolution", ko_ip_resolution == ok ? "ok" : ko_ip_resolution, "; new state", statestr[st]);
    return st;
}

pair<bool, c::state_t> c::connection_supervisor() {
    bool shch = state_changed.exchange(false);
    log("connection_supervisor", "state", statestr[state], "shch", shch);
    unique_lock<mutex> lock(mx);
    if (state == state_offline) {
        log("state is offline");
        assert(cli == nullptr);
        return make_pair(shch, state);
    }
    if (state == state_req_online) {
        log("Connecting. Retries left", tries_req_online);
        if (cli != nullptr) {
            olog("state transition", statestr[state], "->", statestr[state_connected]);
            state = state_connected;
            shch = true;
            log("push_data needed", 1);
        }
        else {
            lock.unlock();
            log("initiating connection");
            auto st = initiate_connection();
            lock.lock();
            olog("state transition", statestr[state], "->", statestr[st]);
            state = st;
            shch = true;
            log("push_data needed", 2);
        }
    }
    if (state == state_req_offline) {
        log("state is state_req_offline");
        if (cli != nullptr) {
            cli->disconnect(0, "KO 01001 req_offline");
        }
        else {
            olog("state transition", statestr[state], "->", statestr[state_gone_offline]);
            state = state_gone_offline;
            shch = true;
            log("push_data needed", 4);
        }
    }
    if (state == state_gone_offline) {
        log("state is gone_offline");
        if (cli != nullptr) {
            --cli->sendref;
            cli = nullptr;
        }
        else {
            log("client died between state_connected and state_online");
        }
        olog("state transition", statestr[state], "->", statestr[state_offline]);
        state = state_offline;
        shch = true;
        log("push_data needed", 5);
    }
    return make_pair(shch, state);
}

void c::schedule_exec(string cmd) {
    log("schedule_exec", cmd);
    us::gov::io::cfg0::trim(cmd);
    {
        lock_guard<mutex> lock(mx_cmdq);
        cmdq.push(cmd);
    }
    log("queued", cmd);
    task_wakeup();
}

/*
void c::on_start() {
    log("on_start");
}
*/

void c::run() {
    #if CFG_LOGS == 1
        log_start(logdir, logfile);
    #endif
    log("starting trade worker thread", statestr[state]);
/*
    assert(state == state_off);
    {
        unique_lock<mutex> lock(mx);
        //state = state_offline;
    }
    state_changed.store(false);
*/
    olog("Personal trader started. Initial state:", statestr[state]);
    ++busyref;
//    on_start();
    while (t::isup()) {
        auto rcs = connection_supervisor();
        log("connection_supervisor returned", rcs.first, statestr[rcs.second]);
        state_t cur_st = rcs.second;
        string cmd;
        bool exec_cmd = false;
        {
            lock_guard<mutex> lock(mx_cmdq);
            if (rcs.first) {
                log("connection state changed, exec show data");
                if (cur_st != state_req_offline && cur_st != state_gone_offline) { // transitional states
                    cmdq.push("show data");
                }
            }
            if (!cmdq.empty()) {
                exec_cmd = true;
                cmd = cmdq.front();
                log("cmdq sz", cmdq.size(), "next cmd is", cmd);
            }
        }
        if (unlikely(cur_st == state_dying)) {
            log("state_dying. we've been killed.");
            break;
        }
        if (exec_cmd) {
            log("flushing command", cmd);
            istringstream is(cmd);
            string fc;
            is >> fc;
            ch_t ch(0);
            ko r;
            if (requires_online(fc)) {
                log("cmd requires online", "cur_st", statestr[cur_st]);
                if (cur_st != state_connect_failed) {
                    if (cur_st != state_online) {
                        if (cur_st == state_offline || cur_st == state_req_offline) { // || cur_st == state_off) {
                            log("turn online");
                            lock_guard<mutex> lock(mx);
                            state = state_req_online;
                        }
                        else {
                            log("connected but not yet online. Wait for 300ms");
                            this_thread::sleep_for(300ms); //avoid fast loop during connected -> online transition
                        }
                        continue;
                    }
                    assert(cur_st == state_online);
                    assert(cli != nullptr);
                    log("calling exec_online", cmd);
                    r = exec_online(*cli, cmd, ch);
                }
                else {
                    {
                        lock_guard<mutex> lock(mx);
                        state = state_offline;
                    }
                    r = "KO 60943 Connection failed.";
                    log(r, "cur_st", statestr[cur_st], "cmd", cmd);
                }
            }
            else {
                log("calling exec_offline", cmd);
                r = exec_offline(cmd, ch);
            }
            if (is_ko(r)) {
                on_KO(r, cmd);
            }
            else {
                update_peer(*cli, move(ch));
            }
            {
                lock_guard<mutex> lock(mx_cmdq);
                cmdq.pop();
            }
            continue;
        }
        wait();
    }
    bool killed = state == state_dying;
    log("dying");
    {
        if (cli != nullptr) {
            log("disconnecting trader from client");
            cli->disconnect(0, "KO 00100 killed.");
            --cli->sendref;
            log("clinull");
            cli = nullptr;
        }
    }
    --busyref;
    olog("Trader died on state ", statestr[state]);
    if (killed) {
        log("self-destruction");
        dispose();
    }
    state = state_off;
}

ko c::start() {
    log("starting conman", statestr[state]);
    assert(state == state_off);
    return t::start();
}

void c::join() {
    t::join();
    log("wait_busyref");
    wait_busyref();
}

void c::wait() {
    unique_lock<mutex> lock(mx);
    if (t::reset_wait()) {
        return;
    }
    using namespace chrono_literals;
    log("waiting");
    cv.wait(lock, [&]{ return t::resumed(); });
    t::reset_wait();
    log("resumed");
}

void c::onwakeup() {
    log("onwakeup");
    cv.notify_all();
}

void c::die(const string& rson) {
    log("die");
    if (reason.empty()) {
        reason = rson;
    }
    else {
        reason = reason + '\n' + rson;
    }
    set_state(state_dying);
}

void c::wait_busyref() {
    int i = 0;
    while (busyref.load() > 0) {
        log("Waiting for trader to finish");
        ++i;
        if (i < 20) { //2 secs
            this_thread::sleep_for(100ms);
        }
        else {
            log("KO 30029 Trader looks in deadlock!.");
            cerr << "Trader looks in deadlock!." << endl;
            this_thread::sleep_for(5s);
        }
        if (i > 27) { //2 secs (20*100ms) + 25 secs (5*5s)
            log("KO 66988 Killing trade with busyref > 0");
            break;
        }
    }
}

bool c::set_state(state_t s) {
    lock_guard<mutex> lock(mx);
    return set_state_(s);
}

bool c::set_state_(state_t s) {
    if (state == s) {
        return false;
    }
    if (s == state_req_online) {
        if (state == state_online || state == state_connected) {
            log("already offline or connecting");
            return false;
        }
    }
    if (s == state_req_offline) {
        log("req_offline");
        if (state == state_offline || state == state_gone_offline) {
            log("already offline", statestr[state]);
            return false;
        }
    }
    if (s == state_gone_offline) {
        if (state == state_offline) {
            log("already offline");
            return false;
        }
    }
    olog("state transition", statestr[state], "->", statestr[s], "(set_state)");
    state = s;
    log("set state_changed", state_changed.load(), " -> 1");
    state_changed.store(true);
    task_wakeup();
    return true;
}

void c::update_ip() {
    using client = us::gov::socket::client;
    log("update_ip");
    ko& r = ko_ip_resolution;
    if (remote_endpoint.pkh.is_zero()) {
        remote_ip.first = 0;
        remote_ip.second = 0;
        r = "KO 32101 Invalid endpoint.";
        olog(r);
        return;
    }
    if (local_endpoint().pkh != remote_endpoint.pkh) {
        log("different wallet hosts");
        auto ip4 = remote_endpoint.decode_ip4();
        if (ip4.first == 0 || ip4.second == 0) {
            log("Not an encoded IP4, looking up remote wallet ip address from pkh", remote_endpoint.pkh);
            pair<ko, pair<uint32_t, uint16_t>> ans = resolve_ip_address(remote_endpoint.pkh);
            if (is_ok(ans.first)) {
                if (ans.second.first != 0) {
                    if (ans.second != remote_ip) {
                        remote_ip = ans.second;
                    }
                    else {
                        log("ip/port didn't change.", client::endpoint(ans.second));
                    }
                    r = ok;
                }
                else {
                    r = "KO 48862 Resolved to invalid IP address.";
                    log(r, client::endpoint(ans.second));
                }
            }
            else {
                r = ans.first;
            }
        }
        else {
            log("IP4 found encoded in the endpoint.", client::endpoint(ip4));
            remote_ip = ip4;
            r = ok;
        }
    }
    else {
        remote_ip = make_pair(client::ip4_encode("127.0.0.1"), static_cast<us::gov::socket::multipeer::server&>(daemon).port);
        olog("The two wallets live on the same host. Resolved to", client::endpoint(remote_ip));
    }
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
    string age(chrono::system_clock::time_point time) {
        using namespace std::chrono;
        return duration_str(duration_cast<milliseconds>(system_clock::now() - time));
    }
}

void c::show_data(const string& lang, ostream& os) const {
    lock_guard<mutex> lock(mx);
    show_data_(lang, os);
}

void c::show_data_(const string& lang, ostream& os) const {
    os << "state " << statestr[state] << '\n';
    os << "logfile " << ologfile << '\n';

    //assert(state != state_gone_offline);
    if (state == state_online) {
        assert(cli != nullptr);
        os << "online_age " << age(cli->since) << '\n';
    }
    if (!reason.empty()) {
        os << "reason " << reason << '\n';
    }
    os << "remote_endpoint "; remote_endpoint.to_streamX(os); os << '\n';
    os << "remote_ip4 " << us::gov::socket::client::endpoint(remote_ip) << '\n';
    if (is_ko(ko_ip_resolution)) {
        os << "ip4_resolver " << ko_ip_resolution << '\n';
    }
}

void c::offline() { //the client is going to the gc
    log("offline", "WPX48hf", this);
    set_state(state_gone_offline);
}

void c::online(peer_t& peer) {
    log("online");
    lock_guard<mutex> lock(mx);
    reason.clear();
    log("cliassign");
    cli = &peer;
    cli->shutdown = [&]() {
            log("shutdown");
            offline();
        };
    ++cli->sendref; //prevent gc from deleting this object
    set_state_(state_online);
}

c::state_t c::get_stateX() const {
    lock_guard<mutex> lock(mx);
    return state;
}

void c::dump(const string& pfx, ostream& os) const {
    lock_guard<mutex> lock(mx);
    os << "state " << state << '\n';
}

