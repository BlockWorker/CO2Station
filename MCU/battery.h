#ifndef BATTERY_H
#define	BATTERY_H

#include <xc.h>

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
    
    extern float bat_voltage; //in volts, or -1 when unknown
    extern float bat_energy; //in range [0, 1], or -1 when unknown

    void BAT_InitDriver();
    void BAT_Update();

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* BATTERY_H */

