/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.170.0
        Device            :  PIC24FJ512GU405
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.61
        MPLAB 	          :  MPLAB X v5.45
*/

/**
  Section: Included Files
*/
#include <stdio.h>
#include <string.h>

#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/delay.h"
#include "scd.h"
#include "app_time.h"
#include "epd.h"
#include "battery.h"
#include "serial_interface.h"
#include "flash_storage.h"
#include "app_util.h"
#include "mcc_generated_files/rtcc.h"
#include "mcc_generated_files/usb/usb_device.h"
#include "mcc_generated_files/adc1.h"


enum { //bits 0-3: index, bit 4: sleep allowed, bit 5: SCD sleep + screen update events enabled
    APP_INIT = 0x00,
    APP_MEASURE = 0x31,
    APP_NOMEASURE = 0x32,
    APP_EPD_UPDATE = 0x24,
    APP_STANDBY = 0x15,
    APP_SHUTDOWN = 0x16
} app_state;

enum {
    APPSB_STANDBY_ARMED,
    APPSB_STANDBY_QUEUED,
    APPSB_WAKEUP_ARMED
} app_standby_state;


static bool power_pressed = false;
static bool measure_pressed = false;


void __attribute__((weak)) PowerButtonInterrupt() {
    power_pressed = true;
    IOCNEbits.IOCNE0 = 0;
}

void __attribute__((weak)) MeasureButtonInterrupt() {
    measure_pressed = true;
    IOCNEbits.IOCNE1 = 0;
}

void _update_screen(bool is_cal_suggested) {
    static char textBuffer[11];
    static datetime_t time, formattable;

    while (!RTCC_TimeGet(&time));
    Time_MakeFormattable(&time, &formattable);

    EPD_DrawBackground();

    if (scd_co2_ppm >= 0.0f && scd_co2_ppm < 10000.0f) {
        sprintf(textBuffer, "%4.0f", (double)scd_co2_ppm);
    } else {
        strcpy(textBuffer, "----");
    }
    EPD_MakeText(8, 56, textBuffer, 96);

    if (scd_temp_deg >= 0.0f) {
        sprintf(textBuffer, "%4.1f", (double)scd_temp_deg);
    } else {
        strcpy(textBuffer, "--.-");
    }
    EPD_MakeText(38, 168, textBuffer, 32);

    if (scd_rh_percent >= 0.0f) {
        sprintf(textBuffer, "%4.1f", (double)scd_rh_percent);
    } else {
        strcpy(textBuffer, "--.-");
    }
    EPD_MakeText(221, 168, textBuffer, 32);

    strftime(textBuffer, 11, "%Y-%m-%d", &formattable);
    EPD_MakeText(15, 8, textBuffer, 20);

    strftime(textBuffer, 11, "%H:%M", &formattable);
    EPD_MakeText(224, 8, textBuffer, 20);

    if (bat_energy >= 0.0f) {
        uint8_t num_battbars = (uint8_t)(bat_energy * 20.99f); //roughly map to range [0, 21) for equal rounding to [0, 20]
        uint16_t fb_addr = 21 * EPD_ROWBYTES + 2;
        for (; num_battbars > 0; num_battbars--) {
            epd_framebuffer[fb_addr] = 0xaa;
            epd_framebuffer[fb_addr + 1] = 0xaa;
            fb_addr += EPD_ROWBYTES;
            epd_framebuffer[fb_addr] = 0x55;
            epd_framebuffer[fb_addr + 1] = 0x55;
            fb_addr += EPD_ROWBYTES;
        }
    }

    if (USB_VBUS_GetValue()) EPD_DrawLightning();

    char bottomtext_type = 'D';
    if (APP_USB_Available()) bottomtext_type = 'U';
    else if (app_state == APP_STANDBY || app_standby_state == APPSB_STANDBY_QUEUED) bottomtext_type = 'S';
    else if (is_cal_suggested) bottomtext_type = 'C';
    EPD_DrawBottomText(bottomtext_type);

    EPD_StartUpdate();
}
/*
                         Main application
 */
