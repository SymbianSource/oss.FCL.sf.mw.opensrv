/*
 * text-mixin.h - Header for GabbleSearchMixin
 * Copyright (C) 2006 Collabora Ltd.
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

#ifndef __GABBLE_SEARCH_MIXIN_H__
#define __GABBLE_SEARCH_MIXIN_H__

#include "handles.h"
#include "handle-set.h"
#include "util.h"



G_BEGIN_DECLS

typedef struct _GabbleSearchMixinClass GabbleSearchMixinClass;
typedef struct _GabbleSearchMixin GabbleSearchMixin;

struct _GabbleSearchMixinClass {
  guint search_result_received_signal_id;
  guint search_state_changed_signal_id;
};

struct _GabbleSearchMixin {
  
  //todo: any data if required 
  guint search_state;
  
};

GType gabble_search_mixin_get_type(void);

/* TYPE MACROS */
#define GABBLE_SEARCH_MIXIN_CLASS_OFFSET_QUARK (gabble_search_mixin_class_get_offset_quark())
#define GABBLE_SEARCH_MIXIN_CLASS_OFFSET(o) (GPOINTER_TO_UINT (g_type_get_qdata (G_OBJECT_CLASS_TYPE (o), GABBLE_SEARCH_MIXIN_CLASS_OFFSET_QUARK)))
#define GABBLE_SEARCH_MIXIN_CLASS(o) ((GabbleSearchMixinClass *) gabble_mixin_offset_cast (o, GABBLE_SEARCH_MIXIN_CLASS_OFFSET (o)))

#define GABBLE_SEARCH_MIXIN_OFFSET_QUARK (gabble_search_mixin_get_offset_quark())
#define GABBLE_SEARCH_MIXIN_OFFSET(o) (GPOINTER_TO_UINT (g_type_get_qdata (G_OBJECT_TYPE (o), GABBLE_SEARCH_MIXIN_OFFSET_QUARK)))
#define GABBLE_SEARCH_MIXIN(o) ((GabbleSearchMixin *) gabble_mixin_offset_cast (o, GABBLE_SEARCH_MIXIN_OFFSET (o)))

GQuark gabble_search_mixin_class_get_offset_quark (void);
GQuark gabble_search_mixin_get_offset_quark (void);

void gabble_search_mixin_class_init (GObjectClass *obj_cls, glong offset);
void gabble_search_mixin_init (GObject *obj, glong offset );

void gabble_search_mixin_finalize (GObject *obj);

gboolean gabble_search_mixin_search (GObject *obj,GHashTable *params,
									 GabbleConnection *conn,
									 GError **error);

gboolean gabble_search_mixin_get_search_state (	GObject *obj, guint *ret,
                        					GError **error );
                                	 
void
_gabble_search_mixin_emit_search_result_received (GObject *obj,
                                                 guint contact_handle,
                             				     GHashTable *values );

void
_gabble_search_mixin_emit_search_state_changed (GObject *obj,
                                                 guint search_state );
                                                 

void
_gabble_search_mixin_set_search_state (GObject *obj, guint state );

gboolean
gabble_search_mixin_get_search_keys (	GObject *obj,
										gchar **ret_instruction, 
										gchar ***ret_searchkeys,
										GabbleConnection *conn,
		                        		GError **error
                        	  		   );                                                 


G_END_DECLS

#endif /* #ifndef __GABBLE_SEARCH_MIXIN_H__ */

