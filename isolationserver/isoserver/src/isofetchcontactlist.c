/*
* ============================================================================
*  Name        : isofetchcontactlist.h
*  Part of     : isolation server.
*  Version     : %version: 17 %
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <dbus/dbus-glib.h>
#include <glib.h>

#include "isofetchcontactlist.h"
#include "isoutils.h"
#include "msg_enums.h"
#include "msgliterals.h"

const guint KMaxContactFetchCount = 500;

static guint cur_mem_len;
static guint local_mem_len;
static guint remote_mem_len;

static gint cur_last_index = 0;
static gint local_last_index = 0;
static gint remote_last_index = 0;


/*! \brief This function is called as a callback to get the contact names from 
 *  contact handles. Contact names are sent to client in multiple packets of
 *  size MAX_MSG_SIZE
 *
 *  \param proxy unused
 *  \param handles_names 2D array of contact names
 *  \param error error if any 
 *  \param userdata msg_type
 */
static void inspect_handles_cb( DBusGProxy *proxy,char **handles_names,
				 GError *error, gpointer userdata ) 
	{
	guint i;
	gint err = 0;
	gint* msg_type = ( gint* ) userdata;
	
	gchar** members = NULL;
	guint* mem_count = 0;
	gint* last_index = NULL;
	
	
	iso_logger( "%s", "in -- inspect_handles_cb" );
	
	if ( !handles_names || error )
		{
		//Handles error
		iso_logger( "%s", "handle names error" );
		free ( msg_type );
		g_error_free(error);
		return;
		}
	UNUSED_FORMAL_PARAM(proxy);
	//Switch to the type of the message
	//and get corresponding pointers to members and pointer to last index
	switch ( *msg_type ) 
		{
		case ECurrent_Member_Contacts:
			members = globalCon.current_members_names;
			mem_count = &cur_mem_len;
			last_index = &cur_last_index;
			iso_logger( "%s count %d", "current members", mem_count );
			break;
		case ELocal_Pending_Contacts:
			members = globalCon.local_pending_names;
			mem_count = &local_mem_len;
			last_index = &local_last_index;
			iso_logger( "%s count %d", "local members", mem_count );
			break;
		case ERemote_Pending_Contacts:
			members = globalCon.remote_pending_names;
			mem_count = &remote_mem_len;
			last_index = &remote_last_index;
			iso_logger( "%s count %d", "remote members", mem_count );
			break;
			
		default :
		//Error in msg_type
		//Wrong message type
		//quit from here
			free ( msg_type );
			return;
		}
	
	
	//Add the contacts to the global member
	for( i=0; handles_names[i]; i++ )
		{
		iso_logger( "members %s ",handles_names[i] );
		members[*last_index] = handles_names[i];
		(*last_index)++;
		}
		
	iso_logger( "last index is %d", *last_index );
	//If all the contacts are fetched send them to client
	if ( *mem_count == *last_index )	
		{
		//send all contacts to client
		err = send_contacts_to_client( members, *mem_count, *msg_type );
		*last_index = 0;
		if ( err ) 
			{
			//How to handle the error?
			for ( i = 0; i < *mem_count; i++ ) 
				{
				free ( members[i] );
				}
			iso_logger( "%s", "There was error in send_contacts_to_client" );
			free ( msg_type );
			*mem_count = 0;
			return;
			}
		*mem_count = 0;
		
		}
	free ( msg_type );
	iso_logger( "%s", "out -- inspect_handles_cb" );
	}
	
/*! \brief This function is called if there are 0 contacts or there was an error
 *
 *  \param contact_type : Type of the contacts
 *  \param err : err if any, 0 otherwise
 */
