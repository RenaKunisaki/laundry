#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "mem.h"
#include "user_config.h"
#include "user_interface.h"
#include "driver/uart.h"
#include "driver/i2c.h"
//#include "driver/mcp23017.h"
#include "driver/ssd1306.h"
#include "driver/mpu6050.h"

typedef void (*freedom_outside_cb_t)(uint8 status);
int wifi_register_send_pkt_freedom_cb(freedom_outside_cb_t cb);
void wifi_unregister_send_pkt_freedom_cb(void);
int wifi_send_pkt_freedom(uint8 *buf, int len, bool sys_seq);

#define FLASH_USER_CONFIG_ADDR 0x3FE000

#define NUM_LEDS 8

//pin -> GPIO num
// 0: D3
// 1: D9  - TX0
// 2: D4  - blue LED near antenna, active low
// 3: D10 - RX0
// 4: D2  - SDA
// 5: D1  - SCL
// 6: reserved (CLK)
// 7: reserved (SD0)
// 8: reserved (SD1)
// 9: reserved (SD2)
//10: reserved (SD3)
//11: reserved (CMD)
//12: D6  - MISO
//13: D7  - MOSI
//14: D5  - SCK
//15: D8  - SS/CS
//16: D0  - blue LED near USB, active low, funny
#define PIN_D0 16
#define PIN_D1 5
#define PIN_D2 4
#define PIN_D3 0
#define PIN_D4 2
#define PIN_D5 14
#define PIN_D6 12
#define PIN_D7 13
#define PIN_D8 15
#define PIN_D9 3
#define PIN_D10 1

//if changing this doesn't work, you probably need to make clean.
#define SCL_PIN 1 //D1, not GPIO1
#define SDA_PIN 2
#define PIN_CH0 8
#define PIN_CH1 7
#define PIN_CH2 6

#define LED_FILL  0
#define LED_WASH  1
#define LED_RINSE 2
#define LED_SPIN  3
#define LED_DONE  4
#define LED_ROOM  5 //room ambient light level
#define LED_ZERO  6 //should read close to 0
#define LED_ONE   7 //should read close to max

//these only go up to 15. for 16 we have to mess with other things.
#define GPIO_PIN_REG_0          PERIPHS_IO_MUX_GPIO0_U
#define GPIO_PIN_REG_1          PERIPHS_IO_MUX_U0TXD_U
#define GPIO_PIN_REG_2          PERIPHS_IO_MUX_GPIO2_U
#define GPIO_PIN_REG_3          PERIPHS_IO_MUX_U0RXD_U
#define GPIO_PIN_REG_4          PERIPHS_IO_MUX_GPIO4_U
#define GPIO_PIN_REG_5          PERIPHS_IO_MUX_GPIO5_U
#define GPIO_PIN_REG_6          PERIPHS_IO_MUX_SD_CLK_U
#define GPIO_PIN_REG_7          PERIPHS_IO_MUX_SD_DATA0_U
#define GPIO_PIN_REG_8          PERIPHS_IO_MUX_SD_DATA1_U
#define GPIO_PIN_REG_9          PERIPHS_IO_MUX_SD_DATA2_U
#define GPIO_PIN_REG_10         PERIPHS_IO_MUX_SD_DATA3_U
#define GPIO_PIN_REG_11         PERIPHS_IO_MUX_SD_CMD_U
#define GPIO_PIN_REG_12         PERIPHS_IO_MUX_MTDI_U
#define GPIO_PIN_REG_13         PERIPHS_IO_MUX_MTCK_U
#define GPIO_PIN_REG_14         PERIPHS_IO_MUX_MTMS_U
#define GPIO_PIN_REG_15 PERIPHS_IO_MUX_MTDO_U

//why aren't these defined?
//maybe because we can't use them anyway.
#define FUNC_GPIO6                          3
#define FUNC_GPIO7                          3
#define FUNC_GPIO8                          3
#define FUNC_GPIO11                         3

