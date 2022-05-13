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
#include <inttypes.h>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <cassert>
#include <thread>
#include <chrono>
#include <ctime>
#include <condition_variable>
#include <mutex>

#include <us/gov/config.h>
#include <us/gov/bgtask.h>
#include <us/gov/crypto/ripemd160.h>
#include <us/gov/relay/pushman.h>
#include <us/gov/dfs/fileattr_t.h>

#include "peer_t.h"
#include "app.h"
#include "policies.h"
#include "sysops_t.h"
#include "calendar.h"
#include "net_daemon_t.h"
#include "syncd.h"
#include "evidence.h"
#include "evidence_processor.h"
#include "counters_t.h"
#include "neuralyzer_t.h"
#include "votes_t.h"
#include "pools_t.h"
#include "types.h"
#include "track_status_t.h"
#include "auth/app.h"

namespace us::gov::io {

    struct cfg;

}

namespace us::gov::engine {

    struct daemon_t;
    struct local_deltas_t;
    struct diff;
    struct db_t;
    struct evidence;

    struct mezzanine: bgtask {
        using b = bgtask;
        mezzanine(daemon_t*);
        daemon_t* d;
    };

    struct daemon_t: syncd_t::actuator, mezzanine, evidence_processor {
        using t = mezzanine;
        using seconds = std::chrono::seconds;
        using fileattr_t = dfs::fileattr_t;

        #if CFG_COUNTERS == 1
            static counters_t counters;
        #endif

        struct reg final {
            void register_arrival(uint64_t);
            void reset();

        public:
            atomic<uint64_t> sum{0};
            atomic<uint32_t> num{0};
            atomic<uint32_t> max{0};
        };

        struct evt_data_t {
            evt_data_t() {}
/*
            void reset(evt_status_t ist, const hash_t& src) {
                st = ist;
                if (src.is_not_zero()) srcs.emplace(src);
            }

            void reset(evt_status_t ist, const string& iinfo, const hash_t& src) {
                st = ist;
                info = iinfo;
                if (src.is_not_zero()) srcs.emplace(src);
            }
*/
            evt_data_t(evt_status_t st): st(st) {}
            evt_data_t(evt_status_t st, const string& info): st(st), info(info) {}
            evt_data_t(const evt_data_t& other): st(other.st), info(other.info) {}

            track_status_t get_status(ts_t) const;

        public:
            evt_status_t st;
            string info;
        };

        struct ev_track_t final: unordered_map<ts_t, evt_data_t> {
            using evt_ret_t = pair<pair<ts_t, ts_t>, evt_data_t>;
            using status_t = track_status_t;

            void track(calendar_t&, ts_t, track_status_t&);
            status_t set_errorx(ts_t, const string& err);
            status_t set_errorx(ts_t from, ts_t to, const string& err);
            status_t set_statusx(ts_t, evt_status_t st);
            status_t set_statusx(ts_t from, ts_t to, evt_status_t st);

        public:
            void dump(ostream&) const;
            ts_t mints{0};
            mutex mx;
        };

        static void fsinfo(const string& home, bool dot, ostream&);

        daemon_t();
        daemon_t(channel_t, const keys_t&);
        daemon_t(channel_t, const keys_t&, const string& home, port_t, pport_t pport, uint8_t num_edges, uint8_t devices, int workers, const vector<hostport_t>& seed_nodes, const string& status_file);
        daemon_t(const daemon_t&) = delete;
        daemon_t(daemon_t&&) = delete;
        ~daemon_t() override;

        daemon_t& operator = (const daemon_t&) = delete;

        void constructor();

    public:
        int clean_orphaned_blocks();
        void list_apps(const string& prefix, ostream&) const;
        void eat_diff(const hash_t& voted_tip, diff*);
        virtual ko process_evidence2(evidence*);
        bool snapshot(hash_t&);
        void print_map(ostream&) const;
        bool wait_for_secs_to(const seconds&, ts_t block_deadline);
        void fsinfo(bool dot, ostream&) const;
        void dbg_output() const;
        void vote_tip(const diff& b);
        void dump(const string& prefix, ostream&) const;
        bool import(const diff& b, const hash_t&);
        bool import_(const diff& b, const hash_t&);
        bool ready_for_consensus() const;
        hash_t get_last_delta_imported() const;
        void set_last_delta_imported(const hash_t&, ts_t id);
        void set_last_delta_imported_(const hash_t&, ts_t id);

    public:
        ko load_db(const string& file);
        ko load_db(const blob_t&);
        hash_t save_db();
        ko load_dbX(const hash_t&);

    public:
        void logline(ostream&) const;
        void list_files(ostream&) const;
        int clean_unlisted_files();
        bool get_prev_block(const hash_t&, hash_t& prev) const override;
        bool patch_db(vector<hash_t>& patches) override;
        void on_sync() override;
        void clear() override;
        int call_query_block(const hash_t& hash) override;
        void run();
        void load_head();
        void wait(const seconds&) const;
        local_deltas_t* create_local_deltas(ts_t id);
        void update_peers_state();
        void clear_();
        ko shell_command(int app_id, istream&, ostream&);
        void save(const diff&) const;
        #if CFG_COUNTERS == 1
            void print_performances(ostream&) const;
            void save_dashboard() const;
            void write_status(ostream&) const;
        #endif
        void files_to_keep(vector<pair<hash_t, uint32_t>>&) const;
        void update_dfs_index();
        void update_dfs_index_delta();
        void on_block_closure(ts_t block_closure) override;
        bool ready_for_block_opening(ts_t) const override;
        static string get_blocksdir(const string& govhome);
        static string get_fsrootdir(const string& govhome);
        string uptime() const;

        ko start();
        ko wait_ready(const time_point& deadline) const;
        ko wait_ready(int secs_deadline) const;
        void stop();
        void join();
        void onwakeup();
        void watch(ostream& os) const;
        void write_status(const string& line) const;
        void exec_evidence(const evidence& e) override;
        static pair<ko, io::cfg*> init_chain(channel_t, const string& govhome, const string& addrport);
        void report_random_node();
        unsigned int rng_seed() const;
        void new_evidence(evidence*);
        void relay_evidence(const datagram&, const socket::client* exclude);
        virtual pair<ko, evidence*> parse_evidence(const blob_t&) const;
        void set_tx_error(ts_t from, ts_t to, const string& err);
        void set_tx_status(ts_t from, ts_t to, evt_status_t st);
        void set_tx_error(ts_t, const string& err);
        void set_tx_status(ts_t, evt_status_t st);
        void replace_db(const hash_t&, db_t*);

    public:
        hash_t last_vote{0};
        hash_t last_consensus{0};
        condition_variable cv_cyc;
        string home;
        keys_t id;
        time_point running_since;
        net_daemon_t peerd;
        sysops_t sysops;
        bool sysop_allowed{false};
        bool encrypt_protocol{false};
        local_deltas_t* _local_deltas{nullptr};
        votes_t votes;

        #if CFG_COUNTERS == 1
            struct silent_t: set<hash_t> {
                void dump(ostream&) const;
                mutable mutex mx;
            };
            silent_t silent;
        #endif

        pools_t pools;
        syncd_t syncd;
        neuralyzer_t neuralyzer;
        relay::pushman pm;
        relay::pushman::filter_t device_filter;

        db_t* db;

        hash_t last_delta_imported;
        mutable mutex mx_import;
        uint16_t depth{0};
        string blocksdir;
        reg votes_timing;
        reg local_deltas_timing;
        string status_file;
        ev_track_t evt;
        mutable mutex mx_dashboard;

        #if CFG_LOGS == 1
            string logdir;
        #endif
    };

}

