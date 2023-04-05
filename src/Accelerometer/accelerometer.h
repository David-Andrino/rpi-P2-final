#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#define ACC_I2C_DEVICE        "/dev/i2c-1"
#define ACC_I2C_SLAVE_ADDRESS 0x68
#define ACC_REG_BASE          0x3B
#define ACC_POWER_REG_1       0x6B
#define ACC_POWER_REG_2       0x6C

/**
 * @brief Structure to store the acceleration values as doubles
 */
typedef struct {
    double x; /**< X component of the acceleration */
    double y; /**< Y component of the acceleration */
    double z; /**< Z component of the acceleration */
} acc_t;

/** 
 * @brief Reset registers, disable Gyroscope and Temperature Sensor and set readings at 1.25 Hz
 */
extern void acc_init();

/** 
 * @brief Read last acceleration measurement from device 
 * @param acc Pointer to the struct to save the data
 */
extern void acc_read(acc_t* acc);

/** 
 * @brief Close accelerometer connection 
 */
extern void acc_close();

#endif 
