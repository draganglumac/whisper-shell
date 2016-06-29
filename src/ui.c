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

#define COL_LOGO   1
#define COL_LOCAL  2
#define COL_REMOTE 3
#define COL_SYS    4
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
  printw("%s", " Whisper Chat ");
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
  wrefresh(ui->screen);

  ui->prompt = newwin(4, COLS - 1, LINES - 5, 1);
  show_prompt(ui);

  return ui;
}
void destroy_ui(ui_t *ui) {
  delwin(ui->screen);
  delwin(ui->prompt);
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
void display_message(ui_t *ui, char *msg, int col_flag) {
  int row, col;
  getyx(ui->prompt, row, col);
  wattron(ui->screen, COLOR_PAIR(col_flag));
  mvwprintw(ui->screen, ui->next_line, 1, "%s\n", msg);
  wattroff(ui->screen, COLOR_PAIR(col_flag));
  update_next_line(ui);
  box(ui->screen, 0, 0);
  wrefresh(ui->screen);
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
  display_message(ui, msg , COL_SYS);
}
