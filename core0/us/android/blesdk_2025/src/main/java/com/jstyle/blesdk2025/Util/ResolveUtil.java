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
package com.jstyle.blesdk2025.Util;


import android.util.Log;

import com.jstyle.blesdk2025.constant.BleConst;
import com.jstyle.blesdk2025.constant.DeviceKey;
import com.jstyle.blesdk2025.model.DeviceBean;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TimeZone;

import static com.jstyle.blesdk2025.Util.BleSDK.read;

/**
 * Created by Administrator on 2018/1/17.
 */

public class ResolveUtil {
    /**
     * crc校验
     *
     * @param value
     */
    public static void crcValue(byte[] value) {
        byte crc = 0;
        for (int i = 0; i < value.length - 1; i++) {
            crc += value[i];
        }
        value[value.length - 1] = (byte) (crc & 0xff);
    }

    public static byte getTimeValue(int value) {
        String data = value + "";
        Integer m = Integer.parseInt(data, 16);
        return (byte) m.intValue();
    }


    public static String bcd2String(byte bytes) {
        StringBuffer temp = new StringBuffer();
        temp.append((byte) ((bytes & 0xf0) >>> 4));
        temp.append((byte) (bytes & 0x0f));
        return temp.toString();
    }

    /**
     * 设备时间
     *
     * @param value
     * @return
     */
    public static  Map<String,Object> getDeviceTime(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetDeviceTime);
        maps.put(DeviceKey.End,true);

