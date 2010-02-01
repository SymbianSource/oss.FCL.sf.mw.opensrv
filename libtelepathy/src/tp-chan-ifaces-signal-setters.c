/* tp-chan-ifaces-signal-setters.c
 *
 * Copyright (C) 2005 Collabora Ltd.
 * 
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include "tp-chan-ifaces-signal-setters.h"


void tp_chan_set_contactsearch_signatures(DBusGProxy *proxy)
{
  dbus_g_proxy_add_signal(proxy, "SearchResultReceived", G_TYPE_UINT,
			  (dbus_g_type_get_map ("GHashTable",
			  G_TYPE_STRING, G_TYPE_VALUE)), G_TYPE_INVALID);
  dbus_g_proxy_add_signal(proxy, "SearchStateChanged",
			  G_TYPE_UINT, G_TYPE_INVALID);
}


void tp_chan_set_streamedmedia_signatures(DBusGProxy *proxy)
{
  dbus_g_proxy_add_signal(proxy, "StreamStateChanged",
			  G_TYPE_UINT, G_TYPE_UINT,
			  G_TYPE_UINT, G_TYPE_INVALID);
  /* FIXME: Outdated
  dbus_g_proxy_add_signal(proxy, "ReceivedMediaParameters",
			  G_TYPE_UINT, G_TYPE_STRING, G_TYPE_STRING,
			  G_TYPE_INVALID);
  */
}

void tp_chan_set_roomlist_signatures(DBusGProxy *proxy)
{
  dbus_g_proxy_add_signal(proxy, "GotRooms",
			 (dbus_g_type_get_struct ("GValueArray",
			 G_TYPE_UINT, G_TYPE_STRING,
			 (dbus_g_type_get_map ("GHashTable",
			 G_TYPE_STRING, G_TYPE_VALUE)))), G_TYPE_INVALID);
  dbus_g_proxy_add_signal(proxy, "ListingRooms",
			  G_TYPE_BOOLEAN, G_TYPE_INVALID);
}

void tp_chan_set_text_signatures(DBusGProxy *proxy)
{
  dbus_g_proxy_add_signal(proxy, "Received", G_TYPE_UINT, G_TYPE_UINT,
			  G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT,
                          G_TYPE_STRING, G_TYPE_INVALID);
  dbus_g_proxy_add_signal(proxy, "Sent", G_TYPE_UINT, G_TYPE_UINT,
			  G_TYPE_STRING, G_TYPE_INVALID);
  dbus_g_proxy_add_signal(proxy, "SendError", G_TYPE_UINT, G_TYPE_UINT,
			  G_TYPE_UINT, G_TYPE_STRING, G_TYPE_INVALID);
  dbus_g_proxy_add_signal(proxy, "LostMessage", G_TYPE_INVALID);
}

void tp_chan_set_dtmf_signatures(DBusGProxy *proxy)
{
  dbus_g_proxy_add_signal(proxy, "ReceivedDTMF", G_TYPE_UINT,
			  G_TYPE_UINT, G_TYPE_INVALID);
}

void tp_chan_set_group_signatures(DBusGProxy *proxy)
{
  dbus_g_proxy_add_signal(proxy, "GroupFlagsChanged", G_TYPE_UINT,
			  G_TYPE_UINT, G_TYPE_INVALID);
  dbus_g_proxy_add_signal(proxy, "MembersChanged", G_TYPE_STRING,
			  DBUS_TYPE_G_UINT_ARRAY, DBUS_TYPE_G_UINT_ARRAY,
			  DBUS_TYPE_G_UINT_ARRAY, DBUS_TYPE_G_UINT_ARRAY,
			  G_TYPE_UINT, G_TYPE_UINT, G_TYPE_INVALID);
}

void tp_chan_set_hold_signatures(DBusGProxy *proxy)
{
  dbus_g_proxy_add_signal(proxy, "HoldStateChanged", G_TYPE_UINT,
			  G_TYPE_UINT, G_TYPE_INVALID);
}

void tp_chan_set_password_signatures(DBusGProxy *proxy)
{
  dbus_g_proxy_add_signal(proxy, "PasswordFlagsChanged",
			  G_TYPE_UINT, G_TYPE_UINT, G_TYPE_INVALID);
}

void tp_chan_set_icesignalling_signatures(DBusGProxy *proxy)
{
  dbus_g_proxy_add_signal(proxy, "NewIceSessionHandler",
                          DBUS_TYPE_G_OBJECT_PATH, G_TYPE_STRING,
                          G_TYPE_INVALID);
}
