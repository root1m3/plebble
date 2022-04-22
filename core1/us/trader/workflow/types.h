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
#include <type_traits>
#ifdef DEBUG
   struct assert_is_root_namespace;
   static_assert(std::is_same<assert_is_root_namespace, ::assert_is_root_namespace>::value, "Not root namespace. Check includes.");
#endif
#include <us/gov/crypto/types.h>
#include <us/gov/types.h>

namespace us {

    using namespace std;

}

namespace us { namespace trader { namespace workflow {

    using namespace us;

}}}