static gint send_fetch_contacts_error( gint contact_type , gint err ) 
	{
	
	int result = 0;
	int pri = MSG_PRI_NORMAL;
	int timeout1 = TIME_OUT;
	message_hdr_resp* msg_struct = NULL;
	//control is here coz.. there are no contacts in list
	
	msg_struct = ( message_hdr_resp* ) malloc ( sizeof( message_hdr_resp ) );
	if ( NULL == msg_struct ) 
		{
		return 	MEM_ALLOCATION_ERROR;
		}
	//initalize memory to zero
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	//create the header response
	msg_struct->hdr_req.message_type = contact_type;
	msg_struct->hdr_req.protocol_id = 0;
	msg_struct->hdr_req.session_id = 0;
	msg_struct->hdr_req.request_id = 0;
	//Set error types and continue flag
	if ( err < 0 ) 
		{
		//There was some error, pass on the mapped error
		//to client
		msg_struct->error_type = err; 
		msg_struct->response = 0;	
		}
	else 
		{
		msg_struct->error_type = 0;
		msg_struct->response = 1;	
		}
	msg_struct->continue_flag = 0;
	//send the message to client that there are no contacts in the list
	result = MsgQSend( RESPONSE_QUEUE, ( char* )msg_struct, sizeof( message_hdr_resp ),
	 					pri, timeout1, &err );	
	
	free ( msg_struct );
	 					
	if ( result < 0 )
		{
		// failed to deliever
		return MSG_Q_SEND_FAILED;
		}
	return 0;
	}

/*! \brief This function is a callback for request for the contacts 
 *  in local pending contatct list from server. This is 
 *  only called for publish channel members. fetch_contacts is called 
 *  to get the contact names from the handles. If there are no conatcts in 
 *  the list it calls send_fetch_contacts_error else requests for the contact names
 *  from the contact handles
 *
 *  \param proxy unused 
 *  \param current_members array of handles of current contatcs
 *  \param local_pending_members unused 
 *  \param remote_pending_members array of handles of remote pending contatcs
 *  \param error error if any
 *  \param userdata unused 
 */
static void get_roster_local_member_cb( DBusGProxy *proxy,GArray* current_members, 
			GArray* local_pending_members, GArray* remote_pending_members, 
			GError *error, gpointer userdata ) 
	{
	
	gint err = 0;
	gint* msg_type = ( gint* ) malloc ( sizeof ( gint ) );
	
	iso_logger( "%s", "in -- get_roster_member_cb_local" );
	UNUSED_FORMAL_PARAM(proxy);
	UNUSED_FORMAL_PARAM(remote_pending_members);
	UNUSED_FORMAL_PARAM(userdata);
	UNUSED_FORMAL_PARAM(current_members);
	if ( NULL != error ) 
		{
		//err in fetching contacts, send failed
		//map the error code to isoserver
		err =  error->code;
		//Send that there are no contacts in the local pending
		send_fetch_contacts_error( EFetch_Contacts_Error, err );
		return;
		}
	//store the no. of lccal pending contacts
	local_mem_len = local_pending_members->len; 
	
	if ( !local_last_index && local_mem_len ) 
		{
		//Is this the first time to be here..
		//allocate memory
		globalCon.local_pending_names = g_new0 ( char *, local_mem_len + 1);	
		if ( NULL == globalCon.local_pending_names ) 
			{
			return ;
			//err?
			}
		}
	//ownership of this pointer is trnasfered to fetch_contacts
	*msg_type = ELocal_Pending_Contacts;
	//Fetch the contacts given the handles
	err = fetch_contacts( local_pending_members, msg_type );
	if ( err < 0 ) 
		{
		return;
		}
		
	iso_logger( "%s", "out -- get_roster_member_cb_local" );		
	}

/*! \brief Requests the server to get contact names from the handles
 *  The contact names are returned in inspect_handles_cb. From there
 *  contacts are sent to client
 *
 *  \param contact_mem : handles to contact members
 *  \param msg_type : type of the message, which wil be passed to cb
 *  \return  Errors if any, else 0
 */
