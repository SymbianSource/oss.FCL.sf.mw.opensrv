/* tp-chan-ifaces-signal-setters.h
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

#ifndef TP_CHAN_IFACES_SETTERS_H
#define TP_CHAN_IFACES_SETTERS_H

#include <dbus/dbus-glib.h>

/* This header file contains definitions for the signal signature setters
 * for the various interfaces provided by Channel */


void tp_chan_set_contactsearch_signatures(DBusGProxy *proxy);

void tp_chan_set_streamedmedia_signatures(DBusGProxy *proxy);

void tp_chan_set_roomlist_signatures(DBusGProxy *proxy);

void tp_chan_set_text_signatures(DBusGProxy *proxy);

void tp_chan_set_dtmf_signatures(DBusGProxy *proxy);

void tp_chan_set_group_signatures(DBusGProxy *proxy);

void tp_chan_set_hold_signatures(DBusGProxy *proxy);

void tp_chan_set_password_signatures(DBusGProxy *proxy);

void tp_chan_set_icesignalling_signatures(DBusGProxy *proxy);

#endif
