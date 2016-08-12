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
#include <string.h>
#include <sys/time.h>

#include "ui.h"
#include "ui_history.h"

#define MAX_BUFF 1024

#define COL_LOGO   1
#define COL_LOCAL  2
#define COL_REMOTE 3
#define COL_SYS    4

#define CHAT  ui->panels[0]
#define LOG   ui->panels[1]
#define ALERT ui->panels[2]

static char system_buffer[1024];
static char *pb = system_buffer;

static ui_history *chat_history;
static ui_history *log_history;

#define ACTIVE_CHAT 0
#define ACTIVE_LOG  1
static int chat_tab;
static int log_tab;

static void init_colours() {
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
static void show_prompt(ui_t *ui, char *msg) {
  wmove(ui->prompt, 1, 1);
  wclear(ui->prompt);
  if (msg != NULL)
    mvwprintw(ui->prompt, 1, 1, "$> %s", msg);
  else
    mvwprintw(ui->prompt, 1, 1, "$> ");
  wrefresh(ui->prompt);
}
static void display_logo() {
  attron(COLOR_PAIR(COL_LOGO) | A_BOLD);
  printw("%s", " Whisper Shell ");
  attroff(COLOR_PAIR(COL_LOGO) | A_BOLD);
}
static void display_tabs(int active) {
  if (ACTIVE_CHAT == active) {
    attron(A_REVERSE|A_BOLD);
    mvprintw(0, chat_tab, " Chat ");
    attroff(A_REVERSE|A_BOLD);

    attron(A_BOLD);
    mvprintw(0, log_tab, " Log ");
    attroff(A_BOLD);
  }
  else if (ACTIVE_LOG == active) {
    attron(A_REVERSE|A_BOLD);
    mvprintw(0, log_tab, " Log ");
    attroff(A_REVERSE|A_BOLD);

    attron(A_BOLD);
    mvprintw(0, chat_tab, " Chat ");
    attroff(A_BOLD);
  }
}
static void render_tab_bar(int active) {
  move(0,0);
  deleteln();
  display_logo();
  display_tabs(active);
  refresh();
}
static void render_ui(ui_t *ui) {
  chat_tab = COLS-12;
  log_tab = COLS-6;
  render_tab_bar(ACTIVE_CHAT);

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

  ui->alert = newwin(3, COLS, 1, 0);
  ALERT = new_panel(ui->alert);

  set_panel_userptr(CHAT, LOG);
  set_panel_userptr(LOG, ALERT);
  set_panel_userptr(ALERT, CHAT);

  top_panel(CHAT);
  hide_panel(LOG);
  hide_panel(ALERT);
  update_panels();
  doupdate();

  ui->prompt = newwin(4, COLS - 1, LINES - 5, 1);
  show_prompt(ui, NULL);
}
ui_t *create_ui() {
  ui_t *ui = malloc(sizeof(ui_t));

  initscr();
  init_colours();
  noecho();

  /* Get all the mouse events */
  mousemask(ALL_MOUSE_EVENTS, NULL);
  mouseinterval(1);
  keypad(stdscr, TRUE);

  render_ui(ui);
  return ui;
}
void destroy_ui(ui_t *ui) {
  // chat panel and window
  del_panel(CHAT);
  delwin(ui->screen);
  ui_history_destroy(&chat_history);
  // log panel and window
  del_panel(LOG);
  delwin(ui->log);
  ui_history_destroy(&log_history);
  // alert panel and window
  del_panel(ALERT);
  delwin(ui->alert);
  // prompt window
  delwin(ui->prompt);

  endwin();
  free(ui);
}
void process_mouse_events(ui_t *ui) {
  MEVENT event;
  if(getmouse(&event) == OK) {
    if (event.bstate & BUTTON1_PRESSED) {
      if (event.y == 0) {
        if (event.x >= chat_tab && event.x < chat_tab+6) // clicked chat tab
          show_chat(ui);
        else if (event.x >= log_tab && event.x < log_tab+5) // clicked log tab
          show_log(ui);
      }
    }
  }
}
char *get_user_input(ui_t *ui) {
  char *msg = malloc(MAX_BUFF);
  int msg_len = 0, c, px, py;

  wmove(ui->prompt, 1, 4);
  wrefresh(ui->prompt);
  while (c = getch()) 
    switch (c) {
      case KEY_MOUSE:
        mvwinnstr(ui->prompt, 1, 4, msg, msg_len);
        process_mouse_events(ui);
        show_prompt(ui, msg);
        break;

      case KEY_BACKSPACE:
        if (msg_len > 0) {
          getyx(ui->prompt, py, px);
          if (px >= 4) {
            mvwdelch(ui->prompt, py, px-1);
            msg_len--;
            wrefresh(ui->prompt);
          }
        }
        break;

      case '\n':
        mvwinnstr(ui->prompt, 1, 4, msg, msg_len);
        show_prompt(ui, NULL);
        return msg;  

      default:
        getyx(ui->prompt, py, px);
        mvwaddch(ui->prompt, py, px, c);
        msg_len++;
        wrefresh(ui->prompt);
        break;
    }
}
static void update_next_line(ui_t *ui) {
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
static void update_next_log_line(ui_t *ui) {
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
static void display_message(ui_t *ui, char *msg, int col_flag) {
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
static void display_status_message(ui_t *ui, char *msg, int col_flag) {
  int row, col;
  getyx(ui->prompt, row, col);
  wattron(ui->log, COLOR_PAIR(col_flag));
  mvwprintw(ui->log, ui->next_log_line, 1, "%s\n", msg);
  update_next_log_line(ui);
  wattroff(ui->log, COLOR_PAIR(col_flag));
  box(ui->log, 0, 0);
  update_panels();
  doupdate();
  wmove(ui->prompt, row, col);
  wrefresh(ui->prompt);
}
void display_local_message(ui_t *ui, char *msg) {
  ui_history_add(chat_history, msg, MSG_LOCAL);
  display_message(ui, msg, COL_LOCAL);
}
void display_remote_message(ui_t *ui, char *msg) {
  ui_history_add(chat_history, msg, MSG_REMOTE);
  display_message(ui, msg, COL_REMOTE);
}
void display_system_message(ui_t *ui, char *msg) {
  ui_history_add(log_history, msg, MSG_SYSTEM);
  display_status_message(ui, msg , COL_SYS);
}
void show_hist_item(WINDOW *win, hist_item *item, int row) {
  int col_flag;
  if (item->type == MSG_LOCAL)
    col_flag = COL_LOCAL;
  else if (item->type == MSG_REMOTE)
    col_flag = COL_REMOTE;
  else if (item->type == MSG_SYSTEM)
    col_flag = COL_SYS;

  wattron(win, COLOR_PAIR(col_flag));
  mvwprintw(win, row, 1, "%s\n", item->message);
  wattroff(win, COLOR_PAIR(col_flag));
}
void restore_history(WINDOW *win, ui_history *h, int *pnext_line) {
  if (h->end == -1)
    return;
  int num_lines = LINES - 6;
  if (h->end > h->start && h->end < num_lines)
    num_lines = h->end;
  *pnext_line = 1;
  int start = h->end - num_lines;
  if (start < 0)
    start += MAX_HISTORY;
  jnx_list *hitems = ui_history_get(h, start, num_lines);
  void *first = NULL;
  while(first = jnx_list_remove_front(&hitems)) {
    hist_item *hitem = (hist_item*)first;
    show_hist_item(win, hitem, *pnext_line);
    (*pnext_line)++;
  }
}
void reset_borders(ui_t *ui) {
  wborder(ui->screen, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wborder(ui->log, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
}
void show_chat(ui_t *ui) {
  chat_tab = COLS-12;
  log_tab = COLS-6;
  render_tab_bar(ACTIVE_CHAT);

  hide_panel(ALERT);
  reset_borders(ui);
  wclear(ui->screen);
  wresize(ui->screen, LINES - 6, COLS - 1);
  restore_history(ui->screen, chat_history, &ui->next_line);
  box(ui->screen, 0, 0);
  hide_panel(LOG);
  top_panel(CHAT);
  update_panels();
  doupdate();
}
void show_log(ui_t *ui) {
  chat_tab = COLS-12;
  log_tab = COLS-6;
  render_tab_bar(ACTIVE_LOG);

  hide_panel(ALERT);
  reset_borders(ui);
  wclear(ui->log);
  wresize(ui->log, LINES - 6, COLS - 1);
  mvwin(ui->log, 1, 1);
  restore_history(ui->log, log_history, &ui->next_log_line);
  box(ui->log, 0, 0);
  hide_panel(CHAT);
  top_panel(LOG);
  update_panels();
  doupdate();
}
void show_split(ui_t *ui) {
  chat_tab = (COLS/2)-7;
  log_tab = COLS-6;
  render_tab_bar(ACTIVE_CHAT);

  hide_panel(ALERT);
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
  show_prompt(ui, NULL);
}
void show_alert(ui_t *ui, char *message) {
  int cols = COLS;
  int x = (COLS - strlen(message)) / 2;
  wattron(ui->alert, COLOR_PAIR(COL_LOGO) | A_BOLD);
  mvwprintw(ui->alert, 0, 0, "%*s", COLS," ");
  mvwprintw(ui->alert, 1, 0, "%*s", COLS," ");
  mvwprintw(ui->alert, 2, 0, "%*s", COLS," ");
  mvwprintw(ui->alert, 1, x, message);
  wattroff(ui->alert, COLOR_PAIR(COL_LOGO) | A_BOLD);
  show_panel(ALERT);
  top_panel(ALERT);
  update_panels();
  doupdate();
}
void hide_alert(ui_t *ui) {
  hide_panel(ALERT);
}
