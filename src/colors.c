/* Copyright (C) 2005 sgop@users.sourceforge.net This is free software
 * distributed under the terms of the GNU Public License.  See the
 * file COPYING for details.
 */
/* $Revision: 1.8 $
 * $Date: 2008/05/23 14:54:28 $
 * $Author: sgop $
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#include <libxml/parser.h>

#include "tree.h"
#include "utils.h"
#include "colors.h"
#include "gui_main.h"

static GList* Colors = NULL;
static int Saver = 0;

static void color_destroy(color_t* color)
{
    g_list_foreach(color->ext, (GFunc)g_free, NULL);
    if (color->gc) g_object_unref(G_OBJECT(color->gc));
    g_free(color);
}

static void color_init(color_t* color, int r, int g, int b, ColorType type)
{
    color->ext = NULL;
    color->gc = NULL;
    color->c.red = r;
    color->c.green = g;
    color->c.blue = b;
    color->type = type;
}

color_t* color_new(int r, int g, int b, ColorType type)
{
    color_t* color = g_malloc0(sizeof(*color));
    color_init(color, r, g, b, type);
    return color;
}

static void color_add_ext(color_t* color, const char* ext)
{
    if (color->type == COLOR_EXTENSION)
    {
        color->ext = g_list_append(color->ext, g_strdup(ext));
    }
    else
    {
        g_log("COLOR", G_LOG_LEVEL_WARNING,
              "cannot add extension '%s' to type %d", ext, color->type);
    }
}

color_t* color_new_exts(const GdkColor* gcolor, const char* exts)
{
    color_t* color;

    color = color_new(gcolor->red, gcolor->green, gcolor->blue, COLOR_EXTENSION);
    if (exts)
    {
        char* temp = g_strdup(exts);
        char* cur = temp;
        char* pos;

        while ((pos = strchr(cur, ' ')) != NULL)
        {
            *pos = 0;
            color_add_ext(color, cur);
            cur = pos+1;
        }
        color_add_ext(color, cur);
    }
    return color;
}

static color_t* color_new_with_exts(const char* c, const char* exts)
{
    GdkColor gcolor;
    gdk_color_parse(c, &gcolor);
    return color_new_exts(&gcolor, exts);
}


static void xml_ext_save(const char* ext, xmlNodePtr xnode)
{
    xmlNewTextChild(xnode, NULL, "Extension", ext);
}

static void xml_color_save(color_t* color, xmlNodePtr xnode)
{
    xmlNodePtr xsub = xmlNewChild(xnode, NULL, "Color", NULL);
    char* temp;

    switch (color->type)
    {
    case COLOR_DEFAULT:
        xmlNewProp(xsub, "type", "Default");
        break;
    case COLOR_FOLDER:
        xmlNewProp(xsub, "type", "Folder");
        break;
    case COLOR_MARK1:
        xmlNewProp(xsub, "type", "Mark1");
        break;
    case COLOR_MARK2:
        xmlNewProp(xsub, "type", "Mark2");
        break;
    default:
        break;
    }

    temp = g_strdup_printf("0x%04x", color->c.red);
    xmlNewProp(xsub, "r", temp);
    g_free(temp);
    temp = g_strdup_printf("0x%04x", color->c.green);
    xmlNewProp(xsub, "g", temp);
    g_free(temp);
    temp = g_strdup_printf("0x%04x", color->c.blue);
    xmlNewProp(xsub, "b", temp);
    g_free(temp);

    if (color->type == COLOR_EXTENSION)
        g_list_foreach(color->ext, (GFunc)xml_ext_save, xsub);

}

static void colors_save_real(const char* filename)
{
    xmlDocPtr xdoc;
    xmlNodePtr xroot;

/*     g_log("COLOR", G_LOG_LEVEL_DEBUG, "saving '%s'", filename); */

    xdoc = xmlNewDoc("1.0");
    xroot = xmlNewNode(NULL, "Colors");
    xmlDocSetRootElement(xdoc, xroot);

    g_list_foreach(Colors, (GFunc)xml_color_save, xroot);

    create_dir(filename);

    xmlSaveFormatFile(filename, xdoc, 1);
    xmlFreeDoc(xdoc);
}


static gboolean colors_save_delayed()
{
    const char* home = g_get_home_dir();
    char* filename = g_strdup_printf("%s/.gdmap/colors.xml", home);
    colors_save_real(filename);
    g_free(filename);
    if (Saver) g_source_remove(Saver);
    Saver = 0;
    return FALSE;
}

