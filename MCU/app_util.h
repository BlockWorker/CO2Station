#ifndef APP_UTIL_H
#define	APP_UTIL_H

#include <xc.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    void APP_CRITICAL_ERROR(char* message);
    void APP_MINOR_ERROR(char* message);
    bool APP_USB_Available();

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* APP_UTIL_H */

