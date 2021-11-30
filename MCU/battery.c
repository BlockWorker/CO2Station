#include "battery.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/delay.h"
#include "mcc_generated_files/adc1.h"
#include <math.h>

#define COUNTS_PER_VOLT 958.3664f
#define VOLT_OFFSET 0.0f

float bat_voltage;
float bat_energy;

void BAT_InitDriver() {
    bat_voltage = -1.0f;
    bat_energy = -1.0f;
    ADC1_Disable();
}

void BAT_Update() {
    //measure
    BAT_SENSE_EN_SetHigh();
    ADC1_Enable();
    DELAY_milliseconds(10);
    ADC1_SoftwareTriggerEnable();
    DELAY_milliseconds(2);
    ADC1_SoftwareTriggerDisable();
    while (!ADC1_IsConversionComplete(BAT_SENSE));
    
    uint16_t counts = ADC1_ConversionResultGet(BAT_SENSE);
    ADC1_Disable();
    BAT_SENSE_EN_SetLow();
    
    //get voltage
    bat_voltage = (float)counts / COUNTS_PER_VOLT + VOLT_OFFSET;
    
    //approximate energy - https://www.desmos.com/calculator/zpqitddywu
    float bat_wh = 0.0f;
    if (bat_voltage <= 3.0f) {
        bat_wh = 0.0f;
    } else if (bat_voltage < 3.255f) {
        bat_wh = 0.00560035f * (expf(17.6585f * (bat_voltage - 3.0f)) - 1.0f);
    } else if (bat_voltage < 3.46f) {
        bat_wh = 8.82927f * (bat_voltage - 3.255f) + 0.5f;
    } else if (bat_voltage < 3.645f) {
        bat_wh = 19.0405f * (bat_voltage - 3.46f) + 2.31f;
    } else if (bat_voltage < 4.098f) {
        bat_wh = 12.6214f * (bat_voltage - 3.645f) + 5.8325f;
    } else if (bat_voltage < 4.2f) {
        bat_wh = 12.0f - 0.0273117f * (expf(28.0476f * (4.2f - bat_voltage)) - 1.0f);
    } else {
        bat_wh = 12.0f;
    }
    bat_energy = bat_wh / 12.0f;
}
