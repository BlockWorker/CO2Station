#include <string.h>
#include <libpic30.h>
#include "epd.h"
#include "mcc_generated_files/spi1.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/delay.h"
#include "scd.h"
#include "background.h"
#include "bottom_text.h"
#include "font_20px.h"
#include "font_32px.h"
#include "font_96px.h"

#define _epd_maketext_font(x, y, text, font) _epd_maketext(x, y, text, __builtin_tbladdress(&font##_data), font##_char_offsets, \
                                                        FONT_##font##_CHAR_WIDTH, FONT_##font##_CHAR_HEIGHT_BYTES)

enum {
    EPD_NONE,
    EPD_POWERUP,
    EPD_REFRESH,
    EPD_POWERDOWN
} epd_update_state;

const uint8_t lightning_data[28] = {
    0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0xC3, 0xC0, 0xE3, 0xE0, 0xFB, 0xF0, 0xFF, 0xF8,
    0xFF, 0xBC, 0xFF, 0x8E, 0x3F, 0x83, 0x0F, 0x80, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00
};

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

bool _epd_maketext(uint16_t x, uint16_t y, char* text, 
        _prog_addressT font_data, const int32_t* font_offsets, uint16_t font_charwidth, uint8_t font_charheight_bytes) {
    if (y >= EPD_COLUMNS || y % 8 != 0) return false;
    
    uint16_t length = 0;
    while (text[length]) length++;
    if (length == 0) return false;
    
    uint16_t width = length * font_charwidth;
    uint16_t char_x = x + width - 1;
    if (char_x >= EPD_ROWS) return false;
    
    uint8_t* framebuf_dest = epd_framebuffer + ((EPD_ROWS - char_x - 1) * EPD_ROWBYTES) + (y / 8);
    
    int16_t char_index;
    for (char_index = length - 1; char_index >= 0; char_index--) {
        uint8_t charval = ((uint8_t*)text)[char_index];
        int32_t offset = font_offsets[charval];
        if (offset >= 0) {
            _prog_addressT p_addr = font_data + offset;
            
            uint16_t row;
            for (row = 0; row < font_charwidth; row++) {
                p_addr = _memcpy_p2d16(framebuf_dest + (row * EPD_ROWBYTES), p_addr, font_charheight_bytes);
            }
        }
        framebuf_dest += font_charwidth * EPD_ROWBYTES;
    }
    
    return true;
}

bool EPD_MakeText(uint16_t x, uint16_t y, char* text, uint8_t font_size) {
    switch (font_size) {
        case 20:
            return _epd_maketext_font(x, y, text, font_20px);
        case 32:
            return _epd_maketext_font(x, y, text, font_32px);
        case 96:
            return _epd_maketext_font(x, y, text, font_96px);
        default:
            return false;
    }
}

void EPD_DrawBackground() {
    _prog_addressT p_addr;
    _init_prog_address(p_addr, background_data);
    _memcpy_p2d16(epd_framebuffer, p_addr, EPD_FRAME_BYTES);
}

void EPD_DrawBottomText(char type) {
    static char str[2] = { 0, 0 };
    str[0] = type;
    _epd_maketext_font((EPD_ROWS - FONT_bottom_text_CHAR_WIDTH) / 2, 208, str, bottom_text);
}

void EPD_DrawLightning() {
    uint16_t fb_addr = 34 * EPD_ROWBYTES + 2;
    uint16_t data_addr = 0;
    uint16_t i;
    for (i = 0; i < 14; i++) {
        epd_framebuffer[fb_addr] = lightning_data[data_addr++];
        epd_framebuffer[fb_addr + 1] = lightning_data[data_addr++];
        fb_addr += EPD_ROWBYTES;
    }
}