static gint fetch_contacts( GArray* contact_mem, gint* msg_type ) 
	{
	
	guint fetch_count=0;
	guint fetch_loop_count=0;	
	GArray* fetch_members = NULL;
	
	DBusGProxyCall* call_id = NULL;
	TpConn* conn = globalCon.conn;
	
	gint i = 0;
	
	//allocate memory for the contacts to be fetched	
	fetch_members = g_array_new (FALSE, FALSE, sizeof (guint32));
	if ( NULL == fetch_members ) 
		{
		free ( msg_type );
		return MEM_ALLOCATION_ERROR;
		//err?
		}
	
	//check if there are contacts to be retrived 
	if ( contact_mem && contact_mem->len > 0 )
		{

		fetch_count = 0;
	
		//Divide contacts into a 'n' no. of groups .. n is calculated as below..
		fetch_loop_count = ( contact_mem->len ) / KMaxContactFetchCount + ( 
				( contact_mem->len % KMaxContactFetchCount ) ? 1 : 0 );
		
		//Get the contacts in terms of fetch_count
		for ( i=0; i<fetch_loop_count; i++ )
			{
			g_array_remove_range( fetch_members,0,fetch_count);
			
			fetch_count = ( contact_mem->len <= KMaxContactFetchCount ) 
					? contact_mem->len : KMaxContactFetchCount;
			//Fetch the contacts 
			g_array_append_vals (fetch_members,contact_mem->data,fetch_count);
			g_array_remove_range(contact_mem,0,fetch_count);
			//For the given handles contacts will be fetched in inspect_handles_cb
			call_id = tp_conn_inspect_handles_async( DBUS_G_PROXY(conn),
					       TP_CONN_HANDLE_TYPE_CONTACT ,fetch_members,
					       inspect_handles_cb, msg_type );
			//If call_id is null return from here..					       
			if ( NULL == call_id ) 
				{
				free ( msg_type );
				g_array_free( fetch_members, TRUE );
				return MEM_ALLOCATION_ERROR;	
				}
					       	
			}
		}
	else if ( contact_mem && contact_mem->len == 0 ) 
		{
		gint err = 0;
		//There are no contacts for this type(cur or local or remote) of contacts
		//Send that to client
		err = send_fetch_contacts_error( *msg_type, 0 );
		free ( msg_type );
		if ( err < 0 ) 
			{
			//Free the msg_type.. Ownership was transfered to this function
			
			g_array_free( fetch_members, TRUE );
			return err;	
			}
		
		}
	//msg_type is either freed in else part or is freed in inspect_handles_cb
	//no need to free here
	g_array_free( fetch_members, TRUE );
	//Should the fetch_contacts be deleted?	
	return 0;	
	}
	
/*! \brief This function is a callback for request for the contacts 
 *  in msg_type(subscribe or publish) contatct list from server. This is 
 *  only called for subscribe channel members. fetch_contacts is called 
 *  to get the contact names from the handles. If there are no conatcts in 
 *  the list it calls send_fetch_contacts_error else requests for the contacts 
 *  from the handles
 *
 *  \param proxy unused 
 *  \param current_members array of handles of current contatcs
 *  \param local_pending_members unused 
 *  \param remote_pending_members array of handles of remote pending contatcs
 *  \param error error if any
 *  \param userdata unused 
 */
