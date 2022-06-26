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


import android.text.TextUtils;
import android.util.Log;

import com.jstyle.blesdk2025.callback.DataListener2025;
import com.jstyle.blesdk2025.constant.BleConst;
import com.jstyle.blesdk2025.constant.DeviceConst;
import com.jstyle.blesdk2025.constant.DeviceKey;

import com.jstyle.blesdk2025.model.AutoMode;
import com.jstyle.blesdk2025.model.MyAutomaticHRMonitoring;
import com.jstyle.blesdk2025.model.Clock;
import com.jstyle.blesdk2025.model.MyDeviceInfo;
import com.jstyle.blesdk2025.model.MyPersonalInfo;
import com.jstyle.blesdk2025.model.Notifier;
import com.jstyle.blesdk2025.model.MyDeviceTime;
import com.jstyle.blesdk2025.model.MySedentaryReminder;
import com.jstyle.blesdk2025.model.SportPeriod;
import com.jstyle.blesdk2025.model.StepModel;
import com.jstyle.blesdk2025.model.WeatherData;

import java.io.UnsupportedEncodingException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static com.jstyle.blesdk2025.Util.ResolveUtil.bcd2String;
import static com.jstyle.blesdk2025.Util.ResolveUtil.getFloat;
import static com.jstyle.blesdk2025.Util.ResolveUtil.getValue;


/**
 * Created by Administrator on 2018/4/9.
 */

public class BleSDK {
    public static final int DATA_READ_START = 0;
    public static final int DATA_READ_CONTINUE = 2;
    public static final int DATA_DELETE = 99;
    public static final byte DistanceMode_MILE= (byte) 0x81;
    public static final byte DistanceMode_KM= (byte) 0x80;
    public static final byte TimeMode_12h= (byte) 0x81;
    public static final byte TimeMode_24h= (byte) 0x80;
    public static final byte WristOn_Enable= (byte) 0x81;
    public static final byte WristOn_DisEnable= (byte) 0x80;
    public static final byte TempUnit_C = (byte) 0x80;
    public static final byte TempUnit_F = (byte) 0x81;

    public static final String TAG = "BleSDK";
    public  static boolean isruning=false;

    public static byte[] deleteAllClock() {
        byte[] value = new byte[16];
        value[0] = 0x57;
        value[1] = (byte) 0x99;
        crcValue(value);
        return value;
    }


