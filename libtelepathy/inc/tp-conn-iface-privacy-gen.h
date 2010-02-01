/* tp-conn-iface-privacy-gen.h
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

#ifndef DBUS_GLIB_CLIENT_WRAPPERS_org_freedesktop_Telepathy_Connection_Interface_Privacy
#define DBUS_GLIB_CLIENT_WRAPPERS_org_freedesktop_Telepathy_Connection_Interface_Privacy

static
#ifdef G_HAVE_INLINE
inline
#endif
gboolean
tp_conn_iface_privacy_get_privacy_mode (DBusGProxy *proxy, char ** OUT_arg0, GError **error)

{
  return dbus_g_proxy_call (proxy, "GetPrivacyMode", error, G_TYPE_INVALID, G_TYPE_STRING, OUT_arg0, G_TYPE_INVALID);
}

typedef void (*tp_conn_iface_privacy_get_privacy_mode_reply) (DBusGProxy *proxy, char * OUT_arg0, GError *error, gpointer userdata);

static void
tp_conn_iface_privacy_get_privacy_mode_async_callback (DBusGProxy *proxy, DBusGProxyCall *call, void *user_data)
{
  DBusGAsyncData *data = (DBusGAsyncData*)user_data;
  GError *error = NULL;
  char * OUT_arg0;
  dbus_g_proxy_end_call (proxy, call, &error, G_TYPE_STRING, &OUT_arg0, G_TYPE_INVALID);
  (*(tp_conn_iface_privacy_get_privacy_mode_reply)data->cb) (proxy, OUT_arg0, error, data->userdata);
  return;
}

static
#ifdef G_HAVE_INLINE
inline
#endif
DBusGProxyCall*
tp_conn_iface_privacy_get_privacy_mode_async (DBusGProxy *proxy, tp_conn_iface_privacy_get_privacy_mode_reply callback, gpointer userdata)

{
  DBusGAsyncData *stuff;
  stuff = g_new (DBusGAsyncData, 1);
  stuff->cb = G_CALLBACK (callback);
  stuff->userdata = userdata;
  return dbus_g_proxy_begin_call (proxy, "GetPrivacyMode", tp_conn_iface_privacy_get_privacy_mode_async_callback, stuff, g_free, G_TYPE_INVALID);
}
static
#ifdef G_HAVE_INLINE
inline
#endif
gboolean
tp_conn_iface_privacy_get_privacy_modes (DBusGProxy *proxy, char *** OUT_arg0, GError **error)

{
  return dbus_g_proxy_call (proxy, "GetPrivacyModes", error, G_TYPE_INVALID, G_TYPE_STRV, OUT_arg0, G_TYPE_INVALID);
}

typedef void (*tp_conn_iface_privacy_get_privacy_modes_reply) (DBusGProxy *proxy, char * *OUT_arg0, GError *error, gpointer userdata);

static void
tp_conn_iface_privacy_get_privacy_modes_async_callback (DBusGProxy *proxy, DBusGProxyCall *call, void *user_data)
{
  DBusGAsyncData *data = (DBusGAsyncData*)user_data;
  GError *error = NULL;
  char ** OUT_arg0;
  dbus_g_proxy_end_call (proxy, call, &error, G_TYPE_STRV, &OUT_arg0, G_TYPE_INVALID);
  (*(tp_conn_iface_privacy_get_privacy_modes_reply)data->cb) (proxy, OUT_arg0, error, data->userdata);
  return;
}

static
#ifdef G_HAVE_INLINE
inline
#endif
DBusGProxyCall*
tp_conn_iface_privacy_get_privacy_modes_async (DBusGProxy *proxy, tp_conn_iface_privacy_get_privacy_modes_reply callback, gpointer userdata)

{
  DBusGAsyncData *stuff;
  stuff = g_new (DBusGAsyncData, 1);
  stuff->cb = G_CALLBACK (callback);
  stuff->userdata = userdata;
  return dbus_g_proxy_begin_call (proxy, "GetPrivacyModes", tp_conn_iface_privacy_get_privacy_modes_async_callback, stuff, g_free, G_TYPE_INVALID);
}
static
#ifdef G_HAVE_INLINE
inline
#endif
gboolean
tp_conn_iface_privacy_set_privacy_mode (DBusGProxy *proxy, const char * IN_mode, GError **error)

{
  return dbus_g_proxy_call (proxy, "SetPrivacyMode", error, G_TYPE_STRING, IN_mode, G_TYPE_INVALID, G_TYPE_INVALID);
}

typedef void (*tp_conn_iface_privacy_set_privacy_mode_reply) (DBusGProxy *proxy, GError *error, gpointer userdata);

static void
tp_conn_iface_privacy_set_privacy_mode_async_callback (DBusGProxy *proxy, DBusGProxyCall *call, void *user_data)
{
  DBusGAsyncData *data = (DBusGAsyncData*)user_data;
  GError *error = NULL;
  dbus_g_proxy_end_call (proxy, call, &error, G_TYPE_INVALID);
  (*(tp_conn_iface_privacy_set_privacy_mode_reply)data->cb) (proxy, error, data->userdata);
  return;
}

static
#ifdef G_HAVE_INLINE
inline
#endif
DBusGProxyCall*
tp_conn_iface_privacy_set_privacy_mode_async (DBusGProxy *proxy, const char * IN_mode, tp_conn_iface_privacy_set_privacy_mode_reply callback, gpointer userdata)

{
  DBusGAsyncData *stuff;
  stuff = g_new (DBusGAsyncData, 1);
  stuff->cb = G_CALLBACK (callback);
  stuff->userdata = userdata;
  return dbus_g_proxy_begin_call (proxy, "SetPrivacyMode", tp_conn_iface_privacy_set_privacy_mode_async_callback, stuff, g_free, G_TYPE_STRING, IN_mode, G_TYPE_INVALID);
}
#endif /* defined DBUS_GLIB_CLIENT_WRAPPERS_org_freedesktop_Telepathy_Connection_Interface_Privacy */

G_END_DECLS
