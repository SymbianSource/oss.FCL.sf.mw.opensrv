/*
* ============================================================================
*  Name        : isoim.c
*  Part of     : isolation server instant messaing componenet.
*  Version     : %version: bh1cfmsg#28 %
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

#include "tp-conn-gen.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "isoim.h"
#include "isoutils.h"

#include "msgliterals.h"
#include "msg_enums.h"

/*!	\file 
*	Impliments the functions in isoim.h
*/

/*! /brief requests the handles for the contacts to which message should be sent.
 *  request_handles_cb is registered as a callback. necessary data to be sent is 
 *  passed as userdata 
 *
 *  /param msghdr request header that will be passed back to client
 *  /param contact_id all contacts
 *  /param message
 *  /param no_cntcts  no. of contacts the msg shld be sent to
 *  /return : error code on failure, 0 on success
 */
void send_message( send_msg_struct* msg_hdr,
					const gchar **contact_id ) 
	{
	 
	//request for the handles to contact
	tp_conn_request_handles_async( DBUS_G_PROXY( globalCon.conn ),
						TP_CONN_HANDLE_TYPE_CONTACT,
						contact_id,request_handles_cb, ( gpointer ) msg_hdr );	 							
	}


/*! /brief handles for contacts are recieved and text channel created (if already
 *  there same is used) message is sent to those contacts.
 *  
 *  
 *  /param proxy unused
 *  /param handles contact handles
 *  /param error error if any in getting the handles for contact 
 *  /param userdata Has request header and message to be sent to reciever
 *  The request header is used for mapping of response to correct request
 *
 */
void request_handles_cb(DBusGProxy *proxy, GArray *handles, GError* error, gpointer message)
	{
	TpChan* text_chan = NULL; //for send msg
	guint contact_handle;
	DBusGProxy  *text_iface = NULL;
	send_msg_struct* msg_hdr = ( send_msg_struct* ) message;

	UNUSED_FORMAL_PARAM(proxy);
	//There was error in requesting handles to the contacts
	if ( error )
		{
		iso_logger( "%s", error->message);
		 
		iso_logger( "err code: %d ", error->code );
		//Send error to client
		//What happens if 
		send_response_to_client( msg_hdr->hdr_req,  error->code, 0 );
		g_error_free(error);
		return; 	
		}
	 
	contact_handle = g_array_index( handles, guint, 0 );
	//get the text channel for the contact handle 
	text_chan = g_hash_table_find( globalCon.text_channels,
					      (GHRFunc) text_channels_find_func,
					      &contact_handle);	 

	iso_logger( "text_chan : %d ", text_chan );
	
	//create a text channel						      
	if ( text_chan == NULL )	
		{
			 
		text_chan = tp_conn_new_channel( globalCon.dbusConn,globalCon.conn,globalCon.connmgr_bus,TP_IFACE_CHANNEL_TYPE_TEXT,
									TP_CONN_HANDLE_TYPE_CONTACT,contact_handle, TRUE );	
									
		if(!text_chan)
			{
			send_response_to_client( msg_hdr->hdr_req, NOT_CONNECTED , 0 );
			g_error_free(error);
			return;	
			}
			
		iso_logger( "%s", dbus_g_proxy_get_path(DBUS_G_PROXY(text_chan)));							
		g_hash_table_insert( globalCon.text_channels,g_strdup(dbus_g_proxy_get_path(DBUS_G_PROXY(text_chan))),text_chan);							
		}
	 	
	//get interface..	
	text_iface = tp_chan_get_interface( text_chan,TELEPATHY_CHAN_IFACE_TEXT_QUARK );	
	

	
	if ( NULL == text_iface ) 
		{
		free( msg_hdr->msg );
		free( msg_hdr->hdr_req );	
		free( msg_hdr );

		//Free proxy ?
		return ;
		}

	//send async
	tp_chan_type_text_send_async( text_iface, TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL, 
		( const char * )msg_hdr->msg,sendreply_cb, msg_hdr->hdr_req ); // see if any user date should be passed
	
	
	iso_logger( "Message being sent is %s", msg_hdr->msg ); 	 
	
	free( msg_hdr->msg );
	free( msg_hdr );
	 	
	iso_logger( "%s", "tp_chan_type_text_send_async after\n"); 	
	}

