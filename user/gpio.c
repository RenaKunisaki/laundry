#include "main.h"

const int gpioPin[] = {
    //D0 is GPIO16 which is not really usable
    PIN_D0, PIN_D1, PIN_D2, PIN_D3, PIN_D4, PIN_D5, PIN_D6, PIN_D7,
    PIN_D8, PIN_D9, PIN_D10
};


void gpio_enable(int pin, int dir) {
    uint32_t set = (1 << gpioPin[pin]);
    if(dir == GPIO_OUTPUT)
        (*(volatile uint32_t*)0x60000310) =  set;
    else (*(volatile uint32_t*)0x60000314) = set;
}

bool gpio_read(int pin) {
    uint32_t set = (1 << gpioPin[pin]);
    return (*(volatile uint32_t*)0x60000318) & set;
}

void gpio_write(int pin, int val) {
    uint32_t set = (1 << gpioPin[pin]);
    if(val) (*(volatile uint32_t*)0x60000304) = set;
    else (*(volatile uint32_t*)0x60000308) = set; //clear
}


void ICACHE_FLASH_ATTR
gpio16_output_conf(void) {
    WRITE_PERI_REG(PAD_XPD_DCDC_CONF,
        (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbc)
        | (uint32)0x1);	// mux configuration for XPD_DCDC to output rtc_gpio0

    WRITE_PERI_REG(RTC_GPIO_CONF,
        (READ_PERI_REG(RTC_GPIO_CONF) & (uint32)0xfffffffe)
        | (uint32)0x0);	//mux configuration for out enable

    WRITE_PERI_REG(RTC_GPIO_ENABLE,
        (READ_PERI_REG(RTC_GPIO_ENABLE) & (uint32)0xfffffffe)
        | (uint32)0x1);	//out enable
}

void ICACHE_FLASH_ATTR
gpio16_output_set(uint8 value) {
    WRITE_PERI_REG(RTC_GPIO_OUT,
        (READ_PERI_REG(RTC_GPIO_OUT) & (uint32)0xfffffffe)
        | (uint32)(value & 1));
}
