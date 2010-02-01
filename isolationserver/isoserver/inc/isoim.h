/*
* ============================================================================
*  Name        : isoim.h
*  Part of     : isolation server instant messaing componenet.
*  Version     : %version: 14 %
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

/*!
* /def To avoid multiple inclusion of header
*/

#ifndef _ISOIM_H_
#define _ISOIM_H_

#include <glib.h>
#include <gtypes.h>

#include "isoservermain.h"
#include "msg_enums.h"

/*! /file 
 *	Interfaces for the send / recieve of messages.
 *	client anywhere in the documentation refers to anyone
 *	using the isoserver
 */

/*!	/typedef struct send_msg_struct typedefed to send_msg_struct. 
 */
typedef struct send_msg_struct send_msg_struct;

/*! /struct send_msg_struct isoim.h 
 * 	/brief This struct is used to pass multiple pointers as userdata to callbacks
 *
 * 	/var hdr_req header request
 *	/var msg character string(message)	
 */
struct send_msg_struct
	{
	message_hdr_req* hdr_req;
	char *msg;
	};

/*!	/typedef struct _Received_UserData typedefed to Received_UserData. 
 */
typedef struct _Received_UserData Received_UserData;

/*! /struct Received_UserData isoim.h 
 * 	/brief This struct is used to pass multiple pointers as userdata to callbacks
 *
 * 	/var message_body message
 *	/var proxy proxy This is used for acknowledging the recieved message
 *	/var message_id This is used for acknowledging the recieved message
 */
struct _Received_UserData
	{
	gchar  *message_body;
	DBusGProxy *proxy;
	guint   message_id;
	};
	
/*!	/typedef struct send_error_struct typedefed to send_error_struct. 
 */
typedef struct send_error_struct send_error_struct;

/*! /struct send_error_struct isoim.h
 *	/brief This struct is used to pass multiple pointers as userdata to callbacks
 *	
 *	/var hdr_resp response header 
 *	/var msg_type
 *	/var timestamp
 *	/var msg_body
 */
struct send_error_struct
	{
	message_hdr_resp hdr_resp;
	guint msg_type;
	guint timestamp;
	gchar* msg_body;	
	};
	
/*!	/brief requests the handles for the contacts to which message should be sent.
 *	request_handles_cb is registered as a callback. necessary data to be sent is 
 *	passed as userdata 
 *
 * 	/param msghdr request header that will be passed back to client
 * 	/param contact_id all contacts
 * 	/param message
 * 	/param no_cntcts  no. of contacts the msg shld be sent to
 * 	/return : error code on failure, 0 on success
 */
void send_message( send_msg_struct* msg_hdr, const gchar **contact_id ) ;

/*!	/brief handles for contacts are recieved and text channel created (if already
 *	there same is used) message is sent to those contacts.
 *	
 * 	
 * 	/param proxy unused
 * 	/param handles contact handles
 * 	/param error error if any in getting the handles for contact 
 * 	/param userdata Has request header and message to be sent to reciever
 *	The request header is used for mapping of response to correct request
 *
 */
void request_handles_cb( DBusGProxy *proxy, GArray *handles, GError* error, 
			gpointer userdata ) ;


/*!	/brief parse the params for send and validates them
 * 
 * 	/param aMsgBuffer message buffer to be parsed
 * 	/param aContactId pointer to an array of strings, After this function this 
 *	arg wil have the contacts to which message should be sent to
 * 	/param aSendMsg message to be sent
 * 	/param len : msg_len no. of bytes in aMsgBuffer
 * 	/param contact_count : no. of contacts
 * 	/return returns error code on failure, or 0
 */
int parse_for_send( gchar* aMsgBuffer, gchar*** aContactId, 
	gchar** aSendMsg, gint msg_len, gint* contact_count ) ;
	

/*!	/brief calls parse_for_send to parse the parameters, and calls 
 *	send_message for sending the message
 * 
 * 	/param buf : message buffer to be parsed
 * 	/param len : msg_len no. of bytes in msg_buf
 *
 * 	/return gint : parse error code if any
 */
