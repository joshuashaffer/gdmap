/* Copyright (C) 2005 sgop@users.sourceforge.net This is free software
 * distributed under the terms of the GNU Public License.  See the
 * file COPYING for details.
 */
/* $Revision: 1.2 $
 * $Date: 2008/05/23 14:54:28 $
 * $Author: sgop $
 */

#include <gtk/gtk.h>

#define EVENT_FMT  "/%s"
#define EVENT_HFMT "/<u>%s</u>"

#define MARGIN_INDENT 20

/** Create a section with a bold header and a content box, which is
 * indented by 20 pixels. This function can be used by callbacks that
 * are passed to ui_pref_register_leaf()
 * @param header the header text
 * @param help some help text, or NULL. If there is a help text, a
 * small help icon will appear next to the header. Moving the mouse
 * over it will create a tooltip containing the help text.
 * @param[out] hbox the resulting hbox for the content under the
 * header. This box already contains an invisible item of width 20.
 * @param[out] header_hbox the resulting hbox of the header which can be
 * used to add additional stuff there, or NULL
 * @return the GtkVBox containing all the things
 */
GtkWidget* ui_create_section(const char* header, const char* help,
                             GtkWidget** hbox, GtkWidget** header_hbox) {
  GtkWidget* box;
  GtkWidget* label;
  GtkWidget* hbox2;
  char* temp;

  box = gtk_vbox_new(FALSE, 5);
  
  hbox2 = gtk_hbox_new(FALSE, 5);
  gtk_widget_show(hbox2);
  gtk_box_pack_start(GTK_BOX(box), hbox2, FALSE, FALSE, 0);
  
  temp = g_strdup_printf("<b>%s</b>", header);
  label = gtk_label_new(temp);
  g_free(temp);
  gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
  gtk_widget_show(label);
  gtk_box_pack_start(GTK_BOX(hbox2), label, FALSE, FALSE, 0);
  if (help) {
    GtkWidget* ebox = gtk_event_box_new();
    GtkWidget* pixmap =
      gtk_image_new_from_stock
      (GTK_STOCK_DIALOG_INFO/*GTK_STOCK_HELP*/, GTK_ICON_SIZE_MENU);
    gtk_widget_set_events(ebox,
			  GDK_ENTER_NOTIFY_MASK |
			  GDK_LEAVE_NOTIFY_MASK);
    gtk_widget_show(ebox);
    gtk_widget_show(pixmap);
    gtk_container_add(GTK_CONTAINER(ebox), pixmap);
    gtk_box_pack_start(GTK_BOX(hbox2), ebox, FALSE, FALSE, 0);
#if GTK_CHECK_VERSION(2,12,0)
    gtk_widget_set_tooltip_text(ebox, help);
#else
    static GtkTooltips* tips = NULL;
    if (!tips) tips = gtk_tooltips_new();
    gtk_tooltips_set_tip(tips, ebox, help, NULL);
#endif
  }
  if (header_hbox) *header_hbox = hbox2;

  *hbox = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(*hbox);
  gtk_box_pack_start(GTK_BOX(box), *hbox, TRUE, TRUE, 0);
  label = gtk_label_new("");
  gtk_widget_show(label);
  gtk_widget_set_size_request(label, MARGIN_INDENT, -1);
  gtk_box_pack_start(GTK_BOX(*hbox), label, FALSE, FALSE, 0);

  return box;
}

static gboolean
on_ebox_enter(GtkWidget* box, GdkEventCrossing* event, GtkLabel* label) {
  static GdkCursor* Cursor = NULL;
  const char* text;

  (void)event;
  if (gtk_widget_get_sensitive(label)) {
    char* temp;

    text = g_object_get_data(G_OBJECT(label), "label");
    temp = g_strdup_printf(EVENT_HFMT, text);
    gtk_label_set_markup(label, temp);
    g_free(temp);
    gtk_widget_set_state(GTK_WIDGET(label), GTK_STATE_PRELIGHT);

    if (!Cursor) Cursor = gdk_cursor_new(GDK_HAND2);
    gdk_window_set_cursor(box->window, Cursor);
  }
  return TRUE;
}
static gboolean
on_ebox_leave(GtkWidget* box, GdkEventCrossing* event, GtkLabel* label) {
  const char* text;
  char* temp;

  (void)event;
  //  if (GTK_WIDGET_HAS_FOCUS(gtk_widget_get_parent(GTK_WIDGET(label)))) return TRUE;
/*   if (GTK_WIDGET_SENSITIVE(label)) { */
    text = g_object_get_data(G_OBJECT(label), "label");
    temp = g_strdup_printf(EVENT_FMT, text);
    gtk_label_set_markup(label, temp);
    g_free(temp);
    gtk_widget_set_state(GTK_WIDGET(label), GTK_STATE_NORMAL);
    gdk_window_set_cursor(box->window, NULL);
/*   } */
  return TRUE;
}

static void on_clear_label(GtkWidget* widget) {
  char* label = g_object_get_data(G_OBJECT(widget), "label");
  g_free(label);
}

/** Set or unset a event label sensitive.
 * @param ebox the event label
 * @param set TRUE to set sensitive, FALSE to disable
 */
