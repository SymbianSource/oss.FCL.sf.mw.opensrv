/*
* ============================================================================
*  Name        : isogetsearchkeys.c
*  Part of     : isolation server.
*  Version     : %version: 7 %
*
*  Copyright © 2007-2008 Nokia.  All rights reserved.
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

/*!	\file 
*	Impliments the functions in isosearch.h
*/

/*! \def For malloc
*/
#include <stdlib.h>

/*! \def For string operations
*/
#include <string.h>


/*! \def For search related declarations
*/
#include "isosearch.h"

/*! \def For globalCon
*/
#include "isoservermain.h"

/*! \def For tp_chan_type_search_async
*/
#include "tp-chan-type-search-gen.h"

/*! \def For MSG_PRI_NORMAL
*/
#include "msgqlib.h"

/*! \def For message_hdr_req
*/
#include "isoutils.h"

/*! \def For RESPONSE_QUEUE
*/
#include "msgliterals.h"

#include "isogetsearchkeys.h"

/*!	\struct key_value_struct isoserach.h
*	\brief This struct is used for to form the msg buffer that will be sent to client
*	\var msg As the key/value pairs are read those will be appended to msg
* 	\var len is the no. of bytes appended to msg
*/
struct key_value_struct 
	{
	gchar* msg;
	gint len;		
	};
	
/*! \typedef struct key_value_struct to key_value_struct
*/	
typedef struct key_value_struct key_value_struct;

/*! \struct get_search_keys_cb_struct isosearch.h
*	\brief This struct bundles multiple(two ?) pointers to be passed
*	to getsearch keys callback
*	\var search_chan which should be closed when search keys are recieved
*	\var hdr_req message header request
*/	
struct get_search_keys_cb_struct 
	{
	TpChan* search_chan;
	message_hdr_req* hdr_req;	
	};
	
/*! \typedef struct get_search_keys_cb_struct to get_search_keys_cb_struct 
*/		
typedef struct get_search_keys_cb_struct get_search_keys_cb_struct;	


static void getsearchkeys_cb( DBusGProxy *proxy,  gchar *instr,
  gchar **keys, GError *error, gpointer userdata );
  
/*!	\brief get_search_keys_reply is called as a callback to get_search_keys
*	This function creates a new channel and requests telepathy for the 	
*	search keys
*
*	\param userdata message header request
*
*	\remark calling tp_conn_new_channel results into esock_client 8 panic
*	Instead of calling function tp_conn_new_channel which makes a 
*	a call to tp_conn_request_channel, tp_conn_request_channel_async is 
*	called directly. 
*/	

static void get_search_keys_reply( DBusGProxy *proxy, char *chan_object_path, 
 			GError *error, gpointer user_data )
	{
		                               
	
	TpChan *search_chan = NULL;
	DBusGProxy *search_iface = NULL;
	get_search_keys_cb_struct* chan_struct = NULL;
	message_hdr_resp* hdr_req = ( message_hdr_resp* )user_data;
	
	UNUSED_FORMAL_PARAM( proxy );
	iso_logger( "in get_search_keys" );
	
	if ( error )
		{
		send_response_to_client( (message_hdr_req*)hdr_req, error->code, 0 );	
		free ( hdr_req );
		return;			
		}
		
	//Create new channel for the search
	search_chan = tp_chan_new( globalCon.dbusConn, globalCon.connmgr_bus, chan_object_path, 
	TP_IFACE_CHANNEL_TYPE_CONTACT_SEARCH, TP_CONN_HANDLE_TYPE_NONE, 0 );

	
	if ( NULL == search_chan ) 
		{
		free ( hdr_req );
		return ;			
		}							
	//Get chan interface					
	search_iface = tp_chan_get_interface( search_chan, 
		TELEPATHY_CHAN_IFACE_CONTACTSEARCH_QUARK );	
					
	if ( NULL == search_iface ) 
		{
		tp_chan_close_async( DBUS_G_PROXY( search_chan ), 
					search_chan_closed_cb, NULL );
		free ( hdr_req );			
		return ;			
		}
	
	chan_struct = ( get_search_keys_cb_struct* ) malloc ( 
			sizeof ( get_search_keys_cb_struct ) );
			
	chan_struct->search_chan = search_chan;
	chan_struct->hdr_req = ( message_hdr_req* ) malloc ( 
							sizeof( message_hdr_req ) );
	if ( NULL == chan_struct->hdr_req ) 
		{
		tp_chan_close_async( DBUS_G_PROXY( search_chan ), 
					search_chan_closed_cb, NULL );
		free ( hdr_req );			
		return ;			
		}							
	//memset( chan_struct->hdr_req, )
	memcpy( chan_struct->hdr_req, hdr_req, sizeof( message_hdr_req ) );
		
	
	//Ownership of search_chan and chan_struct->hdr_req, 
	//chan_struct is passed to getsearchkeys_cb
	free ( hdr_req );
	
	tp_chan_type_search_get_search_keys_async( search_iface, 
				getsearchkeys_cb, chan_struct );		
		
	iso_logger( "out get_search_keys" );
	
		
	}

