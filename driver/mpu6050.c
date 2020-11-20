#include "driver/mpu6050.h"
static int i2c_id = MPU9250_DEFAULT_ADDR;
static uint16_t mpuType = 0;

int mpu_init(int address) {
    if(address != 0) i2c_id = address;
    else i2c_id = MPU9250_DEFAULT_ADDR;
    return mpu_writeReg(MPU9250_REG_PWR_MGMT_1, MPU9250_H_RESET);
}

int mpu_init2() {
    //call ~100ms after init
    int r = 0;

    //sleep off, cycle off, standby off, vgen on, internal clk
    r = r || mpu_writeReg(MPU9250_REG_PWR_MGMT_1, 0);
    //clear all disable bits
    r = r || mpu_writeReg(MPU9250_REG_PWR_MGMT_2, 0);
    if(r) return r;

    //ask what this is
    r = mpu_readReg(MPU9250_REG_WHO_AM_I);
    if(r < 0) return r;
    switch(r) {
        case MPU9255_ID: mpuType = 0x9255; break;
        case MPU9250_ID: mpuType = 0x9250; break;
        case MPU6050_ID: mpuType = 0x6050; break;
        default:
            os_printf("Unrecognized MPU ID 0x%02X\r\n", r);
    }
    os_printf("Detected MPU%04X\r\n", mpuType);

    //enable I2C passthru
    r = mpu_writeReg(MPU9250_REG_INT_PIN_CFG, MPU9250_BYPASS_EN);
    return r;
}

int mpu_readReg(uint8_t reg) {
    uint8_t response = 0xAA;
    bool success = i2c_slave_read(i2c_id, reg, &response, 1);
    if(!success) return -1;
    else return response;
}

int mpu_writeReg(uint8_t reg, uint8_t val) {
    bool success;
    int i;

    for(i=0; i<10; i++) {
        i2c_start();
        success = i2c_write(i2c_id << 1);
        if(success) break;
        i2c_stop();
        os_delay_us(100);
    }
    success = success && i2c_write(reg);
    success = success && i2c_write(val);
    i2c_stop();
    return success ? 0 : -1;
}

int mpu_read(int16_t *AX, int16_t *AY, int16_t *AZ,
int16_t *GX, int16_t *GY, int16_t *GZ,
int16_t *temp) {
    uint8_t data[16];
    memset(data, 0xAA, 16);

    bool success = i2c_slave_read(i2c_id, MPU9250_REG_ACCEL_XOUT_H, data, 14);
    if(!success) return -1;

    int16_t tempRaw = (data[ 6] << 8) | data[ 7];
    //int16_t tempC = ((tempRaw – RoomTemp_Offset) / Temp_Sensitivity) + 21degC
    //XXX how do we find RoomTemp_Offset and Temp_Sensitivity?
    //int16_t tempC = ((tempRaw - 32) / 256) + 21; //for MPU9250
    int16_t tempC = (tempRaw / 340.0) + 36.53; //for MPU6050

    //store result
    *AX   = (data[ 0] << 8) | data[ 1];
    *AY   = (data[ 2] << 8) | data[ 3];
    *AZ   = (data[ 4] << 8) | data[ 5];
    *temp = tempC;
    *GX   = (data[ 8] << 8) | data[ 9];
    *GY   = (data[10] << 8) | data[11];
    *GZ   = (data[12] << 8) | data[13];

    //we could add 9250 magnetometer here...

    return 0;
}
