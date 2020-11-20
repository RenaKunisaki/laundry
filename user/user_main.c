#include "main.h"

os_event_t user_procTaskQueue[user_procTaskQueueLen];
volatile os_timer_t my_timer;
int g_state = STATE_BOOT;
int g_ledVal[NUM_LEDS];
char g_displayText[256] = "";
bool g_needDisplayRefresh = false;
int g_washState=WASH_STATE_UNKNOWN, g_dryState=DRY_STATE_UNKNOWN;

static const char *resetReasons[] = {
    "Power on", "HW WDT", "EXC", "SW WDT",
    "Reboot", "Wakeup", "Ext"
};


void ICACHE_FLASH_ATTR state_idle() {
    //wait for a while.
    static int count = 0;
    count++;
    if(count >= 3) {
        count = 0;
        g_state = STATE_DO_CONFIG;
    }
}

void ICACHE_FLASH_ATTR
my_timer_cb(void *arg) {
    //os_printf("state %d\r\n", g_state);
    switch(g_state) {
        case STATE_BOOT: state_boot(); break;
        case STATE_REBOOT: state_reboot(); break;
        case STATE_INIT: state_init(); break;
        case STATE_REFRESH_DISPLAY: state_refresh_display(); break;
        case STATE_REDRAW: state_redraw(); break;
        case STATE_CHECK_LEDS: state_check_leds(); break;
        case STATE_CHECK_MOTION: state_check_motion(); break;
        case STATE_SEND_BEACON: state_send_beacon(); break;
        case STATE_IDLE: state_idle(); break;
        case STATE_DO_CONFIG: state_do_config(); break;
        default:
            os_printf(" !!! INVALID STATE %d !!!\r\n", g_state);
            g_state = STATE_REBOOT;
    }
}

void ICACHE_FLASH_ATTR
my_system_init_done(void) {
    PIN_FUNC_SELECT(GPIO_PIN_REG_0,  FUNC_GPIO0); //boot mode
    //PIN_FUNC_SELECT(GPIO_PIN_REG_1,  FUNC_GPIO1); //UART
    PIN_FUNC_SELECT(GPIO_PIN_REG_2,  FUNC_GPIO2); //LED
    //PIN_FUNC_SELECT(GPIO_PIN_REG_3,  FUNC_GPIO3); //UART
    PIN_FUNC_SELECT(GPIO_PIN_REG_4,  FUNC_GPIO4); //I2C
    PIN_FUNC_SELECT(GPIO_PIN_REG_5,  FUNC_GPIO5); //I2C
    //PIN_FUNC_SELECT(GPIO_PIN_REG_6,  FUNC_GPIO6); //crashes
    //PIN_FUNC_SELECT(GPIO_PIN_REG_7,  FUNC_GPIO7); //crashes
    //PIN_FUNC_SELECT(GPIO_PIN_REG_8,  FUNC_GPIO8); //crashes
    //PIN_FUNC_SELECT(GPIO_PIN_REG_9,  FUNC_GPIO9); //crashes
    //PIN_FUNC_SELECT(GPIO_PIN_REG_10, FUNC_GPIO10); //crashes
    //PIN_FUNC_SELECT(GPIO_PIN_REG_11, FUNC_GPIO11); //crashes
    PIN_FUNC_SELECT(GPIO_PIN_REG_12, FUNC_GPIO12); //doesn't work
    PIN_FUNC_SELECT(GPIO_PIN_REG_13, FUNC_GPIO13);
    PIN_FUNC_SELECT(GPIO_PIN_REG_14, FUNC_GPIO14);
    PIN_FUNC_SELECT(GPIO_PIN_REG_15, FUNC_GPIO15);
    //so out of 16 GPIOs:
    //- two are usable
    //- three are usable but also tied to other things
    //  and thus have weirdness
    //- seven don't work or can't be used at all
    //- two might be usable if you didn't need UART
    //- three I don't know
    //great fucking design there.

    //no idea
    //gpio_register_set(GPIO_PIN_ADDR(0),
    //    GPIO_PIN_INT_TYPE_SET(GPIO_PIN_INTR_DISABLE)  |
    //    GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_DISABLE) |
    //    GPIO_PIN_SOURCE_SET(GPIO_AS_PIN_SOURCE));


    //enable outputs on all of these GPIOs
    uint32_t mask = 0;
    int i; for(i=0; i<11; i++) {
        mask |= (1 << gpioPin[i]);
    }
    (*(volatile uint32_t*)0x60000310) = mask;
    /* gpio_enable(0, GPIO_OUTPUT);
    gpio_enable(1, GPIO_OUTPUT);
    gpio_enable(2, GPIO_OUTPUT);
    gpio_enable(3, GPIO_OUTPUT);
    gpio_enable(4, GPIO_OUTPUT);
    gpio_enable(5, GPIO_OUTPUT);
    gpio_enable(6, GPIO_INPUT);
    gpio_enable(7, GPIO_OUTPUT);
    gpio_enable(8, GPIO_OUTPUT);
    gpio_enable(9, GPIO_OUTPUT);
    gpio_enable(10, GPIO_OUTPUT); */
    //gpio_enable(6, GPIO_INPUT);
    gpio16_output_conf();
    gpio16_output_set(1); //USB LED, active low
    gpio_write(4, 1); //Wifi LED, active low
    //these LEDs are just general-purpose, I'm identifying
    //them by where they are.

    wifi_set_opmode(STATION_MODE);
    //wifi_set_channel(11);
    //wifi_softap_set_config_current(&config);
    os_printf("Startup OK\r\n");
}

void ICACHE_FLASH_ATTR
user_init() {
    struct rst_info* rst = system_get_rst_info();
    uart_init(460800, 460800);
    os_printf("\n\n\r\n*** LAUNDRY MONITOR CONTROL PROGRAM ***\r\n"
        "SDK version:%s\r\n"
        "Reset cause: %d %s\r\n"
        "EXC: %d, EPC: 0x%08X\r\n",
        system_get_sdk_version(),
        rst->reason, resetReasons[rst->reason],
        rst->exccause, rst->epc1);
    os_sprintf(g_displayText, "RST:%s %d\r\n%08X",
        resetReasons[rst->reason],
        rst->exccause, rst->epc1);

    // Promiscuous works only with station mode
    //wifi_set_opmode(STATION_MODE);

    gpio_init();
    i2c_init(SCL_PIN, SDA_PIN);
    os_printf("I2C init OK\r\n");
    //ets_wdt_disable();
    //system_soft_wdt_stop();

    // Set timer
    os_timer_disarm(&my_timer);
    os_timer_setfn(&my_timer, (os_timer_func_t *)my_timer_cb, NULL);
    os_timer_arm(&my_timer, 50, 1);

    system_init_done_cb(my_system_init_done);
}
