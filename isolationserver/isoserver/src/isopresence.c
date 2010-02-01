/*
* ============================================================================
*  Name        : isopresence.c
*  Part of     : isolation server.
*  Version     : %version: 18 %
*
*  Copyright © 2007-2008 Nokia.  All rights reserved.
*  All rights reserved.
*  Redistribution and use in source and binary forms, with or without modification, 
*  are permitted provided that the following conditions are met:
*  Redistributions of source code must retain the above copyright notice, this list 
*  of conditions and the following disclaimer.Redistributions in binary form must 
*  reproduce the above copyright notice, this list of conditions and the following 
*  disclaimer in the documentation and/or other materials provided with the distribution.
*  Neither the name of the Nokia Corporation nor the names of its contributors may be used 
*  to endorse or promote products derived from this software without specific prior written 
*  permission.
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
*  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
*  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
*  SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
*  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
*  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
*  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* ============================================================================
* Template version: 1.0
*/

#include "isopresence.h"
#include "stdio.h"
#include "isoservermain.h"
#include "msgliterals.h"
#include "msgqlib.h"
#include "msg_enums.h"
#include "isoutils.h"

//#include "config.h"

#include <string.h>
#include <stdlib.h>

#ifndef SYMBIAN
#include <glib/gi18n.h>
#include <libintl.h>
#define _(x) dgettext (GETTEXT_PACKAGE, x)
#define N_(x) x
#else
#define _(x) x
#define N_(x) x
#endif

/*!	\file 
*	Impliments the functions in isomodifycontactlist.h
*/

#define GET_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TELEPATHY_TYPE_PRESENCE, TelepathyPresencePriv))

/*!	\var glob_handle_name accessed in multiple functions. Stores the 
*	contact name
*	\remark This global variable can be avoided
*/
gchar *              glob_handle_name;

/*!	\typedef long typedefed to TelepathyTime
*/
typedef long TelepathyTime;         /* Note: Always in UTC. */


/*!	\typedef long struct _TelepathyPresencePriv to TelepathyPresencePriv 
*/
typedef struct _TelepathyPresencePriv TelepathyPresencePriv;

/*!	\struct _TelepathyPresencePriv
*	\brief telepathy presence object
*
*	\var state presence state
*	\var status status text
*	\var resource GObject resource
*	\var priority GObject priority
*	\var timestamp set with current time
*/
struct _TelepathyPresencePriv {
	TelepathyPresenceState  state;

	gchar               *status;
	gchar               *resource;

	gint                 priority;
	TelepathyTime           timestamp;
};

/*! \enum 
*	\brief _TelepathyPresencePriv member variables
*/
enum {
	PROP_0,
	PROP_STATE,
	PROP_STATUS,
	PROP_RESOURCE,
	PROP_PRIORITY
};

G_DEFINE_TYPE (TelepathyPresence, telepathy_presence, G_TYPE_OBJECT);

/*! \brief Documented in the header file
*/
TelepathyTime
telepathy_time_get_current (void)
{
	return time (NULL);
}

