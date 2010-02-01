/*
* ============================================================================
*  Name        : isoconnectionmanager.c
*  Part of     : isolation server.
*  Version     : %version: 24 %
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



#include <glib.h>
#include <gtypes.h>
#include <sys/types.h> 
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 

#include "isoconnectionmanager.h"
#include "isomodifycontactlist.h" //For roster_members_changed_cb
#include "isoutils.h" //for loger and othr func
#include "isofetchcontactlist.h" //For request_roster

#include "msgliterals.h"
#include "isoservermain.h"
#include "isopresence.h"
#include "isosearch.h"
#include "msg_enums.h"

enum disconnect_reason
    {
    EConnection_Status_Reason_None_Specified = 0,
    EConnection_Status_Reason_Requested,
    EConnection_Status_Reason_Network_Error,
    EConnection_Status_Reason_Authentication_Failed,
    };
//userConnection globalCon;

/*!	\file 
*	Impliments the functions in isoconnectionmanager.h
*/

/*!	\brief Documented in the header file
*/
static void check_conn_properties(TpConn *conn)
{
    TpPropsIface *pres_iface;

  	pres_iface = TELEPATHY_PROPS_IFACE (tp_conn_get_interface (
        						conn, TELEPATHY_CONN_IFACE_PRESENCE_QUARK));
  	if (pres_iface != NULL)
  		{
  		globalCon.pres_iface = pres_iface;
	  	dbus_g_proxy_connect_signal(DBUS_G_PROXY(pres_iface),
					    "PresenceUpdate",
					    G_CALLBACK(telepathy_presence_handler),
					    NULL, NULL);
  		}
  	else 
  		{
    	g_warning ("The connection object does not support " TP_IFACE_PROPERTIES);  
  		}
}

/*! \brief This function is registered in action_login for statuschanged signal.
 *  This function is called by telepathygabble when the status of the user changes
 *  Once user is connected this function registers for NewChannel signal with telepathygabble 
 *  client is also informed of the status change..
 *
 *  \param proxy
 *  \param status status of the user ( connected/connecting/disconnected )
 *  \param reason reason for status change(server disconnected/ user requested etc.,)
 *  \param user_data  message header request
 *
 *  \return : Boolean True on success, false on failure
 */

