/*
 * gabble-connection-manager.h - Header for GabbleConnectionManager
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

#ifndef __GABBLE_CONNECTION_MANAGER_H__
#define __GABBLE_CONNECTION_MANAGER_H__

#include <glib-object.h>
//vinod
#include <dbus/dbus-glib.h>
#include <dbus/dbus-protocol.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gabble-connection.h"
#include "telepathy-constants.h"
#include "telepathy-errors.h"
#include "telepathy-helpers.h"
//end
#define GABBLE_CONN_MGR_BUS_NAME        "org.freedesktop.Telepathy.ConnectionManager.gabble"
#define GABBLE_CONN_MGR_OBJECT_PATH     "/org/freedesktop/Telepathy/ConnectionManager/gabble"

G_BEGIN_DECLS

typedef struct _GabbleConnectionManager GabbleConnectionManager;
typedef struct _GabbleConnectionManagerClass GabbleConnectionManagerClass;

struct _GabbleConnectionManagerClass {
    GObjectClass parent_class;
};

struct _GabbleConnectionManager {
    GObject parent;

    gpointer priv;
};

typedef struct {
    const gchar *name;          /* name as passed over dbus */
    const gchar *dtype;         /* D-Bus type string */
    const GType gtype;          /* glib type string */
    guint flags;                /* combination of TP_CONN_MGR_PARAM_FLAG_foo */
    const gpointer def;         /* default - gchar * or GINT_TO_POINTER */
    const gsize offset;         /* internal use only */
} GabbleParamSpec;

typedef struct {
    const gchar *name;
    const GabbleParamSpec *parameters;       /* terminated by a NULL name */
} GabbleProtocolSpec;

//start: vinod
/* private data */

typedef struct _GabbleParams GabbleParams;

struct _GabbleParams {
  guint set_mask;

  gchar *account;
  gchar *password;
  gchar *server;
  gchar *resource;
  gint priority;
  guint port;
  gboolean old_ssl;
  gboolean do_register;
  gboolean low_bandwidth;
  gchar *https_proxy_server;
  guint https_proxy_port;
  gchar *fallback_conference_server;
  gchar *stun_server;
  guint stun_port;
  gboolean ignore_ssl_errors;
  gchar *alias;
};

