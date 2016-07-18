/*
 * =====================================================================================
 *
 *       Filename:  ui.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  17/02/15 12:50:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dragan Glumac (draganglumac), dragan.glumac@gmail.com
 *   Organization
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#include "ui.h"
#include "ui_history.h"

#define COL_LOGO   1
#define COL_LOCAL  2
#define COL_REMOTE 3
#define COL_SYS    4

#define CHAT ui->panels[0]
#define LOG  ui->panels[1]

static char system_buffer[1024];
static char *pb = system_buffer;

ui_history *chat_history;
ui_history *log_history;

void init_colours() {
  if (has_colors() == FALSE) {
    endwin();
    printf("Your terminal does not support colours.\n");
    exit(1);
  }
  start_color();
  init_pair(COL_LOGO, COLOR_WHITE, COLOR_BLUE);
  init_pair(COL_LOCAL, COLOR_WHITE, COLOR_BLACK);
  init_pair(COL_REMOTE, COLOR_GREEN, COLOR_BLACK);
  init_pair(COL_SYS, COLOR_BLUE, COLOR_BLACK);
}
void show_prompt(ui_t *ui) {
  wmove(ui->prompt, 1, 1);
  wclear(ui->prompt);
  mvwprintw(ui->prompt, 1, 1, "$> ");
  wrefresh(ui->prompt);
}
void display_logo() {
  attron(COLOR_PAIR(COL_LOGO) | A_BOLD);
  printw("%s", " Whisper Shell ");
  attroff(COLOR_PAIR(COL_LOGO) | A_BOLD);
  refresh();
}
ui_t *create_ui() {
  ui_t *ui = malloc(sizeof(ui_t));

  initscr();
  init_colours();
  display_logo();

  ui->screen = newwin(LINES - 6, COLS - 1, 1, 1);
  scrollok(ui->screen, TRUE);
  box(ui->screen, 0, 0);
  ui->next_line = 1;
  CHAT = new_panel(ui->screen);
  chat_history = ui_history_create();

  ui->log = newwin(LINES - 6, COLS - 1, 1, 1);
  scrollok(ui->log, TRUE);
  box(ui->log, 0, 0);
  ui->next_log_line = 1;
  LOG = new_panel(ui->log);
  log_history = ui_history_create();

  set_panel_userptr(CHAT, LOG);
  set_panel_userptr(LOG, CHAT);
  
  top_panel(CHAT);
  hide_panel(LOG);
  update_panels();
  doupdate();

  ui->prompt = newwin(4, COLS - 1, LINES - 5, 1);
  show_prompt(ui);

  return ui;
}
void destroy_ui(ui_t *ui) {
  del_panel(CHAT);
  delwin(ui->screen);
  ui_history_destroy(&chat_history);
  
  del_panel(LOG);
  delwin(ui->prompt);
  ui_history_destroy(&log_history);

  endwin();
  free(ui);
}
char *get_message(ui_t *ui) {
  char *msg = malloc(1024);
  wmove(ui->prompt, 1, 4);
  wgetstr(ui->prompt, msg);
  show_prompt(ui);
  return msg;
}
void update_next_line(ui_t *ui) {
  int lines, cols;
  getmaxyx(ui->screen, lines, cols);
  if (ui->next_line >= --lines) {
    scroll(ui->screen);
    ui->next_line--;
  }
  else {
    getyx(ui->screen, lines, cols);
    ui->next_line = lines;
  }
}
void update_next_log_line(ui_t *ui) {
  int lines, cols;
  getmaxyx(ui->log, lines, cols);
  if (ui->next_log_line >= --lines) {
    scroll(ui->log);
    ui->next_log_line--;
  }
  else {
    getyx(ui->log, lines, cols);
    ui->next_log_line = lines;
  }
}
void display_message(ui_t *ui, char *msg, int col_flag) {
  int row, col;
  getyx(ui->prompt, row, col);
  wattron(ui->screen, COLOR_PAIR(col_flag));
  mvwprintw(ui->screen, ui->next_line, 1, "%s\n", msg);
  wattroff(ui->screen, COLOR_PAIR(col_flag));
  update_next_line(ui);
  box(ui->screen, 0, 0);
  update_panels();
  doupdate();
  wmove(ui->prompt, row, col);
  wrefresh(ui->prompt);
}
void display_status_message(ui_t *ui, char *msg, int col_flag) {
  int row, col;
  getyx(ui->prompt, row, col);
  char *pm = msg;
  while (*pm != '\0') {
    if (*pm == '\n') {
      *pb = '\0';
      wattron(ui->log, COLOR_PAIR(col_flag));
      mvwprintw(ui->log, ui->next_log_line, 1, "%s\n", system_buffer);
      update_next_log_line(ui);
      wattroff(ui->log, COLOR_PAIR(col_flag));
      box(ui->log, 0, 0);
      update_panels();
      doupdate();
      pb = system_buffer;
      ++pm;
    }
    else {
      *pb = *pm;
      ++pm;
      ++pb;
    }
  }
  wmove(ui->prompt, row, col);
  wrefresh(ui->prompt);
}
void display_local_message(ui_t *ui, char *msg) {
  display_message(ui, msg, COL_LOCAL);
  free(msg);
}
void display_remote_message(ui_t *ui, char *msg) {
  display_message(ui, msg + 2, COL_REMOTE);
  free(msg);
}
void display_system_message(ui_t *ui, char *msg) {
  display_status_message(ui, msg , COL_SYS);
}
void reset_borders(ui_t *ui) {
  wborder(ui->screen, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wborder(ui->log, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
}
void show_chat(ui_t *ui) {
  reset_borders(ui);
  wresize(ui->screen, LINES - 6, COLS - 1);
  box(ui->screen, 0, 0);
  hide_panel(LOG);
  top_panel(CHAT);
  update_panels();
  doupdate();
}
void show_log(ui_t *ui) {
  reset_borders(ui);
  wresize(ui->log, LINES - 6, COLS - 1);
  mvwin(ui->log, 1, 1);
  box(ui->log, 0, 0);
  hide_panel(CHAT);
  top_panel(LOG);
  update_panels();
  doupdate();
}
void show_split(ui_t *ui) {
  reset_borders(ui);
  wresize(ui->screen, LINES - 6, COLS/2 - 1);
  box(ui->screen, 0, 0);
  show_panel(CHAT);
  wresize(ui->log, LINES - 6, COLS/2);
  mvwin(ui->log, 1, COLS/2);
  box(ui->log, 0, 0);
  show_panel(LOG);
  update_panels();
  doupdate();
}

