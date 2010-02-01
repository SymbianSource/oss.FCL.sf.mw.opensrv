/* tp-conn.c
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

#include <string.h>
#include "tp-conn.h"
#include "tp-helpers.h"
#include "tp-connmgr.h"
#include "tp-props-iface.h"



#ifdef EMULATOR
#include "libtelepathy_wsd_solution.h"
#endif

#ifdef EMULATOR
	
	GET_STATIC_VAR_FROM_TLS(parent_class,tp_conn,GObjectClass *)
	#define parent_class (*GET_WSD_VAR_NAME(parent_class,tp_conn,s)())
	
	GET_STATIC_VAR_FROM_TLS(type1,tp_conn,GType)
	#define type1 (*GET_WSD_VAR_NAME(type1,tp_conn,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret1,tp_conn,GQuark)
	#define ret1 (*GET_WSD_VAR_NAME(ret1,tp_conn,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret2,tp_conn,GQuark)
	#define ret2 (*GET_WSD_VAR_NAME(ret2,tp_conn,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret3,tp_conn,GQuark)
	#define ret3 (*GET_WSD_VAR_NAME(ret3,tp_conn,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret4,tp_conn,GQuark)
	#define ret4 (*GET_WSD_VAR_NAME(ret4,tp_conn,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret5,tp_conn,GQuark)
	#define ret5 (*GET_WSD_VAR_NAME(ret5,tp_conn,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret6,tp_conn,GQuark)
	#define ret6 (*GET_WSD_VAR_NAME(ret6,tp_conn,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret7,tp_conn,GQuark)
	#define ret7 (*GET_WSD_VAR_NAME(ret7,tp_conn,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret8,tp_conn,GQuark)
	#define ret8 (*GET_WSD_VAR_NAME(ret8,tp_conn,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret9,tp_conn,GQuark)
    #define ret9 (*GET_WSD_VAR_NAME(ret9,tp_conn,s)())
	
#else
	static GObjectClass *parent_class = NULL;
#endif



static gboolean tp_conn_status_change_handler(DBusGProxy *proxy, 
 					      guint status, guint reason, 
 					      gpointer user_data); 

static void _tp_conn_connect_req_handler(DBusGProxy *proxy,
                                        GError *error, gpointer user_data);

static void _tp_conn_register_signal_marshallers()
{
  /* Register marshaller for NewChannel signal */
  dbus_g_object_register_marshaller(tp_conn_signals_marshal_VOID__OBJECT_STRING_UINT_UINT_BOOLEAN, G_TYPE_NONE, DBUS_TYPE_G_OBJECT_PATH, G_TYPE_STRING, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_BOOLEAN, G_TYPE_INVALID);
  /* Register marshaller for StatusChanged signal */
  dbus_g_object_register_marshaller(tp_conn_signals_marshal_VOID__UINT_UINT, G_TYPE_NONE, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_INVALID);
}

static void _tp_conn_register_interface_signal_marshallers()
{
  /* Register marshaller for Aliasing interface signal AliasUpdate and
     ContactInfo interface signal GotContactInfo*/
  dbus_g_object_register_marshaller(tp_ifaces_signals_marshal_VOID__UINT_STRING, G_TYPE_NONE, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_INVALID);
  /* Register marshaller for Forwarding interface signal ForwardingChanged */
  dbus_g_object_register_marshaller(tp_ifaces_signals_marshal_VOID__UINT, G_TYPE_NONE, G_TYPE_UINT, G_TYPE_INVALID);
  /* Register marshaller for Presence interface signal PresenceUpdate */
  dbus_g_object_register_marshaller(tp_ifaces_signals_marshal_VOID__BOXED, G_TYPE_NONE, G_TYPE_BOXED, G_TYPE_INVALID);
  /* Register marshaller for Privacy interface signal PrivacyModeChanged */
  dbus_g_object_register_marshaller(tp_ifaces_signals_marshal_VOID__STRING, G_TYPE_NONE, G_TYPE_STRING, G_TYPE_INVALID);
  /* Register marshaller for Renaming interface signal Renamed */
  dbus_g_object_register_marshaller(tp_ifaces_signals_marshal_VOID__UINT_UINT, G_TYPE_NONE, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_INVALID);
}


/* We initialize the list of signatures here, so that we can use
 * it to add them for new interface instances later.
 */

/* FIXME: This should be replaced by a more automatic way of doing
 * this. The reason for using a set of function pointers is that there is no
 * apparent cleaner way of doing this, unless DBusGProxy gains a non-varargs
 * version of dbus_g_proxy_add_signal...
 */

