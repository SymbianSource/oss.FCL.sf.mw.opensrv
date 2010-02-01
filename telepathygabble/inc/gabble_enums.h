/*
 * gabble_enums.h - Header for GabbleConnection
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
 
#ifndef enum_gabble
#define enum_gabble

/* signal enum */
#define LAST_SIGNAL_CON_MGR 2
#define LAST_SIGNAL_CON 6
#define LAST_SIGNAL_IM 1
#define LAST_SIGNAL_MUC 4
#define LAST_SIGNAL_MED_CHANNEL 7
#define LAST_SIGNAL_MED_SESSION 3
#define LAST_SIGNAL_MED_STREAM 13
#define MAX_STREAM_NAME_LEN 8
#define LAST_SIGNAL_PRE_CACHE 3
#define LAST_SIGNAL_REGISTER 2
#define LAST_SIGNAL_ROS_CHNL 3
#define LAST_SIGNAL_ROOM_CHNL 3
#define LAST_SIGNAL_ROSTER 1
#define LAST_SIGNAL_VCARD_MGR 1

//todo: what it should be 
#define LAST_SIGNAL_SEARCH 1
typedef struct _GabbleAllocator GabbleAllocator;
struct _GabbleAllocator
{
  gulong size;
  guint limit;
  guint count;
};

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
  GABBLE_DEBUG_SEARCH		 = 1 << 12,	
} GabbleDebugFlags;


typedef enum {
    PRESENCE_CAP_NONE = 0,
    PRESENCE_CAP_GOOGLE_TRANSPORT_P2P = 1 << 0,
    PRESENCE_CAP_GOOGLE_VOICE = 1 << 1,
    PRESENCE_CAP_JINGLE = 1 << 2,
    PRESENCE_CAP_JINGLE_DESCRIPTION_AUDIO = 1 << 3,
    PRESENCE_CAP_JINGLE_DESCRIPTION_VIDEO = 1 << 4,
} GabblePresenceCapabilities;

#endif
