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
#include <unordered_map>
#include <string>
#include <cassert>
#include <thread>
#include <mutex>
#include <vector>
#include <fstream>
#include <chrono>
#include <sstream>
#include <sys/types.h>
#include <atomic>
#include <unistd.h>
#include <iostream>

namespace us::dbg {

    using namespace std;
    using namespace chrono;

    struct thread_logger;

    struct task final {
        task(thread_logger&);
        ~task();

        void start(const string& dir, const string& name);

        template<typename type1>
        void dolog(const type1& arg1) {
            *logfile << arg1 << ' ';
            if (teecout) std::cout << arg1 << ' ';
        }

        template<typename type1>
        void dolog(const vector<type1>& arg1) {
            *logfile << "[sz=" << arg1.size() << "] ";
            if (teecout) std::cout << "[sz=" << arg1.size() << "] ";
        }

        template<typename type1, typename... types>
        void dolog(const type1& arg1, const types& ... args) {
            dolog(arg1);
            dolog(args...);
        }

        static void prefix(ostream&, const thread_logger&, const string& log_level_, const string& log_class_);

        template<typename... Args>
        void log(const string& log_level, const string& log_class, const Args&... args) {
            if (onduty) return; //operator << can trigger recursive log calls which we ignore
            if (logfile == nullptr) start("", "unknown");
            onduty = true;
            prefix(*logfile, logger, log_level, log_class);
            if (teecout) prefix(std::cout, logger, log_level, log_class);
            dolog(args...);
            onduty = false;
            *logfile << endl;
            if (teecout) std::cout << endl;
        }

        template<typename O>
        void logdump(const string& log_level, const string& log_class, const O& o) {
            if (onduty) return;
            ostringstream pfx;
            prefix(pfx, logger, log_level, log_class);
            o.dump(pfx.str(), *logfile);
            if (teecout) {
                o.dump(pfx.str(), std::cout);
            }
        }

        template<typename O>
        void logdump(const string& log_level, const string& log_class, const string& logpfx, const O& o) {
            if (onduty) return;
            ostringstream pfx;
            prefix(pfx, logger, log_level, log_class);
            string spfx = pfx.str() + logpfx;
            o.dump(spfx, *logfile);
            if (teecout) {
                o.dump(spfx, std::cout);
            }
        }

        void log_stacktrace() const;

        bool onduty{false};
        static bool verbose; //false (def): logs go to file; true: logs go to cout
        ostream* logfile{nullptr};
        bool ownlogfile{true};
        const thread_logger& logger;
        static string pname;
        static bool teecout;
        string logdir;
        short g{0};
    };

    template<>
    void task::dolog(const vector<uint8_t>& arg1);

}

