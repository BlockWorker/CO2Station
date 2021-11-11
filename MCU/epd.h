#ifndef EPD_H
#define	EPD_H

#include <xc.h>
#include <stdbool.h>

#define EPD_ROWS 416
#define EPD_COLUMNS 240
#define EPD_ROWBYTES (EPD_COLUMNS / 8)
#define EPD_FRAME_BYTES (EPD_ROWBYTES * EPD_ROWS)

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
    
    extern uint8_t epd_framebuffer[EPD_FRAME_BYTES];
    extern bool epd_updating;

    void EPD_InitDriver();
    void EPD_StartUpdate();
    void EPD_Update_Tasks();

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* EPD_H */