    /**
     * 获得温度修正值
     *
     * @return
     */
    public static byte[] GetTemperatureCorrectionValue() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_TemperatureCorrection;
        value[1] = 0;
        crcValue(value);
        return value;
    }



    public static byte[] setClockData(List<Clock> clockList) {
        int size = clockList.size();
        int length = 39;
        byte[] totalValue = new byte[length * size + 2];
        for (int i = 0; i < clockList.size(); i++) {
            Clock clock = clockList.get(i);
            byte[] value = new byte[length];
            String content = clock.getContent();
            byte[] infoValue = getInfoValue(content, 30);
            value[0] = DeviceConst.CMD_Set_Clock;
            value[1] = (byte) size;
            value[2] = (byte) clock.getNumber();
            value[3] = (byte) (clock.isEnable() ? 1 : 0);
            value[4] = (byte) clock.getType();
            value[5] = ResolveUtil.getTimeValue(clock.getHour());
            value[6] = ResolveUtil.getTimeValue(clock.getMinute());
            value[7] = (byte) clock.getWeek();
            value[8] = (byte) (infoValue.length == 0 ? 1 : infoValue.length);
            System.arraycopy(infoValue, 0, value, 9, infoValue.length);
            System.arraycopy(value, 0, totalValue, i * length, value.length);
        }
        Log.i(TAG, "setClockData: " + totalValue.length);
        totalValue[totalValue.length - 2] = DeviceConst.CMD_Set_Clock;
        totalValue[totalValue.length - 1] = (byte) 0xff;
        return totalValue;
    }

    public static byte[] EnterActivityMode(int activityMode, int WorkMode) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Start_EXERCISE;
        value[1] = (byte) WorkMode;
        value[2] = (byte) activityMode;
        crcValue(value);
        return value;
    }


    public static byte[]  GPSControlCommand(boolean open) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.GPSControlCommand;
        value[1] = open?(byte)0x01:(byte)0x00;
        crcValue(value);
        return value;
    }


    public static byte[] sendHeartPackage(float distance, int space, int rssi) {
        byte[] value = new byte[16];
        byte[] distanceValue = getByteArray(distance);
        int min = space / 60;
        int second = space % 60;
        value[0] = DeviceConst.CMD_heart_package;
        System.arraycopy(distanceValue, 0, value, 1, distanceValue.length);
        value[5] = (byte) min;
        value[6] = (byte) second;
        value[7] = (byte) rssi;
        crcValue(value);
        return value;
    }

    private static byte[] getByteArray(float f) {
        int intbits = Float.floatToIntBits(f);//将float里面的二进制串解释为int整数
        return getByteArray(intbits);
    }

    public static byte[] SportMode(List<Integer>list){
        byte[] value = new byte[16];
        value[0] = DeviceConst.SportMode;
        for(int i=0;i<5;i++){
            value[i+1]= (byte) 0xff;
        }
        int startIndex=1;
        for(int i:list){
            value[startIndex]= (byte) (i<6?i:i+1);
            startIndex++;
        }
        crcValue(value);
        return value;
    }


    public static byte[] GetSportMode(){
        byte[] value = new byte[16];
        value[0] = DeviceConst.GetSportMode;
        crcValue(value);
        return value;
    }


  /*  public static byte[] setWorkOutReminder(SportPeriod activityAlarm) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_WorkOutReminder;
        value[1] = ResolveUtil.getTimeValue(activityAlarm.getStartHour());
        value[2] = ResolveUtil.getTimeValue(activityAlarm.getStartMinute());
        value[3]= (byte) activityAlarm.getDays();//天数
        value[4] = (byte) activityAlarm.getWeek();
        value[5] = (byte) (activityAlarm.isEnable()?1:0);
        int min=activityAlarm.getIntervalTime();
        value[6] = (byte) ((min) & 0xff);
        value[7] = (byte) ((min >> 8) & 0xff);
        crcValue(value);
        return value;
    }*/

    public static byte[] getWorkOutReminder() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_WorkOutReminder;
        crcValue(value);
        return value;
    }



    public static void DataParsingWithData(byte[] value, final DataListener2025 dataListener) {
        Map<String, String> map = new HashMap<>();
        switch (value[0]) {
            case DeviceConst.CMD_Set_Goal:
                dataListener.dataCallback(ResolveUtil.setMethodSuccessful(BleConst.SetStepGoal));
                break;
            case DeviceConst.CMD_Set_UseInfo:
                dataListener.dataCallback(ResolveUtil.setMethodSuccessful(BleConst.SetPersonalInfo));
                break;
         /*   case DeviceConst.CMD_Set_Name:
                dataListener.dataCallback(ResolveUtil.setMethodSuccessful(BleConst.SetDeviceName));
                break;*/
            case DeviceConst.CMD_Set_MOT_SIGN:
                dataListener.dataCallback(ResolveUtil.setMethodSuccessful(BleConst.SetMotorVibrationWithTimes));
                break;
            case DeviceConst.CMD_Set_DeviceInfo:
                if(!isSetDialinterface){
                    dataListener.dataCallback(ResolveUtil.setMethodSuccessful(BleConst.SetDeviceInfo));
                }else{
                    dataListener.dataCallback(ResolveUtil.setMethodSuccessful(BleConst.SetDialinterface));
                }
                break;
            case DeviceConst.CMD_Set_Auto:
                dataListener.dataCallback(ResolveUtil.setMethodSuccessful(BleConst.SetAutomatic));
                break;
            case DeviceConst.CMD_Set_ActivityAlarm:
                dataListener.dataCallback(ResolveUtil.setMethodSuccessful(BleConst.SetSedentaryReminder));
                break;
           case DeviceConst.CMD_Set_DeviceID:
               dataListener.dataCallback(ResolveUtil.setMacSuccessful());
               break;
            case DeviceConst.CMD_Start_EXERCISE:
                Map<String, Object> mapEXERCISE = new HashMap<>();
                mapEXERCISE.put(DeviceKey.DataType, BleConst.EnterActivityMode);
                mapEXERCISE.put(DeviceKey.End, true);
                Map<String, String> mapsx = new HashMap<>();
                mapsx.put(DeviceKey.enterActivityModeSuccess, ResolveUtil.getValue(value[1], 0) + "");
                mapEXERCISE.put(DeviceKey.Data, mapsx);
                dataListener.dataCallback(mapEXERCISE);
                break;
            case DeviceConst.CMD_Set_TemperatureCorrection:
                if (value[2] != 0 && value[3] != 0) {
                    dataListener.dataCallback(ResolveUtil.GetTemperatureCorrectionValue(value));
                }
                break;
            case DeviceConst.CMD_HeartPackageFromDevice:
                dataListener.dataCallback(ResolveUtil.getActivityExerciseData(value));
                break;
            case DeviceConst.CMD_SET_TIME:
                dataListener.dataCallback(ResolveUtil.setTimeSuccessful(value));

                break;

            case DeviceConst.CMD_Get_SPORTData:
                if(GetActivityModeDataWithMode){
                    dataListener.dataCallback(ResolveUtil.setMethodSuccessful( BleConst.Delete_ActivityModeData));
                }else{
                    dataListener.dataCallback(ResolveUtil.getExerciseData(value));
                }

                break;
            case DeviceConst.CMD_GET_TIME:
                dataListener.dataCallback(ResolveUtil.getDeviceTime(value));
                break;
            case DeviceConst.CMD_GET_USERINFO:
                dataListener.dataCallback(ResolveUtil.getUserInfo(value));
                break;
            case DeviceConst.CMD_Get_DeviceInfo:
                dataListener.dataCallback(ResolveUtil.getDeviceInfo(value));
                dataListener.dataCallback(value);
                break;
            case DeviceConst.CMD_Enable_Activity:
                dataListener.dataCallback(ResolveUtil.getActivityData(value));
                break;
            case DeviceConst.CMD_Get_Goal:
                dataListener.dataCallback(ResolveUtil.getGoal(value));
                break;
            case DeviceConst.CMD_Get_BatteryLevel:

                dataListener.dataCallback(ResolveUtil.getDeviceBattery(value));
                break;
            case DeviceConst.CMD_Get_Address:
                dataListener.dataCallback(ResolveUtil.getDeviceAddress(value));
                break;
            case DeviceConst.CMD_Get_Version:
                dataListener.dataCallback(ResolveUtil.getDeviceVersion(value));
                break;
            case DeviceConst.CMD_Get_Name:
                dataListener.dataCallback(ResolveUtil.getDeviceName(value));
                break;
            case DeviceConst.CMD_Get_Auto:
                dataListener.dataCallback(ResolveUtil.getAutoHeart(value));
                break;
            case DeviceConst.CMD_Reset:
                dataListener.dataCallback(ResolveUtil.Reset());
                break;
            case DeviceConst.CMD_Mcu_Reset:
                dataListener.dataCallback(ResolveUtil.MCUReset());
                break;
            case DeviceConst.CMD_Notify:
                dataListener.dataCallback(ResolveUtil.Notify());
                break;
            case DeviceConst.CMD_Get_ActivityAlarm:
                dataListener.dataCallback(ResolveUtil.getActivityAlarm(value));
                break;
            case DeviceConst.CMD_Get_TotalData:
                if(GetTotalActivityDataWithMode){
                    dataListener.dataCallback(ResolveUtil.setMethodSuccessful(BleConst.Delete_GetTotalActivityData));
                }else{
                    dataListener.dataCallback(ResolveUtil.getTotalStepData(value));
                }

                break;
            case DeviceConst.CMD_Get_DetailData:
                dataListener.dataCallback(ResolveUtil.getDetailData(value));
                break;
            case DeviceConst.CMD_Get_SleepData:
                dataListener.dataCallback(ResolveUtil.getSleepData(value));
                break;
            case DeviceConst.CMD_Get_HeartData:
                if(GetDynamicHRWithMode){
                    dataListener.dataCallback(ResolveUtil.setMethodSuccessful(BleConst.Delete_GetDynamicHR));
                }else{
                    dataListener.dataCallback(ResolveUtil.getHeartData(value));
                }

                break;
            case DeviceConst.CMD_Get_OnceHeartData:
                if(GetStaticHRWithMode){
                    dataListener.dataCallback(ResolveUtil.setMethodSuccessful(BleConst.Delete_GetStaticHR));
                }else{
                    dataListener.dataCallback(ResolveUtil.getOnceHeartData(value));
                }

                break;
            case DeviceConst.CMD_Get_HrvTestData:
                if(readhrv){
                    dataListener.dataCallback(ResolveUtil.getHrvTestData(value));
                }else{
                    dataListener.dataCallback(ResolveUtil.DeleteHrv());
                }
                break;
            case DeviceConst.CMD_Get_Clock:
                if(GetAlarmClock){
                    dataListener.dataCallback(ResolveUtil.setMethodSuccessful(BleConst.Delete_AlarmClock));
                }else{
                    dataListener.dataCallback(ResolveUtil.getClockData(value));
                }

                break;
            case DeviceConst.CMD_Set_Clock:
                dataListener.dataCallback(ResolveUtil.updateClockSuccessful(value));
                break;
            case DeviceConst.CMD_Set_HeartbeatPackets:
                Map<String,Object> maps=new HashMap<>();
                Map<String, String> mapb = new HashMap<>();
                mapb.put(DeviceKey.Type, value[1] == 0 ?DeviceKey.Manual:DeviceKey.automatic);
                maps.put(DeviceKey.Data, mapb);
                maps.put(DeviceKey.DataType, BleConst.HeartBeatpacket);
                maps.put(DeviceKey.End, true);
                dataListener.dataCallback(maps);
                break;

            case DeviceConst.Enter_photo_modeback:
                Map<String,Object> mapdd=new HashMap<>();
                Map<String, String> mapbb = new HashMap<>();
                switch (value[1]){
                    case 1:
                        mapbb.put(DeviceKey.type, value[2]==0?DeviceKey.HangUp:DeviceKey.Telephone);
                        break;
                    case 2:
                        if (value[1] == 2) {
                            mapbb.put(DeviceKey.type, DeviceKey.Photograph);
                        } else {
                            mapbb.put(DeviceKey.type, DeviceKey.CanclePhotograph);
                        }
                        break;
                    case 3:
                        switch (value[2]){
                            case 0:
                                mapbb.put(DeviceKey.type,DeviceKey.Suspend);
                                break;
                            case 1:
                                mapbb.put(DeviceKey.type, DeviceKey.Play);
                                break;
                            case 2:
                                mapbb.put(DeviceKey.type, DeviceKey.LastSong);
                                break;
                            case 3:
                                mapbb.put(DeviceKey.type, DeviceKey.NextSong);
                                break;
                            case 4:
                                mapbb.put(DeviceKey.type, DeviceKey.VolumeReduction);
                                break;
                            case 5:
                                mapbb.put(DeviceKey.type, DeviceKey.VolumeUp);
                                break;
                        }
                        break;
                    case 4:
                        switch (value[2]) {
                            case 1:
                                mapbb.put(DeviceKey.type, DeviceKey.FindYourPhone);
                                break;
                            case 2:
                                map.put(DeviceKey.type, DeviceKey.Cancle_FindPhone);
                                break;
                        }
                        break;
                    case ((byte) 0XFE):
                        mapbb.put(DeviceKey.type, DeviceKey.SOS);
                        break;
                }
                mapdd.put(DeviceKey.Data, mapbb);
                mapdd.put(DeviceKey.DataType, BleConst.DeviceSendDataToAPP);
                mapdd.put(DeviceKey.End, true);
                dataListener.dataCallback(mapdd);
                break;
            case DeviceConst.Exit_photo_mode:
                Map<String,Object> aaaa=new HashMap<>();
                aaaa.put(DeviceKey.DataType, BleConst. BackHomeView);
                aaaa.put(DeviceKey.End, true);
                aaaa.put(DeviceKey.Data,new HashMap<>());
                dataListener.dataCallback(aaaa);
                break;
            case DeviceConst.CMD_ECGQuality:
                Map<String,Object> bn=new HashMap<>();
                Map<String, String> ccc = new HashMap<>();
                ccc.put(DeviceKey.heartValue,getValue(value[1], 0)+"");
                ccc.put(DeviceKey.hrvValue, getValue(value[2], 0)+"");
                ccc.put(DeviceKey.Quality, getValue(value[3], 0)+"");
                bn.put(DeviceKey.DataType,BleConst. EcgppG);
                bn.put(DeviceKey.End, true);
                bn.put(DeviceKey.Data, ccc);
                dataListener.dataCallback(bn);
                break;

            case DeviceConst.CMD_ECGDATA:
            case DeviceConst.CMD_PPGGDATA:
                dataListener.dataCallback(value);
                break;
            case DeviceConst. GetEcgPpgStatus:
                dataListener.dataCallback(ResolveUtil.GetEcgPpgStatus(value));
                break;
            case DeviceConst.Openecg:
                if(ecgopen){
                    if(16<=value.length){
                        dataListener.dataCallback(ResolveUtil.ecgData(value));
                    }
                }else{
                    Map<String,Object> ffff=new HashMap<>();
                    ffff.put(DeviceKey.DataType, BleConst.ECG);
                    ffff.put(DeviceKey.End, true);
                    ffff.put(DeviceKey.Data,new HashMap<>());
                    dataListener.dataCallback(ffff);
                }
                break;
            case DeviceConst.Closeecg:
                Map<String,Object> ffff=new HashMap<>();
                ffff.put(DeviceKey.DataType, BleConst.CloseECG);
                ffff.put(DeviceKey.End, true);
                ffff.put(DeviceKey.Data,new HashMap<>());
                dataListener.dataCallback(ffff);
                break;
            case DeviceConst.Weather:
                Map<String,Object> fffff=new HashMap<>();
                fffff.put(DeviceKey.DataType, BleConst.Weather);
                fffff.put(DeviceKey.End, true);
                fffff.put(DeviceKey.Data,new HashMap<>());
                dataListener.dataCallback(fffff);
                break;
            case DeviceConst.Braceletdial:
                if(isruning){
                    Map<String,Object> lll=new HashMap<>();
                    lll.put(DeviceKey.DataType, BleConst.Braceletdial);
                    lll.put(DeviceKey.End, true);
                    lll.put(DeviceKey.Data,new HashMap<>());
                    dataListener.dataCallback(lll);
                }else{
                    Map<String,Object> lll=new HashMap<>();
                    Map<String, String> lm = new HashMap<>();
                    lm.put(DeviceKey.index,value[1]+"");
                    lll.put(DeviceKey.DataType, BleConst.Braceletdialok);
                    lll.put(DeviceKey.End, true);
                    lll.put(DeviceKey.Data, lm);
                    dataListener.dataCallback(lll);
                }
                break;

            case DeviceConst.SportMode:
                Map<String,Object> baba=new HashMap<>();
                baba.put(DeviceKey.DataType, BleConst.SportMode);
                baba.put(DeviceKey.End, true);
                baba.put(DeviceKey.Data,new HashMap<>());
                dataListener.dataCallback(baba);
                break;
            case DeviceConst.GetSportMode:
                Map<String,Object> nnn=new HashMap<>();
                nnn.put(DeviceKey.DataType, BleConst.GetSportMode);
                nnn.put(DeviceKey.End, true);
                StringBuffer workOutType=new StringBuffer();
                int length=getValue(value[1],0);
                for(int i=0;i<length;i++){//workoutType里没有呼吸模式选项
                    int selected=getValue(value[i+2],0);
                    workOutType.append(selected).append(",");
                }
                nnn.put(DeviceKey.Data, workOutType.toString());
                dataListener.dataCallback(nnn);
                break;

            case DeviceConst.MeasurementWithType:
                if(StartDeviceMeasurementWithType){
                    Map<String,Object> vv=new HashMap<>();
                    switch (value[1]){
                        case 1://hrv
                            vv.put(DeviceKey.DataType, BleConst.MeasurementHrvCallback);
                            vv.put(DeviceKey.End, true);
                            Map<String, String> lm = new HashMap<>();
                            lm.put(DeviceKey.HeartRate,getValue(value[2], 0)+"");
                            lm.put(DeviceKey.Blood_oxygen,getValue(value[3], 0)+"");
                            lm.put(DeviceKey.HRV,getValue(value[4], 0)+"");
                            lm.put(DeviceKey.Stress,getValue(value[5], 0)+"");
                            lm.put(DeviceKey.HighPressure,getValue(value[6], 0)+"");
                            lm.put(DeviceKey.LowPressure,getValue(value[7], 0)+"");
                            vv.put(DeviceKey.Data,lm);
                            dataListener.dataCallback(vv);
                            break;
                        case 2://heart
                            vv.put(DeviceKey.DataType, BleConst.MeasurementHeartCallback);
                            vv.put(DeviceKey.End, true);
                            Map<String, String> lmB = new HashMap<>();
                            lmB.put(DeviceKey.HeartRate,getValue(value[2], 0)+"");
                            lmB.put(DeviceKey.Blood_oxygen,getValue(value[3], 0)+"");
                            lmB.put(DeviceKey.HRV,getValue(value[4], 0)+"");
                            lmB.put(DeviceKey.Stress,getValue(value[5], 0)+"");
                            lmB.put(DeviceKey.HighPressure,getValue(value[6], 0)+"");
                            lmB.put(DeviceKey.LowPressure,getValue(value[7], 0)+"");
                            vv.put(DeviceKey.Data,lmB);
                           dataListener.dataCallback(vv);
                            break;
                        case 3://0xy
                            vv.put(DeviceKey.DataType, BleConst.MeasurementOxygenCallback);
                            vv.put(DeviceKey.End, true);
                            Map<String, String> lmC = new HashMap<>();
                            lmC.put(DeviceKey.HeartRate,getValue(value[2], 0)+"");
                            lmC.put(DeviceKey.Blood_oxygen,getValue(value[3], 0)+"");
                            lmC.put(DeviceKey.HRV,getValue(value[4], 0)+"");
                            lmC.put(DeviceKey.Stress,getValue(value[5], 0)+"");
                            lmC.put(DeviceKey.HighPressure,getValue(value[6], 0)+"");
                            lmC.put(DeviceKey.LowPressure,getValue(value[7], 0)+"");
                            vv.put(DeviceKey.Data,lmC);
                            dataListener.dataCallback(vv);
                            break;
                    }
                }else{
                    Map<String,Object> vv=new HashMap<>();
                    switch (value[1]){
                        case 1://hrv
                            vv.put(DeviceKey.DataType, BleConst.StopMeasurementHrvCallback);
                            vv.put(DeviceKey.End, true);
                            vv.put(DeviceKey.Data,new HashMap<>());
                            dataListener.dataCallback(vv);
                            break;
                        case 2://heart
                            vv.put(DeviceKey.DataType, BleConst.StopMeasurementHeartCallback);
                            vv.put(DeviceKey.End, true);
                            vv.put(DeviceKey.Data,new HashMap<>());
                            dataListener.dataCallback(vv);
                            break;
                        case 3://0xy
                            vv.put(DeviceKey.DataType, BleConst.StopMeasurementOxygenCallback);
                            vv.put(DeviceKey.End, true);
                            vv.put(DeviceKey.Data,new HashMap<>());
                            dataListener.dataCallback(vv);
                            break;
                    }
                }
                break;

       /*     case DeviceConst.CMD_Get_WorkOutReminder:
            String[] workOutReminder = ResolveUtil.getWorkOutReminder(value);
            map.put("StartHour",workOutReminder[0]);
            map.put("StartMinter",workOutReminder[1]);
            map.put("WorkOutDay",workOutReminder[2]);
            map.put("SportWeekEnable",workOutReminder[3]);
            map.put("WorkOutReminderEnable",workOutReminder[4]);
            map.put("WorkOutReminderTimer",workOutReminder[5]);
                Map<String,Object> vvV=new HashMap<>();
                vvV.put(DeviceKey.DataType, BleConst.CMD_Get_WorkOutReminder);
                vvV.put(DeviceKey.End, true);
                vvV.put(DeviceKey.Data,map);
                dataListener.dataCallback(vvV);
                break;*/
/*            case DeviceConst.ReadSerialNumber:
                Map<String,Object> jjjjg=new HashMap<>();
                jjjjg.put(DeviceKey.DataType, BleConst.ReadSerialNumber);
                jjjjg.put(DeviceKey.End, true);
                StringBuilder sb = new StringBuilder();
                for (int i = 1; i < 8; i++) {
                    sb.append(String.format("%02X", value[i]));
                }
                jjjjg.put(DeviceKey.Data,sb.toString());
                dataListener.dataCallback(jjjjg);
                break;*/
            case DeviceConst.GPSControlCommand:
                Map<String,Object> GPSControlCommand=new HashMap<>();
                GPSControlCommand.put(DeviceKey.DataType, BleConst.GPSControlCommand);
                GPSControlCommand.put(DeviceKey.End, true);

                Map<String, String> hashMap = new HashMap<>();
                String date = "20"+bcd2String(value[1]) + "-"
                        + bcd2String(value[2]) + "-" + bcd2String(value[3]) + " "
                        + bcd2String(value[4]) + ":" + bcd2String(value[5]) + ":" + bcd2String(value[6]);
                byte[] valueLatitude = new byte[4];
                byte[] valueLongitude = new byte[4];
                for (int j = 0; j < 4; j++) {
                    valueLatitude[3 - j] = value[9 + j];
                    valueLongitude[3 - j] = value[14 + j];
                }
                String Latitude = String.valueOf(getFloat(valueLatitude, 0));
                String Longitude = String.valueOf(getFloat(valueLongitude, 0));
                int count = getValue(value[18], 0);
                hashMap.put(DeviceKey.KActivityLocationTime, date);
                hashMap.put(DeviceKey.KActivityLocationLatitude, Latitude);
                hashMap.put(DeviceKey.KActivityLocationLongitude, Longitude);
                hashMap.put(DeviceKey.KActivityLocationCount, String.valueOf(count));
                GPSControlCommand.put(DeviceKey.Data,hashMap.toString());
                dataListener.dataCallback(GPSControlCommand);
                break;

            case DeviceConst.CMD_Get_GPSDATA:
                dataListener.dataCallback(ResolveUtil.getHistoryGpsData(value));

                break;

            case DeviceConst.Clear_Bracelet_data:
                Map<String,Object> language=new HashMap<>();
                language.put(DeviceKey.DataType, BleConst.Clear_Bracelet_data);
                language.put(DeviceKey.End, true);
                language.put(DeviceKey.Data,new HashMap<>());
                dataListener.dataCallback(language);
                break;
            case DeviceConst.CMD_Get_Blood_oxygen:
                if(GetBloodOxygen){
                    dataListener.dataCallback(ResolveUtil.setMethodSuccessful(BleConst.Delete_Blood_oxygen));
                }else{
                    dataListener.dataCallback(ResolveUtil.getBloodoxygen(value));
                }

                break;
            case  DeviceConst.CMD_SET_SOCIAL:
                if(isSettingSocial){
                    dataListener.dataCallback(ResolveUtil.setMethodSuccessful(BleConst.SocialdistanceSetting));
                }else{
                    Map<String, Object> mapsa = new HashMap<>();
                    mapsa.put(DeviceKey.DataType, BleConst.SocialdistanceGetting);
                    mapsa.put(DeviceKey.End, true);
                    Map<String, String> mapll = new HashMap<>();
                    int interval = ResolveUtil.getValue(value[2], 0);
                    int duration = ResolveUtil.getValue(value[3], 0);
                    mapll.put(DeviceKey.scanInterval, interval+"");
                    mapll.put(DeviceKey.scanTime, duration+"");
                    mapll.put(DeviceKey.signalStrength,value[4]+"");
                    mapsa.put(DeviceKey.Data, mapll);
                    dataListener.dataCallback(mapsa);
                }
                break;
            case  DeviceConst.Sos:
                    dataListener.dataCallback(ResolveUtil.setMethodSuccessful(BleConst.Sos));
                break;
            case  DeviceConst.Temperature_history:
                if(GetTemperature_historyDataWithMode){
                    dataListener.dataCallback(ResolveUtil.setMethodSuccessful(BleConst.deleteGetTemperature_historyDataWithMode));
                }else{
                    dataListener.dataCallback(ResolveUtil.getTempData(value));
                }
                break;
            case  DeviceConst.GetAxillaryTemperatureDataWithMode:
                if(GetAxillaryTemperatureDataWithMode){
                    dataListener.dataCallback(ResolveUtil.setMethodSuccessful(BleConst.deleteGetAxillaryTemperatureDataWithMode));

                }else{
                    dataListener.dataCallback(ResolveUtil.getTempDataer(value));
                }

                break;
            case  DeviceConst. Get3D:
                dataListener.dataCallback(ResolveUtil.get3d(value));
                break;
            case  DeviceConst. PPG:
                dataListener.dataCallback(ResolveUtil.getPPG(value));
                break;
            case  DeviceConst.Qrcode:
                if(Qrcode){
                    Map<String,Object> qr=new HashMap<>();
                    qr.put(DeviceKey.DataType, BleConst.EnterQRcode);
                    qr.put(DeviceKey.End, true);
                    qr.put(DeviceKey.Data, new HashMap<>());
                    dataListener.dataCallback(qr);
                }else{
                    if((byte) 0x80==value[1]||(byte)0x81==value[1]){
                        Map<String,Object> qr=new HashMap<>();
                        qr.put(DeviceKey.DataType, BleConst.QRcodebandBack);
                        qr.put(DeviceKey.End, true);
                        Map<String, String> mapll = new HashMap<>();
                        mapll.put(DeviceKey.Band,((byte)0x81==value[1])?1+"":0+"");
                        qr.put(DeviceKey.Data, mapll);
                        dataListener.dataCallback(qr);
                    }else{
                        Map<String,Object> qr=new HashMap<>();
                        qr.put(DeviceKey.DataType, BleConst.ExitQRcode);
                        qr.put(DeviceKey.End, true);
                        qr.put(DeviceKey.Data, new HashMap<>());
                        dataListener.dataCallback(qr);

                    }

                }
                break;
            case DeviceConst.GetAutomaticSpo2Monitoring:
                if(Obtain_The_data_of_manual_blood_oxygen_test){
                    dataListener.dataCallback(ResolveUtil.setMethodSuccessful(BleConst.Delete_Obtain_The_data_of_manual_blood_oxygen_test));
                }else{
                    dataListener.dataCallback(ResolveUtil.GetAutomaticSpo2Monitoring(value));
                }
                break;
           case  DeviceConst. GetECGwaveform:
               dataListener.dataCallback(ResolveUtil.getEcgHistoryData(value));
                break;
            case DeviceConst.spo2:
                if(issetting){
                    dataListener.dataCallback(ResolveUtil.setMethodSuccessful(BleConst.SetSpo2));
                }else{
                    dataListener.dataCallback(ResolveUtil.getSpo2(value));
                }
                break;
        }
        //  return map;
    }


    public static byte[] GetDetailSleepDataWithMode(byte mode, String dateOfLastData) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_SleepData;
        value[1] = mode;
        insertDateValue(value, dateOfLastData);
        crcValue(value);
        return value;
    }


    public static byte[] GetGpsData(int index) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_GPSDATA;
        value[1] = (byte) index;
        crcValue(value);
        return value;
    }
    public static byte[] ClearBraceletData() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.Clear_Bracelet_data;
        crcValue(value);
        return value;
    }

    public static byte[] Sos() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.Sos;
        crcValue(value);
        return value;
    }

    public static byte[] MotorVibrationWithTimes(int times) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_MOT_SIGN;
        value[1] = (byte) times;
        crcValue(value);
        return value;
    }

    public static byte[] setStepModel(StepModel stepModel) {
        byte[] value = new byte[16];
        //  value = stepModel.isStepState() ? RealTimeStep() : stopGo();
        return value;
    }


    /**
     * 开始实时计步
     *
     * @return
     */
    public static byte[] RealTimeStep(boolean enable,boolean tempEnable) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Enable_Activity;
        value[1] = (byte) (enable ? 1 : 0);
        value[2] = (byte) (tempEnable ? 1 : 0);
        crcValue(value);
        return value;
    }


    /**
     * 停止实时计步
     *
     * @return
     */
    public static byte[] stopGo() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Enable_Activity;
        value[1] = 0;
        crcValue(value);
        return value;
    }



    /**
     * 设置个人信息
     *
     * @param
     * @return
     */
    public static byte[] SetPersonalInfo(MyPersonalInfo info) {
        byte[] value = new byte[16];
        int male = info.getSex();
        int age = info.getAge();
        int height = info.getHeight();
        int weight = info.getWeight();
        int stepLength = info.getStepLength();
        value[0] = DeviceConst.CMD_Set_UseInfo;
        value[1] = (byte) male;
        value[2] = (byte) age;
        value[3] = (byte) height;
        value[4] = (byte) weight;
        value[5] = (byte) stepLength;
        crcValue(value);

        return value;
    }


    /**
     * 获取个人信息
     *
     * @return
     */
    public static byte[] GetPersonalInfo() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_GET_USERINFO;
        crcValue(value);
        return value;
    }


    /**
     * 设置设备时间
     *
     * @param
     * @return
     */
    public static byte[] SetDeviceTime(MyDeviceTime time) {
        byte[] value = new byte[16];
        String timeZone = ResolveUtil.getCurrentTimeZone();
        String zone = timeZone.substring(3);
        byte zoneValue = 0;
        if (zone.contains("-")) {
            zone = zone.replace("-", "");
            zoneValue = Byte.valueOf(String.valueOf(zone));
        } else {
            zoneValue = (byte) (Byte.valueOf(zone) + 0x80);
        }
        int year = time.getYear();
        int month = time.getMonth();
        int day = time.getDay();
        int hour = time.getHour();
        int min = time.getMinute();
        int second = time.getSecond();
        value[0] = DeviceConst.CMD_SET_TIME;
        value[1] = ResolveUtil.getTimeValue(year);
        value[2] = ResolveUtil.getTimeValue(month);
        value[3] = ResolveUtil.getTimeValue(day);
        value[4] = ResolveUtil.getTimeValue(hour);
        value[5] = ResolveUtil.getTimeValue(min);
        value[6] = ResolveUtil.getTimeValue(second);
        value[8] = zoneValue;
        crcValue(value);

        return value;
    }


    /**
     * 获取设备时间
     *
     * @return
     */
    public static byte[] GetDeviceTime() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_GET_TIME;
        crcValue(value);
        return value;
    }


    /**
     * 99: 删除步数详细数据 ,
     * 0:读最近的步数详细数据。
     * 1：读指定位置的步数详细数据。
     * 2：继续上次读的位置下一段数据
     *
     * @param
     * @return
     *  * dateOfLastData "yyyy-MM-dd HH:mm:ss   or  yyyy.MM.dd HH:mm:ss"
     */
    public static byte[] GetDetailActivityDataWithMode(byte mode, String dateOfLastData) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_DetailData;
        value[1] = (byte) mode;
        insertDateValue(value,dateOfLastData);
        crcValue(value);
        return value;
    }
    /**
     * 99: 删除温度数据 ,
     * 0:读最近的温度详细数据。
     * 1：读指定位置的步数详细数据。
     * 2：继续上次读的位置下一段数据
     *
     * @param
     * @return
     */
    protected  static boolean GetTemperature_historyDataWithMode;
    public static byte[] GetTemperature_historyDataWithMode(byte mode, String dateOfLastData) {
        GetTemperature_historyDataWithMode=(byte)0x99==mode;
        byte[] value = new byte[16];
        value[0] = DeviceConst.Temperature_history;
         value[1] = mode;
        insertDateValue(value, dateOfLastData);
        crcValue(value);
        return value;
    }

    /**
     * 99: 删除温度数据 ,
     * 0:读最近的温度详细数据。
     * 1：读指定位置的步数详细数据。
     * 2：继续上次读的位置下一段数据
     *
     * @param
     * @return
     */
    private static boolean GetAxillaryTemperatureDataWithMode;
    public static byte[] GetAxillaryTemperatureDataWithMode(byte mode, String dateOfLastData) {
        GetAxillaryTemperatureDataWithMode= (byte) 0x99 == mode;
        byte[] value = new byte[16];
        value[0] = DeviceConst.GetAxillaryTemperatureDataWithMode;
        value[1] = mode;
        insertDateValue(value, dateOfLastData);
        crcValue(value);
        return value;
    }

    public static byte[] SetAutomatic(boolean open, int time, AutoMode type) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_Auto;
        value[1] = open ? (byte) 0x02 : (byte) 0x00;
        value[2] = (byte) 0x00;
        value[3] = (byte) 0x00;
        value[4] = (byte) 0x23;
        value[5] = (byte) 0x59;
        value[6] = (byte) 255;
        value[7] = (byte) (time & 0xff);
        value[8] = (byte) ((time >> 8) & 0xff);
        if(null==type){
            value[9] = (byte) 0x01;
        }else{
            switch (type){
                case AutoHeartRate:
                    value[9] = (byte) 0x01;
                    break;
                case AutoSpo2:
                    value[9] = (byte) 0x02;
                    break;
                case AutoTemp:
                    value[9] = (byte) 0x03;
                    break;
                case AutoHrv:
                    value[9] = (byte) 0x04;
                    break;
            }
        }

        crcValue(value);
        return value;
    }
    public static byte[] GetAutomatic(AutoMode type) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_Auto;
        if(null==type){
            value[1] = (byte) 0x01;
        }else{
            switch (type){
                case AutoHeartRate:
                    value[1] = (byte) 0x01;
                    break;
                case AutoSpo2:
                    value[1] = (byte) 0x02;
                    break;
                case AutoTemp:
                    value[1] = (byte) 0x03;
                    break;
                case AutoHrv:
                    value[1] = (byte) 0x04;
                    break;
            }
        }
        crcValue(value);
        return value;
    }



    /**
     * 获取某天总数据
     * 0
     *
     * @param mode 0:表⽰是从最新的位置开始读取(最多50组数据)  2:表⽰接着读取(当数据总数⼤于50的时候) 0x99:表⽰删除所有运
     *             动数据
     * @return
     */
  protected  static boolean GetTotalActivityDataWithMode;
    public static byte[] GetTotalActivityDataWithMode(byte mode, String dateOfLastData) {
        GetTotalActivityDataWithMode= (byte) 0x99 == mode;
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_TotalData;
        value[1] = mode;
        insertDateValueNoH(value, dateOfLastData);
        crcValue(value);
        return value;
    }

    public static byte[] enableEcgPPg(int level){
        byte[] value = new byte[16];
        value[0]=DeviceConst.Openecg;
        value[1]= (byte) level;
        crcValue(value);
        return value;
    }


   protected static boolean ecgopen=false;
    public static byte[] enableEcg(boolean open) {
        ecgopen=open;
        byte[] value = new byte[16];
        value[0] = DeviceConst.Openecg;
        value[1] = open?(byte)0x01:(byte)0x00 ;
         crcValue(value);
        return value;
    }



    public static byte[] enableEcgPPg(int level, int time) {
            byte[] value = new byte[16];
            value[0] = DeviceConst.Openecg;
            value[1] = (byte) level;
            value[3] = (byte) ((time) & 0xff);
            value[4] = (byte) ((time >> 8) & 0xff);
            crcValue(value);
            return value;
        }

    public static byte[] stopEcgPPg(){
        byte[] value = new byte[16];
        value[0]=DeviceConst.Closeecg;
        crcValue(value);
        return value;
    }

    public static byte[] GetEcgPpgStatus(){
        byte[] value = new byte[16];
        value[0]=DeviceConst.GetEcgPpgStatus;
        crcValue(value);
        return value;
    }

    /**
     * 进入dfu模式
     *
     * @return
     */
    public static byte[] enterOTA() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Start_Ota;
        crcValue(value);
        return value;

    }


    /**
     * 获取设备版本号
     *
     * @return
     */
    public static byte[] GetDeviceVersion() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_Version;
        crcValue(value);
        return value;
    }


    /**
     * 读取闹钟 99删除闹钟，0读取闹钟
     *
     * @param
     * @return
     */
    protected static boolean GetAlarmClock;
    public static byte[] GetAlarmClock(byte mode) {
        GetAlarmClock= (byte) 0x99 == mode;
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_Clock;
        value[1] = mode;
        crcValue(value);
        return value;
    }


    /**
     * 设置闹钟
     *
     * @param clock
     * @return
     */
    public static byte[] SetAlarmClockWithAllClock(Clock clock) {
        byte[] value = new byte[37];
        String content = clock.getContent();
        byte[] infoValue = getInfoValue(content, 30);
        value[0] = DeviceConst.CMD_Set_Clock;
        value[1] = (byte) clock.getNumber();
        value[2] = (byte) clock.getType();
        value[3] = ResolveUtil.getTimeValue(clock.getHour());
        value[4] = ResolveUtil.getTimeValue(clock.getMinute());
        value[5] = (byte) clock.getWeek();
        value[6] = (byte) infoValue.length;
        System.arraycopy(infoValue, 0, value, 7, infoValue.length);
        return value;

    }


    /**
     * 设置运动提醒
     *
     * @param activityAlarm
     * @return
     */
    public static byte[] SetSedentaryReminder(MySedentaryReminder activityAlarm) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_ActivityAlarm;
        value[1] = ResolveUtil.getTimeValue(activityAlarm.getStartHour());
        value[2] = ResolveUtil.getTimeValue(activityAlarm.getStartMinute());
        value[3] = ResolveUtil.getTimeValue(activityAlarm.getEndHour());
        value[4] = ResolveUtil.getTimeValue(activityAlarm.getEndMinute());
        value[5] = (byte) activityAlarm.getWeek();
        value[6] = (byte) activityAlarm.getIntervalTime();
        value[7] = (byte) (activityAlarm.getLeastStep() & 0xff);
        value[8] = (byte) (activityAlarm.isEnable() ? 1 : 0);
        crcValue(value);
        return value;
    }


    /**
     * 获取运动提醒
     *
     * @return
     */
    public static byte[] GetSedentaryReminder() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_ActivityAlarm;
        crcValue(value);
        return value;
    }


    /**
     * 恢复出厂设置
     *
     * @return
     */
    public static byte[] Reset() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Reset;
        crcValue(value);
        return value;
    }


    /**
     * 获取设备mac地址
     *
     * @return
     */
    public static byte[] GetDeviceMacAddress() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_Address;
        crcValue(value);
        return value;
    }


    /**
     * 获取设备电量
     *
     * @return
     */
    public static byte[] GetDeviceBatteryLevel() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_BatteryLevel;
        crcValue(value);
        return value;
    }


    /**
     * 重启设备
     *
     * @return
     */
    public static byte[] MCUReset() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Mcu_Reset;
        crcValue(value);
        return value;
    }


    /**
     * 设置设备名字
     *
     * @param
     * @return
     */
    public static byte[] SetDeviceName(String strDeviceName) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_Name;
        int length = strDeviceName.length() > 14 ? 14 : strDeviceName.length();
        for (int i = 0; i < length; i++) {
            value[i + 1] = (byte) strDeviceName.charAt(i);
        }
        crcValue(value);
        return value;
    }


    /**
     * 获取设备名字
     *
     * @return
     */
    public static byte[] GetDeviceName() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_Name;
        crcValue(value);
        return value;
    }


    /**
     * 设置距离单位
     *
     * @return
     */
    public static byte[] SetDistanceUnit(boolean km) {
        isSetDialinterface=false;
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_DeviceInfo;
        value[1] =  km?(byte)0x80:(byte)0x81;
        crcValue(value);
        return value;
    }

    /**
     * 设置时间模式
     *
     * @param unit12
     * @return
     */
    public static byte[] SetTimeModeUnit(boolean unit12) {
        isSetDialinterface=false;
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_DeviceInfo;
        value[2] = unit12?(byte)0x81:(byte)0x80;
        crcValue(value);
        return value;
    }
    /**
     * 抬手亮屏
     * @param enable
     * @return
     */
    public static byte[] setWristOnEnable(boolean enable) {
        isSetDialinterface=false;
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_DeviceInfo;
        value[3] = enable?WristOn_Enable:WristOn_DisEnable;
        crcValue(value);
        return value;
    }

    /**
     * 华氏度摄氏度切换
     * @param Fahrenheit_degree
     * @return
     */
    public static byte[] setTemperatureUnit(boolean Fahrenheit_degree) {
        isSetDialinterface=false;
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_DeviceInfo;
        value[4] = Fahrenheit_degree?WristOn_Enable:WristOn_DisEnable;
        crcValue(value);
        return value;
    }
    /**
     * 是否开启夜间模式
     * @param
     * @return
     */
    public static byte[] setLightMode(boolean LightMode) {
        isSetDialinterface=false;
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_DeviceInfo;
        value[5] = LightMode?WristOn_Enable:WristOn_DisEnable;
        crcValue(value);
        return value;
    }

    public static byte[] disableAncs() {
        isSetDialinterface=false;
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_DeviceInfo;
        value[6] = (byte) 0x80;
        crcValue(value);
        return value;
    }



    private  static final byte[] BrightnessLevel={(byte)0x8f, (byte)0x8d, (byte)0x8b,(byte)0x89, (byte)0x87, (byte)0x85};
    public static byte[] SetBrightness(int level) {
        isSetDialinterface=false;
        if(level<0){level=0;}else if(level>5){level=5;}
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_DeviceInfo;
        value[11] = BrightnessLevel[level];
        crcValue(value);
        return value;
    }


    static boolean isSetDialinterface=false;
    public static byte[] SetDialinterface(int index) {
        isSetDialinterface=true;
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_DeviceInfo;
        value[12] =(byte)( 0x80+index);
        crcValue(value);
        return value;
    }


    /**
     * 语言（中英文）切换
     * @param
     * @return
     */
    public static byte[] setLauage(boolean chaina) {
        isSetDialinterface=false;
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_DeviceInfo;
        value[14] = chaina?WristOn_Enable:WristOn_DisEnable;
        crcValue(value);
        return value;
    }

    public static byte[] SetDeviceInfo(MyDeviceInfo deviceBaseParameter) {
        isSetDialinterface=false;
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_DeviceInfo;
        value[1] = (byte) (deviceBaseParameter.isDistanceUnit() ? 0x81 : 0x80);
        value[2] = (byte) (deviceBaseParameter.isIs12Hour() ? 0x81 : 0x80);
        value[3] = (byte) (deviceBaseParameter.isBright_screen() ? 0x81 : 0x80);
        value[4] = (byte) (!deviceBaseParameter.isTemperature_unit() ? 0x81 : 0x80);
        value[5] = (byte) (deviceBaseParameter.isFahrenheit_or_centigrade() ? 0x81 : 0x80);
        value[6] = (byte )0x80;
        value[9] =(byte)  deviceBaseParameter.getBaseheart();
        if(-1!=deviceBaseParameter.getScreenBrightness()){
            if(deviceBaseParameter.getScreenBrightness()<0){deviceBaseParameter.setScreenBrightness(0);}else if(deviceBaseParameter.getScreenBrightness()>5){deviceBaseParameter.setScreenBrightness(5);}
            value[11] = BrightnessLevel[deviceBaseParameter.getScreenBrightness()];
        }
        if(-1!=deviceBaseParameter.getDialinterface()){
            value[12] =(byte) (0x80 + deviceBaseParameter.getDialinterface());
        }
        value[14] =(byte) (deviceBaseParameter.isChinese_English_switch() ? 0x81 : 0x80);
        crcValue(value);
        return value;
    }










    /**
     */
    public static byte[] SetMusicStatus(Boolean start) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.Enter_photo_modeback;
        value[1]=(byte)0x03;
        value[2] = start?(byte)0x04:(byte)0x05;
        crcValue(value);
        return value;
    }

    /*
     *  @method StartDeviceMeasurementWithType
     *   @param dataType    The type of measurement that needs to be turned on
     *   @param isOpen   When its value is YES, it means on, otherwise it means off
     *  @discussion  Turn on device measurement
     *
     */
    public static boolean StartDeviceMeasurementWithType=false;
    public static byte[] StartDeviceMeasurementWithType(int dataType,boolean open) {
        StartDeviceMeasurementWithType=open;
        byte[] value = new byte[16];
        value[0] = DeviceConst.MeasurementWithType;
        value[1]=(byte)dataType;
        value[2] = open?(byte)0x01:(byte)0x00;
        crcValue(value);
        return value;
    }




    /**
     */
    public static byte[] SendMusicname(String name) {
        byte[] titleValue =TextUtils.isEmpty(name)?new byte[1]: getInfoValue(name, 12);
        byte[] value = new byte[16];
        value[0] = DeviceConst.Enter_photo_modeback;
        value[1] = (byte)0x03;
        value[2] = (byte) 0xFE;
        System.arraycopy(titleValue, 0, value, 3, titleValue.length);
        crcValue(value);
        return value;
    }


    public static byte[] setWeather(WeatherData weatherData){
        String name = weatherData.getCityName();
        byte[] value = new byte[42];
        value[0] = DeviceConst.Weather;
        if(!TextUtils.isEmpty(name)){
            value[1] = (byte) weatherData.getWeatherId();
            int tempNow = weatherData.getTempNow();
            value[2] = (byte) (tempNow );
            int tempHigh = weatherData.getTempHigh();
            value[3] = (byte) (tempHigh);
            int templow = weatherData.getTempLow();
            value[4] = (byte) (templow);
            byte[] valueName = getInfoValue(name, 32);
            value[7]= (byte) valueName.length;
            System.arraycopy(valueName, 0, value, 8, valueName.length);
            crcValue(value);
        }
        return value;
    }



    public static byte[] SetBraceletdial(int mode) {
        isruning=true;
        byte[] value = new byte[16];
        value[0] = DeviceConst.Braceletdial;
        value[1] = (byte) mode;
        crcValue(value);
        return value;
    }


    public static byte[] GetBraceletdial() {
        isruning=false;
        byte[] value = new byte[16];
        value[0] = DeviceConst.Braceletdial;
        value[1] = (byte)0x00;
        value[2] = (byte)0x01;
        crcValue(value);
        return value;
    }

  private static  boolean readhrv=false;
    public static byte[] GetHRVDataWithMode(byte mode, String dateOfLastData) {
        readhrv=(byte) 0x99!=mode;
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_HrvTestData;
        value[1] = mode;
        insertDateValue(value, dateOfLastData);
        crcValue(value);
        return value;
    }

    private static boolean isSettingSocial=false;
    public static byte[] setSocialSetting(int Interval,int duration,short rssi){
        isSettingSocial=true;
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_SET_SOCIAL;
        value[1] =  1;//1设置，0读取
        value[2]= (byte) Interval;
        value[3]= (byte) duration;
        value[4]= (byte) rssi;
        crcValue(value);
        return value;
    }

    public static byte[] getSocialSetting(){
        isSettingSocial=false;
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_SET_SOCIAL;
        value[1] =  0;//1设置，0读取
        crcValue(value);
        return value;
    }



   /* public static byte[] SetBaseHeartRate(int BaseHeartRate) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_BaseHeartRateVale;
        value[1] = (byte) BaseHeartRate;
        crcValue(value);
        return value;
    }


    public static byte[] GetBaseHeartRate() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_BaseHeartRateVale;
        crcValue(value);
        return value;
    }*/


