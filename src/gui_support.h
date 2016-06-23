/* Copyright (C) 2005 sgop@users.sourceforge.net This is free software
 * distributed under the terms of the GNU Public License.  See the
 * file COPYING for details.
 */
/* $Revision: 1.1 $
 * $Date: 2005/12/27 14:29:51 $
 * $Author: sgop $
 */

GtkWidget* ui_create_section(const char* header, const char* help,
                             GtkWidget** hbox, GtkWidget** header_hbox);
void ui_event_label_set_sensitive(GtkWidget* ebox, gboolean set);
GtkWidget* ui_create_event_label(const char* text, GCallback callback,
                                 gpointer user_data, gboolean swapped);

GtkWidget* ui_default_entry_new(const char* text, const char* default_text);
const char* ui_default_entry_get_text(GtkEntry* entry);
