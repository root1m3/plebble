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
 * Created by Administrator on 2018/4/10.
 */

public class DeviceKey {
    public static final String heartValue = "heartValue";
    public static final String hrvValue = "hrvValue";
    public static final String Quality = "Quality";

    public static final String HangUp = "HangUp";
    public static final String Telephone = "Telephone";
    public static final String Photograph = "Photograph";
    public static final String CanclePhotograph = "CanclePhotograph";
    public static final String type = "type";
    public static final String Play = "Play";
    public static final String Suspend = "Suspend";
    public static final String LastSong = "LastSong";
    public static final String NextSong = "NextSong";
    public static final String VolumeReduction = "VolumeReduction";
    public static final String VolumeUp = "VolumeUp";
    public static final String FindYourPhone = "FindYourPhone";
    public static final String Cancle_FindPhone = "Cancle_FindPhone";
    public static final String SOS = "SOS";
    public static final String DataType = "dataType";
    public static final String enterActivityModeSuccess = "enterActivityModeSuccess";
    public static final String Type = "Type";
    public static final String Manual = "Manual";
    public static final String automatic = "automatic";
    public static final String Data = "dicData";
    public static final String End = "dataEnd";
    public static final String index = "index";
    public static final String scanInterval = "scanInterval";
    public static final String scanTime = "scanTime";
    public static final String signalStrength = "signalStrength";
    public static final String arrayX = "arrayX";
    public static final String arrayY = "arrayY";
    public static final String arrayZ = "arrayZ";
    public static final String arrayPpgRawData = "arrayPpgRawData";
    public static final String KGpsResCheck0 = "KGpsResCheck0"; // 设备时间   GET_DEVICE_Time
    public static final String KGpsResCheck1 = "KGpsResCheck1"; // 设备时间
    public static final String Band = "Band";
    public static final String KFinishFlag = "finish";
    public static final String DeviceTime = "strDeviceTime"; // 设备时间   GET_DEVICE_Time
    public static final String GPSTime = "gpsExpirationTime"; // 设备时间   GET_DEVICE_Time
    public static final String TimeZone = "TimeZone"; // 设备时间   GET_DEVICE_Time

    /*
     *  GET_PERSONAL_INFO
     *   sex         性别
     *   Age         年龄
     *   Height      身高
     *   Weight      体重
     *   stepLength  步长
     *   deviceId    设备ID 
     */
    public static final String Gender = "MyGender";
    public static final String Age = "MyAge";
    public static final String Height = "MyHeight";
    public static final String Weight = "MyWeight";
    public static final String Stride = "MyStride";
    public static final String KUserDeviceId = "deviceId";


    /*
     *  GET_DEVICE_INFO
     *  distanceUnit  距离单位
     *  hourState     12小时24小时显示
     *  handleEnable  抬手检查使能标志
     *  handleSign    抬手检测左右手标志
     *  screenState   横竖屏显示
     *  anceEnable    ANCS使能开关
     */
    public static final String DistanceUnit = "distancUnit";
    public static final String TimeUnit = "timeUnit";
    public static final String TempUnit = "temperatureUnit";
    public static final String WristOn = "wristOn";
    public static final String TemperatureUnit = "TemperatureUnit";
    public static final String NightMode = "NightMode";
    public static final String LeftOrRight = "handleSign";
    //public static final String ScreenShow = "screenState";
    public static final String Dialinterface = "dialinterface";
    public static final String SocialDistancedwitch = "SocialDistancedwitch";
    public static final String ChineseOrEnglish= "ChineseOrEnglish";
    public static final String ScreenBrightness = "dcreenBrightness";
    public static final String KBaseHeart = "baseHeartRate";
    public static final String isHorizontalScreen = "isHorizontalScreen";

    /*
     *  SET_STEP_MODEL
     *totalSteps   总步数
     *calories     卡路里
     *distance     距离
     *time         时间
     *heartValue   心率值
     */

    public static final String Step = "step";
    public static final String Calories = "calories";
    public static final String Distance = "distance";
    public static final String ExerciseMinutes = "exerciseMinutes";
    public static final String HeartRate = "heartRate";
    public static final String ActiveMinutes = "ExerciseTime";
    public static final String TempData = "TempData";
    public static final String StepGoal = "stepGoal";   // 目标步数值  GET_GOAL
    public static final String BatteryLevel = "batteryLevel";  // 电量级别    READ_DEVICE_BATTERY
    public static final String MacAddress = "macAddress"; // MAC地址    READ_MAC_ADDRESS
    public static final String DeviceVersion = "deviceVersion";  // 版本号     READ_VERSION
    public static final String DeviceName = "deviceName";  // 设备名称    GET_DEVICE_NAME
    public static final String TemperatureCorrectionValue = "TemperatureCorrectionValue";  // 设备名称    GET_DEVICE_NAME

