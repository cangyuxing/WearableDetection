#include "user_rtc.h"

void User_RTC_Init(void) {
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    HAL_RTC_Init(&hrtc);
    BSP_RTCEx_BKUPRead();
}

uint8_t RTC_Set_Alarm(uint8_t Week, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec,uint32_t alarm) {
    RTC_AlarmTypeDef sAlarm;
    sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
    sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    sAlarm.AlarmDateWeekDay  = sday;
    sAlarm.AlarmTime.Hours   = hour;
    sAlarm.AlarmTime.Minutes = min;
    sAlarm.AlarmTime.Seconds = sec;
    sAlarm.Alarm = alarm; //RTC_ALARM_A
    HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);
    return 1;
}

uint8_t RTC_Get(uint8_t *Week, uint16_t *syear, uint8_t *smon, uint8_t *sday, uint8_t *hour, uint8_t *min, uint8_t *sec) { //BIN码
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    *Week  = sDate.WeekDay ;
    *syear = sDate.Year    ;
    *smon  = sDate.Month   ;
    *sday  = sDate.Date    ;
    *hour  = sTime.Hours   ;
    *min   = sTime.Minutes ;
    *sec   = sTime.Seconds ;
    MYLOG(INFO, "RTC Get Time :%02d %02d-%02d-%02d %02d:%02d:%02d",sDate.WeekDay, 2000 + sDate.Year, sDate.Month, sDate.Date,sTime.Hours, sTime.Minutes, sTime.Seconds);
    return 1;
}

uint8_t RTC_Set(uint8_t Week, uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec) { //BIN码
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    sDate.WeekDay = Week;
    sDate.Year    = syear;
    sDate.Month   = smon;
    sDate.Date    = sday;
    sTime.Hours   = hour;
    sTime.Minutes = min;
    sTime.Seconds = sec;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    MYLOG(INFO, "RTC Set Time :%02d %02d-%02d-%02d %02d:%02d:%02d",sDate.WeekDay, 2000 + sDate.Year, sDate.Month, sDate.Date,sTime.Hours, sTime.Minutes, sTime.Seconds);
    return 1;
}

uint32_t BSP_mktime(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec){
    struct tm timeinfo;
    time_t  time;
    timeinfo.tm_year = syear - 1900;
    timeinfo.tm_mon  = smon-1;
    timeinfo.tm_mday = sday;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min  = min;
    timeinfo.tm_sec  = sec;
    time = mktime(&timeinfo);
    MYLOG(DEBUG,"BSP_mktime = %x",time);
    return time;
}

struct tm BSP_localtime(time_t time){
    struct tm *timeinfo = localtime(&time);
    timeinfo->tm_year += 1900;
    timeinfo->tm_mon += 1;
    MYLOG(DEBUG, "BSP_localtime :%02d %02d-%02d-%02d %02d:%02d:%02d",timeinfo->tm_wday, timeinfo->tm_year, timeinfo->tm_mon, timeinfo->tm_mday,timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    return *timeinfo;
}
void BSP_RTCEx_BKUPRead(void) {
    time_t   time;
    uint16_t syear;
    uint8_t  Week, smon, sday, hour, min, sec;
    uint32_t bkup = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
    //if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0X5050) { //是否第一次配置
    MYLOG(INFO, "RTC bkup = %#x",bkup);
    if(bkup != 0X5050) { //是否第一次配置
        RTC_Set(2, 20, 12, 13, 8, 1, 0);
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0X5050); //标记已经初始化过了
        MYLOG(INFO, "RTC Frist Set\r\n");
    }
    RTC_Get(&Week, &syear, &smon, &sday, &hour, &min, &sec);
    time = BSP_mktime(syear+2000, smon, sday, hour, min, sec);
    BSP_localtime(time);
    RTC_Set_Alarm(0,13, 8, 1, 5 ,RTC_ALARM_A);
    RTC_Set_Alarm(0,13, 8, 1, 10,RTC_ALARM_B);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
    if(hrtc->Instance == RTC) {
        MYLOG(INFO, "HAL_RTC_AlarmAEventCallback");
    }
}

void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc) {
    if(hrtc->Instance == RTC) {
        MYLOG(INFO, "HAL_RTCEx_AlarmBEventCallback");
    }
}
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc) {
    if(hrtc->Instance == RTC) {
        MYLOG(INFO, "HAL_RTCEx_WakeUpTimerEventCallback");
    }
}
