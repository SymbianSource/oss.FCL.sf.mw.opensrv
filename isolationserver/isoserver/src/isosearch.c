/*
* ============================================================================
*  Name        : isosearch.c
*  Part of     : isolation server.
*  Version     : %version: 17 %
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

/*! \def For GHashTable
*/
#include <glib/ghash.h>

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
* 
*/	
typedef struct key_value_struct key_value_struct;


static void search_result_received_cb ( DBusGProxy	*proxy,
									guint       contact_handle,
									GHashTable	*values, 
									gpointer	user_data
								   );
								   
static void search_state_changed_cb ( DBusGProxy	*proxy,
										guint       search_state,
										gpointer	user_data
									   );
									   
static void searchreply_cb( DBusGProxy *proxy, GError *error, gpointer userdata );


static void do_search_again( GHashTable* search_hash );

		
                      
/*! \brief This function called by action_parse_search parses rmsg and 
*	validates the arguments(key/value pairs). If value for any key found missing
*	returns INVALID_PARAMETERES
*
*	\param rmsg Message which is parsed
*	\param rmsg_len message len 
*	\param search_hash parsed key/value pairs are filled into this hash table
*
*	\return error if any else 0
*/
gint parse_for_search( gchar* rmsg, gint rmsg_len, GHashTable* search_hash )
	{
	gint pointer_nav = 0;
	gchar* value_str = NULL;
	gchar* key_str = NULL;
	gint err = 0;
	GValue *value = NULL; 
	
	iso_logger( "in parse_for_search");
	//Skip the header field
	pointer_nav += sizeof( message_hdr_req );
	
	while ( '\0' != *( rmsg + pointer_nav ) && pointer_nav < rmsg_len ) 
		{
		//Copy the key
		err = parse_a_string( rmsg, &key_str, &pointer_nav, rmsg_len );
		if ( err < 0 ) 
			{
			//if error, return error
			return err;
			}
		//End of message reached and no value found for
		//corresponding key
		if ( '\0' == *( rmsg + pointer_nav ) ) 
			{
			return INVALID_PARAMETERES;				
			}
		//Copy the value
		err = parse_a_string( rmsg, &value_str, &pointer_nav, rmsg_len );
		if ( err < 0 )
			{
			//if error, return error
			return err;				
			}
			
		value = g_new0( GValue, 1 );
		g_value_init( value, G_TYPE_STRING );
		g_value_set_string ( value, value_str );
	
	
		//insert the key value pair into the hash table
		g_hash_table_insert ( search_hash, key_str, value );						
		}
	iso_logger( "out parse_for_search");
	return 0;			
	}
	
/*!	\brief This function is called as a callback to search chan request
*	This function creates a search channel and interface which are
*	used for subsequent search requests. Logically this function is called 
*	only once per login session.
*	\remark Not creating a search channel each time is a workaround
*
*	\param proxy unused
*	\param chan_object_path channel object path to create a new channel
*	\param error if any
*	\param user_data hash table entry(key/value pair)
*
*	\return void
*/

 void do_search_reply( DBusGProxy *proxy, char *chan_object_path, 
 			GError *error, gpointer user_data )
 {
 	GHashTable *search_hash = ( GHashTable* ) user_data;
 	TpChan *search_chan = NULL;
	DBusGProxy *search_iface = NULL;
	
	
	UNUSED_FORMAL_PARAM( proxy );
 
 	iso_logger( "in do_search_reply");
 	/* Create the object to represent the channel */
	if ( error ) 
		{
		iso_logger( "Error in do_search_reply");
		//There was an error.. send it to client
		send_response_to_client( globalCon.search_hdr_req, error->code, 0 );
		//Should error be returned to the client ?
		return;
		}
	search_chan = tp_chan_new( globalCon.dbusConn, globalCon.connmgr_bus, chan_object_path, 
	TP_IFACE_CHANNEL_TYPE_CONTACT_SEARCH, TP_CONN_HANDLE_TYPE_NONE, 0 );
	                         
    globalCon.search_chan = search_chan;
	                    
	g_free(chan_object_path);
  
  
  	if ( NULL == search_chan ) 
		{
		iso_logger( "returning because of search_chan");
		//search chan not created
		//sending to client
		send_response_to_client( globalCon.search_hdr_req, TP_SEARCH_CHAN_ERROR, 0 );
		return ;	
		}
	//Get chan interface					
	search_iface = tp_chan_get_interface( search_chan, 
		TELEPATHY_CHAN_IFACE_CONTACTSEARCH_QUARK );	
	
	if ( NULL == search_iface ) 
		{
		iso_logger( "returning because of search_iface");
		//interface for search chan not created 
		//send that to client
		send_response_to_client( globalCon.search_hdr_req, TP_SEARCH_IFACE_ERROR, 0 );
		return ;	
		}					
	
	//Register for the SearchResultReceived signal
	//ownership of the srch_result->user_data is transfered
	dbus_g_proxy_connect_signal( search_iface, "SearchResultReceived",
							G_CALLBACK( search_result_received_cb ),
							NULL, NULL );
	
	
	//Register for the SearchStateChanged signal
	dbus_g_proxy_connect_signal( search_iface, "SearchStateChanged",
							G_CALLBACK( search_state_changed_cb ),
							NULL , NULL );
	//Call the search on tp		      										      
	tp_chan_type_search_async( search_iface, search_hash , searchreply_cb, 
								NULL );
			
	iso_logger( "out do_search_reply");			
	
  
 }


