#include "main.h"

float g_temperature = 0;

void state_check_motion() {
    static int16_t prevGX=0, prevGY=0, prevGZ=0;
    static int16_t prevAX=0, prevAY=0, prevAZ=0;
    static int hiCount=0, loCount=0;
    static const char *state = "WAIT";
    int16_t ax, ay, az, gx, gy, gz, temp;
    int r = mpu_read(&ax, &ay, &az, &gx, &gy, &gz, &temp);
    if(r) {
        //os_printf("MPU READ FAILED\r\n");
        g_dryState = DRY_STATE_ERROR;
    }
    else {
        ax /= 30;
        ay /= 30;
        az /= 30;
        gx /= 30;
        gy /= 30;
        gz /= 30;
        int ad = abs(ax-prevAX)+abs(ay-prevAY)+abs(az-prevAZ);
        int gd = abs(gx-prevGX)+abs(gy-prevGY)+abs(gz-prevGZ);
        g_temperature = (temp / 340.0) + 36.53;

        if(ad > g_config.drySensitivity) hiCount++;
        else loCount++;
        if(hiCount >= 5) {
            g_dryState = DRY_STATE_ON;
            hiCount = 0;
            loCount=0;
        }
        if(loCount >= 5) {
            g_dryState = DRY_STATE_OFF;
            loCount = 0;
            hiCount=0;
        }

        #if 0
            //os_printf("MPU A=%6d %6d %6d G=%6d %6d %6d T=%6d\r\n",
            //    ax, ay, az, gx, gy, gz, temp);
            char msg[256];
            os_sprintf(msg, "A %3d %3d %3d %3d\r\nG %3d %3d %3d %3d\r\nT %3d %s",
            ax, ay, az, ad,
            gx, gy, gz, gd,
            //abs(ax-prevAX), abs(ay-prevAY), abs(az-prevAZ), ad,
            //abs(gx-prevGX), abs(gy-prevGY), abs(gz-prevGZ), gd,
            temp, state);
            ssd1306_putstr(msg, 0, 0, false);
            ssd1306_power(1);
        #endif

        prevGX = gx;
        prevGY = gy;
        prevGZ = gz;
        prevAX = ax;
        prevAY = ay;
        prevAZ = az;
    }

    g_state = STATE_REDRAW;
    //g_needDisplayRefresh = true;
    //g_state = STATE_REFRESH_DISPLAY;
}
