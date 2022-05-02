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
#include <us/gov/crypto/types.h>
#include <us/gov/crypto/ec.h>
#include <us/gov/engine/evidence.h>
/*
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <deque>
#include <fcntl.h>

#include <us/gov/io/cfg0.h>
#include <us/gov/bgtask.h>
#include <us/gov/config.h>
#include <us/gov/engine/daemon_t.h>
#include <us/gov/engine/net_daemon_t.h>
#include <us/gov/engine/peer_t.h>
#include <us/gov/relay/protocol.h>
#include <us/gov/engine/evidence.h>
#include <us/gov/engine/protocol.h>
#include <us/gov/crypto/ripemd160.h>

#include <us/wallet/wallet/local_api.h>
#include <us/wallet/cli/hmi.h>

#include "../test_platform.h"
*/

namespace us { namespace test {

    struct okev: us::gov::engine::evidence {
        using b = us::gov::engine::evidence;

        okev();
        ~okev() override;
        string name() const override;
        void hash_data_to_sign(us::gov::crypto::ec::sigmsg_hasher_t& h) const override;
        void hash_data(hasher_t& h) const override;
        void write_pretty_en(ostream& os) const override;
        void write_pretty_es(ostream& os) const override;
        size_t blob_size() const override;
        void to_blob(blob_writer_t& writer) const override;
        ko from_blob(blob_reader_t& reader) override;

        vector<uint8_t> payload;
        uint16_t src_port;
    };

}}