static gboolean status_changed_cb(DBusGProxy *proxy,
				  guint status, guint reason,
				  gpointer user_data)
	{

	
	int err = 0;
	
	message_hdr_req* hdr_req = ( message_hdr_req* )user_data;
	// create the msg queue
	iso_logger( "%s", "In - status_changed_cb\n" );
	
	//switch to the connection status
	switch ( status ) 
		{
		case EConnected:
			{
			//Connected..
			iso_logger( "%s", "connected\n" );
			//Get connection interfaces
			check_conn_properties(TELEPATHY_CONN(proxy));
			//Connect to the signal for new channels
			dbus_g_proxy_connect_signal( DBUS_G_PROXY( globalCon.conn ), "NewChannel",
			      G_CALLBACK( new_channel_handler ),
			       NULL, NULL );
		   	//send response to client..
			err = send_response_to_client( hdr_req, reason, 1 );
			//hdr_req would be freed here
			//Disconnect the previous signal for connected
			//This is to pass the diff msg hdr as cb userdata for disconnect
			/*dbus_g_proxy_disconnect_signal( DBUS_G_PROXY( globalCon.conn ), StatusChangedStr,
					G_CALLBACK( status_changed_cb ),
					hdr_req );*/
			
			globalCon.conn_stat = connected;
			break;
			}
		case EConnecting:
			{//Connecting..
			iso_logger( "%s", "connecting\n" );
			//already done after creating the thread
			//globalCon.conn_stat = connecting;
			break;
			}
		case ENotConnected: 
			{
			//DisConnected....
			//Set the reason for disconnection
			iso_logger( "%s : %d : %s : %d", "disconnected and reason is", reason, "state is ", globalCon.conn_stat );
			//reset the flag to not connected
			
			//should here be  not_connected != globalCon.conn_stat  &&  disconnected != globalCon.conn_stat 
			//and all login errors should set the conn_stat to disconnected
			if ( not_connected != globalCon.conn_stat &&  disconnected != globalCon.conn_stat ) 
				{
				int success = 1;
				int error = 0;
				
				action_logedout();
				
				//switch for the reason of logout
				switch ( reason )
				    {
				    //set appropriate message types and error if any
				    case EConnection_Status_Reason_None_Specified:
				        if ( connecting == globalCon.conn_stat )
				            {
				            //not yet connected and user has cancelled the a/p
				            hdr_req->message_type = ELogin_Request;
				            error = ELOGIN_NONE_SPECIFIED;
				            success = 0;
				            }
				        else {
				            //connected and user has cancelled the a/p
                            //state is going to be not connected as fr these cases ossadaptation is 
                            //going to send handle request terminated signal 
				            hdr_req->message_type = EUserEndGprs;
				            
				            }
				        
				        break;
				    case EConnection_Status_Reason_Requested:
				        hdr_req->message_type = ELogout_Request;
				        
				        break;
				    case EConnection_Status_Reason_Network_Error:
				        if ( connecting == globalCon.conn_stat )
                            {
                            //not yet connected and user has cancelled the a/p
                            hdr_req->message_type = ELogin_Request;
                            error = ELOGIN_NETWORK_ERROR;
                            success = 0;
                            }
				        else {
                            //state is going to be not connected as fr these cases ossadaptation is 
				            //going to send handle request terminated signal
                            hdr_req->message_type = EServer_DisConnect_Request;   
                            
				        }
				        
				        break;
				    case EConnection_Status_Reason_Authentication_Failed:
				        hdr_req->message_type = ELogin_Request;
				        error = ELOGIN_AUTHENTICATION;
				        success = 0;
				        
				        break;
				    default:
				        hdr_req->message_type = ELogin_Request;
                        error = ELOGIN_AUTHORIZATION;
                        success = 0;
                        
                        break;

				    }
				//to set to no more requests allowed
				globalCon.conn_stat = not_connected;
				
				err = send_response_to_client( hdr_req, error, success );
				
				}
						
			break;
			}
		default:
			{
			break;	
			}
		
		}

	if ( 0 != err )
		{
		//failed
		return ERROR;	
		}
	iso_logger( "%s", "out - status_changed_cb\n" );
	return TRUE;
	}
	
/*! \brief new_channel_handler is registered as a callback for any new 
 *  channels craetion request, in function status_changed_cb after status is changed 
 *  to log in. new_channel_handler creates channel and also registers for the signals
 *  MembersChanged for contact related channels and Recieved and SendError for IM
 *  related channels.
 *
 *  \param proxy unused
 *  \param object_path object path of the channel
 *  \param channel_type used to get a new channel
 *  \param handle_type used to get a new channel, channel handle type
 *  \param handle used to get a new channel, channel handle 
 *  \param suppress_handler unused
 *  \param user_data unused
 *
 *  \return void
 */
