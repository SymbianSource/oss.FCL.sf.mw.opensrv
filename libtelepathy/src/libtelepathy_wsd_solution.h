/* libtelepathy_wsd_solution.h part of libtelepathy
 *
 * 
 * Copyright (C) 2007 Nokia Corporation.
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


#ifndef _LIBTELEPATHY_WSD_H
#define _LIBTELEPATHY_WSD_H
#include "libtelepathy_wsd_macros.h"

#include <glib/ghash.h>
#include <glib/gquark.h>
#include <glib-object.h>
#include "glibconfig.h"
#include <dbus/dbus.h>

#include <gobject/gvaluecollector.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>


#define LAST_SIGNAL_TP_PROPS_IFACE 2

#if EMULATOR
#ifdef __cplusplus
extern "C" 
{
#endif

struct libtelepathy_global_struct
{

	VARIABLE_DECL(parent_class,s,tp_chan,GObjectClass *)
	VARIABLE_DECL(type1,s,tp_chan,GType)
	VARIABLE_DECL(ret1,s,tp_chan,GQuark)
	VARIABLE_DECL(ret2,s,tp_chan,GQuark)
	VARIABLE_DECL(ret3,s,tp_chan,GQuark)
	VARIABLE_DECL(ret4,s,tp_chan,GQuark)
	VARIABLE_DECL(ret5,s,tp_chan,GQuark)
	VARIABLE_DECL(ret6,s,tp_chan,GQuark)
	VARIABLE_DECL(ret7,s,tp_chan,GQuark)
	VARIABLE_DECL(ret8,s,tp_chan,GQuark)
	VARIABLE_DECL(ret9,s,tp_chan,GQuark)
	VARIABLE_DECL(ret10,s,tp_chan,GQuark)
	VARIABLE_DECL(ret11,s,tp_chan,GQuark)
	
	VARIABLE_DECL(parent_class,s,tp_conn,GObjectClass *)
	VARIABLE_DECL(type1,s,tp_conn,GType)
	VARIABLE_DECL(ret1,s,tp_conn,GQuark)
	VARIABLE_DECL(ret2,s,tp_conn,GQuark)
	VARIABLE_DECL(ret3,s,tp_conn,GQuark)
	VARIABLE_DECL(ret4,s,tp_conn,GQuark)
	VARIABLE_DECL(ret5,s,tp_conn,GQuark)
	VARIABLE_DECL(ret6,s,tp_conn,GQuark)
	VARIABLE_DECL(ret7,s,tp_conn,GQuark)
	VARIABLE_DECL(ret8,s,tp_conn,GQuark)
	VARIABLE_DECL(ret9,s,tp_conn,GQuark)
	
	VARIABLE_DECL(parent_class,s,tp_connmgr,GObjectClass *)
	VARIABLE_DECL(type1,s,tp_connmgr,GType)
	
	VARIABLE_DECL(bus_proxy,s,tp_helpers,DBusGProxy *)
	VARIABLE_DECL(bus1,s,tp_helpers,DBusGConnection *)
	
	VARIABLE_DECL(parent_class,s,tp_props_iface,GObjectClass *)
	VARIABLE_DECL(type1,s,tp_props_iface,GType)
	VARIABLE_DECL(ret,s,tp_props_iface,GQuark)
	VARIABLE_DECL_ARRAY(signals,s,tp_props_iface,guint,LAST_SIGNAL_TP_PROPS_IFACE)

      /*END-global vars*/
 
    };

struct libtelepathy_global_struct * libtelepathy_ImpurePtr();
int libtelepathy_Init(struct libtelepathy_global_struct *);
#ifdef __cplusplus
}
#endif
#endif //EMULATOR
#endif //header guard _LIBTELEPATHY_WSD_H