/*    public static byte[] SetAutomaticHRMonitoring(MyAutomaticHRMonitoring autoHeart) {
        byte[] value = new byte[16];
        int time = autoHeart.getTime();
        value[0] = DeviceConst.CMD_Set_AutoHeart;
        value[1] = (byte) autoHeart.getOpen();
        value[2] = ResolveUtil.getTimeValue(autoHeart.getStartHour());
        value[3] = ResolveUtil.getTimeValue(autoHeart.getStartMinute());
        value[4] = ResolveUtil.getTimeValue(autoHeart.getEndHour());
        value[5] = ResolveUtil.getTimeValue(autoHeart.getEndMinute());
        value[6] = (byte) autoHeart.getWeek();
        value[7] = (byte) (time & 0xff);
        value[8] = (byte) ((time >> 8) & 0xff);
        crcValue(value);
        return value;
    }*/



    /*public static byte[] ReadSerialNumber() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.ReadSerialNumber;
        crcValue(value);
        return value;
    }*/






   /* public static byte[] StartHeartRateMode() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Enable_ActivityHeartRate;
        value[1] = 1;
        crcValue(value);
        return value;
    }


    public static byte[] StopHeartRateMode() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Enable_ActivityHeartRate;
        value[1] = 0;
        crcValue(value);
        return value;
    }


    public static byte[] SetHeartRateDataMode(int DataMode) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_ActivityHeartRateMode;
        value[1] = (byte) DataMode;
        crcValue(value);
        return value;
    }*/





    /**
     * 获取多模式运动数据
     *
     * @param mode 0:表⽰是从最新的位置开始读取(最多50组数据)  2:表⽰接着读取(当数据总数⼤于50的时候) 0x99:表⽰删除所有
     *             GPS数据
     * @return
     */
    protected static boolean GetActivityModeDataWithMode;
    public static byte[] GetActivityModeDataWithMode(byte mode) {
        GetActivityModeDataWithMode=(byte)0x09==mode;
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_SPORTData;
        value[1] =  mode;
        crcValue(value);
        return value;
    }


    public static byte[] LanguageSwitching(boolean isEglish) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.LanguageSwitching;
        value[1] = (byte)0x01;
        value[2] = isEglish?(byte)0x00:(byte)0x01;
        crcValue(value);
        return value;
    }


    protected static boolean GetStaticHRWithMode;
    public static byte[] GetStaticHRWithMode(byte mode, String dateOfLastData) {
        GetStaticHRWithMode=(byte)0x99==mode;
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_OnceHeartData;
        value[1] =  mode;
        insertDateValue(value, dateOfLastData);
        crcValue(value);
        return value;
    }


    protected  static boolean GetDynamicHRWithMode;
    public static byte[] GetDynamicHRWithMode(byte Number, String dateOfLastData) {
        GetDynamicHRWithMode=(byte)0x99==Number;
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_HeartData;
        value[1] = (byte) Number;
        insertDateValue(value, dateOfLastData);
        crcValue(value);
        return value;
    }

    private static boolean GetBloodOxygen;
    public static byte[] GetBloodOxygen(byte Number,String dateOfLastData) {
        GetBloodOxygen=((byte) 0x99 == Number);
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_Blood_oxygen;
        value[1] = (byte) Number;
        insertDateValue(value, dateOfLastData);
        crcValue(value);
        return value;
    }

    public static byte[] SetStepGoal(int stepGoal) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_Goal;
        value[4] = (byte) ((stepGoal >> 24) & 0xff);
        value[3] = (byte) ((stepGoal >> 16) & 0xff);
        value[2] = (byte) ((stepGoal >> 8) & 0xff);
        value[1] = (byte) ((stepGoal) & 0xff);
        crcValue(value);
        return value;
    }


    public static byte[] GetStepGoal() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_Goal;
        crcValue(value);
        return value;
    }


    public static byte[] SetDeviceID(String deviceId) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_DeviceID;
        for (int i = 0; i < 6; i++) {
            value[i + 1] = (byte) deviceId.charAt(i);
        }
        crcValue(value);
        return value;
    }





    protected static boolean RealTimeThreeAxisSensorData=true;
    public static byte[] RealTimeThreeAxisSensorData(boolean open) {
        RealTimeThreeAxisSensorData=open;
        byte[] value = new byte[16];
        value[0] = DeviceConst.Get3D;
        value[1] = open?(byte)0x01:(byte)0x00;
        crcValue(value);
        return value;
    }

    public static byte[] GetPpgRawDataWithStatus(boolean open) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.PPG;
        value[1] = open?(byte)0x01:(byte)0x00;
        crcValue(value);
        return value;
    }