static void new_channel_handler( DBusGProxy *proxy, const char *object_path,
				const gchar *g_channel_type, guint handle_type,
				guint handle, gboolean suppress_handler,
				gpointer user_data )
	{
	
	TpChan *new_chan = NULL;
	UNUSED_FORMAL_PARAM(proxy);
	UNUSED_FORMAL_PARAM(suppress_handler);
	UNUSED_FORMAL_PARAM(user_data);
	iso_logger( "%s", "In - new_channel_handler\n" );
	
	//create new channel
	new_chan = tp_chan_new( globalCon.dbusConn,
	       globalCon.connmgr_bus, object_path,
		       g_channel_type, handle_type, handle );

	if ( !new_chan ) 
		{
		return ;
		}	
		
	
	if ( strcmp( g_channel_type, TP_IFACE_CHANNEL_TYPE_CONTACT_LIST  ) == 0 
	 		&& ( g_strrstr(object_path, "RosterChannel/known" )  ) )
		{
		//channel type is contact rosters..
		//request for the roster
			
		globalCon.group_iface_known =
	     	tp_chan_get_interface( new_chan,
					      TELEPATHY_CHAN_IFACE_GROUP_QUARK);
		//register for the members changed signal			      			      
		dbus_g_proxy_connect_signal (globalCon.group_iface_known, "MembersChanged",
						     G_CALLBACK (roster_members_changed_cb),
						     NULL, NULL);			      		       
				       
		}
	else if ( strcmp( g_channel_type, TP_IFACE_CHANNEL_TYPE_CONTACT_LIST  ) == 0 
	 		&& ( g_strrstr(object_path, "RosterChannel/publish" )  ) )
		{
		//channel type is contact rosters..
		//request for the roster
			
		globalCon.group_iface_publish =
	     	tp_chan_get_interface( new_chan,
					      TELEPATHY_CHAN_IFACE_GROUP_QUARK);
		//register for the members changed signal			      			      
		dbus_g_proxy_connect_signal (globalCon.group_iface_publish, "MembersChanged",
						     G_CALLBACK (roster_members_changed_cb),
						     NULL, NULL);
					      		       
		request_roster( EPublish_Channel );		       
		} 		
	else if ( strcmp( g_channel_type, TP_IFACE_CHANNEL_TYPE_CONTACT_LIST  ) == 0 
 		&& (g_strrstr(object_path, "RosterChannel/subscribe") ) )
 		  
		{
		//roster fetch should be done using subscribe, as it will give 
		//current and remote pending members in differnet arrays, that can be sued on ui side
		//local pending members can be fetched only using "RosterChannel/publish",that can be fetched if required
		
		globalCon.group_iface_subscribe =
	     	tp_chan_get_interface( new_chan,
					      TELEPATHY_CHAN_IFACE_GROUP_QUARK);
					      
			
		//register for the members changed signal			      			      
		dbus_g_proxy_connect_signal (globalCon.group_iface_subscribe, "MembersChanged",
						     G_CALLBACK (roster_members_changed_cb),
						     NULL, NULL);
		request_roster( ESubscribe_Channel );
	    				     
					       
		}
				
	//recieve message channel handlers
	else if ( ( strcmp( g_channel_type, TP_IFACE_CHANNEL_TYPE_TEXT ) == 0 ) )
		{
		//recieve message 
		if ( globalCon.text_channels == NULL )
			{
			globalCon.text_channels = g_hash_table_new_full( g_str_hash,
				              g_str_equal,
				             ( GDestroyNotify ) g_free,
				             ( GDestroyNotify ) g_object_unref );	
			}
		if ( globalCon.text_channels ) 
			{
			g_hash_table_insert( globalCon.text_channels, 
					g_strdup( object_path ), new_chan );		
			}
		
		iso_logger( "b4 text_channel_init" );
		//check for  new_chan objects for text channel should be loaclly stored/freed
		text_channel_init( new_chan );
		}
	iso_logger( "%s", "Out - new_channel_handler\n" );
	}    


/*! \brief Is called after logout.. Any cleanup operations to
 *   be performed here. Setting isConnected to EFalse, cleaning up 
 *  of resources is done
 */
