/* Copyright (C) 2005-2006 sgop@users.sourceforge.net
 * This is free software distributed under the terms of the
 * GNU Public License.  See the file COPYING for details.
 */
/* $Revision: 1.12 $
 * $Date: 2008/05/23 14:54:28 $
 * $Author: sgop $
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <gtk/gtk.h>

#include "tree.h"
#include "utils.h"
#include "gui_main.h"
#include "preferences.h"

static ProgressFunc TheProgressFunc = NULL;
static void* TheProgressData = NULL;
static gboolean Aborted = FALSE;
static dev_t RootDevice = 0;

#define SPECIAL_FILE(mode)                                              \
    ( S_ISCHR(mode) || S_ISBLK(mode) || S_ISFIFO(mode) || S_ISSOCK(mode) )


static void tree_print(tree_t* tree)
{
    GList* dlist;
    printf("%*s%-20s "FF"\n", tree->depth*2, "", tree->name, tree->size);
    for (dlist = tree->entries; dlist; dlist = dlist->next)
    {
        tree = dlist->data;
        tree_print(tree);
    }
}

char* tree_full_name(tree_t* tree)
{
    char* res;

    if (tree->parent)
    {
        char* p = tree_full_name(tree->parent);
        res = g_strdup_printf("%s/%s", p, tree->name);
        g_free(p);
    }
    else
    {
        res = g_strdup(tree->name);
    }

    return res;
}

void tree_destroy(tree_t* tree)
{
    g_list_foreach(tree->entries, (GFunc)tree_destroy, NULL);
    g_list_free(tree->entries);
    g_free(tree->name);
    g_free(tree);
}

static tree_t* tree_new(const char* sname, gint64 size, unsigned depth)
{
    tree_t* result;

    result = g_malloc0(sizeof(tree_t));
    result->name = g_strdup(sname);
    result->size = size;
    result->entries = NULL;
    result->parent = NULL;
    result->depth = depth;

    if (TheProgressFunc)
    {
        static struct timeval last_t = {0,0};
        struct timeval t;
        int diff;

        gettimeofday(&t, 0);

        diff = (t.tv_sec - last_t.tv_sec) * 1000 +
            (t.tv_usec - last_t.tv_usec) / 1000;

        if (t.tv_sec - last_t.tv_sec > 1 ||
            (t.tv_sec - last_t.tv_sec) * 1000 + (t.tv_usec - last_t.tv_usec) / 1000 > 100)
        {
            if (!TheProgressFunc(TheProgressData)) Aborted = TRUE;
            last_t = t;
        }
    }

    return result;
}

static gint comp_func(gconstpointer p1, gconstpointer p2)
{
    const tree_t* t1 = (tree_t*)p1;
    const tree_t* t2 = (tree_t*)p2;
  
    if (t1->size < t2->size) return 1;
    if (t1->size > t2->size) return -1;
    return 0;
}

static tree_t* tree_scan_rec(const char* dirname, const char* shortname, int depth)
{
    struct stat buf;
    const char* name;
    tree_t* result = NULL;
    tree_t* tree = NULL;
    GDir* dir;
    GError* error = NULL;

    if (lstat(dirname, &buf) < 0) return NULL;
    if (!RootDevice) RootDevice = buf.st_dev;
    
    if ( SPECIAL_FILE(buf.st_mode) )
    {
/*         g_message("skip %s", dirname); */
        return NULL;
    }
    else if ( S_ISDIR(buf.st_mode) )
    {
        if (!pref_get_leave_device() && buf.st_dev != RootDevice)
        {
/*             g_message("Skip new device: %s", dirname); */
            return NULL;
        }

        if ((dir = g_dir_open(dirname, 0, &error)) == NULL)
        {
            // don't report "permission denied".
            if (error->code != G_FILE_ERROR_ACCES)
                g_message("(%d) %s", error->code, error->message);
            g_error_free(error);
            return NULL;
        }
        
        result = tree_new(shortname, 0, depth);
        
        while (result && !Aborted && (name = g_dir_read_name(dir)) != NULL)
        {
            char* nname;
            if (!strcmp(name, ".")) continue;
            if (!strcmp(name, "..")) continue;
            
            if (dirname[strlen(dirname)-1] == '/')
                nname = g_strdup_printf("%s%s", dirname, name);
            else
                nname = g_strdup_printf("%s/%s", dirname, name);
            
            if ((tree = tree_scan_rec(nname, name, depth+1)) != NULL)
            {
                tree->parent = result;
                result->size += tree->size;
                result->entries = g_list_insert_sorted(result->entries, tree, comp_func);
            }
            g_free(nname);
        }
        g_dir_close(dir);
        
        return result;
    }
    else
    {
        gint64 rsize = buf.st_size;
        gint64 ssize = buf.st_blocks * 512;

        if (S_ISREG(buf.st_mode) && ssize && ssize < rsize)
        {
/*             g_message("sparse %s ("FF"/"FF")", dirname, ssize, rsize); */
            if (pref_get_use_reported_size())
            {
/*                 g_message("-reported: ("FF")", rsize); */
                return tree_new(shortname, rsize, depth);
            }
            else
            {
/*                 g_message("-on disk: ("FF")", ssize); */
                return tree_new(shortname, ssize, depth);
            }
        }
        else
        {
            return tree_new(shortname, rsize, depth);
        }
    }
}

tree_t* tree_load(const char* folder, ProgressFunc func, void* data, unsigned depth)
{
    tree_t* tree;

    TheProgressFunc = func;
    TheProgressData = data;
    RootDevice = 0;
    Aborted = FALSE;

    g_assert(folder);
    tree = tree_scan_rec(folder, folder, depth);

    TheProgressFunc = NULL;
    if (Aborted)
    {
        tree_destroy(tree);
        return NULL;
    }
    else
    {
        return tree;
    }
}
















tree_t* tree_find_path(tree_t* root, tree_t* child) {
    tree_t* temp;

    g_assert(child && root);

    for (temp = child; temp; temp = temp->parent) {
        if (temp->parent == root) return temp;
    }
    return NULL;
}

tree_t* dir_tree_get_root(tree_t* dir) {
    tree_t* root = dir;

    if (!root) return root;
    while (root->parent) root = root->parent;

    return root;
}

/* void tree_refresh(tree_t* tree) { */
/*   tree_t* parent; */
/*   tree_t* new_tree; */
/*   tree_t* temp; */
/*   char str[2048]; */

/*   if (!tree) return; */
/*   parent = tree->parent; */

/*   if (parent) { */
/*     parent->entries = g_list_remove(parent->entries, tree); */
/*     for (temp = parent; temp; temp = temp->parent) { */
/*       temp->size -= tree->size; */
/*     } */
/*     new_tree = tree_scan_rec(tree_full_name(tree), tree->name, tree->depth); */
/*     tree_destroy(tree); */
/*     if (new_tree) { */
/*       for (temp = parent; temp; temp = temp->parent) { */
/*         temp->size += new_tree->size; */
/*       } */
/*       parent->entries = */
/*         g_list_insert_sorted(parent->entries, new_tree, comp_func); */
/*       new_tree->parent = parent; */
/*       gui_tree_display(new_tree); */
/*     } */
/*   } else { */
/*     strcpy(str, tree_full_name(tree)); */
/*     tree_load(str); */
/*   } */
/* } */

