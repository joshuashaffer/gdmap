/* Copyright (C) 2005 sgop@users.sourceforge.net This is free software
 * distributed under the terms of the GNU Public License.  See the
 * file COPYING for details.
 */
/* $Revision: 1.6 $
 * $Date: 2008/05/26 20:33:11 $
 * $Author: sgop $
 */

#include <stdlib.h>
#include <gtk/gtk.h>
#include "l_i18n.h"
#include "preferences.h"
#include "utils.h"
#include "colors.h"
#include "gui_support.h"

static GtkWidget* PrefWin = NULL;
static RedrawFunc RedrawCallback = NULL;

static unsigned DisplayMode = DISPLAY_SQUARE_CUSHION;
static double HValue = 0.5;
static double FValue = 0.8;
static unsigned MaxDepth = 0;
static gboolean UseColors = TRUE;
static gboolean UseAverage = FALSE;
static gboolean LeaveDevice = FALSE;
static gboolean UseReportedSize = TRUE;

static GtkWidget* AddLabel = NULL;
static GtkBox* ColorBox = NULL;
static GList* Boxes = NULL;


static void pref_save()
{
    GKeyFile* file = g_key_file_new();
    
    g_key_file_set_integer(file, "Display", "Mode", DisplayMode);
    g_key_file_set_integer(file, "Display", "MaxDepth", MaxDepth);
    g_key_file_set_integer(file, "Display", "HValue", (int)(HValue*100));
    g_key_file_set_integer(file, "Display", "FValue", (int)(FValue*100));
    g_key_file_set_boolean(file, "Display", "UseColors", UseColors);
    g_key_file_set_boolean(file, "Display", "UseAverage", UseAverage);
    g_key_file_set_boolean(file, "Scan", "UseReportedSize", UseReportedSize);
    g_key_file_set_boolean(file, "Scan", "LeaveDevice", LeaveDevice);
    
    {
        const char* home = g_get_home_dir();
        char* filename = g_strdup_printf("%s/.gdmap/settings.ini", home);
        gsize len;
        char* temp = g_key_file_to_data(file, &len, NULL);
        
        if (temp)
        {
            FILE* fd;
            create_dir(filename);
            fd = fopen(filename, "w");
            if (fd)
            {
                fwrite(temp, len, 1, fd);
                fclose(fd);
            }
        }
        g_free(temp);
        g_free(filename);
    }
    
    g_key_file_free(file);
}


void pref_set_redraw_callback(RedrawFunc func)
{
    RedrawCallback = func;
}

unsigned pref_get_display_mode(void)
{
    return DisplayMode;
}

double pref_get_h(void)
{
    return HValue;
}

double pref_get_f(void)
{
    return FValue;
}

unsigned pref_get_max_depth(void)
{
    return MaxDepth;
}

gboolean pref_get_use_colors(void)
{
    return UseColors;
}

gboolean pref_get_use_average(void)
{
    return UseAverage;
}

gboolean pref_get_leave_device(void)
{
    return LeaveDevice;
}

gboolean pref_get_use_reported_size(void)
{
    return UseReportedSize;
}



static void pref_set_display_mode(unsigned mode)
{
    if (mode == DISPLAY_STANDARD_CUSHION || mode == DISPLAY_SQUARE_CUSHION)
    {
        DisplayMode = mode;
        if (RedrawCallback) RedrawCallback();
        pref_save();
    }
}

static void pref_set_h(double value)
{
    HValue = value;
    if (RedrawCallback) RedrawCallback();
    pref_save();
}

static void pref_set_f(double value)
{
    FValue = value;
    if (RedrawCallback) RedrawCallback();
    pref_save();
}

static void pref_set_max_depth(unsigned value)
{
    MaxDepth = value;
    if (RedrawCallback) RedrawCallback();
    pref_save();
}

static void pref_set_use_colors(gboolean value)
{
    UseColors = value;
    if (RedrawCallback) RedrawCallback();
    pref_save();
}

static void pref_set_use_average(gboolean value)
{
    UseAverage = value;
    if (RedrawCallback) RedrawCallback();
    pref_save();
}

static void pref_set_leave_device(gboolean value)
{
    LeaveDevice = value;
    pref_save();
}

static void pref_set_use_reported_size(gboolean value)
{
    UseReportedSize = value;
    pref_save();
}


static void on_display_mode(GtkToggleButton* button, void* data)
{
    unsigned mode = GPOINTER_TO_INT(data);
    if (gtk_toggle_button_get_active(button))
        pref_set_display_mode(mode);
}