/*! \brief This function is called by action_parse_search to search for fields
*	\remark This function routes the searches to do_search_again if searched more 
*	than once. This is a workaround because we are getting the signal callbacks 
*	'n'ths( where n is nth search ) time if search channel is created each time. 
*
*	\param search_hash Hash table having the search key and value pairs to be searched
*
*	\return void
*/

gint do_search( GHashTable* search_hash ) 
	{
	
	iso_logger( "in do_search");
	
	if(globalCon.search_chan == NULL)
		{
		tp_conn_request_channel_async( DBUS_G_PROXY( globalCon.conn ),
	                               TP_IFACE_CHANNEL_TYPE_CONTACT_SEARCH, 
	                               TP_CONN_HANDLE_TYPE_NONE, 0, TRUE,
	                               do_search_reply, (gpointer)search_hash ) ;
		}

	else
		{
		 iso_logger( "calling do_search_again");
		 do_search_again(search_hash);
		}
	iso_logger( "out do_search");
	
	return 0;		
	}

/*! \brief This function is called if search is done more than once in same login session. 
*	\remark This is a workaround because we are getting the signal callbacks 
*	'n'ths( where n is nth search ) time. 
*
*	\param search_hash Hash table having the search key and value pairs to be searched
*
*	\return void
*/
void do_search_again( GHashTable* search_hash )
 {
 
	DBusGProxy *search_iface = NULL;
	
	iso_logger( "in do_search_again");
 	
  	if ( NULL == globalCon.search_chan ) 
		{
		iso_logger( "returning because of search_chan");
		//search chan not created
		//sending to client
		send_response_to_client( globalCon.search_hdr_req, TP_SEARCH_CHAN_ERROR, 0 );
		return ;	
		}
	//Get chan interface					
	search_iface = tp_chan_get_interface( globalCon.search_chan, 
		TELEPATHY_CHAN_IFACE_CONTACTSEARCH_QUARK );	
	
	if ( NULL == search_iface ) 
		{
		iso_logger( "returning because of search_iface");
		//interface for search chan not created 
		//send that to client
		send_response_to_client( globalCon.search_hdr_req, TP_SEARCH_IFACE_ERROR, 0 );
		return ;	
		}					
	

	//Call the search on tp		      										      
	tp_chan_type_search_async( search_iface, search_hash, searchreply_cb, 
			NULL );
			
	iso_logger( "out do_search_again");			
	
  
 }


/*
! /brief This function is called by message_send_recv function in isoservermain.c if message type
*   is ESearch. This function parses the rmsg, validates the parameter passed, if parameters are 
*   correct a search performed for them. else INVALID_PARAMETERES error is returned
*
*   /param rmsg message buffer to be parsed
*   /param rmsg_len the length of the rmsg
*   /remark rmsg_len is not strlen(rmsg)
*   /return returns error code or 0 on success
*/
gint action_parse_search( gchar* rmsg, gint rmsg_len ) 
	{
		
	gint err = 0;
	GHashTable* search_hash = NULL;
	message_hdr_req* hdr_req = NULL;
	//Allocate memory and set the memory to '\0'
	iso_logger( "in action_parse_search");	
	hdr_req = ( message_hdr_req* ) malloc ( sizeof ( message_hdr_req ) );
	
	if ( NULL == hdr_req ) 
		{
		return MEM_ALLOCATION_ERROR;			
		}
	memset( hdr_req, '\0', sizeof( message_hdr_req ) );
	//Copy the header into message struct
	memcpy( hdr_req, rmsg, sizeof( message_hdr_req ) );
	//Create the hash table
	search_hash = g_hash_table_new( g_str_hash, g_str_equal );
	if ( NULL == search_hash ) 
		{
		return MEM_ALLOCATION_ERROR;			
		}
	//Parse the key value pairs to be searched into the hash
	err = parse_for_search( rmsg, rmsg_len, search_hash );
	if ( !err ) 
		{
		//Do the search on the fields
		globalCon.search_hdr_req = hdr_req;
		err = do_search( search_hash );		
		}
	iso_logger( "out action_parse_search");	
	
	return err;	
	}
	