void ui_event_label_set_sensitive(GtkWidget* ebox, gboolean set) {
  gpointer callback = g_object_get_data(G_OBJECT(ebox), "callback");
  GtkWidget* child = GTK_BIN(ebox)->child;
  if (gtk_widget_get_sensitive(child) == set) return;

  if (set && !callback) return;
  gtk_widget_set_sensitive(child, set);

  if (!set) {
    if (!callback) return;
    g_signal_handlers_block_matched
      (ebox, G_SIGNAL_MATCH_FUNC, 0, 0, NULL, callback, NULL);
  } else {
    g_signal_handlers_unblock_matched
      (ebox, G_SIGNAL_MATCH_FUNC, 0, 0, NULL, callback, NULL);
  }
}
/** Create a new event label. An event label is similar to a html link
 * and can be clicked to do some action.
 * @param text the text of the label
 * @param callback the function to be called when clicking the label
 * @param user_data the data passed to the callback function
 * @param swapped TRUE to call the GCallback with swapped arguments
 * @return the label
 */
GtkWidget* ui_create_event_label(const char* text, GCallback callback,
                                 gpointer user_data, gboolean swapped) {
  GtkWidget* ebox;
  GtkWidget* label;
  char* temp;

  ebox = gtk_event_box_new();
  gtk_widget_set_events(ebox,
                        GDK_BUTTON_PRESS_MASK |
                        GDK_KEY_PRESS_MASK |
                        // GDK_FOCUS_CHANGE_MASK |
                        GDK_ENTER_NOTIFY_MASK |
                        GDK_LEAVE_NOTIFY_MASK);
  gtk_widget_set_can_focus(ebox, TRUE);

  gtk_widget_show(ebox);

  temp = g_strdup_printf(EVENT_FMT, text);
  label = gtk_label_new(temp);
  g_free(temp);

  gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
  temp = g_strdup(text);
  g_object_set_data(G_OBJECT(label), "label", temp);
  g_signal_connect(G_OBJECT(label), "destroy",
                   G_CALLBACK(on_clear_label), NULL);

  g_signal_connect(G_OBJECT(ebox), "enter-notify-event",
                   G_CALLBACK(on_ebox_enter), label);
  g_signal_connect(G_OBJECT(ebox), "leave-notify-event",
                   G_CALLBACK(on_ebox_leave), label);
  
  gtk_widget_set_name(label, "EventLabel");
  gtk_widget_show(label);
  gtk_container_add(GTK_CONTAINER(ebox), label);

  if (callback) {
    g_object_set_data(G_OBJECT(ebox), "callback", callback);
    if (swapped) {
      g_signal_connect_swapped(G_OBJECT(ebox), "button-press-event",
			       callback, user_data);
    } else {
      g_signal_connect(G_OBJECT(ebox), "button-press-event",
		       callback, user_data);
    }
  } else {
    ui_event_label_set_sensitive(ebox, FALSE);
  }
  return ebox;
}

static gboolean on_entry_focus_in(GtkEntry* entry) {
  gint active = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(entry), "active"));

  if (active) gtk_entry_set_text(entry, "");
  return FALSE;
}

static gboolean on_entry_focus_out(GtkEntry* entry) {
  const char* text = gtk_entry_get_text(entry);
  
  if (text && *text) {
    g_object_set_data(G_OBJECT(entry), "active", GINT_TO_POINTER(0));
  } else {
    const char* default_text = g_object_get_data(G_OBJECT(entry), "default");
    gtk_entry_set_text(entry, default_text);
    g_object_set_data(G_OBJECT(entry), "active", GINT_TO_POINTER(1));
  }
  return FALSE;
}

/** Create an entry that shows a default text when no text was entered
 * and the entry is not focused.
 * @param text the initial text of the entry
 * @param default_text the default text
 * @return a new GtkEntry
 */ 
GtkWidget* ui_default_entry_new(const char* text, const char* default_text) {
  GtkWidget* entry = gtk_entry_new();
  char* dup = g_strdup_printf("<%s>", default_text);

  if (text && *text) {
    gtk_entry_set_text(GTK_ENTRY(entry), text);
    g_object_set_data(G_OBJECT(entry), "active", GINT_TO_POINTER(0));
  } else {
    gtk_entry_set_text(GTK_ENTRY(entry), dup);
    g_object_set_data(G_OBJECT(entry), "active", GINT_TO_POINTER(1));
  }
  g_object_set_data_full(G_OBJECT(entry), "default", dup, (GDestroyNotify)g_free);

  g_signal_connect(G_OBJECT(entry), "focus-in-event",
                   G_CALLBACK(on_entry_focus_in), NULL);
  g_signal_connect(G_OBJECT(entry), "focus-out-event",
                   G_CALLBACK(on_entry_focus_out), NULL);
  
  return entry;
}

/** Get the text of the entry. Never call on widgets that have focus.
 * @param entry the entry.
 * @return the text of the entry.
 */
const char* ui_default_entry_get_text(GtkEntry* entry) {
  gint active = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(entry), "active"));
  if (active) return "";
  else return gtk_entry_get_text(entry);
}
