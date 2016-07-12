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
#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <whisper-core/session_controller.h>
#include <jnxc_headers/jnx_log.h>
#include <jnxc_headers/jnx_term.h>
#include <jnxc_headers/jnx_thread.h>
#include <time.h>
#include "ui.h"
static char *baddr = NULL;
static connection_controller *connectionc;
static session_controller *sc;
static discovery_service *ds = NULL;
static char *interface = NULL;
static peerstore *store = NULL;
static ui_t *ui = NULL;
static int log_thread_run = 1;

static int app_is_input_guid_size(char *input) {
  if ((strlen(input) / 2) == 16) {
      return 1;
    }
  return 0;
}
peer *app_peer_from_input(char *param) {
  if (app_is_input_guid_size(param)) {
      jnx_guid g;
      jnx_guid_from_string(param, &g);
      peer *p = peerstore_lookup(store, &g);
      return p;
    } else {
        peer *p = peerstore_lookup_by_username(store, param);
        return p;
      }
  return NULL;
}
static void pretty_print_peer(int i, peer *p) {
  char *guid;
  jnx_guid_to_string(&p->guid, &guid);
  size_t needed = snprintf(NULL,0,"\n(%d) %-32s %-16s %s\n", i, guid, p->host_address, p->user_name);
  char  *buffer = malloc(needed);
  snprintf(buffer,needed,"\n(%d) %-32s %-16s %s\n", i, guid, p->host_address, p->user_name);
  display_system_message(ui,buffer);
  free(buffer);
  free(guid);
}
static void pretty_print_peer_in_col(peer *p, jnx_int32 colour) {
  char *guid;
  jnx_guid_to_string(&p->guid, &guid);
  jnx_term_printf_in_color(colour,
      "%-32s %-16s %s\n", guid, p->host_address, p->user_name);
  free(guid);
}

static void show_active_peers(peerstore *ps) {
  jnx_guid **active_guids;
  display_system_message(ui,"\nPeers:\n");
  if(!ps) {
    display_system_message(ui,"\nPeerstore not initialised - start whisper core\n");
    return;
  }
  int num_guids = peerstore_get_active_guids(ps, &active_guids);
  int i;
  if(num_guids == 0) {
    display_system_message(ui,"\nNo peers found\n");
    return;
  }
  peer *local = peerstore_get_local_peer(ps);
  for (i = 0; i < num_guids; i++) {
    jnx_guid *next_guid = active_guids[i];
    peer *p = peerstore_lookup(ps, next_guid);
    if (peers_compare(p, local) != 0) {
      pretty_print_peer(i,p);
    }
  }
}

void on_new_session_message(const session *s, 
    const connection_request *c, const jnx_char *message,
    jnx_size message_len) {

  display_system_message(ui,(char*)message);
}

FILE *JNXLOG_OUTPUT_FP = NULL;

void *run_log_thread(void *args) {
  sleep(2);
  //delayed start
  ui_t *ui = (ui_t*)args;
  int fd = open("logtest.conf", O_RDONLY);
  if (fd < 0) {
    perror("read file: ");
    exit(1);
  }

  struct timespec interval;
  interval.tv_sec = 0;
  interval.tv_nsec = 10L * 1000 * 1000;
  ssize_t bytesread = 0, last_read_pos = 0, end_pos = 0, offset = 0;
  while(log_thread_run) {
    end_pos = lseek(fd,0L,SEEK_END);
    offset = end_pos - last_read_pos;
    if (offset > 0) {
      char *message = malloc(offset + 1);
      lseek(fd, last_read_pos, SEEK_SET);
      bytesread = read(fd, (void*)message, offset);
      message[bytesread+1] = '\0';
      last_read_pos += bytesread;
      display_system_message(ui, message);
      //      free(message);
    }
    nanosleep(&interval, NULL);
  }
  close(fd);
  return NULL;
}
int main(int argc, char **argv) {

  system("rm -rf logtest.conf");

  FILE* fp;
  if ((fp = fopen("logtest.conf", "w")) == NULL) {
    perror("file: ");
    return -1;
  }
  if (0 != setvbuf(fp, NULL, _IOLBF, 0)) {
    perror("setbuf: ");
    return -1;
  }
  JNXLOG_OUTPUT_FP = fp;
  jnx_log_set_output(fp);
  ui = create_ui();
  context_t *context = malloc(sizeof(context_t));
  context->ui = ui;
  context->msg = NULL;

  int should_tick_core = 0;

  int current_log_line = 0;

  jnx_thread_create_disposable(run_log_thread,ui);

  while(TRUE) {

    char *message = get_message(ui);

    if(strcmp(message,":q") == 0) {
      break;
    }
    if(strcmp(message,":help") == 0) {
      display_system_message(ui,"\nCOMMANDS\n :q to quit\n :help for help\n :start to start whisper-core\n :peers to list peers\n :stop to stop whisper-core\n");
    }
    if(strcmp(message,":connect") == 0) {
      display_system_message(ui,"\n Name of user to connect to:\n");
      message = get_message(ui);

      peer *p = app_peer_from_input(message);
      if(!p) {
        display_system_message(ui,"\nPeer not found\n");
        continue;
      }else {
        display_system_message(ui,"\nConnecting...\n");

        session *ses = session_controller_session_create(sc,p);

        while(!session_controller_is_session_ready(sc,ses)) {
          connection_controller_tick(connectionc);
          sleep(1.0);
        }
      }
    }
    if(strcmp(message,":peers") == 0) {

      show_active_peers(store);
    }
    if(strcmp(message,":start") == 0) {
      display_system_message(ui,"STARTING WHISPER_CORE\n"); 

      char *username = getenv("USER");
      if(!username) {
        username = "local";
      }
      store = peerstore_init(local_peer_for_user(username,
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
      log_thread_run = 0;
      display_system_message(ui,"STOPPING WHISPER_CORE\n"); 
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
  jnx_log_set_output(NULL);
  fclose(fp);
  return 0;
}
