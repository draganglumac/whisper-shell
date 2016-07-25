/*
 * =====================================================================================
 *
 *       Filename:  ui_display_log.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  21/07/16 13:26:14
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
#include "ui_display_log.h"

#define MAX_BUF 5012

char *ui_display_log_chunk(ui_t *ui, char *chunk, char *leftover) {
  char *buffer = calloc(MAX_BUF, sizeof(char));
  char *pb, *pm = chunk;
  if (leftover != NULL) {
    strncpy(buffer, leftover, strlen(leftover));
    pb = buffer + strlen(buffer);
    free(leftover);
  }
  else {
    pb = buffer;
  }
  while (*pm != '\0') {
    if (*pm == '\n') {
      *pb = '\0';
      display_system_message(ui, buffer);
      pb = buffer;
      ++pm;
    }
    else {
      *pb = *pm;
      ++pm;
      ++pb;
    }
  }
  return buffer;
}