static void colors_save()
{
    if (Saver) return;
    Saver = g_timeout_add(100, (GSourceFunc)colors_save_delayed, NULL);
}


static void xml_color_get(xmlDocPtr xfile, xmlNodePtr xnode)
{
    xmlChar* attr;
    int r=0,g=0,b=0;
    color_t* color;
    ColorType type = COLOR_EXTENSION;

    attr = xmlGetProp(xnode, "type");
    if (attr)
    {
        if (!strcmp(attr, "Default"))
            type = COLOR_DEFAULT;
        else if (!strcmp(attr, "Folder"))
            type = COLOR_FOLDER;
        else if (!strcmp(attr, "Mark1"))
            type = COLOR_MARK1;
        else if (!strcmp(attr, "Mark2"))
            type = COLOR_MARK2;
        else
            g_log("COLOR", G_LOG_LEVEL_WARNING,
                  "unknown color type '%s'", attr);
        xmlFree(attr);
    }

    attr = xmlGetProp(xnode, "r");
    if (attr && !strncmp(attr, "0x", 2))
    {
        r = strtoul(attr+2, NULL, 16);
    }
    else
    {
        r = atoi(attr);
    }
    if (attr) xmlFree(attr);

    attr = xmlGetProp(xnode, "g");
    if (attr && !strncmp(attr, "0x", 2))
    {
        g = strtoul(attr+2, NULL, 16);
    }
    else
    {
        g = atoi(attr);
    }
    if (attr) xmlFree(attr);

    attr = xmlGetProp(xnode, "b");
    if (attr && !strncmp(attr, "0x", 2))
    {
        b = strtoul(attr+2, NULL, 16);
    }
    else
    {
        b = atoi(attr);
    }
    if (attr) xmlFree(attr);

    color = color_new(r, g, b, type);

    if (type == COLOR_EXTENSION)
    {
        xmlNodePtr xsub = xnode->xmlChildrenNode;
        while (xsub != NULL)
        {
            attr = xmlNodeListGetString(xfile, xsub->xmlChildrenNode, 1);
            if (attr)
            {
                color_add_ext(color, attr);
                xmlFree(attr);
            }
            xsub = xsub->next;
        }
    }

    Colors = g_list_append(Colors, color);
}

static void xml_colors_get(xmlDocPtr xfile, xmlNodePtr xnode)
{
    xmlNodePtr xsub = xnode->xmlChildrenNode;

    while (xsub != NULL)
    {
        if (!strcmp(xsub->name, "Color"))
        {
            xml_color_get(xfile, xsub);
        }
        xsub = xsub->next;
    }
}

static gboolean colors_load(const char* filename)
{
    xmlDocPtr xfile;
    xmlNodePtr xroot;

    if (!g_file_test(filename, G_FILE_TEST_EXISTS)) return FALSE;
/*     g_log("COLOR", G_LOG_LEVEL_DEBUG, "loading '%s'", filename); */

    xfile = xmlParseFile(filename);
    if (!xfile) return FALSE;

    xroot = xmlDocGetRootElement(xfile);
    if (xroot == NULL)
    {
        g_log("COLOR", G_LOG_LEVEL_WARNING, "no root element in file: %s", filename);
        xmlFreeDoc(xfile);
        return FALSE;
    }
  
    if (strcmp(xroot->name, "Colors"))
    {
        g_log("COLOR", G_LOG_LEVEL_WARNING, "Root element != 'Colors'");
        xmlFreeDoc(xfile);
        return FALSE;
    }
  
    xml_colors_get(xfile, xroot);
  
    xmlFreeDoc(xfile);

    return TRUE;
}

