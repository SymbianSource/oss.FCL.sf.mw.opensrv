/* tp-chan.c
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
#include "tp-chan.h"
#include "tp-chan-signals-marshal.h"
#include "tp-props-iface.h"
#include "tp-helpers.h"

#ifdef EMULATOR
#include "libtelepathy_wsd_solution.h"
#endif

#ifdef EMULATOR
	
	GET_STATIC_VAR_FROM_TLS(parent_class,tp_chan,GObjectClass *)
	#define parent_class (*GET_WSD_VAR_NAME(parent_class,tp_chan,s)())
	
	GET_STATIC_VAR_FROM_TLS(type1,tp_chan,GType)
	#define type1 (*GET_WSD_VAR_NAME(type1,tp_chan,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret1,tp_chan,GQuark)
	#define ret1 (*GET_WSD_VAR_NAME(ret1,tp_chan,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret2,tp_chan,GQuark)
	#define ret2 (*GET_WSD_VAR_NAME(ret2,tp_chan,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret3,tp_chan,GQuark)
	#define ret3 (*GET_WSD_VAR_NAME(ret3,tp_chan,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret4,tp_chan,GQuark)
	#define ret4 (*GET_WSD_VAR_NAME(ret4,tp_chan,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret5,tp_chan,GQuark)
	#define ret5 (*GET_WSD_VAR_NAME(ret5,tp_chan,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret6,tp_chan,GQuark)
	#define ret6 (*GET_WSD_VAR_NAME(ret6,tp_chan,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret7,tp_chan,GQuark)
	#define ret7 (*GET_WSD_VAR_NAME(ret7,tp_chan,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret8,tp_chan,GQuark)
	#define ret8 (*GET_WSD_VAR_NAME(ret8,tp_chan,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret9,tp_chan,GQuark)
	#define ret9 (*GET_WSD_VAR_NAME(ret9,tp_chan,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret10,tp_chan,GQuark)
	#define ret10 (*GET_WSD_VAR_NAME(ret10,tp_chan,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret11,tp_chan,GQuark)
	#define ret11 (*GET_WSD_VAR_NAME(ret11,tp_chan,s)())
	
	
#else
	static GObjectClass *parent_class = NULL;
#endif

static void synthesize_closed(TpChan *chan);

static void _tp_chan_register_signal_marshallers()
{
  /* Register marshaller for the Close signal */
  dbus_g_object_register_marshaller(tp_chan_signals_marshal_VOID__VOID,
				    G_TYPE_NONE, G_TYPE_INVALID);
}

