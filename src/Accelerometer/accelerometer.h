#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#define ACC_I2C_DEVICE        "/dev/i2c-1"
#define ACC_I2C_SLAVE_ADDRESS 0x68
#define ACC_REG_BASE          0x3B
#define ACC_POWER_REG_1       0x6B
#define ACC_POWER_REG_2       0x6C

typedef struct {
    double x, y, z;
} acc_t;

/** Reset registers, disable Gyroscope and Temperature Sensor and set readings at 1.25 Hz*/
extern void acc_init();

/** Read last acceleration measurement from device */
extern void acc_read(acc_t* acc);

/** Close accelerometer connection */
extern void acc_close();

#endif 
