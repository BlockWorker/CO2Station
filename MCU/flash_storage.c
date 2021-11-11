#include "flash_storage.h"
#include "app_util.h"
#include "app_time.h"
#include "scd.h"

void _rtcc_writeDirect(uint16_t* regs) { //4 values: DATEH DATEL TIMEH TIMEL    
    __builtin_write_RTCC_WRLOCK();

    RTCCON1Lbits.RTCEN = 0;

    IFS3bits.RTCIF = false;
    IEC3bits.RTCIE = 0;
    
    DATEH = regs[0];
    DATEL = regs[1];
    TIMEH = regs[2];
    TIMEL = regs[3];  
    
    RTCCON1Lbits.RTCEN = 1;  

    asm volatile("DISI #6");
    asm volatile("MOV #NVMKEY, W1");
    asm volatile("MOV #0x55, W2");
    asm volatile("MOV W2, [W1]");
    asm volatile("MOV #0xAA, W3");
    asm volatile("MOV W3, [W1]");
    asm volatile("BSET RTCCON1L, #11");

    IEC3bits.RTCIE = 1;
}

uint16_t _fs_encodeTimeWord(datetime_t* datetime) {
    uint16_t result = (datetime->tm_hour & 0x1f) << 11;
    result |= (datetime->tm_min & 0x3f) << 5;
    result |= (datetime->tm_sec & 0x3e) >> 1;
    return result;
}

void _fs_decodeTimeWord(uint16_t word, datetime_t* datetime) {
    datetime->tm_hour = word >> 11;
    datetime->tm_min = (word >> 5) & 0x3f;
    datetime->tm_sec = (word & 0x1f) << 1;
}

void FS_Init() {
    if (DEE_Init() != DEE_NO_ERROR) APP_CRITICAL_ERROR("CRITICAL ERROR: Flash storage corrupt or expired!");
    
    wakeup_interval_sec = 60;
    screen_interval_wakeups = 5;
    battery_interval_wakeups = 10;
    next_cal.tm_year = 0;
    next_cal.tm_mon = next_cal.tm_mday = 1;
    standby_start.tm_hour = standby_start.tm_min = standby_start.tm_sec = 0;
    standby_end.tm_hour = standby_end.tm_min = standby_end.tm_sec = 0;
    cal_interval_days = 21;
    scd_cal_ref_ppm = 400;
    scd_measure_interval_sec = 30;
    scd_alt_comp_m = 0;
    scd_temp_offset = 0;
    
    uint16_t initialized = 0;
    if (DEE_Read(FS_INITIALIZED, &initialized) != DEE_NO_ERROR || initialized != 1) {
        Time_CalcNextCal();
        if (DEE_Write(FS_INITIALIZED, 1) != DEE_NO_ERROR) APP_MINOR_ERROR("Error: Failed to initialize flash storage");
        else FS_SaveAll();
    } else {
        bool success = true;
        uint16_t time_regs[4];
        uint16_t cal_dateval = 0;
        uint16_t standby_start_word = 0;
        uint16_t standby_end_word = 0;
        
        success &= (DEE_Read(FS_DATEH, time_regs) == DEE_NO_ERROR);
        success &= (DEE_Read(FS_DATEL, time_regs + 1) == DEE_NO_ERROR);
        success &= (DEE_Read(FS_TIMEH, time_regs + 2) == DEE_NO_ERROR);
        success &= (DEE_Read(FS_TIMEL, time_regs + 3) == DEE_NO_ERROR);
        success &= (DEE_Read(FS_WAKEUP_INT, &wakeup_interval_sec) == DEE_NO_ERROR);
        success &= (DEE_Read(FS_SCREEN_INT, &screen_interval_wakeups) == DEE_NO_ERROR);
        success &= (DEE_Read(FS_BATTERY_INT, &battery_interval_wakeups) == DEE_NO_ERROR);
        success &= (DEE_Read(FS_STANDBY_START, &standby_start_word) == DEE_NO_ERROR);
        success &= (DEE_Read(FS_STANDBY_END, &standby_end_word) == DEE_NO_ERROR);
        success &= (DEE_Read(FS_CAL_INT, &cal_interval_days) == DEE_NO_ERROR);
        success &= (DEE_Read(FS_NEXT_CAL_DATE, &cal_dateval) == DEE_NO_ERROR);
        success &= (DEE_Read(FS_CAL_REF, &scd_cal_ref_ppm) == DEE_NO_ERROR);
        success &= (DEE_Read(FS_MEASURE_INT, &scd_measure_interval_sec) == DEE_NO_ERROR);
        success &= (DEE_Read(FS_ALT_COMP, &scd_alt_comp_m) == DEE_NO_ERROR);
        success &= (DEE_Read(FS_TEMP_OFFSET, &scd_temp_offset) == DEE_NO_ERROR);
        
        if (!success) APP_MINOR_ERROR("Error: Failed to read data from flash storage");
        else {
            _rtcc_writeDirect(time_regs);
            _fs_decodeTimeWord(standby_start_word, &standby_start);
            _fs_decodeTimeWord(standby_end_word, &standby_end);
            next_cal.tm_year = cal_dateval >> 9;
            next_cal.tm_mon = (cal_dateval >> 5) & 0xf;
            next_cal.tm_mday = cal_dateval & 0x1f;
        }
    }
}

