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

#include <us/wallet/trader/trader_protocol.h>
#include "workflows_t.h"

namespace us { namespace wallet { namespace trader { namespace workflow {

        using namespace std;
        struct item_t;
        struct doc0_t;

        struct trader_protocol: us::wallet::trader::trader_protocol {
            using b = us::wallet::trader::trader_protocol;
            using workflow_item_t = workflow::item_t;
            using workflow_doc0_t = workflow::doc0_t;

            trader_protocol(business_t&);
            ~trader_protocol() override;

            ko rehome(ch_t&) override;
            bool requires_online(const string& cmd) const override;
            void help_online(const string& indent, ostream&) const override;
            void help_onoffline(const string& indent, ostream&) const override;
            void help_show(const string& indent, ostream&) const override;
            void on_file_updated(const string& path, const string& name, ch_t&) override;
            ko exec_offline(const string& cmd0, ch_t&) override;
            ko exec_online(peer_t&, const string& cmd0, ch_t&) override;
            bool on_signal(int sig, ostream&) override;
            ko trading_msg(peer_t&, svc_t svc, blob_t&&) override;
            blob_t push_payload(uint16_t pc) const override;
            ko rehome_workflows(ch_t& ch);
            virtual ko on_receive(peer_t&, item_t&, doc0_t*, ch_t&);
            virtual ko workflow_item_requested(item_t&, peer_t&, ch_t&);
            bool sig_reset(ostream&);
            bool sig_hard_reset(ostream&);
            void sig_reload(ostream&);

            virtual void on_send_item(const string& what) {}
            enum push_code_t { //communications node-HMI
                push_begin = b::push_end,

                push_workflow_item = push_begin, // 203,
                push_doc, // 204,
                push_redirects,

                push_end, //push 205 end
                push_r2r_begin = b::push_r2r_begin
            };

            enum service_t { //communications node-node
                svc_begin = b::svc_end,

                svc_workflow_item_request = svc_begin,
                svc_workflow_item,
                svc_redirects_request,
                svc_redirects,

                svc_end,
                svc_r2r_begin = b::svc_r2r_begin
            };

            template<typename t>
            t* add(t* workflow, ch_t& ch) {
                _workflows.add(workflow, ch);
                return workflow;
            }

            void data(const string& lang, ostream&) const override;
            virtual void judge(const string& lang); //requires lock mx_user_state
            uint32_t trade_state_() const;

            workflows_t _workflows;
            bookmarks_t redirects;
            mutable mutex redirects_mx;

            mutable mutex mx_user_state;
            pair<uint32_t, string> _trade_state{0, "idle / available"};
            string _user_hint{"Let's have a chat."};
        };

}}}}

