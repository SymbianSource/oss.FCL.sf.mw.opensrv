/* tp-connmgr.c
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

#include "tp-connmgr.h"
#include "tp-connmgr-signals-marshal.h"
#include "tp-conn.h"
#include "tp-helpers.h"

#ifdef EMULATOR
#include "libtelepathy_wsd_solution.h"
#endif

#ifdef EMULATOR
	
	GET_STATIC_VAR_FROM_TLS(parent_class,tp_connmgr,GObjectClass *)
	#define parent_class (*GET_WSD_VAR_NAME(parent_class,tp_connmgr,s)())
	
	GET_STATIC_VAR_FROM_TLS(type1,tp_connmgr,GType)
	#define type1 (*GET_WSD_VAR_NAME(type1,tp_connmgr,s)())
#else	
	static GObjectClass *parent_class = NULL;
#endif
	

static void _tp_connmgr_register_signal_marshallers()
{
  /* Register marshaller for the NewConnection signal */
  dbus_g_object_register_marshaller(tp_connmgr_signals_marshal_VOID__STRING_OBJECT_STRING, G_TYPE_NONE, G_TYPE_STRING, DBUS_TYPE_G_OBJECT_PATH, G_TYPE_STRING, G_TYPE_INVALID);
}

static void tp_connmgr_dispose(GObject *obj)
{
  TpConnMgr *self = TELEPATHY_CONNMGR(obj);

  if (self->first_run)
  {
    self->first_run = FALSE;
  }

  /* Call the parent dispose method */
  if (G_OBJECT_CLASS(parent_class)->dispose)
  {
    G_OBJECT_CLASS(parent_class)->dispose(obj);
  }

}


static void tp_connmgr_finalize(GObject *obj)
{
  if(G_OBJECT_CLASS(parent_class)->finalize)
  {
    G_OBJECT_CLASS(parent_class)->finalize(obj);
  }
}


static void tp_connmgr_init(GTypeInstance *instance, gpointer g_class)
{
  TpConnMgr *self = TELEPATHY_CONNMGR(instance);

  self->first_run = TRUE;
}


static void tp_connmgr_class_init(TpConnMgrClass *klass)
{
  GObjectClass *obj = G_OBJECT_CLASS(klass);
  parent_class = g_type_class_peek_parent(klass);

  obj->set_property = parent_class->set_property;
  obj->get_property = parent_class->get_property;

  obj->dispose = tp_connmgr_dispose;
  obj->finalize = tp_connmgr_finalize;
  
  _tp_connmgr_register_signal_marshallers();
}


GType tp_connmgr_get_type(void)
{
#ifndef EMULATOR
  static GType type1 = 0;
#endif
  
  if (type1 == 0)
  {
    static const GTypeInfo info = 
    {
      sizeof(TpConnMgrClass),
      NULL,
      NULL,
      (GClassInitFunc)tp_connmgr_class_init,
      NULL,
      NULL,
      sizeof(TpConnMgr),
      0,
      (GInstanceInitFunc)tp_connmgr_init

    };
    type1 = g_type_register_static(DBUS_TYPE_G_PROXY,
        "TpConnMgr", &info, 0);
  }
  return type1;
}




#ifdef SYMBIAN
EXPORT_C
#endif
TpConnMgr *tp_connmgr_new(DBusGConnection *connection,
			  const char *bus_name,
			  const char *object_path,
			  const char *interface_name)
{
  TpConnMgr *obj;
  g_return_val_if_fail(connection != NULL, NULL);
  g_return_val_if_fail(bus_name != NULL, NULL);
  g_return_val_if_fail(object_path, NULL);
  g_return_val_if_fail(interface_name, NULL);
  
  obj = g_object_new(TELEPATHY_CONNMGR_TYPE,
		     "name", bus_name,
		     "path", object_path,
		     "interface", interface_name,
		     "connection", connection, NULL);

  dbus_g_proxy_add_signal(DBUS_G_PROXY(obj), 
			  "NewConnection",
			  G_TYPE_STRING, DBUS_TYPE_G_OBJECT_PATH,
			  G_TYPE_STRING, G_TYPE_INVALID);

  return obj;
}


#ifdef SYMBIAN
EXPORT_C
#endif
TpConn *tp_connmgr_new_connection(TpConnMgr *self, 
				  GHashTable *connection_parameters,
				  gchar *protocol)
{
  GError *error = NULL;
  TpConn *tp_conn_obj = NULL;
  DBusGConnection *connection = tp_get_bus ();
  gchar *bus_name = NULL, *object_path = NULL;
  g_return_val_if_fail(TELEPATHY_IS_CONNMGR(self), NULL);
  g_return_val_if_fail(connection_parameters != NULL, NULL);

  /* Create the actual connection and acquire service and path
     information that the TpConn object will need */

  if (!tp_connmgr_request_connection(DBUS_G_PROXY(self), protocol,
                          connection_parameters, &bus_name,
                          &object_path, &error))
  {
    g_warning("Connect() failed: %s\n", error -> message);
    g_error_free(error);
    return NULL;
  }

  if (bus_name == NULL || object_path == NULL)
  {
    return NULL;
  }

  /* Create the TpConn object */

  tp_conn_obj = tp_conn_new(connection, bus_name, object_path);

  return tp_conn_obj;
}


#ifdef SYMBIAN
EXPORT_C
#endif
gboolean tp_connmgr_list_protocols(TpConnMgr *self, char *** proto)
{
  GError *error = NULL;

  return dbus_g_proxy_call (DBUS_G_PROXY(self), "ListProtocols", &error, G_TYPE_INVALID, G_TYPE_STRV, proto, G_TYPE_INVALID);
}


#ifdef SYMBIAN
EXPORT_C
#endif
gboolean
tp_connmgr_get_parameters (TpConnMgr *self, const char * IN_proto, GPtrArray** OUT_arg1)

{
  return dbus_g_proxy_call (DBUS_G_PROXY(self), "GetParameters", NULL, G_TYPE_STRING, IN_proto, G_TYPE_INVALID, dbus_g_type_get_collection ("GPtrArray", dbus_g_type_get_struct ("GValueArray", G_TYPE_STRING, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_VALUE, G_TYPE_INVALID)), OUT_arg1, G_TYPE_INVALID);
}

