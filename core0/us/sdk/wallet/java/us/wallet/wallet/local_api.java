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
package us.wallet.wallet;

public class local_api {

    public static final int push_begin = 50;
    public static final int push_txlog = push_begin + 0;
    public static final int push_end = push_begin + 1;

    public enum wallet_track_status_t {
        wts_unknown,
        wts_error,
        wts_wait_rcpt_info,
        wts_wait_signature,
        wts_cancelled,
        wts_delivered,

        wts_num;

        public short as_short() {
            return (short)ordinal();
        }

        public static wallet_track_status_t from_short(int i) {
            return wallet_track_status_t.values()[i];
        }
    };

    public static String[] wallet_track_status_str = {"unknown", "error", "wait_rcpt_info", "wait_signature", "cancelled", "delivered"};

}

