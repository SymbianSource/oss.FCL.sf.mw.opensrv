/*
 * search-mixin.c - Source for GabbleSearchMixin
 * Copyright (C) 2006 Collabora Ltd.
 * 
 *   @author Ole Andre Vadla Ravnaas <ole.andre.ravnaas@collabora.co.uk>
 *   @author Robert McQueen <robert.mcqueen@collabora.co.uk>
 *   @author Senko Rasic <senko@senko.net>
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


#include "loudmouth/loudmouth.h"
#include <dbus/dbus-glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "telepathy-constants.h"
#include "telepathy-errors.h"


#include "debug.h"
#include "gabble-connection.h"
#include "namespaces.h"
#include "roster.h"
#include "util.h"


#include "search-mixin.h"
#include "search-mixin-signals-marshal.h"


#include "gabble_enums.h"

#define _GNU_SOURCE /* Needed for strptime (_XOPEN_SOURCE can also be used). */

#define DEBUG_FLAG GABBLE_DEBUG_SEARCH


/* allocator */

#ifdef DEBUG_FLAG
//#define DEBUG(format, ...)
#define DEBUGGING 0
#define NODE_DEBUG(n, s)
#endif /* DEBUG_FLAG */


#ifdef EMULATOR
#include "libgabble_wsd_solution.h"

	GET_STATIC_VAR_FROM_TLS(offset_quark1,gabble_search_mixin,GQuark)
	#define offset_quark1 (*GET_WSD_VAR_NAME(offset_quark1,gabble_search_mixin, s)())	
	
	GET_STATIC_VAR_FROM_TLS(offset_quark,gabble_search_mixin,GQuark)
	#define offset_quark (*GET_WSD_VAR_NAME(offset_quark,gabble_search_mixin, s)())	
	
	GET_STATIC_VAR_FROM_TLS(alloc1,gabble_search_mixin,GabbleAllocator)
	#define alloc1 (*GET_WSD_VAR_NAME(alloc1,gabble_search_mixin, s)())		
	
#endif

/*
Moved to gabble_enums.h
typedef struct _GabbleAllocator GabbleAllocator;
struct _GabbleAllocator
{
  gulong size;
  guint limit;
  guint count;
};*/


typedef struct _IsKeyValidUserData IsKeyValidUserData;

struct _IsKeyValidUserData
{
  GError **error;
  gchar **search_key_names;
  gboolean is_key_found;
};


typedef struct _SearchKeyVarUserData SearchKeyVarUserData;

struct _SearchKeyVarUserData
{
  LmMessageNode *x_node;
  GabbleConnection *conn;
  
};


#define ga_new0(alloc, type) \
    ((type *) gabble_allocator_alloc0 (alloc))

static void
gabble_allocator_init (GabbleAllocator *alloc, gulong size, guint limit)
{
  g_assert (alloc != NULL);
  g_assert (size > 0);
  g_assert (limit > 0);

  alloc->size = size;
  alloc->limit = limit;
}

static gpointer gabble_allocator_alloc0 (GabbleAllocator *alloc)
{
  gpointer ret;

  g_assert (alloc != NULL);
  g_assert (alloc->count <= alloc->limit);

  if (alloc->count == alloc->limit)
    {
      ret = NULL;
    }
  else
    {
      ret = g_malloc0 (alloc->size);
      alloc->count++;
    }

  return ret;
}

static void gabble_allocator_free (GabbleAllocator *alloc, gpointer thing)
{
  g_assert (alloc != NULL);
  g_assert (thing != NULL);

  g_free (thing);
  alloc->count--;
}


/**
 * gabble_search_mixin_class_get_offset_quark:
 *
 * Returns: the quark used for storing mixin offset on a GObjectClass
 */
GQuark
gabble_search_mixin_class_get_offset_quark ()
{
#ifndef EMULATOR  
  static GQuark offset_quark1 = 0;
#endif
  
  if (!offset_quark1)
    offset_quark1 = g_quark_from_static_string("SearchMixinClassOffsetQuark");
  return offset_quark1;
}

/**
 * gabble_search_mixin_get_offset_quark:
 *
 * Returns: the quark used for storing mixin offset on a GObject
 */
GQuark
gabble_search_mixin_get_offset_quark ()
{
#ifndef EMULATOR  
  static GQuark offset_quark = 0;
#endif
  
  if (!offset_quark)
    offset_quark = g_quark_from_static_string("SearchMixinOffsetQuark");
  return offset_quark;
}


