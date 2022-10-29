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
#pragma once
#include <us/gov/config.h>
#include <us/gov/socket/busyled_t.h>
#include <us/gov/io/shell_args.h>
#include <us/gov/io/screen.h>
#include <us/gov/io/params.h>
#include <us/gov/io/cfg.h>
#include <us/gov/io/cfg1.h>
#include <us/gov/config.h>
#include <us/gov/socket/datagram.h>

#include "hmi0.h"
#include "rpc_daemon_t.h"
#include "rpc_peer_t.h"

namespace us::gov::engine {
    struct daemon_t;
}

namespace us::gov::cli {

    struct printer_t;

    struct hmi: us::cli::hmi {
        using params = us::gov::io::params;
        using shell_args = us::gov::io::shell_args;
        using cfg_daemon = us::gov::io::cfg_daemon;
        using cfg1 = us::gov::io::cfg1;
        using screen = us::gov::io::screen;
        using datagram = us::gov::socket::datagram;
        using busyled_t = us::gov::socket::busyled_t;

        static const char* KO_10100, *KO_91809, *KO_11000;

        struct dispatcher_t: us::gov::socket::datagram::dispatcher_t {
            dispatcher_t(hmi&);
            bool dispatch(us::gov::socket::datagram* d);
            hmi& m;
        };

    public:
        hmi(ostream&);
        hmi(int argc, char** argv, ostream&);
        hmi(const shell_args&, ostream&);
        hmi(const params&, ostream&);
        void constructor();

        ~hmi() override;

    public:
        static string parse_options(shell_args& args, params&);
        static void help(const params& p, ostream&);
        void write_rpc_client_key();
        void dump_diff(const string& diff_file) const;
        void dump_db(const string& snapshot_block, int detail) const;
        void print_id() const;
        virtual void setup_signals(bool on);
        void interactive_shell();
        bool on_datagram(datagram*);
        ko start();
        ko start(datagram::dispatcher_t*);
        ko start(busyled_t::handler_t* send, busyled_t::handler_t* recv, datagram::dispatcher_t*);
        void stop();
        void join();
        static void banner(const params&, ostream&);
        ko init_chain(const string& addr);
        ko exec(const string& cmd);
        ko exec_offline(const string& cmd);
        ko exec_online(const string& cmd);
        ko exec_online1(const string& cmd);
        pair<ko, string> exec_cmd(const string& cmd);
        string run();
        string run_();
        void set_mute(bool n);
        datagram::dispatcher_t* create_dispatcher();
        virtual pair<ko, us::gov::io::cfg1*> load_cfg(const string& home, bool gen);
        virtual void on_stop();

        static void process_cleanup();
        void import_v5(const string& snapshot_block) const;
    public:
        ko start_daemon(busyled_t::handler_t* send, busyled_t::handler_t* recv, datagram::dispatcher_t*);
        gov::engine::daemon_t* daemon{nullptr};

    public:
        ko start_rpc_daemon(busyled_t::handler_t* send, busyled_t::handler_t* recv, datagram::dispatcher_t*);
        rpc_daemon_t* rpc_daemon{nullptr};
        rpc_peer_t* rpc_peer{nullptr};

    public:
        virtual void upgrade_software();

    public:
        virtual void verification_completed(bool verif_ok);
        virtual void on_I_disconnected(const string& reason);
        virtual void on_peer_disconnected(const string& reason);
        virtual void on_connect(ko error);
        virtual string rewrite(ko r) const;

    public:
        bool interactive{false};
        printer_t* printer{nullptr};
        cfg_daemon* cfg{nullptr};
        cfg1* cfgcli{nullptr};
        params p;
        mutable screen scr;
        string home;

    public:
        #if CFG_LOGS == 1
            string logdir;
        #endif
    };

}

