/**
 * @brief Main file of the project.
 * @date March 21st of 2023
 * @authors David Andrino, Fernando Sanz
 */
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#include "Accelerometer/accelerometer.h"
#include "ColorSensor/colorSensor.h"

static volatile int gb_stop = 0;
static pthread_mutex_t g_stop_mutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_acc_mutex   = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_color_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_data_mutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_stop_cond    = PTHREAD_COND_INITIALIZER;
static pthread_cond_t g_data_cond    = PTHREAD_COND_INITIALIZER;

static acc_t        g_acceleration;
static color_t      g_rgb_color;
static volatile int gb_data_ready = 0;

static sigset_t g_stopsignals;

/**
 * @brief Get the stop flag, thread safe
 * 
 * @return (int) Stop flag. 1 if the threads should exit.
 */
static inline int get_stop() {
	// pthread_sigmask(SIG_BLOCK, &g_stopsignals, NULL);
	pthread_mutex_lock(&g_stop_mutex);
	int tmp_stop = gb_stop;
	pthread_mutex_unlock(&g_stop_mutex);
	// pthread_sigmask(SIG_UNBLOCK, &g_stopsignals, NULL);
	return tmp_stop;
}

/**
 * @brief ISR for the SIGINT signal. Sets the stop flag to 1 and notifies all sleeping threads
 * 
 * @param signal Signal identifier
 */
void sigint_isr(int signal) {
	#ifdef DEBUG
		printf("[DEBUG] SIGINT received\n");
	#endif
	gb_stop = 1;
	pthread_cond_broadcast(&g_stop_cond);
	pthread_cond_broadcast(&g_data_cond);
}

/**
 * @brief Thread for the acceleration sensor. Starts the sensor and periodically polls its content. When the stop flag is set, closes the sensor. 
 * 
 * @param ptr Not used
 * @return (void*) Not used
 */
void* acc_thread_fn(void *ptr) {
	#ifdef DEBUG
		printf("[DEBUG] Begin of acceleration thread\n");
	#endif
	acc_init();
	#ifdef DEBUG
		printf("[DEBUG] Acceleration sensor inited\n");
	#endif
	while (!get_stop()) {
		// Read acceleration from the sensor
		pthread_mutex_lock(&g_acc_mutex);
			acc_read(&g_acceleration);
		pthread_mutex_unlock(&g_acc_mutex);

		// Change data_ready flag to indicate the display thread to read
		pthread_mutex_lock(&g_data_mutex);
			gb_data_ready = 1;
			pthread_cond_broadcast(&g_data_cond);
		pthread_mutex_unlock(&g_data_mutex);
		usleep(500000);
	}
	#ifdef DEBUG
		printf("[DEBUG] Closing acceleration sensor\n");
	#endif
	acc_close();
	#ifdef DEBUG
		printf("[DEBUG] End of acceleration thread\n");
	#endif
	pthread_exit(NULL);
}

/**
 * @brief Thread for the color sensor. Starts the sensor and periodically polls its content. When the stop flag is set, closes the sensor. 
 * 
 * @param ptr Not used
 * @return (void*) Not used
 */
void* color_thread_fn(void *ptr) {
	#ifdef DEBUG
		printf("[DEBUG] Begin of color thread\n");
	#endif
	cs_init();
	#ifdef DEBUG
		printf("[DEBUG] Color sensor inited\n");
	#endif
	while (!get_stop()) {
		// Read acceleration from the sensor
		pthread_mutex_lock(&g_acc_mutex);
			cs_read_clear_corrected(&g_rgb_color);
		pthread_mutex_unlock(&g_acc_mutex);

		// Change data_ready flag to indicate the display thread to read
		pthread_mutex_lock(&g_data_mutex);
			gb_data_ready = 1;
			pthread_cond_broadcast(&g_data_cond);
		pthread_mutex_unlock(&g_data_mutex);
		usleep(500000);
	}
	#ifdef DEBUG
		printf("[DEBUG] Closing color sensor\n");
	#endif
	cs_close();
	#ifdef DEBUG
		printf("[DEBUG] End of color thread\n");
	#endif
	pthread_exit(NULL);
}

/**
 * @brief Display thread. When there is data ready, prints it to the screen. When the stop flag is set, prints the exit message.
 * 
 * @param ptr Not used
 * @return (void*) Not used
 */
