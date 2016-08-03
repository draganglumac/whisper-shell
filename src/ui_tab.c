/*
 * =====================================================================================
 *
 *       Filename:  ui_tab.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/08/16 11:54:27
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
#include <jnxc_headers/jnx_log.h>
#include "ui_tab.h"

static void initialise(ui_tab *tab, char *name) {
  // ToDo
}
ui_tab *ui_tab_create(char *name) {
  if (!name) {
    JNX_LOG(LERROR, "You must supply a name for ui_tab, it cannot be NULL");
    exit(1);    
  }
  ui_tab *tab = calloc(1, sizeof(ui_tab));
  initialise(tab, name);
  return tab; 
}
void ui_tab_destroy(ui_tab **tab);
void ui_tab_set_name(ui_tab *tab, char *name);


