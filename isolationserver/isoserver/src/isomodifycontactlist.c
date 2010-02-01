/*
* ============================================================================
*  Name        : isomodifycontacts.h
*  Part of     : isolation server.
*  Version     : %version: 16 %
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
#include <glib.h>
#include "msgliterals.h"
#include "isomodifycontactlist.h"
#include "isoutils.h"
#include "isofetchcontactlist.h" //For send_contact_to_client func

/*!	\file 
*	Impliments the functions in isomodifycontactlist.h
*/

/*!
 *  /brief This function is called by message_send_recv function in isoservermain.c if message type
 *  is EReject_Contact_Request, EAccept_Contact_Request, EAdd_Contact_Request and EDelete_Contact_Request. 
 *  Contacts added/removed to/from list. This function parses the message buffer, 
 *  validates for the arguments and calls modify_contact_list to add/remove contacts
 * 
 *  /param buf : buffer to be parsed
 *  /param buf_len : buffer length
 *  /return : error code if any, 0 on success 
 */
gint action_parse_modify_contact_list( gchar* buf, gint buf_len ) 
	{
	gchar** contactid = NULL;
	gchar* msg = NULL;
	gint meesage_type_err = 0;
	gint len = 0;
	gint contact_count = 0;
	
	//parse contacts, message.. 	
	iso_logger( "%s", "In - action_parse_modify_contact_list" );
	len += sizeof( message_hdr_req );
	//example send message - message header omitted..
	//test.ximp@gmail.com\0ximp.telepathy@gmail.com\0\0I would like to add u!\0
	//					  ^ 						^ ^	                      ^ 
	//contact1------------|contact2-----------------|-|message----------------|
	//Till Message part is parsed by the following function
	meesage_type_err = parse_into_array_of_strings( buf, &contactid, &len, buf_len, &contact_count );
	if ( meesage_type_err < 0 ) 
		{
		gchar* msg = NULL;
		//An error has occured, so propagate to next level
		free_msg_args( contactid, contact_count, msg );
		//got error, print to file
		return meesage_type_err;
		}
	
	//Parse the message
	//message is optional.. so check if message has been sent
	if ( len < buf_len ) 
		{
		
		gint err = 0;
		
	//	iso_logger( "%x", len );
    //		iso_logger( "%f", len );
		
		//parse for the message..
		err = parse_a_string( buf, &msg, &len, buf_len );
		
		if ( err < 0 ) 
			{
			//return err; 
			//ignoring the error..
			//as message is not that important
			msg = NULL;
			}
		}	
	//If there is atleast one contact to add
	if ( contact_count > 0 ) 
		{
		//Add the header and message to the msg struct
		//this struct will be used in the callbacks
		send_msg_struct* msg_struct = NULL;
		msg_struct = ( send_msg_struct* ) malloc( sizeof( send_msg_struct ) );
		if ( NULL == msg_struct ) 
			{
			//An error has occured, so propagate to next level
			free_msg_args( contactid, contact_count, msg );
			return MEM_ALLOCATION_ERROR;
			}
		//allocate memory and check for the returned value	
		msg_struct->hdr_req = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
		if ( NULL == msg_struct->hdr_req ) 
			{
			//An error has occured, so propagate to next level
			free_msg_args( contactid, contact_count, msg );
			free ( msg_struct );
			return MEM_ALLOCATION_ERROR;
			}
		//initalize to 0...
	    memset( msg_struct->hdr_req, '\0', sizeof( message_hdr_req ) );
	    //read message header from buffer
	    memcpy( msg_struct->hdr_req, buf, sizeof( message_hdr_req ) );
	    //message, msg can be NULL as well
	    msg_struct->msg = msg;
	    
		meesage_type_err = modify_contact_list( msg_struct, 
				( const char** )contactid, meesage_type_err );	
		if ( meesage_type_err < 0 ) 
			{
			//Do cleanup here
			//delete contact_ids and message
			//An error has occured, so propagate to next level
			free_msg_args( contactid, contact_count, msg );
			free ( msg_struct->hdr_req );
			free ( msg_struct );
			return meesage_type_err;
			}
		}
	
	iso_logger( "%s", "In - action_parse_modify_contact_list" );	
		
	return meesage_type_err;	
	}