void action_logedout() 
	{
	
	iso_logger( "%s", "In - action_logedout\n" ); 
    //not quiting the main loop
    //release the connection manager
    if ( globalCon.connmgr ) 
	    {
	    g_object_unref ( globalCon.connmgr );
		globalCon.connmgr = NULL;	
	    }
	//release the dbus connection
	if ( globalCon.dbusConn ) 
	    {
	    dbus_g_connection_unref ( globalCon.dbusConn );
		globalCon.dbusConn = NULL;	
	    }
	//release the connection object
	if ( globalCon.conn ) 
	    {
	    g_object_unref ( globalCon.conn );
		globalCon.conn = NULL;	
	    }
	//release the text channels
	if ( globalCon.text_channels ) 
	    {
	    g_hash_table_destroy( globalCon.text_channels ); 
		globalCon.text_channels = NULL;	
	    }
	//release the subscribe interface   
	if ( globalCon.group_iface_subscribe ) 
	    {
	    g_object_unref ( globalCon.group_iface_subscribe );
		globalCon.group_iface_subscribe = NULL;	
	    }
	//release the publish interface       
	if ( globalCon.group_iface_publish ) 
	    {
	    g_object_unref ( globalCon.group_iface_publish );
		globalCon.group_iface_publish = NULL;	
	    }
	//release the known interface       
	if ( globalCon.group_iface_known ) 
	    {
	    g_object_unref ( globalCon.group_iface_known );
		globalCon.group_iface_known = NULL;	
	    }
	
	iso_logger( "%s", "Out - action_logedout\n" );
	}
	
/*! \brief Callback for the logout 
 * 
 *  \param proxy : unused
 *  \param error : unused
 *  \param userdata : unused
 *  \return : void
 */
void logout_cb( DBusGProxy *proxy, GError *error, gpointer userdata )
	{

	iso_logger( "%s", "In - logout_cb\n" );
        UNUSED_FORMAL_PARAM(proxy);
        UNUSED_FORMAL_PARAM(error);
        UNUSED_FORMAL_PARAM(userdata);
	iso_logger( "%s", "Out - logout_cb\n" );
	}
	
/*! \brief requests for closing of session. Also closes the search channel.
 *  \remark should not close the search channel
 * 
 *  Request for log - out
 *  \param pc - message  which has the request header
 *  \return : MEM_ALLOCATION_ERROR or 0
 */
int action_logout( char* pc ) 
	{
	
	TpConn* conn = globalCon.conn;
	message_hdr_req *msg_hdr = NULL;

	iso_logger( "%s", "In - action_logout\n" );
    
    if( globalCon.search_chan )
	    {
	    tp_chan_close_async( DBUS_G_PROXY( globalCon.search_chan ), search_chan_closed_cb, NULL );	
	    }

	//allocate memory and check for errors
	msg_hdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
	if ( NULL == msg_hdr ) 
		{
		return MEM_ALLOCATION_ERROR;	
		}
	//intialize the memory to 0s
    memset( msg_hdr, '\0', sizeof( message_hdr_req ) );
    //read message header from buffer
    memcpy( msg_hdr, pc, sizeof( message_hdr_req ) );
	
	//disconnect in async way as glib signal is not workin
	tp_conn_disconnect_async( DBUS_G_PROXY( conn ), logout_cb, msg_hdr );
	iso_logger( "%s", "Out - action_logout\n" );
	
	return 0;
	}

/*! \brief validates params, gets a dbus, creates a connection manager,
 *  and connection. Registers for the statuschange
 * 
 *  \param ls_hdr login struct
 *  \return Errors if invalid param or getting dbus, conn mgr or conn fails else 0
 */