/*!	\brief Function to get the search keys(fields on which to
*	search ) supported by server
* 	
*	\param hdr_req message header request
*	
*	\return gint MEM_ALLOCATION_ERROR
*/
gint get_search_keys( message_hdr_resp* hdr_req ) 
	{
	
	message_hdr_resp* hdr_resp = ( message_hdr_resp* ) 
						malloc ( sizeof ( message_hdr_resp ) );
						
	if ( NULL == hdr_resp )
		{
		return MEM_ALLOCATION_ERROR;		
		}
	
	memcpy( hdr_resp, hdr_req, sizeof ( message_hdr_resp ) );
	//ownership hdr_resp transfered
	tp_conn_request_channel_async( DBUS_G_PROXY( globalCon.conn ),
	                               TP_IFACE_CHANNEL_TYPE_CONTACT_SEARCH, 
	                               TP_CONN_HANDLE_TYPE_NONE, 0, TRUE,
	                               get_search_keys_reply, (gpointer)hdr_resp ) ;
	
	return 0;			
	}


/*!	\brief This is a callback to get_search_keys. The search keys
*	are sent to the requested client
*
*	\param proxy unused
*	\param instr instruction on search keys
*	\param keys 2D string array of search keys supported by server
*	\param error Error from server, if any
*	\param userdata get_search_keys_cb_struct 
*/	

static void getsearchkeys_cb( DBusGProxy *proxy,  gchar *instr,
  gchar **keys, GError *error, gpointer userdata )
	{
	
	get_search_keys_cb_struct * search_struct = ( 
				get_search_keys_cb_struct* )userdata;
				
	message_hdr_resp hdr_resp;
	gchar* msg = NULL;
	gint i = 0;
	gint len = 0;
	
	gint pri = MSG_PRI_NORMAL;
	gint result = 0;
	gint timeout = NO_WAIT;
	gint err = 0;
	
	
	UNUSED_FORMAL_PARAM( proxy );
	UNUSED_FORMAL_PARAM( instr );
	iso_logger( "in getsearchkeys_cb" );
	
	
	tp_chan_close_async( DBUS_G_PROXY( search_struct->search_chan ), 
					search_chan_closed_cb, NULL );
	
	if ( error ) 
		{
		send_response_to_client( search_struct->hdr_req, error->code, 0 );	
		
		free ( search_struct->hdr_req );
		free ( search_struct );
		
		return;
		}
		
	//Create the response header to be sent to client
	//Copy the message header request
	hdr_resp.hdr_req.message_type = ESearch_Get_Keys;
	hdr_resp.hdr_req.protocol_id = search_struct->hdr_req->protocol_id;
	hdr_resp.hdr_req.request_id = search_struct->hdr_req->request_id;
	hdr_resp.hdr_req.session_id = search_struct->hdr_req->session_id;
	//set continue flag
	hdr_resp.continue_flag = 0;
	hdr_resp.error_type = 0;
	hdr_resp.response = 1;	
	
	free ( search_struct->hdr_req );
	free ( search_struct );
	
	//Allocate memory for the message and set memory to '\0'	
	msg = ( gchar* ) malloc ( MAX_PARAM_LEN );
	if ( NULL == msg ) 
		{
		iso_logger( "malloc error @ pairs.msg" );
		return ;			
		}
	memset( msg, '\0', MAX_PARAM_LEN );
	len = 0;
	//Copy the response
	memcpy( msg, &hdr_resp, sizeof ( message_hdr_resp ) );
	len += sizeof( message_hdr_resp );
	
	
	iso_logger( "Get search keys : Keys are" );
	
	while ( keys[i] )
		{
		strcpy( msg + len, keys[i] );
		iso_logger( "Key %d : %s", i, keys[i] );
		len += 	strlen( keys[i] ) + 1;
		i++;
		}
		
	iso_logger( "Get search keys : Keys End" );
	
	//End the message with a '\0'
	len++;
	msg[len] = '\0';
	//Send the message to client
	result = MsgQSend( RESPONSE_QUEUE, msg, len, 
						pri, timeout, &err );
	
	
	
	if ( result )
		{
			
		}
	
	iso_logger( "out getsearchkeys_cb" );	
	
	}
