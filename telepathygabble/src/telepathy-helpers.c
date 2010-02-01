/*
 * telepathy-helpers.c - Source for some Telepathy D-Bus helper functions
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdlib.h>
#include <dbus/dbus-glib.h>
#include "telepathy-helpers.h"

#ifdef EMULATOR
#include "libgabble_wsd_solution.h"

	GET_STATIC_VAR_FROM_TLS(busCon,gabble_helpers,DBusGConnection*)
	#define busCon (*GET_WSD_VAR_NAME(busCon,gabble_helpers, s)())
	
	GET_STATIC_VAR_FROM_TLS(bus_proxy,gabble_helpers,DBusGProxy*)
	#define bus_proxy (*GET_WSD_VAR_NAME(bus_proxy,gabble_helpers, s)())	
	
#endif
	

DBusGConnection *
tp_get_bus ()
{
#ifndef EMULATOR
  static DBusGConnection *busCon = NULL;
#endif

  if (busCon == NULL)
    {
      GError *error = NULL;

      busCon = dbus_g_bus_get (DBUS_BUS_SESSION/*DBUS_BUS_STARTER*/, &error);

      if (busCon == NULL)
        {
          g_warning ("Failed to connect to starter bus: %s", error->message);
          exit (1);
        }
    }

  return busCon;
}

DBusGProxy *
tp_get_bus_proxy ()
{
#ifndef EMULATOR
  static DBusGProxy *bus_proxy = NULL;
#endif

  if (bus_proxy == NULL)
    {
      DBusGConnection *bus = tp_get_bus ();

      bus_proxy = dbus_g_proxy_new_for_name (bus,
                                            "org.freedesktop.DBus",
                                            "/org/freedesktop/DBus",
                                            "org.freedesktop.DBus");

      if (bus_proxy == NULL)
        g_error ("Failed to get proxy object for bus.");
    }

  return bus_proxy;
}

static void _list_builder (gpointer key, gpointer value, gpointer data);

GSList *
tp_hash_to_key_value_list (GHashTable *hash)
{
  GSList *ret = NULL;

  g_hash_table_foreach (hash, _list_builder, &ret);

  return ret;
}

void
tp_key_value_list_free (GSList *list)
{
  GSList *iter;

  for (iter = list; iter; iter = g_slist_next(iter))
  {
    g_free (iter->data);
  }

  g_slist_free (list);
}

static void _list_builder (gpointer key, gpointer value, gpointer data)
{
  GSList **list = (GSList **) data;
  TpKeyValue *kv = g_new0 (TpKeyValue, 1);

  kv->key = key;
  kv->value = value;

  *list = g_slist_prepend (*list, kv);
}

