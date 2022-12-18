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

public class fragment_fan2club_club extends role_fragment  {

    static void log(final String line) {                       //--strip
        CFG.log_android("fragment_fan2club_club: " + line);    //--strip
    }                                                          //--strip

    public fragment_fan2club_club() {
        under_construction = true;
    }

    @Override public String init_cards() {
        log("init_cards"); //--strip
        String cards = "";
        return cards;
    }

}

