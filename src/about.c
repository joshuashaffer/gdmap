/*
 * Copyright (C) 2005-2006 sgop@users.sourceforge.net
 * Copyright (C) 2024 Raphael Rosch <gnome-dev@insaner.com>
 * 
 * This is free software
 * distributed under the terms of the GNU Public License.  See the
 * file COPYING for details.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <gtk/gtk.h>
#include "l_i18n.h"

static GtkAboutDialog* AboutWin = NULL;

static GdkPixbuf* create_pixbuf(const gchar *filename)
{
    char* temp;
    GdkPixbuf* pix;
    
#ifdef PACKAGE_SOURCE_DIR
    temp = g_strdup_printf("%s/data/%s", PACKAGE_SOURCE_DIR, filename);
#else
    temp = g_strdup_printf("/usr/share/pixmaps/%s", filename);
#endif
    pix = gdk_pixbuf_new_from_file(temp, NULL);
    g_free(temp);

    return pix;
}

void gui_show_about(GtkWindow* parent)
{
    GtkWidget* win;
    const char* authors[] = {
        "Markus Lausser <sgop@users.sourceforge.net>",
        "Raphael Rosch <gnome-dev@insaner.com>",
        NULL
    };
    
    if (AboutWin)
    {
        gtk_window_present(GTK_WINDOW(AboutWin));
        return;
    }
    win = gtk_about_dialog_new();
    AboutWin = GTK_ABOUT_DIALOG(win);
#if GTK_CHECK_VERSION(2,12,0)
    gtk_about_dialog_set_program_name(AboutWin, PACKAGE_NAME);
#else
    gtk_about_dialog_set_name(AboutWin, PACKAGE_NAME);
#endif
    gtk_about_dialog_set_version(AboutWin, PACKAGE_VERSION);
    gtk_about_dialog_set_copyright(AboutWin, "Copyright \302\251 2005-2008 Markus Lausser");
    gtk_about_dialog_set_license(AboutWin, "GPL (GNU General Public License)\n\nsee http://www.gnu.org/licenses/gpl.html");
    gtk_about_dialog_set_website(AboutWin, "http://gdmap.sourceforge.net");
    gtk_about_dialog_set_authors(AboutWin, authors);
    gtk_about_dialog_set_comments(AboutWin, _("GdMap is a tool which allows you to graphically explore your disks."));
    gtk_about_dialog_set_logo(AboutWin, create_pixbuf("gdmap_icon.png"));
    gtk_about_dialog_set_translator_credits(AboutWin, _("translator-credits"));
    
    g_signal_connect_swapped(win, "response", G_CALLBACK(gtk_widget_destroy), win);
    
    g_object_add_weak_pointer(G_OBJECT(AboutWin), (void*)&AboutWin);
    
    if (parent)
    {
        gtk_window_set_transient_for(GTK_WINDOW(win), parent);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(win), TRUE);
    }
    
    gtk_widget_show(win);
}