    /*
     *  GET_AUTOMIC_HEART
     *workModel         工作模式
     *heartStartHour    开始运动时间的小时
     *heartStartMinter  开始运动时间的分钟
     *heartEndHour      结束运动时间的小时
     *heartEndMinter      结束运动时间的分钟
     *heartWeek         星期使能
     *workTime          工作模式时间
     */
    public static final String WorkMode = "workModel";
    public static final String StartTime = "heartStartHour";
    public static final String KHeartStartMinter = "heartStartMinter";
    public static final String EndTime = "heartEndHour";
    public static final String KHeartEndMinter = "heartEndMinter";
    public static final String Weeks = "weekValue";
    public static final String IntervalTime = "intervalTime";


    /*
     *  READ_SPORT_PERIOD
     *StartTimeHour       开始运动时间的小时
     *StartTimeMin     开始运动时间的分钟
     *EndTimeHour         结束运动时间的小时
     *EndTimeMin       结束运动时间的分钟
     *Week      星期使能
     *KSportNotifierTime    运动提醒周期
     */
    public static final String StartTimeHour = "sportStartHour";
    public static final String StartTimeMin = "sportStartMinter";
    public static final String EndTimeHour = "sportEndHour";
    public static final String EndTimeMin = "sportEndMinter";

    public static final String LeastSteps = "leastSteps";

    /*
     *  GET_STEP_DATA
     *historyDate       日期：年月日
     *historySteps      步数
     *historyTime       运动时间
     *historyDistance   距离
     *Calories  卡路里
     *historyGoal       目标
     */
    public static final String Date = "date";
    public static final String Size = "size";
    public static final String Goal = "goal";



    /*
     *  GET_STEP_DETAIL
     *Date       日期：年月日时分秒
     *ArraySteps          步数
     *Calories       卡路里
     *Distance       距离
     *KDetailMinterStep     10分钟内每一分钟的步数
     */

    public static final String ArraySteps = "arraySteps";

    public static final String KDetailMinterStep = "detailMinterStep";
    public static final String temperature = "temperature";
    public static final String axillaryTemperature = "axillaryTemperature";

    /*
     * GET_SLEEP_DETAIL
     *Date        日期：年月日时分秒
     *KSleepLength      睡眠数据的长度
     *ArraySleep    5分钟的睡眠质量 (总共24个数据，每一个数据代表五分钟)
     */

    public static final String KSleepLength = "sleepLength";
    public static final String ArraySleep = "arraySleepQuality";
    public static final String sleepUnitLength = "sleepUnitLength";//是不是一分钟的睡眠数据 1为1分钟数据 0为5分钟数据

    /*
     *  GET_HEART_DATA
     *Date        日期：年月日时分秒
     *ArrayDynamicHR        10秒一个心率值，总共12个心率值
     */

    public static final String ArrayDynamicHR = "arrayDynamicHR";
    public static final String Blood_oxygen = "Blood_oxygen";



    /*
     * GET_ONCE_HEARTDATA
     *Date        日期：年月日时分秒
     *StaticHR       心率值
     */

    public static final String StaticHR = "onceHeartValue";

    /*
     *  GET_HRV_DATA
     *Date          日期：年月日时分秒
     *HRV         HRV值
     *VascularAging    血管老化度值
     *HeartRate    心率值
     *Stress         疲劳度
     */

    public static final String HRV = "hrv";
    public static final String VascularAging = "vascularAging";
    public static final String Fatiguedegree = "fatigueDegree";

    public static final String Stress = "stress";
    public static final String HighPressure = "highPressure";
    public static final String LowPressure = "lowPressure";
    public static final String highBP = "highBP";
    public static final String lowBP = "lowBP";

    /*
     *GET_ALARM
     *KAlarmId          0到4闹钟编号
     *ClockType        闹钟类型
     *ClockTime        闹钟时间的小时
     *KAlarmMinter      闹钟时间的分钟
     *Week  星期使能
     *KAlarmLength      长度
     *KAlarmContent     文本的内容
     */
    public static final String KAlarmId = "alarmId";
    public static final String OpenOrClose = "clockOpenOrClose";
    public static final String ClockType = "clockType";
    public static final String ClockTime = "alarmHour";
    public static final String KAlarmMinter = "alarmMinter";
    public static final String Week = "weekValue";
    public static final String KAlarmLength = "alarmLength";
    public static final String KAlarmContent = "dicClock";


    /***********************GET_HRV_TESTDATA***************************************************/
    /*
     *KBloodTestLength      数据长度
     *KBloodTestProgress    进度
     *KBloodTestValue       本次PPG获得的值
     *KBolldTestCurve       本次波型的高度
     */
    public static final String KBloodTestLength = "bloodTestLength";
    public static final String KBloodTestProgress = "bloodTestProgress";
    public static final String KBloodTestValue = "bloodTestValue";
    public static final String KBloodTestCurve = "bloodTestCurve";

