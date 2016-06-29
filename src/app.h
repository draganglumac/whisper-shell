/*
 * =====================================================================================
 *
 *       Filename:  app.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  18/02/15 16:54:21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dragan Glumac (draganglumac), dragan.glumac@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __APP_H__
#define __APP_H__

#include "ui.h"

typedef struct {
	ui_t *ui;
	char *msg;
} context_t;

void init_app();
void *read_loop(void *data);
int output_next_message_in_context(context_t *context);

#endif // __APP_H__
