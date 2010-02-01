/*
 * tp-helpers.h - Header for various helper functions 
 * for telepathy implementation
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

#ifndef __TELEPATHY_HELPERS_H__
#define __TELEPATHY_HELPERS_H__

#include <glib.h>

G_BEGIN_DECLS

typedef struct
{
  gpointer key;
  gpointer value;
} TpKeyValue;

DBusGConnection * tp_get_bus ();
DBusGProxy * tp_get_bus_proxy ();
GSList *tp_hash_to_key_value_list(GHashTable *hashtable);
void tp_key_value_list_free(GSList *list);


typedef struct _tp_connmgr_info TpConnMgrInfo;
typedef struct _tp_connmgr_protocol_info TpConnMgrProtInfo;

struct _tp_connmgr_protocol_info
{
  gchar **mandatory_params;
  gchar **optional_params;

  /* defaul value hash */
  GHashTable *default_params;
};

struct _tp_connmgr_info
{
  gchar *name;
  gchar *bus_name;
  gchar *object_path;
  gchar **protocols;

  /* hash of protocol infos */
  GHashTable *protocol_info;
};

/*
 * Lists all the connection manager files.
 * @return A list of connection manager names
 */
GSList *tp_connmgr_list_cms(void);

/*
 * Returns the connection manager information for the given connection manager
 * @param cm: the connection manager name string
 * @return A struct containing all the information read from the connection
 * manager file.
 */
TpConnMgrInfo *tp_connmgr_get_info(gchar *cm);


G_END_DECLS

#endif /* __TELEPATHY_HELPERS_H__ */