gint action_login( login_struct* ls_hdr ) 
	{
	
	DBusGConnection *connection = NULL;
	TpConnMgr *connmgr = NULL;
	TpConn	*conn = NULL;
	GError *error = NULL;
	
	gchar* def1 = NULL;
	char* def2 = NULL;
	char* def3 = NULL; 
	
	gchar* def4 = NULL;
	guint def5 = 0;
	gboolean def6 = 0;
		
	#ifdef __WINSCW__	
	gchar* def8 = NULL;
	guint def9 = 0; 
	#endif
		
    //Build hash table
    GHashTable *connection_parameters = NULL;
    //initalize the Gvalues
	GValue *value1 = g_new0(GValue, 1);
	GValue *value2 = g_new0(GValue, 1);
	GValue *value3 = g_new0(GValue, 1);
	GValue *value4 = g_new0(GValue, 1);
	GValue *value5 = g_new0(GValue, 1);
	GValue *value6 = g_new0(GValue, 1);
	
	#ifdef __WINSCW__	
	GValue *value9 = g_new0(GValue, 1);
	GValue *value10 = g_new0(GValue, 1);
	#endif
	
	//log message
	iso_logger( "%s", "In - action_login\n" );
	
	//Check if all the mandatory settings are there
	if ( NULL == ls_hdr->strs[0] || NULL == ls_hdr->strs[1] || NULL == ls_hdr->strs[2]
		|| NULL == ls_hdr->strs[4] || NULL == ls_hdr->strs[5] ||
		NULL == ls_hdr->strs[7] || NULL == ls_hdr->strs[8] || NULL == ls_hdr->strs[9] ) 
		{
		return INVALID_PARAMETERES;
		}
	//Only for winscw 	
	#ifdef __WINSCW__
	if ( NULL == ls_hdr->strs[10] || NULL == ls_hdr->strs[11] ) 
		{
		return INVALID_PARAMETERES;
		}
	#endif
	
	//0 is username
	def1 = g_strdup( ls_hdr->strs[0] );
	//1 is passwd
	def2 = g_strdup( ls_hdr->strs[1] ) ; 
	//2 is server addr
	def3 = g_strdup( ls_hdr->strs[2] ) ; 
	if ( NULL != ls_hdr->strs[3] ) 
		{
		//3 is resource	
		def4 = g_strdup( ls_hdr->strs[3] ) ;
		} 
	else 
		{
		//Fix required from lalitha
		//Is this manadatory? or Can this be NULL
		def4 = g_strdup( TestId );
		}
	//4 is ssl
	def5 = atoi( ls_hdr->strs[4] );
	//5 is server port
	def6 = atoi( ls_hdr->strs[5] );
	//6 is IapId is not used
	//def7 = g_strdup( ls_hdr->strs[6] ) ;
	
	iso_logger( "username is %s", ls_hdr->strs[0] );
	iso_logger( "server is %s", ls_hdr->strs[2] );
	iso_logger( "resource is %s", ls_hdr->strs[3] );
	iso_logger( "ssl is %s", ls_hdr->strs[4] );
	iso_logger( "server port is %s", ls_hdr->strs[5] );
	iso_logger( "connmgr bus is %s", ls_hdr->strs[7] );
	iso_logger( "connmgr path is %s", ls_hdr->strs[8] );
	iso_logger( "protocol is %s", ls_hdr->strs[9] );
	
	
	globalCon.connmgr_bus = strdup( ls_hdr->strs[7]);
	
	if ( NULL == globalCon.connmgr_bus ) 
		{
		return MEM_ALLOCATION_ERROR;
		}
	
	#ifdef __WINSCW__
	//7 is ProxyServer
	def8 = g_strdup( ls_hdr->strs[10] ) ;
	//proxy port
	def9 = atoi( ls_hdr->strs[11] );
	#endif
	
	//Get DBus - do not change the order of following calls to 
	//dbus, g_hash_table_new
	

	connection = dbus_g_bus_get( DBUS_BUS_SESSION , &error);
	
	//DBus Connection is NULL		
	//return with error
	if ( connection == NULL ) 
		{
		if ( error ) 
			{
			g_error_free(error);
			}
		iso_logger( "failed in  connection == NULL " );
		return DBUS_CONNECTION_ERROR;		
		}
	/* printf("connected to DBus with connection %p\n", connection);*/
    globalCon.dbusConn = connection;

	//Get connection manager
	connmgr = tp_connmgr_new(connection, ls_hdr->strs[7], ls_hdr->strs[8],TP_IFACE_CONN_MGR_INTERFACE);
	if ( connmgr == NULL ) 
		{
		iso_logger( "failed in tp_connmgr_new" );
		//Free already allocated resources
		//return with error
		dbus_g_connection_unref ( globalCon.dbusConn );
		globalCon.dbusConn = NULL;	
		return TP_CONNMGR_ERROR;
		}
	
	//Create values for hash table 
	connection_parameters = g_hash_table_new( g_str_hash, g_str_equal );
	if ( NULL == connection_parameters ) 
		{
		//Free already allocated resources
		//return with error
	    g_object_unref (connmgr);
		connmgr = NULL;	
		dbus_g_connection_unref ( globalCon.dbusConn );
		globalCon.dbusConn = NULL;	
		iso_logger( "failed in g_hash_table_new" );
		return TP_HASHTABLE_ERROR;	
		}
		
	//Add AccountStr values to hash table
	g_value_init( value1, G_TYPE_STRING );
	g_value_set_string ( value1, def1 );
	g_hash_table_insert( connection_parameters, ( gpointer ) AccountStr, value1 ); 
	
	//Add PasswdStr values to hash table
	g_value_init( value2, G_TYPE_STRING ); 
	g_value_set_string ( value2, def2 );
	g_hash_table_insert( connection_parameters, ( gpointer ) PasswdStr, value2 ); 
	//Add ServerStr values to hash table
	g_value_init(value3, G_TYPE_STRING); 
	g_value_set_string (value3, def3);
	g_hash_table_insert( connection_parameters, ( gpointer ) ServerStr, value3 ); 
	//Add ResourceStr values to hash table
	if ( NULL != def4 ) 
		{
		//This is not mandatory settings item
		//so check for NULL value before useing it..
		g_value_init( value4, G_TYPE_STRING ); 
		g_value_set_string ( value4, def4 );
		g_hash_table_insert( connection_parameters, ( gpointer ) ResourceStr, value4 ); 	
		}
	
	//Add PortStr values to hash table
	g_value_init(value5, G_TYPE_UINT); 
	g_value_set_uint (value5, def6);
    g_hash_table_insert(connection_parameters, ( gpointer ) PortStr, value5); 
	//Add OldsslStr values to hash table
    g_value_init(value6, G_TYPE_BOOLEAN);
	g_value_set_boolean (value6, def5);
    g_hash_table_insert(connection_parameters, ( gpointer ) OldsslStr, value6 ); 
    //Settings Only for emulator
    #ifdef __WINSCW__   
    //Add proxy server values to hash table 
    g_value_init(value9, G_TYPE_STRING); 
	g_value_set_string (value9, def8);
    g_hash_table_insert(connection_parameters, "https-proxy-server", value9); 

	//Add proxy port values to hash table
    g_value_init(value10, G_TYPE_UINT); 
	g_value_set_uint (value10, def9);
    g_hash_table_insert(connection_parameters, "https-proxy-port", value10); 
	#endif

    
	//Get connection from connection manager - i.e., Login
	conn = tp_connmgr_new_connection( connmgr,connection_parameters, ls_hdr->strs[9] );
	
	if ( !conn )
		{
		iso_logger( "failed in tp_connmgr_new_connection" );
		//Free already allocated resources
		//return with error
		g_object_unref (connmgr);
		connmgr = NULL;	
		dbus_g_connection_unref ( globalCon.dbusConn );
		globalCon.dbusConn = NULL;
		
		g_hash_table_destroy( connection_parameters );
		return TP_CONNECTION_ERROR;
		}
		
	
	//register for StatusChanged callback - login / logout			      
	dbus_g_proxy_connect_signal( DBUS_G_PROXY( conn ), StatusChangedStr,
					G_CALLBACK( status_changed_cb ),
					ls_hdr->hdr_req, NULL );
	//Assign to global variable struct				
	globalCon.conn = conn;
	globalCon.connmgr = connmgr;
	globalCon.dbusConn = connection;
	//Free 
	g_hash_table_destroy( connection_parameters );
	iso_logger( "%s", "Out - action_login\n" );
	
	return 0;	
	}	

