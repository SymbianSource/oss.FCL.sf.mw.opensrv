/* tp-chan-type-search-gen.h
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

#ifndef DBUS_GLIB_CLIENT_WRAPPERS_org_freedesktop_Telepathy_Channel_Type_Search
#define DBUS_GLIB_CLIENT_WRAPPERS_org_freedesktop_Telepathy_Channel_Type_Search


static
#ifdef G_HAVE_INLINE
inline
#endif
gboolean
tp_chan_type_search (DBusGProxy *proxy, const GHashTable* IN_terms, GError **error)

{
  return dbus_g_proxy_call (proxy, "Search", error, dbus_g_type_get_map ("GHashTable", G_TYPE_STRING, G_TYPE_VALUE), IN_terms, G_TYPE_INVALID, G_TYPE_INVALID);
}

typedef void (*tp_chan_type_search_reply) (DBusGProxy *proxy, GError *error, gpointer userdata);

static void
tp_chan_type_search_async_callback (DBusGProxy *proxy, DBusGProxyCall *call, void *user_data)
{
  DBusGAsyncData *data = (DBusGAsyncData*)user_data;
  GError *error = NULL;
  dbus_g_proxy_end_call (proxy, call, &error,G_TYPE_INVALID);
  (*(tp_chan_type_search_reply)data->cb) (proxy, error, data->userdata);
  return;
}

#ifdef SYMBIAN
EXPORT_C 
#endif
static
#ifdef G_HAVE_INLINE
inline
#endif
DBusGProxyCall*
tp_chan_type_search_async (DBusGProxy *proxy, const GHashTable *IN_params, tp_chan_type_search_reply callback, gpointer userdata)

{
  DBusGAsyncData *stuff; 
  stuff = g_new (DBusGAsyncData, 1);
  stuff->cb = G_CALLBACK (callback);
  stuff->userdata = userdata;
  return dbus_g_proxy_begin_call (proxy, "Search", tp_chan_type_search_async_callback, stuff, g_free, dbus_g_type_get_map ("GHashTable", G_TYPE_STRING, G_TYPE_VALUE), IN_params, G_TYPE_INVALID);
}

static
#ifdef G_HAVE_INLINE
inline
#endif
gboolean
tp_chan_type_search_get_search_state (DBusGProxy *proxy, guint* OUT_arg0, GError **error)

{
  return dbus_g_proxy_call (proxy, "GetSearchState", error, G_TYPE_INVALID, G_TYPE_UINT, OUT_arg0, G_TYPE_INVALID);
}

typedef void (*tp_chan_type_search_get_search_state_reply) (DBusGProxy *proxy, guint OUT_arg0, GError *error, gpointer userdata);

static void
tp_chan_type_search_get_search_state_async_callback (DBusGProxy *proxy, DBusGProxyCall *call, void *user_data)
{
  DBusGAsyncData *data = (DBusGAsyncData*)user_data;
  GError *error = NULL;
  guint OUT_arg2;
  dbus_g_proxy_end_call (proxy, call, &error,G_TYPE_UINT, &OUT_arg2, G_TYPE_INVALID);
  (*(tp_chan_type_search_get_search_state_reply)data->cb) (proxy, OUT_arg2, error, data->userdata);
  return;
}

#ifdef SYMBIAN
EXPORT_C 
#endif
static
#ifdef G_HAVE_INLINE
inline
#endif
DBusGProxyCall*
tp_chan_type_search_get_search_state_async (DBusGProxy *proxy, tp_chan_type_search_get_search_state_reply callback, gpointer userdata)

{
  DBusGAsyncData *stuff; 
  stuff = g_new (DBusGAsyncData, 1);
  stuff->cb = G_CALLBACK (callback);
  stuff->userdata = userdata;
  return dbus_g_proxy_begin_call (proxy, "GetSearchState", tp_chan_type_search_get_search_state_async_callback, stuff, g_free, G_TYPE_INVALID);
}


static
#ifdef G_HAVE_INLINE
inline
#endif
gboolean
tp_chan_type_search_get_search_keys (DBusGProxy *proxy, char ** OUT_arg0, char *** OUT_arg1, GError **error)

{
  return dbus_g_proxy_call (proxy, "GetSearchKeys", error, G_TYPE_INVALID, G_TYPE_STRING, OUT_arg0, G_TYPE_STRV, OUT_arg1, G_TYPE_INVALID);
}

typedef void (*tp_chan_type_search_get_search_keys_reply) (DBusGProxy *proxy, char * OUT_arg0, char * *OUT_arg1, GError *error, gpointer userdata);


static void
tp_chan_type_search_get_search_keys_async_callback (DBusGProxy *proxy, DBusGProxyCall *call, void *user_data)
{
  DBusGAsyncData *data = (DBusGAsyncData*) user_data;
  GError *error = NULL;
  char * OUT_arg0;
  char ** OUT_arg1;
  dbus_g_proxy_end_call (proxy, call, &error, G_TYPE_STRING, &OUT_arg0, G_TYPE_STRV, &OUT_arg1, G_TYPE_INVALID);
  (*(tp_chan_type_search_get_search_keys_reply)data->cb) (proxy, OUT_arg0, OUT_arg1, error, data->userdata);
  return;
}


#ifdef SYMBIAN
EXPORT_C 
#endif
static
#ifdef G_HAVE_INLINE
inline
#endif
DBusGProxyCall*
tp_chan_type_search_get_search_keys_async (DBusGProxy *proxy, tp_chan_type_search_get_search_keys_reply callback, gpointer userdata)

{
  DBusGAsyncData *stuff;
  stuff = g_new (DBusGAsyncData, 1);
  stuff->cb = G_CALLBACK (callback);
  stuff->userdata = userdata;
  return dbus_g_proxy_begin_call (proxy, "GetSearchKeys", tp_chan_type_search_get_search_keys_async_callback, stuff, g_free, G_TYPE_INVALID);
}



#endif /* defined DBUS_GLIB_CLIENT_WRAPPERS_org_freedesktop_Telepathy_Channel_Type_Search */

G_END_DECLS
