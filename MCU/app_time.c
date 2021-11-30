#include "app_time.h"
#include <stdio.h>
#include "mcc_generated_files/usb/usb.h"
#include "mcc_generated_files/rtcc.h"
#include "scd.h"
#include "app_util.h"
#include "battery.h"
#include "flash_storage.h"

uint16_t wakeup_interval_sec; //MCU wakeup interval in seconds
uint16_t screen_interval_wakeups; //Screen update interval, in wakeup cycles
uint16_t battery_interval_wakeups; //Battery status update interval, in wakeup cycles
datetime_t next_cal; //Next suggested sensor calibration date, only uses year + mon + mday
uint16_t cal_interval_days; //Suggested sensor calibration interval, in days
datetime_t standby_start; //Starting time for standby mode, only uses hour + min + sec
datetime_t standby_end; //End (wakeup) time for standby mode, only uses hour + min + sec
bool wakeup_by_timer = true;

void Time_OnAlarmInterrupt() {
    static uint16_t seconds_passed = 0;
    
    seconds_passed += wakeup_interval_sec;
    if (seconds_passed >= 60) {
        seconds_passed = 0;
        FS_SaveTime();
    }
    
    wakeup_by_timer = true;
}

uint16_t Time_GetMonthLengthDays(uint16_t month, uint16_t year) {
    switch (month) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return 31;
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        case 2:
            if (year % 4 == 0) return 29;
            else return 28;
        default:
            return 30;
    }
}

uint8_t Time_GetWeekday(datetime_t* date)
{
    static const uint8_t month_offset_table[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    uint8_t year = date->tm_mon < 3 ? date->tm_year - 1 : date->tm_year;
    return (date->tm_mday + month_offset_table[date->tm_mon - 1] + year + (year >> 2)) % 7;
}


void Time_CalcNextCal() {
    //get current time+date
    datetime_t time;
    while (!RTCC_TimeGet(&time));
    
    //calculate target date, may be invalid at first (beyond month length)
    uint16_t year = time.tm_year;
    uint16_t month = time.tm_mon;
    uint16_t day = time.tm_mday + cal_interval_days;
    
    //adjust month/year values to create valid offset date
    uint16_t current_month_days = Time_GetMonthLengthDays(month, year);
    while (day > current_month_days) {
        day -= current_month_days;
        if (++month > 12) {
            month = 1;
            year++;
        }
        current_month_days = Time_GetMonthLengthDays(month, year); 
    }
    
    //set target date
    next_cal.tm_year = year;
    next_cal.tm_mon = month;
    next_cal.tm_mday = day;
}

void Time_SetNextWakeupAlarm() {
    //clear and disable RTCC interrupt
    IFS3bits.RTCIF = false;
    IEC3bits.RTCIE = 0;
    
    //get current time+date
    datetime_t time;
    while (!RTCC_TimeGet(&time));
    
    uint16_t min = time.tm_min;
    uint16_t sec = time.tm_sec + wakeup_interval_sec;
    
    while (sec >= 60) {
        sec -= 60;
        min++;
    }
    
    while (RTCSTATLbits.ALMSYNC);
    
    RTCCON1Hbits.ALRMEN = 0; //disable alarm
    RTCCON1H = 0x4400; //chime enable, 10 minute AMASK
    
    //set alarm time
    ALMTIMEHbits.MINONE = min % 10;
    ALMTIMELbits.SECTEN = sec / 10;
    ALMTIMELbits.SECONE = sec % 10;
    
    RTCSTATLbits.ALMEVT = 0; //clear alarm event flag just in case
    RTCCON1Hbits.ALRMEN = 1; //enable alarm
    
    IEC3bits.RTCIE = 1; //re-enable RTCC interrupt
}

void Time_DisableAlarm() {
    RTCCON1H = 0;
}

int8_t _time_compareTime(datetime_t* time1, datetime_t* time2) {
    int8_t cmp = time1->tm_hour - time2->tm_hour;
    if (cmp != 0) return cmp;
    cmp = time1->tm_min - time2->tm_min;
    if (cmp != 0) return cmp;
    return time1->tm_sec - time2->tm_sec;
}

bool Time_IsInStandbyTime() {
    int8_t stb_compare = _time_compareTime(&standby_start, &standby_end);
    if (stb_compare == 0) return false; //start and end times identical: never in standby
    
    //get current time+date
    datetime_t time;
    while (!RTCC_TimeGet(&time));
    
    int8_t start_compare = _time_compareTime(&time, &standby_start);
    int8_t end_compare = _time_compareTime(&time, &standby_end);
    
    if (stb_compare < 0) { //start before end: standby if between times
        return start_compare >= 0 && end_compare < 0;
    } else { //end before start: standby if outside of times
        return start_compare >= 0 || end_compare < 0;
    }
}

bool Time_IsCalSuggested() {
    //get current time+date
    datetime_t time;
    while (!RTCC_TimeGet(&time));
    
    if (time.tm_year < next_cal.tm_year) return false;
    if (time.tm_year > next_cal.tm_year) return true;
    if (time.tm_mon < next_cal.tm_mon) return false;
    if (time.tm_mon > next_cal.tm_mon) return true;
    return time.tm_mday >= next_cal.tm_mday;
}

bool Time_HasTimePassed(datetime_t* time) {
    //get current time+date
    datetime_t curtime;
    while (!RTCC_TimeGet(&curtime));
    
    return _time_compareTime(&curtime, time) >= 0;
}

void Time_MakeFormattable(datetime_t* datetime, datetime_t* result) {
    result->tm_year = datetime->tm_year + 100; //format expects years since 1900, but the RTCC encodes years since 2000
    result->tm_mon = datetime->tm_mon - 1; //format expects January to be month 0, but the RTCC encodes January as month 1
    result->tm_mday = datetime->tm_mday;
    result->tm_wday = datetime->tm_wday;
    result->tm_hour = datetime->tm_hour;
    result->tm_min = datetime->tm_min;
    result->tm_sec = datetime->tm_sec;
    
    //calculate day of year
    result->tm_yday = datetime->tm_mday - 1; //add days of current month, though day 0 is expected to be January 1
    uint16_t month;
    for (month = 1; month < datetime->tm_mon; month++) {
        result->tm_yday += Time_GetMonthLengthDays(month, datetime->tm_year);
    }
    
    result->tm_isdst = -1; //DST status unknown
}