/*! \brief parses the message buffer and validates the parameters. The values are written
 *  to ls_hdr
 *
 *  \param aMsgBuffer - buf to be parsed
 *  \param ls_hdr - login_struct struct to which values are written 
 *  \param msg_len - no. of bytes in the message buffer aMsgBuffer
 *
 */

gint parse_for_login( gchar* msg_buffer, login_struct** msg_struct, int msg_len ) 
	{

	char* bufPtr = NULL;
	gint index = 0;
	gint arg_count = 0;
	gint str_len = 0;
	char *str1 = NULL;
	
	message_hdr_req *msg_hdr = NULL;
	
	iso_logger( "%s","In - parse_for_login\n" );
	//Reading of the message_hdr_req is done twice..
	//should be changed to read once..
	//Allocate memory and check for the error
	msg_hdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
	if ( NULL == msg_hdr ) 
		{
		return MEM_ALLOCATION_ERROR;
		}
	//initalize memory to zero	
    memset( msg_hdr, '\0', sizeof( message_hdr_req ) );
    memcpy( msg_hdr, msg_buffer, sizeof( message_hdr_req ) );
    //ownership of msg_hdr is transefred to msg_struct
    ( *msg_struct )->hdr_req = msg_hdr;
	
	bufPtr = msg_buffer + sizeof( message_hdr_req );
	
	//validating each arg and copying it..
	
	while ( arg_count < LOGIN_ARG_COUNT ) 
		{
		//Get one string 
		while(  index + str_len < msg_len  && '\0' != *( bufPtr  + index + str_len ) ) 
			{
			str_len++;
			//Check if max param len no. of chars already read..
			//if so return with error
			if ( MAX_PARAM_LEN == index + str_len ) 
				{
				return INVALID_PARAMETERES;
				}
			}
		//str_len == 0, means one setting attrib missing..
		//Assign that to NULL..and continue		
		if ( 0 == str_len ) 
			{
			( *msg_struct )->strs[arg_count] = NULL;
			//Increment the no of args read by one
			arg_count++;
			index++;
			continue;
			}
		//ownership of these strs is transfered to msg_struct
		//Allocate memory and check for the error	
		str1 = ( char* ) malloc ( str_len + 1 );
		if ( NULL == str1 ) 
			{
			return MEM_ALLOCATION_ERROR;
			}
		//Copy the string from buffer
		//copy '\0' as well
		memcpy( str1, bufPtr + index, str_len + 1 ); 
		//Assign to the array
		( *msg_struct )->strs[arg_count] = str1;
		//Skip the '\0' already read..
		index += str_len + 1;
		//reset the str_len to read next char
		str_len = 0;
		//Increment the no of args read by one
		arg_count++;
		
		}
	iso_logger( "%s", "Out - parse_for_login\n" );
	return 0;
	}
