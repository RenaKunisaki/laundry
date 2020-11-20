#include "driver/ssd1306.h"
#include "driver/font.h"
static int i2c_id = SSD1306_ADDRESS;

static const uint8_t zeros[] = {
    0, 0, 0, 0,   0, 0, 0, 0,
    0, 0, 0, 0,   0, 0, 0, 0,
    0, 0, 0, 0,   0, 0, 0, 0,
    0, 0, 0, 0,   0, 0, 0, 0,
};
static uint8_t vram[SSD1306_WIDTH*SSD1306_HEIGHT/4];

int ssd1306_init(int address) {
    if(address != 0) i2c_id = address;
    else i2c_id = SSD1306_ADDRESS;

    memset(vram, 0, sizeof(vram));

    static const uint8_t cmds[] = {
        1, SSD1306_DISPLAY_OFF,
        2, SSD1306_SET_CLOCK_DIVIDE,    0x80,
        2, SSD1306_SET_MULTIPLEX,       0x3F,
        2, SSD1306_SET_DISPLAY_OFFSET,  0x00,
        2, SSD1306_SET_CHARGE_PUMP,     0x14, //14=enable 10=disable
        1, SSD1306_INVERT_OFF,
        1, SSD1306_DISPLAY_NORMAL,
        2, SSD1306_SET_START_LINE,      0x00,
        2, SSD1306_SET_COM_CONFIG,      0x12,
        2, SSD1306_SET_CONTRAST,        0x8F,
        2, SSD1306_SET_PRECHARGE,       0xF1,
        2, SSD1306_SET_VCOMM_DESELECT,  0x40,
        1, SSD1306_DISABLE_SCROLL,
        2, SSD1306_SET_ADDRESSING_MODE, SSD1306_ADDRESSING_PAGE,
        //1, SSD1306_SEGMENT_REMAP_OFF, //no H flip
        //1, SSD1306_SET_COM_ASCENDING, //no V flip
        1, SSD1306_SEGMENT_REMAP_ON, //H flip
        1, SSD1306_SET_COM_DESCENDING, //V flip
        1, SSD1306_DISPLAY_ON,
        //1, SSD1306_DISPLAY_TEST,
        0};
    return ssd1306_sendBatchCmds(cmds);
}

int ssd1306_sendCmd(uint8_t cmd, const void *params, uint32_t paramLen) {
    bool success;
    int i;

    for(i=0; i<10; i++) {
        i2c_start();
        success = i2c_write(i2c_id << 1);
        if(success) break;
        i2c_stop();
        os_delay_us(100);
    }
    success = success && i2c_write(SSD1306_PREFIX_CMD);
    success = success && i2c_write(cmd);

    for(i=0; i<paramLen && params != NULL; i++) {
        const uint8_t *prm = (const uint8_t*)params;
        success = success && i2c_write(prm[i]);
    }

    i2c_stop();
    return success ? 0 : -1;
}

int ssd1306_sendData(const void *data, uint32_t len) {
    /* What a horrible wasteful protocol.
    For each byte we transfer, every second bit is IGNORED.
    Each byte sets one column of FOUR pixels.
    Seems like they adapted this from the 128x64 model by just
    throwing out every second row for whatever reason. Maybe so you
    can use the same images for both sizes if you don't mind having
    every second line missing?
     */

    bool success;
    i2c_start();
    success = i2c_write(i2c_id << 1);
    success = success && i2c_write(SSD1306_PREFIX_DATA);

    int i;
    for(i=0; i<len && data != NULL; i++) {
        const uint8_t *d = (const uint8_t*)data;
        success = success && i2c_write(d[i]);
    }

    i2c_stop();
    return success ? 0 : -1;
}

int ssd1306_sendBatchCmds(const uint8_t *cmds) {
    int idx = 0, r=0;
    while(cmds[idx]) {
        uint8_t len = cmds[idx++];
        uint8_t cmd = cmds[idx++];
        const uint8_t *params = &cmds[idx];
        //os_printf("Send cmd 0x%02X len 0x%02X idx 0x%02X\r\n",
        //    cmd, len, idx-2);
        r = ssd1306_sendCmd(cmd, params, len-1);
        if(r) return r;
        idx += len - 1;
    }
    return r;
}

int ssd1306_power(int on) {
    return ssd1306_sendCmd(
        on ? SSD1306_DISPLAY_ON : SSD1306_DISPLAY_OFF,
        NULL, 0);
}