static void on_h_changed(GtkSpinButton* button)
{
    pref_set_h(gtk_spin_button_get_value(button));
}

static void on_f_changed(GtkSpinButton* button)
{
    pref_set_f(gtk_spin_button_get_value(button));
}

static void on_depth_changed(GtkSpinButton* button, GtkWidget* other)
{
    pref_set_max_depth(gtk_spin_button_get_value_as_int(button));
    gtk_widget_set_sensitive(other, pref_get_use_colors() && pref_get_max_depth() != 0);
}

static void on_use_colors(GtkToggleButton* button, GtkWidget* other)
{
    pref_set_use_colors(gtk_toggle_button_get_active(button));
    gtk_widget_set_sensitive(other, pref_get_use_colors() && pref_get_max_depth() != 0);
}

static void on_use_average(GtkToggleButton* button)
{
    pref_set_use_average(gtk_toggle_button_get_active(button));
}

static void on_leave_device(GtkToggleButton* button)
{
    pref_set_leave_device(gtk_toggle_button_get_active(button));
}

static void on_use_reported(GtkToggleButton* button)
{
    pref_set_use_reported_size(!gtk_toggle_button_get_active(button));
}

static void on_box_destroyed(GtkWidget* box)
{
    Boxes = g_list_remove(Boxes, box);
}

static void on_add_cancel(GtkWidget* hbox)
{
    gtk_widget_hide(hbox);
    ui_event_label_set_sensitive(AddLabel, TRUE);
}

static void on_add_new(GtkWidget* hbox)
{
    gtk_widget_show(hbox);
    ui_event_label_set_sensitive(AddLabel, FALSE);
}


static GtkWidget* _gui_create_color_box(color_t* color);
static void on_add_ok(GtkWidget* hbox) {
  GtkEntry* entry = g_object_get_data(G_OBJECT(hbox), "entry");
  GtkColorButton* button = g_object_get_data(G_OBJECT(hbox), "button");
  color_t color;
  const char* text = ui_default_entry_get_text(entry);
  GtkEntry* entry2;

  if (!text || *text == 0) return;

  color.type = COLOR_EXTENSION;
  color.ext = NULL;
  gtk_color_button_get_color(button, &color.c);
  color.gc = NULL;

  on_add_cancel(hbox);

  hbox = _gui_create_color_box(&color);
  gtk_box_pack_start(ColorBox, hbox, FALSE, FALSE, 0);
  Boxes = g_list_append(Boxes, hbox);
  gtk_widget_show_all(hbox);
  
  entry2 = g_object_get_data(G_OBJECT(hbox), "entry");
  gtk_entry_set_text(entry2, text);

  gtk_entry_set_text(entry, "");
  
}

