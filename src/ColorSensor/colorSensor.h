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

typedef struct {
    uint8_t r, g, b;
} rgbColor_t;

typedef struct {
    uint8_t r, g, b, c;
    uint8_t correctedR, correctedG, correctedB;
    float   IR, temperature, saturation, lux;
} colorParams_t;

extern void cs_init();
extern int  cs_readClearCorrectedColor(rgbColor_t* color);
extern int  cs_readRawColor(rgbColor_t* color);
extern void cs_readColorParams(colorParams_t* params);
extern void cs_close();
extern void readFer(rgbColor_t* color);
#endif
