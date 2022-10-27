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
package us.gov.id;
import static us.stdint.*;                                                                     // *
import us.ko;                                                                                  // ko
import us.pair;                                                                                // pair

public class types {

    public static class pport_t extends uint16_t {

        public pport_t() { super(); }
        public pport_t(int v) { super(v); }

    }

    public static class pin_t extends uint16_t {

        public pin_t() {}
        public pin_t(int v) { super(v); }

    }

    public static class version_fingerprint_t extends uint8_t {

        public version_fingerprint_t() { super(); }
        public version_fingerprint_t(int v) { super((short)v); }

    }


}