static void _tp_chan_register_interface_signal_marshallers()
{
  
  /* Register marshaller for ContactSearch interface signal
     SearchResultReceived*/
 
  dbus_g_object_register_marshaller(tp_ifaces_signals_marshal_VOID__UINT_BOXED, 
  					G_TYPE_NONE, G_TYPE_UINT, G_TYPE_BOXED, G_TYPE_INVALID);

  /* Register marshaller for ContactSearch interface signal
   * SearchStateChanged */
  
  dbus_g_object_register_marshaller(tp_ifaces_signals_marshal_VOID__UINT,
				    G_TYPE_NONE, G_TYPE_UINT, G_TYPE_INVALID);

  /* Register marshaller for StreamedMedia interface signal
     ReceivedMediaParameters */
  dbus_g_object_register_marshaller(tp_ifaces_signals_marshal_VOID__UINT_STRING_STRING, G_TYPE_NONE, G_TYPE_STRING, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_INVALID);
  /* Register marshaller for StreamedMedia interface signal
     StreamStateChanged */

  dbus_g_object_register_marshaller(tp_ifaces_signals_marshal_VOID__UINT_UINT_UINT, G_TYPE_NONE, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_INVALID);

  /* Register marshaller for RoomList interface signal GotRooms */

  dbus_g_object_register_marshaller(tp_ifaces_signals_marshal_VOID__BOXED,
				    G_TYPE_NONE, G_TYPE_BOXED, G_TYPE_INVALID);

  /* Register marshaller for RoomList interface signal ListingRooms */
  dbus_g_object_register_marshaller(tp_ifaces_signals_marshal_VOID__BOOLEAN,
				    G_TYPE_NONE, G_TYPE_BOOLEAN,
				    G_TYPE_INVALID);

  /* Register marshaller for channel type Text interface signal Received */
  dbus_g_object_register_marshaller(tp_ifaces_signals_marshal_VOID__UINT_UINT_UINT_UINT_UINT_STRING, G_TYPE_NONE, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT,
				    G_TYPE_UINT, G_TYPE_UINT, G_TYPE_STRING,
				    G_TYPE_INVALID);

  /* Register marshaller for channel type Text interface Sent */
  dbus_g_object_register_marshaller(tp_ifaces_signals_marshal_VOID__UINT_UINT_STRING, G_TYPE_NONE, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_INVALID);

  /* Register marshaller used by the following iface/signal pairs:
   * DTFM/ReceivedDTMF, Group/GroupFlagsChanged, Hold/HoldStateChanged,
   * Password/PasswordFlagsChanged, Subject/SubjectFlagsChanged */
  dbus_g_object_register_marshaller(tp_ifaces_signals_marshal_VOID__UINT_UINT,
				    G_TYPE_NONE, G_TYPE_UINT, G_TYPE_UINT,
				    G_TYPE_INVALID);
  /* Register marshaller for Group interface signal MembersChanged */
  
  dbus_g_object_register_marshaller(tp_ifaces_signals_marshal_VOID__STRING_BOXED_BOXED_BOXED_BOXED_UINT_UINT, G_TYPE_NONE, G_TYPE_STRING, G_TYPE_BOXED,
                                    G_TYPE_BOXED, G_TYPE_BOXED, G_TYPE_BOXED,
                                    G_TYPE_UINT, G_TYPE_UINT,G_TYPE_INVALID);

  /* Register marshaller for Text Channel interface signal SendError */
  dbus_g_object_register_marshaller(tp_ifaces_signals_marshal_VOID__UINT_UINT_UINT_STRING, G_TYPE_NONE, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_INVALID);

  /* Register marshaller for IceSignalling interface signal
     NewIceSessionHandler */

  dbus_g_object_register_marshaller(tp_ifaces_signals_marshal_VOID__OBJECT_STRING, G_TYPE_NONE, DBUS_TYPE_G_OBJECT_PATH, G_TYPE_STRING, G_TYPE_INVALID);

}


/* We initialize the list of signatures here, so that we can use
 * it to add them for new interface instances later.*/

/* FIXME: This should be replaced by a more automatic way of doing
 * this. The reason for using a set of function pointers is that there is no
 * apparent cleaner way of doing this, unless DBusGProxy gains a non-varargs
 * version of dbus_g_proxy_add_signal...
 */


static void _tp_chan_init_interface_signal_signatures(GData **signal_sigs)
{
  g_datalist_init(signal_sigs);
  
  
  /* Create and store contact search iface signal signatures */
  g_datalist_id_set_data(signal_sigs, TELEPATHY_CHAN_IFACE_CONTACTSEARCH_QUARK,
			 (gpointer)tp_chan_set_contactsearch_signatures);

  /* Store streamed media iface signal signatures */
  g_datalist_id_set_data(signal_sigs,
			 TELEPATHY_CHAN_IFACE_STREAMED_QUARK,
			 (gpointer)tp_chan_set_streamedmedia_signatures);
  /* Store roomlist signal iface signal parameters */
  g_datalist_id_set_data(signal_sigs, TELEPATHY_CHAN_IFACE_ROOMLIST_QUARK,
			 (gpointer)tp_chan_set_roomlist_signatures);
  /* Store text iface signal signatures */
  g_datalist_id_set_data(signal_sigs, TELEPATHY_CHAN_IFACE_TEXT_QUARK,
			 (gpointer)tp_chan_set_text_signatures);
  /* Store DTMF iface signal signatures */
  g_datalist_id_set_data(signal_sigs, TELEPATHY_CHAN_IFACE_DTMF_QUARK,
			 (gpointer)tp_chan_set_dtmf_signatures);
  /* Store group iface signal signatures */
  g_datalist_id_set_data(signal_sigs, TELEPATHY_CHAN_IFACE_GROUP_QUARK,
			 (gpointer)tp_chan_set_group_signatures);
  /* Store hold iface signatures */
  g_datalist_id_set_data(signal_sigs, TELEPATHY_CHAN_IFACE_HOLD_QUARK,
			 (gpointer)tp_chan_set_hold_signatures);
  /* Store password iface signatures */
  g_datalist_id_set_data(signal_sigs, TELEPATHY_CHAN_IFACE_PASSWORD_QUARK,
			 (gpointer)tp_chan_set_password_signatures);
}

