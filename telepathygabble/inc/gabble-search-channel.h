/*
 * gabble-im-channel.h - Header for GabbleSearchChannel
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

#ifndef __GABBLE_SEARCH_CHANNEL_H__
#define __GABBLE_SEARCH_CHANNEL_H__

#include <glib-object.h>
#include <time.h>

#include "loudmouth/loudmouth.h"


#include "handles.h"
#include "telepathy-constants.h"
#include "search-mixin.h"

G_BEGIN_DECLS


                                             	                   	  			                        	  		   
extern LmHandlerResult
search_channel_iq_cb (LmMessageHandler *handler,
					  LmConnection *lmconn,
					  LmMessage *message,
					  gpointer user_data);

typedef struct _GabbleSearchChannel GabbleSearchChannel;
typedef struct _GabbleSearchChannelClass GabbleSearchChannelClass;

struct _GabbleSearchChannelClass {
    GObjectClass parent_class;

    GabbleSearchMixinClass mixin_class;
};

struct _GabbleSearchChannel {
    GObject parent;
	LmMessageHandler *iq_cb;
    GabbleSearchMixin search_mixin;
	//todo: channel_index used for closing/removing the channel..
	//can this be done without index?
	guint channel_index;
    gpointer priv;
};

IMPORT_C GType gabble_search_channel_get_type(void);

/* TYPE MACROS */
#define GABBLE_TYPE_SEARCH_CHANNEL \
  (gabble_search_channel_get_type())
#define GABBLE_SEARCH_CHANNEL(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), GABBLE_TYPE_SEARCH_CHANNEL, GabbleSearchChannel))
#define GABBLE_SEARCH_CHANNEL_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass), GABBLE_TYPE_SEARCH_CHANNEL, GabbleSearchChannelClass))
#define GABBLE_IS_SEARCH_CHANNEL(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj), GABBLE_TYPE_SEARCH_CHANNEL))
#define GABBLE_IS_SEARCH_CHANNEL_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass), GABBLE_TYPE_SEARCH_CHANNEL))
#define GABBLE_SEARCH_CHANNEL_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GABBLE_TYPE_SEARCH_CHANNEL, GabbleSearchChannelClass))



gboolean
gabble_search_channel_close (GabbleSearchChannel *self,
                         		GError **error);

gboolean
gabble_search_channel_get_channel_type (GabbleSearchChannel *self,
	                                    gchar **ret,
	                                    GError **error);


gboolean
gabble_search_channel_get_interfaces (GabbleSearchChannel *self,
                                  		gchar ***ret,
                                  		GError **error);


gboolean
gabble_search_channel_get_search_keys (	GabbleSearchChannel *self,
										gchar **instruction, 
										gchar ***key_names,
		                        		GError **error
                        	  		   );

gboolean
gabble_search_channel_get_search_state (	GabbleSearchChannel *self,
                                  			guint *ret,
                        					GError **error
                        	  			);

gboolean
gabble_search_channel_search (	GabbleSearchChannel *self,
                                  GHashTable *params,
                        			GError **error
                        	  );       
                        	  

gboolean
gabble_search_channel_get_handle (GabbleSearchChannel *self,
                              guint *ret,
                              guint *ret1,
                              GError **error);       
                              
                              
  
G_END_DECLS

#endif /* #ifndef __GABBLE_SEARCH_CHANNEL_H__*/
