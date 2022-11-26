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
#include "types.h"

namespace {
//    using namespace us::gov::engine;
    using namespace us::gov::io;

    template<typename t>
    ko read_datagram(const datagram& d, t& o) {
        static_assert(!std::is_convertible<t*, readable*>::value, "KO 77888 template specialization for readable should have been instantiated instead.");
/*
        if constexpr (std::is_convertible<t*, readable*>::value) {
            #pragma message("## using readable object")
        //typedef typename t::something_made_up X;
            return o.read(d);
        }
        #pragma message("## using blob_reader")
*/

        blob_reader_t reader(d);
        return reader.read(o);
    }
//    template<> ko read_datagram(const datagram& d, readable& o) { return o.read(d); }
    template<> ko read_datagram(const datagram&, blob_t&);

    datagram* write_datagram(channel_t channel, svc_t svc, seq_t seq) {
        return new datagram(channel, svc, seq, 0);
    }

    template<typename t>
    datagram* write_datagram(channel_t channel, svc_t svc, seq_t seq, const t& o) {
        static_assert(!std::is_convertible<t*, writable*>::value, "KO 77897 template specialization for writable should have been instantiated instead.");
        //static_assert(!std::is_convertible<t*, seriable*>::value, "KO 77897 template specialization for writable should have been instantiated instead.");
        //static_assert(!std::is_convertible<t*, blob_t*>::value, "KO 77898 template specialization for blob_b should have been instantiated instead.");
/*
        if constexpr (std::is_convertible<t*, writable*>::value) {
            #pragma message("## using writable object")
        //typedef typename t::something_made_up X;
            return o.get_datagram(channel, svc, seq);
        }
        #pragma message("## using blob_writer " )
*/  
      auto d = new datagram(channel, svc, seq, blob_writer_t::blob_size(o));
        blob_writer_t w(*d);
        w.write(o);
        return d;
    }
//    template<> datagram* write_datagram(channel_t channel, svc_t svc, seq_t seq, const writable& o) {  }
    template<> datagram* write_datagram(channel_t, svc_t, seq_t, const blob_t&);
    
}

namespace {

    using namespace us::gov::io;
    template<> ko read_datagram(const datagram& d, blob_t& o) {
        log("readD blob");
        o.resize(d.size() - datagram::h);
        memcpy(o.data(), d.data() + datagram::h, d.size() - datagram::h);
        return ok;
    }

    template<> datagram* write_datagram(channel_t channel, svc_t svc, seq_t seq, const blob_t& blob) {
        log("get_datagram from blob", svc, seq, blob.size());
        auto d = new datagram(channel, svc, seq, blob.size());
        memcpy(d->data() + datagram::h, blob.data(), blob.size());
        return d;
    }

}

