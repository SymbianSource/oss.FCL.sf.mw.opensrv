/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2003 Imendio AB
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __LM_DEBUG_H__
#define __LM_DEBUG_H__
#include <stdio.h>
#include <glib.h>
#include "lm_enums.h"

//Wsd changes

/*typedef enum {

	LM_LOG_LEVEL_VERBOSE = 1 << (G_LOG_LEVEL_USER_SHIFT),
	LM_LOG_LEVEL_NET     = 1 << (G_LOG_LEVEL_USER_SHIFT + 1),
	LM_LOG_LEVEL_PARSER  = 1 << (G_LOG_LEVEL_USER_SHIFT + 2),
	LM_LOG_LEVEL_SSL     = 1 << (G_LOG_LEVEL_USER_SHIFT + 3),
	LM_LOG_LEVEL_SASL    = 1 << (G_LOG_LEVEL_USER_SHIFT + 4),
	LM_LOG_LEVEL_ALL     = (LM_LOG_LEVEL_NET |
				LM_LOG_LEVEL_VERBOSE |
				LM_LOG_LEVEL_PARSER |
				LM_LOG_LEVEL_SSL |
				LM_LOG_LEVEL_SASL)
} LmLogLevelFlags;
*/
#ifndef LM_LOG_DOMAIN
#  define LM_LOG_DOMAIN "LM"
#endif

#ifndef _DEBUG
#define LM_NO_DEBUG
#endif
#ifdef G_HAVE_ISO_VARARGS
#  ifdef LM_NO_DEBUG
#    define lm_verbose(...)
#  else
#    define lm_verbose(...) \
       g_log (LM_LOG_DOMAIN, LM_LOG_LEVEL_VERBOSE, __VA_ARGS__)
#  endif
#elif defined(G_HAVE_GNUC_VARARGS)
#  if LM_NO_DEBUG
#    define lm_verbose(fmt...)
#  else
#    define lm_verbose(fmt...) \
       g_log (LM_LOG_DOMAIN, LM_LOG_LEVEL_VERBOSE, fmt)
#  endif
#else
#  ifdef LM_NO_DEBUG
  static void
 lm_verbose(const gchar *format, ...) {};
#  else
static void
lm_verbose (const gchar *format, ...)
{
//#ifdef HAVELMLOGS
	FILE *fp;
  va_list args;
  va_start (args, format);
 // g_logv (LM_LOG_DOMAIN, LM_LOG_LEVEL_VERBOSE, format, args);
	fp = fopen("c:\\lmlogs.txt","a");

	if(fp)
		{
	/*			fprintf(fp,arg);
		fprintf(fp,"\n");*/
	//	fprintf (fp, "%lu: 0x%lx: ", _dbus_getpid (), pthread_self ());
		vfprintf (fp, format, args);
		fflush(fp);
		fclose(fp);
		}
  va_end (args);
  //#endif
}
#  endif
#endif

void lm_debug_init (void);

#endif /* __LM_DEBUG_H__ */
#define UNUSED_FORMAL_PARAM(p) (void) p