static void _tp_conn_init_interface_signal_signatures(GData **signal_sigs)
{
  g_datalist_init(signal_sigs);

  /* Create and store aliasing iface signal signatures */
  g_datalist_id_set_data(signal_sigs, TELEPATHY_CONN_IFACE_ALIASING_QUARK,
			 (gpointer)&tp_conn_set_aliasing_signatures);
  /* Create and store capabilities iface signal signatures */
  g_datalist_id_set_data(signal_sigs, TELEPATHY_CONN_IFACE_CAPABILITIES_QUARK,
			 (gpointer)&tp_conn_set_capabilities_signatures);
  /* Create and store contactinfo iface signal signatures */
  g_datalist_id_set_data(signal_sigs, TELEPATHY_CONN_IFACE_CONTACTINFO_QUARK,
			 (gpointer)&tp_conn_set_contactinfo_signatures);
  /* Create and store forwarding iface signal signatures */
  g_datalist_id_set_data(signal_sigs, TELEPATHY_CONN_IFACE_FORWARDING_QUARK,
			 (gpointer)&tp_conn_set_forwarding_signatures);
  /* Create and store presence iface signal signatures */
  g_datalist_id_set_data(signal_sigs, TELEPATHY_CONN_IFACE_PRESENCE_QUARK,
			 (gpointer)&tp_conn_set_presence_signatures);
  /* Create and store privacy iface signal signatures */
  g_datalist_id_set_data(signal_sigs, TELEPATHY_CONN_IFACE_PRIVACY_QUARK,
			 (gpointer)&tp_conn_set_privacy_signatures);
  /* Create and store renaming iface signal signatures */
  g_datalist_id_set_data(signal_sigs, TELEPATHY_CONN_IFACE_RENAMING_QUARK,
			 (gpointer)&tp_conn_set_renaming_signatures);
  g_datalist_id_set_data(signal_sigs, TELEPATHY_CONN_IFACE_AVATAR_QUARK,
               (gpointer)&tp_conn_set_avatar_signatures);
}

static void synthesize_status_changed(TpConn *conn);

static void tp_conn_init(GTypeInstance *instance, gpointer g_class)
{
  TpConn *self = TELEPATHY_CONN(instance);

  self->first_run = TRUE;
}


static void tp_conn_dispose(GObject *obj)
{
  TpConn *self = TELEPATHY_CONN(obj);
      
  if (self->first_run == TRUE)
  {
    self->first_run = FALSE;
    synthesize_status_changed(self);
    g_datalist_clear(&(self->interface_list));
  }
  
  /* Call parent class dispose method */
  if (G_OBJECT_CLASS(parent_class)->dispose)
  {
    G_OBJECT_CLASS(parent_class)->dispose(obj);
  }

}


static void tp_conn_finalize(GObject *obj)
{
  if (G_OBJECT_CLASS(parent_class)->finalize)
    {
      G_OBJECT_CLASS(parent_class)->finalize(obj);
    }

}


static void tp_conn_class_init(TpConnClass *klass)
{
  GObjectClass *obj = G_OBJECT_CLASS(klass);
  parent_class = g_type_class_peek_parent(klass);

  obj->set_property = parent_class->set_property;
  obj->get_property = parent_class->get_property;

  obj->dispose = tp_conn_dispose;
  obj->finalize = tp_conn_finalize;
  _tp_conn_register_signal_marshallers();
  _tp_conn_register_interface_signal_marshallers();
  _tp_conn_init_interface_signal_signatures(&(klass->iface_signal_sigs));
}

#ifdef SYMBIAN
EXPORT_C
#endif
GType tp_conn_get_type(void)
{
#ifndef EMULATOR
  static GType type1 = 0;
#endif
  
  if (type1 == 0)
    {
      static const GTypeInfo info = 
	{
	  sizeof(TpConnClass),
	  NULL,
	  NULL,
	  (GClassInitFunc)tp_conn_class_init,
	  NULL,
	  NULL,
	  sizeof(TpConn),
	  0,
	  (GInstanceInitFunc)tp_conn_init
	  
	};
      type1 = g_type_register_static(DBUS_TYPE_G_PROXY,
				    "TpConn", &info, 0);
    }
  return type1;
}

/* The interface name getters */

GQuark tp_get_conn_interface()
{
#ifndef EMULATOR
  static GQuark ret1 = 0;
#endif

  if (ret1 == 0)
  {
    /* FIXME: The naming conventions should be unified */
    ret1 = g_quark_from_static_string(TP_IFACE_CONN_INTERFACE);
  }

  return ret1;
}