static void tp_chan_init(GTypeInstance *instance, gpointer g_class)
{
  TpChan *self = TELEPATHY_CHAN(instance);
  self->type = NULL;
  self->first_run = TRUE;
}


static void tp_chan_dispose(GObject *obj)
{
  TpChan *self = TELEPATHY_CHAN(obj);

  if (self->first_run)
  {
    self->first_run = FALSE;
    synthesize_closed(self);
    g_datalist_clear(&(self->interface_list));
  }
  
  /* Chain up to the parent class dispose */
  if (G_OBJECT_CLASS(parent_class)->dispose)
  {
    G_OBJECT_CLASS(parent_class)->dispose(obj);
  }

}


static void tp_chan_finalize(GObject *obj)
{
  TpChan *self = TELEPATHY_CHAN(obj);

  if (self->type)
  {
    g_free(self->type);
  }

  if (G_OBJECT_CLASS(parent_class)->finalize)
  {
    G_OBJECT_CLASS(parent_class)->finalize(obj);
  }
}


static void tp_chan_class_init(TpChanClass *klass)
{
  GObjectClass *obj = G_OBJECT_CLASS(klass);
  parent_class = g_type_class_peek_parent(klass);

  obj->set_property = parent_class->set_property;
  obj->get_property = parent_class->get_property;
  obj->dispose = tp_chan_dispose;
  obj->finalize = tp_chan_finalize;
  _tp_chan_register_signal_marshallers();
  _tp_chan_register_interface_signal_marshallers();
  _tp_chan_init_interface_signal_signatures(&(klass->iface_signal_sigs));
}


GType tp_chan_get_type(void)
{
#ifndef EMULATOR
  static GType type1 = 0;
#endif
  
  if (type1 == 0)
  {
    static const GTypeInfo info =
    {
      sizeof(TpChanClass),
      NULL,
      NULL,
      (GClassInitFunc)tp_chan_class_init,
      NULL,
      NULL,
      sizeof(TpChan),
      0,
      (GInstanceInitFunc)tp_chan_init
    };
    type1 = g_type_register_static(DBUS_TYPE_G_PROXY,
        "TpChan", &info, 0);
  }
  return type1;
}



/* Public functions begin */

GQuark tp_get_chan_interface()
{
#ifndef EMULATOR
  static GQuark ret1 = 0;
#endif

  if (ret1 == 0)
  {
    ret1 = g_quark_from_static_string(TP_IFACE_CHANNEL_INTERFACE);
  }

  return ret1;
}

GQuark tp_get_chan_contactlist_interface()
{
#ifndef EMULATOR  
  static GQuark ret2 = 0;
#endif

  if (ret2 == 0)
  {
    ret2 = g_quark_from_static_string(TP_IFACE_CHANNEL_TYPE_CONTACT_LIST);
  }

  return ret2;
}


#ifdef SYMBIAN
EXPORT_C 
#endif
GQuark tp_get_chan_contactsearch_interface()
{
#ifndef EMULATOR
  static GQuark ret3 = 0;
#endif

  if (ret3 == 0)
  {
    ret3 = g_quark_from_static_string(TP_IFACE_CHANNEL_TYPE_CONTACT_SEARCH);
  }

  return ret3;
}


GQuark tp_get_chan_streamed_interface()
{
#ifndef EMULATOR
  static GQuark ret4 = 0;
#endif

  if (ret4 == 0)
  {
    ret4 = g_quark_from_static_string(TP_IFACE_CHANNEL_TYPE_STREAMED_MEDIA);
  }

  return ret4;
}


