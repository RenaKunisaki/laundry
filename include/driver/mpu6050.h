//I2C motion sensor modules MPU6050, MPU9250, MPU9255
//no idea what's different between 9250 and 9255
//other than different WHO_AM_I response.
//6050 just has fewer features.
#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "mem.h"

#define MPU6050_DEFAULT_ADDR           0x68
#define MPU9250_DEFAULT_ADDR           0x68
#define MPU9250_REG_SELF_TEST_X_GYRO   0x00
#define MPU9250_REG_SELF_TEST_Y_GYRO   0x01
#define MPU9250_REG_SELF_TEST_Z_GYRO   0x02
#define MPU9250_REG_SELF_TEST_X_ACCEL  0x0D
#define MPU9250_REG_SELF_TEST_Y_ACCEL  0x0E
#define MPU9250_REG_SELF_TEST_Z_ACCEL  0x0F
#define MPU9250_REG_XG_OFFSET_H        0x13
#define MPU9250_REG_XG_OFFSET_L        0x14
#define MPU9250_REG_YG_OFFSET_H        0x15
#define MPU9250_REG_YG_OFFSET_L        0x16
#define MPU9250_REG_ZG_OFFSET_H        0x17
#define MPU9250_REG_ZG_OFFSET_L        0x18
#define MPU9250_REG_SMPLTR_DIV         0x19
#define MPU9250_REG_CONFIG             0x1A
#define MPU9250_REG_GYRO_CONFIG        0x1B
#define MPU9250_REG_ACCEL_CONFIG       0x1C
#define MPU9250_REG_ACCEL_CONFIG2      0x1D
#define MPU9250_REG_LP_ACCEL_ODR       0x1E
#define MPU9250_REG_WOM_THR            0x1F
#define MPU9250_REG_FIFO_EN            0x23
#define MPU9250_REG_I2C_MST_CTRL       0x24
#define MPU9250_REG_I2C_SLV0_ADDR      0x25
#define MPU9250_REG_I2C_SLV0_REG       0x26
#define MPU9250_REG_I2C_SLV0_CTRL      0x27
#define MPU9250_REG_I2C_SLV1_ADDR      0x28
#define MPU9250_REG_I2C_SLV1_REG       0x29
#define MPU9250_REG_I2C_SLV1_CTRL      0x2A
#define MPU9250_REG_I2C_SLV2_ADDR      0x2B
#define MPU9250_REG_I2C_SLV2_REG       0x2C
#define MPU9250_REG_I2C_SLV2_CTRL      0x2D
#define MPU9250_REG_I2C_SLV3_ADDR      0x2E
#define MPU9250_REG_I2C_SLV3_REG       0x2F
#define MPU9250_REG_I2C_SLV3_CTRL      0x30
#define MPU9250_REG_I2C_SLV4_ADDR      0x31
#define MPU9250_REG_I2C_SLV4_REG       0x32
#define MPU9250_REG_I2C_SLV4_DO        0x33
#define MPU9250_REG_I2C_SLV4_CTRL      0x34
#define MPU9250_REG_I2C_SLV4_DI        0x35
#define MPU9250_REG_I2C_MST_STATUS     0x36
#define MPU9250_REG_INT_PIN_CFG        0x37
#define MPU9250_REG_INT_ENABLE         0x38
#define MPU9250_REG_INT_STATUS         0x3A
#define MPU9250_REG_ACCEL_XOUT_H       0x3B
#define MPU9250_REG_ACCEL_XOUT_L       0x3C
#define MPU9250_REG_ACCEL_YOUT_H       0x3D
#define MPU9250_REG_ACCEL_YOUT_L       0x3E
#define MPU9250_REG_ACCEL_ZOUT_H       0x3F
#define MPU9250_REG_ACCEL_ZOUT_L       0x40
#define MPU9250_REG_TEMP_OUT_H         0x41
#define MPU9250_REG_TEMP_OUT_L         0x42
#define MPU9250_REG_GYRO_XOUT_H        0x43
#define MPU9250_REG_GYRO_XOUT_L        0x44
#define MPU9250_REG_GYRO_YOUT_H        0x45
#define MPU9250_REG_GYRO_YOUT_L        0x46
#define MPU9250_REG_GYRO_ZOUT_H        0x47
#define MPU9250_REG_GYRO_ZOUT_L        0x48
#define MPU9250_REG_EXT_SENS_DATA_00   0x49
#define MPU9250_REG_EXT_SENS_DATA_01   0x4A
#define MPU9250_REG_EXT_SENS_DATA_02   0x4B
#define MPU9250_REG_EXT_SENS_DATA_03   0x4C
#define MPU9250_REG_EXT_SENS_DATA_04   0x4D
#define MPU9250_REG_EXT_SENS_DATA_05   0x4E
#define MPU9250_REG_EXT_SENS_DATA_06   0x4F
#define MPU9250_REG_EXT_SENS_DATA_07   0x50
#define MPU9250_REG_EXT_SENS_DATA_08   0x51
#define MPU9250_REG_EXT_SENS_DATA_09   0x52
#define MPU9250_REG_EXT_SENS_DATA_10   0x53
#define MPU9250_REG_EXT_SENS_DATA_11   0x54
#define MPU9250_REG_EXT_SENS_DATA_12   0x55
#define MPU9250_REG_EXT_SENS_DATA_13   0x56
#define MPU9250_REG_EXT_SENS_DATA_14   0x57
#define MPU9250_REG_EXT_SENS_DATA_15   0x58
#define MPU9250_REG_EXT_SENS_DATA_16   0x59
#define MPU9250_REG_EXT_SENS_DATA_17   0x5A
#define MPU9250_REG_EXT_SENS_DATA_18   0x5B
#define MPU9250_REG_EXT_SENS_DATA_19   0x5C
#define MPU9250_REG_EXT_SENS_DATA_20   0x5D
#define MPU9250_REG_EXT_SENS_DATA_21   0x5E
#define MPU9250_REG_EXT_SENS_DATA_22   0x5F
#define MPU9250_REG_EXT_SENS_DATA_23   0x60
#define MPU9250_REG_I2C_SLV0_DO        0x63
#define MPU9250_REG_I2C_SLV1_DO        0x64
#define MPU9250_REG_I2C_SLV2_DO        0x65
#define MPU9250_REG_I2C_SLV3_DO        0x66
#define MPU9250_REG_I2C_MST_DELAY_CTRL 0x67
#define MPU9250_REG_SIGNAL_PATH_RESET  0x68
#define MPU9250_REG_MOT_DETECT_CTRL    0x69
#define MPU9250_REG_USER_CTRL          0x6A
#define MPU9250_REG_PWR_MGMT_1         0x6B
#define MPU9250_REG_PWR_MGMT_2         0x6C
#define MPU9250_REG_FIFO_COUNTH        0x72
#define MPU9250_REG_FIFO_COUNTL        0x73
#define MPU9250_REG_FIFO_R_W           0x74
#define MPU9250_REG_WHO_AM_I           0x75 //reads 0x71
#define MPU9250_REG_XA_OFFSET_H        0x77
#define MPU9250_REG_XA_OFFSET_L        0x78
#define MPU9250_REG_YA_OFFSET_H        0x7A
#define MPU9250_REG_YA_OFFSET_L        0x7B
#define MPU9250_REG_ZA_OFFSET_H        0x7D
#define MPU9250_REG_ZA_OFFSET_L        0x7E

