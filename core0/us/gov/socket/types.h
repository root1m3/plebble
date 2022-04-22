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

#ifdef DEBUG
   #include <type_traits>
   struct assert_is_root_namespace;
   static_assert(std::is_same<assert_is_root_namespace, ::assert_is_root_namespace>::value, "Not root namespace. Check includes.");
#endif

namespace us {

    using namespace std;

    using shost_t = string;
    using host_t = uint32_t;
    using port_t = uint16_t;
    using shostport_t = pair<shost_t, port_t>;
    using hostport_t = pair<host_t, port_t>;
    using channel_t = uint16_t;
    using svc_t = uint16_t;
    using seq_t = uint16_t;
    using reason_t = string;
    using ts_t = uint64_t; //nanosec
    using ts_ms_t = uint64_t; //millisec
    using sock_t = int;

}

namespace us { namespace gov { namespace socket {

    using namespace us;

}}}