int action_parse_send( gchar* buf, gint msg_len ) ;

/*!	/brief Function to check if the channel is already present in the maintained hash
 * 	text_channels_find_func. 
 *
 * 	/param key unused
 * 	/param text_channel hash table element
 * 	/param contact_handle to be searched item 
 * 	/return boolean 
 */
gboolean text_channels_find_func(	gchar *key,
									TpChan *text_channel,
									guint *contact_handle );
									
/*! /brief Once the send request is sent to n/w server this callback is called
 * 
 * 	/param proxy unused
 * 	/param error n/w error if any
 * 	/param userdata message header from which a response is formed
 * 	/return void
 */
void sendreply_cb( DBusGProxy *proxy, GError *error, gpointer userdata ) ;

/*!	/brief This function is called by tg to as a response to request for the 
 *	handles from the text channel. If there is no error, sent message( 
 *	which has failed ) with its deatils	is then sent to client
 * 
 *	/param proxy unused
 *  /param handles_names contac names (sender name)
 *  /param error error if any
 *  /param userdata send_error_struct
 *  /return void
 */								
static void inspect_handles_for_error_cb( DBusGProxy *proxy,char **handles_names,
				 GError *error, gpointer userdata );

/*!	/brief This function on registered for "senderror" signal is called when the message 
 * 	sending is failed. Calls tp_conn_inspect_handles_async to get the contact name 
 *	from text channel
 *
 * 	/param proxy : unused
 * 	/param error : error code
 * 	/param timestamp : sent to inspect_handles_for_error_cb thru' send_error_struct
 * 	/param message_type : sent to inspect_handles_for_error_cb thru' send_error_struct
 * 	/param message_body : sent to inspect_handles_for_error_cb thru' send_error_struct
 * 	/param user_data text_chan from where to get the contatc name of reciepien
 * 	/return void
 */
static void senderror_cb (  DBusGProxy *proxy,
								guint            error,
							   	guint            timestamp,
							    guint            message_type,
							    gchar           *message_body,
							    gpointer		user_data );
			
/*!	/brief called as a callback to acknowledge the msg ack. 
 * 
 * 	/param proxy unused 
 * 	/param error error if any
 * 	/param userdata unused
 * 	/return void
 */
void msg_ack_pending_cb( DBusGProxy *proxy, GError *error, gpointer userdata ) ;

/*!	/brief This function is a callback for to get the contact name from handles.
 *	This function acknowledges the recieved message and send that to client(adap)
 * 	
 * 	/param proxy unused
 * 	/param handles_names 2D array of message sender(one contact ended with a NULL string)
 * 	/param error error if any
 *  /param userdata Received_UserData that has message body, response header etc.,
 *  /return void
 */
static void getting_sender_cb( DBusGProxy *proxy,char **handles_names, 
		GError *error, gpointer userdata ) ;

/*!	/brief when a message is recieved this function is called by 
 * 	telepathygabble(by emitting signal). This function then requests tg 
 *	to get the contact names corresponding to handles.
 * 
 * 	/param proxy sent to getting_sender_cb as param in userdata
 * 	/param timestamp unused
 * 	/param handles_names
 * 	/param message_type unused
 * 	/param message_flags unused
 * 	/param message_body recieved message
 * 	/param userdata unused
 */
void receivedMessage_cb (  DBusGProxy *proxy,
								guint            message_id,
							   	guint            timestamp,
							    guint            from_handle,
							    guint            message_type,
							    guint            message_flags,
							    gchar           *message_body,
							    gpointer		user_data );
/*!	/brief whenevr a new channel handler for text is created, this function is called. This 
 *	function also registers that channel for Received and SendError signals. Also the pending
 *	messages are also retrieved in this function
 * 
 * 	/param tp_chan new channel created
 */
void text_channel_init  ( TpChan   *text_chan  );

/*!	/brief called by getting_sender_cb to send the recieved message to client
 * 
 * 	/param msg message recieved
 * 	/param sender message from whom recieved
 */
int  send_message_to_client( const gchar *msg , const gchar *sender);	
							
#endif