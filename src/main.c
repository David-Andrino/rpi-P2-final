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

static volatile int gb_stop = 0;
static pthread_mutex_t g_stop_mutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_acc_mutex   = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_color_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_stop_cond    = PTHREAD_COND_INITIALIZER;
static pthread_cond_t g_acc_cond     = PTHREAD_COND_INITIALIZER;
static pthread_cond_t g_color_cond   = PTHREAD_COND_INITIALIZER;

void sigint_isr(int signal) {
	gb_stop = 1;
}

void* acc_thread_fn(void *ptr) {
	int b_acc_stop = 0;
	while (1) {
		pthread_mutex_lock(&g_stop_mutex);
		b_acc_stop = gb_stop;
		pthread_mutex_unlock(&g_stop_mutex);

		if (b_acc_stop) break;
	}
	pthread_exit(NULL);
}

void* input_thread_fn(void *ptr) {
	int b_input_stop = 0;
	while (1) {
		// Check stop flag
		pthread_mutex_lock(&g_stop_mutex);
		b_input_stop = gb_stop;
		pthread_mutex_unlock(&g_stop_mutex);

		if (b_input_stop) break;
	}
	pthread_exit(NULL);
}

void* color_thread_fn(void *ptr) {
	int b_color_stop = 0;
	while (1) {
		// Check stop flag
		pthread_mutex_lock(&g_stop_mutex);
		b_color_stop = gb_stop;
		pthread_mutex_unlock(&g_stop_mutex);

		if (b_color_stop) break;
	}
	pthread_exit(NULL);
}

void* display_thread_fn(void *ptr) {
	int b_display_stop = 0;
	while (1) {
		// Check stop flag
		pthread_mutex_lock(&g_stop_mutex);
		b_display_stop = gb_stop;
		pthread_mutex_unlock(&g_stop_mutex);

		if (b_display_stop) break;
	}
	pthread_exit(NULL);
}

int main(int argc, char** argv) {
	pthread_t acc_tid, color_tid, display_tid, input_tid; 

	signal(SIGINT, sigint_isr);

	pthread_create(acc_tid,     NULL, acc_thread_fn,     NULL);
	pthread_create(color_tid,   NULL, color_thread_fn,   NULL);
	pthread_create(input_tid,   NULL, input_thread_fn,   NULL);
	pthread_create(display_tid, NULL, display_thread_fn, NULL);


	// Wait for stop to be activated
	pthread_mutex_lock(&g_stop_mutex);
		while (gb_stop == 0) pthread_cond_wait(&g_stop_cond, &g_stop_mutex);
	pthread_mutex_unlock(&g_stop_mutex);

	// Wait for the threads to exit
	pthread_join(acc_tid,     NULL);
	pthread_join(color_tid,   NULL);
	pthread_join(input_tid,   NULL);
	pthread_join(display_tid, NULL);

	exit(0);
}