int ssd1306_test(int on) {
    return ssd1306_sendCmd(
        on ? SSD1306_DISPLAY_TEST : SSD1306_DISPLAY_NORMAL,
        NULL, 0);
}

int ssd1306_setXFlip(bool flip) {
    return ssd1306_sendCmd(
        flip ? SSD1306_SEGMENT_REMAP_ON : SSD1306_SEGMENT_REMAP_OFF,
        NULL, 0);
}

int ssd1306_setYFlip(bool flip) {
    return ssd1306_sendCmd(
        flip ? SSD1306_SET_COM_DESCENDING : SSD1306_SET_COM_ASCENDING,
        NULL, 0);
}

int ssd1306_setContrast(uint8_t contrast) {
    return ssd1306_sendCmd(SSD1306_SET_CONTRAST, &contrast, 1);
}

int ssd1306_setPos(int x, int y) {
    /** Set the coordinates to begin drawing at.
     *  x or y can be -1 to leave unchanged.
     *  Note that y is only precise to within 4 pixels.
     */
    int r = 0;
    if(x >= 0) {
        if(y >= 0) {
            uint8_t cmds[]={SSD1306_BATCHCMD_SET_XY(x,y), 0};
            r = ssd1306_sendBatchCmds(cmds);
        }
        else {
            uint8_t cmds[]={SSD1306_BATCHCMD_SET_X(x), 0};
            r = ssd1306_sendBatchCmds(cmds);
        }
        if(r) return r;
    }
    if(y >= 0) {
        uint8_t cmds[]={SSD1306_BATCHCMD_SET_Y(y), 0};
        r = ssd1306_sendBatchCmds(cmds);
        if(r) return r;
    }
    return r;
}

int ssd1306_clear() {
    int x, y, r=0;
    memset(vram, 0, sizeof(vram));
    /* for(y=0; y<SSD1306_HEIGHT; y += 4) {
        r = ssd1306_setPos(-1, y);
        if(r) return r;
        for(x=0; x<SSD1306_WIDTH; x += 16) {
            r = ssd1306_setPos(x, -1);
            r = r || ssd1306_sendData(zeros, 32);
            if(r) return r;
        }
    } */
    return r;
}

void ssd1306_putpixel(int x, int y, bool p) {
    //each byte = 1x4 pixels
    if(x < 0 || y < 0 || x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) return;
    int offs = ((y >> 2) * SSD1306_WIDTH) + x;
    uint8_t mask = 3 << ((y & 3)*2);
    if(p) vram[offs] |= mask;
    else vram[offs] &= ~mask;
}

int ssd1306_refresh() {
    int r = ssd1306_setPos(0, 0);
    if(r) return r;

    int i;
    for(i=0; i<sizeof(vram); i += 32) {
        r = ssd1306_setPos(
            (i % SSD1306_WIDTH), (i/SSD1306_WIDTH) << 2);
        if(r) return r;
        r = ssd1306_sendData(&vram[i], 32);
        system_soft_wdt_feed();
        if(r) return r;
    }

    return r;
}

static int _putEmptyChrDirect(int x, int y) {
    int r;
    uint8_t cmds1[]={SSD1306_BATCHCMD_SET_XY(x, y), 0};

    r = ssd1306_sendBatchCmds(cmds1);
    r = r || ssd1306_sendData(zeros, 8);
    if(r) return r;

    uint8_t cmds2[]={SSD1306_BATCHCMD_SET_XY(x, y+4), 0};

    r = ssd1306_sendBatchCmds(cmds1);
    r = r || ssd1306_sendData(zeros, 8);
    return r;
}

static int _putEmptyChr(int x, int y) {
    int cx, cy;
    for(cx=0; cx<8; cx++) {
        for(cy=0; cy<8; cy++) {
            ssd1306_putpixel(x+cx, y+cy, 0);
        }
    }
    return 0;
}