#define user_procTaskPrio        0
#define user_procTaskQueueLen    1

#pragma pack(4)
typedef struct {
    uint16_t magic;
    uint8_t version;
    uint8_t checksum;
    uint8_t displayFlipX; //flip display horizontally?
    uint8_t displayFlipY; //flip display vertically?
    uint8_t ledBoardFlip; //flip LED board?
    uint8_t displayContrast; //display contrast setting
    uint8_t wifiChannel;
    uint8_t wifiMac1[6];
    uint8_t wifiMac2[6];
    uint8_t wifiSsid[32];
    uint8_t ledOnSensitivity[NUM_LEDS]; //analog threshold / 4
    uint8_t ledOffSensitivity[NUM_LEDS];
    uint8_t washOnSensitivity; //how many reads in a row to be on
    uint8_t washOffSensitivity; //how many reads in a row to be off
    uint8_t drySensitivity; //motion threshold
    uint8_t displayAddr; //display I2C address
} FlashUserConfig;
//round up to 4
#define FLASH_USER_CONFIG_SIZE ( \
    (sizeof(FlashUserConfig) & ~3) + \
    ((sizeof(FlashUserConfig) & 3) ? 4 : 0) \
)
#define FLASH_USER_CONFIG_MAGIC 0x7511
#define FLASH_USER_CONFIG_VERSION 1

typedef struct {
    const char *text;
    uint8_t *item;
    uint8_t size;
} ConfigMenuItem;

//g_state values
enum {
    STATE_BOOT = 0,
    STATE_REBOOT,
    STATE_INIT,
    STATE_REFRESH_DISPLAY,
    STATE_REDRAW,
    STATE_CHECK_LEDS,
    STATE_CHECK_MOTION,
    STATE_SEND_BEACON,
    STATE_IDLE,
    STATE_DO_CONFIG,
    NUM_STATES
} AppState;

enum {
    WASH_STATE_UNKNOWN = 0,
    WASH_STATE_OFF,
    WASH_STATE_FILLING,
    WASH_STATE_WASHING,
    WASH_STATE_RINSING,
    WASH_STATE_SPINNING,
    WASH_STATE_DONE,
    WASH_STATE_ERROR,
    WASH_STATE_SENSOR_FAIL,
    NUM_WASH_STATES
} WashState;

enum {
    DRY_STATE_UNKNOWN = 0,
    DRY_STATE_OFF,
    DRY_STATE_ON,
    DRY_STATE_ERROR,
    NUM_DRY_STATES
} DryState;

//boot.c
void ICACHE_FLASH_ATTR state_boot();
void ICACHE_FLASH_ATTR state_reboot();
void ICACHE_FLASH_ATTR state_init();

//config.c
extern FlashUserConfig g_config;
int configResetDefaults();
int configSave();
int configLoad();
void state_do_config();

//display.c
void ICACHE_FLASH_ATTR state_refresh_display();
void ICACHE_FLASH_ATTR state_redraw();

//dry.c
extern float g_temperature; //in 1/10 degC
void state_check_motion();

//gpio.c
extern const int gpioPin[];
void gpio_enable(int pin, int dir);
bool gpio_read(int pin);
void gpio_write(int pin, int val);
void ICACHE_FLASH_ATTR gpio16_output_conf(void);
void ICACHE_FLASH_ATTR gpio16_output_set(uint8 value);

//user_main.c
extern volatile os_timer_t my_timer;
extern int g_state;
extern int g_ledVal[NUM_LEDS];
extern char g_displayText[256];
extern bool g_needDisplayRefresh;
extern int g_washState, g_dryState;

//util.c
void hexdump(const void *data, uint32_t len);
int ICACHE_FLASH_ATTR i2c_scan_cb();

//wash.c
void state_check_leds();

//wifi.c
void ICACHE_FLASH_ATTR state_send_beacon();