/*! /brief parse the params for send and validates them
 * 
 *  /param aMsgBuffer message buffer to be parsed
 *  /param aContactId pointer to an array of strings, After this function this 
 *  arg wil have the contacts to which message should be sent to
 *  /param aSendMsg message to be sent
 *  /param len : msg_len no. of bytes in aMsgBuffer
 *  /param contact_count : no. of contacts
 *  /return returns error code on failure, or 0
 */
gint parse_for_send( gchar* msg_buf, gchar*** contact_ids, 
			gchar** send_msg, gint msg_len, gint* contact_count ) 
	{
	gchar* users = NULL;
	
	gint len = 0;
	gint err = 0;
	
	iso_logger( "%s", "In - parse_for_send\n" );
	//skip the msg_hdr part
	len += sizeof( message_hdr_req );
	//example send message - message header omitted..
	//test.ximp@gmail.com\0ximp.telepathy@gmail.com\0\0hi, how r u?\0
	//					  ^ 						^ ^	            ^ 
	//contact1------------|contact2-----------------|-|message------|
	
	//gets the contacts, returns no. of contacts on success 
	//error code on error
	err = parse_into_array_of_strings( msg_buf, contact_ids, &len, msg_len,
	 contact_count );
	if ( err < 0 ) 
		{
		//An error has occured, so propagate to next level
		return err;
		}
	
		
	/*
	Will never come here.. coz it has been already handled in 
	parse_into_array_of_strings
	if ( *contact_count > MAX_MSG_RECEIPIENTS ) 
		{
		return INVALID_PARAMETERES;
		}*/
	//reset the userlen	
	
	//parse for the message..
	err = parse_a_string( msg_buf, &users, &len, msg_len );
	
	if ( err < 0 ) 
		{
		return err; 
		}
	//set the pointer to send message
	*send_msg = users;
	
	iso_logger( "%s", "Out - parse_for_send\n" );	
	//return the no. of contacts to be sent to	
	return 0;
	}
/*! /brief calls parse_for_send to parse the parameters, and calls 
 *  send_message for sending the message
 * 
 *  /param buf : message buffer to be parsed
 *  /param len : msg_len no. of bytes in msg_buf
 *
 *  /return gint : parse error code if any
 */
int action_parse_send( gchar* buf, gint msg_len ) 
	{
	gchar** contactid = NULL;
	gchar* sendmsg = NULL;
	message_hdr_req *msg_hdr = NULL;
	gint meesage_type_err = 0;
	//It is very imp to initailize the contact_count to 0 here
	gint contact_count = 0;
	send_msg_struct* msg_struct = NULL;
	//parse contacts, message.. 	
	iso_logger( "%s", "In - action_parse_send\n" );
	meesage_type_err = parse_for_send( buf, 
			&contactid,&sendmsg, msg_len, &contact_count );
	
	//if <= 0 there is some error in the message formation
	if ( contact_count > 0  && meesage_type_err == 0 ) 
		{
		
		//send message
		msg_hdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
		if ( NULL == msg_hdr ) 
			{
			//free resources allocated for this operation
			free_msg_args( contactid, contact_count, sendmsg );
			return MEM_ALLOCATION_ERROR;
			}
	    memset( msg_hdr, '\0', sizeof( message_hdr_req ) );
	    //read message header from buffer
	    memcpy( msg_hdr, buf, sizeof( message_hdr_req ) );
	    
	    //fill the contacts and message into a struct
	    msg_struct = ( send_msg_struct* ) malloc ( sizeof ( send_msg_struct ) );
	    if ( NULL == msg_struct ) 
			{
			free ( msg_hdr );
			free_msg_args( contactid, contact_count, sendmsg );
			return MEM_ALLOCATION_ERROR;
			}
		msg_struct->hdr_req = msg_hdr;
		msg_struct->msg = sendmsg;
		//msg_struct is sent callback... which should be feed there..
		send_message( msg_struct, 
				( const gchar** ) contactid );
		}
	else  
		{
		//there was some error, free resources allocated for this operation
		free_msg_args( contactid, contact_count, sendmsg );
		}
	
	iso_logger( "%s", "Out - action_parse_send\n" );
	//return error on failure, or no. of contacts message sent to on success
	return meesage_type_err;
	
	}