GQuark tp_get_chan_roomlist_interface()
{
#ifndef EMULATOR
  static GQuark ret5 = 0;
#endif

  if (ret5 == 0)
  {
    ret5 = g_quark_from_static_string(TP_IFACE_CHANNEL_TYPE_ROOM_LIST);
  }

  return ret5;
}

#ifdef SYMBIAN
EXPORT_C 
#endif
GQuark tp_get_chan_text_interface()
{
#ifndef EMULATOR
  static GQuark ret6 = 0;
#endif

  if (ret6 == 0)
  {
    ret6 = g_quark_from_static_string(TP_IFACE_CHANNEL_TYPE_TEXT);
  }

  return ret6;
}


GQuark tp_get_chan_dtmf_interface()
{
#ifndef EMULATOR
  static GQuark ret7 = 0;
#endif
  
  if (ret7 == 0)
  {
    ret7 = g_quark_from_static_string(TP_IFACE_CHANNEL_INTERFACE_DTMF);
  }

  return ret7;
}


#ifdef SYMBIAN
EXPORT_C 
#endif
GQuark tp_get_chan_group_interface()
{
#ifndef EMULATOR
  static GQuark ret8 = 0;
#endif

  if (ret8 == 0)
  {
    ret8 = g_quark_from_static_string(TP_IFACE_CHANNEL_INTERFACE_GROUP);
  }

  return ret8;
}


GQuark tp_get_chan_hold_interface()
{
#ifndef EMULATOR
  static GQuark ret9 = 0;
#endif

  if (ret9 == 0)
  {
    ret9 = g_quark_from_static_string(TP_IFACE_CHANNEL_INTERFACE_HOLD);
  }

  return ret9;
}


GQuark tp_get_chan_password_interface()
{
#ifndef EMULATOR
  static GQuark ret10 = 0;
#endif

  if (ret10 == 0)
  {
    ret10 = g_quark_from_static_string(TP_IFACE_CHANNEL_INTERFACE_PASSWORD);
  }

  return ret10;
}

GQuark tp_get_chan_transfer_interface()
{
#ifndef EMULATOR
  static GQuark ret11 = 0;
#endif

  if (ret11 == 0)
  {
    ret11 = g_quark_from_static_string(TP_IFACE_CHANNEL_INTERFACE_TRANSFER);
  }

  return ret11;
}


#ifdef SYMBIAN
EXPORT_C 
#endif
TpChan *tp_chan_new(DBusGConnection *connection, const gchar *bus_name,
                    const gchar *object_path, const gchar *type,
                    guint handle_type, guint handle)
{
  GError *error = NULL;
  gchar *unique_name;
  gchar **interfaces;
  TpChan *chan;
  g_return_val_if_fail(connection != NULL, NULL);
  g_return_val_if_fail(bus_name != NULL, NULL);
  g_return_val_if_fail(object_path != NULL, NULL);
  g_return_val_if_fail(type != NULL, NULL);

  if (!dbus_g_proxy_call (tp_get_bus_proxy (), 
                          "GetNameOwner", &error, G_TYPE_STRING, bus_name, 
                          G_TYPE_INVALID, G_TYPE_STRING, &unique_name, 
                          G_TYPE_INVALID))
  {
    g_warning("tp_chan_new: getting unique name failed: %s", error->message);
    g_error_free(error);
    return NULL;
  }

  /* Create the channel object */
  chan = g_object_new(TELEPATHY_CHAN_TYPE,
                      "name", unique_name, "path", object_path,
                      "interface", TP_IFACE_CHANNEL_INTERFACE,
                      "connection", connection, NULL);

  g_free(unique_name);

  dbus_g_proxy_add_signal(DBUS_G_PROXY(chan), "Closed", G_TYPE_INVALID);

  g_datalist_init(&(chan->interface_list));

  /* Store interface information for the channel */
  if (tp_chan_get_interfaces(DBUS_G_PROXY(chan), &interfaces, &error))
  {
    tp_chan_local_set_interfaces(chan, interfaces);

    /* Free the strings used for interface object creation */
    g_strfreev(interfaces);
  }
  else
  {
    g_warning("GetInterfaces for channel failed: %s\n", error->message);
    g_error_free(error);
  }

  /* Store necessary information for this object */
  chan->type = g_strdup(type);
  chan->handle_type = handle_type;
  chan->handle = handle;

  return chan;
}


