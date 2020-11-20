#include "main.h"

FlashUserConfig g_config;

int configResetDefaults() {
    static uint8_t defaultMac[6] = {
        0x75, 0x11, 'R', 'e', 'n', 'a'};
    static uint8_t defaultSsid[32] = {
        //poo emoji lol
        0xF0, 0x9F, 0x92, 0xA9, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    memset(&g_config, 0, sizeof(g_config));
    g_config.magic = FLASH_USER_CONFIG_MAGIC;
    g_config.version = FLASH_USER_CONFIG_VERSION;
    g_config.displayFlipX = 0;
    g_config.displayFlipY = 0;
    g_config.ledBoardFlip = 0;
    g_config.displayContrast = 0x8F;
    g_config.wifiChannel = 11;
    memcpy(g_config.wifiMac1, defaultMac, 6);
    memcpy(g_config.wifiMac2, defaultMac, 6);
    memcpy(g_config.wifiSsid, defaultSsid, 32);
    int i; for(i=0; i<NUM_LEDS; i++) {
        g_config.ledOnSensitivity [i] = 400 / 4;
        g_config.ledOffSensitivity[i] = 100 / 4;
    }
    g_config.washOnSensitivity = 4;
    g_config.washOffSensitivity = 4;
    g_config.drySensitivity = 180;
    g_config.displayAddr = 0x3C;
    os_printf("Reset config to default...\r\n");
    return configSave();
}

static const char* getFlashErrorStr(SpiFlashOpResult result) {
    switch(result) {
        case SPI_FLASH_RESULT_OK: return "OK";
        case SPI_FLASH_RESULT_ERR: return "ERR";
        case SPI_FLASH_RESULT_TIMEOUT: return "TIMEOUT";
        default: return "UNKNOWN";
    }
}

int configSave() {
    //set checksum
    uint8_t *data = (uint8_t*)&g_config;
    uint8_t cksum = 0;
    int i;
    for(i=4; i<sizeof(FlashUserConfig); i++) {
        cksum += data[i];
    }
    g_config.checksum = cksum;
    os_printf("Write flash data (%X -> %X):\r\n",
        sizeof(FlashUserConfig), FLASH_USER_CONFIG_SIZE);
    hexdump(&g_config, sizeof(FlashUserConfig));

    SpiFlashOpResult result;
    result = spi_flash_erase_sector(
        FLASH_USER_CONFIG_ADDR / SPI_FLASH_SEC_SIZE);
    os_printf("flash erase: %s\r\n", getFlashErrorStr(result));
    if(result != SPI_FLASH_RESULT_OK) return -1;

    result = spi_flash_write(
        FLASH_USER_CONFIG_ADDR, (uint32*)&g_config,
        FLASH_USER_CONFIG_SIZE);

    os_printf("Save config %s\r\n", getFlashErrorStr(result));

    switch(result) {
        case SPI_FLASH_RESULT_OK: return 0;
        case SPI_FLASH_RESULT_ERR: return -1;
        case SPI_FLASH_RESULT_TIMEOUT: return -2;
        default: return -3;
    }
}

int configLoad() {
    int i;

    memset(&g_config, 0, sizeof(g_config));
    SpiFlashOpResult result = spi_flash_read(
        FLASH_USER_CONFIG_ADDR, (uint32*)&g_config,
        FLASH_USER_CONFIG_SIZE);
    os_printf("Read config %s\r\n", getFlashErrorStr(result));
    if(result != SPI_FLASH_RESULT_OK) {
        return -1;
    }

    uint8_t *data = (uint8_t*)&g_config;
    os_printf("user config data:\r\n");
    hexdump(&g_config, sizeof(FlashUserConfig));

    //check header
    if(g_config.magic != FLASH_USER_CONFIG_MAGIC) {
        os_printf("Config magic invalid (%04X)\r\n", g_config.magic);
        return configResetDefaults() || -1;
    }
    if(g_config.version != FLASH_USER_CONFIG_VERSION) {
        os_printf("Config version invalid (%02X)\r\n", g_config.version);
        return configResetDefaults() || -1;
    }

    uint8_t cksum = 0;
    for(i=4; i<sizeof(FlashUserConfig); i++) {
        cksum += data[i];
    }
    if(cksum != g_config.checksum) {
        os_printf("Config checksum invalid (%02X, want %02X)\r\n",
        g_config.checksum, cksum);
        return configResetDefaults() || -1;
    }

    gpio_enable(PIN_D4, GPIO_INPUT);
    if(gpio_read(PIN_D4)) {
        os_printf("PIN_D4 is high, so resetting config\r\n");
        return configResetDefaults() || -1;
    }
    os_printf("PIN_D4 is low, so not resetting config\r\n");

    os_printf("Config is OK\r\n");
    return 0;
}

void state_do_config() {
    static const uint8_t cfgAddr = 0x27;
    static const ConfigMenuItem menuItems[] = {
        {"Display Flip X",   &g_config.displayFlipX,         1},
        {"Display Flip Y",   &g_config.displayFlipY,         1},
        {"LED Board Flip",   &g_config.ledBoardFlip,         1},
        {"Display Contrast", &g_config.displayContrast,      1},
        {"Wifi Channel",     &g_config.wifiChannel,          1},
        {"Wifi MAC1",        &g_config.wifiMac1[0],          6},
        {"Wifi MAC2",        &g_config.wifiMac2[0],          6},
        {"Wifi SSID",        &g_config.wifiSsid[0],         32},
        {"LED On Sens.",     &g_config.ledOnSensitivity[0], NUM_LEDS},
        {"LED Off Sens.",    &g_config.ledOffSensitivity[0],NUM_LEDS},
        {"Wash On Sens.",    &g_config.washOnSensitivity,    1},
        {"Wash Off Sens.",   &g_config.washOffSensitivity,   1},
        {"Dry Sens.",        &g_config.drySensitivity,       1},
        {"Display Addr",     &g_config.displayAddr,          1},
        {NULL, NULL, 0},
    };
    static int curItem = 0;
    static uint8_t prevBtn = 0;

    int nItems = 0;
    while(menuItems[nItems].text) nItems++;

    int r = mcp23017_setDirection(cfgAddr, 0xFFFF); //all input
    os_delay_us(1000);
    if(r) { //not connected
        g_state = STATE_CHECK_LEDS;
        return;
    }
    wifi_set_opmode(NULL_MODE);
    os_delay_us(1000);
    //mcp23017_setPullup(cfgAddr, 0xFFFF); //all on
    //ets_intr_lock();
    os_delay_us(1000);
    int resp = mcp23017_read(cfgAddr);
    if(resp < 0) {
        os_printf("MCP read error %d\r\n", resp);
        return;
    }
    uint16_t val = ~resp;
    //ets_intr_unlock();
    uint8_t curIdx = val & 0xFF; //DIP switch pos
    uint8_t curBtn = (val >> 8) & 0xFF;
    uint8_t changeBtn = curBtn & ~prevBtn;
    prevBtn = curBtn;

    uint8_t *pData = menuItems[curItem].item;
    uint8_t *data  = &pData[curIdx % menuItems[curItem].size];

    bool up    = changeBtn & 0x40;
    bool down  = changeBtn & 0x08;
    bool left  = changeBtn & 0x10;
    bool right = changeBtn & 0x04;
    bool back  = changeBtn & 0x80;
    bool fwd   = changeBtn & 0x20;

    if(val != 0x0F0F) {
        if(up)        (*data)++;
        else if(down) (*data)--;
        else if(left)  curItem--;
        else if(right) curItem++;
        else if(fwd)   configSave();
        else if(back)  configResetDefaults();
        if(curItem < 0) curItem = nItems-1;
        if(curItem >= nItems) curItem = 0;
    }

    char buf[64];

    ssd1306_power(1);
    os_sprintf(buf, "[%d] %02X", curIdx, *data);
    ssd1306_putstr(menuItems[curItem].text, 0, 0, false);
    ssd1306_putstr(buf, 0, 8, false);

    os_sprintf(buf, "V=%04X B=%02X %02X", resp, curBtn, changeBtn);
    ssd1306_putstr(buf, 0, 16, false);

    ssd1306_setXFlip(g_config.displayFlipX);
    ssd1306_setYFlip(g_config.displayFlipY);
    ssd1306_setContrast(g_config.displayContrast);
    ssd1306_refresh();
    ssd1306_clear(); //clear vram
}