/*! /brief Function to check if the channel is already present in the maintained hash
 *  text_channels_find_func. 
 *
 *  /param key unused
 *  /param text_channel hash table element
 *  /param contact_handle to be searched item 
 *  /return boolean 
 */
gboolean text_channels_find_func(	gchar *key,
									TpChan *text_channel,
									guint *contact_handle )
	{
	iso_logger( "%s", "In - text_channels_find_func\n" );
	//Check for the handles
	UNUSED_FORMAL_PARAM(key);
	if ( ( text_channel->handle == *contact_handle ) )
		return TRUE;

	return FALSE;
	} 

/*! /brief Once the send request is sent to n/w server this callback is called
 * 
 *  /param proxy unused
 *  /param error n/w error if any
 *  /param userdata message header from which a response is formed
 *  /return void
 */
void sendreply_cb( DBusGProxy *proxy, GError *error, gpointer userdata ) 
	{
	int err = 0;

	message_hdr_req* msg_hdr = ( message_hdr_req* ) userdata;
	// create the msg queue
	//user data is of type message_hdr_req
	iso_logger( "%s", "In - sendreply_cb\n" );
    UNUSED_FORMAL_PARAM(proxy);
	if ( NULL != error ) 
		{
		//There was some error
		//send the response for the msg_hdr request to client
		err = send_response_to_client( msg_hdr,  error->code, 0 );
		//free the message
		g_error_free(error);
		
		}
	else 
		{
		//send the response for the msg_hdr request to client
		err = send_response_to_client( msg_hdr, 0, 1 );
		}
	//Free the header recved as callback userdata
	free( msg_hdr );

	if ( err < 0 )
		{
		// failed to delievered	
		return ;
		}
	iso_logger( "%s", "Out - sendreply_cb\n" );
	}
	
/*! /brief This function is called by tg to as a response to request for the 
 *  handles from the text channel. If there is no error, sent message( 
 *  which has failed ) with its deatils is then sent to client
 * 
 *  /param proxy unused
 *  /param handles_names contac names (sender name)
 *  /param error error if any
 *  /param userdata send_error_struct
 *  /return void
 */     
