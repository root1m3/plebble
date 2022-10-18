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

#ifndef CFG_CHANNEL
    error, #include <us/gov/config.h> before this file.
#endif
#include <string>
#include <iostream>

#include <us/gov/log/thread_logger.h>

template<typename type1>
void scrout(const type1& arg1) {
    std::cout << arg1 << '\n';
}

template<typename type1, typename... types>
void scrout(const type1& arg1, const types& ... args) {
    std::cout << arg1 << ' ';
    scrout(args...);
}

#if CFG_LOGS == 1
    template<typename... F>
    static void log_(const F&... s) {
        if (us::dbg::thread_logger::instance.disabled) return;
        us::dbg::thread_logger::instance.log(loglevel, s...);
    }

    template<typename O>
    static void logdump(const O& o) {
        if (us::dbg::thread_logger::instance.disabled) return;
        us::dbg::thread_logger::instance.logdump(loglevel, logclass, o);
    }

    template<typename O>
    static void logdump(const std::string& prefix, const O& o) {
        if (us::dbg::thread_logger::instance.disabled) return;
        us::dbg::thread_logger::instance.logdump(loglevel, logclass, prefix, o);
    }

    static void log_start(const std::string& dir, const std::string& n) {
        assert(!n.empty());
        if (us::dbg::thread_logger::instance.disabled) return;
        us::dbg::thread_logger::instance.start(dir, n);
    }

    static void log_pstart(const std::string& n) {
        assert(!n.empty());
        if (us::dbg::thread_logger::instance.disabled) return;
        auto i = n.find_last_of('/');
        if (i == std::string::npos) {
            us::dbg::thread_logger::instance.pstart(n);
        }
        else {
            us::dbg::thread_logger::instance.pstart(n.substr(i + 1));
        }
    }

    static void log_stacktrace() {
        if (us::dbg::thread_logger::instance.disabled) return;
        us::dbg::thread_logger::instance.log_stacktrace();
    }

    static void log_info(std::ostream&os) {
        us::dbg::thread_logger::instance.log_info(os);
    }

    static const std::string& log_dir() {
        return us::dbg::thread_logger::instance.get_logdir();
    }

    #define log(...) log_(__FILE__, __LINE__, __VA_ARGS__)

    template<typename t>
    void tee(const t& s) {
        log(s);
        std::cout << s << '\n';
    }

    template<typename type1, typename... types>
    void tee(const type1& arg1, const types& ... args) {
        log(arg1, args...);
        scrout(arg1, args...);
        std::cout << '\n';
    }

#else

    #define log (void)sizeof
    #define logdump (void)sizeof
    #define log_start (void)sizeof
    #define log_pstart (void)sizeof
    #define log_stacktrace (void)sizeof
    #define log_info (void)sizeof
    #define log_dir (void)sizeof

    template<typename t>
    void tee(const t& s) {
        std::cout << s << '\n';
    }

    template<typename type1, typename... types>
    void tee(const type1& arg1, const types& ... args) {
        scrout(arg1, args...);
        std::cout << '\n';
    }

#endif

