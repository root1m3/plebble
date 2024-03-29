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
public static final String src = "https://github.com/user/project";
public static final String devjob = "";
public static final String devjobtag = "";
public static final String brand = "us";
public static final String branch = "alpha-55";
public static final String codehash = "8ed560640bbac160f2c4ff07c09b527b0b5eb827";
public static final String cfghash = "7615d6d92447bc5e0b76b4adc471d883955af60796eeae5d5b12b48a6ba981b1";
public static final String hashname = "8ed560.7615d6";
public static final String version_name = "us-alpha-55_8ed560.7615d6";
public static final String build_date = "2023-01-30_12-19-51";
//-/----------------------------------------------------------___end___------generated by configure, do not edit.

    public static String apkfilename() {
        return brand + "-wallet_android_" + branch + '_' + hashname + ".apk";
    }

    public static String sdk_name() {
         return brand + "-wallet-sdk_java_" + branch;
    }

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

