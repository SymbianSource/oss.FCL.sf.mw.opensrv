/*
 * gabble-media-session-enumtypes.c - Source for Gabble service discovery
 *
 * Copyright (C) 2006 Collabora Ltd.
 * 
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
 *
 * 
 */
/* Generated data (by glib-mkenums) */
//vinod: changed system path to local
#include "gabble-media-session.h"

#ifdef EMULATOR
#include "libgabble_wsd_solution.h"

	GET_STATIC_VAR_FROM_TLS(etype,gabble_media_session_enum_types,GType)
	#define etype (*GET_WSD_VAR_NAME(etype,gabble_media_session_enum_types,s)())
	
	GET_STATIC_VAR_FROM_TLS(etype1,gabble_media_session_enum_types,GType)
	#define etype1 (*GET_WSD_VAR_NAME(etype1,gabble_media_session_enum_types,s)())
	
	GET_STATIC_VAR_FROM_TLS(etype2,gabble_media_session_enum_types,GType)
	#define etype2 (*GET_WSD_VAR_NAME(etype2,gabble_media_session_enum_types,s)())

#endif


/* enumerations from "gabble-media-session.h" */
GType
gabble_media_session_mode_get_type (void)
{
#ifndef EMULATOR
  static GType etype = 0;
#endif
  
  if (etype == 0) {
    static const GEnumValue values[] = {
      { MODE_GOOGLE, "MODE_GOOGLE", "MODE_GOOGLE" },
      { MODE_JINGLE, "MODE_JINGLE", "MODE_JINGLE" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("GabbleMediaSessionMode", values);
  }
  return etype;
}
GType
jingle_session_state_get_type (void)
{
#ifndef EMULATOR
  static GType etype1 = 0;
#endif
  
  if (etype1 == 0) {
    static const GEnumValue values[] = {
      { JS_STATE_INVALID, "JS_STATE_INVALID", "JS_STATE_INVALID" },
      { JS_STATE_PENDING_CREATED, "JS_STATE_PENDING_CREATED", "JS_STATE_PENDING_CREATED" },
      { JS_STATE_PENDING_INITIATE_SENT, "JS_STATE_PENDING_INITIATE_SENT", "JS_STATE_PENDING_INITIATE_SENT" },
      { JS_STATE_PENDING_INITIATED, "JS_STATE_PENDING_INITIATED", "JS_STATE_PENDING_INITIATED" },
      { JS_STATE_PENDING_ACCEPT_SENT, "JS_STATE_PENDING_ACCEPT_SENT", "JS_STATE_PENDING_ACCEPT_SENT" },
      { JS_STATE_ACTIVE, "JS_STATE_ACTIVE", "JS_STATE_ACTIVE" },
      { JS_STATE_ENDED, "JS_STATE_ENDED", "JS_STATE_ENDED" },
      { 0, NULL, NULL }
    };
    etype1 = g_enum_register_static ("JingleSessionState", values);
  }
  return etype1;
}
GType
debug_message_type_get_type (void)
{
#ifndef EMULATOR
  static GType etype2 = 0;
#endif
  
  if (etype2 == 0) {
    static const GEnumValue values[] = {
      { DEBUG_MSG_INFO, "DEBUG_MSG_INFO", "DEBUG_MSG_INFO" },
      { DEBUG_MSG_DUMP, "DEBUG_MSG_DUMP", "DEBUG_MSG_DUMP" },
      { DEBUG_MSG_WARNING, "DEBUG_MSG_WARNING", "DEBUG_MSG_WARNING" },
      { DEBUG_MSG_ERROR, "DEBUG_MSG_ERROR", "DEBUG_MSG_ERROR" },
      { DEBUG_MSG_EVENT, "DEBUG_MSG_EVENT", "DEBUG_MSG_EVENT" },
      { 0, NULL, NULL }
    };
    etype2 = g_enum_register_static ("DebugMessageType", values);
  }
  return etype2;
}

/* Generated data ends here */