        Map<String,String>mapData=new HashMap<>();
        String date = "20"+ByteToHexString(value[1]) + "-"
                + ByteToHexString(value[2]) + "-" + ByteToHexString(value[3]) + " "
                + ByteToHexString(value[4]) + ":" + ByteToHexString(value[5]) + ":" + ByteToHexString(value[6]);
        String gpsDate = ByteToHexString(value[9]) + "."
                + ByteToHexString(value[10]) + "." + ByteToHexString(value[11]) ;
        mapData.put(DeviceKey.DeviceTime, date);
        mapData.put(DeviceKey.GPSTime, gpsDate);
        maps.put(DeviceKey.Data, mapData);
        return maps;
    }
    public static String getGpsTime(byte[] value) {

        String gpsDate = ByteToHexString(value[9]) + "."
                + ByteToHexString(value[10]) + "." + ByteToHexString(value[11]) ;

        return gpsDate;
    }

    /**
     * 用户信息
     *
     * @param value
     * @return
     */
    public static Map<String,Object> getUserInfo(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetPersonalInfo);
        maps.put(DeviceKey.End,true);
        Map<String,String>mapData=new HashMap<>();
        maps.put(DeviceKey.Data, mapData);
        String[] userInfo = new String[6];
        for (int i = 0; i < 5; i++) {
            userInfo[i] = String.valueOf(getValue(value[i + 1], 0));
        }
        String deviceId = "";
        for (int i = 6; i < 12; i++) {
            if (value[i] == 0)
                continue;
            deviceId += (char) getValue(value[i], 0);
        }
        userInfo[5] = deviceId;
        mapData.put(DeviceKey.Gender, userInfo[0]);
        mapData.put(DeviceKey.Age, userInfo[1]);
        mapData.put(DeviceKey.Height, userInfo[2]);
        mapData.put(DeviceKey.Weight, userInfo[3]);
        mapData.put(DeviceKey.Stride, userInfo[4]);
        mapData.put(DeviceKey.KUserDeviceId, userInfo[5]);
        return maps;
    }

    public static Map<String,Object> GetAutomaticSpo2Monitoring(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetAutomaticSpo2Monitoring);
        maps.put(DeviceKey.End,false);
        List<Map<String, String>> list = new ArrayList<>();
        maps.put(DeviceKey.Data, list);
        int count = 10;
        int length = value.length;
        int size = length / count;
        if (size == 0) {
            maps.put(DeviceKey.End,true);
            return maps;
        }
        for (int i = 0; i < size; i++) {
            int flag = 1 + (i + 1) * count;
            if (value[length-1] == (byte) 0xff) {
                maps.put(DeviceKey.End,true);
            }
            Map<String, String> hashMap = new HashMap<>();
            String date = "20"+ByteToHexString(value[3 + i * count]) + "."
                    + ByteToHexString(value[4 + i * count]) + "." + ByteToHexString(value[5 + i * count]) + " "
                    + ByteToHexString(value[6 + i * count]) + ":" + ByteToHexString(value[7 + i * count]) + ":" + ByteToHexString(value[8 + i * count]);
            hashMap.put(DeviceKey.Date, date);
            hashMap.put(DeviceKey.Blood_oxygen, String.valueOf(getValue(value[9 + i * count], 0)));
            list.add(hashMap);
        }

        return maps;
    }
    /**
     * 设备信息
     *
     * @param value
     * @return
     */
    public static Map<String,Object> getDeviceInfo(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetDeviceInfo);
        maps.put(DeviceKey.End,true);
        Map<String,String>mapData=new HashMap<>();
        maps.put(DeviceKey.Data, mapData);
        mapData.put(DeviceKey.DistanceUnit, String.valueOf(getValue(value[ 1], 0)));
        mapData.put(DeviceKey.TimeUnit, String.valueOf(getValue(value[ 2], 0)));
        mapData.put(DeviceKey.WristOn,  String.valueOf(getValue(value[ 3], 0)));
        mapData.put(DeviceKey.TempUnit, String.valueOf(getValue(value[ 4], 0)));
        mapData.put(DeviceKey.NightMode, String.valueOf(getValue(value[ 5], 0)));
        mapData.put(DeviceKey.KBaseHeart, String.valueOf(getValue(value[ 9], 0)));
        mapData.put(DeviceKey.ScreenBrightness, String.valueOf(getValue(value[ 11], 0)));
        mapData.put(DeviceKey.Dialinterface, String.valueOf(getValue(value[12], 0)));
       // mapData.put(DeviceKey.SocialDistancedwitch, String.valueOf(getValue(value[13], 0)));
        mapData.put(DeviceKey.ChineseOrEnglish, String.valueOf(getValue(value[14], 0)));
        return maps;
    }

    /**
     * 实时计步信息
     *
     * @param value
     * @return
     */
    public static Map<String,Object> getActivityData(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.RealTimeStep);
        maps.put(DeviceKey.End,true);
        Map<String,String>mapData=new HashMap<>();
        maps.put(DeviceKey.Data, mapData);
        String[] activityData = new String[6];
        int step = 0;
        float cal = 0;
        float distance = 0;
        int time = 0;
        int heart = 0;
        int exerciseTime = 0;
        for (int i = 1; i < 5; i++) {
            step += getValue(value[i], i - 1);
        }
        for (int i = 5; i < 9; i++) {
            cal += getValue(value[i], i - 5);
        }
        for (int i = 9; i < 13; i++) {
            distance += getValue(value[i], i - 9);
        }
        for (int i = 13; i < 17; i++) {
            time += getValue(value[i], i - 13);
        }
        for (int i = 17; i < 21; i++) {
            exerciseTime += getValue(value[i], i - 17);
        }
        heart = getValue(value[21], 0);
       int temp = getValue(value[22], 0)+getValue(value[23],1);
        NumberFormat numberFormat = getNumberFormat(1);
        BigDecimal bigDecimal=new BigDecimal(String.valueOf(cal/100));
        BigDecimal bigDecimaCal= bigDecimal.setScale(1, RoundingMode.HALF_DOWN);
        activityData[0] = String.valueOf(step);
        activityData[1] = bigDecimaCal.floatValue()+"";
        numberFormat.setMinimumFractionDigits(2);
        activityData[2] = numberFormat.format(distance / 100);
        activityData[3] = String.valueOf(time/60);
        activityData[4] = String.valueOf(heart);
        activityData[5] = String.valueOf(exerciseTime);
        mapData.put(DeviceKey.Step, activityData[0]);
        mapData.put(DeviceKey.Calories, activityData[1]);
        mapData.put(DeviceKey.Distance, activityData[2]);
        mapData.put(DeviceKey.ExerciseMinutes, activityData[3]);
        mapData.put(DeviceKey.HeartRate, activityData[4]);
        mapData.put(DeviceKey.ActiveMinutes, activityData[5]);
        numberFormat.setMinimumFractionDigits(1);
        mapData.put(DeviceKey.TempData,numberFormat.format(temp*0.1f));
        return maps;
    }

    public static Map<String,Object> get3d(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetMAC);
        maps.put(DeviceKey.End,true);
        List<Map<String, Object>> list = new ArrayList<>();
        maps.put(DeviceKey.Data, list);
        for(int i=0;i<value.length/8;i++) {
            Map<String, Object> hashMap = new HashMap<>();
            int valueddd = i * 6;
            int data = getValue(value[1 + valueddd], 0) + getValue(value[2 + valueddd], 1);
            int data2 =  getValue(value[3 + valueddd], 0) + getValue(value[4 + valueddd], 1);
            int data3 = getValue(value[5 + valueddd], 0) + getValue(value[6 + valueddd], 1);
            hashMap.put(DeviceKey.arrayX, data);
            hashMap.put(DeviceKey.arrayY, data2);
            hashMap.put(DeviceKey.arrayZ,  data3);
            list.add(hashMap);
        }
        return maps;
    }


    public static Map<String,Object> getPPG(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.Getppg);
        maps.put(DeviceKey.End,true);
        List<Map<String, Object>> list = new ArrayList<>();
        maps.put(DeviceKey.Data, list);
        byte[] bs = new byte[value.length-3];
        System.arraycopy(value, 3, bs, 0, bs.length);
        for(int i=0;i<bs.length/2;i++) {
            Map<String, Object> hashMap = new HashMap<>();
            int valueddd = i * 2;
            int data = getValue(value[valueddd], 0) + getValue(value[valueddd+1], 1);
            hashMap.put(DeviceKey.DataType, value[1]);
            hashMap.put(DeviceKey.arrayPpgRawData, data);
            list.add(hashMap);
        }
        return maps;
    }



    /**
     * 步数目标
     *
     * @param value
     * @return
     */
    public static Map<String,Object>  getGoal(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetStepGoal);
        maps.put(DeviceKey.End,true);
        Map<String,String>mapData=new HashMap<>();
        maps.put(DeviceKey.Data, mapData);
        int goal = 0;
        for (int i = 0; i < 4; i++) {
            goal += getValue(value[i + 1], i);
        }
        mapData.put(DeviceKey.StepGoal, String.valueOf(goal));
        return maps;
    }

    /**
     * 设备电量
     *
     * @param value
     * @return
     */
    public static Map<String,Object> getDeviceBattery(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetDeviceBatteryLevel);
        maps.put(DeviceKey.End,true);
        Map<String,String>mapData=new HashMap<>();
        maps.put(DeviceKey.Data, mapData);
        int battery = getValue(value[1], 0);
        mapData.put(DeviceKey.BatteryLevel, String.valueOf(battery));
        return maps;
    }

    /**
     * 设备mac地址
     *
     * @param value
     * @return
     */
    public static Map<String,Object> getDeviceAddress(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetDeviceMacAddress);
        maps.put(DeviceKey.End,true);
        Map<String,String>mapData=new HashMap<>();
        maps.put(DeviceKey.Data, mapData);
        StringBuilder sb = new StringBuilder();
        for (int i = 1; i < 7; i++) {
            sb.append(String.format("%02X", value[i])).append(":");
        }
        String address = sb.toString();
        mapData.put(DeviceKey.MacAddress, address.substring(0, address.lastIndexOf(":")));
        return maps;
    }

    /**
     * 设备版本号
     *
     * @param value
     * @return
     */
    public static Map<String,Object> getDeviceVersion(byte[] value) {
        //    String version = "";
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetDeviceVersion);
        maps.put(DeviceKey.End,true);
        Map<String,String>mapData=new HashMap<>();
        maps.put(DeviceKey.Data, mapData);
        StringBuffer stringBuffer = new StringBuffer();
        for (int i = 1; i < 5; i++) {
            stringBuffer.append(String.format("%X", value[i])).append(i == 4 ? "" : ".");
        }
        mapData.put(DeviceKey.DeviceVersion, stringBuffer.toString());
        return maps;
    }



    public static Map<String,Object> Reset() {
        //    String version = "";
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.CMD_Reset);
        maps.put(DeviceKey.End,true);
        return maps;
    }
    public static Map<String,Object> MCUReset() {
        //    String version = "";
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.CMD_MCUReset);
        maps.put(DeviceKey.End,true);
        return maps;
    }

    public static Map<String,Object> Notify() {
        //    String version = "";
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.Notify);
        maps.put(DeviceKey.End,true);
        return maps;
    }

    /**
     * 设备名字
     *
     * @param value
     * @return
     */
    public static Map<String,Object> getDeviceName(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetDeviceName);
        maps.put(DeviceKey.End,true);
        Map<String,String>mapData=new HashMap<>();
        maps.put(DeviceKey.Data, mapData);
        String name = "";
        for (int i = 1; i < 15; i++) {
            int charValue=getValue(value[i],0);
            if(charValue==0||charValue>127)continue;
            name += (char) charValue;
        }
        mapData.put(DeviceKey.DeviceName, name);
        return maps;
    }

    public static String getByteString(byte b) {
        byte[] array = new byte[8];
        StringBuffer stringBuffer = new StringBuffer();
        for (int i = 0; i <= 6; i++) {
            array[i] = (byte) (b & 1);
            b = (byte) (b >> 1);
            stringBuffer.append(String.valueOf(array[i])).append(i == 6 ? "" : "-");
        }
        return stringBuffer.toString();
    }

    public static String getByteArray(byte b) {
        byte[] array = new byte[8];
        StringBuffer stringBuffer = new StringBuffer();
        for (int i = 0; i <= 7; i++) {
            array[i] = (byte) (b & 1);
            b = (byte) (b >> 1);
            stringBuffer.append(String.valueOf(array[i]));
        }
        return stringBuffer.toString();
    }

    /**
     * 自动测量心率时间段
     *
     * @param value
     * @return
     */
    public static Map<String,Object> getAutoHeart(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetAutomatic);
        maps.put(DeviceKey.End,true);
        Map<String,String>mapData=new HashMap<>();
        maps.put(DeviceKey.Data, mapData);
        String[] autoHeart = new String[7];
        int enable = getValue(value[1], 0);
        String startHour = ByteToHexString(value[2]);
        String startMin = ByteToHexString(value[3]);
        String stopHour = ByteToHexString(value[4]);
        String stopMin = ByteToHexString(value[5]);
        String week = getByteString(value[6]);
        int time = getValue(value[7], 0) + getValue(value[8], 1);
        autoHeart[0] = String.valueOf(enable);
        autoHeart[1] = startHour;
        autoHeart[2] = startMin;
        autoHeart[3] = stopHour;
        autoHeart[4] = stopMin;
        autoHeart[5] = week;
        autoHeart[6] = String.valueOf(time);
        mapData.put(DeviceKey.WorkMode, autoHeart[0]);
        mapData.put(DeviceKey.StartTime, autoHeart[1]);
        mapData.put(DeviceKey.KHeartStartMinter, autoHeart[2]);
        mapData.put(DeviceKey.EndTime, autoHeart[3]);
        mapData.put(DeviceKey.KHeartEndMinter, autoHeart[4]);
        mapData.put(DeviceKey.Weeks, autoHeart[5]);
        mapData.put(DeviceKey.IntervalTime, autoHeart[6]);
        return maps;
    }



    public static Map<String,Object> getSpo2(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GEtSpo2);
        maps.put(DeviceKey.End,true);
        Map<String,String>mapData=new HashMap<>();
        maps.put(DeviceKey.Data, mapData); //29 01 01 39 01 30 01 30 7F 00 00 00 00 00 00 45
        String time = ByteToHexString(value[3]);
        String startHour = ByteToHexString(value[4]);
        String startMin = ByteToHexString(value[5]);
        String stopHour = ByteToHexString(value[6]);
        String stopMin = ByteToHexString(value[7]);
        String week = getByteString(value[8]);
        mapData.put(DeviceKey.StartTime, startHour);
        mapData.put(DeviceKey.KHeartStartMinter, startMin);
        mapData.put(DeviceKey.EndTime, stopHour);
        mapData.put(DeviceKey.KHeartEndMinter, stopMin);
        mapData.put(DeviceKey.Weeks, week);
        mapData.put(DeviceKey.IntervalTime, time);
        return maps;
    }

    /**
     * 运动提醒
     *
     * @param value
     * @return
     */
    public static Map<String,Object> getActivityAlarm(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetSedentaryReminder);
        maps.put(DeviceKey.End,true);
        Map<String,String>mapData=new HashMap<>();
        maps.put(DeviceKey.Data, mapData);
        String[] activityAlarm = new String[7];
        String startHour = ByteToHexString(value[1]);
        String startMin = ByteToHexString(value[2]);
        String stopHour = ByteToHexString(value[3]);
        String stopMin = ByteToHexString(value[4]);
        String week = getByteString(value[5]);
        int time = getValue(value[6], 0);
        int step = getValue(value[7], 0);
        activityAlarm[0] = startHour;
        activityAlarm[1] = startMin;
        activityAlarm[2] = stopHour;
        activityAlarm[3] = stopMin;
        activityAlarm[4] = week;
        activityAlarm[5] = String.valueOf(time);
        activityAlarm[6] = String.valueOf(step);
        mapData.put(DeviceKey.StartTimeHour, activityAlarm[0]);
        mapData.put(DeviceKey.StartTimeMin, activityAlarm[1]);
        mapData.put(DeviceKey.EndTimeHour, activityAlarm[2]);
        mapData.put(DeviceKey.EndTimeMin, activityAlarm[3]);
        mapData.put(DeviceKey.Week, activityAlarm[4]);
        mapData.put(DeviceKey.IntervalTime, activityAlarm[5]);
        mapData.put(DeviceKey.LeastSteps, activityAlarm[6]);
        return maps;
    }

    /**
     * 总运动数据
     *
     * @param value
     * @return
     */
    public static Map<String,Object> getTotalStepData(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetTotalActivityData);
        maps.put(DeviceKey.End,false);
        List<Map<String, String>> list = new ArrayList<>();
        maps.put(DeviceKey.Data, list);
        int count = getStepCount(value);
        int length = value.length;
        int size = length / count;
        if (size == 0) {
            maps.put(DeviceKey.End,true);
            return maps;
        }
        NumberFormat numberFormat = NumberFormat.getNumberInstance();
        numberFormat.setMinimumFractionDigits(2);
        numberFormat.setGroupingUsed(false);
        for (int i = 0; i < size; i++) {
            int flag = 1 + (i + 1) * count;
            if (flag < length && value[flag] == (byte) 0xff) {
                maps.put(DeviceKey.End,true);
            }
            Map<String, String> hashMap = new HashMap<>();
            String date ="20"+ByteToHexString(value[2 + i * count]) + "." +
                    ByteToHexString(value[3 + i * count]) + "." + ByteToHexString(value[4 + i * count]);
            int step = 0;
            int time = 0;
            float cal = 0;
            float distance = 0;

            for (int j = 0; j < 4; j++) {
                step += getValue(value[5 + j + i * count], j);
            }
            for (int j = 0; j < 4; j++) {
                time += getValue(value[9 + j + i * count], j);
            }
            for (int j = 0; j < 4; j++) {
                distance += getValue(value[13 + j + i * count], j);
            }
            for (int j = 0; j < 4; j++) {
                cal += getValue(value[17 + j + i * count], j);
            }
            int exerciseTime = 0;
            int goal = count==26?getValue(value[21 + i * count], 0):(getValue(value[21 + i * count], 0)+getValue(value[22 + i * count], 1));

            for (int j = 0; j < 4; j++) {
                exerciseTime += getValue(value[count-4 + j + i * count], j);
            }
            hashMap.put(DeviceKey.Date, date);
            hashMap.put(DeviceKey.Step, String.valueOf(step));
            hashMap.put(DeviceKey.ExerciseMinutes, String.valueOf(time));
            hashMap.put(DeviceKey.Calories, numberFormat.format(cal / 100));
            hashMap.put(DeviceKey.Distance, numberFormat.format(distance / 100));
            hashMap.put(DeviceKey.Goal, String.valueOf(goal));
            hashMap.put(DeviceKey.ActiveMinutes, String.valueOf(exerciseTime));
            list.add(hashMap);
        }

        return maps;
    }

    private static int getStepCount(byte[]value){
        int goal=27;
        int length=value.length;
        if(length!=2){
            if(length%26==0){
                goal=26;
            }else if(length%27==0){
                goal=27;
            }else{
                if((length-2)%26==0){
                    goal=26;
                }else if((length-2)%27==0){
                    goal=27;
                }
            }
        }
        return goal;
    }
    /**
     * 详细运动数据
     *
     * @param value
     * @return
     */
    public static Map<String,Object> getDetailData(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetDetailActivityData);
        maps.put(DeviceKey.End,false);
        List<Map<String, String>> list = new ArrayList<>();
        maps.put(DeviceKey.Data, list);
        int count = 25;
        int length = value.length;
        int size = length / count;
        DeviceBean deviceBean = new DeviceBean();
        if (size == 0) {
            maps.put(DeviceKey.End,true);
            return maps;
        }
        NumberFormat numberFormat = NumberFormat.getNumberInstance();
        numberFormat.setMaximumFractionDigits(2);
        numberFormat.setGroupingUsed(false);
        for (int i = 0; i < size; i++) {
            Map<String, String> hashMap = new HashMap<>();
            int flag = 1 + (i + 1) * count;
            if ( value[length-1] == (byte) 0xff) {
                maps.put(DeviceKey.End,true);
            }
            String date ="20"+ByteToHexString(value[3 + i * 25]) + "."
                    + ByteToHexString(value[4 + i * 25]) + "." + ByteToHexString(value[5 + i * 25]) + " "
                    + ByteToHexString(value[6 + i * 25]) + ":" + ByteToHexString(value[7 + i * 25]) + ":" + ByteToHexString(value[8 + i * 25]);
            int step = 0;
            float cal = 0;
            float distance = 0;
            StringBuffer stringBuffer = new StringBuffer();
            for (int j = 0; j < 2; j++) {
                step += getValue(value[9 + j + i * 25], j);
            }
            for (int j = 0; j < 2; j++) {
                cal += getValue(value[11 + j + i * 25], j);
            }
            for (int j = 0; j < 2; j++) {
                distance += getValue(value[13 + j + i * 25], j);
            }
            for (int j = 0; j < 10; j++) {
                stringBuffer.append(String.valueOf(getValue(value[15 + j + i * 25], 0))).append(j == 9 ? "" : " ");
            }
            hashMap.put(DeviceKey.Date, date);
            hashMap.put(DeviceKey.KDetailMinterStep, String.valueOf(step));
            hashMap.put(DeviceKey.Calories, numberFormat.format(cal / 100));
            hashMap.put(DeviceKey.Distance, numberFormat.format(distance / 100));
            hashMap.put(DeviceKey.ArraySteps, stringBuffer.toString());
            list.add(hashMap);
        }

        return maps;
    }

    /**
     * 睡眠数据
     *
     * @param value
     * @return
     */
    public static Map<String,Object> getSleepData(byte[] value) {
        int length = value.length;
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetDetailSleepData);
        maps.put(DeviceKey.End,false);
        List<Map<String, String>> list = new ArrayList<>();
        maps.put(DeviceKey.Data, list);
        if (value[value.length - 1] == (byte) 0xff)  maps.put(DeviceKey.End,true);;
        if(130==length){//一分钟睡眠数据
            Map<String, String> hashMap = new HashMap<>();
            String date = "20" + bcd2String(value[3]) + "-" + bcd2String(value[4]) + "-" + bcd2String(value[5]) + " "
                    + bcd2String(value[6]) + ":" + bcd2String(value[7]) + ":" + bcd2String(value[8]);
            hashMap.put(DeviceKey.Date, date);
            int sleepLength = getValue(value[9], 0);
            StringBuilder stringBuffer = new StringBuilder();
            for (int j = 0; j < sleepLength; j++) {
                stringBuffer.append(getValue(value[10 + j], 0)).append(j == sleepLength ? "" : " ");
            }
            hashMap.put(DeviceKey.ArraySleep, stringBuffer.toString());
            hashMap.put(DeviceKey.sleepUnitLength,"1");
            list.add(hashMap);
        }else{
            int count = 34;
            int size = length / count;
            if (size == 0) {
                maps.put(DeviceKey.Data, list);
                maps.put(DeviceKey.End,true);
                return maps;
            }
            for (int i = 0; i < size; i++) {
                Map<String, String> hashMap = new HashMap<>();
                String date = "20" + bcd2String(value[3 + i * 34]) + "-"
                        + bcd2String(value[4 + i * 34]) + "-" + bcd2String(value[5 + i * 34]) + " "
                        + bcd2String(value[6 + i * 34]) + ":" + bcd2String(value[7 + i * 34]) + ":" + bcd2String(value[8 + i * 34]);
                hashMap.put(DeviceKey.Date, date);
                int sleepLength = getValue(value[9 + i * 34], 0);
                StringBuilder stringBuffer = new StringBuilder();
                try{
                    for (int j = 0; j < sleepLength; j++) {
                        stringBuffer.append(getValue(value[10 + j + i * 34], 0)).append(j == sleepLength ?"":" ");
                    }
                }catch (ArrayIndexOutOfBoundsException E){E.fillInStackTrace(); }
                hashMap.put(DeviceKey.ArraySleep, stringBuffer.toString());
                hashMap.put(DeviceKey.sleepUnitLength,"5");
                list.add(hashMap);
            }
        }
        return maps;
    }


    /**
     * 历史心率数据
     *
     * @param value
     * @return
     */
    public static Map<String,Object> getBloodoxygen(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.Blood_oxygen);
        maps.put(DeviceKey.End,false);
        List<Map<String, String>> list = new ArrayList<>();
        maps.put(DeviceKey.Data, list);
        int count = 10;
        int length = value.length;
        int size = length / count;
        if (size == 0) {
            maps.put(DeviceKey.End,true);
            return maps;
        }
        for (int i = 0; i < size; i++) {
            int flag = 1 + (i + 1) * count;
            if (value[length-1] == (byte) 0xff) {
                maps.put(DeviceKey.End,true);
            }
            Map<String, String> hashMap = new HashMap<>();
            String date = "20"+ByteToHexString(value[3 + i * count]) + "."
                    + ByteToHexString(value[4 + i * count]) + "." + ByteToHexString(value[5 + i * count]) + " "
                    + ByteToHexString(value[6 + i * count]) + ":" + ByteToHexString(value[7 + i * count]) + ":" + ByteToHexString(value[8 + i * count]);
            hashMap.put(DeviceKey.Date, date);
            hashMap.put(DeviceKey.Blood_oxygen, String.valueOf(getValue(value[9 + i * 10], 0)));
            list.add(hashMap);
        }

        return maps;
    }





    /**
     * 历史心率数据
     *
     * @param value
     * @return
     */
    public static Map<String,Object> getHeartData(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetDynamicHR);
        maps.put(DeviceKey.End,false);
        List<Map<String, String>> list = new ArrayList<>();
        maps.put(DeviceKey.Data, list);
        int count = 24;
        int length = value.length;
        int size = length / count;
        if (size == 0) {
            maps.put(DeviceKey.End,true);
            return maps;
        }
        for (int i = 0; i < size; i++) {
            if (value[length-1] == (byte) 0xff) {
                maps.put(DeviceKey.End,true);
            }
            Map<String, String> hashMap = new HashMap<>();
            String date = "20"+ByteToHexString(value[3 + i * count]) + "."
                    + ByteToHexString(value[4 + i * count]) + "." + ByteToHexString(value[5 + i * count]) + " "
                    + ByteToHexString(value[6 + i * count]) + ":" + ByteToHexString(value[7 + i * count]) + ":" + ByteToHexString(value[8 + i * count]);


            StringBuffer stringBuffer = new StringBuffer();
            for (int j = 0; j < 15; j++) {
                stringBuffer.append(String.valueOf(getValue(value[9 + j + i * count], 0))).append(j == 14 ? "" : " ");
            }
            hashMap.put(DeviceKey.Date, date);
            hashMap.put(DeviceKey.ArrayDynamicHR, stringBuffer.toString());
            list.add(hashMap);
        }

        return maps;
    }


    /**
     * 单次历史心率数据
     *
     * @param value
     * @return
     */
    public static Map<String,Object> getOnceHeartData(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetStaticHR);
        maps.put(DeviceKey.End,false);
        List<Map<String, String>> list = new ArrayList<>();
        maps.put(DeviceKey.Data, list);
        int count = 10;
        int length = value.length;
        int size = length / count;
        if (size == 0) {
            maps.put(DeviceKey.End,true);
            return maps;
        }
        for (int i = 0; i < size; i++) {
            int flag = 1 + (i + 1) * count;
            if ( value[length-1] == (byte) 0xff) {
                maps.put(DeviceKey.End,true);
            }
            Map<String, String> hashMap = new HashMap<>();
            String date ="20"+ByteToHexString(value[3 + i * 10]) + "."
                    + ByteToHexString(value[4 + i * 10]) + "." + ByteToHexString(value[5 + i * 10]) + " "
                    + ByteToHexString(value[6 + i * 10]) + ":" + ByteToHexString(value[7 + i * 10]) + ":" + ByteToHexString(value[8 + i * 10]);
            String heart = String.valueOf(getValue(value[9 + i * 10], 0));
            hashMap.put(DeviceKey.Date, date);
            hashMap.put(DeviceKey.StaticHR, heart);
            list.add(hashMap);
        }
        return maps;
    }
    /**
     * htv测试数据
     *
     * @param
     * @return
     */
    public static Map<String,Object> DeleteHrv() {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.DeleteHrv);
        maps.put(DeviceKey.End,true);
        List<Map<String, String>> list = new ArrayList<>();
        maps.put(DeviceKey.Data, list);
        return maps;
    }
    /**
     * htv测试数据
     *
     * @param value
     * @return
     */
    public static Map<String,Object> getHrvTestData(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetHRVData);
        maps.put(DeviceKey.End,false);
        List<Map<String, String>> list = new ArrayList<>();
        maps.put(DeviceKey.Data, list);
        int count = 15;
        int length = value.length;
        int size = length / count;

        if (size == 0) {
            maps.put(DeviceKey.End,true);
            return maps;
        }
        if(value[value.length-1]==  (byte) 0xff)   maps.put(DeviceKey.End,true);;
        for (int i = 0; i < size; i++) {

            Map<String, String> hashMap = new HashMap<>();
            String date ="20"+ByteToHexString(value[3 + i * count]) + "."
                    + ByteToHexString(value[4 + i * count]) + "." + ByteToHexString(value[5 + i * count]) + " "
                    + ByteToHexString(value[6 + i * count]) + ":" + ByteToHexString(value[7 + i * count]) + ":" + ByteToHexString(value[8 + i * count]);
            String hrv = String.valueOf(getValue(value[9 + i * count], 0));
            String blood = String.valueOf(getValue(value[10 + i * count], 0));
            String heart = String.valueOf(getValue(value[11 + i * count], 0));
            String Tired = String.valueOf(getValue(value[12 + i * count], 0));

            String moodValue = String.valueOf(getValue(value[13 + i * count], 0));
            String breathRate = String.valueOf(getValue(value[14 + i * count], 0));
            hashMap.put(DeviceKey.Date, date);
            hashMap.put(DeviceKey.HRV, hrv);
            hashMap.put(DeviceKey.VascularAging, blood);
            hashMap.put(DeviceKey.Stress, Tired);
            hashMap.put(DeviceKey.highBP, moodValue);
            hashMap.put(DeviceKey.lowBP, breathRate);
            hashMap.put(DeviceKey.HeartRate, heart);
            list.add(hashMap);
        }

        return maps;
    }

    /**
     * 闹钟数据
     *
     * @param value
     * @return
     */
    public static Map<String,Object> getClockData(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetAlarmClock);
        maps.put(DeviceKey.End,false);
        List<Map<String, String>> list = new ArrayList<>();
        maps.put(DeviceKey.Data, list);
        int count = 41;
        int length = value.length;
        int size = length / count;

        DeviceBean deviceBean = new DeviceBean();
        if (size == 0) {
            maps.put(DeviceKey.End,true);
            return maps;
        }
        for (int i = 0; i < size; i++) {
            int flag = 1 + (i + 1) * count;
            if (flag < length && value[flag] == (byte) 0xff) {
                maps.put(DeviceKey.End,true);
            }
            Map<String, String> hashMap = new HashMap<>();
            String id = String.valueOf(getValue(value[4 + i * count], 0));
            String enable = String.valueOf(getValue(value[5 + i * count], 0));
            String type = String.valueOf(getValue(value[6 + i * count], 0));
            String hour = ByteToHexString(value[7 + i * count]);
            String min = ByteToHexString(value[8 + i * count]);
            String week = getByteString(value[9 + i * count]);
            int lengthS = getValue(value[10 + i * count], 0);
            String content = "";
            for (int J = 0; J < lengthS; J++) {
                if (value[11 + i * count] == 0)
                    continue;
                content += (char) getValue(value[11 + J + i * count], 0);
            }
            hashMap.put(DeviceKey.KAlarmId, id);
            hashMap.put(DeviceKey.OpenOrClose, enable);
            hashMap.put(DeviceKey.ClockType, type);
            hashMap.put(DeviceKey.ClockTime, hour);
            hashMap.put(DeviceKey.KAlarmMinter, min);
            hashMap.put(DeviceKey.Week, week);
            hashMap.put(DeviceKey.KAlarmContent, content);
            hashMap.put(DeviceKey.KAlarmLength, String.valueOf(lengthS));
            list.add(hashMap);
        }

        return maps;
    }

    private static final String TAG = "ResolveUtil";

