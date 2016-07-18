/*
 * =====================================================================================
 *
 *       Filename:  ui_history.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  18/07/16 09:15:29
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <string.h>
#include "ui_history.h"


ui_history* ui_history_create() {
  ui_history *h = calloc(1, sizeof(ui_history));
  h->end = -1;
  return h;
}
void ui_history_destroy(ui_history **phist) {
  ui_history *h = *phist;
  int i;
  for(i = 0; i < MAX_HISTORY; i++) {
    if (h->history[i] != NULL) {
      free(h->history[i]->message);
      free(h->history[i]);
    }
    else {
      break;
    }
  }
  free(h);
  *phist = NULL;
}
void safe_increment(int *pos) {
  int temp = *pos;
  *pos = (temp + 1) % MAX_HISTORY;
}
void insert_at_end(ui_history *h, hist_item *hitem) {
  if (h->history[h->end] != NULL) {
    free(h->history[h->end]->message);
    free(h->history[h->end]);
  }
  h->history[h->end] = hitem;
}
void ui_history_add(ui_history *h, char *msg, MSG_TYPE type) {
  hist_item *item = malloc(sizeof(hist_item));
  item->type = type;
  // make a copy of the message
  int msg_len = strlen(msg);
  item->message = malloc(msg_len);
  strncpy(item->message, msg, msg_len);

  safe_increment(&h->end);
  if (h->start == h->end && h->history[0] != NULL) {
    safe_increment(&h->start);
  }
  insert_at_end(h, item);
}
jnx_list *ui_history_get(ui_history *h, int start, int num_items) {
  jnx_list *result = jnx_list_create();
  if (num_items > MAX_HISTORY)
    num_items = MAX_HISTORY;
  int count = 0;
  int next = (h->start + start) % MAX_HISTORY;
  while (count < num_items) {
    jnx_list_add(result, (void*)h->history[next]);
    safe_increment(&next);
    count++;
  }
  return result;
}