int main(void) {
    static uint16_t screen_wakeups;
    static uint16_t bat_wakeups;
    static uint16_t sleep_wakeups;
    static bool power_longpress = false;
    static bool measure_longpress = false;
    static bool cal_queued = false;
    static int16_t cal_warmup = 0;
    
    app_state = APP_INIT;
    
    // initialize the device
    SYSTEM_Initialize();
    
    DELAY_milliseconds(5000);

    FS_Init();
    
    SCD_InitDriver();
    EPD_InitDriver();
    BAT_InitDriver();
    
    POWER_SW_SetInterruptHandler(&PowerButtonInterrupt);
    MEASURE_SW_SetInterruptHandler(&MeasureButtonInterrupt);
    
    screen_wakeups = 1;
    bat_wakeups = 1;
    sleep_wakeups = 2;
    
    SCD_PowerUp();
    
    app_state = APP_MEASURE;
    Time_SetNextWakeupAlarm();
    
    app_standby_state = Time_IsInStandbyTime() ? APPSB_WAKEUP_ARMED : APPSB_STANDBY_ARMED;
    
    while (1) {
        if (app_state == APP_EPD_UPDATE) {
            EPD_Update_Tasks();
            if (!epd_updating) {
                if (app_standby_state == APPSB_STANDBY_QUEUED) {
                    app_standby_state = APPSB_WAKEUP_ARMED;
                    app_state = APP_STANDBY;
                } else app_state = (scd_state == SCD_SHUTDOWN) ? APP_NOMEASURE : APP_MEASURE;
            }
        } else if (power_pressed) {
            DELAY_milliseconds(1000);
            power_longpress = !POWER_SW_GetValue();
        } else if (measure_pressed) {
            DELAY_milliseconds(1000);
            measure_longpress = !MEASURE_SW_GetValue();
        }
        
        if (USB_VBUS_GetValue()) {
            if (USBGetDeviceState() == DETACHED_STATE) {
                USBDeviceInit();
                USBDeviceAttach();
                screen_wakeups = 1;
                bat_wakeups = 1;
                wakeup_by_timer = true;
                DELAY_milliseconds(500);
            } else if (APP_USB_Available()) SERIAL_Tasks();
        } else if (USBGetDeviceState() != DETACHED_STATE) {
            USBModuleDisable();
            screen_wakeups = 1;
            bat_wakeups = 1;
            wakeup_by_timer = true;
        }
        
        if (measure_pressed && (app_state & 0x20) != 0 && app_state != APP_EPD_UPDATE) {
            if (measure_longpress) {
                cal_queued = true;
                cal_warmup = 300;
                if (app_state == APP_NOMEASURE) {
                    sleep_wakeups = cal_warmup / wakeup_interval_sec + 2;
                    if (sleep_wakeups < scd_run_dur_wakeups) sleep_wakeups = scd_run_dur_wakeups;
                    SCD_PowerUp();
                    app_state = APP_MEASURE;
                }
                GREEN_LED_SetHigh();
            } else {
                screen_wakeups = 1;
                wakeup_by_timer = true;
            }
        }
        
        if (power_pressed && !cal_queued) {
            if (power_longpress && app_state != APP_SHUTDOWN) {
                SCD_PowerDown();
                scd_co2_ppm = -1.0f;
                scd_temp_deg = -1.0f;
                scd_rh_percent = -1.0f;
                memset(epd_framebuffer, 0, EPD_FRAME_BYTES);
                EPD_StartUpdate();
                while (epd_updating) EPD_Update_Tasks();
                wakeup_by_timer = false;
                Time_DisableAlarm();
                app_state = APP_SHUTDOWN;
            } else switch (app_state) {
                case APP_SHUTDOWN:
                    screen_wakeups = 1;
                    bat_wakeups = 1;
                    sleep_wakeups = 2;
                    SCD_PowerUp();
                    app_state = APP_MEASURE;
                    wakeup_by_timer = true;
                    break;
                case APP_STANDBY:
                    SCD_PowerUp();
                    screen_wakeups = 1;
                    sleep_wakeups = 2;
                    app_state = APP_MEASURE;
                    wakeup_by_timer = true;
                    break;
                case APP_EPD_UPDATE:
                    break;
                default:
                    SCD_PowerDown();
                    screen_wakeups = 1;
                    scd_co2_ppm = -1.0f;
                    scd_temp_deg = -1.0f;
                    scd_rh_percent = -1.0f;
                    app_state = APP_STANDBY;
                    _update_screen(false);
                    while (epd_updating) EPD_Update_Tasks();
                    break;
            }
        }
        
        if (wakeup_by_timer) {
            wakeup_by_timer = false;
            
            if (--bat_wakeups == 0) {
                BAT_Update();
                bat_wakeups = battery_interval_wakeups;
            }
            
            if (app_standby_state == APPSB_WAKEUP_ARMED && !Time_IsInStandbyTime()) {
                if (app_state == APP_STANDBY) {
                    SCD_PowerUp();
                    screen_wakeups = 1;
                    sleep_wakeups = 2;
                    app_state = APP_MEASURE;
                }
                app_standby_state = APPSB_STANDBY_ARMED;
            }
            
            if (SCD_IsDataReady()) {
                SCD_ReadMeasurement();
            }
            
            if (app_standby_state == APPSB_STANDBY_ARMED && app_state != APP_EPD_UPDATE && !cal_queued && Time_IsInStandbyTime()) {
                if (app_state != APP_STANDBY) {
                    SCD_PowerDown();
                    screen_wakeups = 1;
                    scd_co2_ppm = -1.0f;
                    scd_temp_deg = -1.0f;
                    scd_rh_percent = -1.0f;
                    app_standby_state = APPSB_STANDBY_QUEUED;
                } else app_standby_state = APPSB_WAKEUP_ARMED;
            }
            
            bool cal_suggested = Time_IsCalSuggested() && !cal_queued;
            
            if ((app_state & 0x20) != 0 && !epd_updating && --screen_wakeups == 0) {
                _update_screen(cal_suggested);
                app_state = APP_EPD_UPDATE;
                screen_wakeups = screen_interval_wakeups;
            }
            
            if ((app_state & 0x20) != 0 && app_standby_state != APPSB_STANDBY_QUEUED && scd_sleep_dur_wakeups > 0 && scd_run_dur_wakeups > 0) {
                if (--sleep_wakeups == 0) {
                    if (scd_state == SCD_SHUTDOWN) {
                        sleep_wakeups = scd_run_dur_wakeups;
                        SCD_PowerUp();
                        if (app_state != APP_EPD_UPDATE) app_state = APP_MEASURE;
                    } else {
                        sleep_wakeups = scd_sleep_dur_wakeups;
                        SCD_PowerDown();
                        if (app_state != APP_EPD_UPDATE) app_state = APP_NOMEASURE;
                    }
                }
            }
            
            if (cal_queued) {
                cal_warmup -= wakeup_interval_sec;
                if (cal_warmup <= 0) {
                    cal_queued = false;
                    cal_warmup = 0;
                    SCD_TriggerCal();
                    GREEN_LED_SetLow();
                    Time_CalcNextCal();
                    FS_SaveCalDate();
                }
            }
            
            if (bat_energy >= 0.0f && bat_energy < 0.05f) {
                RED_LED_SetHigh();
                DELAY_milliseconds(200);
                RED_LED_SetLow();
            }
            
            DELAY_milliseconds(200);
            
            if (cal_suggested) {
                GREEN_LED_SetHigh();
                DELAY_milliseconds(200);
                GREEN_LED_SetLow();
            }
            
            Time_SetNextWakeupAlarm();
        }
        
        if (power_pressed) {
            power_pressed = false;
            power_longpress = false;
            IOCNEbits.IOCNE0 = 1;
        }
        
        if (measure_pressed) {
            measure_pressed = false;
            measure_longpress = false;
            IOCNEbits.IOCNE1 = 1;
        }
        
        if (!USB_VBUS_GetValue() && USBGetDeviceState() == DETACHED_STATE && (app_state & 0x10) != 0) {
            RCONbits.VREGS = 0;
            RCONbits.RETEN = 1;
            Sleep();
        }
    }

    return 1;
}
/**
 End of File
*/

