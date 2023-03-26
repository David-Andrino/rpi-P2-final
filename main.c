/*
 *  main.c - Main file of the project
 *
 *  Created on: Mar 21=02023
 *      Author: David Andrino & Fernando Sanz
 */
#include <stdio.h>
#include "Accelerometer/accelerometer.h"
#include "ColorSensor/colorSensor.h"
#include <stdint.h>
#include <signal.h>

static int stop = 0;

void closeISR(int signal) {
	stop = 1;
}

int main(int argc, char** argv) {
	signal(SIGINT, closeISR);

	acc_t acceleration;
	rgbColor_t color;

	cs_init();
	acc_init();

	printf("Sensors - David Andrino & Fernando Sanz\n\n\n\n\n\n");
	while (!stop) {
	cs_readClearCorrectedColor(&color);
	acc_read(&acceleration);

	printf("\033[F\033[F\033[F\033[F\033[F\r"
			"Color: R,G,B: (%03d, %03d, %03d)\n"
			"Acceleration:\n"
			"    X: %03f\n"
			"    Y: %03f\n"
			"    Z: %03f\n",
			color.r, color.g, color.b, acceleration.x, acceleration.y, acceleration.z);

	}
	cs_close();
	acc_close();
	return 0;
}
