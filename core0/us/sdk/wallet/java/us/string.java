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
package us;
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok

/// mutable String class (can be passed byref)
public class string extends blob_reader_t.readable {

    public string() {
    }

    public string(String _value) {
        value = _value;
    }

    @Override public final ko from_blob(blob_reader_t reader) {
        pair<ko, String> r = reader.read_string();
        if (ko.is_ko(r.first)) {
            return r.first;
        }
        value = r.second;
        return ok;
    }

    public String value;
}

