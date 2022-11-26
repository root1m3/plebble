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

#include <us/gov/io/seriable.h>

#include "apps_t.h"
#include "types.h"

namespace us::gov::engine::auth {
    struct app;
}

namespace us::gov::cash {
    struct app;
}

namespace us::gov::sys {
    struct app;
}

namespace us::gov::traders {
    struct app;
}

namespace us::gov::engine {

    struct daemon_t;
    struct app;

    struct db_t final: virtual io::seriable {

        static constexpr int num_apps{4};
        db_t(daemon_t&);

    private:
        void add(app*);

    public:
        static constexpr serid_t serid{'S'};

        serid_t serial_id() const override { return serid; }

        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

        void dump(int detail, ostream& os) const;

    public:
        apps_t apps;
        auth::app* auth_app{nullptr};
        sys::app* sys_app{nullptr};
        cash::app* cash_app{nullptr};
        traders::app* traders_app{nullptr};
        ts_t last_delta_imported_id{0};

    };

}

/*
namespace us::gov::io {
    template<> inline ko blob_reader_t::readD(const us::gov::socket::datagram& d, us::gov::engine::db_t& o) { return o.read(d); }
    template<> inline datagram* blob_writer_t::get_datagram(channel_t channel, svc_t svc, seq_t seq, const us::gov::engine::db_t& o) { return o.get_datagram(channel, svc, seq); }
}
*/
