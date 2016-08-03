/*
 * =====================================================================================
 *
 *       Filename:  ui_tab.h
 *
 *    Description:  Tab UI abstraction 
 *
 *        Version:  1.0
 *        Created:  03/08/16 11:45:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  draganglumac 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __UI_TAB_H__
#define __UI_TAB_H__

#include <ncurses.h>

typedef struct {
  WINDOW *tab_ctrl;
  WINDOW *window;
  char *name;
} ui_tab;

ui_tab *ui_tab_create(char *name);
void ui_tab_destroy(ui_tab **tab);
void ui_tab_set_name(ui_tab *tab, char *name);

#endif //__UI_TAB_H__