static void inspect_handles_for_error_cb( DBusGProxy *proxy,char **handles_names,
				 GError *error, gpointer userdata ) 
	{
	
	gint pri = MSG_PRI_NORMAL;
	gint result = 0;
	gint timeout = NO_WAIT;
	gint err = 0;
	
	send_error_struct* msg_struct = ( send_error_struct* ) userdata;
	gchar* rmsg = NULL;
	gint index = 0;
	gint len = 0;
	UNUSED_FORMAL_PARAM(proxy);
	iso_logger( "%s", "In - inspect_handles_for_error_cb" );		
	if ( !handles_names || error || NULL == handles_names[0] ) 
		{
		if ( error ) 
			{
			g_error_free( error );	
			}
		//Free the userdata passed to the callback 
		if ( NULL != msg_struct ) 
			{
			if ( msg_struct->msg_body ) 
				{
				g_free( msg_struct->msg_body );		
				}
			g_free( msg_struct );			
			}
		return;
		}
	//Allocate the memory and check for NULL	
	rmsg = ( gchar* ) malloc ( MAX_MSG_SIZE );
	if ( NULL == rmsg ) 
		{
		goto mem_clean;
		}
	//set memory to 0
	memset( rmsg, '\0', MAX_MSG_SIZE );
    //copy the header
    //the response header, message type(from libtp), timestamp
	memcpy( rmsg + index, msg_struct, sizeof( message_hdr_resp )
	+ 2 * sizeof( guint )  );
	
    index += sizeof( message_hdr_resp ) + 2 * sizeof( guint );
    
    //Copy the name to whom message was sent
    len = strlen( handles_names[0] );
    strcpy( rmsg + index, handles_names[0] );
    index += len + 1;
    
    //Copy the message body
    if ( msg_struct->msg_body ) 
	    {
	    len = strlen( msg_struct->msg_body );
	    strcpy( rmsg + index, msg_struct->msg_body );
	    index += len + 1;	   	
	    }
    rmsg[index++] = '\0';
    //Header format
    //msgHdr|msg_type|timestamp|reciepient_name\0sent_msg\0
	// 		^                                   ^         ^
    //      |			                        |		  |
    //no dilimeter here                        nul		 nul
    //send to the client
    result = MsgQSend( RESPONSE_QUEUE, rmsg, index, 
						pri, timeout, &err );
	iso_logger( "%s", "After MsgQSend" );						
//If memory allocation for rmsg has failed control comes
//here
mem_clean:

	for ( len = 0; handles_names[len]; len++ )	
		{
		free ( handles_names[len] );
		}
		
	if ( msg_struct->msg_body ) 
		{
		g_free( msg_struct->msg_body );		
		}
	g_free( msg_struct );
						
	if ( result < 0 )
		{
		//Failed to deliver the message
		iso_logger( "%s", "Failed to deliver the message to client" );
		return;
		}
	iso_logger( "%s", "Out - inspect_handles_for_error_cb" );
	
	}

/*! /brief This function on registered for "senderror" signal is called when the message 
 *  sending is failed. Calls tp_conn_inspect_handles_async to get the contact name 
 *  from text channel
 *
 *  /param proxy : unused
 *  /param error : error code
 *  /param timestamp : sent to inspect_handles_for_error_cb thru' send_error_struct
 *  /param message_type : sent to inspect_handles_for_error_cb thru' send_error_struct
 *  /param message_body : sent to inspect_handles_for_error_cb thru' send_error_struct
 *  /param user_data text_chan from where to get the contatc name of reciepien
 *  /return void
 */
static void senderror_cb (  DBusGProxy *proxy,
								guint            error,
							   	guint            timestamp,
							    guint            message_type,
							    gchar           *message_body,
							    gpointer		user_data )
	{
	
	TpChan *text_chan = ( TpChan * ) user_data;
	send_error_struct* msg_struct = NULL;
	GArray* fetch_members = NULL;
	DBusGProxyCall* call_id = NULL;
	
	//Logs
	iso_logger( "%s", "In - SendError_cb\n" );
	iso_logger( "error is %d", error );
	iso_logger( "error is %d", message_type );
	iso_logger( "error is %d", timestamp );
	iso_logger( "error is %s", message_body );
	UNUSED_FORMAL_PARAM(proxy);
	//Allocate memory and check for NULL
	//This msg_struct is passed to callback..
	//So not deleting here.. ownership is transfered
	msg_struct = ( send_error_struct* ) malloc ( sizeof( send_error_struct ) );

	if ( NULL == text_chan || NULL == msg_struct ) 
		{
		return;
		}
	//Set the values to NULL		
	memset( msg_struct, '\0', sizeof( send_error_struct ) );
	//Message type is send error
	msg_struct->hdr_resp.hdr_req.message_type = ESend_Error;
	//other values are set to 0
	msg_struct->hdr_resp.hdr_req.protocol_id = 0;
	msg_struct->hdr_resp.hdr_req.session_id = 0;
	msg_struct->hdr_resp.hdr_req.request_id = 0;
	
	//Message is sent as single entity..
	//Message is not long enough to be broken into 
	msg_struct->hdr_resp.continue_flag = 0;
	msg_struct->hdr_resp.response = 0;
	
	//Set the error type
	msg_struct->hdr_resp.error_type = error;
	//Set the values from libtelepathy
	msg_struct->msg_type = message_type;
	msg_struct->timestamp = timestamp;
	//Not checking for NULL here.. 
	//I feel it is ok to send without the message 
	//Taken care of this in inspect_handles_for_error_cb
	msg_struct->msg_body = strdup( message_body );
	
	//Create an array of handles to get the handle name
	fetch_members = g_array_new ( FALSE, FALSE, sizeof ( guint32 ) );
	g_array_append_val( fetch_members, text_chan->handle );
	//Call libtp function to get the contact name for the handle
	//msg_struct is passed to inspect handles call back to be passed to 
	//client as error
	call_id = tp_conn_inspect_handles_async( DBUS_G_PROXY( globalCon.conn ),
					       TP_CONN_HANDLE_TYPE_CONTACT ,fetch_members,
					       inspect_handles_for_error_cb, msg_struct );
	
	g_array_free( fetch_members, TRUE );
	if ( NULL == call_id )
		{
		return ;
		}
	
	iso_logger( "%s", "Out - SendError_cb" );
	
	}




