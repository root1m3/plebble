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
 * Created by Administrator on 2018/4/9.
 */

public class MyDeviceInfo extends SendData{

    boolean DistanceUnit;//1 mile（true）,0km(false)
    boolean is12Hour;//1（12小时）（true），0（24小时制）(false)
    boolean Bright_screen;//1 turn on the bright screen, 0 turn off the bright screen
    boolean Fahrenheit_or_centigrade;//Fahrenheit or centigrade
    boolean isHorizontalScreen;//1 Horizontal screen, 0 Vertical screen
    boolean Night_mode;//夜间模式
    boolean Temperature_unit;//夜间模式
    boolean Social_distance_switch;//社交距离开关 Social distance switch
    boolean Chinese_English_switch;//中英文切换 Chinese_English_switch
    int baseheart=60;//Basic heart rate
    int ScreenBrightness=-1;//Screen brightness  屏幕亮度0-5
    int Dialinterface=-1;//表盘更换 Dial replacement 0-10

    public boolean isTemperature_unit() {
        return Temperature_unit;
    }

    public void setTemperature_unit(boolean temperature_unit) {
        Temperature_unit = temperature_unit;
    }

    public boolean isChinese_English_switch() {
        return Chinese_English_switch;
    }

    public void setChinese_English_switch(boolean chinese_English_switch) {
        Chinese_English_switch = chinese_English_switch;
    }

    public boolean isSocial_distance_switch() {
        return Social_distance_switch;
    }

    public void setSocial_distance_switch(boolean social_distance_switch) {
        Social_distance_switch = social_distance_switch;
    }

    public boolean isNight_mode() {
        return Night_mode;
    }

    public void setNight_mode(boolean night_mode) {
        Night_mode = night_mode;
    }

    public boolean isDistanceUnit() {
        return DistanceUnit;
    }

    public void setDistanceUnit(boolean distanceUnit) {
        DistanceUnit = distanceUnit;
    }

    public boolean isIs12Hour() {
        return is12Hour;
    }

    public void setIs12Hour(boolean is12Hour) {
        this.is12Hour = is12Hour;
    }

    public boolean isBright_screen() {
        return Bright_screen;
    }

    public void setBright_screen(boolean bright_screen) {
        Bright_screen = bright_screen;
    }

    public boolean isFahrenheit_or_centigrade() {
        return Fahrenheit_or_centigrade;
    }

    public void setFahrenheit_or_centigrade(boolean fahrenheit_or_centigrade) {
        Fahrenheit_or_centigrade = fahrenheit_or_centigrade;
    }

    public boolean isHorizontalScreen() {
        return isHorizontalScreen;
    }

    public void setHorizontalScreen(boolean horizontalScreen) {
        isHorizontalScreen = horizontalScreen;
    }

    public int getBaseheart() {
        return baseheart;
    }

    public void setBaseheart(int baseheart) {
        this.baseheart = baseheart;
    }

    public int getScreenBrightness() {
        return ScreenBrightness;
    }

    public void setScreenBrightness(int screenBrightness) {
        ScreenBrightness = screenBrightness;
    }

    public int getDialinterface() {
        return Dialinterface;
    }

    public void setDialinterface(int dialinterface) {
        Dialinterface = dialinterface;
    }
}
