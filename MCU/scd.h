#ifndef SCD_H
#define	SCD_H

#include <xc.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
    
    typedef enum {
        SCD_SHUTDOWN = 0,
        SCD_INITIALIZING = 1,
        SCD_CONFIGURED = 2
    } scd_state_t;
    

    extern uint16_t scd_cal_ref_ppm; //Sensor calibration reference value, in ppm
    extern uint16_t scd_measure_interval_sec; //Sensor measurement interval, in seconds
    extern uint16_t scd_alt_comp_m; //Sensor altitude calibration, in meters above sea level
    extern uint16_t scd_temp_offset; //Sensor temperature offset, in hundredths of degrees C
    
    extern scd_state_t scd_state; //State of the module
    extern bool scd_busy; //Whether there is an ongoing data transfer
    
    extern float scd_co2_ppm; //CO2 concentration of last measurement in ppm, -1 if unknown
    extern float scd_rh_percent; //Relative humidity of last measurement in %, -1 if unknown
    extern float scd_temp_deg; //Temperature of last measurement in °C, -1 if unknown
    
    
    void SCD_InitDriver();
    
    void SCD_PowerDown();
    void SCD_PowerUp();
    
    bool SCD_IsDataReady();
    void SCD_ReadMeasurement();
    void SCD_SendConfig();
    void SCD_TriggerCal();

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* SCD_H */

