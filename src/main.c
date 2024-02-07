/*
 * Copyright (C) 2005-2006 sgop@users.sourceforge.net
 * Copyright (C) 2024 Raphael Rosch <gnome-dev@insaner.com>
 * 
 * This is free software distributed under the terms of the
 * GNU Public License.  See the file COPYING for details.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <gtk/gtk.h>

#include "l_i18n.h"
#include "gui_main.h"
#include "colors.h"
#include "preferences.h"
#include "../config.h"

#define NL "\n"

const char* AppDisplayName = "Graphical Disk Map";
const char* Folder = NULL;

static const char* GtkResource =
  "style \"event\" {"NL
  "  fg[NORMAL] = { 0, 0.5, 1.0 }"NL
  "  fg[PRELIGHT] = { 0.8, 0.6, 0.4 }"NL
  "}"NL
  ""NL
  "widget \"*EventLabel*\" style \"event\""NL;

static GOptionEntry Options[] = {
  { "folder", 'f', 0, G_OPTION_ARG_STRING, &Folder, "Inspect folder", "dir" },
/*   { "mount", 'm', 0, G_OPTION_ARG_NONE, &AllowLeaveDevice, "Descend directories on other filesystems", NULL }, */
/*   { "reportedsize", 'r', 0, G_OPTION_ARG_NONE, &ReportedSize, "Don't show disk size of files but reported size", NULL }, */
  { NULL, 0, 0, 0, NULL, NULL, NULL }
};

static gboolean on_load() {
  if (Folder) gui_tree_load_and_display(Folder);
  return FALSE;
}

int main (int argc, char *argv[]) {
  GOptionContext* context = g_option_context_new("- Graphical Disk Map");
  GError* error = NULL;

	bindtextdomain(GETTEXT_PACKAGE, GDMAP_LOCALE_DIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);

  g_option_context_set_ignore_unknown_options(context, FALSE);
  g_option_context_set_help_enabled(context, TRUE);
  g_option_context_add_main_entries(context, Options, NULL);
  if (!g_option_context_parse(context, &argc, &argv, &error)) {
    g_warning("%s", error->message);
    g_option_context_free(context);
    return 0;
  }
  g_option_context_free(context);
  
  gtk_init(&argc, &argv);
  
  gtk_rc_parse_string(GtkResource);
  
  gui_create_main_win();

  colors_init();
  pref_init();

  g_idle_add((GSourceFunc)on_load, NULL);
  
  gtk_main();

  return 0;
}