/*!
 *  /brief gets the handles for contacts. Handles are returned in request_modify_contact_list_handles_cb 
 *  callback which is an arg for the async request to get handles.
 *  
 *  /param msg_hdr : The header and message passed to callbacks
 *  /param contact_id : contact ids whose handles to be returned
 *  /param no_cntcts : total no. of contacts to be added
 *  /return : error code if any, handle on success 
 */
guint get_handles_for_contacts ( send_msg_struct* msg_hdr, const gchar  **contact_id, 
		gint no_cntcts )
	{
	
	iso_logger( "%s", "in -- get_handles_for_contacts" );
	
	//check for the return value if 0 is ok or should some negative value to be passed
	g_return_val_if_fail (contact_id != NULL, 0); 	

	//Send request to get the handles for the contacts..
	//handles will be returned in request_addcontacts_handles_cb
	tp_conn_request_handles_async( DBUS_G_PROXY(globalCon.conn), 
			TP_CONN_HANDLE_TYPE_CONTACT, contact_id, 
	 		request_modify_contact_list_handles_cb, ( gpointer )msg_hdr );
	 		
	iso_logger( "%s", "in -- get_handles_for_contacts" );
	//no of contacts is returned on successful completion of getting handles
	return no_cntcts;
	}


/*!
 *  /brief Handles are returned in this callback. Which will then actually do modify 
 *  the contact list based on the request type(add/remove/accept/reject)
 *  modify_contact_list_cb is registered as the callback.
 *
 *  /param proxy : unused
 *  /param handles : handles of the contacts
 *  /param error : error if any
 *  /param message : userdata(send_msg_struct) passed back
 *  /return : error code if any, handle on success 
 */     
void request_modify_contact_list_handles_cb( DBusGProxy *proxy, GArray *handles, 
			GError* error, gpointer message ) 
	{
	send_msg_struct* msg_hdr = ( send_msg_struct* ) message;
    UNUSED_FORMAL_PARAM(proxy);
	iso_logger( "%s", "in -- request_modify_contact_list_handles_cb" );

	if ( !handles || error ) 
		{
		 send_response_to_client( msg_hdr->hdr_req,  error->code, 0 );
		
		if ( error ) 
			{
			g_error_free(error);
			}
		free ( msg_hdr->msg );
		free ( msg_hdr->hdr_req );
		free ( msg_hdr );
		iso_logger( "%s", "handle got is invalid" );
		return;
		}
		
	//Check if the request is to add contact or delete contact
	if ( EAdd_Contact_Request == msg_hdr->hdr_req->message_type ) 
		{
		//got the handles for contacts now send request to add those contacts
		tp_chan_iface_group_add_members_async ( globalCon.group_iface_subscribe, handles, 
			msg_hdr->msg, modify_contact_list_cb, msg_hdr->hdr_req );
		}
	else if ( EDelete_Contact_Request == msg_hdr->hdr_req->message_type ) 
		{
		//got the handles for contacts now send request to delete those contacts
		tp_chan_iface_group_remove_members_async ( globalCon.group_iface_known, handles, 
			msg_hdr->msg, modify_contact_list_cb, msg_hdr->hdr_req );	
		}
	else if ( EAccept_Contact_Request == msg_hdr->hdr_req->message_type ) 
		{
		//got the handles for contacts now send request to add those contacts
		tp_chan_iface_group_add_members_async ( globalCon.group_iface_publish, handles, 
			msg_hdr->msg, modify_contact_list_cb, msg_hdr->hdr_req );
		}
	else if ( EReject_Contact_Request == msg_hdr->hdr_req->message_type ) 
		{
		//got the handles for contacts now send request to delete those contacts
		tp_chan_iface_group_remove_members_async ( globalCon.group_iface_publish, handles, 
			msg_hdr->msg, modify_contact_list_cb, msg_hdr->hdr_req );	
		}
			
	//free handles
	g_array_free ( handles, TRUE );
	//free header
	//hdr_req not to be freed, it is freed by modify_contact_list_cb
	free ( msg_hdr->msg );
	free ( msg_hdr );
	iso_logger( "%s", "out -- request_modify_contact_list_handles_cb" );	
	}
	
/*!
 *  /brief Calls get_handles_for_contacts to get the handles for contact. 
 * 
 *  /param msg_hdr : message request header passed to callbacks
 *  /param contact_id : contact ids to be added
 *  /param message : messgae if any
 *  /param no_cntcts : no of contacts to be added
 *  /return void 
 */
