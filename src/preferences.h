/* Copyright (C) 2005 sgop@users.sourceforge.net This is free software
 * distributed under the terms of the GNU Public License.  See the
 * file COPYING for details.
 */
/* $Revision: 1.4 $
 * $Date: 2008/05/23 14:54:28 $
 * $Author: sgop $
 */

#ifndef _GUI_PREFERENCES_H_
#define _GUI_PREFERENCES_H_

#define DISPLAY_STANDARD_CUSHION    0
#define DISPLAY_SQUARE_CUSHION      1

typedef void (*Func_p)(void);

/* extern gboolean LeaveDevice; */
/* extern gboolean UseReportedSize; */

void pref_set_window_title_callback(Func_p func);
void pref_set_redraw_callback(Func_p func);
void pref_init();

unsigned pref_get_display_mode(void);
double pref_get_h(void);
double pref_get_f(void);
unsigned pref_get_max_depth(void);
gboolean pref_get_use_colors(void);
gboolean pref_get_use_average(void);
gboolean pref_get_leave_device(void);
gboolean pref_get_use_reported_size(void);
gboolean pref_get_show_path_in_title(void);
gboolean pref_get_show_version_in_title(void);

void gui_show_preferences(GtkWindow* parent);


#endif