/*    public static Map<String,Object> getHistoryGpsData(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetGPSData);
        maps.put(DeviceKey.End,false);
        List<Map<String, String>> list = new ArrayList<>();
        maps.put(DeviceKey.Data, list);
        int count = 59;
        int length = value.length;
        int size = length / count;


        if (size == 0) {
            maps.put(DeviceKey.End,true);
            return maps;
        }
        for (int i = 0; i < size; i++) {
            int flag = 1 + (i + 1) * count;
            if (flag < length && i==size-1&&value[flag] == (byte) 0xff) {
                maps.put(DeviceKey.End,true);
            }
            Map<String, String> hashMap = new HashMap<>();
            int id=getValue(value[1+i*count],0)+getValue(value[2+i*count],1);
            String date = ByteToHexString(value[3 + i * count]) + "."
                    + ByteToHexString(value[4 + i * count]) + "." + ByteToHexString(value[5 + i * count]) + " "
                    + ByteToHexString(value[6 + i * count]) + ":" + ByteToHexString(value[7 + i * count]) + ":" + ByteToHexString(value[8 + i * count]);
            byte[] valueLatitude = new byte[4];
            byte[] valueLongitude = new byte[4];
            StringBuffer stringBufferLatitude = new StringBuffer();
            StringBuffer stringBufferLongitude = new StringBuffer();
            for (int k = 0; k < 6; k++) {
                for (int j = 0; j < 4; j++) {
                    valueLatitude[3 - j] = value[9 + j + i * count + k * 8];
                    valueLongitude[3 - j] = value[13 + j + i * count + k * 8];
                }
                String Latitude = String.valueOf(getFloat(valueLatitude, 0));
                String Longitude = String.valueOf(getFloat(valueLongitude, 0));
                stringBufferLatitude.append(Latitude).append(k == 5 ? "" : ",");
                stringBufferLongitude.append(Longitude).append(k == 5 ? "" : ",");
            }
            hashMap.put(DeviceKey.Date, date);
            hashMap.put(DeviceKey.KDataID,String.valueOf(id));
            hashMap.put(DeviceKey.Latitude, stringBufferLatitude.toString());
            hashMap.put(DeviceKey.Longitude, stringBufferLongitude.toString());
            list.add(hashMap);
        }

        return maps;
    }*/