/* GabbleSearchMixin */
void
gabble_search_mixin_class_init (GObjectClass *obj_cls, glong offset)
{
  GabbleSearchMixinClass *mixin_cls;

  g_assert (G_IS_OBJECT_CLASS (obj_cls));

  g_type_set_qdata (G_OBJECT_CLASS_TYPE (obj_cls),
      GABBLE_SEARCH_MIXIN_CLASS_OFFSET_QUARK,
      GINT_TO_POINTER (offset));

  mixin_cls = GABBLE_SEARCH_MIXIN_CLASS (obj_cls);


  mixin_cls->search_result_received_signal_id = g_signal_new ("search-result-received",
                G_OBJECT_CLASS_TYPE (obj_cls),
                G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                0,
                NULL, NULL,
                search_mixin_marshal_VOID__UINT_BOXED,
                G_TYPE_NONE,
                2, G_TYPE_UINT, dbus_g_type_get_map ("GHashTable", G_TYPE_STRING, G_TYPE_VALUE ));

  mixin_cls->search_state_changed_signal_id = g_signal_new ("search-state-changed",
                G_OBJECT_CLASS_TYPE (obj_cls),
                G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                0,
                NULL, NULL,
                g_cclosure_marshal_VOID__UINT,
                G_TYPE_NONE, 1, G_TYPE_UINT );

}

void
gabble_search_mixin_init (GObject *obj,
                        	glong offset )
{
  GabbleSearchMixin *mixin;

  g_assert (G_IS_OBJECT (obj));

  g_type_set_qdata (G_OBJECT_TYPE (obj),
                    GABBLE_SEARCH_MIXIN_OFFSET_QUARK,
                    GINT_TO_POINTER (offset));

  mixin = GABBLE_SEARCH_MIXIN (obj);
  
  mixin->search_state = TP_CHANNEL_CONTACT_SEARCH_STATE_BEFORE;

 }

void
gabble_search_mixin_finalize (GObject *obj)
{
  GabbleSearchMixin *mixin = GABBLE_SEARCH_MIXIN (obj);
  /* free any data held directly by the object here */
}

static void
setfield_foreach (gpointer key, gpointer value, gpointer user_data)
{
  const gchar *search_data_string = NULL;  
  SearchKeyVarUserData *key_var_struct = (SearchKeyVarUserData*)user_data;
  LmMessageNode *field_node;  
  const gchar *search_key_var = NULL;
  GType g_type = G_VALUE_TYPE (value);
  
  switch (g_type) 
  {
    case G_TYPE_STRING:
    	search_data_string =  g_value_get_string(value);
    	break;
    default:
          g_assert_not_reached ();
  }
  
  search_key_var = (gchar *) g_hash_table_lookup (key_var_struct->conn->search_key_ht,
              										 key /*Label*/);
  field_node = lm_message_node_add_child ( key_var_struct->x_node, "field", NULL );
  lm_message_node_set_attribute ( field_node, "var", search_key_var );
  lm_message_node_add_child ( field_node, "value", search_data_string ); 
}
static void 
gabble_search_keynames_are_valid ( gpointer key, gpointer value,
                         			gpointer userdata )
{
  guint i;
  const gchar *search_data_string =  g_value_get_string(value);
  IsKeyValidUserData *key_valid_struct = (IsKeyValidUserData*)userdata;
  gchar **search_key_names = key_valid_struct->search_key_names;

  if( !key_valid_struct->is_key_found )
  	return;
  
  if( key == NULL )
	  {
	  	key_valid_struct->is_key_found = FALSE;
	  	return;
	  }
	   
	  
  for (i = 0; search_key_names[i]; i++)
    {
      if (0 == strcmp (search_key_names[i], key))
	      {
	      g_message("searchkey %s is valid\n",key);
	      if( search_data_string )
	      	g_message("value is %s\n",search_data_string);
	      return;	
	      }
    }
 key_valid_struct->is_key_found = FALSE;
 g_message("searchkey %s is invalid\n",key);
 g_set_error ( key_valid_struct->error, TELEPATHY_ERRORS, InvalidArgument,
		              "invalid search key found : %s\n", key);
}


/**
 * gabble_search_mixin_search
 *
 * Implements D-Bus method Search
 * on interface org.freedesktop.Telepathy.Channel.Type.ContactSearch
 *
 * @error: Used to return a pointer to a GError detailing any error
 *         that occurred, D-Bus will throw the error only if this
 *         function returns false.
 *
 * Returns: TRUE if successful, FALSE if an error was thrown.
 */
