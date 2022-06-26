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
package com.jstyle.blesdk2025.model;

/**
 * Created by Administrator on 2018/1/16.
 */

public class Notifier extends SendData{
    public static final  int Data_Tel=0;
    public static final  int Data_Sms=1;
    public static final  int Data_WeChat=2;
    public static final  int Data_Facebook=3;
    public static final  int Data_Instagram=4;
    public static final  int Data_Skype=5;
    public static final  int Data_Telegram=6;
    public static final  int Data_Twitter=7;
    public static final  int Data_Vkclient=8;
    public static final  int Data_WhatApp=9;
    public static final  int Data_QQ=10;
    public static final  int Data_IN=11;
    public static final  int Data_Stop_Tel=0xff;

    int type;
    String info;

    public int getType() {
        return type;
    }

    public void setType(int type) {
        this.type = type;
    }

    public String getInfo() {
        return info;
    }

    public void setInfo(String info) {
        this.info = info;
    }

    @Override
    public String toString() {
        return "Notifier{" +
                "type=" + type +
                ", info='" + info + '\'' +
                '}';
    }
}
