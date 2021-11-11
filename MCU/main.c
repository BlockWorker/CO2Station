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

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/delay.h"
#include "scd.h"
#include "mcc_generated_files/crc.h"
#include "mcc_generated_files/usb/usb_device_cdc.h"
#include "app_time.h"
#include "epd.h"

/*
                         Main application
 */
int main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    scd_cal_ref_ppm = 400;
    scd_measure_interval_sec = 2;
    scd_alt_comp_m = 290;
    scd_temp_offset = 0;
    SCD_InitDriver();
    EPD_InitDriver();
    
    //SCD_PowerUp();
    
    static bool epd_checker_written = false;
    
    DELAY_milliseconds(5000);
    EPD_StartUpdate();
    
    while (1)
    {
        //Time_Tasks();
        EPD_Update_Tasks();
        CDCTxService();
        
        if (!epd_updating && !epd_checker_written) {
            uint16_t i, j;
            for (i = 0; i < EPD_ROWS; i++) {
                bool invert = (i / 8) % 2 == 0;
                for (j = 0; j < EPD_COLUMNS; j++) {
                    epd_framebuffer[EPD_ROWBYTES * i + j] = ((j % 2 == 0) ^ invert) ? 0xff : 0x00;
                }
            }
            DELAY_milliseconds(5000);
            EPD_StartUpdate();
            epd_checker_written = true;
        }
    }

    return 1;
}
/**
 End of File
*/

