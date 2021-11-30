#include <stdio.h>

#include "scd.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/crc.h"
#include "mcc_generated_files/i2c1.h"
#include "mcc_generated_files/delay.h"
#include "app_util.h"
#include "mcc_generated_files/usb/usb_device_cdc.h"

#define SCD_I2C_ADDR 0x61

uint16_t scd_cal_ref_ppm;
uint16_t scd_measure_interval_sec;
uint16_t scd_alt_comp_m;
uint16_t scd_temp_offset;
uint16_t scd_sleep_dur_wakeups;
uint16_t scd_run_dur_wakeups;

scd_state_t scd_state; //State of the module
bool scd_nvm_initialized; //Whether values stored in SCD NVM have been sent

float scd_co2_ppm; //CO2 concentration of last measurement in ppm, -1 if unknown
float scd_rh_percent; //Relative humidity of last measurement in %, -1 if unknown
float scd_temp_deg; //Temperature of last measurement in °C, -1 if unknown

bool _scd_write(uint16_t command, uint16_t argument_value, bool send_argument) {
    static uint8_t write_data[5];
    static I2C1_MESSAGE_STATUS write_status;
    
    write_data[0] = command >> 8;
    write_data[1] = command & 0xff;
    uint8_t length = 2;
    
    if (send_argument) {
        write_data[2] = argument_value >> 8;
        write_data[3] = argument_value & 0xff;
        CRC_SeedSet(0xfc, CRC_SEED_METHOD_INDIRECT, CRC_SEED_DIRECTION_Msb);
        CRC_CalculateBufferStart(write_data + 2, 2);
        while (!CRC_CalculationIsDone());
        write_data[4] = CRC_CalculationResultGet(false, 0);
        length = 5;
    }
    
    while (!I2C1_MasterQueueIsEmpty());
    
    write_status = I2C1_MESSAGE_PENDING;
    I2C1_MasterWrite(write_data, length, SCD_I2C_ADDR, &write_status);
    
    uint16_t timeout;
    for (timeout = 2000; timeout > 0; timeout--) {
        if (write_status == I2C1_MESSAGE_COMPLETE) return true;
        else if (write_status == I2C1_MESSAGE_FAIL || write_status == I2C1_LOST_STATE) return false;
        DELAY_milliseconds(1);
    }
    return false;
}

bool _scd_read(uint16_t* data, uint16_t data_words) {
    static uint8_t read_data[18];
    static I2C1_MESSAGE_STATUS read_status;
    
    while (!I2C1_MasterQueueIsEmpty());
    
    read_status = I2C1_MESSAGE_PENDING;
    I2C1_MasterRead(read_data, data_words * 3, SCD_I2C_ADDR, &read_status);
    
    uint16_t timeout;
    for (timeout = 2000; timeout > 0; timeout--) {
        if (read_status == I2C1_MESSAGE_COMPLETE) break;
        else if (read_status == I2C1_MESSAGE_FAIL || read_status == I2C1_LOST_STATE) return false;
        DELAY_milliseconds(1);
    }
    if (timeout == 0) return false;
    
    uint8_t crc_result = 0;
    uint16_t word;
    for (word = 0; word < data_words; word++) {
        CRC_SeedSet(0xfc, CRC_SEED_METHOD_INDIRECT, CRC_SEED_DIRECTION_Msb);
        CRC_CalculateBufferStart(read_data + 3 * word, 3);
        while (!CRC_CalculationIsDone());
        crc_result |= CRC_CalculationResultGet(false, 0);
    }
    if (crc_result != 0) return false;
    
    for (word = 0; word < data_words; word++) {
        uint16_t array_loc = 3 * word;
        data[word] = ((uint16_t)read_data[array_loc] << 8) | (uint16_t)read_data[array_loc + 1];
    }
    return true;
}

void SCD_InitDriver() {
    scd_state = SCD_SHUTDOWN;
    scd_nvm_initialized = false;
    scd_co2_ppm = -1.0f;
    scd_rh_percent = -1.0f;
    scd_temp_deg = -1.0f;
}

void SCD_PowerDown() {
    if (scd_state == SCD_SHUTDOWN) return;
    
    scd_state = SCD_SHUTDOWN;
    
    SCD_PDN_SetHigh();
}

void SCD_PowerUp() {
    if (scd_state != SCD_SHUTDOWN) return;
    
    scd_state = SCD_INITIALIZING;
    
    SCD_PDN_SetLow();
    
    DELAY_milliseconds(2000);
    
    if (!scd_nvm_initialized) SCD_SendConfig();
    
    scd_state = SCD_CONFIGURED;
}

bool SCD_IsDataReady() {
    if (scd_state != SCD_CONFIGURED) return false;
    return SCD_RDY_GetValue() != 0;
}

void SCD_ReadMeasurement() {
    static uint16_t read_data[6];
    
    if (scd_state != SCD_CONFIGURED) return;
    
    if (!_scd_write(0x0300, 0, false)) {
        APP_MINOR_ERROR("Error: Failed to trigger SCD measurement read");
        return;
    }
    
    DELAY_milliseconds(5);
    
    if (_scd_read(read_data, 6)) {
        uint32_t co2_int = ((uint32_t)read_data[0] << 16) | (uint32_t)read_data[1];
        uint32_t t_int = ((uint32_t)read_data[2] << 16) | (uint32_t)read_data[3];
        uint32_t rh_int = ((uint32_t)read_data[4] << 16) | (uint32_t)read_data[5];
        
        scd_co2_ppm = *(float*)&co2_int;
        scd_temp_deg = *(float*)&t_int;
        scd_rh_percent = *(float*)&rh_int;
    } else APP_MINOR_ERROR("Error: Failed to read SCD measurement data");
}

void SCD_SendConfig() {
    if (scd_state == SCD_SHUTDOWN) return;
    
    bool success = true;
    success &= _scd_write(0x4600, scd_measure_interval_sec, true); //Write measurement interval
    success &= _scd_write(0x5306, 0, true); //Disable ASC
    success &= _scd_write(0x5403, scd_temp_offset, true); //Write temperature offset
    success &= _scd_write(0x5102, scd_alt_comp_m, true); //Write altitude compensation
    success &= _scd_write(0x0010, 0, true); //Start continuous measurement, no pressure compensation
    
    if (success) scd_nvm_initialized = true;
    
    if (!success) APP_MINOR_ERROR("Error: Failed to configure SCD");
}

void SCD_ReadOffset() {
    static uint16_t value = 0;
    static char buffer[10];
    
    if (!APP_USB_Available()) return;
    
    _scd_write(0x5403, 0, false);
    DELAY_milliseconds(5);
    _scd_read(&value, 1);
    
    sprintf(buffer, "%d", value);
    putsUSBUSART(buffer);
}

void SCD_TriggerCal() {
    if (scd_state != SCD_CONFIGURED) return;
    
    if (!_scd_write(0x5204, scd_cal_ref_ppm, true)) APP_MINOR_ERROR("Error: Failed to trigger SCD calibration");
}
