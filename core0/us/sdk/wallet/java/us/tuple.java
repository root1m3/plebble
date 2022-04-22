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

public class tuple {

    public static class tuple3<t0, t1, t2> {

        public t0 item0;
        public t1 item1;
        public t2 item2;

        public tuple3() {
        }

        public tuple3(t0 i0, t1 i1, t2 i2) {
            item0 = i0;
            item1 = i1;
            item2 = i2;
        }

    }

    public static class tuple4<t0, t1, t2, t3> {
        public t0 item0;
        public t1 item1;
        public t2 item2;
        public t3 item3;

        public tuple4() {
        }

        public tuple4(t0 i0, t1 i1, t2 i2, t3 i3) {
            item0 = i0;
            item1 = i1;
            item2 = i2;
            item3 = i3;
        }
    }

    public static class tuple5<t0, t1, t2, t3, t4> {
        public t0 item0;
        public t1 item1;
        public t2 item2;
        public t3 item3;
        public t4 item4;

        public tuple5() {
        }

        public tuple5(t0 i0, t1 i1, t2 i2, t3 i3, t4 i4) {
            item0 = i0;
            item1 = i1;
            item2 = i2;
            item3 = i3;
            item4 = i4;
        }
    }

}

