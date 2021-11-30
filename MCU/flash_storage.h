#ifndef FLASH_STORAGE_H
#define	FLASH_STORAGE_H

#include <xc.h>
#include "mcc_generated_files/dee.h"

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    typedef enum {
        FS_INITIALIZED = 0x00, //Storage initialized, always 1 or non-existent
        FS_DATEH = 0x01, //Date+time registers for timestamps
        FS_DATEL = 0x02,
        FS_TIMEH = 0x03,
        FS_TIMEL = 0x04,
        FS_WAKEUP_INT = 0x05, //MCU wakeup interval in seconds
        FS_SCREEN_INT = 0x06, //Screen update interval, in wakeup cycles
        FS_BATTERY_INT = 0x07, //Battery status update interval, in wakeup cycles
        FS_STANDBY_START = 0x08, //Starting time for standby mode, 5b hour - 6b minute - 5b second/2
        FS_STANDBY_END = 0x09, //End (wakeup) time for standby mode, 5b hour - 6b minute - 5b second/2
        FS_NEXT_CAL_DATE = 0x10, //Next suggested sensor calibration date, 7b year - 4b month - 5b day
        FS_CAL_INT = 0x11, //Suggested sensor calibration interval, in days
        FS_CAL_REF = 0x12, //Sensor calibration reference value, in ppm
        FS_MEASURE_INT = 0x13, //Sensor measurement interval, in seconds
        FS_ALT_COMP = 0x14, //Sensor altitude calibration, in meters above sea level
        FS_TEMP_OFFSET = 0x15, //Sensor temperature offset, in hundredths of degrees C
        FS_SCD_SLEEP_DUR = 0x16, //Sensor sleep duration, in wakeup cycles
        FS_SCD_RUN_DUR = 0x17 //Sensor run duration, in wakeup cycles
    } fs_addr_t;
    
    void FS_Init();
    void FS_SaveAll();
    void FS_SaveTime();
    void FS_SaveIntervals();
    void FS_SaveStandbyConfig();
    void FS_SaveCalDate();
    void FS_SaveSCDInfo();

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* FLASH_STORAGE_H */