protected static boolean read;
    public static byte[] GetECGwaveform(boolean readdata,int  num) {
        read=readdata;
        byte[] value = new byte[16];
        value[0] = DeviceConst.GetECGwaveform;
        value[1] = readdata?(byte)0x00:(byte)0x99;
        value[2] = (byte)num;
        crcValue(value);
        return value;
    }

    public static byte[] GetECGwaveform(boolean readdata,int  num, String date) {
        read=readdata;
        byte[] value = new byte[16];
        value[0] = DeviceConst.GetECGwaveform;
        value[1] = readdata?(byte)0x00:(byte)0x99;
        value[2] = (byte)num;
        insertDateValue(value, date);
        crcValue(value);
        return value;
    }



   protected static  boolean Obtain_The_data_of_manual_blood_oxygen_test;
    public static byte[] Obtain_The_data_of_manual_blood_oxygen_test(byte Number) {
        Obtain_The_data_of_manual_blood_oxygen_test=((byte) 0x99 == Number);
        byte[] value = new byte[16];
        value[0] = DeviceConst.GetAutomaticSpo2Monitoring;
        value[1] = (byte) Number;
        crcValue(value);
        return value;
    }

    public static byte[] GetDeviceInfo() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Get_DeviceInfo;
        crcValue(value);
        return value;
    }

    public static byte[] SetAutomaticSpo2Monitoring(MyAutomaticHRMonitoring autoHeart) {
        issetting=true;
        byte[] value = new byte[16];
        int time = autoHeart.getTime();
        value[0] = DeviceConst.spo2;
        value[1] = (byte)0x01;
        value[2] = (byte)0x01;
        value[3] = ResolveUtil.getTimeValue(time);
        value[4] = ResolveUtil.getTimeValue(autoHeart.getStartHour());
        value[5] = ResolveUtil.getTimeValue(autoHeart.getStartMinute());
        value[6] = ResolveUtil.getTimeValue(autoHeart.getEndHour());
        value[7] = ResolveUtil.getTimeValue(autoHeart.getEndMinute());
        value[8] = (byte) autoHeart.getWeek();
        crcValue(value);
        return value;
    }

    public  static boolean issetting=false;
    public static byte[] GetAutomaticSpo2Monitoring() {
        issetting=false;
        byte[] value = new byte[16];
        value[0] = DeviceConst.spo2;
        crcValue(value);
        return value;
    }
    public static byte[] setNotifyData(Notifier sendData) {
        String info = sendData.getInfo();
        byte[] infoValue = TextUtils.isEmpty(info) ? new byte[1] : getInfoValue(info, 60);
        byte[] value = new byte[infoValue.length + 3];
        value[0] = DeviceConst.CMD_Notify;
        value[1] = (byte)sendData.getType() ;
        value[2] = (byte) infoValue.length;
        System.arraycopy(infoValue, 0, value, 3, infoValue.length);
        return value;
    }

    /**
     * 设置温度修正值
     *
     * @return
     */
    public static byte[] SetTemperatureCorrectionValue(int tempValue) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_TemperatureCorrection;
        value[1] = 1;
        byte[] tempArray = intTobyte(tempValue);
        value[2] = tempArray[1];
        value[3] = tempArray[0];

        crcValue(value);
        return value;
    }


    public static byte[] intTobyte(int num) {
        return new byte[]{(byte) ((num >> 8) & 0xff), (byte) (num & 0xff)};
    }

    public static int byteArrayToInt(byte[] arr) {
        short targets = (short) ((arr[1] & 0xff) | ((arr[0] << 8) & 0xff00));

        return targets;
    }
    /*!
     *  @method SetHeartbeaPackets
     *  @param heartbeaPacketsInterval   手环向APP发送心跳包的时间间隔，单位是分钟
     *  @discussion 设置手环向APP发送心跳包的时间间隔
     *
     */
    public static byte[] SetHeartbeatPackets(int heartbeatPacketsInterval) {
        byte[] value = new byte[16];
        value[0] = DeviceConst.CMD_Set_HeartbeatPackets;
        value[1] = (byte) heartbeatPacketsInterval;
        crcValue(value);
        return value;
    }

    public static byte[] EnterPhotoMode() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.Enter_photo_mode;
        crcValue(value);
        return value;
    }

    public static byte[] ExitPhotoMode() {
        byte[] value = new byte[16];
        value[0] = DeviceConst.Exit_photo_mode;
        crcValue(value);
        return value;
    }


    public static byte[] Float2ByteArray(float f) {
        int intbits = Float.floatToIntBits(f);//将float里面的二进制串解释为int整数
        return Float2ByteArray(intbits);
    }

    public static byte[] Float2ByteArray(int i) {
        byte[] b = new byte[4];
        b[3] = (byte) ((i & 0xff000000) >> 24);
        b[2] = (byte) ((i & 0x00ff0000) >> 16);
        b[1] = (byte) ((i & 0x0000ff00) >> 8);
        b[0] = (byte) (i & 0x000000ff);
        return b;
    }

    public static byte[] getInfoValue(String info, int maxLength) {
        byte[] nameBytes = null;
        try {
            nameBytes = info.getBytes("UTF-8");
            if (nameBytes.length >= maxLength) {//两条命令总共32个字节，内容只占24个字节（32-2*（1cmd+1消息类型+1长度+1校验））
                byte[] real = new byte[maxLength];
                char[] chars = info.toCharArray();
                int length = 0;
                for (int i = 0; i < chars.length; i++) {
                    String s = String.valueOf(chars[i]);
                    byte[] nameB = s.getBytes("UTF-8");
                    ;
                    if (length + nameB.length == maxLength) {
                        System.arraycopy(nameBytes, 0, real, 0, real.length);
                        return real;
                    } else if (length + nameB.length > maxLength) {//大于24会导致有个字节发不到下位机导致乱码
                        System.arraycopy(nameBytes, 0, real, 0, length);
                        return real;
                    }
                    length += nameB.length;
                }
            }
        } catch (UnsupportedEncodingException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        return nameBytes;
    }

    public static void crcValue(byte[] value) {
        byte crc = 0;
        for (int i = 0; i < value.length - 1; i++) {
            crc += value[i];
        }
        value[value.length - 1] = (byte) (crc & 0xff);
    }



    protected static boolean Qrcode=false;

    public static byte[] lockScreen() {
        Qrcode=true;
        byte[] value = new byte[16];
        value[0] = DeviceConst.Qrcode;
        value[1] = (byte) 0x80;
        crcValue(value);
        return value;
    }

    public static byte[]  unlockScreen() {
        Qrcode=false;
        byte[] value = new byte[16];
        value[0] = DeviceConst.Qrcode;
        value[1] =  (byte) 0x81;
        crcValue(value);
        return value;
    }



    public static byte[] EnterTheMainInterface() {
        Qrcode=true;
        byte[] value = new byte[16];
        value[0] = DeviceConst.Qrcode;
        value[1] = (byte) 0x82;
        crcValue(value);
        return value;
    }
    public static void insertDateValue(byte[] value, String time) {
        if (!TextUtils.isEmpty(time)) {
            String[] timeArray = time.split(" ");
            String INDEX;
            if (time.contains("-")) {
                INDEX = "-";
            } else {
                INDEX = "\\.";
            }
            int year = Integer.parseInt(timeArray[0].split(INDEX)[0]);
            int month = Integer.parseInt(timeArray[0].split(INDEX)[1]);
            int day = Integer.parseInt(timeArray[0].split(INDEX)[2]);
            int hour = Integer.parseInt(timeArray[1].split(":")[0]);
            int min = Integer.parseInt(timeArray[1].split(":")[1]);
            int second = Integer.parseInt(timeArray[1].split(":")[2]);
            value[4] = ResolveUtil.getTimeValue(year);
            value[5] = ResolveUtil.getTimeValue(month);
            value[6] = ResolveUtil.getTimeValue(day);
            value[7] = ResolveUtil.getTimeValue(hour);
            value[8] = ResolveUtil.getTimeValue(min);
            value[9] = ResolveUtil.getTimeValue(second);
        }

    }




    public static void insertDateValueNoH(byte[] value, String time) {
        if (!TextUtils.isEmpty(time)&&time.contains("-")) {
            String[] timeArray = time.split(" ");
            String INDEX;
            if (time.contains("-")) {
                INDEX = "-";
            } else {
                INDEX = "\\.";
            }
            int year = Integer.parseInt(timeArray[0].split(INDEX)[0]);
            int month = Integer.parseInt(timeArray[0].split(INDEX)[1]);
            int day = Integer.parseInt(timeArray[0].split(INDEX)[2]);
            value[4] = ResolveUtil.getTimeValue(year);
            value[5] = ResolveUtil.getTimeValue(month);
            value[6] = ResolveUtil.getTimeValue(day);
        }


    }
}
