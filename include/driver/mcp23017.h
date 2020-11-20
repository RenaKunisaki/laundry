//MCP23017 I2C expander
#ifndef __MCP23017_H__
#define __MCP23017_H__

#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "mem.h"

//address is selectable from 0x20 to 0x27 by pin connections
#define MCP23017_ADDRESS      0x20

//register addresses in linear mode
#define MCP23017_REG_IODIRA   0x00 //I/O direction (1 bit per pin, 0=out 1=in)
#define MCP23017_REG_IODIRB   0x01
#define MCP23017_REG_IPOLA    0x02 //input polarity (1=invert pin input)
#define MCP23017_REG_IPOLB    0x03
#define MCP23017_REG_GPINTENA 0x04 //interrupt on change
#define MCP23017_REG_GPINTENB 0x05
#define MCP23017_REG_DEFVALA  0x06 //default value (interrupt when pin value
#define MCP23017_REG_DEFVALB  0x07 //does not match this value)
#define MCP23017_REG_INTCONA  0x08 //interrupt control (1=when changed from
#define MCP23017_REG_INTCONB  0x09 //DEFVAL, 0=when any change)
#define MCP23017_REG_IOCON    0x0A //I/O configuration
#define MCP23017_REG_IOCON2   0x0B //mirror of IOCON
#define MCP23017_REG_GPPUA    0x0C //GPIO pullup enable
#define MCP23017_REG_GPPUB    0x0D
#define MCP23017_REG_INTFA    0x0E //interrupt flags
#define MCP23017_REG_INTFB    0x0F
#define MCP23017_REG_INTCAPA  0x10 //GPIO value when interrupt occurred
#define MCP23017_REG_INTCAPB  0x11
#define MCP23017_REG_GPIOA    0x12 //GPIO value (pin states)
#define MCP23017_REG_GPIOB    0x13
#define MCP23017_REG_OLATA    0x14 //output latch
#define MCP23017_REG_OLATB    0x15

//register addresses in banked mode
//this basically just rearranges the bits.
#define MCP23017_BREG_IODIRA   0x00
#define MCP23017_BREG_IODIRB   0x10
#define MCP23017_BREG_IPOLA    0x01
#define MCP23017_BREG_IPOLB    0x11
#define MCP23017_BREG_GPINTENA 0x02
#define MCP23017_BREG_GPINTENB 0x12
#define MCP23017_BREG_DEFVALA  0x03
#define MCP23017_BREG_DEFVALB  0x13
#define MCP23017_BREG_INTCONA  0x04
#define MCP23017_BREG_INTCONB  0x14
#define MCP23017_BREG_IOCON    0x05
#define MCP23017_BREG_IOCON2   0x15 //mirror of IOCON
#define MCP23017_BREG_GPPUA    0x06
#define MCP23017_BREG_GPPUB    0x16
#define MCP23017_BREG_INTFA    0x07
#define MCP23017_BREG_INTFB    0x17
#define MCP23017_BREG_INTCAPA  0x08
#define MCP23017_BREG_INTCAPB  0x18
#define MCP23017_BREG_GPIOA    0x09
#define MCP23017_BREG_GPIOB    0x19
#define MCP23017_BREG_OLATA    0x0A
#define MCP23017_BREG_OLATB    0x1A

//IOCON bits
#define MCP23017_IOCON_BANK   BIT(7) //use banked mode
#define MCP23017_IOCON_MIRROR BIT(6) //connect INTA and INTB together
#define MCP23017_IOCON_SEQOP  BIT(5) //do not increment address
#define MCP23017_IOCON_DISSLW BIT(4) //disable slew rate for SDA output
#define MCP23017_IOCON_HAEN   BIT(3) //hardware address enable
#define MCP23017_IOCON_ODR    BIT(2) //open drain INT (overrides INTPOL)
#define MCP23017_IOCON_INTPOL BIT(1) //INT polarity
//bit 0: unused
//on the MCP23017, HAEN bit is not used. On the nearly-identical MCP23S17,
//this flag enables or disables the three external address pins.
//it's included here for sake of completeness.

//INTCON
#define MCP23017_INT_ON_CHANGE 0 //interrupt on any change
#define MCP23017_INT_ON_VALUE  1 //interrupt when != DEFVAL

int mcp23017_init(uint8_t address);
int mcp23017_readReg(uint8_t address, uint8_t reg);
int mcp23017_readRegPair(uint8_t address, uint8_t reg);
int mcp23017_writeReg(uint8_t address, uint8_t reg, uint8_t data);
int mcp23017_writeRegPair(uint8_t address, uint8_t reg, uint16_t data);
int mcp23017_setIocon(uint8_t address, uint8_t val);
int mcp23017_setDirection(uint8_t address, uint16_t dir);
int mcp23017_setPolarity(uint8_t address, uint16_t pol);
int mcp23017_setPullup(uint8_t address, uint16_t pull);
int mcp23017_setIntEnable(uint8_t address, uint16_t enable);
int mcp23017_setIntDefault(uint8_t address, uint16_t val);
int mcp23017_setIntMode(uint8_t address, uint16_t val);
int mcp23017_getIntFlag(uint8_t address);
int mcp23017_getIntValue(uint8_t address);
int mcp23017_read(uint8_t address);
int mcp23017_write(uint8_t address, uint16_t data);

#endif //__MCP23017_H__
