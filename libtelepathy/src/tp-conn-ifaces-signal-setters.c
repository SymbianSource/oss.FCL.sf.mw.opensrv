/* tp-conn-ifaces-signal-setters.c
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

#include "tp-conn-ifaces-signal-setters.h"

void tp_conn_set_aliasing_signatures(DBusGProxy *proxy)
{
  dbus_g_proxy_add_signal(proxy, "AliasesChanged",
                          dbus_g_type_get_struct ("GValueArray",
                            G_TYPE_UINT, G_TYPE_STRING, G_TYPE_INVALID),
			  G_TYPE_INVALID);
}

void tp_conn_set_capabilities_signatures(DBusGProxy *proxy)
{
  dbus_g_proxy_add_signal(proxy, "CapabilitiesChanged",
			  dbus_g_type_get_struct ("GValueArray",
			    G_TYPE_UINT, G_TYPE_STRING, G_TYPE_UINT,
                            G_TYPE_UINT, G_TYPE_INVALID),
			  G_TYPE_INVALID);
}

void tp_conn_set_contactinfo_signatures(DBusGProxy *proxy)
{
  dbus_g_proxy_add_signal(proxy, "GotContactInfo",
			  G_TYPE_UINT, G_TYPE_STRING, G_TYPE_INVALID);
}
void tp_conn_set_forwarding_signatures(DBusGProxy *proxy)
{
  dbus_g_proxy_add_signal(proxy, "ForwardingChanged",
			  G_TYPE_UINT, G_TYPE_INVALID);
}
void tp_conn_set_presence_signatures(DBusGProxy *proxy)
{
  dbus_g_proxy_add_signal(proxy, "PresenceUpdate",
			  dbus_g_type_get_map ("GHashTable", G_TYPE_UINT, 
			  (dbus_g_type_get_struct ("GValueArray", G_TYPE_UINT, 
			  (dbus_g_type_get_map ("GHashTable", G_TYPE_STRING, 
                          (dbus_g_type_get_map ("GHashTable", G_TYPE_STRING, 
			  G_TYPE_VALUE)))), G_TYPE_INVALID))), G_TYPE_INVALID);
}
void tp_conn_set_privacy_signatures(DBusGProxy *proxy)
{
  dbus_g_proxy_add_signal(proxy, "PrivacyModeChanged",
			  G_TYPE_STRING, G_TYPE_INVALID);
}
void tp_conn_set_renaming_signatures(DBusGProxy *proxy)
{
  dbus_g_proxy_add_signal(proxy, "Renamed",
			  G_TYPE_UINT, G_TYPE_UINT, G_TYPE_INVALID);
}
void tp_conn_set_avatar_signatures(DBusGProxy *proxy)
{
    //dbus_g_proxy_add_signal( proxy, "AvatarUpdated", )
    dbus_g_proxy_add_signal(proxy, "AvatarUpdated", G_TYPE_UINT, G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_add_signal(proxy, "AvatarRetrieved", G_TYPE_UINT, G_TYPE_STRING, DBUS_TYPE_G_UCHAR_ARRAY, G_TYPE_STRING,G_TYPE_INVALID); 
}