/*! /brief called as a callback to acknowledge the msg ack. 
 * 
 *  /param proxy unused 
 *  /param error error if any
 *  /param userdata unused
 *  /return void
 */
static void msg_ack_pending_cb( DBusGProxy *proxy, GError *error, gpointer userdata ) 

	{	
	gchar *str = "Inside msg_ack_pending_cb";
	UNUSED_FORMAL_PARAM(proxy);
	UNUSED_FORMAL_PARAM(userdata);
	if ( error ) 
		{
		//If there was error, ignore it and free the memory
		g_error_free(error);	
		}
	
	iso_logger(  str );
	}


	
/*! /brief This function is a callback for to get the contact name from handles.
 *  This function acknowledges the recieved message and send that to client(adap)
 *  
 *  /param proxy unused
 *  /param handles_names 2D array of message sender(one contact ended with a NULL string)
 *  /param error error if any
 *  /param userdata Received_UserData that has message body, response header etc.,
 *  /return void
 */  
static void getting_sender_cb( DBusGProxy *proxy,char **handles_names, GError *error, gpointer userdata ) 
	{

	gchar *str = "Inside getting_sender_cb";
	GArray  *message_ids;
	gint result = 0;
	Received_UserData *user_data = ( Received_UserData* )userdata;
	iso_logger(  "%s", str );
	UNUSED_FORMAL_PARAM(proxy);
	if ( !handles_names || error )
		{
		if ( error ) 
			{
			//If there was error, ignore it and free the memory
			g_error_free(error);	
			}
		//Free the userdata passed to the callback 
		g_free( user_data->message_body );
		g_free( user_data );
		iso_logger( "%s", "handle names error" );
		return;
		}
	
	message_ids = g_array_new ( FALSE, FALSE, sizeof ( guint ) );
	if ( NULL == message_ids ) 
		{
		//Free the userdata passed to the callback 
		g_free( user_data->message_body );
		g_free( user_data );
		return ;	
		}
	g_array_append_val ( message_ids, user_data->message_id );
	//Acknowledge that message has been recieved
	tp_chan_type_text_acknowledge_pending_messages_async( user_data->proxy, message_ids, 
			msg_ack_pending_cb, NULL );
	//Send to the client the messgae and the sender name			
	result = send_message_to_client( user_data->message_body , *handles_names );
	
	if ( 0 > result ) 
		{
		return;
		}
		
	iso_logger(  "%s", "getting_sender_cb" );
	iso_logger(  "%s", user_data->message_body );
	
	//Free the messgae ids array		
	g_array_free ( message_ids, TRUE );
	//free the recieved data
	g_free( user_data->message_body );
	g_free( user_data );
	
	
	}

    
    

/*! /brief when a message is recieved this function is called by 
 *  telepathygabble(by emitting signal). This function then requests tg 
 *  to get the contact names corresponding to handles.
 * 
 *  /param proxy sent to getting_sender_cb as param in userdata
 *  /param timestamp unused
 *  /param handles_names
 *  /param message_type unused
 *  /param message_flags unused
 *  /param message_body recieved message
 *  /param userdata unused
 */