void tp_chan_local_set_interfaces(TpChan *self, gchar **interfaces)
{
  gchar **temp_ifaces;
  gchar *chan_type = NULL;
  GError *error = NULL;
  GData **sig_list = &(TELEPATHY_CHAN_GET_CLASS(self)->iface_signal_sigs);
  void (*signature_setter_func)(DBusGProxy *proxy);
  DBusGConnection *connection;
  /*const*/ gchar *name, *path;

  if (interfaces == NULL)
  {
    return;
  }

  /* Create and store proxy objects corresponding to the
     interfaces */

  g_object_get (G_OBJECT(self),
                "connection", &connection,
                "name", &name,
                "path", &path,
                NULL);

  g_debug ("%s: %p, %s, %s", G_STRFUNC, connection, name, path);

  for (temp_ifaces = interfaces; *temp_ifaces; temp_ifaces++)
    {
      GQuark key = g_quark_from_string(*temp_ifaces);
      DBusGProxy *if_proxy;

      if (key == TELEPATHY_PROPS_IFACE_QUARK)
        {
          if_proxy = DBUS_G_PROXY (tp_props_iface_new (connection, name, path));
        }
      else
        {
          if_proxy = dbus_g_proxy_new_for_name (connection, name,
                                                path, *temp_ifaces);

          if (if_proxy != NULL)
            {
              /* Does the interface have signals? If yes, add their signatures
                 for the interface instance by calling the
                 corresponding setter function */

              signature_setter_func =
                g_datalist_id_get_data(sig_list, key);

              if (signature_setter_func != NULL)
                {
                  (*signature_setter_func)(if_proxy);
                }
            }
        }
      if (if_proxy != NULL)
        {
          g_datalist_id_set_data_full(&(self->interface_list), key,
              if_proxy, g_object_unref);
        }
    }

  /* Finally, add the channel type interface */

  if (!tp_chan_get_channel_type(DBUS_G_PROXY(self), &chan_type, &error))
  {
    g_warning("GetChannelType failed: %s\n", error->message);
    g_error_free(error);
  }
  else
  {
    DBusGProxy *chan_proxy =
      dbus_g_proxy_new_from_proxy(DBUS_G_PROXY(self), chan_type, NULL);

    g_datalist_id_set_data(&(self->interface_list),
        g_quark_from_string(chan_type), chan_proxy);

    /* If the particular channel type interface has signals defined,
       call the corresponding setter function */

    signature_setter_func =
      g_datalist_id_get_data(sig_list, g_quark_from_string(chan_type));

    if (signature_setter_func != NULL)
    {
      (*signature_setter_func)(chan_proxy);
    }

    g_free(chan_type);
  }

  g_free (name);
  g_free (path);
  dbus_g_connection_unref (connection);
}

#ifdef SYMBIAN
EXPORT_C 
#endif
DBusGProxy *tp_chan_get_interface(TpChan *self, GQuark iface_quark)
{
  DBusGProxy *iface_proxy = NULL;

  iface_proxy = (DBusGProxy *)g_datalist_id_get_data(&(self->interface_list),
						     iface_quark);
  return iface_proxy;
}

static void synthesize_closed(TpChan *chan)
{
  DBusMessage *msg = NULL;
  GArray *closed_signal_types = g_array_new(FALSE, FALSE, sizeof(GType));

  if (!closed_signal_types)
  {
    g_warning("%s: Could not allocate the type array for Closed signal",
              G_STRFUNC);
    return;
  }

  msg = dbus_message_new_signal(dbus_g_proxy_get_path(DBUS_G_PROXY(chan)),
                            TP_IFACE_CHANNEL_INTERFACE, "Closed");
  if (!msg)
  {
    g_warning("%s: Could not create the synthetic Closed signal message.",
              G_STRFUNC);
    g_array_free(closed_signal_types, FALSE);
    return;
  }
  g_signal_emit_by_name(DBUS_G_PROXY(chan),
                        TP_IFACE_CHAN_SIGNAL_CLOSED_SYNTHESIZED, msg,
                        closed_signal_types);
  g_array_free(closed_signal_types, FALSE);
  dbus_message_unref(msg);
}
