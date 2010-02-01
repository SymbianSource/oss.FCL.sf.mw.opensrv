/*
 * debug.h - Header for GabbleConnection
 * Copyright (C) 2005 Collabora Ltd.
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
#ifndef __DEBUG_H__
#define __DEBUG_H_

#include "config.h"

#include <glib.h>

#include "gabble_enums.h"


/*#ifdef ENABLE_DEBUG bsr */

G_BEGIN_DECLS

/*Moved to gabble_enums.h
typedef enum
{
  GABBLE_DEBUG_PRESENCE      = 1 << 0,
  GABBLE_DEBUG_GROUPS        = 1 << 1,
  GABBLE_DEBUG_ROSTER        = 1 << 2,
  GABBLE_DEBUG_DISCO         = 1 << 3,
  GABBLE_DEBUG_PROPERTIES    = 1 << 4,
  GABBLE_DEBUG_ROOMLIST      = 1 << 5,
  GABBLE_DEBUG_MEDIA         = 1 << 6,
  GABBLE_DEBUG_MUC           = 1 << 7,
  GABBLE_DEBUG_CONNECTION    = 1 << 8,
  GABBLE_DEBUG_IM            = 1 << 9,
  GABBLE_DEBUG_PERSIST       = 1 << 10,
  GABBLE_DEBUG_VCARD         = 1 << 11,
} GabbleDebugFlags;
*/

#ifdef SYMBIAN
IMPORT_C
#endif
void gabble_debug_set_flags_from_env ();
#ifdef SYMBIAN
IMPORT_C
#endif
void gabble_debug_set_flags (GabbleDebugFlags flags);

gboolean gabble_debug_flag_is_set (GabbleDebugFlags flag);
void gabble_debug (GabbleDebugFlags flag, const gchar *format, ...);
    /* bsr G_GNUC_PRINTF (2, 3); */
#ifdef ENABLE_DEBUG  /* bsr */
#ifdef DEBUG_FLAG

/*#define DEBUG(format, ...) \
  gabble_debug(DEBUG_FLAG, "%s: " format, G_STRFUNC, ##__VA_ARGS__)
*/

#define DEBUG G_DEBUG 
  static inline void G_DEBUG (const gchar *format, ...) 
	{
    gabble_debug(DEBUG_FLAG, "%s: " format, G_STRFUNC, ##__VA_ARGS__)
	}

#define DEBUGGING gabble_debug_flag_is_set(DEBUG_FLAG)

#define NODE_DEBUG(n, s) \
G_STMT_START { \
  gchar *debug_tmp = lm_message_node_to_string (n); \
  gabble_debug (DEBUG_FLAG, "%s: " s ":\n%s", G_STRFUNC, debug_tmp); \
  g_free (debug_tmp); \
} G_STMT_END

#else /* DEBUG_FLAG */
#define DEBUG 0
#define DEBUGGING  0
#define NODE_DEBUG(n, s)
#endif /* DEBUG_FLAG */

#else /* ENABLE_DEBUG */

#define DEBUG
#define DEBUGGING 0
#define NODE_DEBUG(n, s)

#endif /* ENABLE_DEBUG */

G_END_DECLS

#endif