void receivedMessage_cb (  DBusGProxy *proxy,
								guint            message_id,
							   	guint            timestamp,
							    guint            from_handle,
							    guint            message_type,
							    guint            message_flags,
							    gchar           *message_body,
							    gpointer		user_data )
    {
	GArray  *handles = NULL;
	Received_UserData *recv_userdata = NULL;
	
	iso_logger(  "%s", "receivedMessage_cb:" );
	UNUSED_FORMAL_PARAM(timestamp);
	UNUSED_FORMAL_PARAM(message_type);
	UNUSED_FORMAL_PARAM(message_flags);
	UNUSED_FORMAL_PARAM(user_data);
	//Allocate memory and initalize the structure
	recv_userdata =  g_new ( Received_UserData, 1 );
	if ( NULL == recv_userdata ) 
		{
		
		free ( message_body );
		return ;
		}
	//TBD : Check if this assignment works..
	//previously it was strdup of message_body 
	recv_userdata->message_body =  strdup( message_body );
	recv_userdata->message_id = message_id;
	recv_userdata->proxy = proxy;
	
	//logs
	iso_logger(  "%s", message_body );
	//allocate memory for the handles
	handles = g_array_new ( FALSE, FALSE, sizeof ( guint32 ) );
	if ( NULL == handles ) 
		{
		//free previously allocated struct
		g_free( recv_userdata->message_body );
		g_free( recv_userdata );
		return ;
		}
		
	g_array_append_val ( handles, from_handle );
	//get the sender name corresponding to the handles
	tp_conn_inspect_handles_async( DBUS_G_PROXY( globalCon.conn ),
					       TP_CONN_HANDLE_TYPE_CONTACT ,handles,
					       getting_sender_cb,recv_userdata );
	//free handles				       	
	g_array_free ( handles, TRUE );
		
    }


/*! /brief whenevr a new channel handler for text is created, this function is called. This 
 *  function also registers that channel for Received and SendError signals. Also the pending
 *  messages are also retrieved in this function
 * 
 *  /param tp_chan new channel created
 */
 void text_channel_init( TpChan *text_chan )
	{
	
	GPtrArray  *messages_list ;
	guint       i = 0;
	GArray     *message_ids;
	DBusGProxy *text_iface;
	GArray *handles = NULL;
	Received_UserData *recv_userdata = NULL;
	
	
	iso_logger(  "%s", "inside  text_channel_init" );
	//text_chan has been checked for NULL in the callee function
    text_iface = tp_chan_get_interface( text_chan ,
	   								   TELEPATHY_CHAN_IFACE_TEXT_QUARK );
	
	message_ids = g_array_new ( FALSE, TRUE, sizeof ( guint ) );
	
	handles = g_array_new ( FALSE, FALSE, sizeof ( guint32 ) );
	//If memory allocation fialure, return from here
	if ( !text_iface || !message_ids || !handles ) 
		{
		return ;	
		}
	
	tp_chan_type_text_list_pending_messages( text_iface,
						FALSE,
						&messages_list,
						NULL );
	//For all the pending messages get the sender name
	//and send to client the sender name and recv msg
	for ( i = 0; i < messages_list->len ; i++ )
		{
		guint          message_id;
		guint          from_handle;
		const gchar   *message_body;
		GValueArray   *message_struct;
		
		message_struct = ( GValueArray * ) g_ptr_array_index ( messages_list, i );
		
		//Not all the fields used currently
		//Ignoring the unused fields
		message_id = g_value_get_uint( g_value_array_get_nth( message_struct, 0 ) );
		//get the handle to sender
		from_handle = g_value_get_uint(g_value_array_get_nth( message_struct, 2 ) );
		//Get the message body
		message_body = g_value_get_string( g_value_array_get_nth( message_struct, 5 ) );
		
		iso_logger ( "%s", message_body );
			    
    	g_array_append_val( handles, from_handle );
    	//Allocate memory for the recv data
       	recv_userdata =  g_new( Received_UserData, 1 );
       	if ( !recv_userdata ) 
	       	{
	       	break; //Break from the loop
	       	}
	    //Set all the necessary fields
	    recv_userdata->message_body = g_strdup( message_body );
	    recv_userdata->message_id = message_id;
 	    recv_userdata->proxy = text_iface;
 	    //get the name for the sender handle
	    tp_conn_inspect_handles_async( DBUS_G_PROXY( globalCon.conn ),
					       TP_CONN_HANDLE_TYPE_CONTACT ,handles,
					       getting_sender_cb, recv_userdata );	
					       
    	g_array_append_val ( message_ids, message_id );
    	
    	//To reuse the array, remove the 1 that was inserted
    	g_array_remove_range( handles, 0, handles->len );

		}
	iso_logger ( "%s", "before ack pending msg");
	//acknowledge the recved messages
	tp_chan_type_text_acknowledge_pending_messages( text_iface, message_ids,
					       NULL );
	
	iso_logger ( "%s", "after ack pending msg" );
	
	//Listen to Received signal
	dbus_g_proxy_connect_signal( text_iface, "Received",
				G_CALLBACK( receivedMessage_cb ),
					NULL, NULL );
					
	//Register for the senderror signal
	dbus_g_proxy_connect_signal( text_iface, "SendError",
				G_CALLBACK( senderror_cb ),
					text_chan, NULL );					
					
	//Should the messages_list be freed?	
	g_array_free( message_ids, TRUE );
	g_array_free( handles, TRUE );
	
	
	}


 

 /*! /brief called by getting_sender_cb to send the recieved message to client
  * 
  *  /param msg message recieved
  *  /param sender message from whom recieved
  */ 