static void colors_create_default()
{

    Colors = g_list_append(Colors, color_new(0xcccc, 0xcccc, 0xcccc, COLOR_DEFAULT));
    Colors = g_list_append(Colors, color_new(0xffff, 0xffff, 0x0000, COLOR_MARK1));
    Colors = g_list_append(Colors, color_new(0x0f0f, 0xffff, 0xffff, COLOR_MARK2));
    Colors = g_list_append(Colors, color_new(0x2222, 0x4444, 0x0000, COLOR_FOLDER));

    Colors = g_list_append(Colors, color_new_with_exts("#DD0000", "asf avi mpg mpeg wmv mov"));
    Colors = g_list_append(Colors, color_new_with_exts("#FF00FF", "exe msi deb rpm run"));
    Colors = g_list_append(Colors, color_new_with_exts("#BB00BB", "dll o lo a so"));
    Colors = g_list_append(Colors, color_new_with_exts("#00AA00", "c cc cpp py php php3 php4 php5 js sh bat"));
    Colors = g_list_append(Colors, color_new_with_exts("#00FF00", "h hh"));
    Colors = g_list_append(Colors, color_new_with_exts("#EEEE00", "eps bmp svg gif png jpg jpeg tif tiff xpm"));
    Colors = g_list_append(Colors, color_new_with_exts("#00DDDD", "mp3 ogg flac wav wma m4a"));
    Colors = g_list_append(Colors, color_new_with_exts("#66aaff", "mdb doc pdf ps ppt txt po pot xsl xls xml html htm odp ods otp ots ott stc stw sxc sxi sxw hlp tex"));
    Colors = g_list_append(Colors, color_new_with_exts("#0000AA", "zip gz tgz rar bz2 7z"));

}

static GdkGC* color_get_gc(color_t* color)
{
    if (!color->gc)
    {
        GtkWidget* win = gui_get_main_win();
        color->gc = gdk_gc_new(win->window);
        gdk_colormap_alloc_color(gdk_rgb_get_cmap(), &color->c, FALSE, FALSE);
        gdk_gc_set_foreground(color->gc, &color->c);
        if (color->type == COLOR_MARK1 || color->type == COLOR_MARK2)
            gdk_gc_set_function(color->gc, GDK_XOR);
    }
    return color->gc;
}

static gboolean color_has_ext(color_t* color, const char* ext)
{
    GList* dlist;

    for (dlist = color->ext; dlist; dlist = dlist->next)
    {
        const char* temp = dlist->data;
        if (!strcasecmp(temp, ext)) return TRUE;
    }
    return FALSE;
}

static color_t* color_find_by_type(ColorType type)
{
    GList* dlist;

    for (dlist = Colors; dlist; dlist = dlist->next)
    {
        color_t* color = dlist->data;
        if (color->type == type) return color;
    }
    return NULL;
}

static color_t* color_find_by_ext(const char* ext)
{
    GList* dlist;
  
    for (dlist = Colors; dlist; dlist = dlist->next)
    {
        color_t* color = dlist->data;
        if (color_has_ext(color, ext)) return color;
    }
    return NULL;
}

GdkGC* color_get_gc_by_type(ColorType type)
{
    color_t* color = color_find_by_type(type);
    g_assert(color);
    return color_get_gc(color);
}

const GdkColor* color_get_by_type(ColorType type)
{
    color_t* color = color_find_by_type(type);
    g_assert(color);
    return &color->c;
}

const GdkColor* color_get_by_ext(const char* ext)
{
    color_t* color = color_find_by_ext(ext);
    if (color) return &color->c;
    else return color_get_by_type(COLOR_DEFAULT);
}

const GdkColor* color_get_by_file(const char* filename)
{
    char* suffix = strrchr(filename, '.');
    if (!suffix) return color_get_by_type(COLOR_DEFAULT);
    else return color_get_by_ext(suffix+1);
}


static void colors_assure_defaults()
{

    if (!color_find_by_type(COLOR_FOLDER))
        Colors = g_list_prepend(Colors, color_new(0x2200, 0x4400, 0x0000, COLOR_FOLDER));

    if (!color_find_by_type(COLOR_MARK2))
        Colors = g_list_prepend(Colors, color_new(0x0f00, 0xff00, 0xff00, COLOR_MARK2));

    if (!color_find_by_type(COLOR_MARK1))
        Colors = g_list_prepend(Colors, color_new(0xff00, 0xff00, 0x0000, COLOR_MARK1));

    if (!color_find_by_type(COLOR_DEFAULT))
        Colors = g_list_prepend(Colors, color_new(0xcc00, 0xcc00, 0xcc00, COLOR_DEFAULT));

}

GList* colors_get_list()
{
    return Colors;
}

void colors_clear()
{
    g_list_foreach(Colors, (GFunc)color_destroy, NULL);
    g_list_free(Colors);
    Colors = NULL;
    colors_save();
}

void color_add(color_t* color)
{
    Colors = g_list_prepend(Colors, color);
    colors_save();
}

void colors_init()
{
    const char* home = g_get_home_dir();
    char* filename = g_strdup_printf("%s/.gdmap/colors.xml", home);

    if (!colors_load(filename))
    {
        colors_create_default();
    }
    else
    {
        colors_assure_defaults();
    }
    g_free(filename);
}
