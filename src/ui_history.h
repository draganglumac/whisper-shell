/*
 * =====================================================================================
 *
 *       Filename:  ui_history.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  18/07/16 09:01:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author: Dragan Glumac
 *   Organization:  The Achievers
 *
 * =====================================================================================
 */
#ifndef __UI_HISTORY_H__
#define __UI_HISTORY_H__

#include <jnxc_headers/jnx_list.h>

#define MSG_LOCAL 0
#define MSG_REMOTE 1
#define MSG_SYSTEM 2

typedef unsigned short MSG_TYPE;

typedef struct {
  MSG_TYPE type;
  char *message;
} hist_item;

#ifndef MAX_HISTORY
#define MAX_HISTORY 1024
#endif // MAX_HISTORY

typedef struct {
  hist_item *history[MAX_HISTORY];
  int start;
  int end;
} ui_history;

/* PUBLIC API */
// Create ui_history record 
ui_history* ui_history_create();

// Destroy history and its data and set *phist to NULL
void ui_history_destroy(ui_history **phist);

// Add a hist_item * to the history
void ui_history_add(ui_history *hist, char *msg, MSG_TYPE type);

// Returns a linked list of (hist_item *) from position `(h->start + start) % MAX_HISTORY`
jnx_list* ui_history_get(ui_history *h, int start, int num_items);

#endif // __UI_HISTORY_H__