static void get_roster_member_cb( DBusGProxy *proxy,GArray* current_members, 
			GArray* local_pending_members, GArray* remote_pending_members, 
			GError *error, gpointer userdata ) 
	{
	
	int err = 0;
	
	guint j = 0;
	
	iso_logger( "%s", "in -- get_roster_member_cb" );
	UNUSED_FORMAL_PARAM(proxy);
	UNUSED_FORMAL_PARAM(userdata);
	UNUSED_FORMAL_PARAM(local_pending_members);
	if ( NULL != error ) 
		{
		//err in fetching contacts, send failed
		//map the error code to isoserver
		err =  error->code;
		send_fetch_contacts_error( EFetch_Contacts_Error, err );
		return;
		}
	//store the no. of current members
	cur_mem_len = current_members->len;
	//store the no. of remote members
	remote_mem_len = remote_pending_members->len;
	
				
	if ( !cur_last_index && cur_mem_len ) 
		{
		//Is this the first time to be here..
		//allocate memory
		globalCon.current_members_names = g_new0 ( char *, cur_mem_len + 1);	
		if ( NULL == globalCon.current_members_names ) 
			{
			return ;
			//err?
			}
		}
		
	if ( !remote_last_index && remote_mem_len ) 
		{
		//Is this the first time to be here..
		//allocate memory
		globalCon.remote_pending_names = g_new0 ( char *, remote_mem_len + 1);	
		if ( NULL == globalCon.remote_pending_names ) 
			{
			return ;
			//err?
			}
		}
		
	
	//loop thru : 2 for current and remote
	for ( j = 0; j < 2; j++ ) 
		{
		//This msg_type is sent to inspect_handles_cb
		//Where it wil be deleeted
		gint* msg_type = ( gint* ) malloc ( sizeof ( gint ) );
		GArray* contact_mem = NULL;
		switch ( j ) 
			{
			
			case 0 : 
			//ask for the current members first
				*msg_type = ECurrent_Member_Contacts;
				contact_mem = current_members;
				break;
				
			case 1 :
			//ask for remote pending contacts
				*msg_type = ERemote_Pending_Contacts;
				contact_mem = remote_pending_members;
				break;
				
			default:
				break;
			}
		//Fetch the contacts given the handles 
		//ownership of msg_type is transfered to fetch_contacts
		err = fetch_contacts( contact_mem, msg_type );
		if ( err < 0 ) 
			{
			//There was some error
			//return from here
			return;
			}
		
		}

	iso_logger( "%s","out -- get_roster_member_cb" );
	}


/*! \brief this function requests for the contacts in its contatct list from server
 *  
 *  \param  type of the contacts channel( publish, subscribe )
 */
void request_roster( enum channel_type type )
	{

   	DBusGProxyCall* call_id = NULL;
   	
   
   	iso_logger( "%s", "in -- request_roster" );

	//With the interface get the contacts 
	//Request for all of the contacts in subscribe channel( remote and current 
	//members )
	if ( ESubscribe_Channel == type ) 
		{
		call_id = tp_chan_iface_group_get_all_members_async( globalCon.group_iface_subscribe, 
				get_roster_member_cb, NULL );
		//If call_id is NULL return from here..
		if ( NULL == call_id ) 
			{
			return ;
			}	
		}
	else if ( EPublish_Channel == type ) 
		{
		//Request for all of the contacts in publish channel
		call_id = tp_chan_iface_group_get_all_members_async( globalCon.group_iface_publish, 
				get_roster_local_member_cb, NULL );
		//If call_id is NULL return from here..
		if ( NULL == call_id ) 
			{
			return ;
			}
		}
	
	iso_logger( "%s", "out -- request_roster" );	
	}

/*! \brief Sends contacts to the client. Contact names are sent to client in multiple 
 *  packets of size less than or equal to MAX_MSG_SIZE. Contact will be sent as a whole. 
 *  A single contact will not be sent in two consecutive packets
 *
 *  \param members : Contacts to be sent to client
 *  \param mem_count : no. of contacts 
 *  \param contact_type : type of the contact
 *  \return error code..
 */