/*! \brief Documented in the header file
*/
static void
telepathy_presence_class_init (TelepathyPresenceClass *presclass)
{
	GObjectClass *object_class;

	object_class = G_OBJECT_CLASS (presclass);

	object_class->finalize     = presence_finalize;
	object_class->get_property = presence_get_property;
	object_class->set_property = presence_set_property;

	g_object_class_install_property (object_class,
					 PROP_STATE,
					 g_param_spec_int ("state",
							   "Presence State",
							   "The current state of the presence",
							   TELEPATHY_PRESENCE_STATE_AVAILABLE,
							   TELEPATHY_PRESENCE_STATE_EXT_AWAY,
							   TELEPATHY_PRESENCE_STATE_AVAILABLE,
							   G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_STATUS,
					 g_param_spec_string ("status",
							      "Presence Status",
							      "Status string set on presence",
							      NULL,
							      G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_RESOURCE,
					 g_param_spec_string ("resource",
							      "Presence Resource",
							      "Resource that this presence is for",
							      NULL,
							      G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_PRIORITY,
					 g_param_spec_int ("priority",
							   "Presence Priority",
							   "Priority value of presence",
							   G_MININT,
							   G_MAXINT,
							   0,
							   G_PARAM_READWRITE));

	g_type_class_add_private (object_class, sizeof (TelepathyPresencePriv));
}

/*! \brief Documented in the header file
*/
static void
telepathy_presence_init (TelepathyPresence *presence)
{
	TelepathyPresencePriv *priv;

	priv = GET_PRIV (presence);

	priv->state = TELEPATHY_PRESENCE_STATE_AVAILABLE;

	priv->status = NULL;
	priv->resource = NULL;

	priv->priority = 0;

	priv->timestamp = telepathy_time_get_current ();
}

/*! /brief GObject finalize funtion
*
*   /param object object to be finalized
*/
static void
presence_finalize (GObject *object)
{
	TelepathyPresencePriv *priv;

	priv = GET_PRIV (object);

	g_free (priv->status);
	g_free (priv->resource);

	(G_OBJECT_CLASS (telepathy_presence_parent_class)->finalize) (object);
}

/*! /brief GObject get property funtion
*
*   /param object Gobject
*   /param param_id
*   /param value
*   /param pspec
*/
static void
presence_get_property (GObject    *object,
		       guint       param_id,
		       GValue     *value,
		       GParamSpec *pspec)
{
	TelepathyPresencePriv *priv;

	priv = GET_PRIV (object);

	switch (param_id) {
	case PROP_STATE:
		g_value_set_int (value, priv->state);
		break;
	case PROP_STATUS:
		g_value_set_string (value,
				    telepathy_presence_get_status (TELEPATHY_PRESENCE (object)));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
		break;
	}
}

/*! /brief GObject set property funtion
*
*   /param object Gobject
*   /param param_id
*   /param value
*   /param pspec
*/  
static void
presence_set_property (GObject      *object,
		       guint         param_id,
		       const GValue *value,
		       GParamSpec   *pspec)
{
	TelepathyPresencePriv *priv;

	priv = GET_PRIV (object);

	switch (param_id) {
	case PROP_STATE:
		priv->state = g_value_get_int (value);
		break;
	case PROP_STATUS:
		telepathy_presence_set_status (TELEPATHY_PRESENCE (object),
					    g_value_get_string (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
		break;
	}
}

/*! /brief create a presence object instance
 *  /return created presence object instance
 */

TelepathyPresence *
telepathy_presence_new (void)
{
	return g_object_new (TELEPATHY_TYPE_PRESENCE, NULL);
}

/*! /brief get the presence status value
 *
 *  /param presence : presence object
 *  /return state : status as set in presence object
 */
const gchar *
telepathy_presence_get_status (TelepathyPresence *presence)
{
	TelepathyPresencePriv *priv;

	g_return_val_if_fail (TELEPATHY_IS_PRESENCE (presence), NULL);
	
	priv = GET_PRIV (presence);

	return priv->status;
}

/*! /brief get the presence state value
 *
 *  /param presence : presence object
 *  /return state : state as set in presence object
 */
TelepathyPresenceState
telepathy_presence_get_state (TelepathyPresence *presence)
{
	TelepathyPresencePriv *priv;

	g_return_val_if_fail (TELEPATHY_IS_PRESENCE (presence),
			      TELEPATHY_PRESENCE_STATE_AVAILABLE);

	priv = GET_PRIV (presence);

	return priv->state;
}

/*! /brief set the presence state value in presence gobject
 *
 *  /param presence : presence object
 *  /param state : state
 *  /return : void
 */
void
telepathy_presence_set_state (TelepathyPresence      *presence,
			   TelepathyPresenceState  state)
{
	TelepathyPresencePriv *priv;

	g_return_if_fail (TELEPATHY_IS_PRESENCE (presence));

	priv = GET_PRIV (presence);

	priv->state = state;

	g_object_notify (G_OBJECT (presence), "state");
}

/*! /brief set the presence status text value in presence gobject
 *
 *  /param presence : presence object
 *  /param state : status
 *  /return : void
 */
void
telepathy_presence_set_status (TelepathyPresence *presence,
			    const gchar    *status)
{
	TelepathyPresencePriv *priv;

	g_return_if_fail (TELEPATHY_IS_PRESENCE (presence));

	priv = GET_PRIV (presence);

	g_free (priv->status);

	if (status) {
		priv->status = g_strdup (status);
	} else {
		priv->status = NULL;
	}

	g_object_notify (G_OBJECT (presence), "status");

}

/*! /brief converts presence state value from enum to string
 *  /param presence_state : presence state enum value
 *  /return : presence state string value
 */
const gchar *
telepathy_presence_state_to_str (TelepathyPresenceState presence_state)
{

	switch (presence_state) {
	case TELEPATHY_PRESENCE_STATE_AVAILABLE:
		return "available";
	case TELEPATHY_PRESENCE_STATE_BUSY:
		return "dnd";
	case TELEPATHY_PRESENCE_STATE_AWAY:
		return "away";
	case TELEPATHY_PRESENCE_STATE_EXT_AWAY:
		return "xa";
	case TELEPATHY_PRESENCE_STATE_HIDDEN:
		return "hidden";
	case TELEPATHY_PRESENCE_STATE_UNAVAILABLE:
		return "offline";
	default:
		return NULL;
	}
}

/*! /brief converts presence state value from string to enum
 *
 *  /param str : presence state string value
 *  /return : presence state enum value
 */
TelepathyPresenceState
telepathy_telepathy_presence_state_from_str (const gchar *str)
{

	if (strcmp (str, "available") == 0) {
		return TELEPATHY_PRESENCE_STATE_AVAILABLE;
	} else if ((strcmp (str, "dnd") == 0) || (strcmp (str, "busy") == 0)) {
		return TELEPATHY_PRESENCE_STATE_BUSY;
	} else if ((strcmp (str, "away") == 0) || (strcmp (str, "brb") == 0)) {
		return TELEPATHY_PRESENCE_STATE_AWAY;
	} else if (strcmp (str, "xa") == 0) {
		return TELEPATHY_PRESENCE_STATE_EXT_AWAY;
	} else if (strcmp (str, "hidden") == 0) {
		return TELEPATHY_PRESENCE_STATE_HIDDEN;
	} else if (strcmp (str, "offline") == 0) {
		return TELEPATHY_PRESENCE_STATE_UNAVAILABLE;
	} 

	return TELEPATHY_PRESENCE_STATE_AVAILABLE;
}

/*! /brief updates own presence to the n/w server. update_status_cb 
 *  is registered for the callback
 *
 *  /param pres_iface : presence interface
 *  /param presence : presence object 
 *  /param msg_hdr : presence info list to send presence_msg_struct
 *  has state, status etc., 
 *  /return : void
 */
void
telepathy_contacts_send_presence (DBusGProxy *pres_iface,
					 TelepathyPresence          *presence,
					 presence_msg_struct* msg_hdr)
{
	GHashTable          *status_ids;
	GHashTable          *status_options;
	const gchar         *status_id;
	const gchar         *message;
	TelepathyPresenceState  presence_state;
	GValue               value_message = {0, };

	if (!pres_iface) {
		return;
	}
  	iso_logger( "%s", "In - telepathy_contacts_send_presence\n" );

	status_ids = g_hash_table_new_full (g_str_hash,
					    g_str_equal,
					    g_free,
					    (GDestroyNotify) g_hash_table_destroy);
	status_options = g_hash_table_new_full (g_str_hash,
						g_str_equal,
						NULL,
						(GDestroyNotify) g_value_unset); //ToDo: get clarity on freeing this resource

	presence_state = telepathy_presence_get_state (presence);
	status_id = telepathy_presence_state_to_str (presence_state);
	message = telepathy_presence_get_status (presence);

	if (message) {
		g_value_init (&value_message, G_TYPE_STRING);
		g_value_set_string (&value_message, message);
		g_hash_table_insert (status_options, "message", &value_message);
	}

	g_hash_table_insert (status_ids,
			     g_strdup (status_id),
			     status_options);

	tp_conn_iface_presence_set_status_async(DBUS_G_PROXY(pres_iface),
						status_ids,
						update_status_cb,
						(gpointer)msg_hdr->hdr_req );

	g_hash_table_destroy (status_options); //ToDo: get clarity on freeing this resource
	g_hash_table_destroy (status_ids);
  	iso_logger( "%s", "Out - telepathy_contacts_send_presence\n" );

}

/*! /brief This function will be called for presence of each contact
 *  key has got the state and value status
 *
 *  /param key : hash table key ie presence state
 *  /param value : hash table value ie presence information
 *  /param presence : user data
 *  /remark presence object is logically unused ?
 *
 *  /return : void
 */
static void
telepathy_presences_foreach (gpointer        *key,
		      gpointer        *value,
		      TelepathyPresence **presence)
{

	TelepathyPresenceState state;
	const gchar        *message = NULL;

	*presence = telepathy_presence_new();
  	iso_logger( "%s", "In - telepathy_presences_foreach\n" );
  	
  	iso_logger( "%s%s", "state : ", (gchar *)key );

	state = telepathy_telepathy_presence_state_from_str((const gchar *)
							 key);
	telepathy_presence_set_state(*presence, state);

	message = g_value_get_string((GValue *)
				     g_hash_table_lookup((GHashTable *)
							 value, "message"));
  	iso_logger( "%s%s", "message : ", (const gchar *)message );

	send_presence_to_client((const gchar *)key, message );
	
	if (*presence) {
		g_object_unref (*presence);
	}
	
  	iso_logger( "%s", "Out - telepathy_presences_foreach\n" );
		
}

/*! /brief This function will be called for each contact. Contact name is 
 *  got from the contcat handle, telepathy_presences_foreach is called 
 *  to get the presence from value of the hash table
 *
 * @param key : hash table key ie contact handle id
 * @param value : hash table value ie presence
 * @return : void
 */
static void
telepathy_presence_handle_foreach (gpointer        *key,
		      gpointer        *value)
{
	gint                 handle_id;
	GHashTable          *presences_hash;
	GValue              *gvalue;
	GObject      		*presence;
	gchar              **handle_name = NULL;
	GArray              *handles;
	GError         		*error = NULL;

	handle_id = GPOINTER_TO_INT(key);

  	iso_logger( "%s", "In - telepathy_presence_handle_foreach\n" );
	
	handles = g_array_new (FALSE, FALSE, sizeof (gint));
	g_array_append_val (handles, handle_id);
	if(!tp_conn_inspect_handles(DBUS_G_PROXY(globalCon.conn),
			       TP_CONN_HANDLE_TYPE_CONTACT, handles,
			       &handle_name, &error))
		{
		iso_logger( "%s%s", "InspectHandle Error: ", error->message );
		g_clear_error (&error);
		g_array_free (handles, TRUE);
		if( handle_name )
		{
		g_strfreev (handle_name);
		}
		return;
		}

  	if (handle_name)
	  	{
	  	iso_logger( "%s%s", "handle_name : ", *handle_name );
		glob_handle_name = *handle_name;
	  	}
	
	gvalue = g_value_array_get_nth((GValueArray *) value, 1);
	
	presences_hash = (GHashTable *) g_value_get_boxed(gvalue);
	
	g_hash_table_foreach(presences_hash,
			     (GHFunc) telepathy_presences_foreach, &presence );
			     
	g_clear_error (&error);
	g_array_free (handles, TRUE);
	g_strfreev (handle_name);
			     
  	iso_logger( "%s", "Out - telepathy_presence_handle_foreach\n" );
	
}

/*! /brief Registered as a signal for presence updates of 
 *  other users. This function will be called whenevr a contact
 *  to whom this user has subscribed for presence changes his 
 *  presence state or status message.
 * 
 *  /param proxy : unused
 *  /param handle_hash : hash table containing presence information
 *  as hash value, contact handle as key
 *  /return : void
 */
void
telepathy_presence_handler(DBusGProxy      *proxy,
			   GHashTable      *handle_hash)
{
	GSList *data = NULL;
    UNUSED_FORMAL_PARAM(proxy);
  	iso_logger( "%s", "In - telepathy_presence_handler\n" );
	g_hash_table_foreach(handle_hash, (GHFunc) telepathy_presence_handle_foreach, data);
  	iso_logger( "%s", "Out - telepathy_presence_handler\n" );

}


/*! /brief sends presence of the client to n/w server thru' gabble
 *  calls telepathy_contacts_send_presence to send presence
 *
 *  /param msghdr request header that will be passed back to client
 *  /param message_type_err unused
 *  /remark please remove this unused variable(why was this added?)
 *
 *  /return : error code on failure, 0 on success
 */
void send_presence( presence_msg_struct* msg_hdr , gint message_type_err ) 
  {
  	TelepathyPresence        *presence;
	TelepathyPresenceState state;
    UNUSED_FORMAL_PARAM(message_type_err);
  
  	iso_logger( "%s", "In - send_presence\n" );

    //changing own presence status
	presence = telepathy_presence_new();
	state = telepathy_telepathy_presence_state_from_str(msg_hdr->useravailability);   
	telepathy_presence_set_state(presence, state);

	//if (msg_hdr->statustext != NULL)
		telepathy_presence_set_status (presence, msg_hdr->statustext);
	
    telepathy_contacts_send_presence(DBUS_G_PROXY(globalCon.pres_iface), presence, msg_hdr);
   	
   	g_object_unref (presence);
  	iso_logger( "%s", "Out - send_presence\n" );
  }

/*! /brief parses the message buffer. aPresenceStatus and aStatustext
 *  pointers updated
 *
 *  /param aMsgBuffer message buffer
 *  /param aPresenceStatus after call to this function it will
 *  have the presence status to be updated
 *  /param aStatustext after call to this function it will
 *  have the status text to be updated
 * 
 *  /param msg_len : msg_len no. of bytes in msg_buf
 *  /return returns error code on failure, or 0
 */
int parse_for_presence( gchar* msgbuffer, gchar** presencestatus, 
	gchar** statustext, gint msg_len ) 
{

   gint len = 0;
   gint err=0;
   gchar *user_availability =NULL;
   gchar *status_text =NULL;
   
   iso_logger( "%s", "In - parse_for_presence\n" );
   //calculating len for request header
   //skip the msg_hdr part
   len += sizeof( message_hdr_req );
   
   // reading a availability from buffer
   err = parse_a_string( msgbuffer, &user_availability, &len, msg_len );
   *presencestatus=user_availability;
   if ( err < 0 ) 
		{
		return err;
		}
   // reading a status text
   err = parse_a_string( msgbuffer, &status_text, &len, msg_len );
   *statustext=status_text;
  
iso_logger( "%s", "Out - parse_for_presence\n" );
// need to check.....
return 1;	
}

/*! /brief Parses the message from client and sends the presence 
 *  to n/w server. A err is returned if there is any parse error
 *  or out of memory condition
 *
 *  /param buf : buffer to be parsed
 *  /param buf_len : buffer length
 *  /return : error code if any, 0 on success 
 */
int action_parse_presence( gchar* buf, gint buf_len ) 

{
	gchar* presencestatus = NULL;
	gchar* statustext = NULL;
	message_hdr_req *msg_hdr = NULL;
	gint message_type_err = 0;
	presence_msg_struct* msg_struct = NULL;
	//parse user availability and status text .. 	
	iso_logger( "%s", "In - action_parse_presence\n" );
	message_type_err = parse_for_presence( buf, 
			&presencestatus,&statustext, buf_len );
		//if <= 0 there is some error in the message formation
	if ( message_type_err >  0 ) 
		{
		
		//send message
		msg_hdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
		if ( NULL == msg_hdr ) 
			{
			return MEM_ALLOCATION_ERROR;
			}
	    memset( msg_hdr, '\0', sizeof( message_hdr_req ) );
	    //read message header from buffer
	    memcpy( msg_hdr, buf, sizeof( message_hdr_req ) );
	    
	    //fill the contacts and message into a struct
	    msg_struct = ( presence_msg_struct* ) malloc ( sizeof ( presence_msg_struct ) );
	    if ( NULL == msg_struct ) 
			{
			free ( msg_hdr );
			return MEM_ALLOCATION_ERROR;
			}
		msg_struct->hdr_req = msg_hdr;
		msg_struct->useravailability = presencestatus;
		msg_struct->statustext = statustext;
		//send presence should be called
		 send_presence( msg_struct, message_type_err );		
		}
	
	//free ( msg_hdr );
	iso_logger( "%s", "Out - action_parse_send\n" );
	//return error on failure, or no. of contacts message sent to on success
	return message_type_err;
		
			
			
}


/*! /brief callback for updating own presence. This function 
 *  sends response to client
 *
 *  /param proxy : unused
 *  /param error : error if any
 *  /param message : message header  
 *  /return : void
 */
void update_status_cb(DBusGProxy *proxy, GError* error, gpointer userdata)
{

	int err = 0;

	message_hdr_req* msg_hdr = ( message_hdr_req* ) userdata;
	// create the msg queue
	//user data is of type message_hdr_req
	iso_logger( "%s", "In - update_status_cb\n" );
    UNUSED_FORMAL_PARAM(proxy);
	if ( NULL != error ) 
		{
		//There was some error
		//send the response for the msg_hdr request to client
		err = send_response_to_client( msg_hdr, error->code, 0 );
		}
	else 
		{
		//send the response for the msg_hdr request to client
		err = send_response_to_client( msg_hdr, 0, 1 );
		}
						
	//free( msg_hdr );

	if ( err < 0 )
		{
		// failed to deliver
		iso_logger( "%s", "failed to deliver\n" );
		}
	iso_logger( "%s", "Out - update_status_cb\n" );
		
}


/*! /brief Sends presence of conatct to the client. Forms 
 *  the proper presence message format and sends that to client
 *
 *  /param : availability user availability
 *  /param : text custom status message set by user
 *  /return : error code on failure, 0 on success
 */
int send_presence_to_client( const gchar *availability , const gchar *text )
	{
		int len=0;
		int pri = MSG_PRI_NORMAL;
		char rmsg[MAX_MSG_SIZE];
		int index = 0;
		int err;
		message_hdr_resp* msg_resp = NULL;
		int result = 0;
		int timeout = NO_WAIT;
	   
	   	iso_logger( "%s", "In - send_presence_to_client\n" );

	   	msg_resp = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
		if ( NULL == msg_resp ) 
			{
			return MEM_ALLOCATION_ERROR;	
			}
		memset( msg_resp, '\0', sizeof( message_hdr_resp ) );
	    memset( msg_resp, '\0', sizeof( message_hdr_resp ) );
		msg_resp->hdr_req.message_type = EPresenceNotification;
		msg_resp->hdr_req.protocol_id = 1;
		msg_resp->hdr_req.session_id = 1;
		msg_resp->hdr_req.request_id =1;
		msg_resp->response = 1;
		msg_resp->error_type = 0;
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		memcpy( rmsg, msg_resp, sizeof( message_hdr_resp ) );
	    index += sizeof( message_hdr_resp );
	    //putting sender in message queue
	    len = strlen(glob_handle_name);
	    strcpy( rmsg + index, glob_handle_name );
	    index += len + 1;
	    //putting user availability   
	    len = strlen(availability);
	    strcpy( rmsg + index, availability );
	    index += len + 1;
	    // puttin status text
	    len = strlen(text);
	    strcpy( rmsg + index, text );
	    index += len + 1;
	    result = MsgQSend( RESPONSE_QUEUE, rmsg, index, 
							pri, timeout, &err);
							
		free ( msg_resp );
	    if(result<0)
	    {
	    	return FALSE;
	    }
	   
	    
		iso_logger( "%s", "message is:\n");
	   	iso_logger( "%s", "Out - send_presence_to_client\n" );

		return TRUE;
   }
