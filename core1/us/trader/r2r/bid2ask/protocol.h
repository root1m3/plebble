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
#include <vector>

#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/workflow/trader_protocol.h>
#include <us/wallet/wallet/local_api.h>

#include <us/trader/workflow/consumer/basket.h>
#include <us/trader/workflow/consumer/workflow_t.h>
#include <us/trader/workflow/consumer/docs.h>
#include <us/trader/workflow/consumer/basket.h>
#include <us/trader/workflow/logistics/docs.h>

#include "types.h"

namespace us::trader::r2r::bid2ask {

    struct protocol: us::wallet::trader::workflow::trader_protocol {
        using b = us::wallet::trader::workflow::trader_protocol;
        using catalogue_t = us::trader::workflow::consumer::catalogue_t;
        using invoice_t = us::trader::workflow::consumer::invoice_t;
        using receipt_t = us::trader::workflow::consumer::receipt_t;
        using parcel_t = us::trader::workflow::logistics::parcel_t;
        using shipping_receipt_t = us::trader::workflow::logistics::shipping_receipt_t;
        using trader_t = us::wallet::trader::trader_t;
        using basket_t = us::trader::workflow::consumer::basket_t;

        struct workflow_t: us::trader::workflow::consumer::workflow_t {
            using b = us::trader::workflow::consumer::workflow_t;

            workflow_t(trader_t&, ch_t&);
            ~workflow_t() override;
            inline trader_t& trader() const override { return tder; }

            bitem* cat{nullptr};
            bitem* inv{nullptr};
            bitem* pay{nullptr};
            bitem* rcpt{nullptr};

            trader_t& tder;
        };

        protocol(b::business_t&);
        ~protocol() override;

        enum push_code_t { //communications node-HMI
            push_begin = trader_protocol::push_r2r_begin, //push 300
            push_basket = push_begin,  //push 300
            push_catalogue, // //push 301 products without availability
            push_stock, // //push 302 products with availability

            push_end  //push 303 end
        };

        enum service_t { //communications node-node
            svc_begin = trader_protocol::svc_r2r_begin, //svn 300
            svc_catalogue_query = svc_begin, //svc 300
            svc_unused1,  //svc 301 -old catalogue
            svc_invoice_query,  //svc 302
            svc_unused2,  //svc 303 -old invoice
            svc_relay_tx,  //svc 304
            svc_select_product,  //svc 305
            svc_select_product_ans,  //svc 306

            svc_end  //svc 307 end
        };

        struct shipping_option_t {
        };

        struct collect_option_t: shipping_option_t {
        };

        struct send_option_t: shipping_option_t {
        };

        struct shipping_options_t: vector<shipping_option_t*> {
        };

        shipping_receipt_t* ship(const shipping_options_t&, const parcel_t&);

        virtual void post_configure(ch_t&);
        ko on_attach(trader_t&, ch_t&) override;
        void help_show(const string& prefix, ostream&) const override;
        ko exec_offline(const string& cmd, ch_t&) override;
        void data(const string& lang, ostream&) const override;
        ko rehome(ch_t&) override;
        blob_t push_payload(uint16_t pc) const override;
        static ko exec_help(const string& prefix, ostream&);
        static ko exec(istream&, ostream&);

    public:
        basket_t basket;
        workflow_t* _workflow{nullptr};
    };

}

