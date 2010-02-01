/*
 * search-keys-info.c - Source for Jingle info discovery
 * Copyright (C) 2008 Collabora Ltd.
 *  and/or its subsidiaries. All rights reserved.
 *   @author Ole Andre Vadla Ravnaas <ole.andre.ravnaas@collabora.co.uk>
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

#include <string.h>
#include <stdlib.h>

#define DEBUG_FLAG GABBLE_DEBUG_MEDIA

#include "debug.h"
#include "search-keys-info.h"
#include "gabble-error.h"
#include "namespaces.h"
#include "util.h"

#ifdef DEBUG_FLAG
//#define DEBUG(format, ...)
#define DEBUGGING 0
#define NODE_DEBUG(n, s)
#endif /* DEBUG_FLAG */

/**
 * get_search_keys_info:
 *
 * get search keys 
 *
 */
void
get_search_keys_info (GabbleConnection *conn,  const gchar *jid )
{
  LmMessage *msg;
  LmMessageNode *query_node;
  gboolean result;
  gchar *service = NULL;
  GError *error = NULL;

  conn->search_service_jid = g_strdup( jid );
  
  msg= lm_message_new_with_sub_type ( jid,
                                      LM_MESSAGE_TYPE_IQ,
                                      LM_MESSAGE_SUB_TYPE_GET);
  query_node = lm_message_node_add_child (msg->node, "query", NULL);
  
  lm_message_node_set_attribute (query_node, "xmlns", NS_SEARCH);

  if (!_gabble_connection_send (conn, msg, &error))
    {
      g_warning ("%s: send failed: %s\n", G_STRFUNC, error->message);
      goto OUT;
    }

OUT:
  if (msg)
    lm_message_unref (msg);

  if (error)
    g_error_free (error);
 
}


/**
 * _gabble_submit_search_form:
 *
 * Submit search form 
 *
 */
gboolean
_gabble_submit_search_form (GabbleConnection *conn)
{
  LmMessage *msg;
  LmMessageNode *query_node;
  LmMessageNode *x_node;
  LmMessageNode *field_node, *value_node;
  gboolean result;
  const gchar *jid;
  gchar *service = NULL;
  GError **error = NULL;

  g_message("_gabble_submit_search_form\n");
  jid = gabble_handle_inspect (conn->handles, TP_HANDLE_TYPE_CONTACT, conn->self_handle);
  g_message("jid is %s\n",jid);
  gabble_decode_jid(jid, NULL /*username*/,&service,NULL/*resource*/);
  g_message("service is %s\n",service);
  
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
                                  
  field_node = lm_message_node_add_child ( x_node, "x", NULL );
  
  lm_message_node_set_attributes (x_node,
                                  "type", "hidden",
                                  "var", "FORM_TYPE",
                                  NULL); 
                                  
  value_node = lm_message_node_get_child(field_node, "value");
  lm_message_node_set_value(value_node,"NS_SEARCH");   
  
  g_message("_gabble_submit_search_form b4 _gabble_connection_send\n");                                                                
  
  result = _gabble_connection_send (conn, msg, error);
  lm_message_unref (msg);

  if (!result)
    return FALSE;
  
  return TRUE;
}


/**
 * search_keys_iq_cb
 *
 * Called by loudmouth when we get an incoming <iq>. This handler
 * is concerned only with Requesting Search Keys info queries.
 */
