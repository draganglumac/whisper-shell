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
#include <jnxc_headers/jnx_log.h>
#include "ui.h"
static char *baddr = NULL;
static connection_controller *connectionc;
static session_controller *sc;
static discovery_service *ds = NULL;
static char *interface = NULL;

static ui_t *ui = NULL;

void on_new_session_message(const session *s, 
    const connection_request *c, const jnx_char *message,
    jnx_size message_len) {

  display_remote_message(ui,(char*)message);
}

int main(int argc, char **argv) {

  FILE* fp;
  if ((fp = fopen("logtest.conf", "a+")) == NULL) {
    perror("file: ");
    return -1;
  }
  JNXLOG_OUTPUT_REDIRECT_START(fp);

  ui = create_ui();
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

      char *username = getenv("USER");
      if(!username) {
        username = "local";
      }
      peerstore *store = peerstore_init(local_peer_for_user(username,
            10,interface), 0);

      get_broadcast_ip(&baddr,interface);
      ds = discovery_service_create(1234,AF_INET,baddr,store);
      discovery_service_start(ds,BROADCAST_UPDATE_STRATEGY);

      connectionc = connection_controller_create("8080",
          AF_INET, ds,NULL,NULL,NULL,NULL);

      sc = session_controller_create(connectionc,on_new_session_message);

      should_tick_core = 1;
    }
    if(strcmp(message,":stop") == 0) {
      display_system_message(ui,"STOPPING WHISPER_CORE"); 
      session_controller_destroy(&sc);
      connection_controller_destroy(&connectionc);
      should_tick_core = 0;
    }
    if(should_tick_core) {
      if(connectionc) {
        connection_controller_tick(connectionc);
      }
    }
  }

  destroy_ui(ui);
    JNXLOG_OUTPUT_REDIRECT_END()
   fclose(fp);
  return 0;
}