/*! \brief This function is called by search_results_recieved for each key/value 
* 	pair of search result(one entry). This function appends the key/value pair 
* 	to the message buffer in the key_value_struct(.msg) and increments 
*	the no. of bytes written to(key_value_struct.len)
*
*	Format of response header(msg buffer)	
*	key1\0value1\0key2\0value2\0(key3\0value3\0) (3rd round append)
*
*	\param key one field of the search entry
*	\param value value for the above field
*	\param user_data unused
*
*	\return void
*/

void key_value_store( gpointer key, gpointer value, gpointer user_data ) 
	{
	
	key_value_struct* pairs = ( key_value_struct * ) user_data;	
	GValue* value1 = ( GValue* ) value;
	const gchar *value_str = NULL; 
	const gchar *key_str = ( gchar* ) key; 
	
	iso_logger( "in key_value_store");
	
	value_str = g_value_get_string ( value1 );
	
	if ( '\0' == *value_str ) 
		{
		iso_logger( "No value");
		return;			
		}
	
	iso_logger( "key is %s", key_str );
	//copy The results key part 
	strcpy( pairs->msg + pairs->len, key_str );
	pairs->len += strlen( key_str ) + 1;
	
	iso_logger( "value is %s", value_str );
	//copy The results value part 
	strcpy( pairs->msg + pairs->len, value_str );
	pairs->len += strlen( value_str ) + 1;
	iso_logger( "out key_value_store");	
	}
	
	
/*!
* 	\brief This function is called when the search results are recieved
* 	This function is called for each contact found matching the 
* 	search criteria. 
*
*	\param proxy unused
*	\param contact_handle unused
*	\param results Key value pair of one search result entry
*	\param user_data unused
*
*	\return void
*/
static void search_result_received_cb ( DBusGProxy	*proxy,
										guint       contact_handle,
										GHashTable	*results, 
										gpointer	user_data
									   )
	{
	message_hdr_resp hdr_resp;
	//Get the header request

	message_hdr_req* hdr_request = globalCon.search_hdr_req;
	key_value_struct pairs;
	
	gint pri = MSG_PRI_NORMAL;
	gint result = 0;
	gint timeout = NO_WAIT;
	gint err = 0;
	
	//Create the handle 
	iso_logger( "in search_result_received_cb");
	
	UNUSED_FORMAL_PARAM( proxy );
	UNUSED_FORMAL_PARAM( contact_handle );
	UNUSED_FORMAL_PARAM( user_data );
	
	if ( g_hash_table_size( results ) <= 0 ) 
		{
		iso_logger( "No results recieved");
		return;			
		}
	
	iso_logger("search result count for hash table %d", 
			g_hash_table_size( results ) );
	iso_logger("contact handle is %d", 
			contact_handle );
	
	//Create the response header to be sent to client
	//Copy the message header request
	//Not copying from hdr req because it could have been modified
	//in search_state_changed_cb
	hdr_resp.hdr_req.message_type = ESearch;
	hdr_resp.hdr_req.protocol_id = hdr_request->protocol_id;
	hdr_resp.hdr_req.request_id = hdr_request->request_id;
	hdr_resp.hdr_req.session_id = hdr_request->session_id;
	//set continue flag
	hdr_resp.continue_flag = 0;
	hdr_resp.error_type = 0;
	hdr_resp.response = 1;
	//Allocate memory for the message and set memory to '\0'	
	pairs.msg = ( gchar* ) malloc ( MAX_PARAM_LEN );
	if ( NULL == pairs.msg ) 
		{
		iso_logger( "malloc error @ pairs.msg" );
		return ;			
		}
	memset( pairs.msg, '\0', MAX_PARAM_LEN );
	pairs.len = 0;
	//Copy the response
	memcpy( pairs.msg, &hdr_resp, sizeof ( message_hdr_resp ) );
	pairs.len += sizeof( message_hdr_resp );
	//Copy the contact name
	iso_logger( "%s%d", "message type in search_result_received_cb", hdr_request->message_type );


	iso_logger( "%s", "start -------------------------------------------------" );

	iso_logger( "%s", "contacts key value pair start" );

	//Copy all the key/value pairs : 
	//no need to check for size here as it is already done at top
	g_hash_table_foreach ( results, key_value_store, &pairs );	
	
	iso_logger( "%s", "end -------------------------------------------------" );

	//End the message with a '\0'
	pairs.len += 1;
	pairs.msg[pairs.len] = '\0';
	//Send the message to client
	result = MsgQSend( RESPONSE_QUEUE, pairs.msg, pairs.len, 
						pri, timeout, &err );
						
	if ( result < 0 )
		{
			
		}
		
	//Free the hash table itself
	g_hash_table_destroy ( results );
	
	iso_logger( "in search_result_received_cb");
	}
	
