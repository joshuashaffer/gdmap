/* Copyright (C) 2005 sgop@users.sourceforge.net This is free software
 * distributed under the terms of the GNU Public License.  See the
 * file COPYING for details.
 */
/* $Revision: 1.8 $
 * $Date: 2008/05/23 14:54:28 $
 * $Author: sgop $
 */

#ifndef _TREE_H_
#define _TREE_H_

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

typedef struct _tree_t tree_t;

typedef gboolean (*ProgressFunc)(void* data);

struct _tree_t {
  char* name;
  gint64 size;
  GList* entries;
  tree_t* parent;
  unsigned depth;
};

tree_t* tree_load(const char* folder, ProgressFunc func, void* data, unsigned depth);

void tree_destroy(tree_t* tree);

char* tree_full_name(tree_t* tree);
tree_t* tree_find_path(tree_t* root, tree_t* child);
tree_t* dir_tree_get_root(tree_t* dir);

#endif
