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
#pragma once
#include <string>
#include <us/gov/config.h>
#include <us/gov/io/shell_args.h>
#include "types.h"

namespace us{ namespace wallet { namespace cli {

    struct params {
        using shell_args = us::gov::io::shell_args;
        params();
        params(const shell_args&);

        void constructor();

        enum output_mode {
            om_human,
            om_text,
            om_xml,
            om_json,

            num_modes
        };

        constexpr static array<const char*, num_modes> omstr = {"human", "text", "xml", "json"};

        string get_output_mode() const { return omstr[om]; }
        string get_home_wallet() const;
        string get_home_wallet_rpc_client() const;
        string get_home_gov() const;
        string get_home() const;
        void dump(ostream&) const;
        void dump(const string& prefix, ostream&) const;
        void connecting_to(ostream&) const;

    public:
        channel_t channel{CFG_CHANNEL};
        string homedir;
        bool daemon{false};
        bool advanced{true};
        bool ncurses{false};
        bool verbose{false};
        uint8_t max_trade_edges{8};
        uint8_t max_devices{4};
        uint8_t workers;
        string subhome;
        string cmd;
        shell_args args;
        port_t listening_port{CFG_WALLET_PORT};
        port_t published_port{CFG_WALLET_PPORT};
        pin_t pin{0};
        string walletd_host{"localhost"};
        port_t walletd_port{CFG_WALLET_PORT};
        string backend_host{"localhost"};
        port_t backend_port{CFG_PORT};
        bool rpc__connect_for_recv{true};
        bool rpc__stop_on_disconnection{true};
        output_mode om{om_human};
        bool names{true};
        string downloads_dir;
        #if CFG_FCGI == 1
            bool fcgi{false};
        #endif
        bool banner{true};
        string logd{LOGDIR};
        bool nolog{false};
    };

}}}

