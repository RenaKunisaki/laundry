#include "driver/mcp23017.h"

int mcp23017_init(uint8_t address) {
    return 0;
}

int mcp23017_readReg(uint8_t address, uint8_t reg) {
    //to read a register, send its address and then read one byte.
	uint8_t response = 0xAA;
    bool success = i2c_slave_read(address, reg, &response, 1);
    if(!success) return -1;
    else return response;
}

int mcp23017_readRegPair(uint8_t address, uint8_t reg) {
    /* int r1 = mcp23017_readReg(address, reg);
    if(r1 < 0) return r1;
    int r2 = mcp23017_readReg(address, reg+1);
    if(r2 < 0) return r2;
    return (r2 << 8) | r1; */
    uint16 response = 0xABCD;
    bool success = i2c_slave_read(address, reg, &response, 2);
    if(!success) return -1;
    else return response;
}

int mcp23017_writeReg(uint8_t address, uint8_t reg, uint8_t data) {
    //to write a register, send its address and data.
    uint8_t packet[2] = {reg, data};
    bool success = i2c_slave_write(address, packet, 2);
    if(!success) return -1;
    else return 0;
}

int mcp23017_writeRegPair(uint8_t address, uint8_t reg, uint16_t data) {
    /* int      err=mcp23017_writeReg(address,reg,  data & 0xFF);
    if(!err) err=mcp23017_writeReg(address,reg+1,data >> 8);
    return err; */
    uint8_t packet[3] = {reg, data & 0xFF, data >> 8};
    bool success = i2c_slave_write(address, packet, 3);
    if(!success) return -1;
    else return 0;
}

/** Set IOCON register.
 *  val: Value to set to.
 *  Returns 0 on success or a negative error code on failure.
 */
int mcp23017_setIocon(uint8_t address, uint8_t val) {
    return mcp23017_writeReg(
        address, MCP23017_REG_IOCON, val);
}

/** Set I/O direction of GPIO pins.
 *  dir: Direction bits: 0=output, 1=input
 *    bit  0 = port A pin 1
 *    bit  1 = port A pin 2
 *    bit  7 = port A pin 8
 *    bit  8 = port B pin 1
 *    bit  9 = port B pin 2
 *    bit 15 = port B pin 8
 *  Returns 0 on success or a negative error code on failure.
 */
int mcp23017_setDirection(uint8_t address, uint16_t dir) {
    return mcp23017_writeRegPair(
        address, MCP23017_REG_IODIRA, dir);
}

/** Set GPIO input polarity.
 *  pol: Polarity bits. One per pin, as with SetDirection.
 *    1=invert input, 0=don't.
 *  Returns 0 on success or a negative error code on failure.
 */
int mcp23017_setPolarity(uint8_t address, uint16_t pol) {
    return mcp23017_writeRegPair(
        address, MCP23017_REG_IPOLA, pol);
}

/** Set GPIO input pullups.
 *  pull: Pullup bits. One per pin, as with SetDirection.
 *    1=enable internal pullup resistor, 0=disable pullup.
 *  Returns 0 on success or a negative error code on failure.
 */
int mcp23017_setPullup(uint8_t address, uint16_t pull) {
    return mcp23017_writeRegPair(
        address, MCP23017_REG_GPPUA, pull);
}

/** Set GPIO interrupt enable flags.
 *  enable: Pullup bits. One per pin, as with SetDirection.
 *    1=enable interrupt, 0=disable.
 *  Returns 0 on success or a negative error code on failure.
 */
int mcp23017_setIntEnable(uint8_t address, uint16_t enable) {
    return mcp23017_writeRegPair(
        address, MCP23017_REG_GPINTENA, enable);
}

/** Set GPIO interrupt default values.
 *  val: Default value bits. One per pin, as with SetDirection.
 *  Returns 0 on success or a negative error code on failure.
 */
int mcp23017_setIntDefault(uint8_t address, uint16_t val) {
    return mcp23017_writeRegPair(
        address, MCP23017_REG_DEFVALA, val);
}

/** Set GPIO interrupt mode.
 *  mode: Interrupt mode bits. One per pin, as with SetDirection.
 *    0=interrupt on any change
 *    1=interrupt when different from DEFVAL
 *  Returns 0 on success or a negative error code on failure.
 */
int mcp23017_setIntMode(uint8_t address, uint16_t val) {
    return mcp23017_writeRegPair(
        address, MCP23017_REG_INTCONA, val);
}

/** Read interrupt flags.
 *  On success, returns the 16 interrupt flags in the lowest bits of the result.
 *  On failure, returns a negative error code.
 */
int mcp23017_getIntFlag(uint8_t address) {
    return mcp23017_readRegPair(
        address, MCP23017_REG_INTFA);
}

/** Read captured pin states at time of interrupt.
 *  On success, returns the 16 values in the lowest bits of the result.
 *  On failure, returns a negative error code.
 */
int mcp23017_getIntValue(uint8_t address) {
    return mcp23017_readRegPair(
        address, MCP23017_REG_INTCAPA);
}

/** Read GPIO states.
 *  On success, returns the 16 pin values in the lowest bits of the result.
 *  On failure, returns a negative error code.
 *  If some pins are configured as outputs, this returns the output state.
 */
int mcp23017_read(uint8_t address) {
    return mcp23017_readRegPair(
        address, MCP23017_REG_GPIOA);
}

/** Write GPIO states.
 *  data: Output data.
 *  On success, returns 0. On failure, returns a negative error code.
 *  Bits corresponding to input pins are ignored.
 */
int mcp23017_write(uint8_t address, uint16_t data) {
    return mcp23017_writeRegPair(
        address, MCP23017_REG_GPIOA, data);
}