//const GabbleProtocolSpec *gabble_protocols; /* terminated by a NULL name */
static const GabbleParamSpec jabber_params[] = {
  { "account", DBUS_TYPE_STRING_AS_STRING, G_TYPE_STRING, TP_CONN_MGR_PARAM_FLAG_REQUIRED | TP_CONN_MGR_PARAM_FLAG_REGISTER, NULL, G_STRUCT_OFFSET(GabbleParams, account) },
  { "password", DBUS_TYPE_STRING_AS_STRING, G_TYPE_STRING, TP_CONN_MGR_PARAM_FLAG_REQUIRED | TP_CONN_MGR_PARAM_FLAG_REGISTER, NULL, G_STRUCT_OFFSET(GabbleParams, password) },
  { "server", DBUS_TYPE_STRING_AS_STRING, G_TYPE_STRING, 0, NULL, G_STRUCT_OFFSET(GabbleParams, server) },
  { "resource", DBUS_TYPE_STRING_AS_STRING, G_TYPE_STRING, TP_CONN_MGR_PARAM_FLAG_HAS_DEFAULT, GABBLE_PARAMS_DEFAULT_RESOURCE, G_STRUCT_OFFSET(GabbleParams, resource) },
  { "priority", DBUS_TYPE_INT16_AS_STRING, G_TYPE_INT, TP_CONN_MGR_PARAM_FLAG_HAS_DEFAULT, GINT_TO_POINTER(0), G_STRUCT_OFFSET(GabbleParams, priority) },
  { "port", DBUS_TYPE_UINT16_AS_STRING, G_TYPE_UINT, TP_CONN_MGR_PARAM_FLAG_HAS_DEFAULT, GINT_TO_POINTER(GABBLE_PARAMS_DEFAULT_PORT), G_STRUCT_OFFSET(GabbleParams, port) },
  { "old-ssl", DBUS_TYPE_BOOLEAN_AS_STRING, G_TYPE_BOOLEAN, TP_CONN_MGR_PARAM_FLAG_HAS_DEFAULT, GINT_TO_POINTER(FALSE), G_STRUCT_OFFSET(GabbleParams, old_ssl) },
  { "register", DBUS_TYPE_BOOLEAN_AS_STRING, G_TYPE_BOOLEAN, TP_CONN_MGR_PARAM_FLAG_HAS_DEFAULT, GINT_TO_POINTER(FALSE), G_STRUCT_OFFSET(GabbleParams, do_register) },
  { "low-bandwidth", DBUS_TYPE_BOOLEAN_AS_STRING, G_TYPE_BOOLEAN, TP_CONN_MGR_PARAM_FLAG_HAS_DEFAULT, GINT_TO_POINTER(FALSE), G_STRUCT_OFFSET(GabbleParams, low_bandwidth) },
  { "https-proxy-server", DBUS_TYPE_STRING_AS_STRING, G_TYPE_STRING, 0, NULL, G_STRUCT_OFFSET(GabbleParams, https_proxy_server) },
  { "https-proxy-port", DBUS_TYPE_UINT16_AS_STRING, G_TYPE_UINT, TP_CONN_MGR_PARAM_FLAG_HAS_DEFAULT, GINT_TO_POINTER(GABBLE_PARAMS_DEFAULT_HTTPS_PROXY_PORT), G_STRUCT_OFFSET(GabbleParams, https_proxy_port) },
  { "fallback-conference-server", DBUS_TYPE_STRING_AS_STRING, G_TYPE_STRING, 0, NULL, G_STRUCT_OFFSET(GabbleParams, fallback_conference_server) },
  { "stun-server", DBUS_TYPE_STRING_AS_STRING, G_TYPE_STRING, 0, NULL, G_STRUCT_OFFSET(GabbleParams, stun_server) },
  { "stun-port", DBUS_TYPE_UINT16_AS_STRING, G_TYPE_UINT, TP_CONN_MGR_PARAM_FLAG_HAS_DEFAULT, GINT_TO_POINTER(GABBLE_PARAMS_DEFAULT_STUN_PORT), G_STRUCT_OFFSET(GabbleParams, stun_port) },
  { "ignore-ssl-errors", DBUS_TYPE_BOOLEAN_AS_STRING, G_TYPE_BOOLEAN, TP_CONN_MGR_PARAM_FLAG_HAS_DEFAULT, GINT_TO_POINTER(FALSE), G_STRUCT_OFFSET(GabbleParams, ignore_ssl_errors) },
  { "alias", DBUS_TYPE_STRING_AS_STRING, G_TYPE_STRING, 0, NULL, G_STRUCT_OFFSET(GabbleParams, alias) },
  { NULL, NULL, 0, 0, NULL, 0 }
};

static const GabbleProtocolSpec _gabble_protocols[] = {
  { "jabber", jabber_params },
  //{ "jabber", jabber_params }
  { NULL, NULL } //vinod
  
};

const GabbleProtocolSpec *gabble_protocols = _gabble_protocols;

//end: vinod

IMPORT_C GType gabble_connection_manager_get_type(void);

/* TYPE MACROS */
#define GABBLE_TYPE_CONNECTION_MANAGER \
  (gabble_connection_manager_get_type())
#define GABBLE_CONNECTION_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), GABBLE_TYPE_CONNECTION_MANAGER, GabbleConnectionManager))
#define GABBLE_CONNECTION_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass), GABBLE_TYPE_CONNECTION_MANAGER, GabbleConnectionManagerClass))
#define GABBLE_IS_CONNECTION_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj), GABBLE_TYPE_CONNECTION_MANAGER))
#define GABBLE_IS_CONNECTION_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass), GABBLE_TYPE_CONNECTION_MANAGER))
#define GABBLE_CONNECTION_MANAGER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GABBLE_TYPE_CONNECTION_MANAGER, GabbleConnectionManagerClass))

#ifdef SYMBIAN
IMPORT_C
#endif
void _gabble_connection_manager_register (GabbleConnectionManager *self);

gboolean
gabble_connection_manager_get_parameters (GabbleConnectionManager *self,
                                          const gchar *proto,
                                          GPtrArray **ret,
                                          GError **error);

gboolean
gabble_connection_manager_list_protocols (GabbleConnectionManager *self,
                                          gchar ***ret,
                                          GError **error);

gboolean
gabble_connection_manager_request_connection (GabbleConnectionManager *self,
                                              const gchar *proto,
                                              GHashTable *parameters,
                                              gchar **ret,
                                              gchar **ret1,
                                              GError **error);



G_END_DECLS

#endif /* #ifndef __GABBLE_CONNECTION_MANAGER_H__*/
