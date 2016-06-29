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
#include <string.h>
#include <whisper-core/session_controller.h>
#include "ui.h"

int main(int argc, char **argv) {
  ui_t *ui = create_ui();

  context_t *context = malloc(sizeof(context_t));
  context->ui = ui;
  context->msg = NULL;

  int should_tick_core = 0;


  while(TRUE) {
    char *message = get_message(ui);

    if(strcmp(message,":q") == 0) {
      break;
    }
    if(strcmp(message,":help") == 0) {
      display_system_message(ui,"COMMANDS\n :q to quit\n :help for help\n :start to start whisper-core\n :stop to stop whisper-core");
    }

    if(strcmp(message,":start") == 0) {
      display_system_message(ui,"STARTING WHISPER_CORE"); 
    }
    if(strcmp(message,":stop") == 0) {
      display_system_message(ui,"STOPPING WHISPER_CORE"); 
    }
    if(should_tick_core) {
    
    }
  }

  destroy_ui(ui);
  return 0;
}
