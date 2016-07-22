/*
 * =====================================================================================
 *
 *       Filename:  ui.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  17/02/15 12:52:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dragan Glumac (draganglumac), dragan.glumac@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __UI_H__
#define __UI_H__

#include <ncurses.h>
#include <panel.h>

typedef struct {
  PANEL *panels[3];
	WINDOW *prompt;
	WINDOW *screen;	
  WINDOW *log;
  WINDOW *alert;
  int next_line;
  int next_log_line;
} ui_t;
typedef struct {
	ui_t *ui;
	char *msg;
} context_t;


ui_t *create_ui();
void destroy_ui(ui_t *ui);
char *get_message(ui_t *ui);
void display_local_message(ui_t *ui, char *msg);
void display_remote_message(ui_t *ui, char *msg);
void display_system_message(ui_t *ui, char *msg);
void show_chat(ui_t *ui);
void show_log(ui_t *ui);
void show_split(ui_t *ui);
void process_mouse_events(ui_t *ui);
void show_alert(ui_t *ui, char *message);
void hide_alert(ui_t *ui); 

#endif // __UI_H__
