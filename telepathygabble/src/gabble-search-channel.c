/*
 * gabble-im-channel.c - Source for GabbleSearchChannel
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

#include <dbus/dbus-glib.h>
#include "loudmouth/loudmouth.h"
#include <stdio.h>
#include <string.h>
#include <time.h>


#define DEBUG_FLAG GABBLE_DEBUG_SEARCH

#include "debug.h"
#include "disco.h"
#include "gabble-connection.h"
#include "gabble-presence.h"
#include "gabble-presence-cache.h"
#include "handles.h"
#include "roster.h"
#include "telepathy-constants.h"
#include "telepathy-errors.h"
#include "telepathy-helpers.h"
#include "telepathy-interfaces.h"
#include "tp-channel-iface.h"
#include "namespaces.h"

#include "gabble-search-channel.h"
#include "gabble-search-channel-glue.h"
#include "search-mixin-signals-marshal.h"
#include "search-mixin.h"

#include "gabble_enums.h"

#ifndef EMULATOR
G_DEFINE_TYPE_WITH_CODE (GabbleSearchChannel, gabble_search_channel, G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE (TP_TYPE_CHANNEL_IFACE, NULL));
    
#endif

/* signal enum */
enum
{
    CLOSED,
    LAST_SIGNAL 
#ifdef EMULATOR    
    = LAST_SIGNAL_SEARCH 
#endif
    
};

#ifdef EMULATOR
#include "libgabble_wsd_solution.h"

	GET_STATIC_ARRAY_FROM_TLS(signals,gabble_search,guint)
	#define signals (GET_WSD_VAR_NAME(signals,gabble_search, s)())	
    
    GET_STATIC_VAR_FROM_TLS(gabble_search_channel_parent_class,gabble_search,gpointer)
	#define gabble_search_channel_parent_class (*GET_WSD_VAR_NAME(gabble_search_channel_parent_class,gabble_search,s)())
	
	GET_STATIC_VAR_FROM_TLS(g_define_type_id,gabble_search,GType)
	#define g_define_type_id (*GET_WSD_VAR_NAME(g_define_type_id,gabble_search,s)())

    
static void gabble_search_channel_init (GabbleSearchChannel *self); 
static void gabble_search_channel_class_init (GabbleSearchChannelClass *klass); 
static void gabble_search_channel_class_intern_init (gpointer klass) 
{
 gabble_search_channel_parent_class = g_type_class_peek_parent (klass); 
 gabble_search_channel_class_init ((GabbleSearchChannelClass*) klass); 
 } 
 EXPORT_C GType gabble_search_channel_get_type (void) 
 {
  if ((g_define_type_id == 0)) 
  { static const GTypeInfo g_define_type_info = 
  		{ sizeof (GabbleSearchChannelClass), (GBaseInitFunc) ((void *)0), (GBaseFinalizeFunc) ((void *)0), (GClassInitFunc) gabble_search_channel_class_intern_init, (GClassFinalizeFunc) ((void *)0), ((void *)0), sizeof (GabbleSearchChannel), 0, (GInstanceInitFunc) gabble_search_channel_init, ((void *)0) }; g_define_type_id = g_type_register_static ( ((GType) ((20) << (2))), g_intern_static_string ("GabbleSearchChannel"), &g_define_type_info, (GTypeFlags) 0); { { static const GInterfaceInfo g_implement_interface_info = { (GInterfaceInitFunc) ((void *)0) }; g_type_add_interface_static (g_define_type_id, tp_channel_iface_get_type(), &g_implement_interface_info); } ; } } return g_define_type_id; 
  		};
	
	
		
#else

	static guint signals[LAST_SIGNAL] = {0};

#endif

/* properties */
enum
{
  PROP_OBJECT_PATH = 1,
  PROP_CHANNEL_TYPE,
  PROP_HANDLE_TYPE,
  PROP_HANDLE,
  PROP_CONNECTION,
  LAST_PROPERTY
};

/* private structure */
typedef struct _GabbleSearchChannelPrivate GabbleSearchChannelPrivate;

struct _GabbleSearchChannelPrivate
{
  GabbleConnection *conn;
  char *object_path;
  GabbleHandle handle; 
  gboolean closed;
  gboolean dispose_has_run;
};

#define GABBLE_SEARCH_CHANNEL_GET_PRIVATE(obj) \
    ((GabbleSearchChannelPrivate *)obj->priv)