gint modify_contact_list ( send_msg_struct* msg_hdr, const gchar  **contact_ids, 
				gint no_cntcts )
	{

	guint err = 0;
	
	iso_logger( "%s", "in -- modify_contact_list" );	
	
	//g_array_append_val ( handles, handle );
	//gets the handles for contacts
	//Handles are returned in request_addcontacts_handles_cb callback
	err = get_handles_for_contacts( msg_hdr, contact_ids, no_cntcts );
	
	iso_logger( "%s", "Out -- -- modify_contact_list" );
	
	return err;
	
	 	
	}

/*!
 *  /brief called as callback for request for modify contact list 
 *  This function then sends the result to client
 * 
 *  /param proxy unused 
 *  /param error Error if any
 *  /param userdata request message header
 *  /return void 
 */
void modify_contact_list_cb( DBusGProxy *proxy, GError *error, gpointer userdata )
	{
	
	int err = 0;
	//user data is of type message_hdr_req
	message_hdr_req *msg_hdr = ( message_hdr_req * )userdata;
	UNUSED_FORMAL_PARAM(proxy);	
	iso_logger( "%s", "In - modify_contact_list_cb\n" );
	//Check if there is any error
	if ( NULL != error ) 
		{
		//Failed to add contact:
		iso_logger ( "%s", "Failed to modify contact: \n" );	
			
		err = send_response_to_client( msg_hdr,  error->code, 0 );
		g_error_free(error);
		}
	else 
		{
		//Contact added successfully:
		iso_logger ( "%s", "Contact modify successfully:" );			
		err = send_response_to_client( msg_hdr, 0, 1 );
		}
	//free the msg hdr
	free( msg_hdr );

	if ( err < 0 )
		{
		// failed to delievered	
		return ;
		}
	iso_logger( "%s", "out -- modify_contact_list_cb" );			     	
	}


/*!
 *  /brief The contact names corresponding to the contact handles that are changed 
 *  are passed to the client
 *  /param proxy 
 *  /param handles_name
 *  /param error
 *  /param userdata
 *  /return void 
 */
static void rosters_changed_contacts_cb( DBusGProxy *proxy,
	gchar **handles_names, GError *error, gpointer userdata )
	{
	gint i = 0;
	gchar* member = NULL;
	gint mem_count = 0;
	
	gint* msg_type = ( gint* ) userdata;
	
	iso_logger( "%s", "In -- rosters_changed_contacts_cb" );
	UNUSED_FORMAL_PARAM(proxy);
	if ( !handles_names || error )
		{
		if ( error ) 
			{
			g_error_free(error);	
			}
		free ( msg_type );
		iso_logger( "%s", "handle names error" );
		return;
		}
	
	//read all the contacts
	//The handles_names will have NULL as it last elemnt in the array
	for ( i = 0; handles_names[i]; i++ ) 
		{
		member = handles_names[i];
		iso_logger( "This member : %s is trying to add you", 
			member );
		mem_count++;
		}
	//code to send the added contact to client
	if ( mem_count ) 
		{
		gint err = 0;
		err = send_contacts_to_client( handles_names, mem_count, *msg_type );	
		if ( err ) 
			{
			//How to handle the error?
			for ( i = 0; i < mem_count; i++ ) 
				{
				free ( handles_names[i] );
				}
			iso_logger( "%s", "There was error in send_contacts_to_client" );
			free ( msg_type );
			return;
			}	
		}
	
	free ( msg_type );
	
	iso_logger( "%s", "Out -- rosters_changed_contacts_cb" );
	
	}
	
/*! /brief Whenever contact list is changed by others( someone trying to add this user
 *  or someone rejecting the add request etc., ) this callback registered as a signal
 *  is called with the handles of the changed contacts. rosters_changed_contacts_cb is
 *  passed as callback to get the contact name of the changed handles
 * 
 * /param group_iface unused 
 * /param message unused
 * /param added List of contacts that are recently added
 * /param removed List of contacts that were recently removed
 * /param local_pending List of contacts that are locally pending for approval
 * /param remote_pending List of contacts that are remotely pending for approval
 * /param actor unused
 * /param reason unused
 * /param userdata unused
 * /return void
 */
