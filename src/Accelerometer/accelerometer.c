#include "accelerometer.h"
#include <stdio.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

static int g_fd;

void acc_write_register(uint8_t reg, uint8_t data) {
    uint8_t m2sBuf[2]; // Master to slave
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[1];

    // Set data
    m2sBuf[0] = reg;
    m2sBuf[1] = data;

    // Request message
    messages[0].addr  = ACC_I2C_SLAVE_ADDRESS;
    messages[0].flags = 0;
    messages[0].len   = sizeof(m2sBuf);
    messages[0].buf   = m2sBuf;

    // Create communication packet
    packets.msgs  = messages;
    packets.nmsgs = 1;

    // Send packet
    ioctl(g_fd, I2C_RDWR, &packets);
}

void acc_read(acc_t* acc) {
    uint8_t m2sBuf; // Master to slave, slave to master
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];
    uint8_t reading[6];

    // Select register
    m2sBuf = ACC_REG_BASE;

    // Request message
    messages[0].addr  = ACC_I2C_SLAVE_ADDRESS;
    messages[0].flags = 0;
    messages[0].len   = sizeof(m2sBuf);
    messages[0].buf   = &m2sBuf;

    // Catch response message with data
    messages[1].addr  = ACC_I2C_SLAVE_ADDRESS;
    messages[1].flags = I2C_M_RD;
    messages[1].len   = sizeof(reading);
    messages[1].buf   = reading;

    // Create communication packet
    packets.msgs  = messages;
    packets.nmsgs = 2;

    // Send packet
    ioctl(g_fd, I2C_RDWR, &packets);

    #ifdef DEBUG
        printf("[DEBUG] Read %02X\n", reading[0]);
        printf("[DEBUG] Read %02X\n", reading[1]);
        printf("[DEBUG] Read %02X\n", reading[2]);
        printf("[DEBUG] Read %02X\n", reading[3]);
        printf("[DEBUG] Read %02X\n", reading[4]);
        printf("[DEBUG] Read %02X\n", reading[5]);
    #endif

	acc->x = -(((int16_t)(reading[0] << 8 | reading[1])) / 16384.0);
	acc->y = -(((int16_t)(reading[2] << 8 | reading[3])) / 16384.0);
	acc->z = -(((int16_t)(reading[4] << 8 | reading[5])) / 16384.0);
}

void acc_init() {
    g_fd = open(ACC_I2C_DEVICE, O_RDWR);
    ioctl(g_fd, I2C_SLAVE, ACC_I2C_SLAVE_ADDRESS);
    acc_write_register(ACC_POWER_REG_1, 0x80); // Initial reset
    usleep(500000);
    acc_write_register(ACC_POWER_REG_2, 0X07); // Disable Gyro and 1.25 Hz
    acc_write_register(ACC_POWER_REG_1, 0X28); // Cycle Mode, internal clk, temperature off 
    usleep(500000);
}

void acc_close() {
    close(g_fd);
}
