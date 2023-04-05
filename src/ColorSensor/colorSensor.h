#ifndef COLOR_SENSOR_H
#define COLOR_SENSOR_H

#include <stdint.h>

#define COLOR_SENSOR_SLAVE_ADDRESS 0x29
#define COLOR_SENSOR_I2C_DEVICE "/dev/i2c-1"

#define COLOR_SENSOR_COMMAND 0x80
#define COLOR_SENSOR_ONCE    0x00
#define COLOR_SENSOR_INC     0x20

#define COLOR_SENSOR_ENABLE  0x00
#define COLOR_SENSOR_ATIME   0x01
#define COLOR_SENSOR_WTIME   0x03
#define COLOR_SENSOR_AILTL   0x04
#define COLOR_SENSOR_AILTH   0x05
#define COLOR_SENSOR_AIHTL   0x06
#define COLOR_SENSOR_AIHTH   0x07
#define COLOR_SENSOR_PERS    0x0C
#define COLOR_SENSOR_CONFIG  0x0D
#define COLOR_SENSOR_CONTROL 0x0F
#define COLOR_SENSOR_ID      0x12
#define COLOR_SENSOR_STATUS  0x13
#define COLOR_SENSOR_CDATAL  0x14
#define COLOR_SENSOR_CDATAH  0x15
#define COLOR_SENSOR_RDATAL  0x16
#define COLOR_SENSOR_RDATAH  0x17
#define COLOR_SENSOR_GDATAL  0x18
#define COLOR_SENSOR_GDATAH  0x19
#define COLOR_SENSOR_BDATAL  0x1A
#define COLOR_SENSOR_BDATAH  0x1B

#define COLOR_SENSOR_GA      1
#define COLOR_SENSOR_DF      310
#define COLOR_SENSOR_R_COEF  0.136f
#define COLOR_SENSOR_G_COEF  1
#define COLOR_SENSOR_B_COEF  -0.444
#define COLOR_SENSOR_CT_COEF 3810
#define COLOR_SENSOR_CT_OFF  1391

/**
 * @brief Basic struct for a color. Contains the RGB and HSL components.
 */
typedef struct {
    uint8_t r; /**< R component of the color */
    uint8_t g; /**< G component of the color */
    uint8_t b; /**< B component of the color */
    uint16_t h; /**< Hue component of the color */
    double s; /**< Saturation component of the color */
    double v; /**< Value component of the color */
} color_t;

/**
 * @brief Initializes the color sensor in periodic read mode.
 */
extern void cs_init();

/**
 * @brief Read the color and clear correct it
 * 
 * @param color Pointer to the structure to store the color information
 * @return (int) Exit code of the operation
 */
extern int  cs_read_clear_corrected(color_t* color);

/**
 * @brief Read the color raw
 * 
 * @param color Pointer to the structure to store the color information
 * @return (int) Exit code of the operation
 */
extern int  cs_read_raw(color_t* color);

/**
 * @brief Controlledly closes the sensor object
 */
extern void cs_close();
#endif
