#include "main.h"

void ICACHE_FLASH_ATTR state_boot() {
    //test the display for a little while
    static int count = 0;
    if(!count) {
        //if(!i2c_scan_cb()) return;
        configLoad();

        int r = mpu_init(0);
        if(r) os_printf("MPU init failed\r\n");

        os_printf("init display...\r\n");
        r = ssd1306_init(g_config.displayAddr);
        if(r) {
            os_printf("ssd1306_init failed\r\n");
            g_state = STATE_REBOOT;
            return;
        }
        else os_printf("test display...\r\n");
    }
    count++;
    if(count == 64) {
        mpu_init2();
        ssd1306_test(0);
        ssd1306_power(0);
        count = 0;
        g_state = STATE_INIT;
    }
    else ssd1306_test(1);
}

void ICACHE_FLASH_ATTR state_reboot() {
    static bool didRestart = false;
    static int countdown = 100;

    gpio_write(4, countdown & 1); //blink LED
    if(countdown) countdown--;
    else {
        if(!didRestart) system_restart();
        didRestart = true;
    }
}

void ICACHE_FLASH_ATTR state_init() {
    static int countdown = 0;
    int r=0;

    if(countdown) {
        //delay so we can see boot message
        countdown--;
        if(!countdown) {
            ssd1306_clear(); //clear vram
            g_displayText[0] = 0;
            g_state = STATE_REDRAW;
        }
        return;
    }

    //Init the display.
    ssd1306_clear();
    ssd1306_power(1);
    if(!r) {
        os_printf("Init OK\r\n");
        countdown = 30;
    }
}