/*    public static Map<String,Object> getActivityGpsData(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.DeviceSendToApp);
        maps.put(DeviceKey.End,true);
        Map<String, String> hashMap = new HashMap<>();
        maps.put(DeviceKey.Data, hashMap);
        String date = ByteToHexString(value[1]) + "-"
                + ByteToHexString(value[2]) + "-" + ByteToHexString(value[3]) + " "
                + ByteToHexString(value[4]) + ":" + ByteToHexString(value[5]) + ":" + ByteToHexString(value[6]);
        byte[] valueLatitude = new byte[4];
        byte[] valueLongitude = new byte[4];
        for (int j = 0; j < 4; j++) {
            valueLatitude[3 - j] = value[9+j ];
            valueLongitude[3 - j] = value[14+j];
        }
        String Latitude = String.valueOf(getFloat(valueLatitude, 0));
        String Longitude = String.valueOf(getFloat(valueLongitude, 0));
        int count=getValue(value[18],0);

        hashMap.put(DeviceKey.KActivityLocationTime, date);
        hashMap.put(DeviceKey.KActivityLocationLatitude, Latitude);
        hashMap.put(DeviceKey.KActivityLocationLongitude, Longitude);
        hashMap.put(DeviceKey.KActivityLocationCount, String.valueOf(count));

        return maps;
    }*/
