/*
 * text-mixin-signals-marshal.h - Headers for Telepathy Channel Factory interface
 *
 * Copyright (C) 2006 Collabora Ltd.
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

#ifndef __text_mixin_marshal_MARSHAL_H__
#define __text_mixin_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:UINT,UINT,UINT,UINT,UINT,STRING (text-mixin-signals-marshal.list:1) */
extern void text_mixin_marshal_VOID__UINT_UINT_UINT_UINT_UINT_STRING (GClosure     *closure,
                                                                      GValue       *return_value,
                                                                      guint         n_param_values,
                                                                      const GValue *param_values,
                                                                      gpointer      invocation_hint,
                                                                      gpointer      marshal_data);

/* VOID:UINT,UINT,UINT,STRING (text-mixin-signals-marshal.list:2) */
extern void text_mixin_marshal_VOID__UINT_UINT_UINT_STRING (GClosure     *closure,
                                                            GValue       *return_value,
                                                            guint         n_param_values,
                                                            const GValue *param_values,
                                                            gpointer      invocation_hint,
                                                            gpointer      marshal_data);

/* VOID:UINT,UINT,STRING (text-mixin-signals-marshal.list:3) */
extern void text_mixin_marshal_VOID__UINT_UINT_STRING (GClosure     *closure,
                                                       GValue       *return_value,
                                                       guint         n_param_values,
                                                       const GValue *param_values,
                                                       gpointer      invocation_hint,
                                                       gpointer      marshal_data);

G_END_DECLS

#endif /* __text_mixin_marshal_MARSHAL_H__ */

