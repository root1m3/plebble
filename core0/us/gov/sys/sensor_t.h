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
#include <us/gov/types.h>
#include <us/gov/io/seriable.h>

namespace us::gov::sys {

    struct sensor_t: virtual io::seriable {

        sensor_t();
        virtual ~sensor_t() {}

        void dump(const string& prefix, ostream& os) const;
        void hash_data_to_sign(sigmsg_hasher_t&) const;
        void hash_data(hasher_t&) const;

        struct iodata {
            struct data {
                uint32_t num{0};
                size_t bytes{0};
            };

            data sent;
            data received;
        };

        struct io_t: map<uint16_t, iodata> {
        };

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
/*
        uint8_t num_cores{0};
        uint32_t bogomips;
        float load_avg_15;
        uint32_t swap_free;
        uint32_t ram_free;
        uint32_t disk_free;
        io_t io;
        float govd_pcpu;
        float govd_pmem;
        float walletd_pcpu;
        float walletd_pmem;
        uint16_t max_edges;
        bool in_sync;
        vector<pair<uint32_t,uint16_t>> edges;
*/
    };

}