/*! \brief This function is a callback registered to searchstatechanged signal.
*	This function is called by telepathygabble when status of the search is changed.
*
*	\param proxy unused
*	\param search_state state of the search
*	\param user_data unused
*
*	\return void
*/
static void search_state_changed_cb ( DBusGProxy	*proxy,
										guint       search_state,
										gpointer	user_data
									   )
	{

	gint err = 0;
	//Get the header request
	
	iso_logger( "in search_state_changed_cb");
	
	UNUSED_FORMAL_PARAM( proxy );
	UNUSED_FORMAL_PARAM( user_data );
	

	if ( 1 == search_state && globalCon.search_hdr_req )
		{
		globalCon.search_hdr_req->message_type = ESearch_State_During;
		err = send_response_to_client( globalCon.search_hdr_req, 0, 1 );
		if ( err ) 
			{
			iso_logger( " 1 == search_state Error sending to client");
			//?			
			}
		}
	else if ( TP_CHANNEL_CONTACT_SEARCH_STATE_AFTER == search_state 
					 && globalCon.search_hdr_req )
		{
		globalCon.search_hdr_req->message_type = ESearch_State_Finished;
		err = send_response_to_client( globalCon.search_hdr_req, 0, 1 );
		if ( err ) 
			{
			iso_logger( "Error sending to client");
			//?			
			}
		//Since the search is finished delete the hdr request
		free ( globalCon.search_hdr_req );
		globalCon.search_hdr_req  = NULL;
		}
	
	
	iso_logger( "out search_state_changed_cb");
	}
	
/*!	\brief Function called by telepathygabble when search channel is closed
*	This function is regisered when calling the async function to close channel
*
*	\param	proxy unused
*	\param	error Error if any
*	\param	user_data unused
*
*	\return void
*/
void search_chan_closed_cb ( DBusGProxy	*proxy,
									GError *error,
									gpointer	user_data
								  )
	{
	iso_logger( "in search_chan_closed_cb");
	UNUSED_FORMAL_PARAM( proxy );
	UNUSED_FORMAL_PARAM( error );
	UNUSED_FORMAL_PARAM( user_data );
	g_object_unref(globalCon.search_chan);
	globalCon.search_chan = NULL;
	iso_logger( "out search_chan_closed_cb");
	}
	
/*!	\brief Function called by telepathygabble when search request is sent to server.
*	This function is can be thought of as an ack for search request from server.
*
*	\param	proxy unused
*	\param	error Network error if any
*	\param	user_data unused
*
*	\return void
*/	
static void searchreply_cb( DBusGProxy *proxy, GError *error, gpointer userdata ) 
	{
	
	
	iso_logger( "in searchreply_cb" );
	UNUSED_FORMAL_PARAM( proxy );
	UNUSED_FORMAL_PARAM( userdata );
	
	if ( error ) 
		{
		
		iso_logger( "Error!!!!!" );
		//err cant be handled as of now/this is a special case
		//where mem alloc is not possible when sending to client..
		send_response_to_client( globalCon.search_hdr_req, error->code, 0 );
		
		tp_chan_close_async( DBUS_G_PROXY( globalCon.search_chan ), 
					search_chan_closed_cb, NULL );
		//Header request is deleted only in case of error
		//Because the search_state_finished_cb will not be called
		//In othr cases ownership of user_data is with search_state_finished_cb
		
		free ( globalCon.search_hdr_req );
		globalCon.search_hdr_req = NULL;
		}
	
	iso_logger( "out searchreply_cb");

	}
