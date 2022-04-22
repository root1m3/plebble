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
#include "app.h"

#include <thread>
#include <fstream>
#include <chrono>

#include <us/gov/config.h>
#include <us/gov/crypto/base58.h>
#include <us/gov/engine/net_daemon_t.h>
#include <us/gov/likely.h>
#include <us/gov/vcs.h>
#include <us/gov/engine/daemon_t.h>
#include <us/gov/vcs.h>
#include <us/gov/engine/shell.h>
#include <us/gov/vcs.h>

#include "local_delta.h"
#include "delta.h"
#include "install_script.h"
#include "install_script_response.h"
#include "types.h"

#define loglevel "gov/sys"
#define logclass "app"
#include <us/gov/logs.inc>

using namespace us::gov::sys;
using c = us::gov::sys::app;

constexpr const char* c::name;
pubkey_t c::automatic_updates_subscription(AUTOMATIC_UPDATES_PUBKEY);

c::app(engine::daemon_t& d): b(d) {
}

c::~app() {
}

engine::app::local_delta* c::create_local_delta() {
    auto ld = new sys::local_delta();
    ld->id = demon.id.pub.hash();
    return ld;
}

pair<ko, engine::evidence*> c::create_evidence(eid_t evid) {
    switch(evid) {
        case install_script::eid: return  make_pair(ok, new install_script());
        case install_script_response::eid: return  make_pair(ok, new install_script_response());
    }
    auto r = "KO 65092 Invalid evidence id in sys app.";
    log(r);
    return make_pair(r, nullptr);
}

void c::import(const engine::app::delta& gg, const engine::pow_t& w) {
    auto& g = static_cast<const sys::delta&>(gg);
    lock_guard<mutex> lock(db.mx);
    db.sensors = static_cast<const sys::sensors_t&>(g.m);
}

void c::send_response(const string& response, const hash_t& job) {
    log("send_response");
    auto e = new sys::install_script_response();
    auto maxsize = socket::datagram::maxsize * 3 / 4 - 100;
    e->ref = job.to_b58();
    if (response.size() > maxsize) {
        e->response = response.substr(response.size() - maxsize, maxsize);
    }
    else {
        e->response = response;
    }
    e->sign(demon.id.priv);
    log("response evidence", "job", job, "ts", e->ts);
    demon.new_evidence(e);
}

void c::maintenance(int code, const hash_t& jobid) {
    #if CFG_LOGS == 1
    {
        ostringstream os;
        os << "job_" << jobid;
        log_start("patch_os", os.str());
    }
    #endif
    log("maintenance", "code", code == 0 ? "govshell script" : "bash script", "job", jobid);
    ostringstream os;
    os << "Running version ";
    us::vcs::version(os);
    os << '\n';
    ostringstream inpf;
    inpf << "/tmp/us_sysop_script_" << jobid;
    if (code == 0) { //govshell
        ifstream is(inpf.str());
        string govshell;
        is >> govshell;
        if (govshell == "govshell") {
           engine::shell sh(demon);
           sh.command(is, os);
           send_response(os.str(), jobid);
        }
    }
    else if (code == 1) {
        ostringstream file;
        file << "/tmp/us_root_" << jobid;
        ostringstream cmd;
        cmd << "chmod +x " << inpf.str() << "; sudo " << inpf.str() << " >" << file.str() << " 2>&1";
        log("executing", cmd.str());
        int status = system(cmd.str().c_str());
        if (status != 0) {
            log("systemcall failed", status);
            ofstream os(file.str(), ios::app);
            os << "\nERROR: system call failed with err code " << status << '\n';
        }
        {
            ifstream is(file.str());
            while(is.good()) {
                string line;
                getline(is, line);
                os << line << '\n';
            }
        }
        send_response(os.str(), jobid);
        remove(file.str().c_str());
    }
    remove(inpf.str().c_str());
    log("end");
}

bool c::process(const evidence& e) {
    if (e.eid == 0) {
        return process(static_cast<const install_script&>(e));
    }
    else if (e.eid == 1) {
        return process(static_cast<const install_script_response&>(e));
    }
    return false;
}

bool c::process(const install_script_response& t) {
    log("process", "install_script_response", t.ts);
    ostringstream filename;
    filename << "/tmp/groot_log_" << t.ref;
    static mutex mx;
    lock_guard<mutex> lock(mx);
    ofstream os(filename.str(), ios::app);
    os << "========== pkh " << t.pubkey.hash() << '\n';
    t.write_pretty_en(os);
    return true;
}

void c::dump(const string& prefix, int detail, ostream& os) const {
    db.dump(prefix, os);
}

bool c::process(const install_script& t) {
    log("process", "install_script", t.ts);
    if (t.pubkey != automatic_updates_subscription) {
        log("KO 30299 install_script: invalid access.");
        return false;
    }
    ostream nullos(0);
    if (!t.verify(nullos)) {
        log("KO 33023 Invalid access. Failed verification.");
        return false;
    }
    if (t.script.empty()) {
        log("KO 20199 Empty script.");
        return false;
    }
    int code = 1;
    {
        stringstream is(t.script);
        string govshell;
        is >> govshell;
        if (govshell == "govshell") {
            log("script is govshell");
            code = 0;
        }
    }
    {
        ostringstream inpf;
        inpf << "/tmp/us_sysop_script_" << t.hash();
        ofstream os(inpf.str());
        os << t.script << '\n';
    }
    thread work(&c::maintenance, this, code, t.hash());
    work.detach();
    return true;
}

ko c::shell_command(istream& is, ostream& os) {
    while(is.good()) {
        string cmd;
        is >> cmd;
        if (cmd.empty()) {
            os << "sys shell. type h for help.\n";
            return ok;
        }
        if (cmd == "h" || cmd == "help") {
            os << "sys shell.\n";
            os << "h|help              Shows this help.\n";
            os << "db        Dumps db\n";
            os << "exit                Exits this app and returns to parent shell.\n";
            os << '\n';
            continue;
        }
        if (cmd == "db") {
            db.dump("", os);
            continue;
        }
        if (cmd == "exit") {
            return ok;
        }
        return "KO 40399 Unrecognized command";
    }
    return ok;
}

void c::clear() {
    db.clear();
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(db);
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(db);
}

ko c::from_blob(blob_reader_t& reader) {
    clear();
    {
        auto r = reader.read(db);
        if (is_ko(r)) return r;
    }
    return ok;
}