GQuark tp_get_conn_aliasing_interface()
{
#ifndef EMULATOR
  static GQuark ret2 = 0;
#endif

  if (ret2 == 0)
  {
    ret2 = g_quark_from_static_string(TP_IFACE_CONN_INTERFACE_ALIASING);
  }

  return ret2;
}

GQuark tp_get_conn_capabilities_interface()
{
#ifndef EMULATOR
  static GQuark ret3 = 0;
#endif

  if (ret3 == 0)
  {
    ret3 = g_quark_from_static_string(TP_IFACE_CONN_INTERFACE_CAPABILITIES);
  }

  return ret3;
}

GQuark tp_get_conn_contactinfo_interface()
{
#ifndef EMULATOR
  static GQuark ret4 = 0;
#endif

  if (ret4 == 0)
  {
    ret4 = g_quark_from_static_string(TP_IFACE_CONN_INTERFACE_CONTACT_INFO);
  }

  return ret4;
}

GQuark tp_get_conn_forwarding_interface()
{
#ifndef EMULATOR
  static GQuark ret5 = 0;
#endif

  if (ret5 == 0)
  {
    ret5 = g_quark_from_static_string(TP_IFACE_CONN_INTERFACE_FORWARDING);
  }

  return ret5;
}

#ifdef SYMBIAN
EXPORT_C
#endif
GQuark tp_get_conn_presence_interface()
{
#ifndef EMULATOR
  static GQuark ret6 = 0;
#endif

  if (ret6 == 0)
  {
    ret6 = g_quark_from_static_string(TP_IFACE_CONN_INTERFACE_PRESENCE);
  }

  return ret6;
}


GQuark tp_get_conn_privacy_interface()
{
#ifndef EMULATOR
  static GQuark ret7 = 0;
#endif

  if (ret7 == 0)
  {
    ret7 = g_quark_from_static_string(TP_IFACE_CONN_INTERFACE_PRIVACY);
  }

  return ret7;
}



GQuark tp_get_conn_renaming_interface()
{
#ifndef EMULATOR
  static GQuark ret8 = 0;
#endif

  if (ret8 == 0)
  {
    ret8 = g_quark_from_static_string(TP_IFACE_CONN_INTERFACE_RENAMING);
  }

  return ret8;
}

#ifdef SYMBIAN
EXPORT_C 
#endif
GQuark tp_get_conn_avatar_interface(void)
    {
#ifndef EMULATOR
  static GQuark ret9 = 0;
#endif

  if (ret9 == 0)
  {
    ret9 = g_quark_from_static_string(TP_IFACE_CONN_INTERFACE_AVATAR);
  }

  return ret9;    
    }

TpConn *
tp_conn_new_without_connect (DBusGConnection *connection,
                             const gchar *bus_name,
                             const gchar *object_path,
                             guint *status,
                             GError **error)
{
  gchar *unique_name;
  gchar **interfaces;
  guint conn_status = TP_CONN_STATUS_DISCONNECTED;
  TpConn *obj;
  GError *err = NULL;

  g_return_val_if_fail (connection != NULL, NULL);
  g_return_val_if_fail (bus_name != NULL, NULL);
  g_return_val_if_fail (object_path, NULL);

  /* Create the proxy object for this connection. It will be used to
     perform the actual method calls over D-BUS. */

  if (!dbus_g_proxy_call (tp_get_bus_proxy (), "GetNameOwner", error,
        G_TYPE_STRING, bus_name, G_TYPE_INVALID,
        G_TYPE_STRING, &unique_name, G_TYPE_INVALID))
    {
      return NULL;
    }

  obj = g_object_new (TELEPATHY_CONN_TYPE,
      "name", unique_name,
      "path", object_path,
      "interface", TP_IFACE_CONN_INTERFACE,//TP_IFACE_CONNECTION,
      "connection", connection,
      NULL);
  g_free (unique_name);

  g_datalist_init (&(obj->interface_list));

  //ADD_SIGNALS_FOR_CONNECTION (DBUS_G_PROXY (obj)); //commenting this bcoz header file not found 
  //equivalent of above stmt
  dbus_g_proxy_add_signal (DBUS_G_PROXY (obj), "NewChannel",\
    DBUS_TYPE_G_OBJECT_PATH,\
    G_TYPE_STRING,\
    G_TYPE_UINT,\
    G_TYPE_UINT,\
    G_TYPE_BOOLEAN,\
    G_TYPE_INVALID);\
  dbus_g_proxy_add_signal (DBUS_G_PROXY (obj), "StatusChanged",\
    G_TYPE_UINT,\
    G_TYPE_UINT,\
    G_TYPE_INVALID);

  /* Check if the connection is already connected. If so, we can
   * already perform GetInterfaces(). */

  if (!tp_conn_get_status (DBUS_G_PROXY (obj), &conn_status, &err))
    {
      if (err != NULL)
        g_propagate_error (error, err);

      g_object_unref (obj);
      return NULL;
    }

  if (conn_status == TP_CONN_STATUS_CONNECTED)
    {
      if (!tp_conn_get_interfaces (DBUS_G_PROXY (obj), &interfaces, &err))
        {
          if (err != NULL)
            g_propagate_error (error, err);

          g_object_unref (obj);
          return NULL;
        }

      /* Initialize the interface objects for this TpConn object */
      tp_conn_local_set_interfaces (obj, interfaces);

      g_strfreev (interfaces);
    }
  else
    {
      /* Not connected yet, so this is really a new connection. Thus, we
         have to hook up to StatusChanged signal to perform the
         GetInterfaces when it goes Connected */

      dbus_g_proxy_connect_signal (DBUS_G_PROXY (obj), "StatusChanged",
          G_CALLBACK (tp_conn_status_change_handler), NULL, NULL);
    }

  if (status != NULL)
    *status = conn_status;
  return obj;
}


