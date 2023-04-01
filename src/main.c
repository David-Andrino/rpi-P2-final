/*
 *  main.c - Main file of the project
 *
 *  Created on: Mar 21=02023
 *      Author: David Andrino & Fernando Sanz
 */
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

#include "Accelerometer/accelerometer.h"
#include "ColorSensor/colorSensor.h"

static volatile int stop = 0;
static pthread_mutex_t stopMutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t accMutex   = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t colorMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t stopCond    = PTHREAD_COND_INITIALIZER;
static pthread_cond_t accCond     = PTHREAD_COND_INITIALIZER;
static pthread_cond_t colorCond   = PTHREAD_COND_INITIALIZER;

void closeISR(int signal) {
	stop = 1;
}

void* accThFn(void *ptr) {
	int accStop = 0;
	while (1) {
		pthread_mutex_lock(&stopMutex);
		accStop = stop;
		pthread_mutex_unlock(&stopMutex);

		if (accStop) break;
	}
	pthread_exit(NULL);
}

void* inputThFn(void *ptr) {
	int inputStop = 0;
	while (1) {
		// Check stop flag
		pthread_mutex_lock(&stopMutex);
		inputStop = stop;
		pthread_mutex_unlock(&stopMutex);

		if (inputStop) break;
	}
	pthread_exit(NULL);
}

void* colorThFn(void *ptr) {
	int colorStop = 0;
	while (1) {
		// Check stop flag
		pthread_mutex_lock(&stopMutex);
		colorStop = stop;
		pthread_mutex_unlock(&stopMutex);

		if (colorStop) break;
	}
	pthread_exit(NULL);
}

void* displayThFn(void *ptr) {
	int displayStop = 0;
	while (1) {
		// Check stop flag
		pthread_mutex_lock(&stopMutex);
		displayStop = stop;
		pthread_mutex_unlock(&stopMutex);

		if (displayStop) break;
	}
	pthread_exit(NULL);
}

int main(int argc, char** argv) {
	pthread_t accTID, colorTID, displayTID, inputTID; 

	signal(SIGINT, closeISR);

	pthread_create(accTID,     NULL, accThFn,     NULL);
	pthread_create(colorTID,   NULL, colorThFn,   NULL);
	pthread_create(inputTID,   NULL, inputThFn,   NULL);
	pthread_create(displayTID, NULL, displayThFn, NULL);


	// Wait for stop to be activated
	pthread_mutex_lock(&stopMutex);
		while (stop == 0) pthread_cond_wait(&stopCond, &stopMutex);
	pthread_mutex_unlock(&stopMutex);

	// Wait for the threads to exit
	pthread_join(accTID,     NULL);
	pthread_join(colorTID,   NULL);
	pthread_join(inputTID,   NULL);
	pthread_join(displayTID, NULL);

	exit(0);
}
