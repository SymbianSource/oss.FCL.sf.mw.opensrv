/*
 * search-factory.h - Header for GabbleSearchFactory
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

#ifndef __SEARCH_FACTORY_H__
#define __SEARCH_FACTORY_H__

#include <glib-object.h>

#include "gabble-search-channel.h"

G_BEGIN_DECLS

typedef struct _GabbleSearchFactory GabbleSearchFactory;
typedef struct _GabbleSearchFactoryClass GabbleSearchFactoryClass;

struct _GabbleSearchFactoryClass {
  GObjectClass parent_class;
};

struct _GabbleSearchFactory {
  GObject parent;
};

IMPORT_C GType gabble_search_factory_get_type(void);

/* TYPE MACROS */
#define GABBLE_TYPE_SEARCH_FACTORY \
  (gabble_search_factory_get_type())
#define GABBLE_SEARCH_FACTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), GABBLE_TYPE_SEARCH_FACTORY, GabbleSearchFactory))
#define GABBLE_SEARCH_FACTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass), GABBLE_TYPE_SEARCH_FACTORY, GabbleSearchFactoryClass))
#define GABBLE_IS_SEARCH_FACTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj), GABBLE_TYPE_SEARCH_FACTORY))
#define GABBLE_IS_SEARCH_FACTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass), GABBLE_TYPE_SEARCH_FACTORY))
#define GABBLE_SEARCH_FACTORY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GABBLE_TYPE_SEARCH_FACTORY, GabbleSearchFactoryClass))


G_END_DECLS

#endif /* #ifndef __IM_FACTORY_H__ */