int ssd1306_putchr(char c, int x, int y) {
    /* 8 bytes per character, but not contiguous
    first 16 bytes are top row of character 0, 1, 2... 15
    next are next row of character 0, 1, 2... 15
    so char  0 row 0 = 0
       char  0 row 1 = 16
       char  0 row 2 = 32
       char  1 row 0 = 1
       char  1 row 1 = 17
       char 16 row 0 = 128
       char  C row R = ((C & 0xF0) * 8) + (C & 0xF) + (16*R)
    it's awkward, but this is what we get by exporting a 16x16
    grid of 8x8 tiles from Gimp as 1bpp.
    */
    int r=0;
    if(c >= 'a' && c <= 'z') c ^= 0x20;
    if(c == '-') {
        int cx;
        for(cx=1; cx<7; cx++)
            ssd1306_putpixel(x+cx, y+3, 1);
        return r;
    }

    c -= 0x30;
    if(c < 0 || c >= 0x30) return _putEmptyChr(x, y);

    int base = ((c & 0xF0) << 3) + (c & 0x0F);
    int cx, cy, half;

    for(cx=0; cx<8; cx++) {
        for(cy=0; cy<8; cy++) {
            int offs = base + (cy*16);
            if(offs >= 1024) offs = 0;
            uint8_t b = font_bin[offs];
            ssd1306_putpixel(x+cx, y+cy, b & (1 << (7-cx)));
        }
    }

    /* for(cx=0; cx<8; cx++) {
        for(half=0; half<8; half += 4) {
            uint8_t data = 0;
            for(cy=0; cy<4; cy++) {
                int offs = base + ((cy+half)*16);
                if(offs >= 1024) offs = 0;
                uint8_t b = font_bin[offs];
                if(b & (1 << (7-cx))) {
                    data |= (3 << (cy*2));
                }
            }
            r = ssd1306_setPos(x+cx, y+half);
            r = r || ssd1306_sendData(&data, 1);
            if(r) return r;
        }
    } */
    return r;
}

static int _putEmptyChrBigDirect(int x, int y) {
    int r, i;
    for(i=0; i<4; i++) {
        r = ssd1306_setPos(x, y+(i*4));
        r = r || ssd1306_sendData(zeros, 16);
        if(r) return r;
    }
    return r;
}

static int _putEmptyChrBig(int x, int y) {
    int cx, cy;
    for(cx=0; cx<16; cx++) {
        for(cy=0; cy<16; cy++) {
            ssd1306_putpixel(x+cx, y+cy, 0);
        }
    }
    return 0;
}

int ssd1306_putchrbig(char c, int x, int y) {
    int r=0;
    if(c >= 'a' && c <= 'z') c ^= 0x20;
    c -= 0x30;
    if(c < 0 || c >= 0x30) return _putEmptyChrBig(x, y);

    int base = ((c & 0xF0) << 3) + (c & 0xF);
    int cx, cy, half;
    for(cx=0; cx<8; cx++) {
        for(cy=0; cy<8; cy++) {
            int offs = base + (cy*16);
            if(offs >= 1024) offs = 0;
            uint8_t b = font_bin[offs] & (1 << (7-cx));
            ssd1306_putpixel(x+(cx*2),   y+(cy*2),   b);
            ssd1306_putpixel(x+(cx*2)+1, y+(cy*2),   b);
            ssd1306_putpixel(x+(cx*2)+1, y+(cy*2)+1, b);
            ssd1306_putpixel(x+(cx*2),   y+(cy*2)+1, b);
        }
    }

    /* int base = ((c & 0xF0) << 3) + (c & 0xF);
    int cx, cy, half;
    for(cx=0; cx<8; cx++) {
        for(half=0; half<8; half+=4) {
            uint16_t data = 0;
            for(cy=0; cy<4; cy++) {
                int offs = base + ((cy+half)*16);
                if(offs >= 1024) offs = 0;
                uint8_t b = font_bin[offs];
                if(b & (1 << (7-cx))) {
                    data |= (0xF << (cy*4));
                }
            }
            uint8_t d1=data & 0xFF, d2=data >> 8;
            r = ssd1306_setPos(x+(cx*2), y+(half*2));
            r = r || ssd1306_sendData(&d1, 1);
            r = r || ssd1306_setPos(x+(cx*2), y+(half*2)+4);
            r = r || ssd1306_sendData(&d2, 1);
            if(r) return r;
        }
    } */
    return r;
}

int ssd1306_putstr(const char *str, int x, int y, bool big) {
    int r=0;
    int ix = x;
    int w = big ? 16 : 8;
    int h = w;

    while(*str && !r) {
        switch(*str) {
            case '\r': x = ix; break;
            case '\n': y += h; break;
            case ' ':  x += w / 2; break;
            default:
                if(big) r = ssd1306_putchrbig(*str, x, y);
                else r = ssd1306_putchr(*str, x, y);
                x += w;
                /* if(x+w >= SSD1306_WIDTH) {
                    x = 0;
                    y += h;
                } */
        }
        str++;
        system_soft_wdt_feed();
    }
    return r;
}
