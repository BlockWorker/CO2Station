#include <stdio.h>

#include "serial_interface.h"
#include "app_util.h"
#include "mcc_generated_files/usb/usb.h"
#include "app_time.h"
#include "scd.h"
#include "flash_storage.h"
#include "mcc_generated_files/rtcc.h"

#define LIST_HELP_FMT "\
Cmd (current):\n\
 - time dt tm (%s)\n\
 - wakeup s (%u)\n\
 - screen c (%u)\n\
 - bat c (%u)\n\
 - standby t-t (%02u:%02u-%02u:%02u)\n\
 - calint d (%u)\n\
 - calref ppm (%u)\n\
 - scdint s (%u)\n\
 - alt m (%u)\n\
 - tempoff cK (%u)\n\
 - scdsleep sc-rc (%u-%u)\n"

void SCD_ReadOffset();

void SERIAL_Tasks() {
    static char dtBuffer[20];
    static char receiveBuffer[255] = { 0 };
    static char sendBuffer[255] = { 0 };
    static uint16_t scanBuffer[6];
    static datetime_t time, formattable;
    
    if (!APP_USB_Available()) return;
    
    if (USBUSARTIsTxTrfReady()) {
        uint8_t numCharsRead = getsUSBUSART((uint8_t*)receiveBuffer, sizeof(receiveBuffer));
        receiveBuffer[numCharsRead] = 0;
        
        if (numCharsRead == 0) return;
        
        uint16_t numCharsSend = 0;
        
        if (numCharsRead >= 4 && (strncmp(receiveBuffer, "list", 4) == 0 || strncmp(receiveBuffer, "help", 4) == 0)) {
            while (!RTCC_TimeGet(&time));
            Time_MakeFormattable(&time, &formattable);
            strftime(dtBuffer, sizeof(dtBuffer), "%Y-%m-%d %H:%M:%S", &formattable);
            numCharsSend = sprintf(sendBuffer, LIST_HELP_FMT, dtBuffer, wakeup_interval_sec, screen_interval_wakeups,
                    battery_interval_wakeups, standby_start.tm_hour, standby_start.tm_min, standby_end.tm_hour,
                    standby_end.tm_min, cal_interval_days, scd_cal_ref_ppm, scd_measure_interval_sec,
                    scd_alt_comp_m, scd_temp_offset, scd_sleep_dur_wakeups, scd_run_dur_wakeups);
        } else if (numCharsRead >= 19 && sscanf(receiveBuffer, "time %u-%u-%u %u:%u:%u", scanBuffer, scanBuffer + 1, scanBuffer + 2, scanBuffer + 3, scanBuffer + 4, scanBuffer + 5) == 6) {
            if (scanBuffer[0] >= 2000 && scanBuffer[0] < 2100 && scanBuffer[1] >= 1 && scanBuffer[1] <= 12 &&
                    scanBuffer[2] >= 1 && scanBuffer[2] <= Time_GetMonthLengthDays(scanBuffer[1], scanBuffer[0]) &&
                    scanBuffer[3] < 24 && scanBuffer[4] < 60 && scanBuffer[5] < 60) {
                time.tm_year = scanBuffer[0] - 2000;
                time.tm_mon = scanBuffer[1];
                time.tm_mday = scanBuffer[2];
                time.tm_hour = scanBuffer[3];
                time.tm_min = scanBuffer[4];
                time.tm_sec = scanBuffer[5];
                time.tm_wday = Time_GetWeekday(&time);
                
                RTCC_TimeSet(&time);
                
                Time_MakeFormattable(&time, &formattable);
                numCharsSend = strftime(sendBuffer, sizeof(sendBuffer), "Set time to %c\n", &formattable) - 1;
                FS_SaveTime();
                Time_SetNextWakeupAlarm();
            } else {
                strcpy(sendBuffer, "Invalid date/time format\n");
                numCharsSend = 25;
            }
        } else if (numCharsRead >= 8 && sscanf(receiveBuffer, "wakeup %u", scanBuffer) == 1) {
            wakeup_interval_sec = scanBuffer[0];
            numCharsSend = sprintf(sendBuffer, "Set wakeup interval to %u sec\n", wakeup_interval_sec);
            FS_SaveIntervals();
        } else if (numCharsRead >= 8 && sscanf(receiveBuffer, "screen %u", scanBuffer) == 1) {
            screen_interval_wakeups = scanBuffer[0];
            numCharsSend = sprintf(sendBuffer, "Set screen update interval to %u wakeups\n", screen_interval_wakeups);
            FS_SaveIntervals();
        } else if (numCharsRead >= 5 && sscanf(receiveBuffer, "bat %u", scanBuffer) == 1) {
            battery_interval_wakeups = scanBuffer[0];
            numCharsSend = sprintf(sendBuffer, "Set battery update interval to %u wakeups\n", battery_interval_wakeups);
            FS_SaveIntervals();
        } else if (numCharsRead >= 15 && sscanf(receiveBuffer, "standby %u:%u-%u:%u", scanBuffer, scanBuffer + 1, scanBuffer + 2, scanBuffer + 3) == 4) {
            if (scanBuffer[0] < 24 && scanBuffer[1] < 60 && scanBuffer[2] < 24 && scanBuffer[3] < 60) {
                standby_start.tm_hour = scanBuffer[0];
                standby_start.tm_min = scanBuffer[1];
                standby_end.tm_hour = scanBuffer[2];
                standby_end.tm_min = scanBuffer[3];
                numCharsSend = sprintf(sendBuffer, "Set standby to %02u:%02u to %02u:%02u\n", standby_start.tm_hour,
                                       standby_start.tm_min, standby_end.tm_hour, standby_end.tm_min);
                FS_SaveStandbyConfig();
            } else {
                strcpy(sendBuffer, "Invalid time format\n");
                numCharsSend = 20;
            }
        } else if (numCharsRead >= 8 && sscanf(receiveBuffer, "calint %u", scanBuffer) == 1) {
            cal_interval_days = scanBuffer[0];
            numCharsSend = sprintf(sendBuffer, "Set recommended cal interval to %u days\n", cal_interval_days);
            FS_SaveIntervals();
            Time_CalcNextCal();
            FS_SaveCalDate();
        } else if (numCharsRead >= 8 && sscanf(receiveBuffer, "calref %u", scanBuffer) == 1) {
            scd_cal_ref_ppm = scanBuffer[0];
            numCharsSend = sprintf(sendBuffer, "Set cal reference value to %u ppm\n", scd_cal_ref_ppm);
            FS_SaveSCDInfo();
        } else if (numCharsRead >= 8 && sscanf(receiveBuffer, "scdint %u", scanBuffer) == 1) {
            scd_measure_interval_sec = scanBuffer[0];
            numCharsSend = sprintf(sendBuffer, "Set SCD measurement interval to %u sec\n", scd_measure_interval_sec);
            FS_SaveSCDInfo();
            scd_nvm_initialized = false;
            SCD_SendConfig();
        } else if (numCharsRead >= 5 && sscanf(receiveBuffer, "alt %u", scanBuffer) == 1) {
            scd_alt_comp_m = scanBuffer[0];
            numCharsSend = sprintf(sendBuffer, "Set altitude compensation to %u m\n", scd_alt_comp_m);
            FS_SaveSCDInfo();
            scd_nvm_initialized = false;
            SCD_SendConfig();
        } else if (numCharsRead >= 9 && sscanf(receiveBuffer, "tempoff %u", scanBuffer) == 1) {
            scd_temp_offset = scanBuffer[0];
            numCharsSend = sprintf(sendBuffer, "Set temperature offset to %u.%02u deg C\n", scd_temp_offset / 100, scd_temp_offset % 100);
            FS_SaveSCDInfo();
            scd_nvm_initialized = false;
            SCD_SendConfig();
        } else if (numCharsRead >= 11 && sscanf(receiveBuffer, "scdsleep %u-%u", scanBuffer, scanBuffer + 1) == 2) {
            if (scanBuffer[0] == 0 || scanBuffer[1] == 0) {
                scd_sleep_dur_wakeups = 0;
                scd_run_dur_wakeups = 0;
            } else {
                scd_sleep_dur_wakeups = scanBuffer[0];
                scd_run_dur_wakeups = scanBuffer[1];
            }
            numCharsSend = sprintf(sendBuffer, "Set SCD sleep timing to %u sleep, %u run\n", scd_sleep_dur_wakeups, scd_run_dur_wakeups);
            FS_SaveSCDInfo();
        } else if (numCharsRead >= 7 && strncmp(receiveBuffer, "caldate", 7) == 0) {
            Time_MakeFormattable(&next_cal, &formattable);
            numCharsSend = strftime(sendBuffer, sizeof(sendBuffer), "Next cal: %Y-%m-%d\n", &formattable) - 1;
        } else if (numCharsRead >= 10 && strncmp(receiveBuffer, "gettempoff", 7) == 0) {
            SCD_ReadOffset();
        } else {
            strcpy(sendBuffer, "Unknown command, use \"help\" for a list of commands\n");
            numCharsSend = 51;
        }
        
        if (numCharsSend >= 255) numCharsSend = 254;
        sendBuffer[sizeof(sendBuffer) - 1] = 0;
        
        if (numCharsSend > 0) {
            putUSBUSART((uint8_t*)sendBuffer, numCharsSend + 1);
        }
    }
    
    CDCTxService();
}
