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

public class ko {
    public static ko ok = null;

    public ko(String _msg) {
        msg = _msg;
    }

    public static boolean is_ok(ko err) {
        return err == ok;
    }

    public static boolean is_ko(ko err) {
        return err != ok;
    }

    public static String as_string(ko err) {
        return err == ok ? "ok" : err.msg;
    }

    public static String to_string(ko err) { return as_string(err); }

    public String msg;

};