LmHandlerResult
search_keys_iq_cb (LmMessageHandler *handler,
                         LmConnection *lmconn,
                         LmMessage *message,
                         gpointer user_data)
{
  GabbleConnection *conn = GABBLE_CONNECTION (user_data);
  
  LmMessageSubType sub_type;
  LmMessageNode *query_node, *x_node, *result_node,*x_item_node;
  const gchar *str;
  const gchar *type, *field_label,*field_var, *field_type;
  guint  i = 0;
  
  if ( NULL == conn )
        {
        g_debug ("%s: accesing after dereferenced connection", G_STRFUNC);
        return LM_HANDLER_RESULT_ALLOW_MORE_HANDLERS;
        }
  
  query_node = lm_message_node_get_child_with_namespace (message->node,
      "query", NS_SEARCH);

  if (query_node == NULL)
    return LM_HANDLER_RESULT_ALLOW_MORE_HANDLERS;

  sub_type = lm_message_get_sub_type (message);
  
  if (sub_type != LM_MESSAGE_SUB_TYPE_RESULT)
    {
      return LM_HANDLER_RESULT_ALLOW_MORE_HANDLERS;
    }

  if (sub_type == LM_MESSAGE_SUB_TYPE_RESULT)
  {
	  x_node = lm_message_node_get_child_with_namespace (query_node, "x", NS_X_DATA);
	  
	  if( x_node )
			{
			    //if service supports data forms 
		    	result_node = x_node; 
				type   = lm_message_node_get_attribute (x_node, "type" );
				  
		       if (0 == strcmp (type, "form"))
		          {
		          gint filed_count = 0;
		          
		          for (x_item_node = result_node->children; x_item_node; x_item_node = x_item_node->next)
                      {
                      if( 0 == strcmp (x_item_node->name, "field") )
                          {
                          filed_count++;
                          }
                      }
		          
		          if(conn->search_key_names == NULL)
                        {
                          conn->search_key_names = (gchar**)g_new0( gchar*, filed_count);
                        }
                    
                  if(conn->search_key_ht == NULL)
                        {
                          conn->search_key_ht = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
                        }

		            conn->search_form = TRUE;
		            for (x_item_node = result_node->children; x_item_node; x_item_node = x_item_node->next)
		            {
		                
		                 // "reported" should also be used to get the types of search result fields
		                 //'g' type currently not supported by dbus-glib-binding
		                 if (0 != strcmp (x_item_node->name, "field") 
		                        && 0 != strcmp (x_item_node->name, "instructions"))
		                    continue;
		                    
		                
		                 if( 0 == strcmp (x_item_node->name, "instructions") )
		                     {
		                        //get the instructions
		                        g_message("get the instuction\n");
		                        conn->search_instr = g_strdup( lm_message_node_get_value(x_item_node) );    
		                        g_message("after gettting the instuction\n");
		                        continue;
		                     }
		                     
		                 if( 0 == strcmp (x_item_node->name, "field") )
		                    {
		                        //node is a field node
		                        //get the name of each field
		                        //type also ..later
		                        field_type = lm_message_node_get_attribute(x_item_node, "type");
		                        if( 0 == strcmp (field_type, "hidden") )
		                            {
		                            //see later if this must be sent while sending request to the server
		                            continue;   
		                            }
		                       //get the "type" of the attr as well along with "var"
		                       //to send it to the client later 
		                       //dbus-glib-binding tool support 'g' type 
		                       field_label = lm_message_node_get_attribute(x_item_node, "label");
		                      
		                       g_message("b4 gettting the search_key_names\n");
		                       conn->search_key_names[i] = g_strdup(field_label);
		                       g_message("after gettting the search_key_names\n");
		                       g_message("%s\n",conn->search_key_names[i]);
		                       field_var = lm_message_node_get_attribute(x_item_node, "var");
		                       g_hash_table_insert(conn->search_key_ht,g_strdup(field_label),g_strdup(field_var));
		                       i++;                    
		                       
		                    }
		               }
		               
		            conn->search_key_names[i] = NULL;
		            // this should be done when we need to give the 
		            // search result attributes in advance to the client
		            // and if we are able to give the type also     
		            
		        
		          }
		        
			}
	   else
	       {
	       return LM_HANDLER_RESULT_ALLOW_MORE_HANDLERS;
	       }
	    
	   return LM_HANDLER_RESULT_REMOVE_MESSAGE;
		  
      }

   else if (sub_type == LM_MESSAGE_SUB_TYPE_ERROR)
    {
      GabbleXmppError xmpp_error = INVALID_XMPP_ERROR;

      result_node = lm_message_node_get_child (message->node, "error");
      if (result_node)
        {
          xmpp_error = gabble_xmpp_error_from_node (result_node);
        }

      str = gabble_xmpp_error_string (xmpp_error);

      g_warning ("%s: jingle info error: %s", G_STRFUNC,
          (str) ? str : "unknown error");
    }
  else
    {
      NODE_DEBUG (message->node, "unknown message sub type");
    }

  return LM_HANDLER_RESULT_ALLOW_MORE_HANDLERS;
}