bool _fs_saveTime() {
    while (RTCSTATLbits.SYNC);
    uint16_t dateh_val = DATEH;
    uint16_t datel_val = DATEL;
    uint16_t timeh_val = TIMEH;
    uint16_t timel_val = TIMEL;
    bool success = true;
    success &= (DEE_Write(FS_DATEH, dateh_val) == DEE_NO_ERROR);
    success &= (DEE_Write(FS_DATEL, datel_val) == DEE_NO_ERROR);
    success &= (DEE_Write(FS_TIMEH, timeh_val) == DEE_NO_ERROR);
    success &= (DEE_Write(FS_TIMEL, timel_val) == DEE_NO_ERROR);
    return success;
}

void FS_SaveTime() {
    if (!_fs_saveTime()) APP_MINOR_ERROR("Error: Failed to write time to flash storage");
}

bool _fs_saveIntervals() {
    bool success = true;
    success &= (DEE_Write(FS_WAKEUP_INT, wakeup_interval_sec) == DEE_NO_ERROR);
    success &= (DEE_Write(FS_SCREEN_INT, screen_interval_wakeups) == DEE_NO_ERROR);
    success &= (DEE_Write(FS_BATTERY_INT, battery_interval_wakeups) == DEE_NO_ERROR);
    success &= (DEE_Write(FS_CAL_INT, cal_interval_days) == DEE_NO_ERROR);
    return success;
}

void FS_SaveIntervals() {
    if (!_fs_saveIntervals()) APP_MINOR_ERROR("Error: Failed to write intervals to flash storage");
}

bool _fs_saveStandbyConfig() {
    bool success = true;
    success &= (DEE_Write(FS_STANDBY_START, _fs_encodeTimeWord(&standby_start)) == DEE_NO_ERROR);
    success &= (DEE_Write(FS_STANDBY_END, _fs_encodeTimeWord(&standby_end)) == DEE_NO_ERROR);
    return success;
}

void FS_SaveStandbyConfig() {
    if (!_fs_saveStandbyConfig()) APP_MINOR_ERROR("Error: Failed to write standby config to flash storage");
}

bool _fs_saveCalDate() {
    uint16_t dateval = (next_cal.tm_year & 0x7f) << 9;
    dateval |= (next_cal.tm_mon & 0xf) << 5;
    dateval |= next_cal.tm_mday & 0x1f;
    return DEE_Write(FS_NEXT_CAL_DATE, dateval) == DEE_NO_ERROR;
}

void FS_SaveCalDate() {
    if (!_fs_saveCalDate()) APP_MINOR_ERROR("Error: Failed to write cal date to flash storage");
}

bool _fs_saveSCDInfo() {
    bool success = true;
    success &= (DEE_Write(FS_CAL_REF, scd_cal_ref_ppm) == DEE_NO_ERROR);
    success &= (DEE_Write(FS_MEASURE_INT, scd_measure_interval_sec) == DEE_NO_ERROR);
    success &= (DEE_Write(FS_ALT_COMP, scd_alt_comp_m) == DEE_NO_ERROR);
    success &= (DEE_Write(FS_TEMP_OFFSET, scd_temp_offset) == DEE_NO_ERROR);
    return success;
}

void FS_SaveSCDInfo() {
    if (!_fs_saveSCDInfo()) APP_MINOR_ERROR("Error: Failed to write SCD info to flash storage");
}

void FS_SaveAll() {
    bool success = true;
    success &= _fs_saveTime();
    success &= _fs_saveIntervals();
    success &= _fs_saveStandbyConfig();
    success &= _fs_saveCalDate();
    success &= _fs_saveSCDInfo();
    if (!success) APP_MINOR_ERROR("Error: Failed to write data to flash storage");
}
