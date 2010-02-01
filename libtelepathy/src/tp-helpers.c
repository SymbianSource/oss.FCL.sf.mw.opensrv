/*
 * tp-helpers.c - Source for various helper functions
 * for telepathy implementation
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <dbus/dbus-glib.h>
#include <stdlib.h>
#include "tp-helpers.h"


#define CM_CONFS_DIR "/usr/share/telepathy/managers/"
#define CM_CONF_SUFFIX ".manager"
#define CM_CONF_GROUP "ConnectionManager"
#define FILE_SEPARATOR ','
#define PROTO "Proto "


#ifdef EMULATOR
#include "libtelepathy_wsd_solution.h"
#endif

#ifdef EMULATOR
	
	GET_STATIC_VAR_FROM_TLS(bus_proxy,tp_helpers,DBusGProxy *)
	#define bus_proxy (*GET_WSD_VAR_NAME(bus_proxy,tp_helpers,s)())
	
	GET_STATIC_VAR_FROM_TLS(bus1,tp_helpers,DBusGConnection *)
	#define bus1 (*GET_WSD_VAR_NAME(bus1,tp_helpers,s)())	
	
#endif
	
static void _list_builder(gpointer key, gpointer value, gpointer data);

DBusGConnection *
tp_get_bus ()
{
#ifndef EMULATOR
  static DBusGConnection *bus1 = NULL;
#endif
  

  if (bus1 == NULL)
    {
      GError *error = NULL;

      bus1 = dbus_g_bus_get (/*DBUS_BUS_STARTER*/DBUS_BUS_SESSION, &error);

      if (bus1 == NULL)
        g_error ("Failed to connect to starter bus: %s", error->message);
    }

  return bus1;
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

GSList *
tp_hash_to_key_value_list(GHashTable *hash)
{
  GSList *ret = NULL;
  g_hash_table_foreach(hash, _list_builder, &ret);
  return ret;
}

void
tp_key_value_list_free(GSList *list)
{
  GSList *iter;

  for (iter = list; iter; iter = g_slist_next(iter))
  {
    g_free(iter->data);
  }
  g_slist_free(list);
}

static void _list_builder(gpointer key, gpointer value, gpointer data)
{
  GSList **list = (GSList **)data;
  TpKeyValue *kv = g_new0(TpKeyValue, 1);
  kv->key = key;
  kv->value = value;
  *list = g_slist_prepend(*list, kv);
}

GSList *tp_connmgr_list_cms(void)
{
  GError *error = NULL;
  GDir *dir;
  const gchar *filename;
  gchar **name;
  gchar *absolute_filepath;
  GSList *cms = NULL;


  /* Read the configuration file directory */
  if ((dir = g_dir_open(CM_CONFS_DIR, 0, &error)) == NULL)
  {
    g_printerr("Error opening directory %s: %s", CM_CONFS_DIR,
        error->message);
    return NULL;
  }

  while ((filename = g_dir_read_name(dir)) != NULL)
    /* Skip the file if it doesn't contain the required file suffix */
    if (g_str_has_suffix(filename, CM_CONF_SUFFIX))
    {
      absolute_filepath = g_strconcat(CM_CONFS_DIR, filename, NULL);
      name = g_strsplit(filename, ".", 0);
      cms = g_slist_append(cms, *name);
    }

  return cms;
}

