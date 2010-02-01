/*
 * tp-conn-signals-marshal.h - Header for Telepathy interface names
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
 
#ifndef __tp_conn_signals_marshal_MARSHAL_H__
#define __tp_conn_signals_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:OBJECT,STRING,UINT,UINT,BOOLEAN (tp-conn-signals-marshal.list:1) */
extern void tp_conn_signals_marshal_VOID__OBJECT_STRING_UINT_UINT_BOOLEAN (GClosure     *closure,
                                                                           GValue       *return_value,
                                                                           guint         n_param_values,
                                                                           const GValue *param_values,
                                                                           gpointer      invocation_hint,
                                                                           gpointer      marshal_data);

/* VOID:UINT,UINT (tp-conn-signals-marshal.list:2) */
extern void tp_conn_signals_marshal_VOID__UINT_UINT (GClosure     *closure,
                                                     GValue       *return_value,
                                                     guint         n_param_values,
                                                     const GValue *param_values,
                                                     gpointer      invocation_hint,
                                                     gpointer      marshal_data);

/* VOID:BOXED (tp-conn-signals-marshal.list:3) */
#define tp_conn_signals_marshal_VOID__BOXED	g_cclosure_marshal_VOID__BOXED

G_END_DECLS

#endif /* __tp_conn_signals_marshal_MARSHAL_H__ */

