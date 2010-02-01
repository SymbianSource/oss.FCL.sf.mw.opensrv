/*
 * tp-ifaces-signals-marshal.h - Header for Telepathy interface names
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
 
#ifndef __tp_ifaces_signals_marshal_MARSHAL_H__
#define __tp_ifaces_signals_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:UINT,STRING (tp-ifaces-signals-marshal.list:3) */
extern void tp_ifaces_signals_marshal_VOID__UINT_STRING (GClosure     *closure,
                                                         GValue       *return_value,
                                                         guint         n_param_values,
                                                         const GValue *param_values,
                                                         gpointer      invocation_hint,
                                                         gpointer      marshal_data);

/* VOID:UINT,BOXED,BOXED (tp-ifaces-signals-marshal.list:5) */
extern void tp_ifaces_signals_marshal_VOID__UINT_BOXED_BOXED (GClosure     *closure,
                                                              GValue       *return_value,
                                                              guint         n_param_values,
                                                              const GValue *param_values,
                                                              gpointer      invocation_hint,
                                                              gpointer      marshal_data);

/* VOID:STRING (tp-ifaces-signals-marshal.list:7) */
#define tp_ifaces_signals_marshal_VOID__STRING	g_cclosure_marshal_VOID__STRING

/* VOID:INT,BOXED (tp-ifaces-signals-marshal.list:11) */
extern void tp_ifaces_signals_marshal_VOID__INT_BOXED (GClosure     *closure,
                                                       GValue       *return_value,
                                                       guint         n_param_values,
                                                       const GValue *param_values,
                                                       gpointer      invocation_hint,
                                                       gpointer      marshal_data);


/* VOID:STRING,BOXED */
//Added for search
extern void
tp_ifaces_signals_marshal_VOID__UINT_BOXED (GClosure     *closure,
                                           GValue       *return_value,
                                           guint         n_param_values,
                                           const GValue *param_values,
                                           gpointer      invocation_hint,
                                           gpointer      marshal_data);
                                           
/* VOID:UINT (tp-ifaces-signals-marshal.list:13) */
#define tp_ifaces_signals_marshal_VOID__UINT	g_cclosure_marshal_VOID__UINT

/* VOID:UINT,STRING,STRING (tp-ifaces-signals-marshal.list:15) */
extern void tp_ifaces_signals_marshal_VOID__UINT_STRING_STRING (GClosure     *closure,
                                                                GValue       *return_value,
                                                                guint         n_param_values,
                                                                const GValue *param_values,
                                                                gpointer      invocation_hint,
                                                                gpointer      marshal_data);

/* VOID:UINT,UINT,UINT (tp-ifaces-signals-marshal.list:17) */
extern void tp_ifaces_signals_marshal_VOID__UINT_UINT_UINT (GClosure     *closure,
                                                            GValue       *return_value,
                                                            guint         n_param_values,
                                                            const GValue *param_values,
                                                            gpointer      invocation_hint,
                                                            gpointer      marshal_data);

/* VOID:BOOLEAN (tp-ifaces-signals-marshal.list:19) */
#define tp_ifaces_signals_marshal_VOID__BOOLEAN	g_cclosure_marshal_VOID__BOOLEAN

/* VOID:UINT,UINT,UINT,UINT,UINT,STRING (tp-ifaces-signals-marshal.list:21) */
extern void tp_ifaces_signals_marshal_VOID__UINT_UINT_UINT_UINT_UINT_STRING (GClosure     *closure,
                                                                             GValue       *return_value,
                                                                             guint         n_param_values,
                                                                             const GValue *param_values,
                                                                             gpointer      invocation_hint,
                                                                             gpointer      marshal_data);

/* VOID:UINT,UINT,STRING (tp-ifaces-signals-marshal.list:23) */
extern void tp_ifaces_signals_marshal_VOID__UINT_UINT_STRING (GClosure     *closure,
                                                              GValue       *return_value,
                                                              guint         n_param_values,
                                                              const GValue *param_values,
                                                              gpointer      invocation_hint,
                                                              gpointer      marshal_data);

/* VOID:UINT,UINT (tp-ifaces-signals-marshal.list:27) */
extern void tp_ifaces_signals_marshal_VOID__UINT_UINT (GClosure     *closure,
                                                       GValue       *return_value,
                                                       guint         n_param_values,
                                                       const GValue *param_values,
                                                       gpointer      invocation_hint,
                                                       gpointer      marshal_data);

/* VOID:STRING,BOXED,BOXED,BOXED,BOXED,UINT,UINT (tp-ifaces-signals-marshal.list:29) */
extern void tp_ifaces_signals_marshal_VOID__STRING_BOXED_BOXED_BOXED_BOXED_UINT_UINT (GClosure     *closure,
                                                                                      GValue       *return_value,
                                                                                      guint         n_param_values,
                                                                                      const GValue *param_values,
                                                                                      gpointer      invocation_hint,
                                                                                      gpointer      marshal_data);

/* VOID:BOXED (tp-ifaces-signals-marshal.list:33) */
#define tp_ifaces_signals_marshal_VOID__BOXED	g_cclosure_marshal_VOID__BOXED

/* VOID:UINT,UINT,UINT,STRING (tp-ifaces-signals-marshal.list:35) */
extern void tp_ifaces_signals_marshal_VOID__UINT_UINT_UINT_STRING (GClosure     *closure,
                                                                   GValue       *return_value,
                                                                   guint         n_param_values,
                                                                   const GValue *param_values,
                                                                   gpointer      invocation_hint,
                                                                   gpointer      marshal_data);

/* VOID:OBJECT,STRING (tp-ifaces-signals-marshal.list:37) */
extern void tp_ifaces_signals_marshal_VOID__OBJECT_STRING (GClosure     *closure,
                                                           GValue       *return_value,
                                                           guint         n_param_values,
                                                           const GValue *param_values,
                                                           gpointer      invocation_hint,
                                                           gpointer      marshal_data);

G_END_DECLS

#endif /* __tp_ifaces_signals_marshal_MARSHAL_H__ */

