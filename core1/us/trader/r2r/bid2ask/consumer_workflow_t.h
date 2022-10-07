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
#include <us/wallet/trader/workflow/workflows_t.h>
#include <us/trader/workflow/consumer/workflow_t.h>

#include "types.h"

namespace us::trader::r2r::bid2ask {

    struct consumer_workflow_t final: us::trader::workflow::consumer::workflow_t {
        using b = us::trader::workflow::consumer::workflow_t;
        using workflows_t = us::wallet::trader::workflow::workflows_t;

    public:
        consumer_workflow_t();
        ~consumer_workflow_t() override;

    public:
        void init_bid(ch_t&);
        void init_ask(ch_t&);

    private:
        void init2(ch_t&);

    public:
        static constexpr factory_id_t my_factory_id{1};
        inline factory_id_t factory_id() const override { return my_factory_id; }; //final seriable class

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        bitem* cat{nullptr};
        bitem* inv{nullptr};
        bitem* pay{nullptr};
        bitem* rcpt{nullptr};
    };

}