/*! \brief Calls parse_for_login to parse and validate the arguments
 *  And then calls action_login for to do actual login
 *
 *  \param aMsgBuf message buffer
 *  \param msg_len - len of the message
 */
gint action_parse_login( char* aMsgBuf, int msg_len ) 
	{

	gint message_type_err = 0;
	login_struct* message_struct;
	gint arg_count = 0;
	
	iso_logger( "%s", "In - action_parse_login\n" );
	//Allocate memory and check for errors
	message_struct = ( login_struct* ) malloc( sizeof ( login_struct ) );
	if ( NULL == message_struct ) 
		{
		return MEM_ALLOCATION_ERROR;
		}
	//intialize memory to 0s
	memset ( message_struct, '\0', sizeof ( login_struct ) );
	
	//Parse the message into login parameters
	message_type_err = parse_for_login( aMsgBuf, &message_struct, msg_len );
	//If no error in parsing do a login
	if ( !message_type_err ) 
		{
		//parse passed
		message_type_err = action_login( message_struct );
		
		}
	//check if the pointer is not pointing to NULL
	//And free the memory	
	for( arg_count = 0; arg_count < LOGIN_ARG_COUNT; arg_count++ ) 
		{
		if ( NULL != message_struct->strs[arg_count] ) 
			{
			free ( message_struct->strs[arg_count] );
			}
		}
	free ( message_struct );
	iso_logger( "%s", "Out - action_parse_login\n" );
	return message_type_err;
	}
	

