/* Copyright (C) 2005 sgop@users.sourceforge.net This is free software
 * distributed under the terms of the GNU Public License.  See the
 * file COPYING for details.
 */
/* $Revision: 1.4 $
 * $Date: 2005/12/27 14:29:51 $
 * $Author: sgop $
 */

#ifndef _COLORS_H_
#define _COLORS_H_

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

typedef enum {
  COLOR_DEFAULT=0,
  COLOR_FOLDER,
  COLOR_MARK1,
  COLOR_MARK2,
  COLOR_EXTENSION
} ColorType;

typedef struct {
  ColorType type;
  GList* ext;
  GdkColor c;
  GdkGC* gc;
} color_t;

void colors_init();

GdkGC* color_get_gc_by_type(ColorType type);
const GdkColor* color_get_by_type(ColorType type);
const GdkColor* color_get_by_ext(const char* ext);
const GdkColor* color_get_by_file(const char* filename);
GList* colors_get_list();

void colors_clear();
void color_add(color_t* color);
color_t* color_new(int r, int g, int b, ColorType type);
color_t* color_new_exts(const GdkColor* gcolor, const char* exts);


#endif