static GtkWidget* _gui_create_color_box(color_t* color) {
  GtkWidget* hbox;
  GtkWidget* item;
  ColorType type = color?color->type:COLOR_EXTENSION;

  hbox = gtk_hbox_new(FALSE, 5);

  if (color) {
    item = gtk_color_button_new_with_color(&color->c);
  } else {
    item = gtk_color_button_new();
  }
  gtk_box_pack_start(GTK_BOX(hbox), item, FALSE, FALSE, 0);
      
  g_object_set_data(G_OBJECT(hbox), "button", item);
  g_object_set_data(G_OBJECT(hbox), "type", GINT_TO_POINTER(type));

  switch (type) {
  case COLOR_DEFAULT:
    item = gtk_label_new(_("Default color"));
    gtk_misc_set_alignment(GTK_MISC(item), 0, 0.5);
    break;
  case COLOR_MARK1:
    item = gtk_label_new(_("Color for file position"));
    gtk_misc_set_alignment(GTK_MISC(item), 0, 0.5);
    break;
  case COLOR_MARK2:
    item = gtk_label_new(_("Color for folder position"));
    gtk_misc_set_alignment(GTK_MISC(item), 0, 0.5);
    break;
  case COLOR_FOLDER:
    item = gtk_label_new(_("Color for folders"));
    gtk_misc_set_alignment(GTK_MISC(item), 0, 0.5);
    break;
  case COLOR_EXTENSION:
    if (color) {
      char* temp = make_string_from_list(color->ext, " ");
      item = ui_default_entry_new(temp, _("Enter file extensions here"));
      g_free(temp);
    } else {
      item = ui_default_entry_new(NULL, _("Enter file extensions here"));
    }
    g_object_set_data(G_OBJECT(hbox), "entry", item);
    break;
  default:
    item = NULL;
    g_assert(FALSE);
    break;
  }

  gtk_box_pack_start(GTK_BOX(hbox), item, TRUE, TRUE, 0);

  if (!color) {
    item = gtk_button_new_from_stock(GTK_STOCK_ADD);
    gtk_box_pack_start(GTK_BOX(hbox), item, FALSE, FALSE, 0);
    g_signal_connect_swapped(G_OBJECT(item), "clicked",
                             G_CALLBACK(on_add_ok), hbox);

    item = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
    gtk_box_pack_start(GTK_BOX(hbox), item, FALSE, FALSE, 0);
    g_signal_connect_swapped(G_OBJECT(item), "clicked",
                             G_CALLBACK(on_add_cancel), hbox);
    
    //FIXME: add 'add' button
  } else if (color->type == COLOR_EXTENSION) {
    GtkWidget* vbox2;
    GtkRequisition req;

    vbox2 = gtk_vbox_new(FALSE, 0);
    gtk_widget_size_request(item, &req);
    gtk_box_pack_start(GTK_BOX(hbox), vbox2, FALSE, FALSE, 0);
    
    item = gtk_button_new_with_label(_("Delete"));
    gtk_widget_set_size_request(item, -1, req.height);
    g_signal_connect_swapped(G_OBJECT(item), "clicked",
                             G_CALLBACK(gtk_widget_destroy), hbox);
    /*       item = ui_create_event_label */
    /*         (_("delete"), G_CALLBACK(gtk_widget_destroy), hbox, TRUE); */
    
    gtk_box_pack_start(GTK_BOX(vbox2), item, TRUE, FALSE, 0);
    g_signal_connect(G_OBJECT(hbox), "destroy",
                     G_CALLBACK(on_box_destroyed), NULL);
  }
  return hbox;
}

static void on_setup_colors(GtkWindow* parent) {
  GtkWidget* dialog;
  GList* dlist;

  dialog = gtk_dialog_new_with_buttons
      (_("Color profile"), parent, GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT,
       GTK_STOCK_CANCEL, GTK_RESPONSE_NONE,
       GTK_STOCK_OK,     GTK_RESPONSE_OK,
       NULL);
  gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 500);

  {
    GtkWidget* vbox0;
    GtkWidget* hbox0;
    GtkWidget* vbox;
    GtkWidget* vbox1;
    GtkWidget* hbox;
    GtkWidget* nbox;
    GtkWidget* section;
    GtkWidget* item;
    GtkWidget* sw;
    
    vbox0 = gtk_vbox_new(FALSE, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox0), 6);
    
    section = ui_create_section(_("Basic colors"), NULL, &hbox0, NULL);
    gtk_box_pack_start(GTK_BOX(vbox0), section, FALSE, FALSE, 0);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox0), vbox, TRUE, TRUE, 0);

    for (dlist = colors_get_list(); dlist; dlist = dlist->next) {
      color_t* color = dlist->data;
      
      if (color->type == COLOR_EXTENSION) continue;
      
      hbox = _gui_create_color_box(color);
      gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
      Boxes = g_list_append(Boxes, hbox);
    }

    section = ui_create_section(_("File colors"), NULL, &hbox0, &hbox);
    gtk_box_pack_start(GTK_BOX(vbox0), section, TRUE, TRUE, 0);
    
    // vbox which contains hbox for new color and scrolled area for existing colors
    vbox1 = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox0), vbox1, TRUE, TRUE, 0);
    nbox = _gui_create_color_box(NULL);
    gtk_box_pack_start(GTK_BOX(vbox1), nbox, FALSE, FALSE, 6);

    sw = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_shadow_type
      (GTK_SCROLLED_WINDOW(sw), GTK_SHADOW_NONE);
    gtk_scrolled_window_set_policy
      (GTK_SCROLLED_WINDOW(sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox1), sw, TRUE, TRUE, 0);
    
    vbox = gtk_vbox_new(FALSE, 0);
    ColorBox = GTK_BOX(vbox);

    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(sw), vbox);
    gtk_viewport_set_shadow_type
      (GTK_VIEWPORT(gtk_bin_get_child(GTK_BIN(sw))), GTK_SHADOW_NONE);

    item = ui_create_event_label
      (_("add new color"), G_CALLBACK(on_add_new), nbox, TRUE);
    gtk_box_pack_end(GTK_BOX(hbox), item, FALSE, FALSE, 0);
    AddLabel = item;

    for (dlist = colors_get_list(); dlist; dlist = dlist->next) {
      color_t* color = dlist->data;

      if (color->type != COLOR_EXTENSION) continue;

      hbox = _gui_create_color_box(color);
      gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
      Boxes = g_list_append(Boxes, hbox);
    }
   
    gtk_widget_show_all(vbox0);
    gtk_widget_hide(nbox);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), vbox0, TRUE, TRUE, 0);
  }


  switch (gtk_dialog_run(GTK_DIALOG(dialog))) {
  case GTK_RESPONSE_OK:
    colors_clear();
    for (dlist = Boxes; dlist; dlist = dlist->next) {
      GtkWidget* hbox = dlist->data;
      ColorType type = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(hbox), "type"));
      GtkColorButton* button = g_object_get_data(G_OBJECT(hbox), "button");
      GtkEntry* entry = g_object_get_data(G_OBJECT(hbox), "entry");
      GdkColor color;

      gtk_color_button_get_color(button, &color);
      if (type == COLOR_EXTENSION) {
        color_add(color_new_exts(&color, ui_default_entry_get_text(entry)));
      } else {
        color_add(color_new(color.red, color.green, color.blue, type));
      }
    }
    if (RedrawCallback) RedrawCallback();
    break;
  default:
    break;
  }
  g_list_free(Boxes);
  Boxes = NULL;

  gtk_widget_destroy(dialog);
}

