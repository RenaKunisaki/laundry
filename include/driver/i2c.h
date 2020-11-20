/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Johan Kanflo (github.com/kanflo)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __I2C_H__
#define __I2C_H__
#endif

//#include <stdint.h>
//#include <stdbool.h>
#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "mem.h"

#define GPIO_OUTPUT		0
#define GPIO_INPUT	1

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

// Init bitbanging I2C driver on given pins
void i2c_init(uint8_t scl_pin, uint8_t sda_pin);

// Write a byte to I2C bus. Return true if slave acked.
bool i2c_write(uint8_t byte);

// Read a byte from I2C bus. Return true if slave acked.
uint8_t i2c_read(bool ack);

// Write 'len' bytes from 'buf' to slave. Return true if slave acked.
bool i2c_slave_write(uint8_t slave_addr, uint8_t *buf, uint8_t len);

// Issue a read operation and send 'data', followed by reading 'len' bytes
// from slave into 'buf'. Return true if slave acked.
bool i2c_slave_read(uint8_t slave_addr, uint8_t data, uint8_t *buf, uint32_t len);

// Send start and stop conditions. Only needed when implementing protocols for
// devices where the i2c_slave_[read|write] functions above are of no use.
void i2c_start(void);
void i2c_stop(void);
