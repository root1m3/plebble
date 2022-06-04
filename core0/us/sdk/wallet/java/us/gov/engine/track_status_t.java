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
package us.gov.engine;

public class track_status_t  {

    public enum evt_status_t {
        evt_unknown,
        evt_error,
        evt_wait_arrival,
        evt_calendar,
        evt_mempool,
        evt_craftblock,
        evt_consensus,
        evt_settled,
        evt_untracked,

        evt_num;

        public int as_short() {
            return (short)ordinal();
        }

        public static evt_status_t from_short(short i) {
            return evt_status_t.values()[i];
        }
    };

    public static String[] evt_status_str = {"unknown", "error", "wait_arrival", "calendar", "mempool", "craftblock", "consensus", "settled", "untracked"};

}

