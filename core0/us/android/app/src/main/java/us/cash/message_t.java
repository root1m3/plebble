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
package us.cash;
import java.util.Date;                                                                         // Date

class message_t {
    private String message;
    private int type;
    private Date sent_ts;

    message_t(String message, int type, Date sent_ts) {
        this.message = message;
        this.type = type;
        this.sent_ts = sent_ts;
    }

    boolean is_sent() {
        return type == 0;
    }

    Date get_time() {
        return sent_ts;
    }

    String get() {
        return message;
    }
}
