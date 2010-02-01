/* tp-conn-ifaces-signal-setters.h
 *
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

#ifndef TP_CONN_IFACES_SETTERS_H
#define TP_CONN_IFACES_SETTERS_H

#include <dbus/dbus-glib.h>

/* This header file contains definitions for the signal signature setters
 * for the various interfaces provided by Connections */

void tp_conn_set_aliasing_signatures(DBusGProxy *proxy);

void tp_conn_set_capabilities_signatures(DBusGProxy *proxy);

void tp_conn_set_contactinfo_signatures(DBusGProxy *proxy);

void tp_conn_set_forwarding_signatures(DBusGProxy *proxy);

void tp_conn_set_presence_signatures(DBusGProxy *proxy);

void tp_conn_set_privacy_signatures(DBusGProxy *proxy);

void tp_conn_set_renaming_signatures(DBusGProxy *proxy);

void tp_conn_set_avatar_signatures(DBusGProxy *proxy);
#endif