TpConn *
tp_conn_new (DBusGConnection *connection,
             const gchar *bus_name,
             const gchar *object_path)
{
  GError *error = NULL;
  guint status;
  TpConn *obj;

  g_return_val_if_fail (connection != NULL, NULL);
  g_return_val_if_fail (bus_name != NULL, NULL);
  g_return_val_if_fail (object_path, NULL);

  obj = tp_conn_new_without_connect (connection, bus_name, object_path,
      &status, &error);

  if (obj == NULL)
    {
      /* either GetNameOwner, GetStatus or GetInterfaces failed */
      if (error != NULL)
        {
          g_warning ("Failed to create connection for %s %s: %s", bus_name,
              object_path, error->message);
          g_error_free (error);
        }
      else
        {
          g_warning ("Failed to create connection for %s %s: error is NULL",
              bus_name, object_path);
        }

      return NULL;
    }

  if (status != TP_CONN_STATUS_CONNECTED)
    {
      tp_conn_connect_async (DBUS_G_PROXY (obj), _tp_conn_connect_req_handler,
          NULL);
    }

  return obj;
}

#ifdef SYMBIAN
EXPORT_C
#endif
TpChan *tp_conn_new_channel(DBusGConnection *connection,
			    TpConn *tp_conn, const gchar *bus_name,
			    gchar *type, guint handle_type,
			    guint handle, gboolean supress_handler)
{
  GError *error = NULL;
  gchar *chan_object_path = NULL;
  TpChan *new_chan = NULL;

  g_return_val_if_fail(connection, NULL);
  g_return_val_if_fail(TELEPATHY_IS_CONN(tp_conn), NULL);
  g_return_val_if_fail(bus_name, NULL);
  g_return_val_if_fail(type, NULL);

  /* Request a new channel to be created by using the proxy object.
     We also retrieve the object path for it here. */

  if (!tp_conn_request_channel(DBUS_G_PROXY(tp_conn),
                               type, handle_type, handle, supress_handler,
                               &chan_object_path, &error))
  {
    g_warning("RequestChannel() failed: %s\n", error -> message);
    
    g_error_free(error);
    return NULL;
  }


  /* Create the object to represent the channel */

  new_chan = tp_chan_new(connection, bus_name, chan_object_path, type,
                         handle_type, handle);
                         
                    
  g_free(chan_object_path);



  return new_chan;
}


void tp_conn_local_set_interfaces(TpConn *self, gchar **interfaces)
{
  gchar **temp_ifaces = NULL;
  const gchar *bus_name = dbus_g_proxy_get_bus_name(DBUS_G_PROXY(self));
  const gchar *object_path = dbus_g_proxy_get_path(DBUS_G_PROXY(self));

  DBusGConnection *connection = tp_get_bus ();

  if (interfaces == NULL || connection == NULL)
  {
    return;
  }

  /* Create and store the proxy objects for the connection interfaces. */
  for (temp_ifaces = interfaces; *temp_ifaces; temp_ifaces++)
  {
    DBusGProxy *if_proxy;
    GQuark key = g_quark_from_string(*temp_ifaces);

    if (key == TELEPATHY_PROPS_IFACE_QUARK)
      {
        if_proxy = DBUS_G_PROXY (tp_props_iface_new (connection,
                                                     bus_name, object_path));
      }
    else
      {
        if_proxy = dbus_g_proxy_new_for_name(connection, bus_name, object_path,
                                             *temp_ifaces);
        if (if_proxy != NULL)
        {
          GData *sig_list = TELEPATHY_CONN_GET_CLASS(self)->iface_signal_sigs;
          void (*signature_setter_func)();

          /* Does the interface have signals? If yes, add their signatures
             for the interface instance */
          signature_setter_func =
            g_datalist_id_get_data(&sig_list, key);

          if (signature_setter_func != NULL)
          {
            signature_setter_func(if_proxy);
          }
        }
      }

   if (if_proxy != NULL)
    {
      g_datalist_id_set_data_full(&(self->interface_list),
      key, if_proxy, g_object_unref);
    }

  }
}