void* display_thread_fn(void *ptr) {
	#ifdef DEBUG
		printf("[DEBUG] Begin of display thread\n");
	#endif

	printf("Raspberry Pi sensing application - By David Andrino and Fernando Sanz\n\n\n\n\n\n\n\n");
	while (!get_stop()) {
		pthread_mutex_lock(&g_data_mutex);
			int tmpstop;
			while (!gb_data_ready && !(tmpstop = get_stop())) 
				pthread_cond_wait(&g_data_cond, &g_data_mutex);

			if (tmpstop) {
				pthread_mutex_unlock(&g_data_mutex);
				break;
			}

			pthread_mutex_lock(&g_acc_mutex);
			pthread_mutex_lock(&g_color_mutex);
            #ifdef DEBUG
                printf("Print values \n");
            #else
				printf("\033[7A\r");
				printf("Acceleration:\n"
				"\tX: %.02f\n"
				"\tY: %.02f\n"
				"\tZ: %.02f\n"
				"\033[38;2;%d;%d;%dmColor:\n"
				"\tR: %03d\n"
				"\tG: %03d\n"
				"\tB: %03d",
				g_acceleration.x, g_acceleration.y, g_acceleration.z,
				g_rgb_color.r, g_rgb_color.g, g_rgb_color.b,
				g_rgb_color.r, g_rgb_color.g, g_rgb_color.b);
            #endif

			pthread_mutex_unlock(&g_acc_mutex);
			pthread_mutex_unlock(&g_color_mutex);

			usleep(500000);
            gb_data_ready = 0;
		pthread_mutex_unlock(&g_data_mutex);
	}

	printf("\033[7A\r");
	for (int i = 0; i < 8; i++) printf("                                    \n");
	printf("\033[7A\rApplication succesfully ended\n");
	#ifdef DEBUG
		printf("[DEBUG] End of display thread\n");
	#endif
	pthread_exit(NULL);
}

/**
 * @brief Thread to take and process user input. 
 * 
 * @param ptr Not used
 * @return (void*) Not used
 */
void* input_thread_fn(void *ptr) {
	#ifdef DEBUG
		printf("[DEBUG] Begin of input thread\n");
	#endif

	while (!get_stop()) {
		/* char c = getc(stdin);
		switch (c) {
			case 'q': 
				pthread_sigmask(SIG_BLOCK, &g_stopsignals, NULL);
				pthread_mutex_lock(&g_stop_mutex);
				int gb_stop = 1;
				pthread_mutex_unlock(&g_stop_mutex);
				pthread_sigmask(SIG_UNBLOCK, &g_stopsignals, NULL);
				break;
			default:
				printf("\033[F\033[F\033[F\033[F\033[F\033[F\033[F\033[F\rPressed %c\n\n\n\n\n\n\n", c);
				break;
		} */
        usleep(500000);
	}

	#ifdef DEBUG
		printf("[DEBUG] End of input thread\n");
	#endif

	pthread_exit(NULL);
}

/**
 * @brief Entry point for the project. Starts all the threads and waits for them to finish to close the program.
 * 
 * @return (int) Exit code of the program
 */
int main() {
	sigemptyset(&g_stopsignals);
	sigaddset(&g_stopsignals, SIGINT);

	pthread_t acc_tid, color_tid, display_tid, input_tid; 

	signal(SIGINT, sigint_isr);

	#ifdef DEBUG
		printf("[DEBUG] Creating Threads\n");
	#endif
	pthread_create(&acc_tid,     NULL, acc_thread_fn,     NULL);
	pthread_create(&color_tid,   NULL, color_thread_fn,   NULL);
	pthread_create(&input_tid,   NULL, input_thread_fn,   NULL);
	pthread_create(&display_tid, NULL, display_thread_fn, NULL);


	// Wait for stop to be activated
	pthread_mutex_lock(&g_stop_mutex);
		while (gb_stop == 0) pthread_cond_wait(&g_stop_cond, &g_stop_mutex);
	pthread_mutex_unlock(&g_stop_mutex);

	#ifdef DEBUG
		printf("[DEBUG] Exiting threads\n");
	#endif
	// Wait for the threads to exit
	pthread_join(acc_tid,     NULL);
	pthread_join(color_tid,   NULL);
	pthread_join(input_tid,   NULL);
	pthread_join(display_tid, NULL);
	#ifdef DEBUG
		printf("[DEBUG] All threads finished, exiting program\n");
	#endif

	exit(0);
}
