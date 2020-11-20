#include "main.h"

void ICACHE_FLASH_ATTR state_refresh_display() {
    //Send vram to display
    //gpio_write(4, 0); //LED on
    if(g_needDisplayRefresh) {
        ssd1306_setXFlip(g_config.displayFlipX);
        ssd1306_setYFlip(g_config.displayFlipY);
        ssd1306_setContrast(g_config.displayContrast);
        int r = ssd1306_refresh();
        if(!r) g_state = STATE_SEND_BEACON;
        else {
            os_printf("display refresh failed\r\n");
            g_state = STATE_REBOOT;
            //g_state = STATE_IDLE;
        }
        //gpio_write(4, 1); //LED off
        ssd1306_clear(); //clear vram
        //g_state = STATE_IDLE;
        g_needDisplayRefresh = false;
    }
    else g_state = STATE_SEND_BEACON;
}

void ICACHE_FLASH_ATTR state_redraw() {
    //Redraw the display.
    //static int blinkTimer = 0;
    static int prevWash = -1;
    static int prevDry = -1;
    int i;

    #if 0 //debug
        ssd1306_power(1);
        ssd1306_clear();
        for(i=0; i<NUM_LEDS; i++) {
            int px = g_ledVal[i] / 8;
            int j; for(j=0; j<128; j++) {
                ssd1306_putpixel(j, i*3, j < px);
            }
        }
        g_needDisplayRefresh = true;
        g_state = STATE_REFRESH_DISPLAY;
        //g_state = STATE_SEND_BEACON;
        return;
    #endif

    os_printf("w:%d d:%d\r\n", g_washState, g_dryState);
    bool on = !(g_washState == WASH_STATE_OFF
        && g_dryState == DRY_STATE_OFF);

    if(g_ledVal[LED_ROOM] < g_config.ledOffSensitivity[LED_ROOM]) {
        on = false;
    }

    if(!on) {
        ssd1306_power(0);
        g_state = STATE_SEND_BEACON;
    }
    else {
        static const char *washStateNames[] = {
            "---", "OFF", "FILL",
            "WASH", "RINSE", "SPIN",
            "DONE", " ERROR  ", "SENS ERR"};
        static const char *dryStateNames[] = {
            "---", "OFF", "ON", "ERROR"};

        if((g_washState != prevWash)
        || (g_dryState  != prevDry)) {
            ssd1306_putstr("WASH:", 0,  4, false);
            ssd1306_putstr("DRY:",  0, 20, false);
            ssd1306_putstr(
                washStateNames[g_washState], 36, 0, true);
            ssd1306_putstr(
                dryStateNames[g_dryState], 36, 16, true);
            g_needDisplayRefresh = true;
            g_state = STATE_REFRESH_DISPLAY;
        }
        else g_state = STATE_SEND_BEACON;
        ssd1306_power(1);
        prevWash = g_washState;
        prevDry  = g_dryState;
    }
    //blinkTimer = (blinkTimer+1) & 7;
}
