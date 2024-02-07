/* Copyright (C) 2005 sgop@users.sourceforge.net This is free software
 * distributed under the terms of the GNU Public License.  See the
 * file COPYING for details.
 */
/* $Revision: 1.4 $
 * $Date: 2005/10/16 14:36:31 $
 * $Author: sgop $
 */

#ifndef _GUI_MAIN_H_
#define _GUI_MAIN_H_

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "tree.h"

//static const char* Folder = NULL;
extern const char* Folder;
extern const char* AppDisplayName;

static void gui_set_window_title(const char* title);
static void gui_update_window_title(void);
GtkWidget* gui_create_main_win (void);
GtkWidget* gui_get_main_win();
void gui_tree_load_and_display(const char* folder);

#endif
