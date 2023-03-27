#include "colorSensor.h"
#include <stdio.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

static int fd;

void cs_writeRegister(uint8_t reg, uint8_t data);

static void cs_readRawColorRegisters(uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c) {
    uint8_t reading[8];
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];

    uint8_t command = COLOR_SENSOR_COMMAND | COLOR_SENSOR_INC | COLOR_SENSOR_CDATAL;  
    
    messages[0].addr  = COLOR_SENSOR_SLAVE_ADDRESS;
    messages[0].flags = 0;
    messages[0].len   = sizeof(command);
    messages[0].buf   = &command;

    messages[1].addr  = COLOR_SENSOR_SLAVE_ADDRESS;
    messages[1].flags = I2C_M_RD;
    messages[1].len   = sizeof(reading);
    messages[1].buf   = reading;

    packets.msgs  = messages;
    packets.nmsgs = 2;

    ioctl(fd, I2C_RDWR, &packets);

    *c = (((uint16_t) reading[1]) << 8) | reading[0];
    *r = (((uint16_t) reading[3]) << 8) | reading[2];
    *g = (((uint16_t) reading[5]) << 8) | reading[4];
    *b = (((uint16_t) reading[7]) << 8) | reading[6];
}

void readFer(rgbColor_t *color) {
    uint8_t meas[8];
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];

    uint8_t command =COLOR_SENSOR_COMMAND | COLOR_SENSOR_INC | COLOR_SENSOR_CDATAL;

    messages[0].addr = COLOR_SENSOR_SLAVE_ADDRESS;
    messages[0].flags = 0;
    messages[0].len = sizeof(command);
    messages[0].buf = &command;

    messages[1].addr = COLOR_SENSOR_SLAVE_ADDRESS;
    messages[1].flags = I2C_M_RD;
    messages[1].len = sizeof(meas);
    messages[1].buf = meas;

    packets.msgs = messages;
    packets.nmsgs = 2;

    ioctl(fd, I2C_RDWR, &packets);

    uint16_t clear = meas[1] << 8 | meas[0];

    if (clear == 0) return;

    color->r = 255 * (uint16_t)((meas[3] << 8) | meas[2]) / clear;
    color->g = 255 * (uint16_t)((meas[5] << 8) | meas[4]) / clear;
    color->b = 255 * (uint16_t)((meas[7] << 8) | meas[6]) / clear;
}

void cs_init() {
    fd = open(COLOR_SENSOR_I2C_DEVICE, O_RDWR);
    ioctl(fd, I2C_SLAVE, COLOR_SENSOR_SLAVE_ADDRESS);

    cs_writeRegister(COLOR_SENSOR_ENABLE, 0x0B); // Enable with wait (AEN & WEN & !PON)
    usleep(2400);                                // Wait 2.4ms to reset
    cs_writeRegister(COLOR_SENSOR_ATIME, 0x0C);  // 154 ms integration time max 65535
    cs_writeRegister(COLOR_SENSOR_WTIME, 0xAB);  // 204 ms wait time
}

void cs_close() {
    close(fd);
}

int cs_readClearCorrectedColor(rgbColor_t *color) {
    // Get colors
    uint16_t r, g, b, c;
    cs_readRawColorRegisters(&r, &g, &b, &c);   

    if (c == 0) {
        color->r = 0;
        color->g = 0;
        color->b = 0;
        return -1;
    }

    // Convert to compensated (with clear) RGB
    color->r = (uint8_t)(255.0f * r / c);
    color->g = (uint8_t)(255.0f * g / c);
    color->b = (uint8_t)(255.0f * b / c);

    if (color->r > 255 || color->g > 255 || color->b > 255) printf("ERROR\n\n\n\n");
    return 0;
}

int  cs_readRawColor(rgbColor_t* color) {
    // Get colors
    uint16_t r, g, b, c;
    cs_readRawColorRegisters(&r, &g, &b, &c);   

    // Convert to RGB
    color->r = (uint8_t)(255.0f * r / 0xFFFF);
    color->g = (uint8_t)(255.0f * g / 0xFFFF);
    color->b = (uint8_t)(255.0f * b / 0xFFFF);

    return 0;
}

void cs_writeRegister(uint8_t reg, uint8_t data) {
    uint8_t m2sBuf[2]; // Master to slave
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[1];

    // Set data
    m2sBuf[0] = COLOR_SENSOR_COMMAND | COLOR_SENSOR_ONCE | reg;
    m2sBuf[1] = data;

    // Request message
    messages[0].addr  = COLOR_SENSOR_SLAVE_ADDRESS;
    messages[0].flags = 0;
    messages[0].len   = sizeof(m2sBuf);
    messages[0].buf   = m2sBuf;

    // Create communication packet
    packets.msgs  = messages;
    packets.nmsgs = 1;

    // Send packet
    ioctl(fd, I2C_RDWR, &packets);
}