gboolean gabble_search_mixin_search (GObject *obj,GHashTable *params,
									 GabbleConnection *conn,
                                 	 GError **error)
{
  LmMessage *msg;
  LmMessageNode *query_node;
  LmMessageNode *x_node;
  gboolean result;
  const gchar *service = NULL;
  GabbleSearchMixin *mixin = GABBLE_SEARCH_MIXIN (obj);
  IsKeyValidUserData *user_data = NULL;
  SearchKeyVarUserData *get_keyvar_userdata = NULL;
     

  if (params == NULL)
      {
       g_set_error (error, TELEPATHY_ERRORS, InvalidArgument,
	              "invalid argument \n");
  	   return FALSE;	
      }
  
  if ( !g_hash_table_size(params) )
      {
       g_set_error (error, TELEPATHY_ERRORS, InvalidArgument,
	              "invalid argument, no key-value pair to do search\n");
  	   return FALSE;	
      }
  
  service = conn->search_service_jid;
  
  if (service == NULL)
      {
       g_set_error (error, TELEPATHY_ERRORS, NotAvailable,
	              "Search Service is not available\n");
  	   return FALSE;	
      }
      
  g_message("service is %s\n",service);
  
  if (conn->search_key_names == NULL)
      {
       g_set_error (error, TELEPATHY_ERRORS, NotAvailable,
	              "search key names not available");
  	   return FALSE;	
      }
  
  user_data = g_new0 (IsKeyValidUserData, 1);
  user_data->is_key_found = TRUE;
  user_data->error = error;
  user_data->search_key_names = conn->search_key_names;
  
  g_hash_table_foreach (params, gabble_search_keynames_are_valid, user_data );
  
  if(!user_data->is_key_found)
	  {
	  g_free(user_data);
	  g_message("invalid searchkey found\n");
	  return FALSE;		              
	  }
  
  g_free(user_data);
  
  msg= lm_message_new_with_sub_type ( service,
                                      LM_MESSAGE_TYPE_IQ,
                                      LM_MESSAGE_SUB_TYPE_SET);
  query_node = lm_message_node_add_child (msg->node, "query", NULL);
  
  lm_message_node_set_attribute (query_node, "xmlns", NS_SEARCH);
 
  x_node = lm_message_node_add_child ( query_node, "x", NULL );

  lm_message_node_set_attributes (x_node,
                      "xmlns", NS_X_DATA,
                      "type", "submit",
                      NULL);
                      
  get_keyvar_userdata = g_new0 (SearchKeyVarUserData, 1);
  get_keyvar_userdata->x_node = x_node;
  get_keyvar_userdata->conn = conn;

  g_hash_table_foreach (params, setfield_foreach, get_keyvar_userdata );
  
  g_free(get_keyvar_userdata);	                                  	
	 
  result = _gabble_connection_send (conn, msg, error);
  lm_message_unref (msg);

  if (!result)
    return FALSE;
  
  //this means for each search attempt, a new channel should be created
  mixin->search_state = TP_CHANNEL_CONTACT_SEARCH_STATE_DURING;
  
  //send search state changed signal if required
  _gabble_search_mixin_emit_search_state_changed(obj,mixin->search_state);

   return TRUE;
}

gboolean gabble_search_mixin_get_search_state (	GObject *obj, 
												guint *ret,
                        						GError **error )
{
  GabbleSearchMixin *mixin = GABBLE_SEARCH_MIXIN (obj);
  *ret = mixin->search_state; 
  return TRUE;
}


/**
 * gabble_search_mixin_get_search_keys
 *
 * Implements D-Bus method GetSearchKeys
 * on interface org.freedesktop.Telepathy.Channel.Type.ContactSearch
 *
 * @error: Used to return a pointer to a GError detailing any error
 *         that occurred, D-Bus will throw the error only if this
 *         function returns false.
 *
 * Returns: TRUE if successful, FALSE if an error was thrown.
 */
gboolean
gabble_search_mixin_get_search_keys (	GObject *obj,
										gchar **ret_instruction, 
										gchar ***ret_searchkeys,
										GabbleConnection *conn,
		                        		GError **error
                        	  		   )
{
  //later this method should be modified to give 
  //types of search keys fields also
  
  if (conn->search_key_names == NULL)
      {
       g_set_error (error, TELEPATHY_ERRORS, NotAvailable,
	              "search keys not available");
  	   return FALSE;	
      }
  
  *ret_searchkeys = g_strdupv ((gchar **) conn->search_key_names);
  *ret_instruction = g_strdup ( (gchar*)conn->search_instr);

  g_message("conn->search_instr :%s\n",(gchar*)conn->search_instr);
  g_message("ret_instruction  :%s\n",(gchar*)*ret_instruction );

  return TRUE;
}


void
_gabble_search_mixin_emit_search_result_received (GObject *obj,
                                                 guint contact_handle,
                             				     GHashTable *values )
{
  GabbleSearchMixinClass *mixin_cls = GABBLE_SEARCH_MIXIN_CLASS (G_OBJECT_GET_CLASS
      (obj));

  g_signal_emit (obj, mixin_cls->search_result_received_signal_id, 0,
                 contact_handle,
                 values );
}

void
_gabble_search_mixin_emit_search_state_changed (GObject *obj,
                                                 guint search_state )
{
  GabbleSearchMixinClass *mixin_cls = GABBLE_SEARCH_MIXIN_CLASS (G_OBJECT_GET_CLASS
      (obj));
  g_signal_emit (obj, mixin_cls->search_state_changed_signal_id, 0,
                 search_state );
}

void
_gabble_search_mixin_set_search_state (GObject *obj, guint state )
{
  GabbleSearchMixin *mixin = GABBLE_SEARCH_MIXIN (obj);  
  mixin->search_state = state;
}