#define MPU9255_ID          0x73 //WHO_AM_I reg reads this value
#define MPU9250_ID          0x71
#define MPU6050_ID          0x68

//REG_INT_PIN_CFG
#define MPU9250_ACTL              BIT(7)
#define MPU9250_OPEN              BIT(6)
#define MPU9250_LATCH_INT_EN      BIT(5)
#define MPU9250_INT_ANYRD_2CLEAR  BIT(4)
#define MPU9250_ACTL_FSYNC        BIT(3)
#define MPU9250_FSYNC_INT_MODE_EN BIT(2)
#define MPU9250_BYPASS_EN         BIT(1)

//PWR_MGMT_1
#define MPU9250_H_RESET      BIT(7) //write 1 to reset
#define MPU9250_SLEEP        BIT(6) //enable sleep mode
#define MPU9250_CYCLE        BIT(5) //cycle between sleep/sample
#define MPU9250_GYRO_STANDBY BIT(4)
#define MPU9250_PD_PTAT      BIT(3) //power down voltage generator
#define MPU9250_CLKSEL       BIT(2) | BIT(1) | BIT(0)

//PWR_MGMT_2
#define MPU9250_DISABLE_XA BIT(5) //disable X accelerometer
#define MPU9250_DISABLE_YA BIT(4)
#define MPU9250_DISABLE_ZA BIT(3) //disable X gyro
#define MPU9250_DISABLE_XG BIT(2)
#define MPU9250_DISABLE_YG BIT(1)
#define MPU9250_DISABLE_ZG BIT(0)

int mpu_init(int address);
int mpu_init2();
int mpu_readReg(uint8_t reg);
int mpu_writeReg(uint8_t reg, uint8_t val);
int mpu_read(int16_t *AX, int16_t *AY, int16_t *AZ,
int16_t *GX, int16_t *GY, int16_t *GZ,
int16_t *temp);

#endif //__MPU6050_H__