#ifdef SYMBIAN
EXPORT_C
#endif
DBusGProxy *
tp_conn_get_interface (TpConn *self,
                       GQuark iface_quark)
{
  DBusGProxy *iface_proxy = NULL;

  g_return_val_if_fail (self != NULL, NULL);
  g_return_val_if_fail (iface_quark != 0, NULL);

  iface_proxy = (DBusGProxy *) g_datalist_id_get_data (
      &(self->interface_list), iface_quark);

  return iface_proxy;
}

void
_tp_conn_connect_req_handler (DBusGProxy *proxy,
                              GError *error,
                              gpointer user_data)
{
  /* The interfaces for the TpConn are set on the StatusChanged
     handler when we get connected. Just print errors (if any)
     here. */

  if (error)
  {
    g_warning ("Could not perform Connect() for the connection, because: %s",
               error->message);
    g_error_free (error);
    return;
  }

}

static gboolean
tp_conn_status_change_handler (DBusGProxy *proxy,
                               guint status,
                               guint reason,
                               gpointer user_data)
{
  gchar **interfaces = NULL;
  GError *error = NULL;
  TpConn *tp_conn = (TpConn *) proxy;

  /* If the connection is up, we can get the list of interfaces */
  /* FIXME: At some point, we should switch to doing this asynchronously */

  if (status == TP_CONN_STATUS_CONNECTED)
    {
      if (!tp_conn_get_interfaces (DBUS_G_PROXY (proxy), &interfaces, &error))
        {
          g_warning ("GetInterfaces failed: %s\n", error->message);
          g_error_free (error);
          return TRUE;
        }

      /* Initialize the interface objects for this TpConn object */

      tp_conn_local_set_interfaces (tp_conn, interfaces);

      g_strfreev (interfaces);

      dbus_g_proxy_disconnect_signal (proxy, "StatusChanged",
          G_CALLBACK (tp_conn_status_change_handler), NULL);
    }

  return TRUE;
}


static void synthesize_status_changed(TpConn *conn)
{

  DBusMessageIter iter;
  DBusMessage *msg = NULL;
  guint value;
  GType uint_type = G_TYPE_UINT;
  GArray *statuschanged_signal_types = g_array_new(FALSE, FALSE,
                                                  sizeof(GType));
                                                  
  
  if (!statuschanged_signal_types)
  {
    g_warning("%s: Could not allocate type array for StatusChanged",
              G_STRFUNC);
    return;
  }

  msg = dbus_message_new_signal(dbus_g_proxy_get_path(DBUS_G_PROXY(conn)),
                                TP_IFACE_CONN_INTERFACE, "StatusChanged");

  if (!msg)
  {
    g_warning("%s: Could not allocate message for StatusChanged signal",
              G_STRFUNC);
    g_array_free(statuschanged_signal_types, FALSE);
    return;
  }

  dbus_message_iter_init_append(msg, &iter); 
  value = TP_CONN_STATUS_DISCONNECTED; 
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &value); 
  value = TP_CONN_STATUS_REASON_NONE_SPECIFIED; 
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &value);

  /* TODO: It might be worth considering to setup/remove the array in
     base_init/base_deinit to make it persist for the whole class */
  
  if (statuschanged_signal_types)
  {
    g_array_insert_val(statuschanged_signal_types, 0, uint_type);
    g_array_insert_val(statuschanged_signal_types, 1, uint_type);
  }
  else
  {
    g_warning("%s: Could not allocate array for StatusChanged types",
              G_STRFUNC);
    dbus_message_unref(msg);
    return;
  }

  g_signal_emit_by_name(DBUS_G_PROXY(conn), 
                        TP_IFACE_CONN_SIGNAL_STATUSCHANGED_SYNTHESIZED, msg,
                        statuschanged_signal_types);
  g_array_free(statuschanged_signal_types, TRUE);
  dbus_message_unref(msg);
}
