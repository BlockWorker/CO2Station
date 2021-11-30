
#ifndef APP_TIME_H
#define	APP_TIME_H

#include <xc.h>
#include <stdbool.h>
#include <time.h>

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    typedef struct tm datetime_t;
    
    
    extern uint16_t wakeup_interval_sec; //MCU wakeup interval in seconds
    extern uint16_t screen_interval_wakeups; //Screen update interval, in wakeup cycles
    extern uint16_t battery_interval_wakeups; //Battery status update interval, in wakeup cycles
    extern datetime_t next_cal; //Next suggested sensor calibration date, only uses year + mon + mday
    extern uint16_t cal_interval_days; //Suggested sensor calibration interval, in days
    extern datetime_t standby_start; //Starting time for standby mode, only uses hour + min + sec
    extern datetime_t standby_end; //End (wakeup) time for standby mode, only uses hour + min + sec
    extern bool wakeup_by_timer;
    
    
    void Time_OnAlarmInterrupt();
    
    uint16_t Time_GetMonthLengthDays(uint16_t month, uint16_t year);
    uint8_t Time_GetWeekday(datetime_t* date);
    
    void Time_CalcNextCal();
    void Time_SetNextWakeupAlarm();
    void Time_DisableAlarm();
    bool Time_IsInStandbyTime();
    
    bool Time_IsCalSuggested();
    bool Time_HasTimePassed(datetime_t* time);
    
    void Time_MakeFormattable(datetime_t* datetime, datetime_t* result);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* APP_TIME_H */

