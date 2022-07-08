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
package us;
import java.io.PrintStream;                                                                    // PrintStream

public class vcs {

//#include <us/vcs_git_java>
//------------------------------------------------------------__begin__------generated by configure, do not edit.
//content of file: <us/vcs_git_java>
public static final String devjob = "";
public static final String devjobtag = "";
public static final String brand = "us";
public static final String branch = "alpha-37";
public static final String codehash = "8cd81d816a4eaf5a933dbc94627acbe4381fcbd2+";
public static final String cfghash = "1f3c072ee802978ef8aee6abf3155e227059a0743daf96556389c7aacfe53188";
public static final String hashname = "8cd81d+.1f3c07";
public static final String version_name = "us-alpha-37_8cd81d+.1f3c07";
public static final String build_date = "2022-07-06_11-55-23";
//-/----------------------------------------------------------___end___------generated by configure, do not edit.

    public static void version(PrintStream os) {
        os.print(version_name + ' ' + codehash + ' ' + build_date);
    }

    public static String version() {
        return version_name + ' ' + codehash + ' ' + build_date;
    }

    public static String name_date() {
        return version_name + ' ' + build_date;
    }
}

