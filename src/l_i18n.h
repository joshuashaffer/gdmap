/* Copyright (C) 2005 sgop@users.sourceforge.net This is free software
 * distributed under the terms of the GNU Public License.  See the
 * file COPYING for details.
 */
/* $Revision: 1.1 $
 * $Date: 2005/10/02 20:39:12 $
 * $Author: sgop $
 */

#ifndef _L_I18N_H_
#define _L_I18N_H_

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#ifdef ENABLE_NLS
#  include <glib/gi18n.h>
#else /* NLS is disabled */
#  define _(String) (String)
#  define N_(String) (String)
#  define Q_(String) g_strip_context(String)
#  define ngettext(StringS,StringP,Number) ((Number==1)?StringS:StringP)
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,String) (String)
#  define dcgettext(Domain,String,Type) (String)
#  define bindtextdomain(Domain,Directory) (Domain) 
#  define bind_textdomain_codeset(Domain,Codeset) (Codeset) 
#endif /* ENABLE_NLS */


#endif