void gui_show_preferences(GtkWindow* parent) {
  GtkWidget* win;
  GtkWidget* section;
  GtkWidget* table;
  GtkWidget* hbox;
  GtkWidget* vbox;
  GtkWidget* vbox0;
  GtkWidget* item;
  GtkWidget* item2;
  GSList* Group = NULL;

  if (PrefWin) {
    gtk_window_present(GTK_WINDOW(PrefWin));
    return;
  }

  PrefWin = win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(win), _("Preferences"));
  gtk_window_set_role(GTK_WINDOW(win), "Preferences");
/*   gtk_window_set_default_icon(create_pixbuf("gdmap_icon.png")); */
/*   gtk_window_set_default_size(GTK_WINDOW(win), 700, 450); */

  g_object_add_weak_pointer(G_OBJECT(PrefWin), (void*)&PrefWin);
  
  vbox0 = gtk_vbox_new(FALSE, 12);
  gtk_container_set_border_width(GTK_CONTAINER(vbox0), 12);
  gtk_container_add(GTK_CONTAINER(win), vbox0);

  section = ui_create_section(_("Display type"), NULL, &hbox, NULL);
  gtk_box_pack_start(GTK_BOX(vbox0), section, FALSE, FALSE, 0);
  
  vbox = gtk_vbox_new(FALSE, 2);
  gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);

  item = gtk_radio_button_new_with_label(Group, _("Standard cushioned treemaps"));
  Group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(item));
  gtk_box_pack_start(GTK_BOX(vbox), item, FALSE, FALSE, 0);

  item2 = gtk_radio_button_new_with_label(Group, _("Squarified cushioned treemaps"));
  Group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(item2));
  gtk_box_pack_start(GTK_BOX(vbox), item2, FALSE, FALSE, 0);

  if (DisplayMode == DISPLAY_STANDARD_CUSHION)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(item), TRUE);
  else
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(item2), TRUE);
  
  g_signal_connect(G_OBJECT(item), "toggled",
                   G_CALLBACK(on_display_mode), GINT_TO_POINTER(DISPLAY_STANDARD_CUSHION));
  g_signal_connect(G_OBJECT(item2), "toggled",
                   G_CALLBACK(on_display_mode), GINT_TO_POINTER(DISPLAY_SQUARE_CUSHION));

  section = ui_create_section(_("Cushion settings"), NULL, &hbox, NULL);
  gtk_box_pack_start(GTK_BOX(vbox0), section, FALSE, FALSE, 0);
  
  table = gtk_table_new(2, 2, FALSE);
  gtk_box_pack_start(GTK_BOX(hbox), table, TRUE, TRUE, 0);
  gtk_table_set_col_spacings(GTK_TABLE(table), 10);
  gtk_table_set_row_spacings(GTK_TABLE(table), 2);

  item = gtk_label_new(_("Height factor:"));
  gtk_misc_set_alignment(GTK_MISC(item), 0, 0.5);
  gtk_table_attach(GTK_TABLE(table), item, 0, 1, 0, 1,
                   GTK_FILL, GTK_FILL, 0, 0);

  item = gtk_spin_button_new_with_range(0.0, 1.0, 0.1);
  gtk_table_attach(GTK_TABLE(table), item, 1, 2, 0, 1,
                   GTK_FILL, GTK_FILL, 0, 0);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(item), HValue);
  g_signal_connect(G_OBJECT(item), "value-changed",
                   G_CALLBACK(on_h_changed), NULL);
  
  item = gtk_label_new(_("Decreasing height:"));
  gtk_misc_set_alignment(GTK_MISC(item), 0, 0.5);
  gtk_table_attach(GTK_TABLE(table), item, 0, 1, 1, 2,
                   GTK_FILL, GTK_FILL, 0, 0);

  item = gtk_spin_button_new_with_range(0.0, 1.0, 0.1);
  gtk_table_attach(GTK_TABLE(table), item, 1, 2, 1, 2,
                   GTK_FILL, GTK_FILL, 0, 0);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(item), FValue);
  g_signal_connect(G_OBJECT(item), "value-changed",
                   G_CALLBACK(on_f_changed), NULL);

  
  section = ui_create_section(_("General display"), NULL, &hbox, NULL);
  gtk_box_pack_start(GTK_BOX(vbox0), section, FALSE, FALSE, 0);
  
  table = gtk_table_new(3, 2, FALSE);
  gtk_box_pack_start(GTK_BOX(hbox), table, TRUE, TRUE, 0);
  gtk_table_set_col_spacings(GTK_TABLE(table), 10);
  gtk_table_set_row_spacings(GTK_TABLE(table), 2);

  item = gtk_label_new(_("Maximum depth:"));
  gtk_misc_set_alignment(GTK_MISC(item), 1, 0.5);
  gtk_table_attach(GTK_TABLE(table), item, 0, 1, 0, 1,
                   GTK_FILL, GTK_FILL, 0, 0);

  // pre instance
  item2 = gtk_check_button_new_with_label(_("Use average color of children for folders"));

  item = gtk_spin_button_new_with_range(0, 100, 1);
  gtk_table_attach(GTK_TABLE(table), item, 1, 2, 0, 1,
                   GTK_FILL, GTK_FILL, 0, 0);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(item), pref_get_max_depth());
  g_signal_connect(G_OBJECT(item), "value-changed",
                   G_CALLBACK(on_depth_changed), item2);
  
  item = gtk_label_new(_("(0=unlimited)"));
  gtk_misc_set_alignment(GTK_MISC(item), 1, 0.5);
  gtk_table_attach(GTK_TABLE(table), item, 2, 3, 0, 1,
                   GTK_FILL, GTK_FILL, 0, 0);

  
  section = ui_create_section(_("Color settings"), NULL, &hbox, NULL);
  gtk_box_pack_start(GTK_BOX(vbox0), section, FALSE, FALSE, 0);
  
  table = gtk_table_new(2, 2, FALSE);
  gtk_box_pack_start(GTK_BOX(hbox), table, TRUE, TRUE, 0);
  gtk_table_set_col_spacings(GTK_TABLE(table), 10);
  gtk_table_set_row_spacings(GTK_TABLE(table), 2);
