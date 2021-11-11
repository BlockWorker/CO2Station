#include "app_util.h"
#include <stdio.h>
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/delay.h"
#include "mcc_generated_files/usb/usb.h"

void APP_CRITICAL_ERROR(char* message) {
    static char buffer[250];
    
    RED_LED_SetHigh();
    GREEN_LED_SetHigh();
    if (APP_USB_Available()) {
        sprintf(buffer, "%s\n", message);
        putsUSBUSART(buffer);
        while (!USBUSARTIsTxTrfReady()) CDCTxService();
    }
    while (true);
}

void APP_MINOR_ERROR(char* message) {
    static char buffer[250];
    
    uint16_t i;
    for (i = 0; i < 3; i++) {
        DELAY_milliseconds(100);
        RED_LED_SetHigh();
        DELAY_milliseconds(100);
        RED_LED_SetLow();
    }
    
    if (APP_USB_Available()) {
        sprintf(buffer, "%s\n", message);
        putsUSBUSART(buffer);
    }
}

bool APP_USB_Available() {
    return USBGetDeviceState() >= CONFIGURED_STATE && !USBIsDeviceSuspended();
}