void roster_members_changed_cb ( DBusGProxy           *group_iface,
									    gchar                *message,
									    GArray               *added,
									    GArray               *removed,
									    GArray               *local_pending,
									    GArray               *remote_pending,
									    guint                 actor,
									    guint                 reason,
									    gpointer  			  userdata )
	{
	
	gint* msg_type = ( gint* ) malloc ( sizeof( gint* ) );
	
	iso_logger( "%s", "In - roster_members_changed_cb\n" );

	//members in the contact list have changed 
	//Get the contact name of the handles thru rosters_changed_contacts_cb
	UNUSED_FORMAL_PARAM(message);
	UNUSED_FORMAL_PARAM(actor);
	UNUSED_FORMAL_PARAM(reason);
	UNUSED_FORMAL_PARAM(userdata);
	if ( group_iface == globalCon.group_iface_subscribe  && remote_pending 
		&& 0 < remote_pending->len ) 
		{
		*msg_type = ESubscribe_Remote_Pending;
		tp_conn_inspect_handles_async( DBUS_G_PROXY( globalCon.conn ),
						  TP_CONN_HANDLE_TYPE_CONTACT ,remote_pending,
						  rosters_changed_contacts_cb, msg_type );
		}
	if ( group_iface == globalCon.group_iface_subscribe  && removed 
		&& 0 < removed->len ) 
		{
		//User is trying to remove the contact from list
		//
		*msg_type = ESubscribe_Removed_Or_Rejected;
		tp_conn_inspect_handles_async( DBUS_G_PROXY( globalCon.conn ),
						  TP_CONN_HANDLE_TYPE_CONTACT ,removed,
						  rosters_changed_contacts_cb, msg_type );
		}
	if ( group_iface == globalCon.group_iface_subscribe  && added 
		&& 0 < added->len ) 
		{
		//
		*msg_type = ESubscribe_RP_Accepted;
		tp_conn_inspect_handles_async( DBUS_G_PROXY( globalCon.conn ),
						  TP_CONN_HANDLE_TYPE_CONTACT ,added,
						  rosters_changed_contacts_cb, msg_type );
		}
	if ( group_iface == globalCon.group_iface_publish  && added
		&& 0 < added->len ) 
		{
		*msg_type = EPublish_RP_Accepted;
		tp_conn_inspect_handles_async( DBUS_G_PROXY( globalCon.conn ),
						  TP_CONN_HANDLE_TYPE_CONTACT ,added,
						  rosters_changed_contacts_cb, msg_type );
		}
		
	if ( group_iface == globalCon.group_iface_publish  && local_pending 
		&& 0 < local_pending->len ) 
		{
		*msg_type = ETrying_To_Add_Contact_Publish;
		tp_conn_inspect_handles_async( DBUS_G_PROXY( globalCon.conn ),
						  TP_CONN_HANDLE_TYPE_CONTACT ,local_pending,
						  rosters_changed_contacts_cb, msg_type );
		}
	if ( group_iface == globalCon.group_iface_publish  && removed 
		&& 0 < removed->len ) 
		{
		*msg_type = EPublish_Removed_Or_Rejected;
		tp_conn_inspect_handles_async( DBUS_G_PROXY( globalCon.conn ),
						  TP_CONN_HANDLE_TYPE_CONTACT ,removed,
						  rosters_changed_contacts_cb, msg_type );
		}
	if ( group_iface == globalCon.group_iface_known && removed 
		&& 0 < removed->len ) 
		{
		//User is trying to remove the contact from list
		//
		*msg_type = ESubscribe_Removed_Or_Rejected;
		tp_conn_inspect_handles_async( DBUS_G_PROXY( globalCon.conn ),
						  TP_CONN_HANDLE_TYPE_CONTACT ,removed,
						  rosters_changed_contacts_cb, msg_type );
		}
	if ( group_iface == globalCon.group_iface_known && added 
		&& 0 < added->len ) 
		{
		//User is trying to remove the contact from list
		//
		*msg_type = ETrying_To_Add_Contact_Known;
		tp_conn_inspect_handles_async( DBUS_G_PROXY( globalCon.conn ),
						  TP_CONN_HANDLE_TYPE_CONTACT ,added,
						  rosters_changed_contacts_cb, msg_type );
		}
	       
	//log message 					       
	iso_logger( "%s", "out - roster_members_changed_cb\n" );
	}
	