/*   gtk_table_set_homogeneous(GTK_TABLE(table), TRUE); */

  item = gtk_check_button_new_with_label(_("Enable colors"));
  gtk_button_set_alignment(GTK_BUTTON(item), 0, 0.5);
  gtk_table_attach(GTK_TABLE(table), item, 0, 1, 0, 1,
                   GTK_FILL|GTK_EXPAND, GTK_FILL, 0, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(item), pref_get_use_colors());
  g_signal_connect(G_OBJECT(item), "toggled", G_CALLBACK(on_use_colors), item2);

  item = gtk_button_new_with_label(_("Color profile..."));
  gtk_table_attach(GTK_TABLE(table), item, 1, 2, 0, 1,
                   GTK_FILL, GTK_FILL, 0, 0);
  g_signal_connect_swapped(G_OBJECT(item), "clicked",
                           G_CALLBACK(on_setup_colors), PrefWin);

  item = item2;
  gtk_button_set_alignment(GTK_BUTTON(item), 0, 0.5);
  gtk_table_attach(GTK_TABLE(table), item, 0, 2, 1, 2,
                   GTK_FILL, GTK_FILL, 0, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(item), pref_get_use_average());
  gtk_widget_set_sensitive(item, pref_get_use_colors() && pref_get_max_depth() != 0);
  g_signal_connect(G_OBJECT(item), "toggled", G_CALLBACK(on_use_average), NULL);


  section = ui_create_section(_("Scanning options (requires re-scan)"), NULL, &hbox, NULL);
  gtk_box_pack_start(GTK_BOX(vbox0), section, FALSE, FALSE, 0);

  vbox = gtk_vbox_new(FALSE, 2);
  gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);

  item = gtk_check_button_new_with_label(_("Descend directories on other filesystems"));
  gtk_box_pack_start(GTK_BOX(vbox), item, FALSE, FALSE, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(item), pref_get_leave_device());
  g_signal_connect(G_OBJECT(item), "toggled", G_CALLBACK(on_leave_device), item2);

  item = gtk_check_button_new_with_label(_("Use allocated disk size of files"));
  gtk_box_pack_start(GTK_BOX(vbox), item, FALSE, FALSE, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(item), !pref_get_use_reported_size());
  g_signal_connect(G_OBJECT(item), "toggled", G_CALLBACK(on_use_reported), item2);
  

  // buttons
  item = gtk_hseparator_new();
  gtk_box_pack_start(GTK_BOX(vbox0), item, FALSE, FALSE, 0);

  hbox = gtk_hbutton_box_new();
  gtk_button_box_set_layout(GTK_BUTTON_BOX(hbox), GTK_BUTTONBOX_END);
  gtk_box_set_spacing(GTK_BOX(hbox), 10);
  gtk_box_pack_start(GTK_BOX(vbox0), hbox, FALSE, FALSE, 0);

  item = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
  gtk_container_add(GTK_CONTAINER(hbox), item);
  g_signal_connect_swapped(G_OBJECT(item), "clicked",
                           G_CALLBACK(gtk_widget_destroy), win);

  if (parent)
  {
      gtk_window_set_transient_for(GTK_WINDOW(win), parent);
      gtk_window_set_destroy_with_parent(GTK_WINDOW(win), TRUE);
  }

  gtk_widget_show_all(win);
}


