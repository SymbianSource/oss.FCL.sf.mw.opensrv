/* tp-chan-iface-ice-signalling-gen.h
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
/* Generated by dbus-binding-tool; do not edit! */

#include <glib/gtypes.h>
#include <glib/gerror.h>
#include <dbus/dbus-glib.h>

G_BEGIN_DECLS

#ifndef DBUS_GLIB_CLIENT_WRAPPERS_org_freedesktop_Telepathy_Channel_Interface_IceSignalling
#define DBUS_GLIB_CLIENT_WRAPPERS_org_freedesktop_Telepathy_Channel_Interface_IceSignalling

static
#ifdef G_HAVE_INLINE
inline
#endif
gboolean
tp_chan_iface_ice_signalling_get_session_handlers (DBusGProxy *proxy, GPtrArray** OUT_arg0, GError **error)

{
  return dbus_g_proxy_call (proxy, "GetSessionHandlers", error, G_TYPE_INVALID, dbus_g_type_get_collection ("GPtrArray", dbus_g_type_get_struct ("GValueArray", DBUS_TYPE_G_OBJECT_PATH, G_TYPE_STRING, G_TYPE_INVALID)), OUT_arg0, G_TYPE_INVALID);
}

typedef void (*tp_chan_iface_ice_signalling_get_session_handlers_reply) (DBusGProxy *proxy, GPtrArray *OUT_arg0, GError *error, gpointer userdata);

static void
tp_chan_iface_ice_signalling_get_session_handlers_async_callback (DBusGProxy *proxy, DBusGProxyCall *call, void *user_data)
{
  DBusGAsyncData *data = (DBusGAsyncData*)user_data;
  GError *error = NULL;
  GPtrArray* OUT_arg0;
  dbus_g_proxy_end_call (proxy, call, &error, dbus_g_type_get_collection ("GPtrArray", dbus_g_type_get_struct ("GValueArray", DBUS_TYPE_G_OBJECT_PATH, G_TYPE_STRING, G_TYPE_INVALID)), &OUT_arg0, G_TYPE_INVALID);
  (*(tp_chan_iface_ice_signalling_get_session_handlers_reply)data->cb) (proxy, OUT_arg0, error, data->userdata);
  return;
}

static
#ifdef G_HAVE_INLINE
inline
#endif
DBusGProxyCall*
tp_chan_iface_ice_signalling_get_session_handlers_async (DBusGProxy *proxy, tp_chan_iface_ice_signalling_get_session_handlers_reply callback, gpointer userdata)

{
  DBusGAsyncData *stuff;
  stuff = g_new (DBusGAsyncData, 1);
  stuff->cb = G_CALLBACK (callback);
  stuff->userdata = userdata;
  return dbus_g_proxy_begin_call (proxy, "GetSessionHandlers", tp_chan_iface_ice_signalling_get_session_handlers_async_callback, stuff, g_free, G_TYPE_INVALID);
}
#endif /* defined DBUS_GLIB_CLIENT_WRAPPERS_org_freedesktop_Telepathy_Channel_Interface_IceSignalling */

G_END_DECLS
