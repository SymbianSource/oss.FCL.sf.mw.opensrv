/* tp-chan.h
 *
 * 
 * Copyright (C) 2005 Collabora Ltd.
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

#ifndef TP_CHAN_H
#define TP_CHAN_H

#ifndef DBUS_API_SUBJECT_TO_CHANGE
#define DBUS_API_SUBJECT_TO_CHANGE
#endif


#include "tp-chan-gen.h"
#include "tp-interfaces.h"
#include "tp-chan-signals-marshal.h"
#include "tp-ifaces-signals-marshal.h"
#include "tp-chan-ifaces-signal-setters.h"

#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>


#define TELEPATHY_CHAN_TYPE       (tp_chan_get_type ())

#define TELEPATHY_CHAN(obj)       (G_TYPE_CHECK_INSTANCE_CAST \
		                      ((obj), TELEPATHY_CHAN_TYPE, \
				       TpChan))

#define TELEPATHY_CHAN_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST \
		                              ((klass), TELEPATHY_CHAN_TYPE, \
					       TpChanClass))

#define TELEPATHY_IS_CHAN(obj)    (G_TYPE_CHECK_INSTANCE_TYPE \
		                      ((obj), TELEPATHY_CHAN_TYPE))

#define TELEPATHY_IS_CHAN_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE \
		                               ((klass), TELEPATHY_CHAN_TYPE))

#define TELEPATHY_CHAN_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS \
		                              ((obj), TELEPATHY_CHAN_TYPE, \
						TpChanClass))

#define TELEPATHY_CHAN_IFACE_QUARK (tp_get_chan_interface())
#define TELEPATHY_CHAN_IFACE_CONTACTLIST_QUARK (tp_get_chan_contactlist_interface())
#define TELEPATHY_CHAN_IFACE_CONTACTSEARCH_QUARK (tp_get_chan_contactsearch_interface())
#define TELEPATHY_CHAN_IFACE_STREAMED_QUARK (tp_get_chan_streamed_interface())
#define TELEPATHY_CHAN_IFACE_ROOMLIST_QUARK (tp_get_chan_roomlist_interface())
#define TELEPATHY_CHAN_IFACE_TEXT_QUARK (tp_get_chan_text_interface())
#define TELEPATHY_CHAN_IFACE_DTMF_QUARK (tp_get_chan_dtmf_interface())
#define TELEPATHY_CHAN_IFACE_GROUP_QUARK (tp_get_chan_group_interface())
#define TELEPATHY_CHAN_IFACE_HOLD_QUARK (tp_get_chan_hold_interface())
#define TELEPATHY_CHAN_IFACE_PASSWORD_QUARK (tp_get_chan_password_interface())
#define TELEPATHY_CHAN_IFACE_TRANSFER_QUARK (tp_get_chan_transfer_interface())

/* The signal name for the synthesized Closed signal */
#define TP_IFACE_CHAN_SIGNAL_CLOSED_SYNTHESIZED \
        "received::org-freedesktop-Telepathy-Channel-Closed"

typedef struct _tp_chan TpChan;
typedef struct _tp_chanclass TpChanClass;

struct _tp_chan
{
  DBusGProxy parent;
  gchar *type;
  guint handle_type;
  guint handle;
  GData *interface_list;
  gboolean first_run;
};

struct _tp_chanclass
{
  DBusGProxyClass parent_class;
  GData *iface_signal_sigs;
};

GType tp_chan_get_type(void);

/* Actual function definitions */


/*
 * Creates a new channel object.
 *
 * @param connection: The D-BUS connection for this channel.
 * @param bus_name: The D-BUS bus name for this channel.
 * @param object_name: The name of the D-BUS path for this channel.
 * @param type: The type string for this channel.
 * @param handle_type: The type of this channel.
 * @param handle: The handle of this channel, will be filled by this call.
 * @return A new TpChan (channel) object, or NULL if unsuccesfull
 */
#ifdef __cplusplus
extern "C"
{
#endif
#ifdef SYMBIAN
IMPORT_C 
#endif
TpChan *tp_chan_new(DBusGConnection *connection, const gchar *bus_name,
		    const gchar *object_path, const gchar *type,
                    guint handle_type, guint handle);
#ifdef __cplusplus
}
#endif


/* Create and store the D-BUS proxy objects for the interfaces provided
 * by this channel. Expects data provided by the GetInterfaces method.
 * 
 * @param self: The channel object whose interfaces will be stored.
 * @param interfaces: The array of strings containing the interface names
 *
 */


void tp_chan_local_set_interfaces(TpChan *self, gchar **interfaces);


/*
 * This function checks whether the channel has the specified
 * interface and returns a proxy object that can be used to call
 * its methods, if it exists. The supported interfaces are listed
 * in the beginning of this header as macros.
 *
 * @param self The channel object that is queried for the interface
 *             proxy object
 * @param iface_quark GQuark corresponding to the interface name
 *        string.
 * @return A DBusGProxy object for the interface, or NULL if not found
 */
#ifdef __cplusplus
extern "C"
{
#endif
#ifdef SYMBIAN
IMPORT_C 
#endif 
DBusGProxy *tp_chan_get_interface(TpChan *self, GQuark iface_quark);

#ifdef __cplusplus
}
#endif


GQuark tp_get_chan_interface(void);
GQuark tp_get_chan_contactlist_interface(void);

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef SYMBIAN
IMPORT_C 
#endif 
GQuark tp_get_chan_contactsearch_interface(void);
#ifdef __cplusplus
}
#endif
GQuark tp_get_chan_streamed_interface(void);
GQuark tp_get_chan_roomlist_interface(void);

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef SYMBIAN
IMPORT_C 
#endif 
GQuark tp_get_chan_group_interface(void);
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
extern "C"
{
#endif
#ifdef SYMBIAN
IMPORT_C 
#endif 
GQuark tp_get_chan_text_interface(void);
GQuark tp_get_chan_dtmf_interface(void);
GQuark tp_get_chan_hold_interface(void);
GQuark tp_get_chan_password_interface(void);
GQuark tp_get_chan_transfer_interface(void);
#ifdef __cplusplus
}
#endif
#endif

