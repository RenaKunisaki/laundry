// SSD1306 I2C OLED display module
// 128 x 32 pixels @ 1 bit per pixel
#ifndef __SSD1306_H__
#define __SSD1306_H__

#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "mem.h"

#define SSD1306_ADDRESS 0x3C
#define SSD1306_WIDTH   128
#define SSD1306_HEIGHT  32
#define SSD1306_MAX_PACKET_LEN 16 //including prefix byte

#define SSD1306_PREFIX_CMD  0x00 //first byte of command packet
#define SSD1306_PREFIX_DATA 0x40 //first byte of data packet
//note this is reversed from what some examples say...

#define SSD1306_SET_CONTRAST             0x81 //followed by 00-FF
#define SSD1306_DISPLAY_NORMAL           0xA4 //display contents of RAM
#define SSD1306_DISPLAY_TEST             0xA5 //test mode, light all pixels
#define SSD1306_INVERT_OFF               0xA6 //do not invert pixels
#define SSD1306_INVERT_ON                0xA7 //invert pixels (1=off, 0=on)
#define SSD1306_DISPLAY_OFF              0xAE //turn display off
#define SSD1306_DISPLAY_ON               0xAF //turn display on
#define SSD1306_CONTINUOUS_SCROLL_RIGHT  0x26
#define SSD1306_CONTINUOUS_SCROLL_LEFT   0x27
#define SSD1306_CONTINUOUS_SCROLL_RIGHTV 0x29 //right+vertical
#define SSD1306_CONTINUOUS_SCROLL_LEFTV  0x2A //left+vertical
#define SSD1306_DISABLE_SCROLL           0x2E //stop scrolling
#define SSD1306_ENABLE_SCROLL            0x2F //start scrolling
#define SSD1306_SET_VSCROLL_AREA         0xA3

#define SSD1306_SET_ADDRESSING_MODE      0x20
#define SSD1306_ADDRESSING_HORIZONTAL    0x00
#define SSD1306_ADDRESSING_VERTICAL      0x01
#define SSD1306_ADDRESSING_PAGE          0x02

#define SSD1306_SET_COL_START_LO         0x00 //low 4 bits = start addr low nybble
#define SSD1306_SET_COL_START_HI         0x10 //low 4 bits = start addr high nybble
#define SSD1306_SET_PAGE_START           0xB0 //low 3 bits = start addr

// only used for horizontal/vertical addressing mode
#define SSD1306_SET_COLUMN_ADDR          0x21 //start, end
#define SSD1306_SET_PAGE_ADDR            0x22 //start, end

#define SSD1306_SET_START_LINE           0x40 //low 6 bits = start line
#define SSD1306_SEGMENT_REMAP_OFF        0xA0
#define SSD1306_SEGMENT_REMAP_ON         0xA1 //effectively horizontal flip
#define SSD1306_SET_MULTIPLEX            0xA8
#define SSD1306_SET_COM_ASCENDING        0xC0
#define SSD1306_SET_COM_DESCENDING       0xC8 //effectively vertical flip
#define SSD1306_SET_DISPLAY_OFFSET       0xD3
#define SSD1306_SET_COM_CONFIG           0xDA
#define SSD1306_SET_CLOCK_DIVIDE         0xD5
#define SSD1306_SET_PRECHARGE            0xD9
#define SSD1306_SET_VCOMM_DESELECT       0xDB
#define SSD1306_SET_CHARGE_PUMP          0x8D //undocumented!?
#define SSD1306_NOP                      0xE3 //no operation

//Macros to generate batch commands
#define SSD1306_BATCHCMD_SET_X(x) \
    1, SSD1306_SET_COL_START_LO | ((x) & 0x0F), \
    1, SSD1306_SET_COL_START_HI | (((x) >> 4) & 0x0F)
#define SSD1306_BATCHCMD_SET_Y(y) \
    1, SSD1306_SET_PAGE_START   | ((y) >> 2)
#define SSD1306_BATCHCMD_SET_XY(x, y) \
    SSD1306_BATCHCMD_SET_X(x), \
    SSD1306_BATCHCMD_SET_Y(y)

int ssd1306_init(int address);
int ssd1306_sendCmd(uint8_t cmd, const void *params, uint32_t paramLen);
int ssd1306_sendData(const void *data, uint32_t len);
int ssd1306_sendBatchCmds(const uint8_t *cmds);
int ssd1306_power(int on);
int ssd1306_test(int on);
int ssd1306_setXFlip(bool flip);
int ssd1306_setYFlip(bool flip);
int ssd1306_setContrast(uint8_t contrast);
int ssd1306_setPos(int x, int y);
int ssd1306_clear();
void ssd1306_putpixel(int x, int y, bool p);
int ssd1306_refresh();
int ssd1306_putchr(char c, int x, int y);
int ssd1306_putchrbig(char c, int x, int y);
int ssd1306_putstr(const char *str, int x, int y, bool big);

#endif //__SSD1306_H__
