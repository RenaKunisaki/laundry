#include "main.h"

void hexdump(const void *data, uint32_t len) {
    static const char *hex = "0123456789ABCDEF";
    const char *buf = (const char*)data;

    uint32_t i, j;
    //printf("hexdump 0x%X bytes:\r\n", len);
    for(i=0; i<len; i += 16) {
        os_printf("%04X  ", i);
        for(j=0; j<16; j++) {
            if(i+j >= len) os_printf(".. ");
            else os_printf("%02X ", buf[i+j]);
            if((j & 3) == 3) os_printf(" ");
        }
        for(j=0; j<16; j++) {
            if(i+j >= len) os_printf(".");
            else if(buf[i+j] >= 0x20 && buf[i+j] <= 0x7E) {
                os_printf("%c", buf[i+j]);
            }
            else os_printf(".");
            if((j & 3) == 3) os_printf(" ");
        }
        os_printf("\r\n");
    }
}

int ICACHE_FLASH_ATTR
i2c_scan_cb() {
    //Debug: scan for I2C devices
    //Call it from the timer callback.
    //Returns 1 if done. Can call again to begin new scan.
    static int addr = 8;

    i2c_start();
    if(!(addr & 7)) os_printf("\r\n%02X  ", addr);
    if(i2c_write(addr << 1)) os_printf("%02X ", addr);
    else os_printf("-- ");
    addr++;
    if(addr == 0x78) {
        addr = 8;
        os_printf("\r\n");
        return 1;
    }
    return 0;
}
