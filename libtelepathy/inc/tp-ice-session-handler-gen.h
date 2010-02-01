/* tp-ice-session-handler-gen.h
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

#ifndef DBUS_GLIB_CLIENT_WRAPPERS_org_freedesktop_Telepathy_Ice_SessionHandler
#define DBUS_GLIB_CLIENT_WRAPPERS_org_freedesktop_Telepathy_Ice_SessionHandler

static
#ifdef G_HAVE_INLINE
inline
#endif
gboolean
tp_ice_session_handler_error (DBusGProxy *proxy, const guint IN_errno, const char * IN_message, GError **error)

{
  return dbus_g_proxy_call (proxy, "Error", error, G_TYPE_UINT, IN_errno, G_TYPE_STRING, IN_message, G_TYPE_INVALID, G_TYPE_INVALID);
}

typedef void (*tp_ice_session_handler_error_reply) (DBusGProxy *proxy, GError *error, gpointer userdata);

static void
tp_ice_session_handler_error_async_callback (DBusGProxy *proxy, DBusGProxyCall *call, void *user_data)
{
  DBusGAsyncData *data = (DBusGAsyncData*)user_data;
  GError *error = NULL;
  dbus_g_proxy_end_call (proxy, call, &error, G_TYPE_INVALID);
  (*(tp_ice_session_handler_error_reply)data->cb) (proxy, error, data->userdata);
  return;
}

static
#ifdef G_HAVE_INLINE
inline
#endif
DBusGProxyCall*
tp_ice_session_handler_error_async (DBusGProxy *proxy, const guint IN_errno, const char * IN_message, tp_ice_session_handler_error_reply callback, gpointer userdata)

{
  DBusGAsyncData *stuff;
  stuff = g_new (DBusGAsyncData, 1);
  stuff->cb = G_CALLBACK (callback);
  stuff->userdata = userdata;
  return dbus_g_proxy_begin_call (proxy, "Error", tp_ice_session_handler_error_async_callback, stuff, g_free, G_TYPE_UINT, IN_errno, G_TYPE_STRING, IN_message, G_TYPE_INVALID);
}
static
#ifdef G_HAVE_INLINE
inline
#endif
gboolean
tp_ice_session_handler_ready (DBusGProxy *proxy, GError **error)

{
  return dbus_g_proxy_call (proxy, "Ready", error, G_TYPE_INVALID, G_TYPE_INVALID);
}

typedef void (*tp_ice_session_handler_ready_reply) (DBusGProxy *proxy, GError *error, gpointer userdata);

static void
tp_ice_session_handler_ready_async_callback (DBusGProxy *proxy, DBusGProxyCall *call, void *user_data)
{
  DBusGAsyncData *data = (DBusGAsyncData*)user_data;
  GError *error = NULL;
  dbus_g_proxy_end_call (proxy, call, &error, G_TYPE_INVALID);
  (*(tp_ice_session_handler_ready_reply)data->cb) (proxy, error, data->userdata);
  return;
}

static
#ifdef G_HAVE_INLINE
inline
#endif
DBusGProxyCall*
tp_ice_session_handler_ready_async (DBusGProxy *proxy, tp_ice_session_handler_ready_reply callback, gpointer userdata)

{
  DBusGAsyncData *stuff;
  stuff = g_new (DBusGAsyncData, 1);
  stuff->cb = G_CALLBACK (callback);
  stuff->userdata = userdata;
  return dbus_g_proxy_begin_call (proxy, "Ready", tp_ice_session_handler_ready_async_callback, stuff, g_free, G_TYPE_INVALID);
}
#endif /* defined DBUS_GLIB_CLIENT_WRAPPERS_org_freedesktop_Telepathy_Ice_SessionHandler */

G_END_DECLS