static void pref_load(const char* filename)
{
    GKeyFile* file = g_key_file_new();
    GError* error = NULL;

    if (!g_key_file_load_from_file(file, filename, 0, &error))
    {
        if (error->code != G_FILE_ERROR_NOENT) {
	    // The settings file is allowed to not exist
	    g_message("%s", error->message);
        }
        g_error_free(error);
        return;
    }
  
    if (g_key_file_has_key(file, "Display", "Mode", NULL))
    {
        int val = g_key_file_get_integer(file, "Display", "Mode", NULL);
        pref_set_display_mode(val);
    }
  
    if (g_key_file_has_key(file, "Display", "MaxDepth", NULL))
    {
        int val = g_key_file_get_integer(file, "Display", "MaxDepth", NULL);
        pref_set_max_depth(val);
    }
  
    if (g_key_file_has_key(file, "Display", "HValue", NULL))
    {
        int val = g_key_file_get_integer(file, "Display", "HValue", NULL);
        pref_set_h((double)val/100.);
    }
  
    if (g_key_file_has_key(file, "Display", "FValue", NULL))
    {
        int val = g_key_file_get_integer(file, "Display", "FValue", NULL);
        pref_set_f((double)val/100.);
    }
  
    if (g_key_file_has_key(file, "Display", "UseAverage", NULL))
    {
        gboolean val = g_key_file_get_boolean(file, "Display", "UseAverage", NULL);
        pref_set_use_average(val);
    }
  
    if (g_key_file_has_key(file, "Scan", "LeaveDevice", NULL))
    {
        gboolean val = g_key_file_get_boolean(file, "Scan", "LeaveDevice", NULL);
        pref_set_leave_device(val);
    }
  
    if (g_key_file_has_key(file, "Scan", "UseReportedSize", NULL))
    {
        gboolean val = g_key_file_get_boolean(file, "Display", "UseReportedSize", NULL);
        pref_set_use_reported_size(val);
    }
  
    g_key_file_free(file);
}

void pref_init()
{
    const char* home = g_get_home_dir();
    char* filename = g_strdup_printf("%s/.gdmap/settings.ini", home);
    pref_load(filename);
}