TpConnMgrInfo *tp_connmgr_get_info(gchar *cm)
{
  GError *error = NULL;
  GKeyFile *file;
  gsize len;
  gchar *absolute_filepath;
  gchar **protocols;
  TpConnMgrInfo *cm_info = (TpConnMgrInfo *)malloc(sizeof(TpConnMgrInfo));
  cm_info->protocol_info = g_hash_table_new(g_str_hash, g_str_equal);

  absolute_filepath = g_strconcat(CM_CONFS_DIR, cm, CM_CONF_SUFFIX, NULL);

  file = g_key_file_new();
  if (!g_key_file_load_from_file
      (file, absolute_filepath, G_KEY_FILE_NONE, &error))
  {
    /* handle error */
    g_printerr("%s", error->message);
    g_error_free(error);
    return NULL;
  }
  g_key_file_set_list_separator(file, FILE_SEPARATOR);

  cm_info->name = g_key_file_get_string(file, CM_CONF_GROUP,
            "Name", &error);
  if (!(cm_info->name))
  {
    /* handle error and free dynamic memory */
    g_printerr("%s", error->message);
    g_error_free(error);
    g_key_file_free(file);
    g_free(absolute_filepath);
    free(cm_info);
    return NULL;
  }
  cm_info->bus_name = g_key_file_get_string(file, CM_CONF_GROUP,
            "BusName", &error);
  if (!(cm_info->bus_name))
  {
    /* handle error and free dynamic memory */
    g_printerr("%s", error->message);
    g_error_free(error);
    g_key_file_free(file);
    g_free(absolute_filepath);
    g_free(cm_info->name);
    free(cm_info);
    return NULL;
  }
  cm_info->object_path = g_key_file_get_string(file,CM_CONF_GROUP,
            "ObjectPath", &error);
  if (!(cm_info->object_path))
  {
    /* handle error and free dynamic memory */
    g_printerr("%s", error->message);
    g_error_free(error);
    g_key_file_free(file);
    g_free(absolute_filepath);
    g_free(cm_info->name);
    g_free(cm_info->bus_name);
    free(cm_info);
    return NULL;
  }
  cm_info->protocols = g_key_file_get_string_list(file, CM_CONF_GROUP, 
                              "Protos", &len, &error);
  if (!(cm_info->protocols))
  {
    /* handle error and free dynamic memory */
    g_printerr("%s", error->message);
    g_error_free(error);
    g_key_file_free(file);
    g_free(absolute_filepath);
    g_free(cm_info->name);
    g_free(cm_info->bus_name);
    g_free(cm_info->object_path);
    free(cm_info);
    return NULL;
  }

  for(protocols=cm_info->protocols; *protocols; protocols++)
  {
    gchar **keys;
    gchar *key_value;
    gchar *proto_group =  g_strconcat(PROTO, *protocols, NULL);
    TpConnMgrProtInfo *cm_prot_info = 
                       (TpConnMgrProtInfo *)malloc(sizeof(TpConnMgrProtInfo));
    cm_prot_info->default_params = g_hash_table_new(g_str_hash, g_str_equal);
    
    cm_prot_info->mandatory_params = g_key_file_get_string_list(file, 
                                   proto_group, "MandatoryParams", &len, &error);
    if (!(cm_prot_info->mandatory_params))
    {
      g_printerr("%s", error->message);
      g_error_free(error);
    }
    cm_prot_info->optional_params = g_key_file_get_string_list(file, 
                                   proto_group, "OptionalParams", &len, &error);
    if (!(cm_prot_info->optional_params))
    {
      g_printerr("%s", error->message);
      g_error_free(error);
    }

    keys = g_key_file_get_keys (file, proto_group, &len, &error);
    if (!(keys))
    {
      g_printerr("%s", error->message);
      g_error_free(error);
    }
    else
    {
      for(; *keys; keys++)
      {
        if(g_str_has_prefix(*keys, "default"))
        {
          key_value = g_key_file_get_string(file, proto_group, 
                                                   *keys, &error);
          if (!(key_value))
          {
            g_printerr("%s", error->message);
            g_error_free(error);
          }
          else
            g_hash_table_insert(cm_prot_info->default_params, 
                                g_strdup(*keys), key_value);
        }
      }
   }
    

   g_hash_table_insert(cm_info->protocol_info, g_strdup(*protocols), 
                        cm_prot_info);
         
  }

  g_key_file_free(file);
  g_free(absolute_filepath);
  return cm_info;

}
