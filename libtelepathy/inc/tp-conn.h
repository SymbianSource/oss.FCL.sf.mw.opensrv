/* tp-conn.h
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


#ifndef TP_CONN_H
#define TP_CONN_H

#ifndef DBUS_API_SUBJECT_TO_CHANGE
#define DBUS_API_SUBJECT_TO_CHANGE
#endif

#include "tp-conn-gen.h"
#include "tp-conn-iface-forwarding-gen.h"
#include "tp-chan.h"
#include "tp-conn-signals-marshal.h"
#include "tp-ifaces-signals-marshal.h"
#include "tp-conn-ifaces-signal-setters.h"
#include "tp-interfaces.h"
#include "tp-constants.h"

#include <dbus/dbus-glib.h>

#define DBUS_TYPE_G_ARRAY_OF_STRUCTS dbus_g_type_get_collection ("GPtrArray", G_TYPE_VALUE_ARRAY);

#define TELEPATHY_CONN_TYPE  (tp_conn_get_type ())

#define TELEPATHY_CONN(obj)  (G_TYPE_CHECK_INSTANCE_CAST \
		                    ((obj), TELEPATHY_CONN_TYPE, \
				     TpConn))

#define TELEPATHY_CONN_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST \
		                          ((klass), TELEPATHY_CONN_TYPE, \
					  TpConnClass))

#define TELEPATHY_IS_CONN(obj)    (G_TYPE_CHECK_INSTANCE_TYPE \
		                      ((obj), TELEPATHY_CONN_TYPE))

#define TELEPATHY_IS_CONN_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE \
		                           ((klass), TELEPATHY_CONN_TYPE))

#define TELEPATHY_CONN_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS \
		                            ((obj), TELEPATHY_CONN_TYPE, \
					    TpConnClass))

#define TELEPATHY_CONN_IFACE_QUARK (tp_get_conn_interface())
#define TELEPATHY_CONN_IFACE_ALIASING_QUARK (tp_get_conn_aliasing_interface())
#define TELEPATHY_CONN_IFACE_CAPABILITIES_QUARK (tp_get_conn_capabilities_interface())
#define TELEPATHY_CONN_IFACE_CONTACTINFO_QUARK (tp_get_conn_contactinfo_interface())
#define TELEPATHY_CONN_IFACE_FORWARDING_QUARK (tp_get_conn_forwarding_interface())
#define TELEPATHY_CONN_IFACE_PRESENCE_QUARK (tp_get_conn_presence_interface())
#define TELEPATHY_CONN_IFACE_PRIVACY_QUARK (tp_get_conn_privacy_interface())
#define TELEPATHY_CONN_IFACE_RENAMING_QUARK (tp_get_conn_renaming_interface())
#define TELEPATHY_CONN_IFACE_AVATAR_QUARK (tp_get_conn_avatar_interface())


/* The signal name for the synthesized StatusChanged signal */
#define TP_IFACE_CONN_SIGNAL_STATUSCHANGED_SYNTHESIZED \
        "received::org-freedesktop-Telepathy-Connection-StatusChanged"

typedef struct _tp_conn TpConn;
typedef struct _tp_connclass TpConnClass;

struct _tp_conn
{
  DBusGProxy parent;
  GData *interface_list;
  gboolean first_run;
};


struct _tp_connclass
{
  DBusGProxyClass parent_class;
  GData *iface_signal_sigs;
};

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef SYMBIAN
IMPORT_C
#endif
GType tp_conn_get_type(void);
#ifdef __cplusplus
}
#endif

/* Actual function definitions */

/*
 * Create a new TpConn (Connection) object.
 *
 * @param connection The D-BUS connection
 * @param name: The D-BUS service name for the connection object
 * @param path_name: The D-BUS path name for the connection object
 * @param interface_name: The D-BUS interface name for the connection object
 * @return A new TpConn connection object, or NULL if unsuccesful.
 */

TpConn *tp_conn_new(DBusGConnection *connection,
		    const char      *bus_name,
		    const char      *object_path);

/* Create a TpChan channel object for this connection. Also initialises
 * the proxy object that is used for the actual method calls.
 *
 * @param connection: The DBusGConnection for the channel
 * @param tpconn:  The Connection object for which the channel is done
 * @param bus_name:  The D-Bus interface name for the channel
 * @param type: The channel type string
 * @param handle_type: The type of the channel handle
 * @param handle: The channel handle value is set and stored here after
 * @param         the call returns.
 * @param supress_handler: if TRUE, indicates that no handler needs to be
 *                         launched
 *
 * @return A new TpChan (channel) object
 */ 

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef SYMBIAN
IMPORT_C 
#endif 
TpChan *tp_conn_new_channel(DBusGConnection *connection, TpConn *tpconn,
			    const gchar *bus_name,
			    gchar *type, guint handle_type,
			    guint handle, gboolean supress_handler);
#ifdef __cplusplus
}
#endif

/* Creates the proxy objects for the channel interfaces. GetInterfaces
 * method should be called before this to acquire the interface name
 * list.
 * 
 * @param self: The connection object
 * @param interfaces: An array of the interface name strings
 */

void tp_conn_local_set_interfaces(TpConn *self, gchar **interfaces);


/*
 * Provides access to the connection proxy object to use for actual
 * method calls
 *
 * @param self: The connection object whose proxy should be retrieved
 * @return The D-BUS proxy object
 */

DBusGProxy *tp_conn_get_proxy(TpConn *self);

/*
 * This function checks whether the Connection has the specified
 * interface and returns a proxy object that can be used to call
 * its methods, if it exists. The supported interfaces are listed
 * in the beginning of this header as macros.
 *
 * @param self The connection object that is queried for the
 *             interface proxy object
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
DBusGProxy *tp_conn_get_interface(TpConn *self, GQuark iface_quark);
#ifdef __cplusplus
}
#endif

GQuark tp_get_conn_interface(void);
GQuark tp_get_conn_aliasing_interface(void);
GQuark tp_get_conn_capabilities_interface(void);
GQuark tp_get_conn_contactinfo_interface(void);
GQuark tp_get_conn_forwarding_interface(void);
#ifdef __cplusplus
extern "C"
{
#endif
#ifdef SYMBIAN
IMPORT_C
#endif
GQuark tp_get_conn_presence_interface(void);
GQuark tp_get_conn_avatar_interface(void);
#ifdef __cplusplus
}
#endif
GQuark tp_get_conn_privacy_interface(void);
GQuark tp_get_conn_renaming_interface(void);



#endif
