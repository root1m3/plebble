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
#include <string>
#include <us/gov/config.h>
#include "shell_args.h"

namespace us::gov::io {

    using namespace std;

    struct params {
        params();
        params(const shell_args&);

        enum output_mode {
            om_human,
            om_text,
            om_xml,
            om_json,

            num_modes
        };
        constexpr static array<const char*, num_modes> omstr = {"human", "text", "xml", "json"};

        string get_output_mode() const { return omstr[om]; }
        string get_home_gov() const;
        string get_home_gov_rpc_client() const;
        string get_home() const;
        string get_status_file() const;
        void dump(ostream&) const;
        void dump(const string& prefix, ostream&) const;
        void connecting_to(ostream&) const;

    public:
        channel_t channel{CFG_CHANNEL};
        string homedir;
        port_t port{CFG_PORT};
        pport_t pport{CFG_PPORT};
        uint8_t edges{12};
        uint8_t devices{4};
        uint8_t workers;
        bool daemon{false};
        bool shell{false};
        string sysophost{"127.0.0.1"};
        bool dot{false};
        bool force_seeds{false};
        string cmd;
        shell_args args;
        bool verbose{false};
        bool status_file{false};
        bool rpc__connect_for_recv{true};
        bool rpc__stop_on_disconnection{true};
        output_mode om{om_human};
        bool names{false};
        bool banner{true};
        string logd{LOGDIR};
        bool save_evidences{false};
        uint16_t rpc_timeout_secs{CFG_DGRAM_ROUNDTRIP_TIMEOUT_SECS};
    };

}