gint send_message_to_client( const gchar *msg , const gchar *sender )

    {
	gint len=0;
	gint pri = MSG_PRI_NORMAL;
	gchar *rmsg = NULL;
	gint index = 0;
	gint err;
	message_hdr_resp* msg_resp = NULL;
	gint result;
	gint timeout = NO_WAIT;
	
	msg_resp = ( message_hdr_resp* ) malloc ( sizeof( message_hdr_resp ) );
	if ( NULL == msg_resp ) 
		{
		return 	MEM_ALLOCATION_ERROR;
		}
	//Allocate memory and initialze to 0 
	rmsg = ( gchar* ) malloc ( MAX_MSG_SIZE );
	if ( NULL == rmsg ) 
		{
		free ( msg_resp );
		return 	MEM_ALLOCATION_ERROR;
		}
	   
	//Initialze the memory to 0
	memset( msg_resp, '\0', sizeof( message_hdr_resp ) );
    //Set the message type to EText_Message_Receive and set other fields
    //to default values
	msg_resp->hdr_req.message_type = EText_Message_Receive;
	msg_resp->hdr_req.protocol_id = 1;
	msg_resp->hdr_req.session_id = 1;
	msg_resp->hdr_req.request_id =1;
	//Set the respone as success and error is 0
	msg_resp->response = 1;
	msg_resp->error_type = 0;
		
    memset( rmsg, '\0', MAX_MSG_SIZE );
    //copy the header
	memcpy( rmsg, msg_resp, sizeof( message_hdr_resp ) );
    index += sizeof( message_hdr_resp );
    
    //copy the sender
    //putting sender in message queue
    len = strlen(sender);
    strcpy( rmsg + index, sender );
    index += len + 1;
    
    //putting message body   
    len = strlen(msg);
    strcpy( rmsg + index, msg );
    index += len + 1;
    
    //Header format
    //msgHdr|sender_name\0recved_msg\0
	// 		^            ^           ^
    //      |			 |			 |
    //no dilimeter here  nul		nul
    //send to the client
    result = MsgQSend( RESPONSE_QUEUE, rmsg, index, 
						pri, timeout, &err );
	free ( rmsg );
	
	free ( msg_resp );
	
    if ( result < 0 )
    	{
    	return MSG_Q_SEND_FAILED;
    	}
    	
	iso_logger(  "%s", "message is:\n" );
	return TRUE;
    }

// end of files


