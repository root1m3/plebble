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
package us.wallet.trader;

public class trader_t {

        public static final int push_begin = 100;

        public static final int push_log = push_begin + 0;
        public static final int push_data = push_begin + 1;
        public static final int push_peer_personality = push_begin + 2;
        public static final int push_roles_mine = push_begin + 3;
        public static final int push_roles_peer = push_begin + 4;
        public static final int push_roles = push_begin + 5;
        public static final int push_qr_mine = push_begin + 6;
        public static final int push_qr_peer = push_begin + 7;
        public static final int push_trade = push_begin + 8;
        public static final int push_killed = push_begin + 9;
        public static final int push_chat = push_begin + 10;
        public static final int push_bookmarks = push_begin + 11;
        public static final int push_help = push_begin + 12;
        public static final int push_local_functions = push_begin + 13;
        public static final int push_remote_functions = push_begin + 14;

        public static final int push_end = push_begin + 15;

}

