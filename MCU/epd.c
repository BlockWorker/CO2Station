#include <string.h>
#include "epd.h"
#include "mcc_generated_files/spi1.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/delay.h"
#include "scd.h"

enum {
    EPD_NONE,
    EPD_POWERUP,
    EPD_REFRESH,
    EPD_POWERDOWN
} epd_update_state;

uint8_t epd_framebuffer[EPD_FRAME_BYTES];

bool epd_updating;

void _epd_send(uint8_t command, uint8_t* data, uint16_t data_bytes) {
    EPD_DC_SetLow();
    DELAY_microseconds(1);
    EPD_CSB_SetLow();
    DELAY_microseconds(20);
    SPI1_Exchange8bit(command);
    DELAY_microseconds(20);
    EPD_CSB_SetHigh();
    DELAY_microseconds(1);
    EPD_DC_SetHigh();
    DELAY_microseconds(1);
    
    if (data_bytes > 0) {
        EPD_CSB_SetLow();
        DELAY_microseconds(20);
        SPI1_Exchange8bitBuffer(data, data_bytes, NULL);
        DELAY_microseconds(20);
        EPD_CSB_SetHigh();
    }
}

void EPD_InitDriver() {
    epd_updating = false;
    epd_update_state = EPD_NONE;
    memset(epd_framebuffer, 0, EPD_FRAME_BYTES);
}

void EPD_StartUpdate() {
    static uint8_t epd_cmd_args[5] = { 0x0e, 0, 0x02, 0x0f, 0x89 }; //fixed command arguments: soft reset, temp, temp active, PSR (2B)
    
    if (epd_updating) return;
    epd_updating = true;
    
    if (scd_temp_deg >= 0.0f) epd_cmd_args[1] = (uint8_t)(scd_temp_deg + 0.5f); //get temperature from measurement
    else epd_cmd_args[1] = 25; //assume 25° if no measurement
    
    //First section: "Power On COG Driver"
    EPD_PDN_SetLow();
    DELAY_milliseconds(5);
    EPD_RST_N_SetHigh();
    DELAY_milliseconds(5);
    EPD_RST_N_SetLow();
    EPD_CSB_SetHigh();
    DELAY_milliseconds(10);
    EPD_RST_N_SetHigh();
    DELAY_milliseconds(5);
    _epd_send(0x00, epd_cmd_args, 1);
    
    //Second section: "Set environment temperature and PSR"
    _epd_send(0xe5, epd_cmd_args + 1, 1);
    _epd_send(0xe0, epd_cmd_args + 2, 1);
    _epd_send(0x00, epd_cmd_args + 3, 2);
    
    //Third section: "Input image to the EPD"
    _epd_send(0x10, epd_framebuffer, EPD_FRAME_BYTES);
    _epd_send(0x13, NULL, EPD_FRAME_BYTES);
    
    //Fourth section (start): "Send Updating Command"
    while (!EPD_BUSY_N_GetValue());
    _epd_send(0x04, NULL, 0);
    epd_update_state = EPD_POWERUP;
}

void EPD_Update_Tasks() {
    if (epd_update_state == EPD_NONE) return;
    
    if (!EPD_BUSY_N_GetValue()) return;
    
    switch (epd_update_state) {
        case EPD_POWERUP:
            //Fourth section (continued): "Send Updating Command"
            _epd_send(0x12, NULL, 0);
            epd_update_state = EPD_REFRESH;
            break;
        case EPD_REFRESH:
            //Fifth section (start): "Turn-off DC/DC"
            _epd_send(0x02, NULL, 0);
            epd_update_state = EPD_POWERDOWN;
            break;
        case EPD_POWERDOWN:
            //Fifth section (continued): "Turn-off DC/DC"
            EPD_RST_N_SetDigitalInput();
            EPD_CSB_SetLow();
            EPD_DC_SetLow();
            EPD_PDN_SetHigh();
            DELAY_milliseconds(150);
            EPD_RST_N_SetLow();
            EPD_RST_N_SetDigitalOutput();
        default: //deliberate fall-through
            epd_update_state = EPD_NONE;
            epd_updating = false;
            break;
    }
}
