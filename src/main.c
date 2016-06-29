/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  17/02/15 12:41:15
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

#include "app.h"

int main(int argc, char **argv) {
	init_app();
	ui_t *ui = create_ui();

	context_t *context = malloc(sizeof(context_t));
	context->ui = ui;
	context->msg = NULL;
	pthread_t read_thread;
	pthread_create(&read_thread, 0, read_loop, (void *) context);

	while (TRUE) {
		if (-1 == output_next_message_in_context(context)) {
			break;
		}
	}
	
	destroy_ui(ui);
	return 0;
}
