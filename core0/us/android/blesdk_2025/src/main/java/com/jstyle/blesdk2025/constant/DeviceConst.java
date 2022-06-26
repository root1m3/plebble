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
package com.jstyle.blesdk2025.constant;

/**
 * Created by Administrator on 2018/1/17.
 */

public class DeviceConst {
    public static final byte CMD_SET_TIME = (byte)0x01;
    public static final byte CMD_GET_TIME = (byte)0x41;
    public static final byte CMD_Set_UseInfo = (byte)0x02;
    public static final byte CMD_GET_USERINFO =  (byte) 0x42;
    public static final byte CMD_Set_DeviceInfo = (byte)0x03;
    public static final byte CMD_Get_DeviceInfo = (byte)0x04;
    public static final byte CMD_Set_DeviceID = (byte)0x05;
    public static final byte CMD_Enable_Activity = (byte)0x09;
    public static final byte CMD_Set_Goal = (byte)0x0b;
    public static final byte CMD_Get_Goal = (byte)0x4b;
    public static final byte CMD_Get_BatteryLevel = (byte)0x13;
    public static final byte CMD_Get_Address = (byte)0x22;
    public static final byte CMD_Get_Version = (byte)0x27;
    public static final byte CMD_Reset = (byte)0x12;
    public static final byte CMD_Mcu_Reset = (byte)0x2e;
    public static final byte CMD_Set_MOT_SIGN = (byte)0x36;
    public static final byte CMD_Set_Name = (byte)0x3d;
    public static final byte CMD_Get_Name = (byte)0x3e;
    public static final byte CMD_Set_Auto = (byte)0x2a;
    public static final byte CMD_Get_Auto = (byte)0x2b;
    public static final byte CMD_Set_Clock = (byte)0x23;
    public static final byte CMD_Notify = (byte)0x4d;
    public static final byte CMD_Set_ActivityAlarm = (byte)0x25;
    public static final byte CMD_Get_ActivityAlarm = (byte)0x26;
    public static final byte CMD_Get_TotalData = (byte)0x51;
    public static final byte CMD_Get_DetailData = (byte)0x52;
    public static final byte CMD_Get_SleepData = (byte)0x53;
    public static final byte CMD_Get_HeartData = (byte)0x54;
    public static final byte CMD_Get_Blood_oxygen = (byte)0x60;
    public static final byte CMD_Get_OnceHeartData = (byte)0x55;
    public static final byte CMD_Get_Clock = (byte)0x57;
    public static final byte CMD_Get_SPORTData = (byte)0x5C;
    public static final byte CMD_Get_HrvTestData = (byte)0x56;
    public static final byte CMD_SET_SOCIAL = (byte)0x64;
    public static final byte CMD_Start_EXERCISE = (byte)0x19;
    public static final byte CMD_HeartPackageFromDevice = (byte)0x18;//
    public static final byte CMD_Start_Ota = (byte)0x47;
    public static final byte CMD_Set_TemperatureCorrection = (byte)0x38;
    public static final byte CMD_Set_HeartbeatPackets = (byte)0x06;
    public static final byte Enter_photo_mode = (byte)0x20;
    public static final byte Enter_photo_modeback = (byte)0x16;
    public static final byte Exit_photo_mode = (byte)0x10;
    public static final byte Openecg = (byte)0x99;
    public static final byte Closeecg = (byte)0x98;
    public static final byte CMD_ECGQuality = (byte) 0x83;
    public static final byte GetEcgPpgStatus = (byte) 0x9c;
    public static final byte CMD_ECGDATA = (byte) 0xAA;
    public static final byte CMD_PPGGDATA = (byte) 0xAB;
    public static final byte Weather = (byte) 0x15;
    public static final byte Braceletdial = (byte) 0x24;
    public static final byte SportMode = (byte) 0x14;
    public static final byte GetSportMode = (byte) 0x44;
    public static final byte CMD_Set_WorkOutReminder = (byte)0x28;
    public static final byte CMD_Get_WorkOutReminder = (byte)0x29;
    public static final byte ecgandppg= (byte)0x9E;
   /* public static final byte ReadSerialNumber= (byte)0x62;*/
    public static final byte LanguageSwitching= (byte)0x29;
    public static final byte CMD_heart_package= (byte)0x17;
    public static final byte GPSControlCommand= (byte)0x34;
    public static final byte CMD_Get_GPSDATA = (byte)0x5A;
    public static final byte Clear_Bracelet_data = (byte)0x61;
    public static final byte Sos = (byte)0xFE;
    public static final byte Temperature_history = (byte)0x62;
    public static final byte GetAxillaryTemperatureDataWithMode = (byte)0x65;
    public static final byte Get3D = (byte)0x49;
    public static final byte PPG = (byte)0x39;
    public static final byte GetECGwaveform = (byte)0x71;
    public static final byte GetAutomaticSpo2Monitoring = (byte)0x66;
    public static final byte spo2= (byte)0x29;
    public static final byte MeasurementWithType= (byte)0x28;
    public static final byte Qrcode = (byte)0xB0;
}
