/* Copyright (C) 2005 sgop@users.sourceforge.net This is free software
 * distributed under the terms of the GNU Public License.  See the
 * file COPYING for details.
 */
/* $Revision: 1.2 $
 * $Date: 2005/12/27 14:29:51 $
 * $Author: sgop $
 */

#ifndef _UTILS_H_
#define _UTILS_H_

#include <glib.h>

typedef gint64 file_size_t;

#define DIR_SEP G_DIR_SEPARATOR
#define FF "%"G_GINT64_FORMAT  // format string for filesizes

char* arg(char* data, int last);
char* print_filesize(file_size_t bytes);
int create_dir(const char* dir);
char* to_utf8(const char* str);
char* make_string_from_list(GList* glist, const char* sep);

const char* print_bytes_exact(file_size_t bytes);
file_size_t extract_bytes(const char* string);

#endif