static void
gabble_search_channel_init (GabbleSearchChannel *self)
{
  GabbleSearchChannelPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
      GABBLE_TYPE_SEARCH_CHANNEL, GabbleSearchChannelPrivate);

  self->priv = priv;
}

static GObject *
gabble_search_channel_constructor (GType type, guint n_props,
                               GObjectConstructParam *props)
{
  GObject *obj;
  GabbleSearchChannelPrivate *priv;
  DBusGConnection *bus;
 
  g_message("in gabble_search_channel_constructor\n");
  obj = G_OBJECT_CLASS (gabble_search_channel_parent_class)->
           constructor (type, n_props, props);
  priv = GABBLE_SEARCH_CHANNEL_GET_PRIVATE (GABBLE_SEARCH_CHANNEL (obj));

  bus = tp_get_bus ();
  dbus_g_connection_register_g_object (bus, priv->object_path, obj);

  
  gabble_search_mixin_init (obj, G_STRUCT_OFFSET (GabbleSearchChannel, search_mixin));


  return obj;
}

static void
gabble_search_channel_get_property (GObject    *object,
                                guint       property_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  GabbleSearchChannel *chan = GABBLE_SEARCH_CHANNEL (object);
  GabbleSearchChannelPrivate *priv = GABBLE_SEARCH_CHANNEL_GET_PRIVATE (chan);

  switch (property_id) {
    case PROP_OBJECT_PATH:
      g_value_set_string (value, priv->object_path);
      break;
    case PROP_CHANNEL_TYPE:
      g_value_set_static_string (value, TP_IFACE_CHANNEL_TYPE_CONTACT_SEARCH);
      break;
    case PROP_HANDLE_TYPE:
      g_value_set_uint (value, TP_HANDLE_TYPE_NONE);
      break;
    case PROP_HANDLE:
      g_value_set_uint (value, 0 ); 
      break;
    case PROP_CONNECTION:
      g_value_set_object (value, priv->conn);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gabble_search_channel_set_property (GObject     *object,
                                guint        property_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  GabbleSearchChannel *chan = GABBLE_SEARCH_CHANNEL (object);
  GabbleSearchChannelPrivate *priv = GABBLE_SEARCH_CHANNEL_GET_PRIVATE (chan);

  switch (property_id) {
    case PROP_OBJECT_PATH:
      g_free (priv->object_path);
      priv->object_path = g_value_dup_string (value);
      break;
    case PROP_HANDLE:
      priv->handle = g_value_get_uint (value);
      break;
    case PROP_CONNECTION:
      priv->conn = g_value_get_object (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void gabble_search_channel_dispose (GObject *object);
static void gabble_search_channel_finalize (GObject *object);

static void
gabble_search_channel_class_init (GabbleSearchChannelClass *gabble_search_channel_class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (gabble_search_channel_class);
  GParamSpec *param_spec;
  g_message("in gabble_search_channel_class_init");

  g_type_class_add_private (gabble_search_channel_class, sizeof (GabbleSearchChannelPrivate));

  object_class->constructor = gabble_search_channel_constructor;

  object_class->get_property = gabble_search_channel_get_property;
  object_class->set_property = gabble_search_channel_set_property;

  object_class->dispose = gabble_search_channel_dispose;
  object_class->finalize = gabble_search_channel_finalize;

  g_object_class_override_property (object_class, PROP_OBJECT_PATH, "object-path");
  g_object_class_override_property (object_class, PROP_CHANNEL_TYPE, "channel-type");
  g_object_class_override_property (object_class, PROP_HANDLE_TYPE, "handle-type");
  g_object_class_override_property (object_class, PROP_HANDLE, "handle");


 
  param_spec = g_param_spec_object ("connection", "GabbleConnection object",
                                    "Gabble connection object that owns this "
                                    "Search channel object.",
                                    GABBLE_TYPE_CONNECTION,
                                    G_PARAM_CONSTRUCT_ONLY |
                                    G_PARAM_READWRITE |
                                    G_PARAM_STATIC_NICK |
                                    G_PARAM_STATIC_BLURB);
  g_object_class_install_property (object_class, PROP_CONNECTION, param_spec);

  signals[CLOSED] =
    g_signal_new ("closed",
                  G_OBJECT_CLASS_TYPE (gabble_search_channel_class),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                  0,
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  gabble_search_mixin_class_init (object_class, G_STRUCT_OFFSET (GabbleSearchChannelClass, mixin_class));

  dbus_g_object_type_install_info (G_TYPE_FROM_CLASS (gabble_search_channel_class), &dbus_glib_gabble_search_channel_object_info);
}

void
gabble_search_channel_dispose (GObject *object)
{
  GabbleSearchChannel *self = GABBLE_SEARCH_CHANNEL (object);
  GabbleSearchChannelPrivate *priv = GABBLE_SEARCH_CHANNEL_GET_PRIVATE (self);

  g_message("in gabble_search_channel_dispose\n");
  if (priv->dispose_has_run)
    return;

  priv->dispose_has_run = TRUE;

  
  if (!priv->closed)
    g_signal_emit(self, signals[CLOSED], 0);

  if (G_OBJECT_CLASS (gabble_search_channel_parent_class)->dispose)
    G_OBJECT_CLASS (gabble_search_channel_parent_class)->dispose (object);
}

void
gabble_search_channel_finalize (GObject *object)
{
  GabbleSearchChannel *self = GABBLE_SEARCH_CHANNEL (object);
  GabbleSearchChannelPrivate *priv = GABBLE_SEARCH_CHANNEL_GET_PRIVATE (self);

  g_message("in gabble_search_channel_finalize\n");
  /* free any data held directly by the object here */
  if( priv->object_path )
  {
    g_message("freeing priv->object_path in gabble_search_channel_finalize\n");
  	g_free (priv->object_path);
  	priv->object_path = NULL;
  }
  
  //also free the data,if any, held by mixin obj
  //gabble_search_mixin_finalize(object);

  G_OBJECT_CLASS (gabble_search_channel_parent_class)->finalize (object);
}



/**
 * gabble_search_channel_close
 *
 * Implements D-Bus method Close
 * on interface org.freedesktop.Telepathy.Channel
 *
 * @error: Used to return a pointer to a GError detailing any error
 *         that occurred, D-Bus will throw the error only if this
 *         function returns FALSE.
 *
 * Returns: TRUE if successful, FALSE if an error was thrown.
 */
gboolean
gabble_search_channel_close (GabbleSearchChannel *self,
                         GError **error)
{
  GabbleSearchChannelPrivate *priv;

  g_assert (GABBLE_IS_SEARCH_CHANNEL (self));

  gabble_debug (DEBUG_FLAG, "called on %p", self);
  
  g_message("gabble_search_channel_close");
  priv = GABBLE_SEARCH_CHANNEL_GET_PRIVATE (self);
  
  if (priv->closed)
    {
      gabble_debug (DEBUG_FLAG, "channel already closed");

      g_set_error (error, TELEPATHY_ERRORS, NotAvailable,
          "Channel already closed");

      return FALSE;
    }

  
  priv->closed = TRUE;

  g_signal_emit (self, signals[CLOSED], 0);

  return TRUE;
}


/**
 * gabble_search_channel_get_channel_type
 *
 * Implements D-Bus method GetChannelType
 * on interface org.freedesktop.Telepathy.Channel
 *
 * @error: Used to return a pointer to a GError detailing any error
 *         that occurred, D-Bus will throw the error only if this
 *         function returns FALSE.
 *
 * Returns: TRUE if successful, FALSE if an error was thrown.
 */
gboolean
gabble_search_channel_get_channel_type (GabbleSearchChannel *self,
                                    gchar **ret,
                                    GError **error)
{
  *ret = g_strdup (TP_IFACE_CHANNEL_TYPE_CONTACT_SEARCH);

  return TRUE;
}


/**
 * gabble_search_channel_get_interfaces
 *
 * Implements D-Bus method GetInterfaces
 * on interface org.freedesktop.Telepathy.Channel
 *
 * @error: Used to return a pointer to a GError detailing any error
 *         that occurred, D-Bus will throw the error only if this
 *         function returns FALSE.
 *
 * Returns: TRUE if successful, FALSE if an error was thrown.
 */
gboolean
gabble_search_channel_get_interfaces (GabbleSearchChannel *self,
                                  gchar ***ret,
                                  GError **error)
{
  const char *interfaces[] = { NULL };

  *ret = g_strdupv ((gchar **) interfaces);

  return TRUE;
}


/**
 * gabble_search_channel_search
 *
 * Implements D-Bus method Search
 * on interface org.freedesktop.Telepathy.Channel
 *
 * @error: Used to return a pointer to a GError detailing any error
 *         that occurred, D-Bus will throw the error only if this
 *         function returns FALSE.
 *
 * Returns: TRUE if successful, FALSE if an error was thrown.
 */
gboolean
gabble_search_channel_search (	GabbleSearchChannel *self,
                                  GHashTable *params,
                        			GError **error
                        	  )
{
  GabbleSearchChannelPrivate *priv;
  
  g_message("In gabble_search_channel_search: serachchan is %u\n",self);

  g_assert (GABBLE_IS_SEARCH_CHANNEL (self));
  priv = GABBLE_SEARCH_CHANNEL_GET_PRIVATE (self);

  return gabble_search_mixin_search (G_OBJECT (self), params,
     									priv->conn, error);
  
}

/**
 * gabble_search_channel_get_search_state
 *
 * Implements D-Bus method GetSearchState
 * on interface org.freedesktop.Telepathy.Channel
 *
 * @error: Used to return a pointer to a GError detailing any error
 *         that occurred, D-Bus will throw the error only if this
 *         function returns FALSE.
 *
 * Returns: TRUE if successful, FALSE if an error was thrown.
 */
gboolean
gabble_search_channel_get_search_state (	GabbleSearchChannel *self,
                                  			guint *ret,
                        					GError **error
                        	  			)
{
  g_message("In gabble_search_channel_get_search_state\n");
  return gabble_search_mixin_get_search_state (G_OBJECT (self),ret, error);
 }
 

/**
 * gabble_search_channel_get_handle
 *
 * Implements D-Bus method GetHandle
 * on interface org.freedesktop.Telepathy.Channel
 *
 * @error: Used to return a pointer to a GError detailing any error
 *         that occurred, D-Bus will throw the error only if this
 *         function returns FALSE.
 *
 * Returns: TRUE if successful, FALSE if an error was thrown.
 */
gboolean
gabble_search_channel_get_handle (GabbleSearchChannel *self,
                              guint *ret,
                              guint *ret1,
                              GError **error)
{
  GabbleSearchChannelPrivate *priv;

  g_assert (GABBLE_IS_SEARCH_CHANNEL (self));

  priv = GABBLE_SEARCH_CHANNEL_GET_PRIVATE (self);

  *ret = TP_HANDLE_TYPE_NONE;
  //search channel doesnt correspond to any particular handle 
  *ret1 = 0; 

  return TRUE;
}

/**
 * TODO: this method may be modified in future to get the search key types 
 * as dbus-glib binding tool doesnt support 'g' type signature 
 * Also this method should be able to give value
 * for the options for list-single or list-multi type search keys if required
 * gabble_search_channel_get_search_keys
 *
 * Implements D-Bus method GetSearchKeys
 * on interface org.freedesktop.Telepathy.Channel
 *
 * @error: Used to return a pointer to a GError detailing any error
 *         that occurred, D-Bus will throw the error only if this
 *         function returns FALSE.
 *
 * Returns: TRUE if successful, FALSE if an error was thrown.
 */
gboolean
gabble_search_channel_get_search_keys (	GabbleSearchChannel *self,
										gchar **ret_instruction, 
										gchar ***ret_searchkeys,
		                        		GError **error
                        	  		   )
{
  GabbleSearchChannelPrivate *priv;
  
  g_message("In gabble_search_channel_get_search_keys: serachchan is %u\n",self);
  
  g_assert (GABBLE_IS_SEARCH_CHANNEL (self));
  priv = GABBLE_SEARCH_CHANNEL_GET_PRIVATE (self);
  
  return gabble_search_mixin_get_search_keys (G_OBJECT (self),ret_instruction,
  												ret_searchkeys,
  												priv->conn, error);
 }


/**
 * search_channel_iq_cb:
 *
 * Called by loudmouth when we get an incoming <iq>. This handler
 * is concerned only with search queries, and allows other handlers
 * if queries other than search are received.
 */
 
LmHandlerResult
search_channel_iq_cb (LmMessageHandler *handler,
					  LmConnection *lmconn,
					  LmMessage *message,
					  gpointer user_data)
{
  GabbleSearchChannel *chan = GABBLE_SEARCH_CHANNEL (user_data);
  GabbleSearchChannelPrivate *priv = NULL;
  GabbleSearchMixin *mixin = NULL;
  
  LmMessageNode *iq_node, *query_node;
  LmMessageSubType sub_type;
  
  LmMessageNode *x_node, *x_item_node, *result_node;
  const char  *x_node_type;
  const gchar **from = NULL; 
  
  GHashTable *results = NULL;
  guint contact_handle = 0;

  
  g_message("in search_channel_iq_cb: user data - chan is : %u\n", chan);
  
  sub_type = lm_message_get_sub_type (message); 
  
  if (sub_type != LM_MESSAGE_SUB_TYPE_RESULT)
    {
      return LM_HANDLER_RESULT_ALLOW_MORE_HANDLERS;
    }
  iq_node = lm_message_get_node (message);
  query_node = lm_message_node_get_child_with_namespace (iq_node, "query",
      NS_SEARCH);

  if (query_node == NULL || chan == NULL)
    return LM_HANDLER_RESULT_ALLOW_MORE_HANDLERS;
  
    
  
  priv= GABBLE_SEARCH_CHANNEL_GET_PRIVATE (chan);
  mixin = GABBLE_SEARCH_MIXIN (chan);
  
           		
  x_node = lm_message_node_get_child_with_namespace (query_node, "x", NS_X_DATA);
	
  if( x_node )
	{
	    //if service supports data forms 
    	result_node = x_node; 
		x_node_type   = lm_message_node_get_attribute (x_node, "type" );
		if (0 != strcmp (x_node_type, "result"))
		{
		 //this can be for search_keys_iq_cb 
		 //callback received as result of request to get search key fields
		 return LM_HANDLER_RESULT_REMOVE_MESSAGE;	
		}
		//if(type == NULL), do some error handling   
	}
		
   else
    	result_node = query_node;	
		
	
   for (x_item_node = result_node->children; x_item_node; x_item_node = x_item_node->next)
	{
	   	 LmMessageNode *item_node, *node;
	  	 GHashTable *search_result = NULL;
	  	 gboolean reported_field_has_type = FALSE;
	  	 
	  	 
	  	 if(!search_result)
	  	 	search_result = g_hash_table_new_full(g_str_hash, g_str_equal, NULL,NULL);
	  	 
	  	 //there should be only one reported field
  	 	 //Later "reported" should also be used to get the types of search result fields
	  	 //'g' type currently not supported by dbus-glib-binding
	  	 if (0 != strcmp (x_item_node->name, "item") )
        	continue;
       	 
       	 contact_handle = 0;
       	 
	  	 //x_item_node is an item node
	  	 //get the field nodes from each item node
	  	 for (node = x_item_node->children; node; node = node->next)
	      	{
		      	const gchar *key, *value;
		        LmMessageNode *value_node;
		      	GValue *val = g_new0(GValue, 1); 
		      	LmMessageNode *reported_field_node;     
		      	
		            	
		      	if (0 != strcmp (node->name, "field"))
        		continue;
		      	
		      	//node is a field or reported node
		      	//get the name, type and value of each field
		      	key = lm_message_node_get_attribute(node, "var");
		  		      	
		       	value_node = lm_message_node_get_child(node, "value");
		      	 
		      	value = lm_message_node_get_value(value_node);
		      	
		      	if(0 == strcmp (key, "jid"))
			      	{
			      	g_message("getting contact_handle\n");
			      	contact_handle = gabble_handle_for_contact (priv->conn->handles, value, FALSE);
			      	}
		      	
		      	g_value_init (val, G_TYPE_STRING);
		      	//"val" should be a struct with members "var" and "type"
				g_value_set_string (val, g_strdup(value) );
		      	g_hash_table_insert(search_result,g_strdup(key),val);
		      
	      	}
	    
	    
	    g_message("search results: %u",g_hash_table_size(search_result));
	     //emit the signal for search result received 
	     _gabble_search_mixin_emit_search_result_received(G_OBJECT (chan),contact_handle, search_result);
	    
	     g_hash_table_destroy(search_result);
	}

  
  //if no search results, service must return empty query element
  //so emit the signal for the same
  if(!contact_handle)
	  {
	  			
	  	if(!results)
			results = g_hash_table_new_full(g_str_hash, g_str_equal, NULL,NULL);
	  		 
	  	 _gabble_search_mixin_emit_search_result_received(G_OBJECT (chan),contact_handle, results);
	     g_hash_table_destroy(results);

	  }
  
  //search state should be completed 
  g_message("b4 set search state changed \n"); 
  _gabble_search_mixin_set_search_state(G_OBJECT (chan), TP_CHANNEL_CONTACT_SEARCH_STATE_AFTER); 
   
  //search request completed now, emit the signal for search state changed
 g_message("b4 emiiting search state changed signal\n"); 
  _gabble_search_mixin_emit_search_state_changed(G_OBJECT (chan),TP_CHANNEL_CONTACT_SEARCH_STATE_AFTER);
  
 return LM_HANDLER_RESULT_REMOVE_MESSAGE;
}



