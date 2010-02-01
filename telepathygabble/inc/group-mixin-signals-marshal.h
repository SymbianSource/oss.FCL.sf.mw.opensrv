/*
 * group-mixin-signals-marshal.h 
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

#ifndef __group_mixin_marshal_MARSHAL_H__
#define __group_mixin_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:UINT,UINT (group-mixin-signals-marshal.list:1) */
extern void group_mixin_marshal_VOID__UINT_UINT (GClosure     *closure,
                                                 GValue       *return_value,
                                                 guint         n_param_values,
                                                 const GValue *param_values,
                                                 gpointer      invocation_hint,
                                                 gpointer      marshal_data);

/* VOID:STRING,BOXED,BOXED,BOXED,BOXED,UINT,UINT (group-mixin-signals-marshal.list:2) */
extern void group_mixin_marshal_VOID__STRING_BOXED_BOXED_BOXED_BOXED_UINT_UINT (GClosure     *closure,
                                                                                GValue       *return_value,
                                                                                guint         n_param_values,
                                                                                const GValue *param_values,
                                                                                gpointer      invocation_hint,
                                                                                gpointer      marshal_data);

G_END_DECLS

#endif /* __group_mixin_marshal_MARSHAL_H__ */