public static Map<String,Object> getTempDataer(byte[] value) {
    Map<String,Object> maps=new HashMap<>();
    maps.put(DeviceKey.DataType, BleConst.GetAxillaryTemperatureDataWithMode);
    maps.put(DeviceKey.End,false);
    List<Map<String, String>> list = new ArrayList<>();
    maps.put(DeviceKey.Data, list);

    int count = 11;
    int length = value.length;
    int size = length / count;
    if (size == 0||value[length - 1] == (byte) 0xff) {
        maps.put(DeviceKey.End,true);
    }
    NumberFormat numberFormat=getNumberFormat(1);
    for (int i = 0; i < size; i++) {
        if (value[length-1] == (byte) 0xff) {
            maps.put(DeviceKey.End,true);
        }
        Map<String, String> hashMap = new HashMap<>();
        String date = "20"+bcd2String(value[3 + i * count]) + "."
                + bcd2String(value[4 + i * count]) + "." + bcd2String(value[5 + i * count]) + " "
                + bcd2String(value[6 + i * count]) + ":" + bcd2String(value[7 + i * count]) + ":" + bcd2String(value[8 + i * count]);
        int  tempValue = getValue(value[9 + i * count], 0)+getValue(value[10 + i * count], 1);
        hashMap.put(DeviceKey.Date, date);
        hashMap.put(DeviceKey.axillaryTemperature, numberFormat.format(tempValue*0.1f));
        list.add(hashMap);
    }

    return maps;
}

    public static Map<String,Object> getTempData(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        List<Map<String, String>> list = new ArrayList<>();
        maps.put(DeviceKey.DataType, BleConst.Temperature_history);
        maps.put(DeviceKey.End,false);
        maps.put(DeviceKey.Data, list);
        int count = 11;
        int length = value.length;
        int size = length / count;
        if (size == 0||value[length - 1] == (byte) 0xff) {
            maps.put(DeviceKey.End,true);
            return maps;
        }
        NumberFormat numberFormat=getNumberFormat(1);
        for (int i = 0; i < size; i++) {
            if (value[length-1] == (byte) 0xff) {
                maps.put(DeviceKey.End,true);
            }
            Map<String, String> hashMap = new HashMap<>();
            String date = "20"+bcd2String(value[3 + i * count]) + "."
                    + bcd2String(value[4 + i * count]) + "." + bcd2String(value[5 + i * count]) + " "
                    + bcd2String(value[6 + i * count]) + ":" + bcd2String(value[7 + i * count]) + ":" + bcd2String(value[8 + i * count]);
            int  tempValue = getValue(value[9 + i * count], 0)+getValue(value[10 + i * count], 1);
            hashMap.put(DeviceKey.Date, date);
            hashMap.put(DeviceKey.temperature, numberFormat.format(tempValue*0.1f));
            list.add(hashMap);
        }
        return maps;
    }

    //私有属性
    private Databack databack = null;

    public interface Databack {
        void Endback(Map<String,Object> map);
    }

    //setter方法
    public void setDataback(Databack databack1) {
        this.databack = databack1;
    }

  protected static List<Integer> ecgdata=new ArrayList<>();;
  protected static int size=0;
  protected static int count=0;
  protected static  Map<String,Object> maps=new HashMap<>();
  protected static  List<Map<String, String>> list = new ArrayList<>();
    public static void Getecgdata(byte[] value,Databack databack) {
        if (value[value.length - 1] == (byte) 0x71) {
            maps.put(DeviceKey.DataType, BleConst.DeleteECGdata);
            maps.put(DeviceKey.End,true);
           if(null!=databack){databack.Endback(maps);}
        }else if(3==value.length&&value[value.length - 1] == (byte) 0xff){
            if(read){
                Map<String, String> hashMap = new HashMap<>();
                hashMap.put(DeviceKey.ECGValue, ecgdata.toString()+"");
                list.add(hashMap);
                maps.put(DeviceKey.End,true);
                if(null!=databack){databack.Endback(maps);}
            }else{
                maps.put(DeviceKey.DataType, BleConst. ECGdata);
                maps.put(DeviceKey.End,true);
                if(null!=databack){databack.Endback(maps);}


            }
        }else{

            int index =getValue(value[1 ], 0)+getValue(value[2], 1) ;
            if(0==index){
                ecgdata=new ArrayList<>();;
                maps=new HashMap<>();
                list = new ArrayList<>();
                maps.put(DeviceKey.DataType, BleConst. ECGdata);
                maps.put(DeviceKey.Data, list);
                count=0;
                size =getValue(value[9 ], 0)+getValue(value[10], 1) ;
                String date = "20"+bcd2String(value[3]) + "."
                        + bcd2String(value[4 ]) + "." + bcd2String(value[5]) + " "
                        + bcd2String(value[6 ]) + ":" + bcd2String(value[7 ]) + ":" + bcd2String(value[8 ]);
                Map<String, String> hashMap = new HashMap<>();
                hashMap.put(DeviceKey.Date, date);
                hashMap.put(DeviceKey.HRV, getValue(value[11], 0)+"");
                hashMap.put(DeviceKey.HeartRate, getValue(value[12], 0)+"");
                hashMap.put(DeviceKey.ECGMoodValue, getValue(value[12], 0)+"");
                byte[] bs = new byte[value.length-14];
                System.arraycopy(value, 14, bs, 0, bs.length);
                for (int i=0;i<bs.length/2;i++){
                    int valueddd = i*2;
                    int ecg = getValue(bs[valueddd], 0) + getValue(bs[valueddd+1], 1);
                    ecgdata.add(ecg);
                    count++;
                }
                //maps.put(DeviceKey.End,false);
               // hashMap.put(DeviceKey.ECGValue, ecgdata.toString()+"");
                list.add(hashMap);

            }else{

                byte[] bs = new byte[value.length-3];
                System.arraycopy(value, 3, bs, 0, bs.length);
                for (int i=0;i<bs.length/2;i++){
                    int valueddd = i*2;
                    int ecg = getValue(bs[valueddd], 0) + getValue(bs[valueddd+1], 1);
                    ecgdata.add(ecg);
                    count++;
                }
                Log.e("count",count+"***"+size);
               /* if(size==count){
                    Map<String, String> hashMap = new HashMap<>();
                    hashMap.put(DeviceKey.ECGValue, ecgdata.toString()+"");
                    list.add(hashMap);
                    maps.put(DeviceKey.End,true);
                    if(null!=databack){databack.Endback(maps);}
                }*/

            }
        }

    }




    public static Map<String, Object> getEcgHistoryData(byte[] value) {
            Map<String,Object> maps=new HashMap<>();
           Map<String, String> list = new HashMap<>();
            maps.put(DeviceKey.DataType, BleConst.ECGdata);
            maps.put(DeviceKey.End,false);
            maps.put(DeviceKey.Data, list);

        int length = value.length;
        if (length== 3||(value[length - 3] == (byte) 0x71 && value[length - 2] == (byte) 0xff && value[length - 1] == (byte) 0xff)) {
            maps.put(DeviceKey.End,true);
            return maps;
        }
        int id = getValue(value[1], 0) + getValue(value[2], 1);
        int offset = 3;
        Map<String, String> hashMap = new HashMap<>();
        if (id == 0) {//第一条
            String date = "20"+bcd2String(value[3]) + "-"
                    + bcd2String(value[4]) + "-" + bcd2String(value[5]) + " "
                    + bcd2String(value[6]) + ":" + bcd2String(value[7]) + ":" + bcd2String(value[8]);
            String hrv = String.valueOf(getValue(value[11], 0));
            String heart = String.valueOf(getValue(value[12], 0));
            String moodValue = String.valueOf(getValue(value[13], 0));
            list.put(DeviceKey.Date, date);
            list.put(DeviceKey.HRV,hrv);
            list.put(DeviceKey.HeartRate,heart);
            list.put(DeviceKey.ECGMoodValue, moodValue);
           /* list.add(hashMap);*/
            offset = 27;

        }
        byte[] tempValue = new byte[length - offset];
        System.arraycopy(value, offset, tempValue, 0, tempValue.length);
        String ecgData = getEcgDataString(tempValue);
        list.put(DeviceKey.ECGValue, ecgData);
        /*list.add(hashMap);*/
        return maps;
    }


    protected static String getEcgDataString(byte[] value) {
        StringBuffer stringBuffer = new StringBuffer();
        int length = value.length / 2 - 1;
        for (int i = 0; i < length; i++) {
            int ecgValue = getValue(value[i * 2 + 1], 1) + ResolveUtil.getValue(value[i * 2 + 2], 0);
            if (ecgValue >= 32768) ecgValue = ecgValue - 65536;
            stringBuffer.append(ecgValue).append(",");
        }
        return stringBuffer.toString();
    }


    public static Map<String,Object> getExerciseData(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.GetActivityModeData);
        maps.put(DeviceKey.End,false);
        List<Map<String, String>> list = new ArrayList<>();
        maps.put(DeviceKey.Data, list);
        int count = 25;
        int length = value.length;
        int size = length / count;
        if (size == 0) {
            maps.put(DeviceKey.End,true);
            return maps;
        }
        NumberFormat numberFormatCal =getNumberFormat(1);
        NumberFormat numberFormat =getNumberFormat(2);
        for (int i = 0; i < size; i++) {
            int flag = 1 + (i + 1) * count;
            if (flag < length &&  i==size-1&&value[flag] == (byte) 0xff) {
                maps.put(DeviceKey.End,true);
            }
            Map<String, String> hashMap = new HashMap<>();
            String date ="20"+ByteToHexString(value[3 + i * count]) + "."
                    + ByteToHexString(value[4 + i * count]) + "." + ByteToHexString(value[5 + i * count]) + " "
                    + ByteToHexString(value[6 + i * count]) + ":" + ByteToHexString(value[7 + i * count]) + ":" + ByteToHexString(value[8 + i * count]);
            String mode = String.valueOf(getValue(value[9 + i * count], 0));
            String heartRate = String.valueOf(getValue(value[10 + i * count], 0));
            int periodTime = getData(2, 11 + i * count, value);
            int steps = getData(2, 13 + i * count, value);
            int speedMin = getValue(value[15 + i * count],0);
            int speedS = getValue(value[16 + i * count],0);
            byte[] valueCal = new byte[4];
            for (int j = 0; j < 4; j++) {
                valueCal[3 - j] = value[17 +j+ i * count];
            }
            byte[] valueDistance = new byte[4];
            for (int j = 0; j < 4; j++) {
                valueDistance[3 - j] = value[21 +j+ i * count];
            }
            float cal = getFloat(valueCal,0);
            float distance = getFloat(valueDistance,0);

            hashMap.put(DeviceKey.Date, date);
            hashMap.put(DeviceKey.ActivityMode, mode);
            hashMap.put(DeviceKey.HeartRate, heartRate);
            hashMap.put(DeviceKey.ActiveMinutes, String.valueOf(periodTime));
            hashMap.put(DeviceKey.Step, String.valueOf(steps));
            hashMap.put(DeviceKey.Pace, String.format("%02d",speedMin)+"'"+String.format("%02d",speedS)+"\"");
            hashMap.put(DeviceKey.Distance, numberFormat.format(distance));
            hashMap.put(DeviceKey.Calories, numberFormatCal.format(cal));
            list.add(hashMap);
        }

        return maps;
    }

    public static Map<String,Object> getActivityExerciseData(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.SportData);
        maps.put(DeviceKey.End,true);

        Map<String, String> map = new HashMap<>();
        maps.put(DeviceKey.Data, map);
        int heartRate = getValue(value[1], 0);
        int steps = 0;
        float kcal = 0;
        for (int i = 0; i < 4; i++) {
            steps += getValue(value[i + 2], i);
        }
        byte[] valueCal = new byte[4];
        for (int i = 0; i < 4; i++) {
            valueCal[3 - i] = value[i + 6];
            // kcal+=getFloat(value[i+6],i);
        }
        kcal = getFloat(valueCal, 0);
        map.put(DeviceKey.HeartRate, String.valueOf(heartRate));
        map.put(DeviceKey.Step, String.valueOf(steps));
        map.put(DeviceKey.Calories, String.valueOf(kcal));
        return maps;
    }
    public static Map<String,Object> setTimeSuccessful(byte[] value) {//设置时间成功后返回手机一个数据包的最大长度
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType,BleConst.SetDeviceTime);
        maps.put(DeviceKey.End,true);
        Map<String, String> map = new HashMap<>();
        maps.put(DeviceKey.Data, map);

        map.put(DeviceKey.KPhoneDataLength, String.valueOf(getValue(value[1],0)));
        return maps;
    }


    public static Map<String,Object> setMacSuccessful() {//设置时间成功后返回手机一个数据包的最大长度
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType,BleConst.CMD_Set_Mac);
        maps.put(DeviceKey.End,true);
        Map<String, String> map = new HashMap<>();
        maps.put(DeviceKey.Data, map);
        return maps;
    }




    public static Map<String,Object> ECGData(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        Map<String, byte[]> mapData = new HashMap<>();
        maps.put(DeviceKey.End,true);
        maps.put(DeviceKey.DataType, BleConst.EcgppG);
        maps.put(DeviceKey.Data, mapData);
        mapData.put(DeviceKey.ECGValue,value);
        return maps;
    }
    public static Map<String,Object> ecgData(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.End,true);
        maps.put(DeviceKey.DataType, BleConst.EcgppGstatus);
        int index = ResolveUtil.getValue(value[1], 0);
        maps.put(DeviceKey.EcgStatus, index);
       /* if(0==index){//有数据
            String date ="20"+ByteToHexString(value[2]) + "."+
                    ByteToHexString(value[3]) + "." + ByteToHexString(value[4]) + " "+
                    ByteToHexString(value[5]) + ":" + ByteToHexString(value[6]) + ":" + ByteToHexString(value[7]);
            maps.put(DeviceKey.Date,date);
            int SBP = ResolveUtil.getValue(value[8], 0);
            int DBP = ResolveUtil.getValue(value[9], 0);
            int HR = ResolveUtil.getValue(value[10], 0);
            maps.put(DeviceKey.EcgSBP, SBP);
            maps.put(DeviceKey.EcgDBP, DBP);
            maps.put(DeviceKey.EcgHR, HR);
            int  DOWN = getValue(value[11], 0)+getValue(value[12], 1);
            int  UP = getValue(value[13], 0)+getValue(value[14], 1);
            maps.put(DeviceKey.WaveformDownTime, DOWN);
            maps.put(DeviceKey.WaveformRiseTime, UP);
            maps.put(DeviceKey.ECGHrValue, ResolveUtil.getValue(value[15], 0));
            maps.put(DeviceKey.EcgGender, ResolveUtil.getValue(value[16], 0));
            maps.put(DeviceKey.EcgAge, ResolveUtil.getValue(value[17], 0));
            maps.put(DeviceKey.EcgHeight, ResolveUtil.getValue(value[18], 0));
            maps.put(DeviceKey.EcgWeight, ResolveUtil.getValue(value[19], 0));
        }*/
        return maps;
    }

    public static Map<String,Object> GetEcgPpgStatus(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.End,true);
        maps.put(DeviceKey.DataType, BleConst.GetEcgPpgStatus);
        int index = ResolveUtil.getValue(value[1], 0);
        maps.put(DeviceKey.EcgStatus, index);
        if(3==index){//有数据
            int HR = ResolveUtil.getValue(value[2], 0);
            int VascularAging = ResolveUtil.getValue(value[3], 0);
            int HeartRate = ResolveUtil.getValue(value[4], 0);
            int  Fatiguedegree= ResolveUtil.getValue(value[5], 0);
            int HighPressure = ResolveUtil.getValue(value[6], 0);
            int LowPressure = ResolveUtil.getValue(value[7], 0);
            int ECGMoodValue = ResolveUtil.getValue(value[8], 0);
            int ECGBreathValue = ResolveUtil.getValue(value[9], 0);
            maps.put(DeviceKey.HeartRate, HeartRate);
            maps.put(DeviceKey.VascularAging, VascularAging);
            maps.put(DeviceKey.HRV, HR);
            maps.put(DeviceKey.Fatiguedegree, Fatiguedegree);
            maps.put(DeviceKey.HighPressure, HighPressure);
            maps.put(DeviceKey.LowPressure, LowPressure);
            maps.put(DeviceKey.ECGMoodValue, ECGMoodValue);
            maps.put(DeviceKey.ECGBreathValue, ECGBreathValue);
            String date ="20"+ByteToHexString(value[10]) + "."+
                    ByteToHexString(value[11]) + "." + ByteToHexString(value[12]) + " "+
                    ByteToHexString(value[13]) + ":" + ByteToHexString(value[14]) + ":" + ByteToHexString(value[15]);
            maps.put(DeviceKey.Date,date);
        }
        return maps;
    }
    public static Map<String,Object> ECGResult(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        Map<String, String> mapData = new HashMap<>();
        maps.put(DeviceKey.End,true);
        maps.put(DeviceKey.DataType, BleConst.ECGResult);
        maps.put(DeviceKey.Data, mapData);
        int resultValue= getValue(value[1],0);
        String date ="20"+ByteToHexString(value[10]) + "."+
        ByteToHexString(value[11]) + "." + ByteToHexString(value[12]) + " "+
        ByteToHexString(value[13]) + ":" + ByteToHexString(value[14]) + ":" + ByteToHexString(value[15]);
        int hrv = ResolveUtil.getValue(value[2], 0);
        int avBlock = ResolveUtil.getValue(value[3], 0);
        int hr = ResolveUtil.getValue(value[4], 0);
        int strees = ResolveUtil.getValue(value[5], 0);
        int highBp = ResolveUtil.getValue(value[6], 0);
        int lowBp = ResolveUtil.getValue(value[7], 0);
        int moodValue = ResolveUtil.getValue(value[8], 0);
        int breathValue = ResolveUtil.getValue(value[9], 0);
        mapData.put(DeviceKey.ECGResultValue,String.valueOf(resultValue));
        mapData.put(DeviceKey.Date,date);
        mapData.put(DeviceKey.ECGHrvValue,String.valueOf(hrv));
        mapData.put(DeviceKey.ECGAvBlockValue,String.valueOf(avBlock));
        mapData.put(DeviceKey.ECGHrValue,String.valueOf(hr));
        mapData.put(DeviceKey.ECGStreesValue,String.valueOf(strees));
        mapData.put(DeviceKey.ECGhighBpValue,String.valueOf(highBp));
        mapData.put(DeviceKey.ECGLowBpValue,String.valueOf(lowBp));
        mapData.put(DeviceKey.ECGMoodValue,String.valueOf(moodValue));
        mapData.put(DeviceKey.ECGBreathValue,String.valueOf(breathValue));
        return maps;
    }
    public static Map<String,Object> enterEcg(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        Map<String, String> mapData = new HashMap<>();
        maps.put(DeviceKey.End,true);
        maps.put(DeviceKey.DataType, BleConst.ENTERECG);
        maps.put(DeviceKey.Data, mapData);

        return maps;
    }
    public static Map<String,Object> updateClockSuccessful(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.SetAlarmClockWithAllClock);
        maps.put(DeviceKey.End,true);
        Map<String, String> map = new HashMap<>();
        maps.put(DeviceKey.Data, map);
        map.put(DeviceKey.KClockLast, String.valueOf(getValue(value[value.length-1],0)));
        return maps;
    }

    public static int getData(int length, int start, byte[] value) {
        int data = 0;
        for (int j = 0; j < length; j++) {
            data += getValue(value[j + start], j);
        }
        return data;
    }

    public static int getValue(byte b, int count) {
        return (int) ((b & 0xff) * Math.pow(256, count));
    }

    public static String ByteToHexString(byte a) {
        String s = "";
        s = Integer.toHexString(new Byte(a).intValue());
        if (s.length() == 1) {
            s = "0" + s;
        }
        return s;
    }

    public static float getFloat(byte[] arr, int index) {//字节转浮点
        return Float.intBitsToFloat(getInt(arr, index));
    }

    public static int getInt(byte[] arr, int index) {
        return (0xff000000 & (arr[index + 0] << 24)) |
                (0x00ff0000 & (arr[index + 1] << 16)) |
                (0x0000ff00 & (arr[index + 2] << 8)) |
                (0x000000ff & arr[index + 3]);
    }
    public static NumberFormat getNumberFormat(int max){
        NumberFormat numberFormat=NumberFormat.getNumberInstance();
        numberFormat.setMaximumFractionDigits(max);
        numberFormat.setGroupingUsed(false);
        return numberFormat;
    }
    public static String byte2Hex(byte data) {
        return String.format("%02X ", data);
    }
    public static String getCurrentTimeZone() {
        TimeZone tz = TimeZone.getDefault();
        String strTz = tz.getDisplayName(false, TimeZone.SHORT);
        Log.i(TAG, "getCurrentTimeZone: "+strTz);
        int _t = TimeZone.getDefault().getOffset(System.currentTimeMillis()) / (3600 * 1000);
        strTz = String.valueOf(_t);
        strTz = "GMT" + strTz;
        Log.i(TAG, "getCurrentTimeZone: "+strTz);
        return strTz;

    }
    public static Map<String, Object> setMethodSuccessful(String dataType) {//设置时间成功后返回手机一个数据包的最大长度
        Map<String, Object> maps = new HashMap<>();
        maps.put(DeviceKey.DataType, dataType);
        maps.put(DeviceKey.End, true);
        Map<String, String> map = new HashMap<>();
        maps.put(DeviceKey.Data, map);
        return maps;
    }

    public static Map<String,Object>  GetTemperatureCorrectionValue(byte[] value) {
        Map<String,Object> maps=new HashMap<>();
        maps.put(DeviceKey.DataType, BleConst.CMD_Set_TemperatureCorrection);
        maps.put(DeviceKey.End,true);
        Map<String,String>mapData=new HashMap<>();
        maps.put(DeviceKey.Data, mapData);
        byte[]tempValue=new byte[2];
        tempValue[0]=value[3];
        tempValue[1]=value[2];
        int TemperatureCorrectionValue = BleSDK.byteArrayToInt(tempValue);

        mapData.put(DeviceKey.TemperatureCorrectionValue, String.valueOf(TemperatureCorrectionValue));
        return maps;
    }


    public static String[] getWorkOutReminder(byte[] value) {
        String[] activityAlarm = new String[6];
        String startHour = bcd2String(value[1]);
        String startMin = bcd2String(value[2]);
        String days= getValue(value[3], 0)+"";
        String week = getByteString(value[4]);
        int enable=getValue(value[5], 0);
        int time = getValue(value[6], 0)+getValue(value[7],1);

        activityAlarm[0] = startHour;
        activityAlarm[1] = startMin;
        activityAlarm[2] = days;
        activityAlarm[3] = week;
        activityAlarm[4] = String.valueOf(enable);
        activityAlarm[5] = String.valueOf(time);

        return activityAlarm;
    }


    public static   Map<String, Object> getHistoryGpsData(byte[] value) {

        List<Map<String, String>> list = new ArrayList<>();
        Map<String,Object> mmp=new HashMap<>();
        mmp.put(DeviceKey.DataType, BleConst.Gps);
        mmp.put(DeviceKey.End, false);
        int count = 59;
        int length = value.length;
        int size = length / count;
        if (size == 0) {
            return mmp;
        }
        int index=size*50;
        if ((value[value.length - 1] == (byte) 0xff && value[value.length - 2] == (byte) 0x5a)){
            mmp.put(DeviceKey.End, true);
        }
        for (int i = 0; i < size; i++) {
            int flag = 1 + (i + 1) * count;
            Map<String, String> hashMap = new HashMap<>();
            int id = getValue(value[1 + i * count], 0) + getValue(value[2 + i * count], 1)%index;
            String date ="20"+bcd2String(value[3 + i * count]) + "."
                    + bcd2String(value[4 + i * count]) + "." + bcd2String(value[5 + i * count]) + " "
                    + bcd2String(value[6 + i * count]) + ":" + bcd2String(value[7 + i * count]) + ":" + bcd2String(value[8 + i * count]);
            byte[] valueLatitude = new byte[4];
            byte[] valueLongitude = new byte[4];
            StringBuffer stringBufferLatitude = new StringBuffer();
            StringBuffer stringBufferLongitude = new StringBuffer();
            for (int k = 0; k < 6; k++) {
                for (int j = 0; j < 4; j++) {
                    valueLatitude[3 - j] = value[9 + j + i * count + k * 8];
                    valueLongitude[3 - j] = value[13 + j + i * count + k * 8];
                }
                String Latitude = String.valueOf(getFloat(valueLatitude, 0));
                String Longitude = String.valueOf(getFloat(valueLongitude, 0));
                stringBufferLatitude.append(Latitude).append(k == 5 ? "" : ",");
                stringBufferLongitude.append(Longitude).append(k == 5 ? "" : ",");
            }
            hashMap.put(DeviceKey.Date, date.length() == 17 ? date : "2019.01.01 00:00:00");
            hashMap.put(DeviceKey.Latitude, stringBufferLatitude.toString());
            hashMap.put(DeviceKey.Longitude, stringBufferLongitude.toString());
            list.add(hashMap);
        }
        mmp.put(DeviceKey.Data, list.toString());

        return mmp;
    }


    static double[] B_HR = new double[]{
            0.012493658738073,
            0,
            -0.024987317476146,
            0,
            0.012493658738073
    };
    static double[] A_HR = new double[]{
            1,
            -3.658469528008591,
            5.026987876570873,
            -3.078346646055655,
            0.709828779797188
    };
    static double[] inPut = new double[]{0, 0, 0, 0, 0};
    static double[] outPut = new double[]{0, 0, 0, 0, 0};

    public static double filterEcgData(double data) {
        inPut[4] = data * 18.3 / 128 + 0.06;
        outPut[4] = B_HR[0] * inPut[4] +
                B_HR[1] * inPut[3] +
                B_HR[2] * inPut[2] +
                B_HR[3] * inPut[1] +
                B_HR[4] * inPut[0] -
                A_HR[1] * outPut[3] -
                A_HR[2] * outPut[2] -
                A_HR[3] * outPut[1] -
                A_HR[4] * outPut[0];
        for (int i = 0; i < 4; i++) {
            inPut[i] = inPut[i + 1];
            outPut[i] = outPut[i + 1];
        }
        return outPut[4];
    }
}