    /*
     *KBloodResultPercent       反弹的百分比
     *KBloodResultRebound       平均反弹高度
     *KBloodResultMax           最大高度
     *KBloodResultRank          结果级别（1到6）
     */
    public static final String KBloodResultPercent = "bloodPercent";
    public static final String KBloodResultRebound = "bloodRebound";
    public static final String KBloodResultMax = "bloodResultMax";
    public static final String KBloodResultRank = "bloodResultRank";


    /*
     *KHrvTestProgress  进度
     *KHrvTestWidth     本次心跳的宽度
     *KHrvTestValue     心率值
     */
    public static final String KHrvTestProgress = "hrvTestProgress";
    public static final String KHrvTestWidth = "hrvTestWidth";
    public static final String KHrvTestValue = "hrvTestValue";

    /*
     *KHrvResultState   SDNN结果  如果是0,说明检测失败
     *KHrvResultAvg     SDNN平均值
     *KHrvResultTotal   总SDNN结果
     *KHrvResultCount   有效数据个数
     *KHrvResultTired   疲劳指数据
     *KHrvResultValue   心率值
     */
    public static final String KHrvResultState = "hrvResultState";
    public static final String KHrvResultAvg = "hrvResultAvg";
    public static final String KHrvResultTotal = "hrvResultTotal";
    public static final String KHrvResultCount = "hrvResultCount";
    public static final String KHrvResultTired = "hrvResultTired";
    public static final String KHrvResultValue = "hrvResultValue";


    /*
     *KDisturbState     1:开始运动   0：停止运动
     *KSlipHand         1: 带在手上   0;脱手。
     *KPPGData          PPG的波型值
     */
    public static final String KDisturbState = "disturbState";
    public static final String KSlipHand = "slipHand";
    public static final String KPPGData = "ppgData";


    /*
     *@param Date       时间：年月日时分秒
     *@param Latitude   纬度数据
     *@param Longitude  经度数据
     */


    public static final String Latitude = "locationLatitude";
    public static final String Longitude = "locationLongitude";

    public static final String KActivityLocationTime = "ActivityLocationTIme";
    public static final String KActivityLocationLatitude = "ActivityLocationLatitude";
    public static final String KActivityLocationLongitude = "ActivityLocationLongitude";
    public static final String KActivityLocationCount = "KActivityLocationCount";
/*
 *      GET_SPORTMODEL_DATA
 *@param  Date        时间：年月日时分秒
 *@param  ActivityMode 运动类型
 0=Run,
 1=Cycling,
 2=Swimming,
 3=Badminton,
 4=Football,
 5=Tennis,
 6=Yoga,
 7=Medication,
 8=Dance
 
 *@param  HeartRate       心率
 *@param  ActiveMinutes   运动时间
 *@param  Step       运动步数
 *@param  Pace       运动速度
 *@param  Calories    卡路里
 *@param  Distance     距离
 */


    public static final String ActivityMode = "sportModel";
    public static final String Pace = "sportModelSpeed";

    public static final String KDataID = "KDataID";
    public static final String KPhoneDataLength = "KPhoneDataLength";
    public static final String KClockLast = "KClockLast";


    public static final String TakePhotoMode = "TakePhotoMode";
    public static final String KFunction_tel = "TelMode";
    public static final String KFunction_reject_tel = "RejectTelMode";

    public static final String FindMobilePhoneMode = "FindMobilePhoneMode";
    public static final String KEnable_exercise = "KEnable_exercise";
    public static final String ECGQualityValue = "ECGQualityValue";

    public static final String ECGResultValue = "ECGResultVALUE";
    public static final String ECGHrvValue = "ECGHrvValue";
    public static final String ECGAvBlockValue = "ECGAvBlockValue";
    public static final String ECGHrValue = "ECGHrValue";
    public static final String PPGHrValue = "PPGHrValue";
    public static final String ECGStreesValue = "ECGStreesValue";
    public static final String ECGhighBpValue = "ECGhighBpValue";
    public static final String ECGLowBpValue = "ECGLowBpValue";
    public static final String ECGMoodValue = "ECGMoodValue";
    public static final String ECGBreathValue = "ECGBreathValue";
    public static final String KEcgDataString = "KEcgDataString"; // ecg
    public static final String ECGValue = "ECGValue";
    public static final String PPGValue = "PPGValue";
    public static final String EcgStatus = "EcgStatus";
    public static final String EcgSBP = "PPGSBP";
    public static final String EcgDBP = "PPGDBP";
    public static final String EcgHR = "PPGHR";
    public static final String WaveformDownTime = "WaveformDownTime";
    public static final String WaveformRiseTime = "WaveformRiseTime";

    public static final String EcgGender = "Gender";
    public static final String EcgAge = "Age";
    public static final String EcgHeight = "Height";
    public static final String EcgWeight = "Weight";


}
