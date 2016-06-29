/*
 * =====================================================================================
 *
 *       Filename:  app.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  18/02/15 16:57:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dragan Glumac (draganglumac), dragan.glumac@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "app.h"

static pthread_mutex_t output_mutex;
static pthread_cond_t output_cond;

void init_app() {
	pthread_mutex_init(&output_mutex, NULL);
	pthread_cond_init(&output_cond, NULL);
}
void signal_message() {
	int retval;
	if ((retval = pthread_cond_signal(&output_cond)) != 0) {
		printf("Error in signaling arrival of the mesage, Error %d\n", retval);
	}
}
void wait_for_message() {
	int retval;
	if ((retval = pthread_cond_wait(&output_cond, &output_mutex)) != 0) {
		printf("Error in waiting for the mesage, Error %d\n", retval);
	}
}
void send_message_to_context(context_t *context, char *message) {
	pthread_mutex_lock(&output_mutex);
	context->msg = message;
	pthread_mutex_unlock(&output_mutex);
	signal_message();
}

void *read_loop(void *data) {
	context_t *context = (context_t *) data;
	while(TRUE) {
		char *msg = get_message(context->ui);
		send_message_to_context(context, msg);
	}
	return NULL;
}
int output_next_message_in_context(context_t *context) {
	pthread_mutex_lock(&output_mutex);	
	wait_for_message();
	ui_t *cui = context->ui;
	char *msg = context->msg;
	if (strcmp(msg, ":q") == 0) {
		pthread_mutex_unlock(&output_mutex);
		return -1;
	}
	else if (strncmp(msg, "r/", 2) == 0) {
		display_remote_message(cui, msg);
	}
	else {
		display_local_message(cui, msg);
	}
	pthread_mutex_unlock(&output_mutex);
	return 0;
}
