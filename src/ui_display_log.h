/*
 * =====================================================================================
 *
 *       Filename:  ui_display_log.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  21/07/16 13:20:24
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __UI_DISPLAY_LOG_H__
#define __UI_DISPLAY_LOG_H__

#include "ui.h"

// @func ui_display_log_chunk(ui_t *ui, char *chunk, char *leftover)
//
// @brief Function takes a chunk of log as a string and outputs it
//        to the log panel line by line.
//
// @param ui - UI context defined in "ui.h"
// @param chunk - the log chunk to be displayed
// @param leftover - leftover log string from the previous chunk - see @return
//
// @return Returns NULL if the last character in the chunk is '\n'. Otherwise,
//         returns the string that contains the portion of the chunk from the
//         last '\n' until the end. This chunk should be passed unaltered to
//         the next call as 'leftover' parameter.
char *ui_display_log_chunk(ui_t *ui, char *chunk, char *leftover);

#endif // __UI_DISPLAY_LOG_H__
