/*
 * debug.c 
 * Copyright (C) 2006 Collabora Ltd.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <stdarg.h>
#include <stdio.h> // bsr 
#include <glib.h>

#include "debug.h"

/*#ifdef ENABLE_DEBUG*/

#ifdef EMULATOR
#include "libgabble_wsd_solution.h"

	GET_STATIC_VAR_FROM_TLS(flags,gabble_debug,GabbleDebugFlags)
	#define flags (*GET_WSD_VAR_NAME(flags,gabble_debug,s)())

	GET_STATIC_VAR_FROM_TLS(log_handler,gabble_debug,guint)
	#define log_handler (*GET_WSD_VAR_NAME(log_handler,gabble_debug,s)())
	
	GET_STATIC_ARRAY_FROM_TLS(keys,gabble_debug,GDebugKey)
	#define keys (GET_WSD_VAR_NAME(keys,gabble_debug, s)())	


#else
	static GabbleDebugFlags flags = 0;
	static guint log_handler; // bsr
	GDebugKey keys[] = {
  { "presence",      GABBLE_DEBUG_PRESENCE },
  { "groups",        GABBLE_DEBUG_GROUPS },
  { "roster",        GABBLE_DEBUG_ROSTER },
  { "disco",         GABBLE_DEBUG_DISCO },
  { "properties",    GABBLE_DEBUG_PROPERTIES },
  { "roomlist",      GABBLE_DEBUG_ROOMLIST },
  { "media-channel", GABBLE_DEBUG_MEDIA },
  { "muc",           GABBLE_DEBUG_MUC },
  { "connection",    GABBLE_DEBUG_CONNECTION },
  { "persist",       GABBLE_DEBUG_PERSIST },
  { "vcard",         GABBLE_DEBUG_VCARD },
  { 0, },
};

	
#endif

/* bsr added new */
static void
debug_log_handler1 (const gchar    *log_domain,
		   GLogLevelFlags  log_level,
		   const gchar    *message,
		   gpointer        user_data)
{
	#ifdef _DEBUG
	
   FILE* fp;
	
			fp = fopen("c:\\gabblelogs.txt","a");
	if(fp)
	{
		fprintf(fp,message);
		fprintf(fp,"\n");
		fclose(fp);
	}
	#endif //_DEBUG
}

#ifdef SYMBIAN
EXPORT_C
#endif
void gabble_debug_set_flags_from_env ()
{
  guint nkeys;
  const gchar *flags_string;

  for (nkeys = 0; keys[nkeys].value; nkeys++);

  flags_string = g_getenv ("GABBLE_DEBUG");

  if (flags_string)
    gabble_debug_set_flags (g_parse_debug_string (flags_string, keys, nkeys));
    
  // bsr
  	log_handler = g_log_set_handler (NULL, 0xFF, 
			   debug_log_handler1, NULL);    
}

#ifdef SYMBIAN
EXPORT_C
#endif
void gabble_debug_set_flags (GabbleDebugFlags new_flags)
{
  flags |= new_flags;
}


gboolean gabble_debug_flag_is_set (GabbleDebugFlags flag)
{
  return flag & flags;
}

void gabble_debug (GabbleDebugFlags flag,
                   const gchar *format,
                   ...)
{
  if (flag & flags)
    {
      va_list args;
      va_start (args, format);
      g_logv (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, format, args);
      va_end (args);
    }
}

/*#endif /* ENABLE_DEBUG */

