/* tp-connmgr.h
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



#ifndef TP_CONNMGR_H
#define TP_CONNMGR_H

#ifndef DBUS_API_SUBJECT_TO_CHANGE
#define DBUS_API_SUBJECT_TO_CHANGE
#endif

#include "tp-connmgr-gen.h"
#include "tp-conn.h"
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus.h>
#include <glib.h>



#define TELEPATHY_CONNMGR_TYPE  (tp_connmgr_get_type ())

#define TELEPATHY_CONNMGR(obj)  (G_TYPE_CHECK_INSTANCE_CAST \
		                    ((obj), TELEPATHY_CONNMGR_TYPE, \
				     TpConnMgr))

#define TELEPATHY_CONNMGR_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST \
		                          ((klass), TELEPATHY_CONNMGR_TYPE, \
					  TpConnMgrClass))

#define TELEPATHY_IS_CONNMGR(obj)    (G_TYPE_CHECK_INSTANCE_TYPE \
		                      ((obj), TELEPATHY_CONNMGR_TYPE))

#define TELEPATHY_IS_CONNMGR_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE \
		                           ((klass), TELEPATHY_CONNMGR_TYPE))

#define TELEPATHY_CONNMGR_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS \
		                            ((obj), TELEPATHY_CONNMGR_TYPE, \
					    TpConnMgrClass))

typedef struct _tp_connmgr TpConnMgr;
typedef struct _tp_connmgrclass TpConnMgrClass;

struct _tp_connmgr
{
  DBusGProxy parent;

  gboolean first_run;
};

struct _tp_connmgrclass
{
  DBusGProxyClass parent_class;
};

GType tp_connmgr_get_type(void);

/* Actual function definitions */


/*
* Creates a new TpConnmgr (Connection Manager) object.
* @param connection: A connection to the D-BUS.
* @param name: The service name for the connection manager
* @param path_name: The path name for the connection manager
* @param interface_name: The interface name for the connection manager
* @return A new Connection Manager object, or NULL if failed
*/

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef SYMBIAN
IMPORT_C 
#endif 
TpConnMgr *tp_connmgr_new(DBusGConnection *connection,
			const char *bus_name,
			const char *object_path,
			const char *interface_name);	
 #ifdef __cplusplus
}
#endif



/* 
 * Creates a new TpConn object for this connection manager.
 * Also initializes a DBusGProxy object that is used to perform the
 * actual method calls. Thus, the connection should be usable
 * if this succeeds.
 * 
 * @param self: The connection manager object that the new connection
 * object is created for.  
 * @param connection_parameters: The parameters for the connection.
 * @protocol: The protocol for the connection.
 *
 * @return A TpConn object, or NULL if object could not be created.
 */


#ifdef __cplusplus
extern "C"
{
#endif
#ifdef SYMBIAN
IMPORT_C 
#endif 
TpConn *tp_connmgr_new_connection(TpConnMgr *self,
				  GHashTable *connection_parameters,
				  gchar *protocol);
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
gboolean tp_connmgr_list_protocols(TpConnMgr *self, char *** proto);
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
gboolean tp_connmgr_get_parameters (TpConnMgr *self, const char * IN_proto, GPtrArray** OUT_arg1);
#ifdef __cplusplus
}
#endif




#endif

