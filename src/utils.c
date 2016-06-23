/* Copyright (C) 2005 sgop@users.sourceforge.net This is free software
 * distributed under the terms of the GNU Public License.  See the
 * file COPYING for details.
 */
/* $Revision: 1.2 $
 * $Date: 2005/12/27 14:29:51 $
 * $Author: sgop $
 */

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "utils.h"

char* arg(char* data, int last) {
  static char* string;
  char* pos;
  char* pos2;

  if (data) string = data;
  if (!string) return NULL;
  
  if (!last) while (isspace(*string)) string++;

  if (last) {
    pos = string;
    string = NULL;
    if (*pos == 0) return NULL;
    else return pos;
  }
  if (*string == '\"') {
    pos = ++string;
    pos2 = strchr(string, '\"');
    if (pos2) {
      *pos2 = 0;
      string = pos2+1;
      if (*string == ' ') string++;
      return pos;
    } else {
      string = NULL;
      return NULL;
    }
  } else {
    pos = string;
    pos2 = strchr(string, ' ');
    if (pos2) {
      *pos2 = 0;
      string = pos2+1;
    } else {
      string = NULL;
    }
    if (*pos == 0) return NULL;
    else return pos;
  }

  return NULL;
}

char* print_filesize(file_size_t bytes) {
  double size = bytes;
  if (size < 1024) return g_strdup_printf("%.0f B", size);
  size /= 1024.;
  if (size < 1024) return g_strdup_printf("%.2f KiB", size);
  if (size < 1024) return g_strdup_printf("%.1f KiB", size);
  size /= 1024.;
  if (size < 128) return g_strdup_printf("%.2f MiB", size);
  if (size < 1024) return g_strdup_printf("%.1f MiB", size);
  size /= 1024.;
  if (size < 128) return g_strdup_printf("%.2f GiB", size);
  if (size < 1024) return g_strdup_printf("%.1f GiB", size);
  size /= 1024.;
  if (size < 128) return g_strdup_printf("%.2f TiB", size);
  if (size < 1024) return g_strdup_printf("%.1f TiB", size);
  size /= 1024.;
  if (size < 128) return g_strdup_printf("%.2f PiB", size);
  if (size < 1024) return g_strdup_printf("%.1f PiB", size);
  size /= 1024.;
  return g_strdup_printf("%.2f EiB", size);
}

static int directory_exists(char* dir) {
  struct stat st;

  if (stat(dir, &st) < 0) return 0; 
  else return 1;
}

int create_dir(const char *dir) {
  char* ddir = g_strdup(dir);
  char *pos;
  char *slash;

  if (!ddir) return 0;
  pos = strrchr(ddir, DIR_SEP);
  if (!pos) return 0;
  *pos = 0;
  if (directory_exists(ddir)) {
    *pos = DIR_SEP;
    return 1;
  }
  *pos = DIR_SEP;
  
  slash = ddir;
  while ((pos = strchr(slash + 1, DIR_SEP)) != NULL) {
    *pos = 0;
    if (!directory_exists(ddir)) {
      if (mkdir(ddir, 7 * 64 + 5 * 8 + 5)) {
        g_warning("Could not create folder '%s'", ddir);
        *pos = DIR_SEP;
        return 0;
      }
    }
    *pos = DIR_SEP;
    slash = pos;
  }
  g_free(ddir);
  return 1;
}

char* to_utf8(const char* str) {
  static char* result = NULL;
  if (result) g_free(result);
  result = g_locale_to_utf8(str, -1, NULL, NULL, NULL);
  return result;
}

char* make_string_from_list(GList* glist, const char* sep) {
  GList* dlist;
  char* result;
  unsigned len;
  unsigned add = strlen(sep);

  if (glist == NULL) return g_strdup("");

  len = 0;
  for (dlist = glist; dlist; dlist = dlist->next) {
    char* text = dlist->data;
    len += strlen(text)+add;
  }
  len -= add;
  result = g_malloc0(len+1);
  len = 0;
  for (dlist = glist; dlist; dlist = dlist->next) {
    char* text = dlist->data;
    unsigned l = strlen(text);
    if (l && dlist != glist) {
      memcpy(result+len, sep, add);
      len += add;
    }
    memcpy(result+len, text, l);
    len += l;
  }

  return result;
}

file_size_t extract_bytes(const char* string) {
  const char* pos;
  file_size_t result = 0;

  if (!string) return 0;
  if (!isdigit(*string)) return 0;

  pos = string;
  while (isdigit(*pos)) {
    result *= 10;
    result += *pos -'0';
    pos++;
  }

  if (*pos == ' ') pos++;
  switch (*pos) {
  case 't':
  case 'T':
    result *= 1024*1024;
    result *= 1024*1024;
    break;
  case 'g':
  case 'G':
    result *= 1024*1024*1024;
    break;
  case 'm':
  case 'M':
    result *= 1024*1024;
    break;
  case 'k':
  case 'K':
    result *= 1024;
    break;
  case 'b':
  case 'B':
  case ' ':
  case 0:
  default:
    break;
  }

  return result;
}

const char* print_bytes_exact(file_size_t bytes) {
  static char* str = NULL;

  g_free(str);

  if (bytes == 0)
    str = g_strdup("0B");
  else if (bytes % ((file_size_t)1024*(file_size_t)1024*(file_size_t)1024*(file_size_t)1204) == 0)
    str = g_strdup_printf(FF"T", bytes/1024/1024/1024);
  else if (bytes % (1024*1024*1024) == 0)
    str = g_strdup_printf(FF"G", bytes/1024/1024/1024);
  else if (bytes % (1024*1024) == 0)
    str = g_strdup_printf(FF"M", bytes/1024/1024);
  else if (bytes % 1024 == 0)
    str = g_strdup_printf(FF"K", bytes/1024);
  else
    str = g_strdup_printf(FF"B", bytes);

  return str;
}