gint send_contacts_to_client( gchar** members, gint mem_count, 
	gint contact_type ) 
	{
	int result = 0;
	int pri = MSG_PRI_NORMAL;
	int timeout1 = TIME_OUT;
	int err = 0;
	
	char* memBuf = NULL;
	int i = 0;
	int one_msg_size = sizeof ( message_hdr_resp );
	int mem_len = 0;
	message_hdr_resp* msg_struct = NULL;
	
	iso_logger( "%s", "in -- send_contacts_to_client" );
	
	//allocate memory and check for the returned value
	msg_struct = ( message_hdr_resp* ) malloc ( sizeof( message_hdr_resp ) );
	if ( NULL == msg_struct ) 
		{
		return 	MEM_ALLOCATION_ERROR;
		}
		
	memBuf = ( gchar* ) malloc( MAX_MSG_SIZE );
	if ( NULL == memBuf ) 
		{
		// ? 
		free ( msg_struct );
		return MEM_ALLOCATION_ERROR;
		}

	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	//create the response header with type contact fetch
	msg_struct->hdr_req.message_type = contact_type;
	msg_struct->hdr_req.protocol_id = 0;
	msg_struct->hdr_req.session_id = 0;
	msg_struct->hdr_req.request_id = 0;
	
	memset( memBuf, '\0', MAX_MSG_SIZE );
	//sending the req to server
	
	//assuming that one contact size will not be 
	//greater than max_msg_size
	for ( i = 0; i <= mem_count - 1; i++ ) 
		{
		mem_len = strlen ( members[i] ) + 1;
		//check if this contact fits in the current message 
		//to be sent to client
		//one_msg_size is used to navigate thru' the buffer
		if ( one_msg_size + mem_len < MAX_MSG_SIZE ) 
			{
			strcpy( memBuf + one_msg_size, members[i] );
			one_msg_size += mem_len;
			//if i == mem_count - 1 no need to continue
			//Control should fall off(i.e., contacts fit in
			//this message )
			if ( i < mem_count - 1 )
				{
				continue;	
				}
			//If falling off mem_len should be reset because
			//it has been already added to one_msg_size
			mem_len = 0;
			}
		msg_struct->error_type = 0;
		msg_struct->response = 1;	
		//if i == mem_count - 1 and all contacts fits in this message
		//to be sent to server
		if ( i == mem_count - 1 && one_msg_size + mem_len < MAX_MSG_SIZE )	
			{
			msg_struct->continue_flag = 0;	
			}
		else 
			{
			msg_struct->continue_flag = 1;	
			}
		if ( i == mem_count - 1 && one_msg_size + mem_len > MAX_MSG_SIZE ) 
			{
			//One more contact is there (in which case i == mem_count - 1))
			//And that can't be sent because of limitation of 
			//message size..
			//so iterate one more time
			i--;
			}
		//Append '\0' to mark the end..
		one_msg_size++;
		*( memBuf + one_msg_size ) = '\0';
		memcpy( memBuf, msg_struct, sizeof( message_hdr_resp ) );
		//send to the client
		result = MsgQSend( RESPONSE_QUEUE, memBuf, one_msg_size, pri,
				 timeout1, &err );
				 
		
		if ( result < 0 )
			{
			free ( memBuf );
			iso_logger( "%s", "in -- send_contacts_to_client MSG_Q_SEND_FAILED" );
			// failed to deliever	
			//free the allocated memory
			free ( msg_struct );
			
			return MSG_Q_SEND_FAILED;
			}
		
		memset( memBuf , '\0', MAX_MSG_SIZE );
		//reset the one_msg_size for new message to be sent 
		one_msg_size = sizeof ( message_hdr_resp );	

		}
	//free the buffer
	free ( memBuf );
	
	free ( msg_struct );
	//Should the members be freed here?
	for ( i = 0; i < mem_count; i++ ) 
		{
		iso_logger( "freeing contact %s", members[i] );
		free ( members[i] );
		}
	iso_logger( "%s", "out -- send_contacts_to_client" );
			
	return 0;
	}

/*! \brief Documented in the header file
*/
void fetch_cached_contacts()
    {
    
    request_roster( EPublish_Channel );
    request_roster( ESubscribe_Channel );
    
    }
