/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   tstlmblocks.cpp
*
*/




// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>
#include "tstlm.h"

#include "loudmouth/loudmouth.h"
#include <glib.h>
#include <glib/gprintf.h>           // for g_stpcpy
#include <string.h>
#include <stdlib.h>

#include <glowmem.h>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
#define UNEXIST_PORT 0
#define MYPORT 4321
#define HAVE_OPENSSL
#define FAKEPORT NULL
//You need to use 5223 for ssl+sasl and 5222 for non-ssl+sasl connection

#define GTALK_SSL_PORT 443
#define SSL_INVALID_PORT 999

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES

typedef enum
{
	RC_OK,
	RC_ERROR
} RC_Code_t;

typedef struct
{
	gchar *recipient;
	gchar *message;
} MessageData;

typedef struct
{
	gchar      *proxy_server;
	guint       proxy_port;	
}ProxyData;

typedef struct
{
	gchar       *server;
	gchar       *username;
	gchar       *password;		
	gchar       *resource;
	MessageData *msg_data;	
#ifdef __WINSCW__
    ProxyData   *proxy_data;
#endif
} ConnectData;

typedef struct
{
	ConnectData *connect_data;
	GMainLoop   *main_loop;
	RC_Code_t   rCode;	
} AuthData;

typedef struct
{
    GMainLoop *main_loop;
    RC_Code_t Test_Success;	
    int count;
} HandleData;

typedef struct
{
	ConnectData *connect_data;
	LmMessage* message;
	LmMessageHandler* handler;
	GMainLoop   *main_loop;
	RC_Code_t   rCode;	
} SendStreamData;
// ======== MODULE DATA STRUCTURES =========
typedef enum
	{
	XMPP_PRESENCE_TYPE_ONLINE,
	XMPP_PRESENCE_TYPE_CHAT,
	XMPP_PRESENCE_TYPE_AWAY,
	XMPP_PRESENCE_TYPE_XA,
	XMPP_PRESENCE_TYPE_DND,
	XMPP_PRESENCE_TYPE_INVISIBLE,
	XMPP_PRESENCE_TYPE_UNAVAILABLE
	} JabberPresenceType;

// LOCAL FUNCTION PROTOTYPES

// Function to read data from the cfg file
static RC_Code_t read_data ( ConnectData*& connect_data, 
                             CStifItemParser& aItem );
// Function to free the allocated resources
static void free_data ( ConnectData*& connect_data );

// Callback function used in lm_connection_open
static void connection_open_cb ( LmConnection *connection, 
                                 gboolean     success, 
                                 gpointer     data );

static void wrong_input_in_open_cb ( LmConnection *connection, 
                                 gboolean     success, 
                                 gpointer     data );                                 

//Callback function set in lm_connection_authenticate() call
//from lm-connection_send
static void connection_auth_cb ( LmConnection *connection, 
                                 gboolean     success, 
                                 gpointer     data );

//Callback from lm_login_test method after auth
static void lm_login_cb ( LmConnection * connection, 
                     gboolean  success, 
                     gpointer data );
// Callback function called by lm_connection_open in Lm_AuthenticateL
// Will call authenticate method from inside this callback. 
// It uses the correct input data
static void auth_in_connection_cb( LmConnection *connection, 
                              gboolean     success, 
                              gpointer     *data );
                                                               
// Callback function set in auth_from_open_cb for handling 
//wrong input data during authenticate. Written to handle negative test cases.
static void wrong_input_auth_cb ( LmConnection *connection, 
                                 gboolean     success, 
                                 gpointer     data );
// Callback function set by lm_connection_open call.
//written to handle the negative test cases for authenticate
static void auth_from_open_cb ( LmConnection *connection, 
                              gboolean     success, 
                              gpointer     *data );
// Callback function to handle messages                              
static LmHandlerResult handle_messages ( LmMessageHandler *handler,
                                         LmConnection     *conneciton,
                                         LmMessage        *message,
                                         gpointer         user_data );
static LmHandlerResult handle_100_messages ( LmMessageHandler *handler,
                                         LmConnection     *conneciton,
                                         LmMessage        *message,
                                         gpointer         user_data );
// Callback function called when connection is closed
static void connection_close_cb ( LmConnection       *connection, 
                                  LmDisconnectReason reason,
                                  gpointer           user_data );
                                  
// Callback function called when presence is changed
static void jabber_presence_handler_cb ( LmConnection       *connection, 
                                  LmDisconnectReason reason,
                                  gpointer           user_data );                                  
static void message_response_cb ( LmConnection       *connection, 
                                  LmDisconnectReason reason,
                                  gpointer           user_data );                                    
                                  
// Get the MessageType in Type
static void GetMessageType ( LmMessageType MessageType, gchar *Type );
// Get MessageSubType in SubType
static void GetMessageSubType ( LmMessageSubType MessageSubType, 
                                gchar *SubType );

// Callback function called if something goes wrong during the connection phase
static LmSSLResponse ssl_cb ( LmSSL       *ssl, 
                              LmSSLStatus status, 
                              gpointer    user_data );

#ifdef __WINSCW__
// Set connection to connect through a proxy
static void SetProxy ( LmConnection *connection, ProxyData *connect_data );
#endif

// Set SSL for connection
static void SSLInit ( LmConnection *connection );

// Returns the username from the JID ( username@domainname/resource )
static gchar *get_user_name ( const gchar *jid );


static JabberPresenceType
jabber_presence_type_from_string ( const gchar *string );


//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ==============================

//-----------------------------------------------------------------------------
// function_name	: read_data
// description     	: Reads data from the CFG file
// Returns          : None
//-----------------------------------------------------------------------------
//
static RC_Code_t
read_data ( ConnectData*& connect_data, CStifItemParser& aItem )
    {
	TPtrC               string;
	TBuf8<KMaxFileName> temp;
	MessageData         *msg_data   = NULL;
	
#ifdef __WINSCW__
	ProxyData           *proxy_data = NULL;
	TInt                proxy_port;
#endif

	
	connect_data = g_new0 ( ConnectData, 1 );
	if ( !connect_data )
	    {
		return RC_ERROR;
	    }
	    
	msg_data = g_new0 ( MessageData, 1 );
	if ( !msg_data )
	    {
		free_data ( connect_data );
		return RC_ERROR;
	    }
	
	connect_data->msg_data = msg_data;
	
#ifdef __WINSCW__
    proxy_data = g_new0 ( ProxyData, 1 );
    if ( !proxy_data ) 
    	{
    	free_data ( connect_data );
    	return RC_ERROR;
    	}
	connect_data->proxy_data = proxy_data;
#endif

	// Read server name
	if ( aItem.GetNextString ( string ) == KErrNone )
	    {
		temp.Copy ( string );
		temp.Append ( '\0' );
		connect_data->server = g_new0 ( gchar, temp.Length() );
		if ( !connect_data->server )
		    {			
			free_data ( connect_data );
			return RC_ERROR;
		    }
		g_stpcpy ( connect_data->server, ( gchar * ) temp.Ptr() ); 
	    }
	    
	// Read Username
	if ( aItem.GetNextString ( string ) == KErrNone )
	    {
	    temp.Copy ( string );
	    temp.Append ( '\0' );
	    connect_data->username = g_new0 ( gchar, temp.Length() );
	    if ( !connect_data->username )
	        {	    	
	    	free_data ( connect_data );
	    	return RC_ERROR;
	        }
	    g_stpcpy ( connect_data->username, ( gchar * ) temp.Ptr() );		    
	    }
	
	// Read Password corresponding to Username
	if ( aItem.GetNextString ( string ) == KErrNone )
	    {
	    temp.Copy ( string );
	    temp.Append ( '\0' );
	    connect_data->password = g_new0 ( gchar, temp.Length () );		    
	    if ( !connect_data->password )
	        {	    	
	    	free_data ( connect_data );
	    	return RC_ERROR;
	        }
	    g_stpcpy ( connect_data->password, ( gchar *) temp.Ptr() );
	    }
	
	// Read recipient
	if ( aItem.GetNextString ( string ) == KErrNone )
	    {
	    temp.Copy ( string );
	    temp.Append ( '\0' );
	    msg_data->recipient = g_new0 ( gchar, temp.Length() );
	    if ( !msg_data->recipient )
	        {
	    	free_data ( connect_data );
	    	return RC_ERROR;
	        }
	    g_stpcpy ( msg_data->recipient, ( gchar * ) temp.Ptr() );
	    }
	
	// Read Message
	if ( aItem.GetNextString ( string ) == KErrNone )
	    {
	    temp.Copy ( string );
	    temp.Append ( '\0' );	    
	    msg_data->message = g_new0 ( gchar, temp.Length() );
	    if ( !msg_data->message )
	        {
	    	free_data ( connect_data );
	    	return RC_ERROR;
	        }
	    g_stpcpy ( msg_data->message, ( gchar * ) temp.Ptr() );
	    }
	
	// Read resource identifier
	if ( aItem.GetNextString ( string ) == KErrNone )
	    {
	    temp.Copy ( string );
	    temp.Append ( '\0' );
	    connect_data->resource = g_new0 ( gchar, temp.Length() );
	    if ( !connect_data->resource )
	        {
	    	free_data ( connect_data );
	    	return RC_ERROR;
	        }
	    g_stpcpy ( connect_data->resource, ( gchar * ) temp.Ptr() );		    
	    }
	 
#ifdef __WINSCW__
	// Read proxy server and proxy port
    if ( aItem.GetNextString ( string ) == KErrNone )
    	{
    	temp.Copy ( string );
    	temp.Append ( '\0' );
    	proxy_data->proxy_server = g_new0 ( gchar, temp.Length() );
    	if ( !proxy_data->proxy_server )
    		{
    		free_data ( connect_data );
    		return RC_ERROR;
    		}
    	g_stpcpy ( proxy_data->proxy_server, ( gchar * ) temp.Ptr() );
    	}
    	
    	if ( aItem.GetNextInt ( proxy_port ) == KErrNone )
    		{
    		proxy_data->proxy_port = proxy_port;
    		}    	
#endif 

	return RC_OK;
    }


//-----------------------------------------------------------------------------
// function_name	: free_data
// description     	: Deallocates all the data
// Returns          : None
//-----------------------------------------------------------------------------
//
static void
free_data ( ConnectData*& connect_data )
    {
	if ( connect_data )
	    {
		if ( connect_data->server )
		    {
			g_free ( connect_data->server );
			connect_data->server = NULL;
		    }
		
		if ( connect_data->username )
		    {
			g_free ( connect_data->username );
			connect_data->username = NULL;
		    }
		
		if ( connect_data->password )
		    {
			g_free ( connect_data->password );
			connect_data->password = NULL;
		    }
		
		if ( connect_data->msg_data )
		    {
			if ( connect_data->msg_data->recipient )
			    {
				g_free ( connect_data->msg_data->recipient );
				connect_data->msg_data->recipient = NULL;
			    }
			if ( connect_data->msg_data->message )
			    {
				g_free ( connect_data->msg_data->message );
				connect_data->msg_data->message = NULL;
			    }
			g_free ( connect_data->msg_data );
			connect_data->msg_data = NULL;
		    }		
		
		if ( connect_data->resource )
		    {
			g_free ( connect_data->resource );
			connect_data->resource = NULL;
		    }
		    
	#ifdef __WINSCW__
	    if ( connect_data->proxy_data )
	    	{
	    	if ( connect_data->proxy_data->proxy_server )
	    		{
	    		g_free ( connect_data->proxy_data->proxy_server );
	    		connect_data->proxy_data->proxy_server = NULL;
	    		}
	    	g_free ( connect_data->proxy_data );
	    	connect_data->proxy_data = NULL;	
	    	}
	#endif	     
		g_free ( connect_data );
		connect_data = NULL;    
	    }
    }


//-----------------------------------------------------------------------------
// funnction_name	: connection_open_cb
// description		: callback function called by lm_connection_open
// Returns			: None
//-----------------------------------------------------------------------------
//
static void
connection_open_cb ( LmConnection *connection, 
                     gboolean    success  /*success*/, 
                     gpointer data )
    {
    
    gboolean result = success ;
    GMainLoop *main_loop = ( GMainLoop * ) data;
    
	if ( main_loop )
	    {
		g_main_loop_quit ( main_loop );	
	    }		
    /* send whitespace to the server every 30 seconds */
    lm_connection_set_keep_alive_rate (connection, 30);			
    }


//-----------------------------------------------------------------------------
// funnction_name	: wrong_input_in_open_cb
// description		: callback function called by lm_connection_open when the wrong
//input like wrong server , user id etc is given
// Returns			: None
//-----------------------------------------------------------------------------
//
static void
wrong_input_in_open_cb ( LmConnection */*connection*/, 
                     gboolean    success  /*success*/, 
                     gpointer data )
    {
    
    gboolean result = success;
    AuthData *auth_data = ( AuthData * ) data;
    
     //Result is false when the wrong input parameters were successfully handled.
    if (result == FALSE)
		{       
		auth_data->rCode = RC_OK;
		}
    else 
    	{
    	//Result is true, when the wrong input was not handled successfully.
       	auth_data->rCode = RC_ERROR;	
    	}
    g_main_loop_quit ( auth_data->main_loop ); 		
    }
//-----------------------------------------------------------------------------
// funnction_name	: wrong_input_auth_cb
// description		: callback function called by lm_connection_open
// Returns			: None
//-----------------------------------------------------------------------------
//
static void
wrong_input_auth_cb ( LmConnection */*connection*/, 
                     gboolean  success, 
                     gpointer data )
    {
    gboolean result = success;
    AuthData *auth_data = ( AuthData * ) data;
    //Result is false when the wrong input parameters were successfully handled.
    if (result == FALSE)
		{       
		auth_data->rCode = RC_OK;
		}
    else 
    	{
    	//Result is true, when the wrong input was not handled successfully.
       	auth_data->rCode = RC_ERROR;	
    	}
    g_main_loop_quit ( auth_data->main_loop );       		
    }
    
//-----------------------------------------------------------------------------
// funnction_name	: connection_auth_cb
// description		: callback function set by lm_connection_authenticate
// in lm_connection_sendL()
// Returns			: None
//-----------------------------------------------------------------------------
//
static void connection_auth_cb ( LmConnection */*connection*/, 
                     gboolean  /*success*/, 
                     gpointer data )
    {
    GMainLoop *main_loop = ( GMainLoop * ) data;
    
	if ( main_loop )
	    {
		g_main_loop_quit ( main_loop );	
	    }	   		
    }

//-----------------------------------------------------------------------------
// function_name	: auth_in_connection_cb
// description		: callback function called for lm_connection_open
// Returns			: RC_OK if successfull, RC_ERROR otherwise
//-----------------------------------------------------------------------------
static void
auth_in_connection_cb ( LmConnection *connection, 
                  gboolean success, 
                  gpointer *data )
    {
    AuthData *auth_data = ( AuthData * ) data;
         
    if ( !success )
        {
        // Connection failed
        auth_data->rCode = RC_ERROR;
        g_main_loop_quit ( auth_data->main_loop );    	    
        }
    
    // Extract the username from the JID
    gchar *username = get_user_name ( auth_data->connect_data->username );
    
    if ( !lm_connection_authenticate ( connection, 
                                       username,
                                       auth_data->connect_data->password,
                                       auth_data->connect_data->resource, 
                                       NULL, 
                                       auth_data->connect_data->msg_data, 
                                       NULL, 
                                       NULL ) )
        {
       	auth_data->rCode = RC_ERROR;
       	g_free ( username );
       	g_main_loop_quit ( auth_data->main_loop );    	    
        }
    g_free ( username );
        
    auth_data->rCode = RC_OK;    
    g_main_loop_quit ( auth_data->main_loop );    
    
    }

//-----------------------------------------------------------------------------
// function_name	: auth_from_open_cb
// description		: callback function called for lm_connection_authenticate
// Returns			: RC_OK if successfull, RC_ERROR otherwise
//-----------------------------------------------------------------------------
static void
auth_from_open_cb ( LmConnection *connection, 
                  gboolean success, 
                  gpointer *data )
    {
    AuthData *auth_data = ( AuthData * ) data;
         
    if ( !success )
        {
        // Connection failed
        auth_data->rCode = RC_ERROR;
        g_main_loop_quit ( auth_data->main_loop );    	    
        }
    
    // Extract the username from the JID
    gchar *username = get_user_name ( auth_data->connect_data->username );
    
    if ( !lm_connection_authenticate ( connection, 
                                       username,
                                       auth_data->connect_data->password,
                                       auth_data->connect_data->resource, 
                                       ( LmResultFunction ) wrong_input_auth_cb, 
                                       auth_data ,  
                                       NULL, 
                                       NULL ) )
        {
       	auth_data->rCode = RC_ERROR;
       	g_free ( username );
       	g_main_loop_quit ( auth_data->main_loop );    	    
        }
    g_free ( username );
    
    }


//--------------------------------------------------------------------------------
// function_name    : handle_messages
// description      : callback function to handle messages
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_messages ( LmMessageHandler* /*handler*/,
                  LmConnection*     /*connection*/,
                  LmMessage*        /*message*/,
                  gpointer          user_data )
    {
    HandleData *handle_data = ( HandleData * )user_data;	
	
	if ( handle_data )
		{
		handle_data->Test_Success = RC_OK;
	
	    g_main_loop_quit ( handle_data->main_loop );
		}	
	
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    }

//--------------------------------------------------------------------------------
// function_name    : handle_messages
// description      : callback function to handle messages
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_100_messages ( LmMessageHandler* /*handler*/,
                  LmConnection*     /*connection*/,
                  LmMessage*        /*message*/,
                  gpointer          user_data )
    {
    HandleData *handle_data = ( HandleData * )user_data;    
    //static int count=0;
    handle_data->count++;
    GMainLoop *main_loop = ( GMainLoop * )user_data;
    //count++;
    if(handle_data->count==5)
            {
        g_main_loop_quit ( handle_data->main_loop );
        //    g_main_loop_quit ( main_loop );
            } 
        //} 
    
    return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    }

//--------------------------------------------------------------------------------
// function_name    : handle_messages
// description      : callback function to handle messages
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_100_presence_notification ( LmMessageHandler* /*handler*/,
                  LmConnection*     /*connection*/,
                  LmMessage*        /*message*/,
                  gpointer          user_data )
    {
    HandleData *handle_data = ( HandleData * )user_data;    
    handle_data->count++;
    GMainLoop *main_loop = ( GMainLoop * )user_data;
    if(handle_data->count==100)
            {
        g_main_loop_quit ( handle_data->main_loop );
        
            } 
      
    
    return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    }

//--------------------------------------------------------------------------------
// function_name	: connection_close_cb
// description		: callback function called when connection is closed
// Returns			: RC_OK if called
//--------------------------------------------------------------------------------
static void
connection_close_cb ( LmConnection       */*connection*/,
                      LmDisconnectReason reason,
                      gpointer           user_data )
    {   
    HandleData *handle_data = ( HandleData * ) user_data;
    
    switch ( reason )
        {
        case LM_DISCONNECT_REASON_OK:
    	    handle_data->Test_Success = RC_OK;
    	    break;
        case LM_DISCONNECT_REASON_PING_TIME_OUT:
        /* No break, fall through */
        case LM_DISCONNECT_REASON_HUP:
        case LM_DISCONNECT_REASON_ERROR:
        case LM_DISCONNECT_REASON_UNKNOWN:
            handle_data->Test_Success = RC_ERROR;
            break;
        default:
            handle_data->Test_Success = RC_ERROR;
            break;
        }
	
    }


//------------------------------------------------------------------------------
// function_name	: GetMessageType
// description     	: Gets LmMessageType in Type
// Returns          : None
//------------------------------------------------------------------------------
static void
GetMessageType ( LmMessageType MessageType, gchar *Type )
{
	switch ( MessageType )
	    {
		case LM_MESSAGE_TYPE_MESSAGE:		
		    g_stpcpy ( Type, "LM_MESSAGE_TYPE_MESSAGE" );
		    break;
	    case LM_MESSAGE_TYPE_PRESENCE:	        
	        g_stpcpy ( Type, "LM_MESSAGE_TYPE_MESSAGE" );
	        break;	        	    
	    case LM_MESSAGE_TYPE_IQ:	        
	        g_stpcpy ( Type, "LM_MESSAGE_TYPE_IQ" );
	        break;
	    case LM_MESSAGE_TYPE_STREAM:	        
	        g_stpcpy ( Type, "LM_MESSAGE_TYPE_STREAM" );
	        break;
	    case LM_MESSAGE_TYPE_STREAM_ERROR:	        
	        g_stpcpy ( Type, "LM_MESSAGE_TYPE_STREAM_ERROR" );	        
	        break;
	    case LM_MESSAGE_TYPE_UNKNOWN:	        
	        g_stpcpy ( Type, "LM_MESSAGE_TYPE_UNKNOWN" );
	        break;	        
	    }
}


//------------------------------------------------------------------------------
// function_name	: GetMessageSubType
// description     	: Gets LmMessageSubType in SubType
// Returns          : None
//------------------------------------------------------------------------------
static void
GetMessageSubType ( LmMessageSubType MessageSubType, gchar *SubType )
{
	switch ( MessageSubType )
	    {
		case LM_MESSAGE_SUB_TYPE_NOT_SET:
		    g_stpcpy ( SubType, "LM_MESSAGE_SUB_TYPE_NOT_SET" );
		    break;
		case LM_MESSAGE_SUB_TYPE_AVAILABLE:
		    g_stpcpy ( SubType, "LM_MESSAGE_SUB_TYPE_AVAILABLE" );
		    break;
		case LM_MESSAGE_SUB_TYPE_NORMAL:
		    g_stpcpy ( SubType, "LM_MESSAGE_SUB_TYPE_NORMAL" );
		    break;
		case LM_MESSAGE_SUB_TYPE_CHAT:
		    g_stpcpy ( SubType, "LM_MESSAGE_SUB_TYPE_CHAT" );
		    break;
		case LM_MESSAGE_SUB_TYPE_GROUPCHAT:
		    g_stpcpy ( SubType, "LM_MESSAGE_SUB_TYPE_GROUPCHAT" );
		    break;
		case LM_MESSAGE_SUB_TYPE_HEADLINE:
		    g_stpcpy ( SubType, "LM_MESSAGE_SUB_TYPE_HEADLINE" );
		    break;
		case LM_MESSAGE_SUB_TYPE_UNAVAILABLE:
		    g_stpcpy ( SubType, "LM_MESSAGE_SUB_TYPE_UNAVAILABLE" );
		    break;
		case LM_MESSAGE_SUB_TYPE_PROBE:
		    g_stpcpy ( SubType, "LM_MESSAGE_SUB_TYPE_PROBE" );
		    break;
		case LM_MESSAGE_SUB_TYPE_SUBSCRIBE:
		    g_stpcpy ( SubType, "LM_MESSAGE_SUB_TYPE_SUBSCRIBE" );
		    break;
		case LM_MESSAGE_SUB_TYPE_UNSUBSCRIBE:
		    g_stpcpy ( SubType, "LM_MESSAGE_SUB_TYPE_UNSUBSCRIBE" );
		    break;
		case LM_MESSAGE_SUB_TYPE_SUBSCRIBED:
		    g_stpcpy ( SubType, "LM_MESSAGE_SUB_TYPE_SUBSCRIBED" );
		    break;
		case LM_MESSAGE_SUB_TYPE_UNSUBSCRIBED:
		    g_stpcpy ( SubType, "LM_MESSAGE_SUB_TYPE_UNSUBSCRIBED" );
		    break;
		case LM_MESSAGE_SUB_TYPE_GET:
		    g_stpcpy ( SubType, "LM_MESSAGE_SUB_TYPE_GET" );
		    break;
		case LM_MESSAGE_SUB_TYPE_SET:
		    g_stpcpy ( SubType, "LM_MESSAGE_SUB_TYPE_SET" );
		    break;
		case LM_MESSAGE_SUB_TYPE_RESULT:
		    g_stpcpy ( SubType, "LM_MESSAGE_SUB_TYPE_RESULT" );
		    break;
		case LM_MESSAGE_SUB_TYPE_ERROR:
		    g_stpcpy ( SubType, "LM_MESSAGE_SUB_TYPE_ERROR" );
		    break;
	    }
}


// ---------------------------------------------------------------------------
// jabber_presence_type_from_string: Returns the presence type
// ---------------------------------------------------------------------------
//	
static JabberPresenceType
jabber_presence_type_from_string ( const gchar *string )
	{
	
	if ( string == NULL )
		return XMPP_PRESENCE_TYPE_ONLINE;
	
	if ( !strcmp ( string, "online" ) )
		return XMPP_PRESENCE_TYPE_ONLINE;	
	else if ( !strcmp ( string, "chat" ) )
		return XMPP_PRESENCE_TYPE_CHAT;
	else if ( !strcmp ( string, "away" ) )
		return XMPP_PRESENCE_TYPE_AWAY;
	else if ( !strcmp ( string, "xa" ) )
		return XMPP_PRESENCE_TYPE_XA;
	else if ( !strcmp ( string, "dnd" ) )
		return XMPP_PRESENCE_TYPE_DND;
	else if ( !strcmp ( string, "invisible" ) )
		return XMPP_PRESENCE_TYPE_INVISIBLE;
	else if ( !strcmp ( string, "unavailable" ) )
		return XMPP_PRESENCE_TYPE_UNAVAILABLE;	
	else
		return XMPP_PRESENCE_TYPE_ONLINE;
	}


// ---------------------------------------------------------------------------
// function_name       : ssl_cb
// description         : This function is called if something goes wrong
//                       during the connecting phase.
// Arguements          :
//        ssl          : An LmSSL
//        status       : The status informing what went wrong
//        user_data    : User data provided in the callback
//
// Returns             : User should return LM_SSL_RESPONSE_CONTINUE if 
//                       connection should proceed. otherwise 
//                       LM_SSL_RESPONSE_STOP.
// ---------------------------------------------------------------------------
//
static LmSSLResponse
ssl_cb ( LmSSL */*ssl*/, LmSSLStatus status, gpointer /*user_data*/ )
	{
	
	g_print ( "SSL status :%d\n", status );
	
	switch ( status ) 
	    {
	case LM_SSL_STATUS_NO_CERT_FOUND:
       	g_printerr ("No certificate found!\n");
       	break;
     case LM_SSL_STATUS_UNTRUSTED_CERT:
       	g_printerr ("Certificate is not trusted!\n");
       	break;
 	case LM_SSL_STATUS_CERT_EXPIRED:
       	g_printerr ("Certificate has expired!\n");
       	break;
	case LM_SSL_STATUS_CERT_NOT_ACTIVATED:
       	g_printerr ("Certificate has not been activated!\n");
       	break;
	case LM_SSL_STATUS_CERT_HOSTNAME_MISMATCH:
       	g_printerr ("Certificate hostname does not match expected hostname!\n");
       	break;
	 case LM_SSL_STATUS_CERT_FINGERPRINT_MISMATCH:
	 	g_printerr ( "fingerprint error!\n" );
	 	break;
	 case LM_SSL_STATUS_GENERIC_ERROR:
        g_printerr ("Generic SSL error!\n");
        break;
	    }
		
	return LM_SSL_RESPONSE_CONTINUE;
	}


// ---------------------------------------------------------------------------
// function_name       : SetProxy
// description         : Sets the connection to use proxy
// Arguements          :
//        connection   : LmConnection
//        connect_data : ConnectData
// Returns             : None
// ---------------------------------------------------------------------------
//
#ifdef __WINSCW__
static void 
SetProxy ( LmConnection *connection, ProxyData *proxy_data )
	{
	LmProxy *proxy = NULL;
	
	proxy = lm_proxy_new ( LM_PROXY_TYPE_HTTP );
	lm_proxy_set_server ( proxy, proxy_data->proxy_server );
	lm_proxy_set_port ( proxy, proxy_data->proxy_port );
	lm_connection_set_proxy ( connection, proxy );
	lm_proxy_unref ( proxy );
	}
#endif

// ---------------------------------------------------------------------------
// function_name       : SSLInit
// description         : Sets the connection to use SSL
// Arguements          :
//        connection   : LmConnection
// Returns             : None
// ---------------------------------------------------------------------------
//
static void 
SSLInit ( LmConnection *connection )
	{
	LmSSL *ssl = NULL;
	
	ssl = lm_ssl_new ( NULL, ( LmSSLFunction ) ssl_cb, NULL, NULL );
	lm_connection_set_ssl ( connection, ssl );
		lm_ssl_unref ( ssl );
	lm_connection_set_keep_alive_rate (connection, 30);
	}


// ---------------------------------------------------------------------------
// function_name       : get_user_name 
// description         : Returns the username from the jid
// Arguements          :
//        jid          : jid
// Returns             : username
// ---------------------------------------------------------------------------
//
static gchar *
get_user_name ( const gchar *jid )
	{
	const gchar *ch;
	
	g_return_val_if_fail ( jid != NULL, NULL );
	
	ch = strchr ( jid, '@' );
	if ( !ch )
		return ( gchar *) jid;
	
	return g_strndup ( jid, ch - jid );
	}

// ---------------------------------------------------------------------------
// jabber_presence_handler_cb: Callback function to handle the presence info
// ---------------------------------------------------------------------------
//
static LmHandlerResult
jabber_presence_handler_cb ( LmMessageHandler * /*handler*/,
                              LmConnection     * /*connection*/,
                              LmMessage        *message,
                              gpointer          user_data )
	{	
	const gchar        *jid;
	const gchar        *show;
	const gchar        *status;		
	
	LmMessageNode      *node;
	JabberPresenceType type;
	
	if(message != NULL)
		{
		// Get the JID attribute
		jid = lm_message_node_get_attribute ( message->node, "from" );

		// Get the show attribute
		
		node = lm_message_node_get_child ( message->node, "show" );
		show = node ? ( node->value ) : "online";
		
		// Get the status attribute
		node   = lm_message_node_get_child ( message->node, "status" );
		status = node ? ( node->value ) : NULL;
		
		type = jabber_presence_type_from_string ( show );			
		}
	
		
	GMainLoop *main_loop = ( GMainLoop * ) user_data;  
	if ( main_loop )
	    {
	    g_main_loop_quit ( main_loop );	
	    }  		
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
	}





// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Ctstlm::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Ctstlm::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Ctstlm::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Ctstlm::RunMethodL( 
    CStifItemParser& aItem ) 
    {
	iLog->Log ( _L ( "In RunMethodL method" ) );
	

    //TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
    //TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.    
        
        // LmConnection       
        ENTRY( "lm_connection_new", Ctstlm::lm_connection_newL ),
        ENTRY( "lm_new_with_context", Ctstlm::lm_new_with_contextL ),
        ENTRY( "lm_connection_open", Ctstlm::lm_connection_openL ),
        ENTRY( "lm_connection_is_open_WhenConnectionIsClosed", Ctstlm::lm_connection_is_open_WhenConnectionIsClosedL), 
        ENTRY( "lm_connection_is_authenticated_WhenNot_Authenticated", Ctstlm::lm_is_authenticated_WhenNotAuthenticatedL), 
        ENTRY( "lm_connection_set_server_withNULL", Ctstlm::lm_connection_set_server_WithNULL),   
        ENTRY( "lm_connection_set_jid_WithNull", Ctstlm::lm_connection_set_jid_WithNULL),   
        ENTRY( "lm_connection_set_port_WithNULL", Ctstlm::lm_connection_set_port_WithNULL),   
        ENTRY( "lm_connection_set_ssl_WithNULL", Ctstlm::lm_connection_set_ssl_WithNULL),   
        ENTRY( "lm_connection_close", Ctstlm::lm_connection_closeL ),
        ENTRY( "lm_connection_closeWhenNotOpen", Ctstlm::lm_connection_closeWhenNotOpenedL ),
        ENTRY( "lm_authenticate", Ctstlm::lm_authenticateL ),
        ENTRY( "lm_connection_is_open", Ctstlm::lm_connection_is_openL ),
        ENTRY( "lm_is_authenticated", Ctstlm::lm_is_authenticatedL ),
        ENTRY( "lm_connection_set_get_server", Ctstlm::lm_connection_set_get_serverL ),
        ENTRY( "lm_connection_set_get_jid", Ctstlm::lm_connection_set_get_jidL ), 
        ENTRY( "lm_connection_set_get_port", Ctstlm::lm_connection_set_get_portL ),
        ENTRY( "lm_connection_set_get_ssl", Ctstlm::lm_connection_set_get_sslL ),
        ENTRY( "lm_connection_set_get_proxy", Ctstlm::lm_connection_set_get_proxyL ),
        
        ENTRY( "lm_connection_send", Ctstlm::lm_connection_sendL ),
        ENTRY( "lm_connection_send400char", Ctstlm::lm_connection_send400charL ),
        ENTRY( "lm_connection_send_repeated", Ctstlm::lm_connection_send_repeatedL ),       
        
        ENTRY( "lm_send_with_reply", Ctstlm::lm_send_with_replyL ),
        ENTRY( "register_message_handler", Ctstlm::register_message_handlerL ),
        ENTRY( "set_disconnect_function", Ctstlm::set_disconnect_functionL ),
        ENTRY( "lm_connection_send_raw",  Ctstlm::lm_connection_send_rawL ),
        ENTRY( "lm_connection_get_state", Ctstlm::lm_connection_get_stateL ),
        ENTRY( "lm_connection_ref",  Ctstlm::lm_connection_refL ),
        ENTRY( "lm_connection_unref", Ctstlm::lm_connection_unrefL ),
        
        // LmMessage
        ENTRY( "lm_message_new", Ctstlm::lm_message_newL ),
        ENTRY( "lm_message_new_with_sub_type", Ctstlm::lm_message_new_with_sub_typeL ),
        ENTRY( "lm_message_get_type", Ctstlm::lm_message_get_typeL ),
        ENTRY( "lm_message_get_sub_type", Ctstlm::lm_message_get_sub_typeL ),
        ENTRY( "lm_message_get_node", Ctstlm::lm_message_get_nodeL ),
        ENTRY( "lm_message_ref", Ctstlm::lm_message_refL ),
        ENTRY( "lm_message_unref", Ctstlm::lm_message_unrefL ),
        
        // LmMessageHandler
        ENTRY( "lm_message_handler_new", Ctstlm::lm_message_handler_newL ),
        ENTRY( "lm_message_handler_invalidate", Ctstlm::lm_message_handler_invalidateL ),
        ENTRY( "lm_message_handler_is_valid", Ctstlm::lm_message_handler_is_validL ),
        ENTRY( "lm_message_handler_ref", Ctstlm::lm_message_handler_refL ),
        ENTRY( "lm_message_handler_unref", Ctstlm::lm_message_handler_unrefL ),
        
        // LmMessageNode
        ENTRY( "lm_message_node_set_get_value", Ctstlm::lm_message_node_set_get_valueL ),
        ENTRY( "lm_message_node_add_child", Ctstlm::lm_message_node_add_childL ),
        ENTRY( "lm_message_node_get_child", Ctstlm::lm_message_node_get_childL ),
        ENTRY( "lm_message_node_find_child", Ctstlm::lm_message_node_find_childL ),  
        
        ENTRY( "lm_message_node_set_get_attributes", Ctstlm::lm_message_node_set_get_attributesL ),
              
        ENTRY( "lm_message_node_set_get_raw_mode", Ctstlm::lm_message_node_set_get_raw_modeL ),
        
        ENTRY( "lm_message_node_ref", Ctstlm::lm_message_node_refL ),
        ENTRY( "lm_message_node_unref", Ctstlm::lm_message_node_unrefL ),
        
        ENTRY( "lm_message_node_to_string", Ctstlm::lm_message_node_to_stringL ),
        
        // LmSSL
        ENTRY( "lm_ssl_new", Ctstlm::lm_ssl_newL ),
        ENTRY( "lm_ssl_is_supported", Ctstlm::lm_ssl_is_supportedL ),
        ENTRY( "lm_ssl_get_fingerprint", Ctstlm::lm_ssl_get_fingerprintL ),
        ENTRY( "lm_ssl_ref", Ctstlm::lm_ssl_refL ),
        ENTRY( "lm_ssl_unref", Ctstlm::lm_ssl_unrefL ),        
        
        // LmProxy        
        ENTRY( "lm_proxy_new", Ctstlm::lm_proxy_newL ),
        ENTRY( "lm_proxy_new_with_server", Ctstlm::lm_proxy_new_with_serverL ),        
        ENTRY( "lm_proxy_set_get_type", Ctstlm::lm_proxy_set_get_typeL ),        
        ENTRY( "lm_proxy_set_get_server", Ctstlm::lm_proxy_set_get_serverL ),        
        ENTRY( "lm_proxy_set_get_port", Ctstlm::lm_proxy_set_get_portL ),        
        ENTRY( "lm_proxy_set_get_username", Ctstlm::lm_proxy_set_get_usernameL ),        
        ENTRY( "lm_proxy_set_get_password", Ctstlm::lm_proxy_set_get_passwordL ),        
        ENTRY( "lm_proxy_ref", Ctstlm::lm_proxy_refL ),
        ENTRY( "lm_proxy_unref", Ctstlm::lm_proxy_unrefL ),
        
        // lm-send-sync example        
        ENTRY( "lm_send_sync", Ctstlm::lm_send_syncL ),
        
        // lm contact list fetching code
        
        ENTRY( "lm_fetching_contactlist", Ctstlm::lm_fetching_contactlist_L ),
        ENTRY( "lm_fetching_presence", Ctstlm::lm_fetching_presenceL ),
        ENTRY( "lm_subscribe_contact", Ctstlm::lm_subscribe_contact_L ),
        ENTRY( "lm_authenticate_unexistjid", Ctstlm::lm_authenticate_WithUnexistingJID ),
        ENTRY( "lm_authenticate_badpassword", Ctstlm::lm_authenticate_WithBadPasswordL ),
        ENTRY( "lm_authenticate_connectionisnotcreated", Ctstlm::lm_authenticate_WhenConnectionIsNotOpenedL ),
        ENTRY( "lm_connection_open_with_bad_server", Ctstlm::lm_connection_open_with_badserverL ),
        ENTRY( "lm_connection_send_receive", Ctstlm::lm_connection_send_receiveL ),
        ENTRY( "lm_login_test", Ctstlm::lm_login_testL ),
        ENTRY( "lm_add_contact", Ctstlm::lm_add_contactL ),
        ENTRY( "lm_remove_contact", Ctstlm::lm_remove_contactL ),
        ENTRY( "lm_connection_cancel_open", Ctstlm::lm_connection_cancel_openL ),
        ENTRY( "lm_connection_get_privacy_lists", Ctstlm::lm_connection_get_privacy_listsL ),
        ENTRY( "lm_connection_set_privacy_lists", Ctstlm::lm_connection_set_privacy_listsL ),
        ENTRY( "lm_connection_send_image_bytestream_msg", Ctstlm::lm_connection_send_image_bytestream_msgL ),
        ENTRY( "lm_connection_send_image_bytestream_iq", Ctstlm::lm_connection_send_image_bytestream_iqL ),
        ENTRY( "lm_connection_send_multiple_conn", Ctstlm::lm_connection_send_multiple_connL ),
        ENTRY( "lm_connection_block_contact", Ctstlm::lm_connection_block_contactL ),
        ENTRY( "lm_connection_unblock_contact", Ctstlm::lm_connection_unblock_contactL ),
        ENTRY( "lm_connection_set_active_list", Ctstlm::lm_connection_set_active_listL ),
        ENTRY( "lm_connection_set_unexist_active_list", Ctstlm::lm_connection_set_unexist_active_listL ),
        ENTRY( "lm_connection_set_decline_active_list", Ctstlm::lm_connection_set_decline_active_listL ),
        ENTRY( "lm_connection_get_one_privacy_list", Ctstlm::lm_connection_get_one_privacy_listL ),
        ENTRY( "lm_connection_set_one_privacy_list", Ctstlm::lm_connection_set_one_privacy_listL ),
        ENTRY( "lm_connection_get_many_privacy_list_from_listname", Ctstlm::lm_connection_get_many_privacy_list_from_listnameL ),
        ENTRY( "lm_connection_set_one_unexist_privacy_list", Ctstlm::lm_connection_set_one_unexist_privacy_listL ),
        ENTRY( "lm_connection_get_one_unexist_privacy_list", Ctstlm::lm_connection_get_one_unexist_privacy_listL ),
        ENTRY( "lm_connection_create_one_privacy_list", Ctstlm::lm_connection_create_one_privacy_listL ),
        ENTRY( "lm_service_discovery", Ctstlm::lm_service_discoveryL ),
        ENTRY( "lm_service_discovery_connected_resource", Ctstlm::lm_service_discovery_connected_resourceL ),
        ENTRY( "lm_simple_communications_blocking", Ctstlm::lm_simple_communications_blockingL ),
        ENTRY( "lm_send_im_with_reply", Ctstlm::lm_send_im_with_replyL ),
        ENTRY( "lm_receive_any_message", Ctstlm::lm_receive_any_messageL ),
        ENTRY( "lm_login_multiple_sessions", Ctstlm::lm_login_multiple_sessionsL ),
        ENTRY( "lm_change_own_presence", Ctstlm::lm_change_own_presenceL ),
        ENTRY( "lm_login_test_invalid_server", Ctstlm::lm_login_test_invalid_serverL ),
        ENTRY( "lm_login_test_invalid_port", Ctstlm::lm_login_test_invalid_portL ),
        ENTRY( "lm_memory_leak_test", Ctstlm::lm_memory_leak_testL ),
        ENTRY( "lm_nft_updating_presence", Ctstlm::lm_nft_updating_presenceL ),
        ENTRY( "lm_nft_presence_notification", Ctstlm::lm_nft_presence_notificationL ),
        ENTRY( "lm_nft_send_text_message", Ctstlm::lm_nft_send_text_messageL ),
        ENTRY( "lm_nft_open_conversation_with_multiple",Ctstlm::lm_nft_open_conversation_with_multipleL ),
        ENTRY( "lm_nft_fetch300_contact", Ctstlm::lm_nft_fetch300_contactL ),
        ENTRY( "lm_nft_loginlogoutmultipletime", Ctstlm::lm_nft_loginlogoutmultipletime ),
        ENTRY( "lm_nft_receiving100_message_L", Ctstlm::lm_nft_receiving100_message_L ),
        
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


//----------------------------------------------------------------------------
// Ctstlm::lm_connection_newL
// Description	: Creates a new closed connection. Server can be NULL but must
//				  be set before calling lm_connection_open()
// Arguements	:
//		 server	: The hostname of the server for the connection
// Retrurns		: A newly created LmConnection, should be unreffed with 
//				  lm_connection_unref()
//----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_newL ( CStifItemParser& aItem )
	{
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;	
  	
  	SET_LOW_MEMORY_TRAP ( NULL );
  	
  	iLog->Log ( _L("In lm_connection_newL" ) );
  	
  	
  	
__UHEAP_MARK;
__UHEAP_RESET;

  	// Read data from the cfg file
  	if ( read_data ( connect_data, aItem ) != RC_OK )
  	    {
  		iLog->Log ( _L ( "read_data failed" ) );
  		free_data ( connect_data );
  		return KErrGeneral;
  	    }
  	
__UHEAP_FAILNEXT(1);

  	//-----------------------
  	connection = lm_connection_new ( connect_data->server );  	
  	//-----------------------
  	
  	// Free the allocated resources 
  	free_data ( connect_data );
  	
	if ( connection == NULL ) 
	    {
		iLog->Log ( _L( "lm_connection_new failed to allocate memory" ) );		
		return KErrGeneral;	
	    }
	
	lm_connection_unref ( connection );
__UHEAP_MARKEND;
	
	REMOVE_LOW_MEMORY_TRAP ();
	
	iLog->Log ( _L( "lm_connection_new test case passed" ) );
	
	return KErrNone;			
	}
	
	
//----------------------------------------------------------------------------
// Ctstlm::lm_connection_new_with_context
// Description	: Creates a new closed connection running in a certain context
// Arguements	:
//		server	: The hostname of the server for the connection
//		context	: The context this connection should be running on
// Retrurns		: A newly created Lmconnection, should be unreffed with
//				  lm_connection_unref()
//----------------------------------------------------------------------------
//
TInt Ctstlm::lm_new_with_contextL ( CStifItemParser& /* aItem */ )
    {
	GMainContext *context;
	LmConnection *connection = NULL;
	
	iLog->Log ( _L( "In lm_connection_new_with_contextL" ) );
	
	context = g_main_context_new ();
	connection = lm_connection_new_with_context ( NULL, context );
	if ( connection == NULL ) 
		{
		iLog->Log ( _L ( "lm_connection_new_with_context failed" ) );
		return KErrGeneral;
		}
		
	lm_connection_unref ( connection );
	
	iLog->Log ( _L ( "lm_connection_new_with_context test case passed" ) );
	
	return KErrNone;
    }
	

//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_open
// Description	: An async call to open connection. When the connection is open
//                function 'connection_open_cb' is called
// Arguements	:
//	connection	: LmConnection to open
//	function	: Callback function that will be called when the connection 
//         		  is open
//	user_data	: User data that will be passed to function
//	notify		: Function for freeing that user_data, can be NULL
//	error		: location to store error
// Returns		: TRUE if everything went fine, FALSE otherwise
//
// Requieres	: This test case requires GUI for selecting Access point
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_openL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	
	GMainLoop    *main_loop    = NULL;	
		
	iLog->Log ( _L ( "In lm_connection_open" ) );
	
	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Open a new closed connection
	//context = g_main_context_new ();
	//connection = lm_connection_new_with_context ( connect_data->server, context );
	
	connection = lm_connection_new ( connect_data->server );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
		
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Set proxy for emulator
#ifdef __WINSCW__
    SetProxy ( connection, connect_data->proxy_data );
#endif

    // Set connection to use SSL
    SSLInit ( connection );
	
	main_loop = g_main_loop_new ( NULL, FALSE );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	
	lm_connection_close ( connection, NULL );
	lm_connection_unref ( connection );
	free_data ( connect_data );
	
	iLog->Log ( _L ( "lm_connection_open succeded" ) );
	return  KErrNone;	

    }


//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_close
// Description	 : A synchronous call to close the connection
// Arguements    :
//	  connection : LmConnection to close
//			error		: location to store error, or NULL
// Retrurns      :TRUE if no errors were detected, otherwise FALSE
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_closeL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	
	iLog->Log ( _L ( "In lm_connection_close" ) );
	
	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Open a new closed connection	
	connection = lm_connection_new ( connect_data->server );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	lm_connection_set_jid ( connection, connect_data->username );
	// Set proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set connection to use SSL
	SSLInit ( connection );
	GMainLoop *main_loop = g_main_loop_new ( NULL, FALSE );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );		
		return KErrGeneral;
	    }
		
	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	
		
	if ( lm_connection_close ( connection, NULL ) == FALSE )
	    {
		iLog->Log ( _L ( "lm_connection_close failed" ) );
		lm_connection_unref ( connection );
		return KErrGeneral;
	    }
	
	lm_connection_unref ( connection );
	free_data ( connect_data );
	iLog->Log ( _L ( "lm_connection_close passed" ) );
	
	return KErrNone;
    }


//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_closeWhenNotOpenedL
// Description	 : A synchronous call to close the connection
// Arguements    :
//	  connection : LmConnection to close
//			error		: location to store error, or NULL
// Retrurns      :TRUE if no errors were detected, otherwise FALSE
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_closeWhenNotOpenedL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	
	iLog->Log ( _L ( "In lm_connection_close" ) );
	
	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Open a new closed connection	
	connection = lm_connection_new ( connect_data->server );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
		
	if ( lm_connection_close ( connection, NULL ) == TRUE )
	    {
		iLog->Log ( _L ( "lm_connection_close failed" ) );
		lm_connection_unref ( connection );
		return KErrGeneral;
	    }
	
	lm_connection_unref ( connection );
	free_data ( connect_data );
	iLog->Log ( _L ( "lm_connection_closeWhenNotOpenedL passed" ) );
	
	return KErrNone;
    }





//-----------------------------------------------------------------------------
// Ctstlm::lm_authenticateL
// Description	  : Tries to authenticate a user against the server
// Arguements	  :
//	 connnection  : LmConnection to authenticate
//	 username     : Username used to authenticate
//	 password     : Password corresponding to username
//	 resource     : Resource used for this connection
//	 function     : Callback function called when authentication is finished
//   user_data    : Userdata passed to callback function when called
//   notify       : Function for freeing user_data, can be NULL
//   error        : location to store error, or NULL
// Returns        : TRUE if no errors were detected, FALSE otherwise
// Requieres      : This test case requires GUI for selecting Access point
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_authenticateL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;	
	AuthData     *auth_data    = NULL;
		
	iLog->Log ( _L ( "In lm_authenticateL" ) );	

	auth_data = g_new0 ( AuthData, 1 );
	if ( !auth_data )
		{
		iLog->Log ( _L ( "memory allocation failed for auth_data" ) );
		return KErrGeneral;
		}	
		
	if ( read_data ( auth_data->connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );		
		free_data ( auth_data->connect_data );		
		g_free ( auth_data );
		return KErrGeneral;
		}
	
	auth_data->main_loop    = g_main_loop_new ( NULL, FALSE );	
	auth_data->rCode        = RC_ERROR;
	connection = lm_connection_new ( auth_data->connect_data->server );
	if ( connection == NULL )
	    {
	    iLog->Log ( _L ( "lm_connection_new failed" ) );
	    free_data ( auth_data->connect_data );
	    g_main_loop_unref ( auth_data->main_loop );
	    g_free ( auth_data );
	    return KErrGeneral;
	    }
	
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	lm_connection_set_jid ( connection, auth_data->connect_data->username );

#ifdef __WINSCW__
	SetProxy ( connection, auth_data->connect_data->proxy_data );
#endif
	SSLInit ( connection );
	
	if ( !lm_connection_open ( connection, 
	                           ( LmResultFunction ) auth_in_connection_cb,
	                           auth_data, 
	                           NULL, 
	                           NULL ) )                          
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ) );
		free_data ( auth_data->connect_data );
		g_main_loop_unref ( auth_data->main_loop );
		g_free ( auth_data );
		return KErrGeneral;
	    }	   
	                                    
	
	g_main_loop_run ( auth_data->main_loop );
	g_main_loop_unref ( auth_data->main_loop );
	
	lm_connection_close ( connection, NULL );
	lm_connection_unref ( connection );
	free_data ( auth_data->connect_data );	
	
	if ( auth_data->rCode != RC_OK )
	    {
	    iLog->Log ( _L ( "lm_authenticateL failed" ) );
	    g_free ( auth_data );
	    return KErrGeneral;
	    }
	
	g_free ( auth_data );

	iLog->Log ( _L ( "lm_authenticateL passed" ) );
	
	return KErrNone;		
    }

//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_is_open
// Description  : Check if connection is currently open or not
// Arguements   :
//   connection	: an LmConnection to check if it is open
// Returns      : TRUE if connection is open and FALSE if it is closed
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_is_openL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	gboolean NotOpen           = FALSE;	
	
	iLog->Log ( _L ( "In lm_connection_is_open" ) );
	
	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
		
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	lm_connection_set_jid ( connection, connect_data->username );
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	SSLInit ( connection );
	
	// Check for connection in case of connection not yet opened
	if ( !lm_connection_is_open ( connection ) )
	    {
		iLog->Log ( _L ( "Connection is not open" ) );
		NotOpen = TRUE;
	    }	
	
	// Check for open connection in case of connection opened	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop ); 
	
	if ( !lm_connection_is_open ( connection ) && FALSE == NotOpen )
	    {
		iLog->Log ( _L ( "lm_connection_is_open failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
	    }	
	
	lm_connection_close ( connection, NULL );
	lm_connection_unref ( connection );
	free_data ( connect_data );	
	
	iLog->Log ( _L ( "lm_connection_is_open passed" ) );
	
	return KErrNone;
	}
/////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_is_open_WhenConnectionIsClosedL
// Description  : Check if connection is currently open or not
// Arguements   :
//   connection	: an LmConnection to check if it is open
// Returns      : TRUE if connection is open and FALSE if it is closed
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_is_open_WhenConnectionIsClosedL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	gboolean NotOpen           = FALSE;	
	
	iLog->Log ( _L ( "lm_connection_is_open_WhenConnectionIsClosedL" ) );
	
	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
		
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	if ( !connection )
		{
		iLog->Log ( _L ( " lm_connection_is_open" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
		
	if ( lm_connection_is_open ( connection )  )
	    {
		iLog->Log ( _L ( "lm_connection_is_open_WhenConnectionIsClosedL failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
	    }	
	
	lm_connection_close ( connection, NULL );
	lm_connection_unref ( connection );
	free_data ( connect_data );	
	
	iLog->Log ( _L ( "lm_connection_is_open_WhenConnectionIsClosedL passed" ) );
	
	return KErrNone;
	}



//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_is_authenticated
// Description  : Check if connection is authenticated or not
// Arguements   :
//   connection	: an LmConnection to check 
// Returns      : TRUE if connection is authenticated, FALSE otherwise
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_is_authenticatedL ( CStifItemParser& aItem )
    {
    LmConnection *connection       = NULL;
    ConnectData  *connect_data     = NULL;
    gboolean     NotAuthenticated  = FALSE;    
    
    iLog->Log ( _L ( "In lm_connection_is_authenticated" ) );
    
    // Read data from the CFG file
    if ( read_data ( connect_data, aItem ) != RC_OK )
    	{
    	iLog->Log ( _L ( "read_data failed" ) );
    	free_data ( connect_data );
    	return KErrGeneral;
    	}    
    
    // Open a new closed connection
    connection = lm_connection_new ( connect_data->server );
    if ( !connection )
    	{
    	iLog->Log ( _L ( "lm_connection_new failed" ) );
    	free_data ( connect_data );
    	return KErrGeneral;
    	}
    
    // Set the connection to use gtalk's SSL port
    lm_connection_set_port ( connection, GTALK_SSL_PORT );
    
    // Set the JID to be used for connection 
    lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif

	// Set the connection to use SSL
	SSLInit ( connection );
	
    // Open a connection
    GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop ); 
	
    
    // Check for authentication before the connection is authenticated
    if ( !lm_connection_is_authenticated ( connection ) )
        {
    	iLog->Log ( _L ( "Connection not yet authenticated" ) );
    	NotAuthenticated = TRUE;
        }
    
    // Authenticate the connection
    main_loop = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }

	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
    g_free ( username );
    
    // Check for authentication after the connection is authenticated
    if ( !lm_connection_is_authenticated ( connection ) && 
          FALSE == NotAuthenticated )
        {
    	iLog->Log ( _L ( "lm_connection_is_authenticated failed" ) );    	
    	lm_connection_close ( connection, NULL );
    	lm_connection_unref ( connection );
    	free_data ( connect_data );
    	return KErrGeneral;
        }    
    
    lm_connection_close ( connection, NULL );
    lm_connection_unref ( connection );
    free_data ( connect_data );
    
    iLog->Log ( _L ( "lm_connection_is_authenticated passed" ) );
    
    return KErrNone;  
    }




//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_set_get_server
// Description  : Sets the server address for connection to server. Notice that
//                connection can't be open while doing this.
// Arguements   :
//   connection	: an LmConnection
//   server     : Address of the server ( name of the server )
// Returns      : None
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_set_get_serverL ( CStifItemParser& aItem )
    {
    LmConnection *connection   = NULL;
    gchar        *ServerName   = "ganapati.nokia.com";
    ConnectData  *connect_data = NULL;
    gchar        *NewServerName;
    
    iLog->Log ( _L ( "In lm_connection_set_server" ) );
    
    // Read data from the CFG file
    if ( read_data ( connect_data, aItem ) != RC_OK )
    	{
    	iLog->Log ( _L ( "read_data failed" ) );
    	free_data ( connect_data );
    	return KErrGeneral;
    	}
    
    // Open a new closed connection
    connection = lm_connection_new ( connect_data->server );
    if ( !connection )
    	{
    	iLog->Log ( _L ( " lm_connection_new failed" ) );
    	free_data ( connect_data );
    	return KErrGeneral;
    	}
    	
    
    
    // Set the servers name before the connection
    lm_connection_set_server ( connection, ServerName );
    
    // Verify the server name
    NewServerName = (gchar *)lm_connection_get_server ( connection );
    
    if ( strcmp ( NewServerName, ServerName ) != 0 )
        {
    	iLog->Log ( _L ( "lm_connection_set_server failed" ) );    	
    	lm_connection_unref ( connection );    	
    	return KErrGeneral;
        }
         
    lm_connection_unref ( connection );            
    free_data ( connect_data );
    // Server name modified successfully
    iLog->Log ( _L ( "lm_connection_set_server passed" ) );        
    
    return KErrNone;    	
    }


//-----------------------------------------------------------------------------
// Ctstlm:lm_connection_set_server_WithNULL 
// Description  : Sets the server address for connection to server. Notice that
//                connection can't be open while doing this.
// Arguements   :
//   connection	: an LmConnection
//   server     : Address of the server ( name of the server )
// Returns      : None
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_set_server_WithNULL ( CStifItemParser& aItem )
    {
    LmConnection *connection   = NULL;
    gchar        *ServerName   = "\0";
    ConnectData  *connect_data = NULL;
    gchar        *NewServerName;
    
    iLog->Log ( _L ( "In lm_connection_set_server" ) );
    
    // Read data from the CFG file
    if ( read_data ( connect_data, aItem ) != RC_OK )
    	{
    	iLog->Log ( _L ( "read_data failed" ) );
    	free_data ( connect_data );
    	return KErrGeneral;
    	}
    
    // Open a new closed connection
    connection = lm_connection_new ( connect_data->server );
    if ( !connection )
    	{
    	iLog->Log ( _L ( " lm_connection_new failed" ) );
    	free_data ( connect_data );
    	return KErrGeneral;
    	}
    	
    
    
    // Set the servers name before the connection
    lm_connection_set_server ( connection, ServerName );
    
    // Verify the server name
    NewServerName = (gchar *)lm_connection_get_server ( connection );
    
    if ( strcmp ( NewServerName, ServerName ) != 0 )
        {
    	iLog->Log ( _L ( "lm_connection_set_server failed" ) );    	
    	lm_connection_unref ( connection );    	
    	return KErrGeneral;
        }
         
    lm_connection_unref ( connection );            
    free_data ( connect_data );
    // Server name modified successfully
    iLog->Log ( _L ( "lm_connection_set_server passed" ) );        
    
    return KErrNone;    	
    }

//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_set_get_jid
// Description  : Sets the JID to be used for connection
// Arguements   :
//   connection	: an LmConnection
//   jid        : JID to be used for connection
// Returns      : None
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_set_get_jidL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	gchar *Jid                 = "connect.nokia.com";
	const gchar *GetJid        = NULL;
	
	iLog->Log ( _L ( "In lm_connection_set_jid" ) );
	
	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Open a new close connection
	connection = lm_connection_new ( connect_data->server );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set new jid
	Jid = g_strdup_printf ( "%s@%s", connect_data->username, connect_data->server );
	lm_connection_set_jid ( connection, Jid );
	
	
	// Verify the Jid
	GetJid = lm_connection_get_jid ( connection );
	if ( strcmp ( Jid, GetJid ) != 0 )
	    {
		iLog->Log ( _L ( "lm_connection_set_jid failed" ) );
		lm_connection_unref ( connection );
		return KErrGeneral;
	    }
	
	// Remove a referece on connection
	lm_connection_unref ( connection );
	free_data ( connect_data );
	iLog->Log ( _L ( "lm_connection_set_jid passed" ) );
	
	return KErrNone;
    }


//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_set_jid_WithNULL
// Description  : Sets the JID to be used for connection
// Arguements   :
//   connection	: an LmConnection
//   jid        : JID to be used for connection
// Returns      : None
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_set_jid_WithNULL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	gchar *Jid                 = "\0";
	const gchar *GetJid        = NULL;
	
	iLog->Log ( _L ( "In lm_connection_set_jid" ) );
	
	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Open a new close connection
	connection = lm_connection_new ( connect_data->server );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set new jid
	Jid = g_strdup_printf ( "%s@%s", connect_data->username, connect_data->server );
	lm_connection_set_jid ( connection, Jid );
	
	
	// Verify the Jid
	GetJid = lm_connection_get_jid ( connection );
	if ( strcmp ( Jid, GetJid ) != 0 )
	    {
		iLog->Log ( _L ( "lm_connection_set_jid failed" ) );
		lm_connection_unref ( connection );
		return KErrGeneral;
	    }
	
	// Remove a referece on connection
	lm_connection_unref ( connection );
	free_data ( connect_data );
	iLog->Log ( _L ( "lm_connection_set_jid passed" ) );
	
	return KErrNone;
    }



//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_set_get_port
// Description  : Sets the port that connection will be using
// Arguements   :
//   connection	: an LmConnection
//   port       : server port
// Returns      : None 
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_set_get_portL ( CStifItemParser& /*aItem*/ )
    {
	LmConnection *connection = NULL;
	guint        LmPort;	
	
	// Open a new closed connection
	connection = lm_connection_new ( NULL );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		return KErrGeneral;
		}
	
	// Sets the port 
	lm_connection_set_port ( connection, MYPORT );
	
	// Get the port
	LmPort = lm_connection_get_port ( connection );
	
	// Verify the port used by the connection 	
	if ( LmPort != MYPORT )
	    {
		iLog->Log ( _L ( "lm_connection_set_port failed" ) );
		lm_connection_unref ( connection );
		return KErrGeneral;
	    }
	
	// Remove a reference on connection
	lm_connection_unref ( connection );
	
	iLog->Log ( _L ( "lm_connection_set_port passed" ) );
	
	return KErrNone;	
    }


//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_set_port_WithNULL
// Description  : Sets the port that connection will be using
// Arguements   :
//   connection	: an LmConnection
//   port       : server port
// Returns      : None 
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_set_port_WithNULL ( CStifItemParser& /*aItem*/ )
    {
	LmConnection *connection = NULL;
	guint        LmPort;	
	
	// Open a new closed connection
	connection = lm_connection_new ( NULL );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		return KErrGeneral;
		}
	
	// Sets the port 
	lm_connection_set_port ( connection, FAKEPORT );
	
	// Get the port
	LmPort = lm_connection_get_port ( connection );
	
	// Verify the port used by the connection 	
	if ( LmPort != FAKEPORT )
	    {
		iLog->Log ( _L ( "lm_connection_set_port failed" ) );
		lm_connection_unref ( connection );
		return KErrGeneral;
	    }
	
	// Remove a reference on connection
	lm_connection_unref ( connection );
	
	iLog->Log ( _L ( "lm_connection_set_port passed" ) );
	
	return KErrNone;	
    }

//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_set_get_ssl
// Description  : Sets SSL struct or unset if ssl is NULL.
//                If set connection will use SSL for the connection
// Arguements   :
//   connection	: an LmConnection
//   ssl        : an LmSSL
// Returns      : None
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_set_get_sslL ( CStifItemParser& /*aItem*/ )
    {
    LmConnection *connection = NULL;
    LmSSL        *ssl        = NULL;
    
    iLog->Log ( _L ( "In lm_connection_set_ssl" ) );
    
    if ( lm_ssl_is_supported() )
        {       
        connection = lm_connection_new ( NULL );
    
        ssl = lm_ssl_new ( NULL, NULL, NULL, NULL );
    
        lm_connection_set_ssl ( connection, ssl );
        lm_ssl_unref ( ssl );
    
        if ( !lm_connection_get_ssl ( connection ) )
            {
    	    iLog->Log ( _L ( "lm_connection_set_ssl failed" ) );
    	    lm_connection_unref ( connection );    	    
    	    return KErrGeneral;    
            }
        
        lm_connection_unref ( connection );            
        
        iLog->Log ( _L ( "lm_connection_set_ssl passed" ) );
        
        return KErrNone;        
        }
    
    iLog->Log ( _L ( "ssl not supported" ) );           
    
    return KErrNone;    		    
    }

//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_set_ssl_WithNULL
// Description  : Sets SSL struct or unset if ssl is NULL.
//                If set connection will use SSL for the connection
// Arguements   :
//   connection	: an LmConnection
//   ssl        : an LmSSL
// Returns      : None
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_set_ssl_WithNULL ( CStifItemParser& /*aItem*/ )
    {
    LmConnection *connection = NULL;
    LmSSL        *ssl        = NULL;
    
    iLog->Log ( _L ( "In lm_connection_set_ssl" ) );
    
    if ( lm_ssl_is_supported() )
        {       
        connection = lm_connection_new ( NULL );
    
        ssl = lm_ssl_new ( NULL, NULL, NULL, NULL );
    
        lm_connection_set_ssl ( connection, ssl );
        lm_ssl_unref ( ssl );
    
        if ( !lm_connection_get_ssl ( connection ) )
            {
    	    iLog->Log ( _L ( "lm_connection_set_ssl failed" ) );
    	    lm_connection_unref ( connection );    	    
    	    return KErrGeneral;    
            }
        
        lm_connection_unref ( connection );            
        
        iLog->Log ( _L ( "lm_connection_set_ssl passed" ) );
        
        return KErrNone;        
        }
    
    iLog->Log ( _L ( "ssl not supported" ) );           
    
    return KErrNone;    		    
    }



//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_set_get_proxy
// Description  : Sets the proxy to use for this connection. To unset pass NULL
// Arguements   :
//   connection	: an LmConnection
//   proxy      : an LmProxy
// Returns      : None
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_set_get_proxyL ( CStifItemParser& /*aItem*/ )
    {
    LmConnection *connection;
    LmProxy      *Proxy;    
    
    iLog->Log ( _L ( "In lm_connection_set_proxy" ) );
    
    connection = lm_connection_new ( NULL );

    Proxy = lm_proxy_new ( LM_PROXY_TYPE_HTTP );
    
    // Set proxy 
    lm_connection_set_proxy ( connection, Proxy );
    lm_proxy_unref ( Proxy );
    
    // Verify proxy setttings
    if ( !lm_connection_get_proxy ( connection ) )
        {
    	iLog->Log ( _L ( "lm_connection_set_proxy failed" ) );
    	lm_connection_unref ( connection );
    	return KErrGeneral;
        }    
    
    lm_connection_unref ( connection );
    
    iLog->Log ( _L ( "lm_connection_set_proxy passed") );
    
    return KErrNone;    
    }


//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_sendL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_sendL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message;
	
	iLog->Log ( _L ( "In lm_connection_send" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );

	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );
	
	g_main_loop_run ( main_loop );

	g_main_loop_unref ( main_loop );    

	
	GMainLoop	*main_loop1 = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop1,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop1 );
		g_free ( username );
		return KErrGeneral;
	    }

	g_main_loop_run ( main_loop1 );

	g_main_loop_unref ( main_loop1 );
	    

	g_free ( username );
	
	// Construct a message
	message = lm_message_new ( connect_data->msg_data->recipient,               
	                           LM_MESSAGE_TYPE_MESSAGE );
	lm_message_node_add_child ( message->node, "body", 
	                            connect_data->msg_data->message);
	
	// Send message to the server
	if ( !lm_connection_send ( connection, message, NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_send failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		free_data ( connect_data );		
		return KErrGeneral;
	    }	
	
	// Close the connection
	lm_connection_close ( connection, NULL );
	
	// Remove a reference on connection
	lm_connection_unref ( connection );
	
	// Remove a reference on message
	lm_message_unref ( message );
	
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_send passed" ) );
	
	return KErrNone;
}



//-----------------------------------------------------------------------------
// Ctstlm::lm_send_with_replyL
// Description  : Send a LmMessage which will result in a reply
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   handler    : LmMessageHandler that will be used when reply to a message
//                arrives
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_send_with_replyL ( CStifItemParser& aItem )
    {
	LmConnection     *connection;
	ConnectData      *connect_data = NULL;
	LmMessage        *message;
	LmMessageNode    *q_node;
	LmMessageHandler *handler      = NULL;	
	
	iLog->Log ( _L ( "In lm_connection_send_with_reply" ) );
	
	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}	
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the port to gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif

	// Set the connection to use SSL
	SSLInit ( connection );
	
	// Open a connection with the server
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop ); 
	
	
	main_loop = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction )connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	g_free ( username );
	
	// Construct a message
	message = lm_message_new_with_sub_type ( NULL, 
	                                         LM_MESSAGE_TYPE_IQ,
	                                         LM_MESSAGE_SUB_TYPE_GET );	                                         
	q_node = lm_message_node_add_child ( message->node, "query", NULL );
	lm_message_node_set_attributes ( q_node, "xmlns", "jabber:iq:auth", NULL );
	lm_message_node_add_child ( q_node, "username", connect_data->username );
	
	main_loop = g_main_loop_new ( NULL, FALSE );                                         
	// Send message to the server
	handler = lm_message_handler_new ( handle_messages, main_loop, NULL );
	//we are sending a dummy username request to server and the server should reply with error or not supported message
	//we handle this in the handler and quit the mainloop.
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref ( handler );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );
		return KErrGeneral;
	    }	
	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	
	lm_connection_close ( connection, NULL );
	lm_connection_unref ( connection );
	lm_message_unref ( message );
	lm_message_handler_unref ( handler );
	free_data ( connect_data );
	
	iLog->Log ( _L ( "lm_connection_send_with_reply passed" ) );
	
	return KErrNone;
    }


//-----------------------------------------------------------------------------
// Ctstlm::register_message_handlerL
// Description  : Registers a LmMessageHandler to handle incoming messages of
//                certain type. 
// Arguements   :
//   connection	: Connection to register a handle for
//   handler    : Message handler to register
//   type       : Message type that handler will handle
//   priority   : The priority in which to call handler
// Returns      : None
//-----------------------------------------------------------------------------
//
TInt Ctstlm::register_message_handlerL ( CStifItemParser& aItem )
    {
	LmConnection     *connection;
	ConnectData      *connect_data;
	LmMessage        *msg;
	LmMessageHandler *handler;
	HandleData       *handle_data;
	GMainLoop        *main_loop   = NULL;		

	iLog->Log ( _L ( "In register_message_handlerL" ) );
	
	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}	
	
	// Create a new closed connection
	connection = lm_connection_new ( connect_data->server );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	lm_connection_set_jid ( connection, connect_data->username );
	
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif

	SSLInit ( connection );
		
	// Open a connection with the server
	main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, 
								( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop ); 
	
	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	main_loop = g_main_loop_new ( NULL, FALSE );
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction )connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	g_free ( username );
	
	
	main_loop = g_main_loop_new ( NULL, FALSE );
	handle_data = g_new0 ( HandleData, 1 );
	if ( !handle_data )
		{
		iLog->Log ( _L ( "Memory allocation failed for handle_data" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	handle_data->main_loop = main_loop;
	handle_data->Test_Success = RC_ERROR;
	
	// Register a message handler
	handler = lm_message_handler_new ( handle_messages, handle_data, NULL );
	if ( !handler )
		{
		iLog->Log ( _L ( "lm_message_handler_new failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		return KErrGeneral;
		}
	lm_connection_register_message_handler ( connection,
	                                         handler,
	                                         LM_MESSAGE_TYPE_IQ,
	                                         LM_HANDLER_PRIORITY_NORMAL );
	lm_message_handler_unref ( handler );	
	
	
	// Construct a message
	msg = lm_message_new ( connect_data->msg_data->recipient, 
	                           LM_MESSAGE_TYPE_IQ );
	lm_message_node_add_child ( msg->node, "query", NULL );

	
	//Send message to the server
	//Since we are sending dummy query to the recipient, we are expecting
	//an error message. This indicates the response was successfully received.
	if ( !lm_connection_send ( connection, msg, NULL ) )
	    {
		iLog->Log ( _L ( "lm_message_send failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( msg );
		return KErrGeneral;
	    }	
    
    // Wait for the reply from the server
    g_main_loop_run ( main_loop );
    g_main_loop_unref ( main_loop );
    
    // Free the allocated resources
    lm_connection_close ( connection, NULL );
    lm_connection_unref ( connection );
    lm_message_unref ( msg );
    
    // Verify wheather the callback function called or not
    if ( RC_OK != handle_data->Test_Success )
        {
    	iLog->Log ( _L ( "register_message_handlerL failed" ) );
    	g_free ( handle_data );
    	return KErrGeneral;
        }
    
    g_free ( handle_data );
	free_data ( connect_data );
    iLog->Log ( _L ( "register_message_handlerL passed" ) );
    
    return KErrNone;
    }


//-----------------------------------------------------------------------------
// Ctstlm::set_disconnect_functionL
// Description  : Set the callback that will be called when a connection is 
//                closed
// Arguements   :
//   connection	: Connection to register disconnect callback for
//   function   : Function to be called when connection is closed
//   user_data  : User data passed to a function
//   notify     : Function to free user_data
// Returns      : None
//-----------------------------------------------------------------------------
//
TInt Ctstlm::set_disconnect_functionL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;	
	ConnectData  *connect_data = NULL;
	HandleData   *handle_data  = NULL;
	
	iLog->Log ( _L ( "In set_disconnect_functionL" ) );
	
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}		
	
	// Create a new closed connection
	connection = lm_connection_new ( connect_data->server );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	lm_connection_set_jid ( connection, connect_data->username );
	
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif

	SSLInit ( connection );

	
	// Open a connection with the server
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop ); 
	
		
	handle_data = g_new0 ( HandleData, 1 );
	if ( !handle_data )
		{
		iLog->Log ( _L ( "Memory allocation failed for handle_data" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		return KErrGeneral;
		}
	handle_data->Test_Success = RC_ERROR;
	
	// Set a Callback function that will be called when connection is closed
	lm_connection_set_disconnect_function ( connection, 
	                                        connection_close_cb, 
	                                        handle_data,
	                                        NULL );
	
	// Close the opened connection
	lm_connection_close ( connection, NULL );
	lm_connection_unref ( connection );
	
	// Check wheather callback function is called or not
	if ( RC_OK != handle_data->Test_Success )
	    {
		iLog->Log ( _L ( "set_disconnect_functionL failed" ) );
		g_free ( handle_data );
		return KErrGeneral;
	    } 
	    
	g_free ( handle_data );	
	
	iLog->Log ( _L ( "set_disconnect_functionL passed" ) );
	free_data ( connect_data );
	
	return KErrNone;	
    }


//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_send_raw
// Description  : Asynchronus call to send a raw string. 
// Arguements   :
//   connection	: Connection used to send
//   str        : The string to send, the entire string will be sent
//   error      : Set if error was detected during sending
// Returns      : TRUE if no errors were detected during sending, FALSE 
//                otherwise
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_send_rawL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	GError       *error        = NULL;
	
	iLog->Log ( _L ( "In lm_connection_send_raw" ) );
	
	// Read data from the cfg file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Create a new closed connection
	connection = lm_connection_new ( connect_data->server );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	lm_connection_set_jid ( connection, connect_data->username );
	
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif

	SSLInit ( connection );
		
	// Open a connection with the server
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop ); 
	
	
	// Send a raw message 
	if ( !lm_connection_send_raw ( connection, 
	                               connect_data->msg_data->message, 
	                               &error ) )
	    {
		iLog->Log ( _L ( "lm_connection_send_raw failed: %s"), error->message );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		return KErrGeneral;
	    }
    
    lm_connection_close ( connection, NULL );
    lm_connection_unref ( connection );
    free_data ( connect_data );
    
    iLog->Log ( _L ( "lm_connection_send_raw passed" ) );
    
    return KErrNone;	
    }



//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_get_state
// Description  : Returns the state of the connection
// Arguements   :
//      connection	: Connection to get state
// Returns      : The state of the connection
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_get_stateL ( CStifItemParser& aItem )
    {
	LmConnection      *connection   = NULL;
	ConnectData       *connect_data = NULL;
	LmConnectionState state;
	
	iLog->Log ( _L ( "In lm_connection_get_state" ) );
	
	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Create a new closed connection
	connection = lm_connection_new ( connect_data->server );
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	lm_connection_set_jid ( connection, connect_data->username );
	
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif

	SSLInit ( connection );	
	
	// Verify the state when the connection is closed
	state = lm_connection_get_state ( connection );
	
	if ( LM_CONNECTION_STATE_CLOSED != state )
	    {
		iLog->Log ( _L ( "lm_connection_get_state failed:closed" ) );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		return KErrGeneral;
	    }
	
	// Verify the state after opening the connection
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop ); 
	
	    
	state = lm_connection_get_state ( connection );
	
	if ( LM_CONNECTION_STATE_OPEN != state )
	    {
		iLog->Log ( _L ( "lm_connection_get_state failed: open" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		return KErrGeneral;
	    }
	    
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	main_loop = g_main_loop_new ( NULL, FALSE );
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction )connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	g_free ( username );    
	
		    
	state = lm_connection_get_state ( connection );
	
	if ( LM_CONNECTION_STATE_AUTHENTICATED != state )
	    {
	    iLog->Log ( _L ( "lm_connection_get_state failed: authenticated" ) );
	    lm_connection_close ( connection, NULL );
	    lm_connection_unref ( connection );
	    free_data ( connect_data );
	    return KErrGeneral;
	    }    
	
	lm_connection_close ( connection, NULL );
	lm_connection_unref ( connection );
	free_data ( connect_data );
	
	iLog->Log ( _L ( "lm_connection_get_state passed" ) );
	
	return KErrNone;
    }


//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_ref
// Description  : Add a reference on connection
// Arguements   :
//   connection	: Connection to a add a reference to 
// Returns      : Returns the same connection
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_refL ( CStifItemParser& /*aItem*/ )
    {
	LmConnection *connection = NULL;	
	
	iLog->Log ( _L ("In lm_connection_unref" ) );
	
	// Create a new closed connection
	connection = lm_connection_new ( NULL );
		
	// Add a reference on connection
	connection = lm_connection_ref ( connection );
	
    // Loudmouth don't have any API to check the ref count on connection
    // Therefore lm_connection_ref is considered as passed.
    	
    // Decrement the reference count and free the memory
	lm_connection_unref ( connection );
	lm_connection_unref ( connection );
	
	iLog->Log ( _L ( "lm_connection_ref passed" ) );
	
	return KErrNone;
    }


//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_unref
// Description  : Removes the reference on connection. If there are no
//                references to connection, it will be freed and shouldn't
//                used again
//                Check the total available heap space
//                Create a new closed connection,
//                Check that available heap space is reduced
//                Decrement the reference on connection
//                Check the available heap is equal to what it was before                 
// Arguements   :
//   connection	: Connection to remove reference from
// Returns      : None
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_unrefL ( CStifItemParser& /*aItem*/ )
    {
	LmConnection *connection = NULL;
	RHeap&       heap_handle = User::Heap();
	TInt         largest_free_block;
	TInt         heap_available_before;
	TInt         heap_available_after;	
	
	iLog->Log ( _L ( "In lm_connection_unref" ) );	
__UHEAP_MARK;
    // Get the heap space before creating closed connection
	heap_available_before = heap_handle.Available ( largest_free_block );	
	
	// Create a new closed connection
	connection = lm_connection_new ( NULL );	
	
	// Decrement the reference on connection
	lm_connection_unref ( connection );

	// Get the heap space after freeing memory
	heap_available_after = heap_handle.Available ( largest_free_block );	

   	if ( heap_available_after != heap_available_before )
	    {
		iLog->Log ( _L ( "{heap_available_before = %d, heap_available_before = %d}" ),
		                   heap_available_before, heap_available_after );
		iLog->Log ( _L ( "Available memeory is not as expected after free!" ) );
		return KErrGeneral;                   
	    }	
__UHEAP_MARKEND;
	iLog->Log ( _L ( "{ Expected:heap_available_before is equal to heap_available_after }" ) );
	iLog->Log ( _L ( "heap_available_before = %d, heap_available_after = %d" ),
	                  heap_available_before, heap_available_after );
	iLog->Log ( _L ( "lm_connection_unref passed" ) );
		
	return KErrNone;
	
    }
    

// ----------------------------------------------------------------------------
// Ctstlm::lm_message_new
// Description:Creates a new LmMessage which can be sent with 
//             lm_connection_send() or lm_connection_send_with_reply().
// Arguements :
//    to      : recipient jid
//    type    : message type
// Returns    : a newly created LmMessage       
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_newL ( CStifItemParser& aItem )
    {
	LmMessage     *message;
	ConnectData   *connect_data = NULL;
	LmMessageType MessageType;
	gchar         Type[256];	
	
	iLog->Log ( _L ( "\tIn lm_message_new" ) );	
	
	// Read data from the CFG file	
	if ( read_data ( connect_data, aItem ) != RC_OK )
	{
		iLog->Log ( _L ( "\tread_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
	}
	
	// Construct a message with a type=LM_MESSAGE_TYPE_MESSAGE
	iLog->Log ( _L ( "\tConstruct a message" ) );
	
	message = lm_message_new ( connect_data->msg_data->recipient, 
	                           LM_MESSAGE_TYPE_MESSAGE );
	if ( !message )
	{
		iLog->Log ( _L ( "\tlm_message_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
	}
	
	
	// Verify the message type
	iLog->Log ( _L ( "\tVerify the message type" ) );
	
	MessageType = lm_message_get_type ( message );
	if ( LM_MESSAGE_TYPE_MESSAGE != MessageType )
	    {
		iLog->Log ( _L( "\tExpected message type: LM_MESSAGE_TYPE_MESSAGE" ) );
		memset ( Type, '\0', sizeof ( Type ) );
		GetMessageType ( MessageType, Type );
		iLog->Log ( _L8 ( "\tActual message type  : %s" ), Type );
		
		iLog->Log ( _L ( "\tlm_message_get_type failed" ) );
		lm_message_unref ( message );
		return KErrGeneral;
	    }	
	
	lm_message_unref ( message );	
	free_data ( connect_data );
	
	iLog->Log ( _L ( "lm_message_new passed" ) );
	
	return KErrNone;
	
    }

 
 // ---------------------------------------------------------------------------
// Ctstlm::lm_message_new_with_sub_type
// Description:Creates a new LmMessage with subtype set.
// Arguements :
//  to        : recipient jid
//  type      : message type
//  sub_type  : message sub type
// Returns    : a newly created LmMessage       
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_new_with_sub_typeL ( CStifItemParser& aItem )
    {
	LmMessage        *message;
	ConnectData      *connect_data = NULL;
	LmMessageType    MessageType;
	LmMessageSubType MessageSubType;
	gchar            Type[256];
	gchar            SubType[256];
	
	iLog->Log ( _L ( "In lm_message_new_with_sub_type" ) );
		
	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
	    {
		iLog->Log ( _L ( "\tread_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
	    }
	
	// Construct a message with type and sub type	
	message = lm_message_new_with_sub_type ( connect_data->msg_data->recipient,
	                                         LM_MESSAGE_TYPE_IQ,
	                                         LM_MESSAGE_SUB_TYPE_SET );
	if ( !message )
	    {
		iLog->Log ( _L ( "\tlm_message_new_with_sub_type failed" ) );
		free_data ( connect_data );		
		return KErrGeneral;
	    }

	
	// Verify the message type	
	MessageType = lm_message_get_type ( message );
	if ( LM_MESSAGE_TYPE_IQ != MessageType )
	    {
	    iLog->Log ( _L ( "\tExpected message type: LM_MESSAGE_TYPE_IQ" ) );
	    memset ( Type, '\0', sizeof ( Type ) );
	    GetMessageType ( MessageType, Type );
	    iLog->Log ( _L8 ( "\tActual message type  : %s"), Type );		
	    lm_message_unref ( message );
	    return KErrGeneral;
	    }
	
	// Verify the message sub type	
	MessageSubType = lm_message_get_sub_type ( message );
	if ( LM_MESSAGE_SUB_TYPE_SET != MessageSubType )
	    {
		iLog->Log ( _L ( "\tExpected message subtype: LM_MESSAGE_SUB_TYPE_SET" ) );
		memset ( SubType, '\0', sizeof ( SubType ) );
		GetMessageSubType ( MessageSubType, SubType );
		iLog->Log ( _L8 ( "\tActual message subtype  : %s" ), SubType );
		lm_message_unref ( message );
		return KErrGeneral;
	    }	    
	
	lm_message_unref ( message );
		    
	free_data ( connect_data );    
	iLog->Log ( _L ( "lm_message_new_with_sub_type passed" ) );
	
	return KErrNone;
    }
    

// ----------------------------------------------------------------------------
// Ctstlm::lm_message_get_type
// Description:Fetches the type of the message
// Arguements :
//    message : an LmMessage
// Returns    : the message type
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_get_typeL ( CStifItemParser& /*aItem*/ )
    {
	LmMessage     *message      = NULL;
	LmMessageType MessageType;
	gchar         Type[256];
	
	iLog->Log ( _L ( "In lm_message_get_type" ) );
	memset ( Type, '\0', sizeof ( Type ) );
	
	// Construct a message with a type	
	message = lm_message_new ( NULL, LM_MESSAGE_TYPE_PRESENCE );
	if ( !message )
	    {
		iLog->Log ( _L ( "\tlm_message_new failed while constructing PRESENCE msg" ) );
		return KErrGeneral;
	    } 
	
	// Verify the message type=LM_MESSAGE_TYPE_PRESENCE
	iLog->Log ( _L ( "\tMessage type verification" ) );
		
	MessageType = lm_message_get_type ( message );
	if ( LM_MESSAGE_TYPE_PRESENCE != MessageType )
	    {
		iLog->Log ( _L ( "\tExpected message type: LM_MESSAGE_TYPE_PRESENCE" ) ) ;
		memset ( Type, '\0', sizeof ( Type ) );
		GetMessageType ( MessageType, Type );
		iLog->Log ( _L8 ( "\tActual message type  :%s" ), Type );
		lm_message_unref ( message );
		return KErrGeneral;
	    }
	    
    // Construct another message with type=LM_MESSAGE_TYPE_IQ
    iLog->Log ( _L ( "\tConstruct a message with another type" ) );
    
    lm_message_unref ( message );
    message = NULL;    
    
    message = lm_message_new ( NULL, LM_MESSAGE_TYPE_IQ );
    if ( !message )
        {
    	iLog->Log ( _L ( "\tlm_message_new failed while constructing IQ message " ) );
    	lm_message_unref ( message );
    	return KErrGeneral;
        }
    
    // Verify the message type
    iLog->Log ( _L ( "\tVerify the message type" ) );
    
    MessageType = lm_message_get_type ( message );
    if ( LM_MESSAGE_TYPE_IQ != MessageType )
        {
    	iLog->Log ( _L ( "\tExpected message type: LM_MESSAGE_TYPE_IQ" ) );
    	memset ( Type, '\0', sizeof ( Type ) );
    	GetMessageType ( MessageType, Type );
    	iLog->Log ( _L8 ( "\tActual message type  : %s" ), Type );
    	lm_message_unref ( message );
    	return KErrGeneral;
        }
    
    lm_message_unref ( message );
     
    iLog->Log ( _L ( "\tlm_message_get_type passed" ) );    
    
    return KErrNone;
    }
    

// ----------------------------------------------------------------------------
// Ctstlm::lm_message_get_sub_type
// Description:Fetches the sub type of the message
// Arguements :
//    message : an LmMessage
// Returns    : the message sub type
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_get_sub_typeL ( CStifItemParser& /*aItem*/ )
    {
	LmMessage        *message = NULL;
	LmMessageSubType MessageSubType;
	gchar            SubType[256];
	
	iLog->Log ( _L ( "In lm_message_get_sub_type" ) );
	
	// Construct a message with type and subtype=LM_MESSAGE_SUB_TYPE_SUBSCRIBE
	iLog->Log ( _L ( "\tConstruct a message with type & subtype" ) );
	
	message = lm_message_new_with_sub_type ( NULL, 
	                                         LM_MESSAGE_TYPE_MESSAGE,
	                                         LM_MESSAGE_SUB_TYPE_SUBSCRIBE );
	if ( !message )
	    {
		iLog->Log ( _L ( "lm_message_new_with_sub_type failed for SUBSCRIBE" ) );
		return KErrGeneral;
	    }
	
	// Verify message subtype	
	MessageSubType = lm_message_get_sub_type ( message );
	if ( LM_MESSAGE_SUB_TYPE_SUBSCRIBE != MessageSubType )
	    {
		iLog->Log ( _L ( "\tExpected MessageSubType: LM_MESSAGE_SUB_TYPE_SUBSCRIBE" ) );		
		memset ( SubType, '\0', sizeof ( SubType ) );
		GetMessageSubType ( MessageSubType, SubType );
		iLog->Log ( _L8 ( "\tActual MessageSubType  : %s" ), SubType );
		lm_message_unref ( message );
		return KErrGeneral;
	    }
	
	// Construct another message with subtype=LM_MESSAGE_SUB_TYPE_CHAT	
	lm_message_unref ( message );
	message = NULL;
	
	message = lm_message_new_with_sub_type ( NULL,
	                                         LM_MESSAGE_TYPE_MESSAGE,
	                                         LM_MESSAGE_SUB_TYPE_CHAT );
	if ( !message )
	    {
		iLog->Log ( _L ( "\tlm_message_new_with_sub_type failed for CHAT" ) );
		return KErrGeneral;
	    }
	
	// Verify message subtype	
	MessageSubType = lm_message_get_sub_type ( message );
	if ( LM_MESSAGE_SUB_TYPE_CHAT != MessageSubType )
	    {
		iLog->Log ( _L ( "\tExpected MessageSubType: LM_MESSAGE_SUB_TYPE_CHAT" ) );
		
		memset ( SubType, '\0', sizeof ( SubType ) );
		GetMessageSubType ( MessageSubType, SubType );
		iLog->Log ( _L8 ( "\tActual MessageSubType  : %s" ), SubType );
		lm_message_unref ( message );
		return KErrGeneral;
	    }	
	
	lm_message_unref ( message );
	
	iLog->Log ( _L ( "\tlm_message_get_sub_type passed" ) );	
	
	return KErrNone;
    }


// ----------------------------------------------------------------------------
// Ctstlm::lm_message_get_node
// Description:Retrives the root node from LmMessage
// Arguements :
//   message  : an LmMessage
// Returns    : an LmMessageNode
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_get_nodeL ( CStifItemParser& /*aItem*/ )
    {
	LmMessage     *message;
	LmMessageNode *Node = NULL;
	
	iLog->Log ( _L ( "In lm_message_get_node" ) );
	
	// Construct a message with type=LM_MESSAGE_TYPE_MESSAGE
	iLog->Log ( _L ( "Construct a message" ) );
	
	message = lm_message_new ( NULL, LM_MESSAGE_TYPE_MESSAGE );
	if ( !message )
	    {
		iLog->Log ( _L ( "lm_message_new failed" ) );
		return KErrGeneral;
	    }
	
	// Retrive root node from the message
	iLog->Log ( _L ( "Retrive root node from message" ) );
	
	Node = lm_message_get_node ( message );
	if ( !Node )
	    {
		iLog->Log ( _L ( "lm_message_get_node failed" ) );
		lm_message_unref ( message );
		return KErrGeneral;
	    }		
	
	lm_message_unref ( message );
	lm_message_node_unref ( Node );
	
	iLog->Log ( _L ( "lm_message_get_node passed" ) );
	
	return KErrNone;
    }


    
// ----------------------------------------------------------------------------
// Ctstlm::lm_message_ref 
// Description:Adds a reference to message
// Arguements :
//       message   : an LmMessage
// Returns    : an message
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_refL ( CStifItemParser& /*aItem*/ )
{
	LmMessage *message = NULL;
	
	iLog->Log ( _L ( "In lm_message_ref" ) );
	
	// Construct a message	
	message = lm_message_new ( NULL, LM_MESSAGE_TYPE_MESSAGE );
	if ( !message )
	    {
		iLog->Log ( _L ( "lm_message_new failed" ) );
		return KErrGeneral;
	    }
	
	// Add a reference to message 	
	message = lm_message_ref ( message );
	
	// Loudmouth don't have any API to check the ref count on message
    // Therefore lm_message_ref is considered as passed.
    
    // Decrement the reference(2 times) to message and free the memory    
    lm_message_unref ( message );
    lm_message_unref ( message );
    
    iLog->Log ( _L ( "lm_message_ref passed" ) );
    
    return KErrNone;
}


// ----------------------------------------------------------------------------
// Ctstlm::lm_message_unref 
// Description:Removes a reference from message. When no more references are 
//             present, the message is freed.
//             Check the total available heap space
//             Construct a new message by using lm_message_new
//             Check that available heap space is reduced
//             Decrement the reference from message
//             Check the available heap is equal to what it was before
// Arguements :
//   message  : an LmMessage
// Returns    : None
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_unrefL ( CStifItemParser& /*aItem*/ )
{
	LmMessage *message;
	RHeap&    heap_handle = User::Heap();
	TInt 	  largest_free_block;
	TInt      heap_available_before;
	TInt      heap_available_after;
	
	iLog->Log ( _L ( "In lm_message_unref" ) );
	
	// Get the total heap space before constructing a message
	iLog->Log ( _L ( "Getting the heap size before message construction" ) );	
	heap_available_before = heap_handle.Available ( largest_free_block );
		
	// Construct a message 
	iLog->Log ( _L ( "Construct a message" ) );
	
	message = lm_message_new ( NULL, LM_MESSAGE_TYPE_MESSAGE );
	if ( !message )
	{
		iLog->Log ( _L ( "lm_message_new failed" ) );
		return KErrGeneral;
	}
	
	// Decrement the reference from message
	iLog->Log ( _L ( "Remove the reference from the message" ) );
	lm_message_unref ( message );
	
	// Get the heap space after freeing memory
	iLog->Log ( _L ( "Getting the heap size after freeing the memory" ) );
	heap_available_after = heap_handle.Available ( largest_free_block );
	
	// Compare the heap size
	iLog->Log ( _L ( "Comparing heap size before and after freeing memory" ) );
	
	if ( heap_available_after != heap_available_before )
	{
		iLog->Log ( _L ( "heap_available_before = %d, heap_available_after = %d" ),
		                  heap_available_before, heap_available_after );
		iLog->Log ( _L ( "Available memory is not as expected after freeing" ) );
		return KErrGeneral;
	}
	
	iLog->Log ( _L ( "Expected: heap_available_before is equal to heap_available_after" ) );
	iLog->Log ( _L ( "heap_available_before = %d, heap_available_after = %d" ),
	                  heap_available_before, heap_available_after );
	
	return KErrNone;
}


// ----------------------------------------------------------------------------
// Ctstlm::lm_message_handler_new
// Description:Creates a new message handler.This can be set to handle incoming
//             messages and when a message of type the handler is registered to 
//             handle is received function will be called and user_data will be
//             passed to it. notify is called when the message handler is freed,
//             that way any memory allocated by user_data can be freed.
// Arguements :
//  function  : a callback
//  user_data : user data passed to function
//  notify    : function called when the message handler is freed
// Returns    : a newly created message handler
//
// Note       : This test case requires UI mode to select Access point
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_handler_newL ( CStifItemParser& /*aItem*/ )
{
	LmMessageHandler *handler      = NULL;
	
	iLog->Log ( _L ( "In lm_message_handler_new" ) );
	
	// Create a new message handler
	handler = lm_message_handler_new ( handle_messages, NULL, NULL );
	if ( !handler )
		{
		iLog->Log ( _L ( "lm_message_handler_new failed" ) );
		return KErrGeneral;
		}
		
	// Validate the handler
	if ( !lm_message_handler_is_valid ( handler ) )
		{
		iLog->Log ( _L ( "message handler is not valid" ) );
		lm_message_handler_unref ( handler );
		return KErrGeneral;
		}
	
	iLog->Log ( _L ( "lm_message_handler_new passed" ) );
	
	return KErrNone;
}


// ----------------------------------------------------------------------------
// Ctstlm::lm_message_handler_invalidate
// Description:Invalidates the handler. Useful if you need to cancel a reply
// Arguements :
//   handler  : an LmMessageHandler
// Returns    : None
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_handler_invalidateL ( CStifItemParser& /*aItem*/ )
{
	LmMessageHandler *handler      = NULL;	
	gboolean         IsValid;
		
	iLog->Log ( _L ( "In lm_message_handler_invalidate" ) );
	
	// Create a new message handler
	iLog->Log ( _L ( "Creat a new message handler" ) );
	
	handler = lm_message_handler_new ( handle_messages, NULL, NULL );
	if ( !handler )
	    {
		iLog->Log ( _L ( "lm_message_handler_new failed" ) );
		return KErrGeneral;
	    }
	
	// Invalidate the message handler ( handler->valid = FALSE )
	iLog->Log ( _L ( "Invalidate the handler" ) );
	lm_message_handler_invalidate ( handler );
	
	// Check for validity of message handler
	iLog->Log ( _L ( "Checking validity of message handler" ) );
	
	IsValid = lm_message_handler_is_valid ( handler );
	if ( IsValid )
	    {
		iLog->Log ( _L ( "lm_message_handler_invalidate failed" ) );		
		lm_message_handler_unref ( handler );
		return KErrGeneral;
	    }
	
	lm_message_handler_unref ( handler );	
	
	iLog->Log ( _L ( "lm_message_handler_invalidate passed" ) );	
	
	return KErrNone;
}


// ----------------------------------------------------------------------------
// Ctstlm::lm_message_handler_is_valid
// Description:Fetches whether the handler is valid or not.
// Arguements :
//   handler  : an LmMessageHandler
// Returns    : TRUE if handler is valid, otherwise FALSE
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_handler_is_validL ( CStifItemParser& /*aItem*/ )
{
	LmMessageHandler *handler = NULL;
	
	iLog->Log ( _L ( "In lm_message_handler_is_valid" ) );
	
	// Create a new message handler
	iLog->Log ( _L ( "Constructing new message handler" ) );
	handler = lm_message_handler_new ( handle_messages, NULL, NULL );
	if ( !handler )
	    {
		iLog->Log ( _L ( "lm_message_handler_new failed" ) );
		return KErrGeneral;
	    }
	
	// Check the validity of the message handler
	iLog->Log ( _L ( "Check for validity of message handler" ) );
	if ( !lm_message_handler_is_valid ( handler ) )
	    {
		iLog->Log ( _L ( "lm_message_handler_is_valid failed" ) );
		lm_message_handler_unref ( handler );
		return KErrGeneral;
	    }
	
	// Invalidate the handler ( set handler->valid = FALSE )
	iLog->Log ( _L ( "Invalidate the message handler" ) );
	lm_message_handler_invalidate ( handler );
	
	// check for validity of message handler after invalidating
	iLog->Log ( _L ( "Check for validity after invalidating" ) );
	
	if ( lm_message_handler_is_valid ( handler ) )
	    {
		iLog->Log ( _L ( "lm_message_handler_is_valid failed" ) );
		lm_message_handler_unref ( handler );
		return KErrGeneral;
	    }
	
	lm_message_handler_unref ( handler );
	
	iLog->Log ( _L ( "lm_message_handler_is_valid passed" ) );
	
	return KErrNone;
}


// ----------------------------------------------------------------------------
// Ctstlm::lm_message_handler_ref
// Description:Adds a reference to handler
// Arguements :
//   handler  : an LmMessageHandler
// Returns    : the message handler
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_handler_refL ( CStifItemParser& /*aItem*/ )
{
	LmMessageHandler *handler = NULL;	
	
	iLog->Log ( _L ( "In lm_message_handler_ref" ) );
	
	// Create a new message handler
	iLog->Log ( _L ( "Construct a new message handler" ) );
	
	handler = lm_message_handler_new ( handle_messages, NULL, NULL );
	if ( !handler )
	    {
		iLog->Log ( _L ( "lm_message_handler_new failed" ) );
		return KErrGeneral;
	    }	
	
	// Loudmouth don't have any API to check reference to handler
	// Therefore this test is considered as passed without checking
	
	iLog->Log ( _L ( "Remove a reference from the message handler" ) );
	lm_message_handler_unref ( handler );
	
	iLog->Log ( _L ( "lm_message_handler_ref passed" ) );
	
	return KErrNone;
}


// ----------------------------------------------------------------------------
// Ctstlm::lm_message_handler_unref
// Description:Removes a reference from handler. When no more references are 
//             present, the handler is freed
//             Check the total available heap space
//             Construct a new message handler
//             Check that available heap space is reduced
//             Remove reference from the message handler
//             Check the available heap is equal to what it was before
// Arguements :
//    handler : an LmMessageHandler
// Returns    : None
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_handler_unrefL ( CStifItemParser& /*aItem*/ )
{
	LmMessageHandler *handler     = NULL;
	RHeap&            heap_handle = User::Heap();
	TInt              largest_free_block;
	TInt              heap_avail_before;
	TInt              heap_avail_after;
	
	iLog->Log ( _L ( "Inlm_message_handler_unref" ) );
	
	// Get the total heap size before creating new message handler
	iLog->Log ( _L ( "Get the heap size before allocating memory" ) );
	heap_avail_before = heap_handle.Available ( largest_free_block );
	
	// Create a new message handler
	iLog->Log ( _L ( "Construct a new message handler" ) );
	
	handler = lm_message_handler_new ( handle_messages, NULL, NULL );
	if ( !handler )
	    { 
		iLog->Log ( _L ( "lm_message_handler_new failed" ) );
		return KErrGeneral;
	    }
	
	// Remove a reference from the message handler
	iLog->Log ( _L ( "Freeing allocated memory" ) );
	lm_message_handler_unref ( handler );
	
	// Get the heap size after removing reference from message handler
	iLog->Log ( _L ( "Get the heap size after freeing memory" ) );
	heap_avail_after = heap_handle.Available ( largest_free_block );
	
	// Compare the heap size
	iLog->Log ( _L ( "Compare the heap size after freeing memory" ) );
	
	if ( heap_avail_before != heap_avail_after )
	    {
		iLog->Log ( _L ( "heap_avail_before = %d, heap_avail_after = %d" ),
		                  heap_avail_before, heap_avail_after );
		iLog->Log ( _L ( "Available memory is not as expected after freeing" ) );
		return KErrGeneral;		
	    }
	
	iLog->Log ( _L ( "Expected: heap_avail_before is equal to heap_avail_after" ) );
	iLog->Log ( _L ( "heap_avail_before = %d, heap_avail_after = %d" ),
	                 heap_avail_before, heap_avail_after );
	
	return KErrNone;
}



// ----------------------------------------------------------------------------
// Ctstlm::lm_message_node_set_value
// Description:Sets the value of node. If a prev value is set it will be freed
// Arguements :
//   Node     : an LmMessageNode
//   value    : the new value
// Returns    : None
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_node_set_get_valueL ( CStifItemParser& /*aItem*/ )
{
	LmMessage     *message       = NULL;
	LmMessageNode *Node          = NULL;
	const gchar   *SetUsername   = "Nokia";		
	
	iLog->Log ( _L ( "In lm_message_node_get_value" ) );
	
	// Construct a message with type = LM_MESSAGE_TYPE_MESSAGE
	iLog->Log ( _L ( "Construct a message" ) );
	
	message = lm_message_new ( NULL, LM_MESSAGE_TYPE_MESSAGE );
	if ( !message )
	{
		iLog->Log ( _L ( "lm_message_new failed" ) );
		return KErrGeneral;
	}
	
	// Retrive the root node from message
	iLog->Log ( _L ( "Retriving root node from the message" ) );
	Node = lm_message_get_node ( message );
	if ( !Node )
	    {
		iLog->Log ( _L ( "lm_message_get_node failed" ) );
		lm_message_unref ( message );
		return KErrGeneral;		
	    }
	
	// Set the value of the node
	iLog->Log ( _L ( "Set the value of the node" ) );
	lm_message_node_set_value ( Node, SetUsername );
	
	// Retrive the value of node
	iLog->Log ( _L ( "Retrive the value of the node" ) );
	const gchar *GetUsername = lm_message_node_get_value ( Node );	
	
	// Verify the result
	iLog->Log ( _L ( "Verify value of node" ) );	
	if ( strcmp ( SetUsername, GetUsername ) )
	    {
		iLog->Log ( _L ( "lm_message_set_value failed" ) );		
		lm_message_unref ( message );
	    lm_message_node_unref ( Node );
		return KErrGeneral;
	    }	
	
	lm_message_unref ( message );
	lm_message_node_unref ( Node );
	
	iLog->Log ( _L ( "lm_message_set_value passed" ) );
		
	return KErrNone;
}


// ----------------------------------------------------------------------------
// Ctstlm::lm_message_node_add_child
// Description:Add a child node with a name and value
// Arguements :
//   node     : an LmMessageNode
//   name     : name of the new child
//   value    : value of the new child
// Returns    : the newly created child
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_node_add_childL ( CStifItemParser& /*aItem*/ )
{
	LmMessage     *message   = NULL;
	LmMessageNode *Node      = NULL;	
	const gchar   *Username  = "Nokia";		
	
	iLog->Log ( _L ( "In lm_message_node_add_child" ) );
	
	// Construct a message with type = LM_MESSAGE_TYPE_MESSAGE
	iLog->Log ( _L ( "construct a message" ) );
	
	message = lm_message_new ( NULL, LM_MESSAGE_TYPE_MESSAGE );
	if ( !message )
	    {
		iLog->Log ( _L ( "lm_message_new failed" ) );
		return KErrGeneral;
	    }
	
	// Retrive the root node from message
	iLog->Log ( _L ( "Retrive the root node from message" ) );
	
	Node = lm_message_get_node ( message );
	if ( !Node )
	    {
		iLog->Log ( _L ( "lm_message_get_node failed" ) );
		lm_message_unref ( message );
		return KErrGeneral;		
	    }
	
	// Add a child
	iLog->Log ( _L ( "Add a child 'username' to node" ) );
	lm_message_node_add_child ( Node, "username", Username );
	
	// Verify the child
	iLog->Log ( _L ( "Verify the child" ) );
	if ( !lm_message_node_get_child ( Node, "username" ) ) 
	    {
		iLog->Log ( _L ( "lm_message_add_child failed" ) );
		lm_message_unref ( message );
		lm_message_node_unref ( Node );		
		return KErrGeneral;
	    }
	
	iLog->Log ( _L ( "Freeing allocated resources" ) );
	lm_message_unref ( message );
	lm_message_node_unref ( Node );	
	
	iLog->Log ( _L ( "lm_message_node_add_child passed" ) );
	
	return KErrNone;	    
}



// ----------------------------------------------------------------------------
// Ctstlm::lm_message_node_find_child
// Description:Locates a child among all children of node. The entire tree will
//             be searched until a child with a name child_name is located.
// Arguements :
//    node    : an LmMessageNode
//    name    : name of the child to find
// Returns    : located child or NULL if not found
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_node_find_childL ( CStifItemParser& /*aItem*/ )
{
	LmMessage     *message   = NULL;
	LmMessageNode *Node      = NULL;	
	const gchar   *Username  = "Nokia";	
	const gchar   *Password  = "Nokia";		
	
	iLog->Log ( _L ( "In lm_message_node_find_child" ) );
	
	// Construct a message with type = LM_MESSAGE_TYPE_MESSAGE
	iLog->Log ( _L ( "Construct a message" ) );	
	message = lm_message_new ( NULL, LM_MESSAGE_TYPE_MESSAGE );
	if ( !message )
	    {
		iLog->Log ( _L ( "lm_message_new failed" ) );
		return KErrGeneral;
	    }
	
	// Retrive the root node from message
	iLog->Log ( _L ( "Retrive the root node from the message" ) );	
	Node = lm_message_get_node ( message );
	if ( !Node )
	    {
		iLog->Log ( _L ( "lm_message_get_node failed" ) );
		lm_message_unref ( message );
		return KErrGeneral;		
	    }
	
	// Add childrens
	iLog->Log ( _L ( "Adding childrens 'username' and 'password'" ) );
	lm_message_node_add_child ( Node, "username", Username );
	lm_message_node_add_child ( Node, "password", Password );
	
	// Locate the child 'username'	
	iLog->Log ( _L ( "Locate the child 'username'" ) );	
	if ( !lm_message_node_find_child ( Node, "username" ) ) 
	    {	    
		iLog->Log ( _L ( "lm_message_node_find_child: child 'username' not found" ) );
		lm_message_unref ( message );
		lm_message_node_unref ( Node );		
		return KErrGeneral;
	    }
	
	// Locate the child 'password'
	if ( !lm_message_node_get_child ( Node, "password" ) )
	    {
	    iLog->Log ( _L ( "lm_message_node_find_child: child 'password' not found" ) );		
		lm_message_unref ( message );
		lm_message_node_unref ( Node );
		return KErrGeneral;
	    }	
		
	lm_message_unref ( message );
	lm_message_node_unref ( Node );	
	
	iLog->Log ( _L ( "lm_message_node_find_child passed" ) );
	
	return KErrNone;	    
}


// ----------------------------------------------------------------------------
// Ctstlm::lm_message_node_set_get_attributes
// Description:Sets a list of attributes. The arguements should be names and 
//             correspoinding values and needs to be ended with NULL.
// Arguements :
//    node    : an LmMessageNode
//    name    : first attribute, should be fallowed by string with value
//    ..      : rest of the name/value pairs
// Returns    : None
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_node_set_get_attributesL ( CStifItemParser& /*aItem*/ )
{
	LmMessage     *message   = NULL;
	LmMessageNode *Node      = NULL;	
	const gchar   *Value     = "Nokia";		
	
	iLog->Log ( _L ( "In lm_message_node_set_attributes" ) );
	
	// Construct a message with type = LM_MESSAGE_TYPE_MESSAGE
	iLog->Log ( _L ( "Construct a message" ) );
	
	message = lm_message_new ( NULL, LM_MESSAGE_TYPE_MESSAGE );
	if ( !message )
	    {
		iLog->Log ( _L ( "lm_message_new failed" ) );
		return KErrGeneral;
	    }
	
	// Retrive the root node from message
	iLog->Log ( _L ( "Retrive the root node from the message" ) );
	
	Node = lm_message_get_node ( message );
	if ( !Node )
	    {
		iLog->Log ( _L ( "lm_message_get_node failed" ) );
		lm_message_unref ( message );
		return KErrGeneral;		
	    }
	
	// Set attributes
	iLog->Log ( _L ("Setting attributes 'username', 'password' and 'xmlns'" ) );
	lm_message_node_set_attributes ( Node, 
	                                 "username", Value,
	                                 "password", Value,
	                                 "xmlns", "jabber:iq:register",
	                                 NULL );
	                                 
    // Verify the attributes set
    iLog->Log ( _L (" Verify the attributes" ) );
    
    if ( !lm_message_node_get_attribute ( Node, "username" ) )
        {
    	iLog->Log ( _L ( "lm_message_set_attributes failed for username" ) );
    	lm_message_unref ( message );
    	lm_message_node_unref ( Node );
    	return KErrGeneral;
        }
    
    if ( !lm_message_node_get_attribute ( Node, "password" ) )
        {
    	iLog->Log ( _L ( "lm_message_set_attributes failed for password" ) );
    	lm_message_unref ( message );
    	lm_message_node_unref ( Node );
    	return KErrGeneral;
        }
    
    if ( !lm_message_node_get_attribute ( Node, "xmlns" ) )
        {
    	iLog->Log ( _L ( "lm_message_set_attributes failed for xmlns" ) );
    	lm_message_unref ( message );
    	lm_message_node_unref ( Node );
    	return KErrGeneral;
        }
    
    lm_message_unref ( message );
    lm_message_node_unref ( Node );
    
    iLog->Log ( _L ( "lm_message_node_set_attributes passed" ) );
    
    return KErrNone;
}





// ----------------------------------------------------------------------------
// Ctstlm::lm_message_node_set_get_raw_mode
// Description:Checks if the nodes values should be sent as raw mode
// Arguements :
//    node    : an LmMessageNode
// Returns    : TRUE if nodes value should be sent as is
//              FALSE if the value will be escaped before sending
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_node_set_get_raw_modeL ( CStifItemParser& /*aItem*/ )
{
	LmMessage     *message   = NULL;
	LmMessageNode *Node      = NULL;		
	
	iLog->Log ( _L ( "In lm_message_node_set_raw_mode" ) );
	
	// Construct a message with type = LM_MESSAGE_TYPE_MESSAGE
	iLog->Log ( _L ( "Construct a message" ) );
		
	message = lm_message_new ( NULL, LM_MESSAGE_TYPE_MESSAGE );
	if ( !message )
	    {
		iLog->Log ( _L ( "lm_message_new failed" ) );
		return KErrGeneral;
	    }
	
	// Retrive the root node from message
	iLog->Log ( _L ( "Retrive the root node from message" ) );	
	Node = lm_message_get_node ( message );
	if ( !Node )
	    {
		iLog->Log ( _L ( "lm_message_get_node failed" ) );
		lm_message_unref ( message );
		return KErrGeneral;		
	    }
	
	// Set the raw mode to TRUE
	iLog->Log ( _L ( "Set the node to raw mode" ) );
	lm_message_node_set_raw_mode ( Node, TRUE );
	
	// Check if the Node is in raw mode or not after setting
	iLog->Log ( _L ( "Check for raw mode after setting" ) );
	if ( !lm_message_node_get_raw_mode ( Node ) )
	    {
		iLog->Log ( _L ( "get_raw_mode returned FALSE after setting raw mode" ) );
		lm_message_unref ( message );
		lm_message_node_unref ( Node );		
		return KErrGeneral;
	    }
	
	// Set the raw mode to FALSE
	iLog->Log ( _L ( "Unset the raw mode" ) );
	lm_message_node_set_raw_mode ( Node, FALSE );
	
	// Check for raw mode
	iLog->Log ( _L ( "Check for raw mode after unsetting" ) );
	if ( lm_message_node_get_raw_mode ( Node ) )
	    {
		iLog->Log ( _L ( "get_raw_mode returned TRUE after unsetting" ) );
		lm_message_unref ( message );
		lm_message_node_unref ( Node );		
		return KErrGeneral;
	    }
		
	lm_message_unref ( message );
	lm_message_node_unref ( Node );	
	
	iLog->Log ( _L ( "lm_message_node_set_raw_mode passed" ) );
	 
	return KErrNone;    
}


// ----------------------------------------------------------------------------
// Ctstlm::lm_message_node_ref
// Description:Adds a reference to node
// Arguements :
//    node    : an LmMessageNode
// Returns    : the node
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_node_refL ( CStifItemParser& /*aItem*/ )
{
    LmMessage     *message   = NULL;
	LmMessageNode *Node      = NULL;		
	
	iLog->Log ( _L ( "In lm_message_node_ref" ) );
	
	// Construct a message with type = LM_MESSAGE_TYPE_MESSAGE
	iLog->Log ( _L ( "Construct a message" ) );
		
	message = lm_message_new ( NULL, LM_MESSAGE_TYPE_MESSAGE );
	if ( !message )
	    {
		iLog->Log ( _L ( "lm_message_new failed" ) );
		return KErrGeneral;
	    }
	
	// Retrive the root node from message
	iLog->Log ( _L ( "Retrive the root node from message" ) );	
	Node = lm_message_get_node ( message );
	if ( !Node )
	    {
		iLog->Log ( _L ( "lm_message_get_node failed" ) );
		lm_message_unref ( message );
		return KErrGeneral;		
	    }	
	    
	// Add a reference to  node
	lm_message_node_ref ( Node );
	
	// Loudmouth dont have any API to check reference on message node
	// Therefore lm_message_node_ref is considered as passed without checking	
	lm_message_unref ( message );
	lm_message_node_unref ( Node );	
	
	iLog->Log ( _L ( "lm_message_node_ref passed" ) );
	
	return KErrNone;
}


// ----------------------------------------------------------------------------
// Ctstlm::lm_message_node_unref
// Description:  Removes a reference from the the node. When no more 
//             reference are present, the node is freed. When freed 
//             lm_message_node_unref() will be called on all children.
//               Check the size of the heap just to verify whether node 
//             is freed or not when no more references to the node
// Arguements :
//    node    : an LmMessageNode
// Returns    : Node
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_node_unrefL ( CStifItemParser& /*aItem*/ )
{
    LmMessage     *message     = NULL;
	LmMessageNode *Node        = NULL;
	
	RHeap&         heap_handle = User::Heap();
	TInt           largest_free_block;
	TInt           heap_avail_before;
	TInt           heap_avail_after; 		
	
	iLog->Log ( _L ( "In lm_message_node_unref" ) );
	
	// Get the heap size before constructing a message 
	heap_avail_before = heap_handle.Available ( largest_free_block );
	
	// Construct a message with type = LM_MESSAGE_TYPE_MESSAGE
	iLog->Log ( _L ( "Construct a message" ) );
		
	message = lm_message_new ( NULL, LM_MESSAGE_TYPE_MESSAGE );
	if ( !message )
	    {
		iLog->Log ( _L ( "lm_message_new failed" ) );
		return KErrGeneral;
	    }
	
	// Retrive the root node from message
	iLog->Log ( _L ( "Retrive the root node from message" ) );	
	Node = lm_message_get_node ( message );
	if ( !Node )
	    {
		iLog->Log ( _L ( "lm_message_get_node failed" ) );
		lm_message_unref ( message );
		return KErrGeneral;		
	    }	
	    
	// Remove reference from the message
	iLog->Log ( _L ( "Remove reference from the message" ) );
	lm_message_unref ( message );
	
	// Remove reference from the node
	iLog->Log ( _L ( "Remove a reference from the node" ) );
	lm_message_node_unref ( Node );
	
	// Get the heap size after removing references
	heap_avail_after = heap_handle.Available ( largest_free_block );
	
	// Verify the heap size
	if ( heap_avail_after != heap_avail_before )
	    {
		iLog->Log ( _L ( "heap_avail_before = %d, heap_avail_after = %d" ),
		                  heap_avail_before, heap_avail_after );
		iLog->Log ( _L ( "Available memory is not as expected after freeing" ) );
		return KErrGeneral;
	    }
	
	iLog->Log ( _L ( "Expected: heap_available_before is equal to heap_available_after" ) );
	iLog->Log ( _L ( "heap_avail_before = %d, heap_avail_after = %d" ),
	                  heap_avail_before, heap_avail_after );
		
	return KErrNone;
}


// ----------------------------------------------------------------------------
// Ctstlm::lm_message_node_to_string
// Description:Returns an XML string representing the node. This is what
//             is sent over the connection. This is used internally by 
//             loudmouth and is external for debugging  purposes
// Arguements :
//    node    : an LmMessageNode
// Returns    : an XML string representation of node
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_node_to_stringL ( CStifItemParser& /*aItem*/ )
{
	LmMessage     *message     = NULL;	
	gchar         *XmlString      = NULL;
	
	
	iLog->Log ( _L ( "In lm_message_node_to_string" ) );
	
	// Construct a message with type = LM_MESSAGE_TYPE_PRESENCE
	iLog->Log ( _L ( "Construct a message with type=presence" ) );
		
	message = lm_message_new ( NULL, LM_MESSAGE_TYPE_PRESENCE );
	if ( !message )
	    {
		iLog->Log ( _L ( "lm_message_new failed for type = PRESENCE" ) );
		return KErrGeneral;
	    }
	
	// Convert node to XML string
	XmlString = lm_message_node_to_string ( message->node );
	
	// Check in XML string for <presence> stanza
	if ( !g_strrstr ( XmlString, "presence" ) )
	{
		iLog->Log ( _L ( "XML string doesnt contain <presence> stanza" ) );
		lm_message_unref ( message );
		return KErrGeneral;
	}
	
	// Construct a message with type=LM_MESSAGE_TYPE_IQ
	lm_message_unref ( message );
	message = lm_message_new ( NULL, LM_MESSAGE_TYPE_IQ );
	if ( !message )
	{
		iLog->Log ( _L ( "lm_message_new failed for type = IQ" ) );
		return KErrGeneral;
	}
	
	// Convert the message node to XML string
	XmlString = lm_message_node_to_string ( message->node );
	
	// Check in XML string for <iq> stanza
	if ( !g_strrstr ( XmlString, "iq" ) )
	{
		iLog->Log ( _L ( "XML string doesnt contain <iq> stanza" ) );
		lm_message_unref ( message );
		return KErrGeneral;
	}
	
	iLog->Log ( _L ( "lm_message_node_to_string passed" ) );
	lm_message_unref ( message );
	
	return KErrNone;  
}


// ---------------------------------------------------------------------------
// Ctstlm::lm_ssl_new
// Description: Creates a new ssl struct
// Arguements :
//      expected_fingerprint : The expected fingerprint. ssl_function will be
//                             called if there is a mismatch. NULL if you 
//                             are not interested in this check.
//      ssl_cb               : Callback called to inform the user of a problem
//                             during setting up the SSL connection and how 
//                             to proceed.
//      user_data            : Data sent with the callback
//      notify               : Function to free user_data when the connection
//                             is finished. 
// Returns    : A new LmSSL struct
// ---------------------------------------------------------------------------
//
TInt Ctstlm::lm_ssl_newL ( CStifItemParser& /*aItem*/ )
{
	LmSSL *ssl = NULL;
	
	iLog->Log ( _L ( "In lm_ssl_new" ) );
	
	ssl = lm_ssl_new ( NULL, ( LmSSLFunction ) ssl_cb, NULL, NULL );
	if ( !ssl )
		{
		iLog->Log ( _L ( "lm_ssl_new failed" ) );
		return KErrGeneral;
		}		
	
	lm_ssl_unref ( ssl );
	
	iLog->Log ( _L ( "lm_ssl_new passed" ) );
	
	return KErrNone;		
}

// ---------------------------------------------------------------------------
// Ctstlm::lm_ssl_is_supportedL
// Description: Checks wheather the Loudmouth supports SSL or not.
// Arguements : None
// Returns    : TRUE if loudmouth supports SSL, FALSE otherwise
// ---------------------------------------------------------------------------
//
TInt Ctstlm::lm_ssl_is_supportedL ( CStifItemParser& /*aItem*/ )
{	
	iLog->Log ( _L ( "In lm_ssl_is_supported" ) );
	
	if ( !lm_ssl_is_supported() )
		{
		iLog->Log ( _L ( "Loudmouth installation doesn't support SSL" ) );
		}
	else
		{
		iLog->Log ( _L ( "Loudmouth supports SSL" ) );		
		}
		
	iLog->Log ( _L ( "lm_ssl_is_supported passed" ) );
	
	return KErrNone;
}


// ---------------------------------------------------------------------------
// Ctstlm::lm_ssl_get_fingerprintL
// Description: Returns the MD5 fingerprint of the remote server's certificate.
// Arguements : 
//      ssl   : An LmSSL
// Returns    : A 16-byte array indicating fingerprint or NULL if unknow
// ---------------------------------------------------------------------------
//
TInt Ctstlm::lm_ssl_get_fingerprintL ( CStifItemParser& /*aItem*/ )
{
	LmSSL *ssl;	
	
	iLog->Log ( _L ( "In lm_ssl_get_fingerprint" ) );
	
	ssl = lm_ssl_new ( "--ssl", ( LmSSLFunction )ssl_cb, NULL, NULL );
	
	const gchar *fingerprint = lm_ssl_get_fingerprint ( ssl );
	if ( !fingerprint )
		{
		iLog->Log ( _L ( "Not able to get the remote server's certificate" ) );
		}
	
	iLog->Log ( _L ( "lm_ssl_get_fingerprint passed" ) );
	lm_ssl_unref ( ssl );
	
	return KErrNone;
}


// ---------------------------------------------------------------------------
// Ctstlm::lm_ssl_ref
// Description: Adds a reference to ssl
// Arguements : 
//      ssl   : An LmSSL
// Returns    : LmSSL
// ---------------------------------------------------------------------------
//
TInt Ctstlm::lm_ssl_refL ( CStifItemParser& /*aItem*/ )
{
	LmSSL *ssl;
	
	iLog->Log ( _L ( "In lm_ssl_ref" ) );
	
	ssl = lm_ssl_new ( NULL, NULL, NULL, NULL );
	if ( !ssl )
		{
		iLog->Log ( _L ( "lm_ssl_new failed" ) );
		return KErrGeneral;
		}
	
	// Add a reference to ssl
	lm_ssl_ref ( ssl );
	
	// Loudmouth don't have any API to get the reference to ssl.
	// Therefore this test case is considered as passed without verifying it	
	lm_ssl_unref ( ssl );
	
	iLog->Log ( _L ( "lm_ssl_ref passed" ) );
	return KErrNone;	
}


// ---------------------------------------------------------------------------
// Ctstlm::lm_ssl_unref
// Description: Removes a reference from ssl. 
//              When no more references are present, ssl is freed.
//              Check the heap size to ensure that lm_ssl_unref frees the ssl
//              when no more references are present.
// Arguements : 
//      ssl   : An LmSSL
// Returns    : None
// ---------------------------------------------------------------------------
//
TInt Ctstlm::lm_ssl_unrefL ( CStifItemParser& /*aItem*/ )
{	
	RHeap& heap_handle = User::Heap();
	TInt   largest_free_block;
	TInt   heap_avail_before;
	TInt   heap_avail_after;
	
	LmSSL  *ssl = NULL;
	
	iLog->Log ( _L ( "In lm_ssl_unref" ) );
	
	// Get the heap size before creating a new ssl
	heap_avail_before = heap_handle.Available ( largest_free_block );
	
	// Create a new ssl
	ssl = lm_ssl_new ( NULL, NULL, NULL, NULL );
	if ( !ssl )
		{
		iLog->Log ( _L ( "lm_ssl_new failed" ) );
		return KErrGeneral;
		}
	
	// Remove a reference from the ssl
	lm_ssl_unref ( ssl );
	
	// Get the heap size after freeing memory
	heap_avail_after = heap_handle.Available ( largest_free_block );
	
	// Compare the heap size
	if ( heap_avail_after != heap_avail_before )
		{
		iLog->Log ( _L ( "heap_avail_before = %d, heap_avail_after = %d" ),
		                  heap_avail_before, heap_avail_after );
		iLog->Log ( _L ( "Available memory is not as expected after unref" ) );
		return KErrGeneral;
		}
	
	iLog->Log ( _L ( "Expected: heap_avail_before is equal to heap_avail_after" ) );
	iLog->Log ( _L ( "heap_avail_before = %d, heap_avail_after = %d" ),
	                  heap_avail_before, heap_avail_after );	
	
	return KErrNone;	
}
    

// ---------------------------------------------------------------------------
// Ctstlm::lm_proxy_new
// Description:Creates a new proxy
// Arguements :
//    type    : proxy type (LM_PROXY_TYPE_NONE/LM_PROXY_TYPE_HTTP)
// Returns    : a newly created proxy       
// ---------------------------------------------------------------------------
//
TInt Ctstlm::lm_proxy_newL ( CStifItemParser& /*aItem*/ )
{
	LmProxy     *proxy = NULL;
	LmProxyType Type;
	
	iLog->Log ( _L ( "In lm_proxy_new" ) );
	
	iLog->Log ( _L ( "Create a new proxy" ) );	
	proxy = lm_proxy_new ( LM_PROXY_TYPE_HTTP );
	if ( !proxy )
	{
		iLog->Log ( _L ( "lm_proxy_new failed" ) );
		lm_proxy_unref ( proxy );
		return KErrGeneral;
	}
	
	// Verify the proxy type
	iLog->Log ( _L ( "Proxy type verification" ) );
	
	Type = lm_proxy_get_type ( proxy );
	if ( LM_PROXY_TYPE_HTTP != Type )
	{
		iLog->Log ( _L ( "lm_proxy_new failed" ) );
		lm_proxy_unref ( proxy );
		return KErrGeneral;
	}
		
	lm_proxy_unref ( proxy );
	
	iLog->Log ( _L ( "lm_proxy_new passed" ) );
	
	return KErrNone;	
}


// ---------------------------------------------------------------------------
// Ctstlm::lm_proxy_new_with_server
// Description:Creates a new proxy
// Arguements :
//   type    : proxy type (LM_PROXY_TYPE_NONE/LM_PROXY_TYPE_HTTP)
//   server  : proxy server
//   port    : proxy server port
// Returns   : a newly created proxy       
// ---------------------------------------------------------------------------
//
TInt Ctstlm::lm_proxy_new_with_serverL ( CStifItemParser& /*aItem*/ )
{
	LmProxy     *proxy  = NULL;	
	guint       port    = 8000;
	const gchar *server = "nokia.com";
	
	
	iLog->Log ( _L ( "In lm_proxy_new_with_server" ) );
	// Create a new proxy with the server
	proxy = lm_proxy_new_with_server ( LM_PROXY_TYPE_HTTP, server, port );
	if ( !proxy )
	    {
		iLog->Log ( _L ( "lm_proxy_new_with_server failed" ) );		
		return KErrGeneral;
	    }
	
	// Verify proxy type, server and port
	iLog->Log ( _L ( "Verify server, type and port in proxy" ) );
	
	if ( LM_PROXY_TYPE_HTTP != lm_proxy_get_type ( proxy ) )
	    {
		iLog->Log ( _L ( "lm_proxy_new_with_server failed for proxy type setting" ) );		
		lm_proxy_unref ( proxy );
		return KErrGeneral;
	    }
	
	if ( strcmp ( server, lm_proxy_get_server ( proxy ) ) )
	    {
		iLog->Log ( _L ( "lm_proxy_new_with_server failed for server setting" ) );		
		lm_proxy_unref ( proxy );
		return KErrGeneral;
	    }
	
	if ( port != lm_proxy_get_port ( proxy ) )
	    {
		iLog->Log ( _L ( "lm_proxy_new_with_server failed for port setting" ) );		
		lm_proxy_unref ( proxy );
		return KErrGeneral;
	    }
	
	lm_proxy_unref ( proxy );	
	
	iLog->Log ( _L ( "lm_proxy_new_with_server passed" ) );	
	
	return KErrNone;
}



// ---------------------------------------------------------------------------
// Ctstlm::lm_proxy_set_get_type
// Description:Sets the proxy type for proxy to type
// Arguements :
//       proxy    : an LmProxy
//       type     : proxy type
// Returns    : None
// ---------------------------------------------------------------------------
//
TInt Ctstlm::lm_proxy_set_get_typeL ( CStifItemParser& /*aItem*/ )
{
	LmProxy     *proxy = NULL;	
	
	iLog->Log ( _L ( "In lm_proxy_set_get_type" ) );
	
	// Create a new proxy with type=LM_PROXY_TYPE_NONE
	proxy = lm_proxy_new ( LM_PROXY_TYPE_NONE );
	if ( !proxy )
	    {
		iLog->Log ( _L ( "lm_proxy_new failed" ) );
		return KErrGeneral;
	    }
	
	// Set the proxy type=LM_PROXY_TYPE_HTTP
	lm_proxy_set_type ( proxy, LM_PROXY_TYPE_HTTP );
	
	// Verify the proxy type for LM_PROXY_TYPE_HTTP		
	if ( LM_PROXY_TYPE_HTTP != lm_proxy_get_type ( proxy ) )
	    {
		iLog->Log ( _L ( "lm_proxy_set_type failed" ) );
		lm_proxy_unref ( proxy );
		return KErrGeneral;
	    }	
		
	lm_proxy_unref ( proxy );
	
	iLog->Log ( _L ( "lm_proxy_set_get_type passed" ) );
	
	return KErrNone;
}



// ---------------------------------------------------------------------------
// Ctstlm::lm_proxy_set_get_server
// Description:Sets the server address for proxy
// Arguements :
//   proxy    : an LmProxy
//   server   : address of the proxy server
// Returns    : None
// ---------------------------------------------------------------------------
//
TInt Ctstlm::lm_proxy_set_get_serverL ( CStifItemParser& /*aItem*/ )
{
	LmProxy     *proxy  = NULL;
	const gchar *server = "nokia.com";	
	
	iLog->Log ( _L ( "In lm_proxy_get_server" ) );
	
	// Create a new proxy with type = LM_PROXY_TYPE_HTTP
	proxy = lm_proxy_new ( LM_PROXY_TYPE_HTTP );
	if ( !proxy )
	    {
		iLog->Log ( _L ( "lm_proxy_new failed" ) );
		return KErrGeneral;
	    }
	
	// Set the proxy server to "nokia.com"
	lm_proxy_set_server ( proxy, server );
	
	// Verify the server address in proxy
	if ( strcmp ( server, lm_proxy_get_server ( proxy ) ) )
	    {
		iLog->Log ( _L ( "lm_proxy_set_server failed" ) );
		lm_proxy_unref ( proxy );
		return KErrGeneral;
	    }
	
	lm_proxy_unref ( proxy );
	
	iLog->Log ( _L ( "lm_proxy_set_server passed" ) );
	
	return KErrNone;	
}


// ---------------------------------------------------------------------------
// Ctstlm::lm_proxy_set_port
// Description:Sets the server port that proxy will be using
// Arguements :
//    proxy   : an LmProxy
//    port    : proxy server port
// Returns    : None
// ---------------------------------------------------------------------------
//
TInt Ctstlm::lm_proxy_set_get_portL ( CStifItemParser& /*aItem*/ )
{
	LmProxy *proxy = NULL;
	guint   port1  = 80;
	guint   port2  = 21;
	
	iLog->Log ( _L ( "In lm_proxy_set_get_port" ) );
	
	// Create new proxy
	proxy = lm_proxy_new ( LM_PROXY_TYPE_HTTP );
	if ( !proxy )
	    {
		iLog->Log ( _L ( "lm_proxy_new failed" ) );
		return KErrGeneral;
	    }
	
	// Set the port = port1
	lm_proxy_set_port ( proxy, port1 );
	
	// Verify the port
	if ( port1 != lm_proxy_get_port ( proxy ) )
	    {
		iLog->Log ( _L ( "lm_proxy_set_port failed for port = %d" ), port1 );
		lm_proxy_unref ( proxy );
		return KErrGeneral;
	    }
	
	// Set the port = port2
	lm_proxy_set_port ( proxy, port2 );
	
	// Verify the port
	if ( port2 != lm_proxy_get_port ( proxy ) )
	    {
		iLog->Log ( _L ( "lm_proxy_set_port failed for port = %d" ), port2 );
		lm_proxy_unref ( proxy );
		return KErrGeneral;
	    }
	
	lm_proxy_unref ( proxy );
	
	iLog->Log ( _L ( "lm_proxy_set_get_port passed" ) );
	
	return KErrNone;
}



// ---------------------------------------------------------------------------
// Ctstlm::lm_proxy_set_username
// Description:Sets the username for proxy or NULL to unset
// Arguements :
//   proxy    : an LmProxy
//   username : username
// Returns    : None
// ---------------------------------------------------------------------------
//
TInt Ctstlm::lm_proxy_set_get_usernameL ( CStifItemParser& /*aItem*/ )
{
	LmProxy     *proxy = NULL;
	const gchar *user1 = "nokia";
	const gchar *user2 = "linux";
	
	iLog->Log ( _L ( "In lm_proxy_set_get_username" ) );
	
	// Create a new proxy
	proxy = lm_proxy_new ( LM_PROXY_TYPE_HTTP );
	if ( !proxy )
	    {
		iLog->Log ( _L ( "lm_proxy_new failed" ) );
		return KErrGeneral;
	    }
	
	// Set the proxy username to "nokia"
	lm_proxy_set_username ( proxy, user1 );
	
	// Verify the proxy username
	if ( strcmp ( user1, lm_proxy_get_username ( proxy ) ) )
	    {
		iLog->Log ( _L ( "lm_proxy_set_username failed for user1" ) );
		lm_proxy_unref ( proxy );
		return KErrGeneral;
	    }
	
	// Set the proxy username to "linux"
	lm_proxy_set_username ( proxy, user2);
	
	// Verify the proxy username
	if ( strcmp ( user2, lm_proxy_get_username ( proxy ) ) )
	    {
		iLog->Log ( _L ( "lm_proxy_set_username failed for user2" ) );
		lm_proxy_unref ( proxy );
		return KErrGeneral;
	    }
	
	lm_proxy_unref ( proxy );
	
	iLog->Log ( _L ( "lm_proxy_set_get_username passed" ) );
	
	return KErrNone;
}



// ---------------------------------------------------------------------------
// Ctstlm::lm_proxy_set_password
// Description:Sets the password for proxy
// Arguements :
//   proxy    : an LmProxy
// Returns    : the proxy password
// ---------------------------------------------------------------------------
//
TInt Ctstlm::lm_proxy_set_get_passwordL ( CStifItemParser& /*aItem*/ )
{
	LmProxy     *proxy = NULL;
	const gchar *password = "nokia";
	
	iLog->Log ( _L ( "In lm_proxy_set_get_passrod" ) );
	
	// Create a new proxy
	proxy = lm_proxy_new ( LM_PROXY_TYPE_HTTP );
	if ( !proxy )
	    {
		iLog->Log ( _L ( "lm_proxy_new failed" ) );
		return KErrGeneral;
	    }
	
	// Set the password proxy to "nokia"
	lm_proxy_set_password ( proxy, password );
	
	// Verify the password
	if ( strcmp ( password, lm_proxy_get_password ( proxy ) ) )
	    {
		iLog->Log ( _L ( "lm_proxy_set_password failed" ) );
		lm_proxy_unref ( proxy );
		return KErrGeneral;
	    }
	
	// Unset the password
	lm_proxy_set_password ( proxy, NULL );
	
	// Verify the password
	if ( lm_proxy_get_password ( proxy ) )
	    {
		iLog->Log ( _L ( "lm_proxy_set_password failed" ) );
		lm_proxy_unref ( proxy );
		return KErrGeneral;
	    }
	
	iLog->Log ( _L ( "lm_proxy_set_password passed" ) );
	lm_proxy_unref ( proxy );
	
	return KErrNone; 
}


// ---------------------------------------------------------------------------
// Ctstlm::lm_proxy_ref
// Description:Adds a reference to proxy
// Arguements :
//   proxy    : an LmProxy
// Returns    : the proxy
// ---------------------------------------------------------------------------
//
TInt Ctstlm::lm_proxy_refL ( CStifItemParser& /*aItem*/ )
{
	LmProxy *proxy = NULL;
	
	iLog->Log ( _L ( "In lm_proxy_ref" ) );
	
	// Creat a new proxy
	proxy = lm_proxy_new ( LM_PROXY_TYPE_HTTP );
	if ( !proxy )
	{
		iLog->Log ( _L ( "lm_proxy_new failed" ) );
		return KErrGeneral;
	}
	
	// Add a reference to proxy
	proxy = lm_proxy_ref ( proxy );
	
	// Loudmouth don't provide API for getting reference to the proxy
	// Therefore this test case is considered as passed without checking
	iLog->Log ( _L ( "lm_proxy_ref passed" ) );
	lm_proxy_unref ( proxy );
	lm_proxy_unref ( proxy );
	
	return KErrNone;
}


// ---------------------------------------------------------------------------
// Ctstlm::lm_proxy_unref
// Description:Removes a reference from the proxy
//             When no more rererences are present proxy is freed
//             Check the heap size to ensure that lm_proxy_unref free 
//             the proxy when no more references are present.
// Arguements :
//   proxy    : an LmProxy
// Returns    : None
// ---------------------------------------------------------------------------
//
TInt Ctstlm::lm_proxy_unrefL ( CStifItemParser& /*aItem*/ )
{
	LmProxy *proxy      = NULL;
	
	RHeap&  heap_handle = User::Heap();
	TInt    largest_free_block;
	TInt    heap_avail_before;
	TInt    heap_avail_after;
	
	iLog->Log ( _L ( "In lm_proxy_unref" ) );
	
	// Get the heap size before creating a new proxy
	heap_avail_before = heap_handle.Available ( largest_free_block );
	
	// Create a new proxy
	proxy = lm_proxy_new ( LM_PROXY_TYPE_HTTP );
	if ( !proxy )
	{
	    iLog->Log ( _L ( "lm_proxy_new failed" ) );
	    return KErrGeneral;
	}
	
	// Remove a reference from the proxy
	lm_proxy_unref ( proxy );
	
	// Get the heap size 
	heap_avail_after = heap_handle.Available ( largest_free_block );
	
	// Compare the heap size
	if ( heap_avail_after != heap_avail_before )
	{
		iLog->Log ( _L ( "heap_available_before = %d, heap_available_after = %d" ),
		                  heap_avail_before, heap_avail_after );
		iLog->Log ( _L ( "Available memory is not as expected after freeing" ) );
		return KErrGeneral;	
	}
	
	iLog->Log ( _L ( "Expected: heap_available_before is equal to heap_available_after" ) );
	iLog->Log ( _L ( "heap_available_before = %d, heap_available_after = %d" ),
	                  heap_avail_before, heap_avail_after );
	iLog->Log ( _L ( "lm_message_unref passed" ) );
	
	return KErrNone;	
}
	
//-----------------------------------------------------------------------------
// Ctstlm::lm_send_sync
// Description	: Opens a connection with the gtalk server and sends a jabber
//                message to the another person  
// Requires		: This test case requires GUI mode for selecting Access point
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_send_syncL ( CStifItemParser& aItem )
    {
	LmConnection *connection  = NULL;
	LmMessage    *message     = NULL;
	
	ConnectData *connect_data = NULL;
	
	GError      *error        = NULL;
	
	iLog->Log ( _L ( "In lm_send_sync" ) );	
	
	// Read data from the CFG file
	iLog->Log ( _L ( "Reading data from the CFG file" ) );	
	if ( read_data ( connect_data, aItem ) != RC_OK )
	    {
	    iLog->Log ( _L ( "read_data failed" ) );
	    free_data ( connect_data );
		return KErrGeneral;
	    }
	iLog->Log ( _L ( "Read data from the CFG file" ) );
	iLog->Log ( _L8 ( "Server name: %s" ), connect_data->server );
	
	// Open a new closed connection
	iLog->Log ( _L ( "Open a new closed connection" ) );
	connection = lm_connection_new ( connect_data->server );	
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "New closed connection opened" ) );
	
	iLog->Log ( _L ( "Setting jid" ) );	
	lm_connection_set_jid ( connection, connect_data->username );
	
	iLog->Log ( _L ( "Setting gtalks SSL port" ) );
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Proxy settings for emulator
#ifdef __WINSCW__
    SetProxy ( connection, connect_data->proxy_data );
#endif

	// Set connection to use SSL
	iLog->Log ( _L ( "Setting SSL for the connection" ) );
	SSLInit ( connection );
	
	iLog->Log ( _L ( "Open a connection with the server" ) );
	//Calling connection open
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop ); 
	
	iLog->Log ( _L ( "connection with the server successfull" ) );
	
	iLog->Log ( _L ( "Getting the username from the server" ) );
	iLog->Log ( _L8 ( "jid: %s" ), connect_data->username );
	// Get the user from the full JID
	gchar *username = get_user_name ( connect_data->username );
	iLog->Log ( _L ( "after extracting from the jid: %s" ), username );
	
	
	iLog->Log ( _L ( "Authenticating with the server" ) );
	
	main_loop = g_main_loop_new ( NULL, FALSE );
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction )connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	g_free ( username );
	iLog->Log ( _L ( "Authentication done" ) );
	
	// Send a message to the server
	iLog->Log ( _L8 ( "Create a new message: %s" ),
	                   connect_data->msg_data->recipient );
	message = lm_message_new ( connect_data->msg_data->recipient, 
	                           LM_MESSAGE_TYPE_MESSAGE );
	if ( !message )
		{
		iLog->Log ( _L ( "lm_message_new failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	iLog->Log ( _L ( "Add a node" ) );	
	lm_message_node_add_child ( message->node, "body", 
	                            connect_data->msg_data->message );
	
	
	iLog->Log ( _L ( "Send message to the server" ) );
	if ( !lm_connection_send ( connection, message, NULL ) )
		{
		iLog->Log ( _L ( "lm_connection_send failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "Message has been sent" ) );
	
	iLog->Log ( _L ( "Free the allocated resources" ) );
	lm_connection_close ( connection, NULL );
	lm_connection_unref ( connection );	
	lm_message_unref ( message );
	free_data ( connect_data );
	iLog->Log ( _L ( "Freed" ) );

	iLog->Log ( _L ( "lm_send_sync passed" ) );
	
	return KErrNone;	
    }
	
 //-----------------------------------------------------------------------------
// Ctstlm::lm_is_authenticated_WhenNotAuthenticatedL
// Description  : Check if connection is authenticated or not
// Arguements   :
//   connection	: an LmConnection to check 
// Returns      : TRUE if connection is authenticated, FALSE otherwise
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_is_authenticated_WhenNotAuthenticatedL ( CStifItemParser& aItem )
    {
    LmConnection *connection       = NULL;
    ConnectData  *connect_data     = NULL;
    gboolean     NotAuthenticated  = FALSE;    
    
    iLog->Log ( _L ( "In lm_connection_is_authenticated" ) );
    
    // Read data from the CFG file
    if ( read_data ( connect_data, aItem ) != RC_OK )
    	{
    	iLog->Log ( _L ( "read_data failed" ) );
    	free_data ( connect_data );
    	return KErrGeneral;
    	}    
    
    // Open a new closed connection
    connection = lm_connection_new ( connect_data->server );
    if ( !connection )
    	{
    	iLog->Log ( _L ( "lm_connection_new failed" ) );
    	free_data ( connect_data );
    	return KErrGeneral;
    	}
    
    // Set the connection to use gtalk's SSL port
    lm_connection_set_port ( connection, GTALK_SSL_PORT );
    
    // Set the JID to be used for connection 
    lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif

	// Set the connection to use SSL
	SSLInit ( connection );
	
    // Open a connection
    GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop ); 
	
    // Check for authentication after the connection is authenticated
    if ( lm_connection_is_authenticated ( connection ) )
        {
    	iLog->Log ( _L ( "lm_is_authenticated_WhenNotAuthenticatedL failed" ) );    	
    	lm_connection_close ( connection, NULL );
    	lm_connection_unref ( connection );
    	free_data ( connect_data );
    	return KErrGeneral;
        }    
    
    lm_connection_close ( connection, NULL );
    lm_connection_unref ( connection );
    free_data ( connect_data );
    
    iLog->Log ( _L ( "lm_is_authenticated_WhenNotAuthenticatedL passed" ) );
    
    return KErrNone;  
    }



//--------------------------------------------------------------------------------
// function_name    : handle_fetch_contactlist_messages
// description      : callback function to handle messages
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_fetch_contactlist_messages ( LmMessageHandler* /*handler*/,
                  LmConnection*     /*connection*/,
                  LmMessage*        reply,
                  gpointer          user_data )
    {
    GMainLoop *main_loop = ( GMainLoop * )user_data;	
	LmMessageSubType  type;
	LmMessageNode *q_node,*item_node;
	type = lm_message_get_sub_type (reply); 
     	
       	
	switch (type) 
    	{
    	case LM_MESSAGE_SUB_TYPE_RESULT:        		        		
    		break;
    	case LM_MESSAGE_SUB_TYPE_ERROR:          	          		
    		//return NULL;
    		break;
    //	default:
    	//	g_assert_not_reached ();
    	//	break;
    	} 
    
   	q_node = lm_message_node_get_child (reply->node, "query");

	if (!q_node) 
    	{
   		//return NULL;
    	}
    	
    item_node = lm_message_node_get_child (q_node, "item");

	while (item_node) 
    	{          	
    	const char* reply_string;
        reply_string = lm_message_node_get_attribute(item_node,"jid");  
        char* friendname;
        const char* atstring = "@";
        friendname = strtok((char*)reply_string,atstring );
        item_node = item_node->next;  
     	}  
	
	
	if ( main_loop )
		{
		g_main_loop_quit ( main_loop );
		}	
	
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    }
    
//-----------------------------------------------------------------------------
// Ctstlm::lm_fetching_contactlistL
// Description  : fetching contact list 
// Arguements   :
//   connection	: 
//   message    : 
//   error      : 
// Returns      : 
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_fetching_contactlist_L ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message;
	LmMessageNode *q_node;
    //LmMessageSubType  type;
    LmMessageHandler *handler = NULL;
	
	iLog->Log ( _L ( "In lm_fetching_contactlist_L" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );

	//Open call    
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		free_data ( connect_data );	
		g_main_loop_unref ( main_loop );	
		return KErrGeneral;
	    }
		
	
	g_main_loop_run ( main_loop );

	g_main_loop_unref ( main_loop );    
    

	main_loop = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction )connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	g_free ( username );
	

                
	// Construct a message
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type (NULL, LM_MESSAGE_TYPE_IQ,
    					  LM_MESSAGE_SUB_TYPE_GET);
    q_node = lm_message_node_add_child (message->node, "query", NULL);
    lm_message_node_set_attributes (q_node,
    					"xmlns", "jabber:iq:roster",
    					NULL); 
	// Send message to the server
	handler = lm_message_handler_new ( handle_fetch_contactlist_messages, main_loop, NULL );
	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );                					
    					
   				
	
	// Close the connection
	lm_connection_close ( connection, NULL );
	
	// Remove a reference on connection
	lm_connection_unref ( connection );
	
	// Remove a reference on message
	lm_message_unref ( message );
	
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_fetching_contactlist_L passed" ) );
	
	return KErrNone;
    }


//-----------------------------------------------------------------------------
// Ctstlm::lm_fetching_presenceL
// Description  : fetching contact list 
// Arguements   :
//   connection	: 
//   message    : 
//   error      : 
// Returns      : 
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_fetching_presenceL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message;
	//LmMessageNode *q_node;
   // LmMessageSubType  type;
    LmMessageHandler *handler = NULL;
    GMainLoop    *main_loop    = NULL;
	
	iLog->Log ( _L ( "In lm_fetching_presenceL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	
	main_loop = g_main_loop_new ( NULL, FALSE );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	
	g_main_loop_run ( main_loop );

	g_main_loop_unref ( main_loop );    

	main_loop = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	g_free ( username );
	
	
	message = lm_message_new_with_sub_type ( NULL,
	                                     LM_MESSAGE_TYPE_PRESENCE,
	                                     LM_MESSAGE_SUB_TYPE_AVAILABLE );
	
    gboolean result = lm_connection_send ( connection, message, NULL );	
   	// Close the connection
	main_loop = g_main_loop_new ( NULL, FALSE );
	
	//------------------------------------------------------------------------
	//
	// Register a handler to recieve and update presence information
	//
	handler = lm_message_handler_new ( 
	                     (LmHandleMessageFunction)jabber_presence_handler_cb,
	                     main_loop,
	                     NULL );
	
	lm_connection_register_message_handler ( connection,
	                                         handler,
	                                         LM_MESSAGE_TYPE_PRESENCE,
	                                         LM_HANDLER_PRIORITY_NORMAL );
	
	//------------------------------------------------------------------------
	
	g_main_loop_run ( main_loop );	
	g_main_loop_unref ( main_loop );
	
	lm_connection_close ( connection, NULL );	
	// Remove a reference on connection
	lm_connection_unref ( connection );	
	// Remove a reference on message
	lm_message_unref ( message );	
	free_data ( connect_data );
	lm_message_handler_unref(handler);
	
	// Message sending passed
	iLog->Log ( _L ( "lm_fetching_presenceL passed" ) );
	
	return KErrNone;
    }

//-----------------------------------------------------------------------------
// Ctstlm::lm_subscribe_contact_L
// Description  : fetching contact list 
// Arguements   :
//   connection	: 
//   message    : 
//   error      : 
// Returns      : 
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_subscribe_contact_L ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message;
	//LmMessageNode *q_node;
    //LmMessageSubType  type;
	
	iLog->Log ( _L ( "In lm_subscribe_contact_L" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	// Open a connection with the server
	GMainLoop	*main_loop1 = g_main_loop_new ( NULL, FALSE );
	
	if ( !lm_connection_open ( connection, 
								( LmResultFunction ) connection_open_cb,
	                           main_loop1, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop1 );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	
	
	g_main_loop_run ( main_loop1 );

	g_main_loop_unref ( main_loop1 );   

	
	GMainLoop	*main_loop2 = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop2,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop2 );
	g_main_loop_unref ( main_loop2 );    
	g_free ( username );
	
	
	gchar *jid = "rakesh.harsha@gmail.com";
    message = lm_message_new (jid, LM_MESSAGE_TYPE_PRESENCE );
    lm_message_node_set_attribute ( message->node, "type", "subscribe" );
    //Send the message					
   	if ( !lm_connection_send ( connection, message, NULL ) )
    	{
    	lm_message_unref ( message );
    	return KErrGeneral;
    	}
    
    lm_connection_close ( connection, NULL );	
	// Remove a reference on connection
	lm_connection_unref ( connection );	
	// Remove a reference on message
	lm_message_unref ( message );	
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_subscribe_contact_L passed" ) );	
	return KErrNone;
    }
    
    
    
 //-----------------------------------------------------------------------------
// Ctstlm::lm_authenticate_WithUnexistingJID
// Description  : Tries to authenticate a user against the server. 
// Arguements   :
//   connection : an LmConnection 
//   username   : Username used to authenticate
//   password   : Password corresponding to Username
//   resource   : Resource used for this connection
//   error      : location to store error, or NULL
// Returns      : TRUE if no errors were detected & authentication was 
//                successful,
//                FALSE otherwise
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_authenticate_WithUnexistingJID ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;	
	//ConnectData  *connect_data = NULL;
	AuthData     *auth_data    = NULL;
	//GMainLoop    *main_loop    = NULL;
	//GMainContext *context      = NULL;
		
	iLog->Log ( _L ( "In lm_connection_authenticate" ) );	

	auth_data = g_new0 ( AuthData, 1 );
	if ( !auth_data )
		{
		iLog->Log ( _L ( "memory allocation failed for auth_data" ) );
		return KErrGeneral;
		}	
		
	if ( read_data ( auth_data->connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );		
		free_data ( auth_data->connect_data );		
		g_free ( auth_data );
		return KErrGeneral;
		}
	
	auth_data->main_loop    = g_main_loop_new ( NULL, FALSE );	
	auth_data->rCode        = RC_ERROR;
	
	//connection = lm_connection_new_with_context ( connect_data->server,context );
	connection = lm_connection_new ( auth_data->connect_data->server );
	if ( connection == NULL )
	    {
	    iLog->Log ( _L ( "lm_connection_new_with_context failed" ) );
	    free_data ( auth_data->connect_data );
	    g_main_loop_unref ( auth_data->main_loop );
	    g_free ( auth_data );
	    return KErrGeneral;
	    }
	
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	lm_connection_set_jid ( connection, auth_data->connect_data->username );

#ifdef __WINSCW__
	SetProxy ( connection, auth_data->connect_data->proxy_data );
#endif
	SSLInit ( connection );
	
	if ( !lm_connection_open ( connection, 
	                           ( LmResultFunction ) connection_open_cb,
	                           auth_data->main_loop, 
	                           NULL, 
	                           NULL ) )                          
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ) );
		free_data ( auth_data->connect_data );
		g_main_loop_unref ( auth_data->main_loop );
		g_free ( auth_data );
		return KErrGeneral;
	    }	   
	                                    
	
	g_main_loop_run ( auth_data->main_loop );
	g_main_loop_unref ( auth_data->main_loop );
	
	// Extract the username from the JID
    gchar *username = get_user_name ( auth_data->connect_data->username );
    auth_data->main_loop    = g_main_loop_new ( NULL, FALSE );
    if ( !lm_connection_authenticate ( connection, 
                                       username,
                                       auth_data->connect_data->password,
                                       auth_data->connect_data->resource, 
                                       ( LmResultFunction ) wrong_input_auth_cb, 
                                       auth_data ,  
                                       NULL, 
                                       NULL ) )
        {
       	auth_data->rCode = RC_ERROR;
       	lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
       	g_free ( username );
       	g_main_loop_unref ( auth_data->main_loop );
       	free_data ( auth_data->connect_data );	
       	g_free ( auth_data );
       	return KErrGeneral;
        }
            
    g_main_loop_run ( auth_data->main_loop );
	g_main_loop_unref ( auth_data->main_loop );
	g_free ( username );
	
	
	
	lm_connection_close ( connection, NULL );
	lm_connection_unref ( connection );
	free_data ( auth_data->connect_data );	
	
	if ( auth_data->rCode == RC_ERROR )
	    {
	    iLog->Log ( _L ( "lm_authenticate_WithUnexistingJID failed" ) );
	    g_free ( auth_data );
	    return KErrGeneral;
	    }
	
	g_free ( auth_data );

	iLog->Log ( _L ( "lm_authenticate_WithUnexistingJID passed" ) );
	
	return KErrNone;		
}


//-----------------------------------------------------------------------------
// Ctstlm::lm_authenticate_WithBadPasswordL
// Description  : Tries to authenticate a user against the server. 
// Arguements   :
//   connection : an LmConnection 
//   username   : Username used to authenticate
//   password   : Password corresponding to Username
//   resource   : Resource used for this connection
//   error      : location to store error, or NULL
// Returns      : TRUE if no errors were detected & authentication was 
//                successful,
//                FALSE otherwise
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_authenticate_WithBadPasswordL ( CStifItemParser& aItem )
    {
		LmConnection *connection   = NULL;	
	//ConnectData  *connect_data = NULL;
	AuthData     *auth_data    = NULL;
	//GMainLoop    *main_loop    = NULL;
	//GMainContext *context      = NULL;
		
	iLog->Log ( _L ( "In lm_connection_authenticate" ) );	

	auth_data = g_new0 ( AuthData, 1 );
	if ( !auth_data )
		{
		iLog->Log ( _L ( "memory allocation failed for auth_data" ) );
		return KErrGeneral;
		}	
		
	if ( read_data ( auth_data->connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );		
		free_data ( auth_data->connect_data );		
		g_free ( auth_data );
		return KErrGeneral;
		}
	
	//context = g_main_context_new ();
	//main_loop = g_main_loop_new ( context, FALSE );	
	auth_data->main_loop    = g_main_loop_new ( NULL, FALSE );	
	//auth_data->connect_data = connect_data;
	auth_data->rCode        = RC_ERROR;
	//auth_data->main_loop    = main_loop;
	
	//connection = lm_connection_new_with_context ( connect_data->server,context );
	connection = lm_connection_new ( auth_data->connect_data->server );
	if ( connection == NULL )
	    {
	    iLog->Log ( _L ( "lm_connection_new_with_context failed" ) );
	    free_data ( auth_data->connect_data );
	    g_main_loop_unref ( auth_data->main_loop );
	    g_free ( auth_data );
	    return KErrGeneral;
	    }
	
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	lm_connection_set_jid ( connection, auth_data->connect_data->username );

#ifdef __WINSCW__
	SetProxy ( connection, auth_data->connect_data->proxy_data );
#endif
	SSLInit ( connection );
	
	if ( !lm_connection_open ( connection, 
	                           ( LmResultFunction ) auth_from_open_cb,
	                           auth_data, 
	                           NULL, 
	                           NULL ) )                          
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ) );
		free_data ( auth_data->connect_data );
		g_main_loop_unref ( auth_data->main_loop );
		g_free ( auth_data );
		return KErrGeneral;
	    }	   
	                                    
	
	g_main_loop_run ( auth_data->main_loop );
	g_main_loop_unref ( auth_data->main_loop );
	
	lm_connection_close ( connection, NULL );
	lm_connection_unref ( connection );
	free_data ( auth_data->connect_data );	
	
	if ( auth_data->rCode == RC_ERROR )
	    {
	    iLog->Log ( _L ( "lm_authenticate_WithBadPasswordL failed" ) );
	    g_free ( auth_data );
	    return KErrGeneral;
	    }
	
	g_free ( auth_data );

	iLog->Log ( _L ( "lm_authenticate_WithBadPasswordL passed" ) );
	
	return KErrNone;		
}
   
  //-----------------------------------------------------------------------------
// Ctstlm::lm_authenticate_WhenConnectionIsNotOpenedL
// Description  : Tries to authenticate a user against the server.handles 
// 				  a reply to the authentication in callback method
// Arguments    :
//   connection : an LmConnection 
//   username   : Username used to authenticate
//   password   : Password corresponding to Username
//   resource   : Resource used for this connection
//   error      : location to store error, or NULL
// Returns      : TRUE if no errors were detected & authentication was 
//                successful,
//                FALSE otherwise
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_authenticate_WhenConnectionIsNotOpenedL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;	
	ConnectData  *connect_data = NULL;
	
	
	iLog->Log ( _L ( "In lm_authenticate_WhenConnectionIsNotOpenedL" ) );
	
	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Open a new closed connection	
	connection = lm_connection_new ( connect_data->server );
	if ( connection == NULL)
	    {
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
	    }
	
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	lm_connection_set_jid ( connection, connect_data->username );
	
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	SSLInit ( connection );
	
		
	
	// Get the username from the JID
   GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server without doing a connection open
	if (! lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_open_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_authenticate_WhenConnectionIsNotOpened passed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );  
		//returning kerrnone since we are testing negative scenario and this call is 
		//expected to return false
		return KErrNone; 
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	g_free ( username );

	
	lm_connection_close ( connection, NULL );
	lm_connection_unref ( connection );
	free_data ( connect_data );
	
	iLog->Log ( _L ( "lm_authenticate_WhenConnectionIsNotOpened when connection is not opened failed since connection must be already open" ) );
	//Returning KErrNone, since the test case could not be tested as the connection must be open
	//The control will never come here for the test case :)
	return KErrNone;	
    }
    
    
 //-----------------------------------------------------------------------------
// Ctstlm::lm_connection_open_with_badserver server name
// Description  : Tries to authenticate a user against the wrong server. 
// Arguements   :
//   connection : an LmConnection 
//   username   : Username used to authenticate
//   password   : Password corresponding to Username
//   resource   : Resource used for this connection
//   error      : location to store error, or NULL
// Returns      : TRUE if no errors were detected & authentication was 
//                successful,
//                FALSE otherwise
//-----------------------------------------------------------------------------
//
TInt Ctstlm::lm_connection_open_with_badserverL ( CStifItemParser& aItem )
    {
		LmConnection *connection   = NULL;	
	//ConnectData  *connect_data = NULL;
	AuthData     *auth_data    = NULL;
	//GMainLoop    *main_loop    = NULL;
	//GMainContext *context      = NULL;
		
	iLog->Log ( _L ( "In lm_connection_open_with_badserverL" ) );	

	auth_data = g_new0 ( AuthData, 1 );
	if ( !auth_data )
		{
		iLog->Log ( _L ( "memory allocation failed for auth_data" ) );
		return KErrGeneral;
		}	
		
	if ( read_data ( auth_data->connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );		
		free_data ( auth_data->connect_data );		
		g_free ( auth_data );
		return KErrGeneral;
		}
	
	//context = g_main_context_new ();
	//main_loop = g_main_loop_new ( context, FALSE );	
	auth_data->main_loop    = g_main_loop_new ( NULL, FALSE );	
	//auth_data->connect_data = connect_data;
	auth_data->rCode        = RC_ERROR;
	//auth_data->main_loop    = main_loop;
	
	//connection = lm_connection_new_with_context ( connect_data->server,context );
	connection = lm_connection_new ( auth_data->connect_data->server );
	if ( connection == NULL )
	    {
	    iLog->Log ( _L ( "lm_connection_new failed" ) );
	    free_data ( auth_data->connect_data );
	    g_main_loop_unref ( auth_data->main_loop );
	    g_free ( auth_data );
	    return KErrGeneral;
	    }
	
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	lm_connection_set_jid ( connection, auth_data->connect_data->username );

#ifdef __WINSCW__
	SetProxy ( connection, auth_data->connect_data->proxy_data );
#endif
	SSLInit ( connection );
	
	if ( !lm_connection_open ( connection, 
	                           ( LmResultFunction ) wrong_input_in_open_cb,
	                           auth_data, 
	                           NULL, 
	                           NULL ) )                          
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ) );
		free_data ( auth_data->connect_data );
		g_main_loop_unref ( auth_data->main_loop );
		g_free ( auth_data );
		return KErrGeneral;
	    }	   
	                                    
	
	g_main_loop_run ( auth_data->main_loop );
	iLog->Log ( _L ( "main loop exited returned" ) );
	g_main_loop_unref ( auth_data->main_loop );
	lm_connection_close ( connection, NULL );
	lm_connection_unref ( connection );
	free_data ( auth_data->connect_data );	
	
	if ( auth_data->rCode == RC_ERROR )
	    {
	    iLog->Log ( _L ( "lm_connection_open_with_badserver failed" ) );
	    g_free ( auth_data );
	    return KErrGeneral;
	    }
	
	g_free ( auth_data );
	
	iLog->Log ( _L ( "lm_connection_open_with_badserver passed" ) );
	
	return KErrNone;		
}

 
// ----------------------------------------------------------------------------
// Ctstlm::lm_message_node_get_child
// Description:Fetches the child from node. If child is not found as an 
//             immediate child of node, NULL is returned
// Arguements :
//    node    : an LmMessageNode
//    name    : name of the new child
//    value   : value of the new child
// Returns    : the child node or NULL if child not found
// ----------------------------------------------------------------------------
//
TInt Ctstlm::lm_message_node_get_childL ( CStifItemParser& /*aItem*/ )
{
	LmMessage     *message   = NULL;
	LmMessageNode *Node      = NULL;	
	const gchar   *Username  = "Nokia";	
	const gchar   *Password  = "Nokia";		
	
	iLog->Log ( _L ( "In lm_message_node_get_child" ) );
	
	// Construct a message with type = LM_MESSAGE_TYPE_MESSAGE
	iLog->Log ( _L ( "Construct a message" ) );
	
	message = lm_message_new ( NULL, LM_MESSAGE_TYPE_MESSAGE );
	if ( !message )
	    {
		iLog->Log ( _L ( "lm_message_new failed" ) );
		return KErrGeneral;
	    }
	
	// Retrive the root node from message
	iLog->Log ( _L ( "Retrive the root node from the message" ) );
	
	Node = lm_message_get_node ( message );
	if ( !Node )
	    {
		iLog->Log ( _L ( "lm_message_get_node failed" ) );
		lm_message_unref ( message );
		return KErrGeneral;		
	    }
	
	// Add childrens
	iLog->Log ( _L ( "Adding childrens 'username' and 'password'" ) );
	lm_message_node_add_child ( Node, "username", Username );
	lm_message_node_add_child ( Node, "password", Password );
	
	// Verify the childs
	iLog->Log ( _L ( "Verify the childs" ) );
	
	if ( !lm_message_node_get_child ( Node, "username" ) ) 
	    {
		iLog->Log ( _L ( "lm_message_get_child failed" ) );
		lm_message_unref ( message );
		lm_message_node_unref ( Node );		
		return KErrGeneral;
	    }
	
	if ( !lm_message_node_get_child ( Node, "password" ) )
	    {
		iLog->Log ( _L ( "lm_message_get_child failed" ) );
		lm_message_unref ( message );
		lm_message_node_unref ( Node );
		return KErrGeneral;
	    }	
		
	lm_message_unref ( message );
	lm_message_node_unref ( Node );	
	
	iLog->Log ( _L ( "lm_message_node_get_child passed" ) );
	
	return KErrNone;	    
}

//--------------------------------------------------------------------------------
// function_name    : handle_receive_messages
// description      : callback function to handle messages
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_receive_messages ( LmMessageHandler* /*handler*/,
                  LmConnection*     /*connection*/,
                  LmMessage*        /*message*/,
                  gpointer          user_data )
    {
    GMainLoop *main_loop = ( GMainLoop * )user_data;	
	
	if ( main_loop )
		{
		g_main_loop_quit ( main_loop );
		}	
	
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    }



//-----------------------------------------------------------------------------
// function_name	: connection_auth_cb
// description		: callback function set by lm_connection_authenticate
// in lm_connection_sendL()
// Returns			: None
//-----------------------------------------------------------------------------
//
static void lm_login_cb ( LmConnection * /*connection*/, 
                     gboolean  success, 
                     gpointer data )
    {
    gboolean result = success;
    AuthData *auth_data = ( AuthData * ) data;
    //Result is false when the wrong input parameters were successfully handled.
    if (result == FALSE)
		{       
		auth_data->rCode = RC_OK;
		}
    else 
    	{
    	//Result is true, when the wrong input was not handled successfully.
       	auth_data->rCode = RC_ERROR;	
    	}
    g_main_loop_quit ( auth_data->main_loop ); 		
    }

//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_login_testL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_login_testL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	//ConnectData  *connect_data = NULL;
	AuthData     *auth_data    = NULL;
	
	
	iLog->Log ( _L ( "In lm_login_testL" ) );
	
	auth_data = g_new0 ( AuthData, 1 );
	if ( !auth_data )
		{
		iLog->Log ( _L ( "memory allocation failed for auth_data" ) );
		return KErrGeneral;
		}	
	
	// Read a data from the CFG file
	if ( read_data ( auth_data->connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( auth_data->connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( auth_data->connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( auth_data->connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, auth_data->connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, auth_data->connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	auth_data->main_loop = g_main_loop_new ( NULL, FALSE );

	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           auth_data->main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( auth_data->main_loop );
		free_data ( auth_data->connect_data );
				
		g_free(auth_data);
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );
	
	g_main_loop_run ( auth_data->main_loop );

	g_main_loop_unref ( auth_data->main_loop );    

	
	auth_data->main_loop = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( auth_data->connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     auth_data->connect_data->password,
                                     auth_data->connect_data->resource,
                                      ( LmResultFunction ) lm_login_cb,
                                     auth_data,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( auth_data->connect_data );
		g_main_loop_unref ( auth_data->main_loop );
		g_free ( username );
		g_free ( auth_data );
		return KErrGeneral;
	    }

	g_main_loop_run ( auth_data->main_loop );
	g_main_loop_unref ( auth_data->main_loop ); 
	g_free ( username );
	
	// Close the connection
	lm_connection_close ( connection, NULL );	
	// Remove a reference on connection
	lm_connection_unref ( connection );
	free_data ( auth_data->connect_data );
	
	if ( auth_data->rCode == RC_ERROR )
	    {
	    iLog->Log ( _L ( "lm_login_testL failed" ) );
	    g_free ( auth_data );
	    return KErrGeneral;
	    }	
	g_free ( auth_data );

	// Message sending passed
	iLog->Log ( _L ( "lm_login_testL passed" ) );
	
	return KErrNone;
}


//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_send400charL
// Description  : Asynchronous call to send a message of 400 characters
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_send400charL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message;
	
	iLog->Log ( _L ( "In lm_connection_send" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );

	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	
	main_loop = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }

	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	g_free ( username );
	
	// Construct a message
	message = lm_message_new ( connect_data->msg_data->recipient,               
	                           LM_MESSAGE_TYPE_MESSAGE );
	
	gchar* msg = {"sdfsadFsdfsadfsdfsdjfksdjfl;jdsf;jsadfl;jsdvdfggdfg:   \
				fjsdljfsldfjsfwerpwerpweripwierweirpiwepriwperiwperipwer  \
				fjsdljfsldfjsfwerpwerpweripwierweirpiwepriwperiwperipwer  \
				fjsdljfsldfjsfwerpwerpweripwierweirpiwepriwperiwperipwer  \
				fjsdljfsldfjsfwerpwerpweripwierweirpiwepriwperiwperipwer  \
				fjsdljfsldfjsfwerpwerpweripwierweirpiwepriwperiwperipwer  \
				fjsdljfsldfjsfwerpwerpweripwierweirpiwepriwperiwperipwer  \
				fjsdljfsldfjsfwerpwerpweripwierweirpiwepriwperiwperipwer  \
				fjsdljfsldfjsfwerpwerpweripwierweirpiwepriwperiwperipwer"};
	
	lm_message_node_add_child ( message->node, "body", 
	                            msg );
	
	// Send message to the server
	if ( !lm_connection_send ( connection, message, NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_send failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		free_data ( connect_data );		
		return KErrGeneral;
	    }	
	
	// Close the connection
	lm_connection_close ( connection, NULL );
	// Remove a reference on connection
	lm_connection_unref ( connection );
	// Remove a reference on message
	lm_message_unref ( message );
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_send passed" ) );
	
	return KErrNone;
    
    }

//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_send_repeatedL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_send_repeatedL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message;
	
	iLog->Log ( _L ( "In lm_connection_send_repeatedL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );

	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	
	main_loop = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	g_free ( username );
	
	// Construct a message
	message = lm_message_new ( connect_data->msg_data->recipient,               
	                           LM_MESSAGE_TYPE_MESSAGE );
	lm_message_node_add_child ( message->node, "body", 
	                            connect_data->msg_data->message);
	
	// Send message to the server
	if ( !lm_connection_send ( connection, message, NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_send failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		free_data ( connect_data );		
		return KErrGeneral;
	    }	
	
		// Send message to the server
	if ( !lm_connection_send ( connection, message, NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_send_repeatedL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		free_data ( connect_data );		
		return KErrGeneral;
	    }	
	    
	// Send message to the server
	if ( !lm_connection_send ( connection, message, NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_send_repeatedL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		free_data ( connect_data );		
		return KErrGeneral;
	    }	
	// Send message to the server
	if ( !lm_connection_send ( connection, message, NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_send_repeatedL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		free_data ( connect_data );		
		return KErrGeneral;
	    }	
	// Send message to the server
	if ( !lm_connection_send ( connection, message, NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_send_repeatedL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		free_data ( connect_data );		
		return KErrGeneral;
	    }		    	   	    
	// Close the connection
	lm_connection_close ( connection, NULL );
	
	// Remove a reference on connection
	lm_connection_unref ( connection );
	
	// Remove a reference on message
	lm_message_unref ( message );
	
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_send passed" ) );
	
	return KErrNone;
}
//--------------------------------------------------------------------------------
// function_name    : handle_add_contact
// description      : callback function to handle messages
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_add_contact ( LmMessageHandler* /*handler*/,
                  LmConnection*     /*connection*/,
                  LmMessage*        reply,
                  gpointer          user_data )
    {
    GMainLoop *main_loop = ( GMainLoop * )user_data;	
	LmMessageSubType  type;
	LmMessageNode *q_node,*item_node;
	type = lm_message_get_sub_type (reply); 
     	
    
	switch (type) 
    	{
    	case LM_MESSAGE_SUB_TYPE_SET:        		        		
	   			{
    			/*
			    <iq to='juliet@example.com/balcony' type='set' id='a78b4q6ha463'>
					<query xmlns='jabber:iq:roster'>
					    <item jid='nurse@example.com'
					          name='Nurse'
					          subscription='none'>
					      <group>Servants</group>
					    </item>
			  		</query>
				</iq>
				*/				
    			q_node = lm_message_node_get_child (reply->node, "query");   	
				if (q_node) 
			    	{
			   		item_node = lm_message_node_get_child (q_node, "item");
			   		if(item_node)
			   			{
			   			const char* reply_string;
			        	reply_string = lm_message_node_get_attribute(item_node,"jid");  
			        	}
			    
        			}	
    			
    			break;
	   			}
    	case LM_MESSAGE_SUB_TYPE_ERROR:          	          		
    		{
    		/*<iq from='prima@chat.gizmoproject.com/mytest12345rt' 
			to='prima@chat.gizmoproject.com/mytest12345rt' 
			id='164121175000' type='error'>
				<query xmlns='jabber:iq:roster'> 
					<item jid='testlmnokia1111@chat.gizmoproject.com'/>
				</query>
				<error code='404' type='cancel'>
					<item-not-found xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>
				</error>
			</iq>
			*/  
			//return NULL;
    		LmMessageNode* error_node=NULL;
    		LmMessageNode* error_node_child=NULL;
	    	error_node = lm_message_node_get_child (reply->node, "error");
		   	if(error_node)
	   			{
	   			const char* error_code;
	        	error_code = lm_message_node_get_attribute(error_node,"code");  
	        	error_node_child = lm_message_node_get_child (error_node, "item-not-found");
	        	}
    		break;
    		}
    //	default:
    	//	g_assert_not_reached ();
    	//	break;
    	} 
    
	if ( main_loop )
		{
		g_main_loop_quit ( main_loop );
		}	
	
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    }    

//-----------------------------------------------------------------------------
// Ctstlm::lm_add_contactL
// Description  : Asynchronous call to add a new contact
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_add_contactL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message;
	LmMessageNode *q_node,*item_node;
	LmMessageHandler* handler = NULL;
	
	iLog->Log ( _L ( "In lm_add_contactL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );

	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );
	
	g_main_loop_run ( main_loop );

	g_main_loop_unref ( main_loop );    

	
	main_loop = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	g_free ( username );	
	
    /***********fetch contact list********************/
  
    // Construct a message
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type (NULL, LM_MESSAGE_TYPE_IQ,
    					  LM_MESSAGE_SUB_TYPE_GET);
    q_node = lm_message_node_add_child (message->node, "query", NULL);
    lm_message_node_set_attributes (q_node,"xmlns", "jabber:iq:roster",NULL); 
	handler = lm_message_handler_new ( handle_fetch_contactlist_messages, main_loop, NULL );
	// Send message to the server
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		free_data ( connect_data );
		lm_message_handler_unref(handler);
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );                					
    lm_message_unref ( message );
    lm_message_handler_unref(handler);
  
    /****send own presence********/
	
	message = lm_message_new_with_sub_type ( NULL,
	                                     LM_MESSAGE_TYPE_PRESENCE,
	                                     LM_MESSAGE_SUB_TYPE_AVAILABLE );
	gboolean result = lm_connection_send ( connection, message, NULL );	
    lm_message_unref ( message );
    
    /*********Send request to add a contact**********************/
  
	// Construct a message
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_SET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    item_node = lm_message_node_add_child ( q_node, "item", NULL );
    
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:roster" );
    lm_message_node_set_attribute ( item_node, "jid", connect_data->msg_data->recipient );
    //If we specify the name we cannot add the contact, maybe the name has to be unique.
    /*lm_message_node_set_attribute ( item_node, "name", "harsh_rakesh" );
    if ( group )
    	{
    	lm_message_node_add_child ( item, "group", group );
    	}*/
	
	// Send message to the server
	handler = lm_message_handler_new ( handle_add_contact, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_add_contactL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );	
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);
	// Send subscription request to the server
    message = lm_message_new_with_sub_type ( connect_data->msg_data->recipient,
                                         LM_MESSAGE_TYPE_PRESENCE,
                                         LM_MESSAGE_SUB_TYPE_SUBSCRIBE );
    result = lm_connection_send ( connection, message, NULL ) ;
    // Remove a reference on message
	lm_message_unref ( message );
	    
	// Close the connection
	lm_connection_close ( connection, NULL );
	
	// Remove a reference on connection
	lm_connection_unref ( connection );
		
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_add_contactL passed" ) );
	
	return KErrNone;
}
//-----------------------------------------------------------------------------
// Ctstlm::lm_remove_contactL
// Description  : Asynchronous call to add a new contact
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_remove_contactL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message;
	LmMessageNode *q_node,*item_node;
	LmMessageHandler* handler = NULL;
	TBool result = EFalse;
	
	iLog->Log ( _L ( "In lm_remove_contactL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );

	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    	
	main_loop = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	g_free ( username );	
	
    /***********fetch contact list********************/
  
    // Construct a message
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type (NULL, LM_MESSAGE_TYPE_IQ,
    					  LM_MESSAGE_SUB_TYPE_GET);
    q_node = lm_message_node_add_child (message->node, "query", NULL);
    lm_message_node_set_attributes (q_node,"xmlns", "jabber:iq:roster",NULL); 
	handler = lm_message_handler_new ( handle_fetch_contactlist_messages, main_loop, NULL );
	// Send message to the server
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		free_data ( connect_data );
		lm_message_handler_unref(handler);
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );                					
    lm_message_unref ( message );
    lm_message_handler_unref(handler);
    
    /*********Send request to ADD a contact**********************/
    //If we specify the name we cannot add the contact, maybe the name has to be unique.
    /*lm_message_node_set_attribute ( item_node, "name", "harsh_rakesh" );
    	if ( group )
	    	{
	    	lm_message_node_add_child ( item, "group", group );
	    	}
	*/
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_SET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    item_node = lm_message_node_add_child ( q_node, "item", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:roster" );
    lm_message_node_set_attribute ( item_node, "jid", connect_data->msg_data->recipient );
    // Send message to the server
	handler = lm_message_handler_new ( handle_add_contact, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_add_contactL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );	
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);
	// Send subscription request to the server
    message = lm_message_new_with_sub_type ( connect_data->msg_data->recipient,
                                         LM_MESSAGE_TYPE_PRESENCE,
                                         LM_MESSAGE_SUB_TYPE_SUBSCRIBE );
    result = lm_connection_send ( connection, message, NULL ) ;
    lm_message_unref ( message );    
	
    /*********Send request to REMOVE a contact**********************/
    /*
      <iq from='juliet@example.com/balcony' type='set' id='roster_4'>
  			<query xmlns='jabber:iq:roster'>
    			<item jid='nurse@example.com' subscription='remove'/>
  			</query>
	  </iq>
	*/
	
	// Construct a message
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_SET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    item_node = lm_message_node_add_child ( q_node, "item", NULL );
    
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:roster" );
    lm_message_node_set_attribute ( item_node, "jid", connect_data->msg_data->recipient );
    lm_message_node_set_attribute ( item_node, "subscription", "remove" );
    // Send message to the server
	handler = lm_message_handler_new ( handle_add_contact, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_remove_contactL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);	 
	
	/***********FETCH contact list********************/
  
    // Construct a message
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type (NULL, LM_MESSAGE_TYPE_IQ,
    					  LM_MESSAGE_SUB_TYPE_GET);
    q_node = lm_message_node_add_child (message->node, "query", NULL);
    lm_message_node_set_attributes (q_node,"xmlns", "jabber:iq:roster",NULL); 
	handler = lm_message_handler_new ( handle_fetch_contactlist_messages, main_loop, NULL );
	// Send message to the server
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		free_data ( connect_data );
		lm_message_handler_unref(handler);
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );                					
    lm_message_unref ( message );
    lm_message_handler_unref(handler);   		
	
	/*********Send request to ADD a contact**********************/
  	// Construct a message
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_SET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    item_node = lm_message_node_add_child ( q_node, "item", NULL );
    
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:roster" );
    lm_message_node_set_attribute ( item_node, "jid", connect_data->msg_data->recipient);
    
	// Send message to the server
	handler = lm_message_handler_new ( handle_add_contact, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_add_contactL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );	
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);
	// Send subscription request to the server
    message = lm_message_new_with_sub_type ( connect_data->msg_data->recipient,
                                         LM_MESSAGE_TYPE_PRESENCE,
                                         LM_MESSAGE_SUB_TYPE_SUBSCRIBE );
    result = lm_connection_send ( connection, message, NULL ) ;
	// Remove a reference on message
	lm_message_unref ( message );
	
	/***********FETCH contact list********************/
  
    // Construct a message
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type (NULL, LM_MESSAGE_TYPE_IQ,
    					  LM_MESSAGE_SUB_TYPE_GET);
    q_node = lm_message_node_add_child (message->node, "query", NULL);
    lm_message_node_set_attributes (q_node,"xmlns", "jabber:iq:roster",NULL); 
	handler = lm_message_handler_new ( handle_fetch_contactlist_messages, main_loop, NULL );
	// Send message to the server
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		free_data ( connect_data );
		lm_message_handler_unref(handler);
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );                					
    lm_message_unref ( message );
    lm_message_handler_unref(handler);
	
	/*********Send request to REMOVE a contact**********************/
    /*
      <iq from='juliet@example.com/balcony' type='set' id='roster_4'>
  			<query xmlns='jabber:iq:roster'>
    			<item jid='nurse@example.com' subscription='remove'/>
  			</query>
	  </iq>
	*/
	
	// Construct a message
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_SET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    item_node = lm_message_node_add_child ( q_node, "item", NULL );
    
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:roster" );
    lm_message_node_set_attribute ( item_node, "jid", connect_data->msg_data->recipient );
    lm_message_node_set_attribute ( item_node, "subscription", "remove" );
    // Send message to the server
	handler = lm_message_handler_new ( handle_add_contact, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_remove_contactL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);	    
	
	/***********FETCH contact list********************/
  
    // Construct a message
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type (NULL, LM_MESSAGE_TYPE_IQ,
    					  LM_MESSAGE_SUB_TYPE_GET);
    q_node = lm_message_node_add_child (message->node, "query", NULL);
    lm_message_node_set_attributes (q_node,"xmlns", "jabber:iq:roster",NULL); 
	handler = lm_message_handler_new ( handle_fetch_contactlist_messages, main_loop, NULL );
	// Send message to the server
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		free_data ( connect_data );
		lm_message_handler_unref(handler);
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );                					
    lm_message_unref ( message );
    lm_message_handler_unref(handler);		


	/*********Send request to ADD a contact**********************/
    //If we specify the name we cannot add the contact, maybe the name has to be unique.
    /*lm_message_node_set_attribute ( item_node, "name", "harsh_rakesh" );
    	if ( group )
	    	{
	    	lm_message_node_add_child ( item, "group", group );
	    	}
	*/
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_SET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    item_node = lm_message_node_add_child ( q_node, "item", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:roster" );
    lm_message_node_set_attribute ( item_node, "jid", connect_data->msg_data->recipient );
    // Send message to the server
	handler = lm_message_handler_new ( handle_add_contact, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_add_contactL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );	
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);
	// Send subscription request to the server
    message = lm_message_new_with_sub_type ( connect_data->msg_data->recipient,
                                         LM_MESSAGE_TYPE_PRESENCE,
                                         LM_MESSAGE_SUB_TYPE_SUBSCRIBE );
    result = lm_connection_send ( connection, message, NULL ) ;
    lm_message_unref ( message );  
    	    
	/***********cleanup*****************/
	// Close the connection
	lm_connection_close ( connection, NULL );	
	// Remove a reference on connection
	lm_connection_unref ( connection );	
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_remove_contactL passed" ) );
	
	return KErrNone;
}
//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_send_receiveL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send and receive
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_send_receiveL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	LmMessage    *messagetoself = NULL;
	LmMessageHandler *handler = NULL;
	HandleData *handle_data = NULL;
	
	iLog->Log ( _L ( "In lm_connection_send" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );

	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	
	main_loop = g_main_loop_new ( NULL, FALSE );	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }

	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	g_free ( username );

	//------------------------------------------------------------------------
	//Send own presence
	message = lm_message_new_with_sub_type ( NULL,
	                                     LM_MESSAGE_TYPE_PRESENCE,
	                                     LM_MESSAGE_SUB_TYPE_AVAILABLE );
	
    gboolean result = lm_connection_send ( connection, message, NULL );	
   	
	//------------------------------------------------------------------------
	//
	// Register a handler to recieve msgs
	//
	main_loop = g_main_loop_new ( NULL, FALSE );
	handler = lm_message_handler_new ( 
	                     (LmHandleMessageFunction)handle_messages,
	                     main_loop,
	                     NULL );
	
	lm_connection_register_message_handler ( connection,
	                                         handler,
	                                         LM_MESSAGE_TYPE_MESSAGE,
	                                         LM_HANDLER_PRIORITY_FIRST );
	
	// Construct a message for sending to self
	messagetoself = lm_message_new ( connect_data->username,               
	                           LM_MESSAGE_TYPE_MESSAGE );
	lm_message_node_add_child ( message->node, "body", 
	                            connect_data->msg_data->message);
	
	// Send message to the server
	if ( !lm_connection_send ( connection, messagetoself, NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_send failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( messagetoself );
		lm_message_unref ( message );
		g_main_loop_unref ( main_loop );
		lm_message_handler_unref(handler);
		free_data ( connect_data );		
		return KErrGeneral;
	    }	
    // Wait for the message sent to self
    g_main_loop_run ( main_loop );
    g_main_loop_unref ( main_loop );
    lm_message_handler_unref(handler);
	// Remove a reference on message
	lm_message_unref ( message );
	lm_message_unref ( messagetoself );
	
	//------------------------------------------------------------------------
	
	// Close the connection
	lm_connection_close ( connection, NULL );	
	// Remove a reference on connection
	lm_connection_unref ( connection );		
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_send passed" ) );
	
	return KErrNone;
}



//-----------------------------------------------------------------------------
// funnction_name	: connection_open_cancel_cb
// description		: callback function called by lm_connection_open
// Returns			: None
//-----------------------------------------------------------------------------
//
static void
connection_open_cancel_cb ( LmConnection * /*connection*/, 
                     gboolean    success  /*success*/, 
                     gpointer data )
    {
    
    gboolean result = success ;
    GMainLoop *main_loop = ( GMainLoop * ) data;
	if ( main_loop )
	    {
		g_main_loop_quit ( main_loop );	
	    }		
    }

//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_cancel_openL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_cancel_openL ( CStifItemParser& aItem )
 	{
		LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	
	iLog->Log ( _L ( "In lm_connection_cancel_openL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );

	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cancel_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
	
	if(!lm_connection_is_open(connection))
		{
		lm_connection_cancel_open(connection);
		//lm_connection_unref(connection);
		}
	
	//g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    

	// Close the connection
	lm_connection_close ( connection, NULL );
	
	// Remove a reference on connection
	lm_connection_unref ( connection );
	
	
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_cancel_openL passed" ) );
	
	return KErrNone;
	}

//--------------------------------------------------------------------------------
// function_name    : handle_get_privacy_lists
// description      : callback function to handle messages
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_get_privacy_lists ( LmMessageHandler* /*handler*/,
                  LmConnection*     /*connection*/,
                  LmMessage*        reply,
                  gpointer          user_data )
    {
    GMainLoop *main_loop = ( GMainLoop * )user_data;	
	LmMessageSubType  type;
	LmMessageNode *q_node,*item_node;
	type = lm_message_get_sub_type (reply); 
    /*
    <iq type='result' id='getlist1' to='romeo@example.net/orchard'>
		<query xmlns='jabber:iq:privacy'>
		    <active name='private'/>
		    <default name='public'/>
		    <list name='public'/>
		    <list name='private'/>
			<list name='special'/>
  		</query>
	</iq>
    */ 	
       	
	switch (type) 
    	{
    	case LM_MESSAGE_SUB_TYPE_RESULT:        		        		
    			{
				const char* list_name1,*list_name2,*list_name3,*list_name4;
    			q_node = lm_message_node_get_child (reply->node, "query");
    			item_node = lm_message_node_get_child (q_node, "list");
				list_name1 = lm_message_node_get_attribute(item_node,"name");
				
				item_node = lm_message_node_get_child(q_node, "list");
				list_name2 = lm_message_node_get_attribute(item_node,"name");  
				
				item_node = lm_message_node_get_child (q_node, "active");
				if(item_node!=NULL)
					list_name3 = lm_message_node_get_attribute(item_node,"name");  
				
				item_node = lm_message_node_get_child (q_node, "default");
				if(item_node!=NULL)
					list_name4 = lm_message_node_get_attribute(item_node,"name");  
				
    			break;
    			}
    	case LM_MESSAGE_SUB_TYPE_ERROR:
    			{
    			/*
    			<error type='cancel'>
    				<item-not-found	xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>
    				
  				</error>
  				OR
  				<error type='modify'>
			    	<bad-request xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>
  				</error>
  				*/
				const char* error_type;
    			q_node = lm_message_node_get_child (reply->node, "query");
    			item_node = lm_message_node_get_child (q_node, "error");
				error_type = lm_message_node_get_attribute(item_node,"type");
    			break;
    			}
    //	default:
    	//	g_assert_not_reached ();
    	//	break;
    	} 
    
   

	if (!q_node) 
    	{
   		//return NULL;
    	}
    	
   
	if ( main_loop )
		{
		g_main_loop_quit ( main_loop );
		}	
	
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    }   
//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_get_privacy_listsL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_get_privacy_listsL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	LmMessageNode* q_node = NULL;
	LmMessageHandler *handler = NULL;
	
	iLog->Log ( _L ( "In lm_connection_get_privacy_listsL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );

	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    

	main_loop = g_main_loop_new ( NULL, FALSE );
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	g_free ( username );
	
	// Construct a message with type=LM_MESSAGE_TYPE_IQ
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_GET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:privacy" );
    // Send message to the server
	handler = lm_message_handler_new ( handle_get_privacy_lists, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_add_contactL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);

   
	// Close the connection
	lm_connection_close ( connection, NULL );
	
	// Remove a reference on connection
	lm_connection_unref ( connection );
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_get_privacy_listsL passed" ) );
	
	return KErrNone;
}


//--------------------------------------------------------------------------------
// function_name    : handle_set_privacy_lists
// description      : callback function to handle messages
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_set_privacy_lists ( LmMessageHandler*/* handler*/,
                  LmConnection*     /*connection*/,
                  LmMessage*        reply,
                  gpointer          user_data )
    {
    GMainLoop *main_loop = ( GMainLoop * )user_data;	
	LmMessageSubType  type;
	LmMessageNode *q_node = NULL;
	LmMessageNode *item_node = NULL;
	type = lm_message_get_sub_type (reply); 
    /*
    <iq type='result' id='edit1' to='romeo@example.net/orchard'/>
    */ 	
       	
	switch (type) 
    	{
    	case LM_MESSAGE_SUB_TYPE_RESULT:        		        		
    		{
    		const char* reply_string;
    		reply_string = lm_message_node_get_attribute(reply->node,"id");  
			break;
    		}
    	case LM_MESSAGE_SUB_TYPE_ERROR:          	          		
    		{
    		/*
		    <iq to='romeo@example.net/orchard' type='error' id='getlist6'>
			    <error type='modify'>
			    	<bad-request xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>
			    </error>
		    </iq>
			*/ 	
    		if (q_node) 
		    	{
		    	LmMessageNode* error_node = NULL;
		    	error_node = lm_message_node_get_child(reply->node,"error");
	    		const char* error_code;
	    		error_code = lm_message_node_get_attribute(error_node,"code");  
				const char* error_type;
	    		error_type = lm_message_node_get_attribute(error_node,"type");  
				item_node = lm_message_node_get_child (q_node, "feature-not-implemented");
		    	}
			break;
			}
    //	default:
    	//	g_assert_not_reached ();
    	//	break;
    	} 
    	
    
	if ( main_loop )
		{
		g_main_loop_quit ( main_loop );
		}	
	
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    }   
//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_set_privacy_listsL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_set_privacy_listsL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	LmMessageNode* q_node, *item_node1, *item_node2 = NULL;
	LmMessageHandler *handler = NULL;
	
	iLog->Log ( _L ( "In lm_connection_set_privacy_listsL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );
	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop ); 
	
	main_loop = g_main_loop_new ( NULL, FALSE );	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }

	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	g_free ( username );
	
	// Construct a message with type=LM_MESSAGE_TYPE_IQ
	/*
	<iq from='romeo@example.net/orchard' type='set' id='edit1'>
		<query xmlns='jabber:iq:privacy'>
		    <list name='public'>
		      <item type='jid'
		            value='tybalt@example.com'
		            action='deny'
		            order='3'/>
		      <item type='jid'
		            value='paris@example.org'
		            action='deny'
		            order='5'/>
		      <item action='allow' order='68'/>
		    </list>
		    <list name='active'>
		      <item type='jid'
		            value='tybalt@example.com'
		            action='deny'
		            order='3'/>
		      <item type='jid'
		            value='paris@example.org'
		            action='deny'
		            order='5'/>
		      <item action='allow' order='68'/>
		    </list>
  		</query>
	</iq>
	*/
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_SET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:privacy" );
    item_node1 = lm_message_node_add_child ( q_node, "list", NULL );
    lm_message_node_set_attribute ( item_node1, "name", "public" );
    item_node2 = lm_message_node_add_child ( item_node1, "item", NULL );
    lm_message_node_set_attribute ( item_node2, "type", "jid" );
    lm_message_node_set_attribute ( item_node2, "value", "rakesh_harsh@chat.gizmoproject.com" );
    lm_message_node_set_attribute ( item_node2, "action", "deny" );
    lm_message_node_set_attribute ( item_node2, "order", "3" );
    
    item_node1 = lm_message_node_add_child ( q_node, "list", NULL );
    lm_message_node_set_attribute ( item_node1, "name", "active" );
    item_node2 = lm_message_node_add_child ( item_node1, "item", NULL );
    lm_message_node_set_attribute ( item_node2, "type", "jid" );
    lm_message_node_set_attribute ( item_node2, "value", "rakesh_harsh@chat.gizmoproject.com" );
    lm_message_node_set_attribute ( item_node2, "action", "deny" );
    lm_message_node_set_attribute ( item_node2, "order", "4" );
    
    // Send message to the server
	handler = lm_message_handler_new ( handle_set_privacy_lists, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_add_contactL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);

   
	// Close the connection
	lm_connection_close ( connection, NULL );
	
	// Remove a reference on connection
	lm_connection_unref ( connection );
	
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_set_privacy_listsL passed" ) );
	
	return KErrNone;
}

//--------------------------------------------------------------------------------
// function_name    : handle_send_image_cb
// description      : callback function to handle messages
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_send_image_cb ( LmMessageHandler* /*handler*/,
                  LmConnection*     /*connection*/,
                  LmMessage*        reply,
                  gpointer          user_data )
    {
    SendStreamData *stream_data = ( SendStreamData * )user_data;	
	LmMessageSubType  type;
	LmMessageNode *q_node,*item_node;
	type = lm_message_get_sub_type (reply); 
        	
	switch (type) 
    	{
    	case LM_MESSAGE_SUB_TYPE_RESULT:        		        		
			{
			const char* reply_string;
			reply_string = lm_message_node_get_attribute(reply->node,"id");  
			break;
			}
    	case LM_MESSAGE_SUB_TYPE_ERROR:          	          		
    		{
    		/*<message from='juliet@capulet.com/balcony' to='romeo@montague.net/orchard' 
    						id='msg1' type='error'>
    			...
			    <error code='504' type='cancel'>
			         <remote-server-timeout xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>
			    </error>
			</message>*/

    		const char* reply_string;
    		reply_string = lm_message_node_get_attribute(reply->node,"id");  
			q_node = lm_message_node_get_child (reply->node, "error");
			if (q_node) 
		    	{
		   		const char* error_code;
		   		error_code = lm_message_node_get_attribute(q_node,"code");  
		   		item_node = lm_message_node_get_child (q_node, "feature-not-implemented");
		   		if(item_node==NULL)
		   			{
		   			item_node = lm_message_node_get_child (q_node, "remote-server-timeout");
		   			}
		    	}
		    	
			break;
    		}
    //	default:
    	//	g_assert_not_reached ();
    	//	break;
    	} 
    	
    
	if ( stream_data->main_loop )
		{
		g_main_loop_quit ( stream_data->main_loop );
		}	
	lm_message_unref(stream_data->message);
	lm_message_handler_unref(stream_data->handler);
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    }
//--------------------------------------------------------------------------------
// function_name    : handle_send_image_stream_msg_open_cb
// description      : callback function to handle messages
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_send_image_stream_msg_open_cb ( LmMessageHandler* /*handler*/,
                  LmConnection*     connection,
                  LmMessage*        reply,
                  gpointer          user_data )
    {
    SendStreamData *stream_data = ( SendStreamData * )user_data;	
    LmMessageSubType  type;
	LmMessageNode *q_node = NULL;
	LmMessageNode *item_node1 = NULL;
	LmMessageNode *item_node2 = NULL;
	LmMessageNode* item_node = NULL;
	LmMessage * message = NULL;
	LmMessageHandler* handler = NULL;
	type = lm_message_get_sub_type (reply); 
    /*
    //success response
    <iq type='result' from='juliet@capulet.com/balcony' to='romeo@montague.net/orchard'
    	id='inband_1'/>
    	
    //error response
    <iq type='error' from='juliet@capulet.com/balcony' to='romeo@montague.net/orchard'
    	id='inband_1'/>
  			<error code='501' type='cancel'>
    			<feature-not-implemented xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>
  			</error>
	</iq>
	*/ 	
   
	switch (type) 
    	{
    	case LM_MESSAGE_SUB_TYPE_RESULT:
    		{
    		const char* reply_string;
    		reply_string = lm_message_node_get_attribute(q_node,"id");  
    			
			/****************************************************/
			// send bytes in message
			/*
			<message from='romeo@montague.net/orchard' to='juliet@capulet.com/balcony' id='msg1'>
				  <data xmlns='http://jabber.org/protocol/ibb' sid='mySID' seq='0'>
				    qANQR1DBwU4DX7jmYZnncmUQB/9KuKBddzQH+tZ1ZywKK0yHKnq57kWq+RFtQdCJ
				    WpdWpR0uQsuJe7+vh3NWn59/gTc5MDlX8dS9p0ovStmNcyLhxVgmqS8ZKhsblVeu
				    IpQ0JgavABqibJolc3BKrVtVV1igKiX/N7Pi8RtY1K18toaMDhdEfhBRzO/XB0+P
				    AQhYlRjNacGcslkhXqNjK5Va4tuOAPy2n1Q8UUrHbUd0g+xJ9Bm0G0LZXyvCWyKH
				    kuNEHFQiLuCY6Iv0myq6iX6tjuHehZlFSh80b5BVV9tNLwNR5Eqz1klxMhoghJOA
				  </data>
				  <amp xmlns='http://jabber.org/protocol/amp'>
				    <rule condition='deliver' value='stored' action='error'/>
				    <rule condition='match-resource' value='exact' action='error'/>
				  </amp>
			</message>
			*/
			message = lm_message_new ( NULL, LM_MESSAGE_TYPE_MESSAGE );
			lm_message_node_set_attribute ( message->node, "to", "donna.prima@gmail.com" );
		    q_node = lm_message_node_add_child ( message->node, "data", 
		    		"qANQR1DBwU4DX7jmYZnncmUQB/9KuKBddzQH+tZ1ZywKK0yHKnq57kWq+RFtQdCJ \
				    WpdWpR0uQsuJe7+vh3NWn59/gTc5MDlX8dS9p0ovStmNcyLhxVgmqS8ZKhsblVeu \
				    IpQ0JgavABqibJolc3BKrVtVV1igKiX/N7Pi8RtY1K18toaMDhdEfhBRzO/XB0+P \
				    AQhYlRjNacGcslkhXqNjK5Va4tuOAPy2n1Q8UUrHbUd0g+xJ9Bm0G0LZXyvCWyKH \
				    kuNEHFQiLuCY6Iv0myq6iX6tjuHehZlFSh80b5BVV9tNLwNR5Eqz1klxMhoghJOA" );
		    lm_message_node_set_attribute ( q_node, "xmlns", "http://jabber.org/protocol/ibb" );
		    lm_message_node_set_attribute ( q_node, "sid", "mySID" );
		    lm_message_node_set_attribute ( q_node, "seq", "0" );
		    
		    q_node = lm_message_node_add_child ( message->node, "amp", NULL );
		    lm_message_node_set_attribute ( q_node, "xmlns", "http://jabber.org/protocol/amp" );
		    item_node1 = lm_message_node_add_child ( q_node, "rule", NULL );
		    lm_message_node_set_attribute ( item_node1, "condition", "deliver" );
		    lm_message_node_set_attribute ( item_node1, "value", "stored" );
		    lm_message_node_set_attribute ( item_node1, "action", "error" );
		    item_node2 = lm_message_node_add_child ( q_node, "rule", NULL );
		    lm_message_node_set_attribute ( item_node1, "condition", "match-resource" );
		    lm_message_node_set_attribute ( item_node1, "value", "exact" );
		    lm_message_node_set_attribute ( item_node1, "action", "error" );
		    
		    stream_data->message = message;
		    
		    // Send message to the server
			handler = lm_message_handler_new ( handle_send_image_cb, stream_data, NULL );	
			
			stream_data->handler = handler;
			if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
			    {
				//lm_connection_close ( connection, NULL );
				//lm_connection_unref ( connection );
				lm_message_unref ( stream_data->message );
				lm_message_handler_unref(stream_data->handler);
				if ( stream_data->main_loop)
					{
					g_main_loop_quit ( stream_data->main_loop );
					}
				return LM_HANDLER_RESULT_REMOVE_MESSAGE;
			    }
			/****************************************************/
			break;
    		}        		        		
    		
    	case LM_MESSAGE_SUB_TYPE_ERROR:          	          		
    		{
    		const char* reply_string;
    		reply_string = lm_message_node_get_attribute(reply->node,"id");  
			q_node = lm_message_node_get_child (reply->node, "error");
			if (q_node) 
		    	{
		   		const char* error_code;
		   		error_code = lm_message_node_get_attribute(q_node,"code");  
		   		item_node = lm_message_node_get_child (q_node, "feature-not-implemented");
		    	}
		    if ( stream_data->main_loop)
				{
				g_main_loop_quit ( stream_data->main_loop );
				}
			break;
    		}
    //	default:
    	//	g_assert_not_reached ();
    	//	break;
    	} 
    	
  
		
	
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    }  
   
//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_send_image_bytestream_msgL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_send_image_bytestream_msgL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	LmMessageNode* q_node, *item_node1, *item_node2 = NULL;
	LmMessageHandler *handler = NULL;
	SendStreamData* stream_data = NULL;
	iLog->Log ( _L ( "In lm_connection_send_imageL" ) );
	
	stream_data = g_new0 ( SendStreamData, 1 );
	if (! stream_data )
		{
		return KErrGeneral;
		}			
	if ( read_data ( stream_data->connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );		
		free_data ( stream_data->connect_data );		
		g_free ( stream_data );
		return KErrGeneral;
		}
	stream_data->main_loop    = g_main_loop_new ( NULL, FALSE );	
	stream_data->rCode        = RC_ERROR;
	
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( stream_data->connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( stream_data->connect_data );
		g_free(stream_data);
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, stream_data->connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, stream_data->connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	stream_data->main_loop = g_main_loop_new ( NULL, FALSE );

	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           stream_data->main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( stream_data->main_loop );
		free_data ( stream_data->connect_data );
		g_free(stream_data);		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( stream_data->main_loop );
	g_main_loop_unref ( stream_data->main_loop );    

	
	stream_data->main_loop = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( stream_data->connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     stream_data->connect_data->password,
                                     stream_data->connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     stream_data->main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( stream_data->connect_data );
		g_main_loop_unref ( stream_data->main_loop );
		g_free(stream_data);
		g_free ( username );
		return KErrGeneral;
	    }

	g_main_loop_run ( stream_data->main_loop );
	g_main_loop_unref ( stream_data->main_loop );
	g_free ( username );
	
	/**************************************/
	//Opening a byte stream
	/*
	<iq type='set' from='romeo@montague.net/orchard'
	    			to='juliet@capulet.com/balcony'	id='inband_1'>
	  	<open sid='mySID' block-size='4096' xmlns='http://jabber.org/protocol/ibb'/>
	</iq>
	*/
	stream_data->main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_SET );
	lm_message_node_set_attribute ( message->node, "to", "donna.prima@gmail.com" );
    q_node = lm_message_node_add_child ( message->node, "open", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "http://jabber.org/protocol/ibb" );
    lm_message_node_set_attribute ( q_node, "sid", "mySID" );
    lm_message_node_set_attribute ( q_node, "block-size", "4096" );
    
    // Send message to the server
	handler = lm_message_handler_new ( handle_send_image_stream_msg_open_cb, stream_data, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_add_contactL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( stream_data->connect_data );
		g_main_loop_unref ( stream_data->main_loop );
		g_free(stream_data);
		return KErrGeneral;
	    }
	g_main_loop_run ( stream_data->main_loop );
	g_main_loop_unref ( stream_data->main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);
	/***************************************/
   
	// Close the connection
	lm_connection_close ( connection, NULL );
	
	// Remove a reference on connection
	lm_connection_unref ( connection );
	
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_send_imageL passed" ) );
	
	return KErrNone;
}

//--------------------------------------------------------------------------------
// function_name    : handle_send_image_iq_cb
// description      : callback function to handle messages
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_send_image_iq_cb ( LmMessageHandler* /*handler*/,
                  LmConnection*     /*connection*/,
                  LmMessage*        reply,
                  gpointer          user_data )
    {
    SendStreamData* stream_data = ( SendStreamData * )user_data;	
	LmMessageSubType  type;
	LmMessageNode *q_node,*item_node;
	type = lm_message_get_sub_type (reply); 
        	
	switch (type) 
    	{
    	case LM_MESSAGE_SUB_TYPE_RESULT:        		        		
			{
			const char* reply_string;
			reply_string = lm_message_node_get_attribute(reply->node,"id");  
			break;
			}
    	case LM_MESSAGE_SUB_TYPE_ERROR:          	          		
    		{
    		/*<iq from='juliet@capulet.com/balcony' to='romeo@montague.net/orchard' id='ibb1' type='error'>
				  ...
				  <error code='504' type='cancel'>
				    <remote-server-timeout xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>
				  </error>
			  </iq>
			*/

    		const char* reply_string;
    		reply_string = lm_message_node_get_attribute(reply->node,"id");  
			q_node = lm_message_node_get_child (reply->node, "error");
			if (q_node) 
		    	{
		   		const char* error_code;
		   		error_code = lm_message_node_get_attribute(q_node,"code");  
		   		item_node = lm_message_node_get_child (q_node, "feature-not-implemented");
		   		if(item_node==NULL)
		   			{
		   			item_node = lm_message_node_get_child (q_node, "remote-server-timeout");
		   			}
		    	}
		    	
			break;
    		}
    //	default:
    	//	g_assert_not_reached ();
    	//	break;
    	} 
    	
    
	if ( stream_data->main_loop )
		{
		g_main_loop_quit ( stream_data->main_loop );
		}	
	lm_message_unref(stream_data->message);
	lm_message_handler_unref(stream_data->handler);
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    } 
    
//--------------------------------------------------------------------------------
// function_name    : handle_send_image_stream_iq_open_cb
// description      : callback function to handle messages
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_send_image_stream_iq_open_cb ( LmMessageHandler* /*handler*/,
                  LmConnection*     connection,
                  LmMessage*        reply,
                  gpointer          user_data )
    {
    SendStreamData *stream_data = ( SendStreamData * )user_data;	
    LmMessageSubType  type;
	LmMessageNode *q_node = NULL;
	LmMessageNode* item_node = NULL;
	LmMessage * message = NULL;
	LmMessageHandler* handler = NULL;
	type = lm_message_get_sub_type (reply); 
    /*
    //success response
    <iq type='result' from='juliet@capulet.com/balcony' to='romeo@montague.net/orchard'
    	id='inband_1'/>
    	
    //error response
    <iq type='error' from='juliet@capulet.com/balcony' to='romeo@montague.net/orchard'
    	id='inband_1'/>
  			<error code='501' type='cancel'>
    			<feature-not-implemented xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>
  			</error>
	</iq>
	*/ 	
 
	switch (type) 
    	{
    	case LM_MESSAGE_SUB_TYPE_RESULT:
    		{
    		const char* reply_string;
    		reply_string = lm_message_node_get_attribute(q_node,"id");  
    			
			/****************************************************/
			// send bytes in message
			/*
			<iq from='romeo@montague.net/orchard' to='juliet@capulet.com/balcony' type='set' id='ibb1'>
			  <data xmlns='http://jabber.org/protocol/ibb' sid='mySID' seq='0'>
			    qANQR1DBwU4DX7jmYZnncmUQB/9KuKBddzQH+tZ1ZywKK0yHKnq57kWq+RFtQdCJ
			    WpdWpR0uQsuJe7+vh3NWn59/gTc5MDlX8dS9p0ovStmNcyLhxVgmqS8ZKhsblVeu
			    IpQ0JgavABqibJolc3BKrVtVV1igKiX/N7Pi8RtY1K18toaMDhdEfhBRzO/XB0+P
			    AQhYlRjNacGcslkhXqNjK5Va4tuOAPy2n1Q8UUrHbUd0g+xJ9Bm0G0LZXyvCWyKH
			    kuNEHFQiLuCY6Iv0myq6iX6tjuHehZlFSh80b5BVV9tNLwNR5Eqz1klxMhoghJOA
			  </data>
			</iq>
			*/
			message = lm_message_new_with_sub_type( NULL, LM_MESSAGE_TYPE_IQ, 
													LM_MESSAGE_SUB_TYPE_SET);
			lm_message_node_set_attribute ( message->node, "to", "donna.prima@gmail.com" );
		    q_node = lm_message_node_add_child ( message->node, "data", 
		    		"qANQR1DBwU4DX7jmYZnncmUQB/9KuKBddzQH+tZ1ZywKK0yHKnq57kWq+RFtQdCJ \
				    WpdWpR0uQsuJe7+vh3NWn59/gTc5MDlX8dS9p0ovStmNcyLhxVgmqS8ZKhsblVeu \
				    IpQ0JgavABqibJolc3BKrVtVV1igKiX/N7Pi8RtY1K18toaMDhdEfhBRzO/XB0+P \
				    AQhYlRjNacGcslkhXqNjK5Va4tuOAPy2n1Q8UUrHbUd0g+xJ9Bm0G0LZXyvCWyKH \
				    kuNEHFQiLuCY6Iv0myq6iX6tjuHehZlFSh80b5BVV9tNLwNR5Eqz1klxMhoghJOA" );
		    lm_message_node_set_attribute ( q_node, "xmlns", "http://jabber.org/protocol/ibb" );
		    lm_message_node_set_attribute ( q_node, "sid", "mySID" );
		    lm_message_node_set_attribute ( q_node, "seq", "0" );
		    stream_data->message = message;
		   
		    // Send message to the server
			handler = lm_message_handler_new ( handle_send_image_iq_cb, stream_data, NULL );	
			stream_data->handler = handler;
			if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
			    {
				lm_message_unref ( stream_data->message );
				lm_message_handler_unref(stream_data->handler);
				if ( stream_data->main_loop)
				{
				g_main_loop_quit ( stream_data->main_loop );
				}				
				return LM_HANDLER_RESULT_REMOVE_MESSAGE;
			    }
			/****************************************************/
			break;
    		}        		        		
    		
    	case LM_MESSAGE_SUB_TYPE_ERROR:          	          		
    		{
    		const char* reply_string;
    		reply_string = lm_message_node_get_attribute(reply->node,"id");  
			q_node = lm_message_node_get_child (reply->node, "error");
			if (q_node) 
		    	{
		   		const char* error_code;
		   		error_code = lm_message_node_get_attribute(q_node,"code");  
		   		item_node = lm_message_node_get_child (q_node, "feature-not-implemented");
		    	}
		    if ( stream_data->main_loop)
				{
				g_main_loop_quit ( stream_data->main_loop );
				}
			break;
    		}
    //	default:
    	//	g_assert_not_reached ();
    	//	break;
    	} 
    	
	
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    }        
//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_send_image_bytestream_iqL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_send_image_bytestream_iqL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	LmMessageNode* q_node= NULL;
	LmMessageHandler *handler = NULL;
	SendStreamData* stream_data = NULL;
	iLog->Log ( _L ( "In lm_connection_send_imageL" ) );
	
	stream_data = g_new0 ( SendStreamData, 1 );
	if (! stream_data )
		{
		return KErrGeneral;
		}			
	if ( read_data ( stream_data->connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );		
		free_data ( stream_data->connect_data );		
		g_free ( stream_data );
		return KErrGeneral;
		}
	stream_data->main_loop    = g_main_loop_new ( NULL, FALSE );	
	stream_data->rCode        = RC_ERROR;
	
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( stream_data->connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( stream_data->connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, stream_data->connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, stream_data->connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	stream_data->main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           stream_data->main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( stream_data->main_loop );
		free_data ( stream_data->connect_data );
		g_free(stream_data);		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( stream_data->main_loop );
	g_main_loop_unref ( stream_data->main_loop );   
	
	stream_data->main_loop = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( stream_data->connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     stream_data->connect_data->password,
                                     stream_data->connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     stream_data->main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( stream_data->connect_data );
		g_main_loop_unref ( stream_data->main_loop );
		g_free(stream_data);
		g_free ( username );
		return KErrGeneral;
	    }

	g_main_loop_run ( stream_data->main_loop );
	g_main_loop_unref ( stream_data->main_loop );
	g_free ( username );
	/**************************************/
	//Opening a byte stream
	/*
	<iq type='set' from='romeo@montague.net/orchard'
	    			to='juliet@capulet.com/balcony'	id='inband_1'>
	  	<open sid='mySID' block-size='4096' xmlns='http://jabber.org/protocol/ibb'/>
	</iq>
	*/
	stream_data->main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_SET );
	lm_message_node_set_attribute ( message->node, "to", "donna.prima@gmail.com" );
    q_node = lm_message_node_add_child ( message->node, "open", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "http://jabber.org/protocol/ibb" );
    lm_message_node_set_attribute ( q_node, "sid", "mySID" );
    lm_message_node_set_attribute ( q_node, "block-size", "4096" );
    
    // Send message to the server
	handler = lm_message_handler_new ( handle_send_image_stream_iq_open_cb, stream_data, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_add_contactL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( stream_data->connect_data );
		g_free(stream_data);
		g_main_loop_unref ( stream_data->main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( stream_data->main_loop );
	g_main_loop_unref ( stream_data->main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);
	/****************************************************/
   
	// Close the connection
	lm_connection_close ( connection, NULL );	
	// Remove a reference on connection
	lm_connection_unref ( connection );
	free_data ( stream_data->connect_data );
	g_free(stream_data);
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_send_imageL passed" ) );
	
	return KErrNone;
}

//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_send_multiple_connL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_send_multiple_connL ( CStifItemParser& aItem )
    {
	LmConnection *connection1   = NULL;
	LmConnection *connection2   = NULL;
	ConnectData  *connect_data1 = NULL;
	LmMessage    *message;
	iLog->Log ( _L ( "In lm_connection_send_multiple_connL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data1, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data1 );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection1 = lm_connection_new ( connect_data1->server );	
	connection2 = lm_connection_new ( connect_data1->server );
	
	
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection1 || !connection2 )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data1 );
		//free_data ( connect_data2 );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port and Set the JID
	lm_connection_set_port ( connection1, GTALK_SSL_PORT );
	lm_connection_set_jid ( connection1, connect_data1->username );
	
	// Set the gtalk's SSL port and Set the JID
	lm_connection_set_port ( connection2, GTALK_SSL_PORT );
	lm_connection_set_jid ( connection2, connect_data1->username );

	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection1, connect_data1->proxy_data );
	SetProxy ( connection2, connect_data1->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection1 );
	SSLInit ( connection2 );
	/********************************************************/
	//CONNECTION1
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );
	if ( !lm_connection_open ( connection1, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection1 );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data1 );		
		return KErrGeneral;
	    }
	iLog->Log ( _L ( "after lm_connection_open" ) );
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
    main_loop = g_main_loop_new ( NULL, FALSE );
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data1->username );	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection1,
                                     username,
                                     connect_data1->password,
                                     connect_data1->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection1, NULL );
		lm_connection_unref ( connection1 );
		free_data ( connect_data1 );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	g_free ( username );
	
	/****************************************************/
	//CONNECTION1 MSG
	// Construct a message
	message = lm_message_new ( connect_data1->msg_data->recipient,               
	                           LM_MESSAGE_TYPE_MESSAGE );
	lm_message_node_add_child ( message->node, "body", 
	                            connect_data1->msg_data->message);
	// Send message to the server
	if ( !lm_connection_send ( connection1, message, NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_send failed" ) );
		lm_connection_close ( connection1, NULL );
		lm_connection_unref ( connection1 );
		lm_connection_unref ( connection2 );
		lm_message_unref ( message );
		free_data ( connect_data1 );		
		return KErrGeneral;
	    }	
	// Remove a reference on message
	lm_message_unref ( message );	
	
	/***********************************************/
	//CONNECTION2
	main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );
	if ( !lm_connection_open ( connection2, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_close ( connection1, NULL );
		lm_connection_unref ( connection1 );
		lm_connection_unref ( connection2 );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data1 );		
		return KErrGeneral;
	    }
	iLog->Log ( _L ( "after lm_connection_open" ) );
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
    main_loop = g_main_loop_new ( NULL, FALSE );
	// Get the username from the JID
	username = get_user_name ( connect_data1->username );	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection2,
                                     username,
                                     connect_data1->password,
                                     /*connect_data2->resource*/
                                     "RSC1234",
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection1, NULL );
		lm_connection_unref ( connection1 );
		lm_connection_close ( connection2, NULL );
		lm_connection_unref ( connection2 );
		free_data ( connect_data1 );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	g_free ( username );
	/***********************************************/
	// CONNECTION2 MSG
	// Construct a message
	message = lm_message_new ( connect_data1->msg_data->recipient,               
	                           LM_MESSAGE_TYPE_MESSAGE );
	lm_message_node_add_child ( message->node, "body", 
	                            connect_data1->msg_data->message);
	// Send message to the server
	if ( !lm_connection_send ( connection2, message, NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_send failed" ) );
		lm_connection_close ( connection2, NULL );
		lm_connection_unref ( connection2 );
		lm_message_unref ( message );
		free_data ( connect_data1);		
		return KErrGeneral;
	    }	
	// Remove a reference on message
	lm_message_unref ( message );
	
	/**********************************************/
	// Close the connection
	lm_connection_close ( connection1, NULL );
	// Remove a reference on connection
	lm_connection_unref ( connection1 );
	free_data ( connect_data1 );
	
	// Close the connection
	lm_connection_close ( connection2, NULL );
	// Remove a reference on connection
	lm_connection_unref ( connection2 );
	
	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_send_multiple_connL passed" ) );
	
	return KErrNone;
}
//--------------------------------------------------------------------------------
// function_name    : handle_block_contact
// description      : callback function to handle messages
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_block_contact ( LmMessageHandler* /*handler*/,
                  LmConnection*     /*connection*/,
                  LmMessage*        reply,
                  gpointer          user_data )
    {
    GMainLoop *main_loop = ( GMainLoop * )user_data;	
	LmMessageSubType  type;
	LmMessageNode *q_node,*item_node;
	type = lm_message_get_sub_type (reply); 
    /*
    <iq type='result' id='edit1' to='romeo@example.net/orchard'/>
    */ 	
    /*
    <iq to='romeo@example.net/orchard' type='set' id='push1'>
		  <query xmlns='jabber:iq:privacy'>
		    <list name='public'/>
		  </query>
	</iq>

	<iq to='romeo@example.net/home' type='set' id='push2'>
		  <query xmlns='jabber:iq:privacy'>
		    <list name='public'/>
		  </query>
	</iq>
	*/
       	
	switch (type) 
    	{
    	case LM_MESSAGE_SUB_TYPE_RESULT:        		        		
    		break;
    	case LM_MESSAGE_SUB_TYPE_ERROR:          	          		
    		//return NULL;
    		break;
    //	default:
    	//	g_assert_not_reached ();
    	//	break;
    	} 
    
   //	q_node = lm_message_node_get_child (reply->node, "query");

	if (!q_node) 
    	{
   		//return NULL;
    	}
    	
    //item_node = lm_message_node_get_child (q_node, "id");
	const char* reply_string;
    reply_string = lm_message_node_get_attribute(reply->node,"id");  
	
	if ( main_loop )
		{
		g_main_loop_quit ( main_loop );
		}	
	
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    }  
//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_block_contactL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_block_contactL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	LmMessageNode* q_node = NULL;
	LmMessageNode* item_node1 = NULL;
	LmMessageNode* item_node2 = NULL;
	LmMessageNode* item_node3 = NULL;
	
	LmMessageHandler *handler = NULL;
	
	iLog->Log ( _L ( "In lm_connection_block_contactL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );

	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
	iLog->Log ( _L ( "after lm_connection_open" ) );
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    

	main_loop = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }

	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	g_free ( username );
	/**********************************/	
  	/*<iq>
  		<query xmlns='jabber:iq:privacy'>
    		<list name='foo'>
			    <item
			          type='[jid|group|subscription]'
			          value='bar'
			          action='[allow|deny]'
			          order='unsignedInt'>
			        [<message/>]
			        [<presence-in/>]
			        [<presence-out/>]
			        [<iq/>]
      			</item>
    		</list>
  		</query>
	</iq>*/

	// Construct a message with type=LM_MESSAGE_TYPE_IQ
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_SET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:privacy" );
    item_node1 = lm_message_node_add_child ( q_node, "list", NULL );
    lm_message_node_set_attribute ( item_node1, "name", "public" );
    item_node2 = lm_message_node_add_child ( item_node1, "item", NULL );
    lm_message_node_set_attribute ( item_node2, "type", "jid" );
    lm_message_node_set_attribute ( item_node2, "value", "rakesh_harsh@chat.gizmoproject.com" );
    lm_message_node_set_attribute ( item_node2, "action", "deny" );
    lm_message_node_set_attribute ( item_node2, "order", "1" );
    item_node3 = lm_message_node_add_child ( item_node2, "presence-in", NULL );
    item_node3 = lm_message_node_add_child ( item_node2, "message", NULL );
    
    
    // Send message to the server
	handler = lm_message_handler_new ( handle_block_contact, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_add_contactL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);

	/**********************************/	
   // Construct a message with type=LM_MESSAGE_TYPE_IQ
	/*
	<iq from='romeo@example.net/orchard' type='get' id='getlist2'>
		  <query xmlns='jabber:iq:privacy'>
		    <list name='public'/>
		  </query>
	</iq>
	*/
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_GET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:privacy" );
    item_node1 = lm_message_node_add_child ( q_node, "list", NULL );
    lm_message_node_set_attribute ( item_node1, "name", "public" );
    
    // Send message to the server
	handler = lm_message_handler_new ( handle_get_privacy_lists, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_get_one_privacy_listL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);
	/************************************/
   
	// Close the connection
	lm_connection_close ( connection, NULL );
	
	// Remove a reference on connection
	lm_connection_unref ( connection );
	
	
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_block_contactL passed" ) );
	
	return KErrNone;
}
//--------------------------------------------------------------------------------
// function_name    : handle_unblock_contact
// description      : callback function to handle messages
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_unblock_contact ( LmMessageHandler* /*handler*/,
                  LmConnection*     /*connection*/,
                  LmMessage*        reply,
                  gpointer          user_data )
    {
    GMainLoop *main_loop = ( GMainLoop * )user_data;	
	LmMessageSubType  type;
	LmMessageNode *q_node,*item_node;
	type = lm_message_get_sub_type (reply); 
    /*
    <iq type='result' id='edit1' to='romeo@example.net/orchard'/>
    */ 	
    /*
    <iq to='romeo@example.net/orchard' type='set' id='push1'>
		  <query xmlns='jabber:iq:privacy'>
		    <list name='public'/>
		  </query>
	</iq>

	<iq to='romeo@example.net/home' type='set' id='push2'>
		  <query xmlns='jabber:iq:privacy'>
		    <list name='public'/>
		  </query>
	</iq>
	*/
       	
	switch (type) 
    	{
    	case LM_MESSAGE_SUB_TYPE_RESULT:        		        		
    		break;
    	case LM_MESSAGE_SUB_TYPE_ERROR:          	          		
    		//return NULL;
    		break;
    //	default:
    	//	g_assert_not_reached ();
    	//	break;
    	} 
    
   //	q_node = lm_message_node_get_child (reply->node, "query");

	if (!q_node) 
    	{
   		//return NULL;
    	}
    	
    //item_node = lm_message_node_get_child (q_node, "id");
	const char* reply_string;
    reply_string = lm_message_node_get_attribute(reply->node,"id");  
	
	if ( main_loop )
		{
		g_main_loop_quit ( main_loop );
		}	
	
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    } 
//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_unblock_contactL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_unblock_contactL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	LmMessageNode* q_node = NULL;
	LmMessageNode* item_node1 = NULL;
	LmMessageNode* item_node2 = NULL;
	LmMessageNode* item_node3 = NULL;	
	LmMessageHandler *handler = NULL;
	
	iLog->Log ( _L ( "In lm_connection_unblock_contactL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    

	
	main_loop = g_main_loop_new ( NULL, FALSE );	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop ); 
	g_free ( username );
	
	/**********************************/	
    // Construct a message with type=LM_MESSAGE_TYPE_IQ
	main_loop = g_main_loop_new ( NULL, FALSE );
	
	/*<iq>
  		<query xmlns='jabber:iq:privacy'>
    		<list name='foo'>
			    <item
			          type='[jid|group|subscription]'
			          value='bar'
			          action='[allow|deny]'
			          order='unsignedInt'>
			        [<message/>]
			        [<presence-in/>]
			        [<presence-out/>]
			        [<iq/>]
      			</item>
    		</list>
  		</query>
	</iq>*/

	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_SET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:privacy" );
    item_node1 = lm_message_node_add_child ( q_node, "list", NULL );
    lm_message_node_set_attribute ( item_node1, "name", "public" );
    item_node2 = lm_message_node_add_child ( item_node1, "item", NULL );
    lm_message_node_set_attribute ( item_node2, "type", "jid" );
    lm_message_node_set_attribute ( item_node2, "value", "rakesh_harsh@chat.gizmoproject.com" );
    lm_message_node_set_attribute ( item_node2, "action", "allow" );
    lm_message_node_set_attribute ( item_node2, "order", "1" );
    item_node3 = lm_message_node_add_child ( item_node2, "presence-in", NULL );
    item_node3 = lm_message_node_add_child ( item_node2, "message", NULL );
    
    // Send message to the server
	handler = lm_message_handler_new ( handle_unblock_contact, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_connection_unblock_contactL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);
	/**********************************/	
   // Construct a message with type=LM_MESSAGE_TYPE_IQ
	/*
	<iq from='romeo@example.net/orchard' type='get' id='getlist2'>
		  <query xmlns='jabber:iq:privacy'>
		    <list name='public'/>
		  </query>
	</iq>
	*/
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_GET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:privacy" );
    item_node1 = lm_message_node_add_child ( q_node, "list", NULL );
    lm_message_node_set_attribute ( item_node1, "name", "public" );
    
    // Send message to the server
	handler = lm_message_handler_new ( handle_get_privacy_lists, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_get_one_privacy_listL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);
	/************************************/
   
	// Close the connection
	lm_connection_close ( connection, NULL );
	
	// Remove a reference on connection
	lm_connection_unref ( connection );
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_unblock_contactL passed" ) );
	
	return KErrNone;
}

//--------------------------------------------------------------------------------
// function_name    : handle_set_active_list
// description      : callback function to handle messages
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_set_active_list ( LmMessageHandler*/* handler*/,
                  LmConnection*     /*connection*/,
                  LmMessage*        reply,
                  gpointer          user_data )
    {
    GMainLoop *main_loop = ( GMainLoop * )user_data;	
	LmMessageSubType  type;
	LmMessageNode *error_node = NULL;
	LmMessageNode *item_node = NULL;
	type = lm_message_get_sub_type (reply); 
    /*
    <iq type='result' id='edit1' to='romeo@example.net/orchard'/>
    */ 	
       	
	switch (type) 
    	{
    	case LM_MESSAGE_SUB_TYPE_RESULT:        		        		
    		{
    		const char* id;
    		id = lm_message_node_get_attribute(reply->node,"id");  
    		break;
    		}
    	case LM_MESSAGE_SUB_TYPE_ERROR:
    		{
    		/*<iq to='romeo@example.net/orchard' type='error' id='active2'>
			  <query xmlns='jabber:iq:privacy'>
			    <active name='The Empty Set'/>
			  </query>
			  <error code='404' type='cancel'>
			    <item-not-found
			        xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>
			  </error>
			</iq>*/
    		error_node = lm_message_node_get_child(reply->node,"error");
    		const char* error_code;
    		error_code = lm_message_node_get_attribute(error_node,"code");  
			break;
    		}
    //	default:
    	//	g_assert_not_reached ();
    	//	break;
    	} 
	if ( main_loop )
		{
		g_main_loop_quit ( main_loop );
		}	
	
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    } 
    
//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_set_active_listL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_set_active_listL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	LmMessageNode* q_node, *item_node1, *item_node2 = NULL;
	LmMessageHandler *handler = NULL;
	
	iLog->Log ( _L ( "In lm_connection_set_active_listL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	
	main_loop = g_main_loop_new ( NULL, FALSE );	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	g_free ( username );
	
	// Construct a message with type=LM_MESSAGE_TYPE_IQ
	/*
	<iq from='romeo@example.net/orchard' type='set' id='active1'>
		  <query xmlns='jabber:iq:privacy'>
		    <active name='special'/>
		  </query>
	</iq>
	*/
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_SET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:privacy" );
    item_node1 = lm_message_node_add_child ( q_node, "active", NULL );
    lm_message_node_set_attribute ( item_node1, "name", "public" );
    
    // Send message to the server
	handler = lm_message_handler_new ( handle_set_active_list, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_add_contactL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);

   
	// Close the connection
	lm_connection_close ( connection, NULL );	
	// Remove a reference on connection
	lm_connection_unref ( connection );	
	free_data ( connect_data );	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_set_active_listL passed" ) );	
	return KErrNone;
}

//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_set_unexist_active_listL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_set_unexist_active_listL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	LmMessageNode* q_node, *item_node1, *item_node2 = NULL;
	LmMessageHandler *handler = NULL;
	
	iLog->Log ( _L ( "In lm_connection_set_unexist_active_listL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	
	main_loop = g_main_loop_new ( NULL, FALSE );	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	g_free ( username );
	
	// Construct a message with type=LM_MESSAGE_TYPE_IQ
	/*
	<iq from='romeo@example.net/orchard' type='set' id='active1'>
		  <query xmlns='jabber:iq:privacy'>
		    <active name='special'/>
		  </query>
	</iq>
	*/
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_SET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:privacy" );
    item_node1 = lm_message_node_add_child ( q_node, "active", NULL );
    lm_message_node_set_attribute ( item_node1, "name", "publicunexist" );
    
    // Send message to the server
	handler = lm_message_handler_new ( handle_set_active_list, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_add_contactL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);

   
	// Close the connection
	lm_connection_close ( connection, NULL );	
	// Remove a reference on connection
	lm_connection_unref ( connection );	
	free_data ( connect_data );	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_set_unexist_active_listL passed" ) );	
	return KErrNone;
}

//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_set_decline_active_listL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_set_decline_active_listL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	LmMessageNode* q_node, *item_node1, *item_node2 = NULL;
	LmMessageHandler *handler = NULL;
	
	iLog->Log ( _L ( "In lm_connection_set_decline_active_listL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	
	main_loop = g_main_loop_new ( NULL, FALSE );	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop);
	g_main_loop_unref ( main_loop );
	g_free ( username );
	
	// Construct a message with type=LM_MESSAGE_TYPE_IQ
	/*
	<iq from='romeo@example.net/orchard' type='set' id='active3'>
		  <query xmlns='jabber:iq:privacy'>
		    <active/>
		  </query>
	</iq>

	*/
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_SET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:privacy" );
    item_node1 = lm_message_node_add_child ( q_node, "active", NULL );
    
    // Send message to the server
	handler = lm_message_handler_new ( handle_set_active_list, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_connection_set_decline_active_listL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);

   
	// Close the connection
	lm_connection_close ( connection, NULL );	
	// Remove a reference on connection
	lm_connection_unref ( connection );	
	free_data ( connect_data );	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_set_decline_active_listL passed" ) );	
	return KErrNone;
}

//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_get_one_privacy_listL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_get_one_privacy_listL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	LmMessageNode* q_node, *item_node1 = NULL;
	LmMessageHandler *handler = NULL;
	
	iLog->Log ( _L ( "In lm_connection_get_one_privacy_listL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	
	main_loop = g_main_loop_new ( NULL, FALSE );	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	g_free ( username );
	
	// Construct a message with type=LM_MESSAGE_TYPE_IQ
	/*
	<iq from='romeo@example.net/orchard' type='get' id='getlist2'>
		  <query xmlns='jabber:iq:privacy'>
		    <list name='public'/>
		  </query>
	</iq>
	*/
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_GET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:privacy" );
    item_node1 = lm_message_node_add_child ( q_node, "list", NULL );
    lm_message_node_set_attribute ( item_node1, "name", "public" );
    
    // Send message to the server
	handler = lm_message_handler_new ( handle_get_privacy_lists, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_get_one_privacy_listL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);

   
	// Close the connection
	lm_connection_close ( connection, NULL );	
	// Remove a reference on connection
	lm_connection_unref ( connection );	
	free_data ( connect_data );	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_get_one_privacy_listL passed" ) );	
	return KErrNone;
}

//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_set_one_privacy_listL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_set_one_privacy_listL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	LmMessageNode* q_node, *item_node1, *item_node2 = NULL;
	LmMessageHandler *handler = NULL;
	
	iLog->Log ( _L ( "In lm_connection_set_one_privacy_listL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );

	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	
	main_loop = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }

	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	g_free ( username );
	
	// Construct a message with type=LM_MESSAGE_TYPE_IQ
	/*
	<iq from='romeo@example.net/orchard' type='set' id='edit1'>
		<query xmlns='jabber:iq:privacy'>
		    <list name='public'>
		      <item type='jid'
		            value='tybalt@example.com'
		            action='deny'
		            order='3'/>
		      <item type='jid'
		            value='paris@example.org'
		            action='deny'
		            order='5'/>
		      <item action='allow' order='68'/>
		    </list>		    
  		</query>
	</iq>
	*/
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_SET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:privacy" );
    item_node1 = lm_message_node_add_child ( q_node, "list", NULL );
    lm_message_node_set_attribute ( item_node1, "name", "public" );
    item_node2 = lm_message_node_add_child ( item_node1, "item", NULL );
    lm_message_node_set_attribute ( item_node2, "type", "jid" );
    lm_message_node_set_attribute ( item_node2, "value", "rakesh_harsh@chat.gizmoproject.com" );
    lm_message_node_set_attribute ( item_node2, "action", "deny" );
    lm_message_node_set_attribute ( item_node2, "order", "3" );
    
    // Send message to the server
	handler = lm_message_handler_new ( handle_set_privacy_lists, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_connection_set_one_privacy_listL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);

   
	// Close the connection
	lm_connection_close ( connection, NULL );
	
	// Remove a reference on connection
	lm_connection_unref ( connection );
	
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_set_one_privacy_listL passed" ) );
	
	return KErrNone;
}

//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_create_one_privacy_listL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_create_one_privacy_listL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	LmMessageNode* q_node, *item_node1, *item_node2 = NULL;
	LmMessageHandler *handler = NULL;
	
	iLog->Log ( _L ( "In lm_connection_create_one_privacy_listL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );

	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    

	
	main_loop = g_main_loop_new ( NULL, FALSE );	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }

	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );	    
	g_free ( username );
	
	// Construct a message with type=LM_MESSAGE_TYPE_IQ
	/*
	<iq from='romeo@example.net/orchard' type='set' id='edit1'>
		<query xmlns='jabber:iq:privacy'>
		    <list name='public'>
		      <item type='jid'
		            value='tybalt@example.com'
		            action='deny'
		            order='3'/>
		      <item type='jid'
		            value='paris@example.org'
		            action='deny'
		            order='5'/>
		      <item action='allow' order='68'/>
		    </list>		    
  		</query>
	</iq>
	*/
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_SET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:privacy" );
    item_node1 = lm_message_node_add_child ( q_node, "list", NULL );
    lm_message_node_set_attribute ( item_node1, "name", "publicnewlist" );
    item_node2 = lm_message_node_add_child ( item_node1, "item", NULL );
    lm_message_node_set_attribute ( item_node2, "type", "jid" );
    lm_message_node_set_attribute ( item_node2, "value", "rakesh_harsh@chat.gizmoproject.com" );
    lm_message_node_set_attribute ( item_node2, "action", "allow" );
    lm_message_node_set_attribute ( item_node2, "order", "10" );
    
    // Send message to the server
	handler = lm_message_handler_new ( handle_set_privacy_lists, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_connection_create_one_privacy_listL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);
	g_main_loop_unref ( main_loop );
	/*****************************/
	//Now retrieve all the privacy lists and see if it is created
	// Construct a message with type=LM_MESSAGE_TYPE_IQ
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_GET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:privacy" );
    // Send message to the server
	handler = lm_message_handler_new ( handle_get_privacy_lists, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_add_contactL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);
	g_main_loop_unref ( main_loop );
	/*******************************/
   
	// Close the connection
	lm_connection_close ( connection, NULL );	
	// Remove a reference on connection
	lm_connection_unref ( connection );	
	free_data ( connect_data );	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_create_one_privacy_listL passed" ) );
	
	return KErrNone;
}

//--------------------------------------------------------------------------------
// function_name    : handle_get_many_privacy_lists
// description      : callback function to handle messages
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_get_many_privacy_lists ( LmMessageHandler*/* handler*/,
                  LmConnection*     /*connection*/,
                  LmMessage*        reply,
                  gpointer          user_data )
    {
    GMainLoop *main_loop = ( GMainLoop * )user_data;	
	LmMessageSubType  type;
	LmMessageNode *error_node = NULL;
	LmMessageNode *item_node = NULL;
	type = lm_message_get_sub_type (reply); 
    
    /*<iq to='romeo@example.net/orchard' type='error/result' id='getlist6'>   	*/
	switch (type) 
    	{
    	case LM_MESSAGE_SUB_TYPE_RESULT:        		        		
    		{
    		const char* id;
    		id = lm_message_node_get_attribute(reply->node,"id");  
    		break;
    		}
    	case LM_MESSAGE_SUB_TYPE_ERROR:
    		{
    		/*
		    <iq to='romeo@example.net/orchard' type='error' id='getlist6'>
			    <error type='modify'>
			    	<bad-request xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>
			    </error>
		    </iq>
			*/ 	
    		error_node = lm_message_node_get_child(reply->node,"error");
    		const char* error_code;
    		error_code = lm_message_node_get_attribute(error_node,"code");  
			const char* error_type;
    		error_type = lm_message_node_get_attribute(error_node,"type");  
			break;
    		}
    //	default:
    	//	g_assert_not_reached ();
    	//	break;
    	} 
	if ( main_loop )
		{
		g_main_loop_quit ( main_loop );
		}	
	
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    } 
//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_get_many_privacy_list_from_listnameL
// Description  : Asynchronous call to get more than one privacy list details by 
//				  specifying the name. The server should return error, since we 
//  			  can only retrieve info of one list at a time
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_get_many_privacy_list_from_listnameL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	LmMessageNode* q_node, *item_node1, *item_node2, *item_node3 = NULL;
	LmMessageHandler *handler = NULL;
	
	iLog->Log ( _L ( "In lm_connection_get_many_privacy_list_from_listname" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	
	main_loop = g_main_loop_new ( NULL, FALSE );	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	g_free ( username );
	
	// Construct a message with type=LM_MESSAGE_TYPE_IQ
	/*
	<iq from='romeo@example.net/orchard' type='get' id='getlist2'>
		  <query xmlns='jabber:iq:privacy'>
		    <list name='public'/>
		    <list name='active'/>
		    <list name='default'/>		    
		  </query>
	</iq>
	*/
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_GET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:privacy" );
    item_node1 = lm_message_node_add_child ( q_node, "list", NULL );
    lm_message_node_set_attribute ( item_node1, "name", "public" );
    item_node2 = lm_message_node_add_child ( q_node, "list", NULL );
    lm_message_node_set_attribute ( item_node2, "name", "private" );
    item_node3 = lm_message_node_add_child ( q_node, "list", NULL );
    lm_message_node_set_attribute ( item_node3, "name", "default" );
    
    
    // Send message to the server
	handler = lm_message_handler_new ( handle_get_many_privacy_lists, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_get_many_privacy_list_from_listname failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);

   
	// Close the connection
	lm_connection_close ( connection, NULL );	
	// Remove a reference on connection
	lm_connection_unref ( connection );	
	free_data ( connect_data );	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_get_many_privacy_list_from_listname passed" ) );	
	return KErrNone;
}

//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_get_one_unexist_privacy_listL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_get_one_unexist_privacy_listL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	LmMessageNode* q_node, *item_node = NULL;
	LmMessageHandler *handler = NULL;
	
	iLog->Log ( _L ( "In lm_connection_get_one_unexist_privacy_list" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	
	main_loop = g_main_loop_new ( NULL, FALSE );	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	g_free ( username );
	
	// Construct a message with type=LM_MESSAGE_TYPE_IQ
	/*
	<iq from='romeo@example.net/orchard' type='get' id='getlist2'>
		  <query xmlns='jabber:iq:privacy'>
		    <list name='publicunexist'/>
		  </query>
	</iq>
	*/
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_GET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:privacy" );
    item_node = lm_message_node_add_child ( q_node, "list", NULL );
    lm_message_node_set_attribute ( item_node, "name", "publicunexist" );
    
    // Send message to the server
	handler = lm_message_handler_new ( handle_get_privacy_lists, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_get_one_unexist_privacy_list failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);

   
	// Close the connection
	lm_connection_close ( connection, NULL );	
	// Remove a reference on connection
	lm_connection_unref ( connection );	
	free_data ( connect_data );	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_get_one_unexist_privacy_list passed" ) );	
	return KErrNone;
}


//-----------------------------------------------------------------------------
// Ctstlm::lm_connection_set_one_unexist_privacy_listL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_connection_set_one_unexist_privacy_listL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	LmMessageNode* q_node, *item_node1, *item_node2 = NULL;
	LmMessageHandler *handler = NULL;
	
	iLog->Log ( _L ( "In lm_connection_set_one_unexist_privacy_list" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	
	main_loop = g_main_loop_new ( NULL, FALSE );	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	g_free ( username );
	
// Construct a message with type=LM_MESSAGE_TYPE_IQ
	/*
	<iq from='romeo@example.net/orchard' type='set' id='edit1'>
		<query xmlns='jabber:iq:privacy'>
		    <list name='publicunexist'>
		      <item type='jid'
		            value='tybalt@example.com'
		            action='deny'
		            order='3'/>
		      <item type='jid'
		            value='paris@example.org'
		            action='deny'
		            order='5'/>
		      <item action='allow' order='68'/>
		    </list>
  		</query>
	</iq>
	*/
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_SET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "jabber:iq:privacy" );
    item_node1 = lm_message_node_add_child ( q_node, "list", NULL );
    lm_message_node_set_attribute ( item_node1, "name", "publicunexist" );
    item_node2 = lm_message_node_add_child ( item_node1, "item", NULL );
    lm_message_node_set_attribute ( item_node2, "type", "jid" );
    lm_message_node_set_attribute ( item_node2, "value", "rakesh_harsh@chat.gizmoproject.com" );
    lm_message_node_set_attribute ( item_node2, "action", "deny" );
    lm_message_node_set_attribute ( item_node2, "order", "3" );
    
    // Send message to the server
	handler = lm_message_handler_new ( handle_set_privacy_lists, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_add_contactL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);

   
	// Close the connection
	lm_connection_close ( connection, NULL );	
	// Remove a reference on connection
	lm_connection_unref ( connection );	
	free_data ( connect_data );	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_set_one_unexist_privacy_list passed" ) );	
	return KErrNone;
}

//--------------------------------------------------------------------------------
// function_name    : handle_service_discovery
// description      : callback function to handle messages
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_service_discovery ( LmMessageHandler*/* handler*/,
                  LmConnection*     /*connection*/,
                  LmMessage*        reply,
                  gpointer          user_data )
    {
    GMainLoop *main_loop = ( GMainLoop * )user_data;	
	LmMessageSubType  type;
	LmMessageNode *q_node = NULL;
	LmMessageNode *item_node = NULL;
	type = lm_message_get_sub_type (reply); 
    /*
    <iq from='capulet.com' to='juliet@capulet.com/chamber' 
		    type='result' id='disco1'>
		<query xmlns='http://jabber.org/protocol/disco#info'>
		    ...
		    <feature var='urn:xmpp:blocking'/>
		    ...
  		</query>
	</iq>
    */ 	
       	
	switch (type) 
    	{
    	case LM_MESSAGE_SUB_TYPE_RESULT:        		        		
    		{
    		const char* reply_string;
    		reply_string = lm_message_node_get_attribute(reply->node,"id");  
			break;
    		}
    	case LM_MESSAGE_SUB_TYPE_ERROR:          	          		
    		{
    		/*
		    <iq to='romeo@example.net/orchard' type='error' id='getlist6'>
			    <error type='modify'>
			    	<bad-request xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>
			    </error>
		    </iq>
			*/ 	
    		if (reply->node) 
		    	{
		    	LmMessageNode* error_node = NULL;
		    	error_node = lm_message_node_get_child(reply->node,"error");
	    		const char* error_code;
	    		error_code = lm_message_node_get_attribute(error_node,"code");  
				const char* error_type;
	    		error_type = lm_message_node_get_attribute(error_node,"type");  
				item_node = lm_message_node_get_child (reply->node, "feature-not-implemented");
		    	}
			break;
			}
    //	default:
    	//	g_assert_not_reached ();
    	//	break;
    	} 
    	
    
	if ( main_loop )
		{
		g_main_loop_quit ( main_loop );
		}	
	
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    }   
//-----------------------------------------------------------------------------
// Ctstlm::lm_service_discoveryL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_service_discoveryL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message;
	LmMessageNode * q_node = NULL;
	LmMessageHandler *handler = NULL;
	iLog->Log ( _L ( "In lm_service_discoveryL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );

	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );
	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	
	main_loop = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }

	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );	    
	g_free ( username );
	
	// Construct a message
	/*
	<iq from='juliet@capulet.com/chamber' to='capulet.com' type='get' id='disco1'>
  		<query xmlns='http://jabber.org/protocol/disco#info'/>
	</iq>
	*/
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type (connect_data->server,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_GET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "http://jabber.org/protocol/disco#info" );
    
	// Send message to the server
	handler = lm_message_handler_new ( handle_service_discovery, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_service_discoveryL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);
	
	
	// Close the connection
	lm_connection_close ( connection, NULL );
	// Remove a reference on connection
	lm_connection_unref ( connection );
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_service_discoveryL passed" ) );
	
	return KErrNone;
}

//-----------------------------------------------------------------------------
// Ctstlm::lm_service_discovery_connected_resourceL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_service_discovery_connected_resourceL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message;
	LmMessageNode * q_node = NULL;
	LmMessageHandler *handler = NULL;
	iLog->Log ( _L ( "In lm_service_discovery_connected_resourceL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );

	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );
	
	g_main_loop_run ( main_loop );

	g_main_loop_unref ( main_loop );    

	
	main_loop = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }

	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );	    
	g_free ( username );
	
	// Construct a message
	/*
	<iq from='juliet@capulet.com/chamber' to='capulet.com' type='get' id='disco1'>
  		<query xmlns='http://jabber.org/protocol/disco#info'/>
	</iq>
	*/
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type ("testlmnokia@chat.gizmoproject.com/mytest12345rt17",
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_GET );
    q_node = lm_message_node_add_child ( message->node, "query", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "http://jabber.org/protocol/disco#info" );
    
	// Send message to the server
	handler = lm_message_handler_new ( handle_service_discovery, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_add_contactL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);
	
	
	// Close the connection
	lm_connection_close ( connection, NULL );
	// Remove a reference on connection
	lm_connection_unref ( connection );	
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_service_discovery_connected_resourceL passed" ) );
	
	return KErrNone;
}


//--------------------------------------------------------------------------------
// function_name    : handle_simple_communications_blocking
// description      : callback function to handle messages
//					  XEP-0191: Simple Communications Blocking
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_simple_communications_blocking ( LmMessageHandler*/* handler*/,
                  LmConnection*     /*connection*/,
                  LmMessage*        reply,
                  gpointer          user_data )
    {
    GMainLoop *main_loop = ( GMainLoop * )user_data;	
	LmMessageSubType  type;
	LmMessageNode *q_node = NULL;
	LmMessageNode *item_node = NULL;
	type = lm_message_get_sub_type (reply); 
    /*
    <iq from='capulet.com' to='juliet@capulet.com/chamber' 
		    type='result' id='disco1'>
		<query xmlns='http://jabber.org/protocol/disco#info'>
		    ...
		    <feature var='urn:xmpp:blocking'/>
		    ...
  		</query>
	</iq>
    */ 	
       	
	switch (type) 
    	{
    	case LM_MESSAGE_SUB_TYPE_RESULT:        		        		
    		{
    		const char* reply_string;
    		reply_string = lm_message_node_get_attribute(reply->node,"id");  
			break;
    		}
    	case LM_MESSAGE_SUB_TYPE_ERROR:          	          		
    		{
    		/*
		    <iq to='romeo@example.net/orchard' type='error' id='getlist6'>
			    <error type='modify'>
			    	<bad-request xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>
			    </error>
		    </iq>
			*/ 	
    		if (reply->node) 
		    	{
		    	LmMessageNode* error_node = NULL;
		    	error_node = lm_message_node_get_child(reply->node,"error");
	    		const char* error_code;
	    		error_code = lm_message_node_get_attribute(error_node,"code");  
				const char* error_type;
	    		error_type = lm_message_node_get_attribute(error_node,"type");  
				item_node = lm_message_node_get_child (error_node, "feature-not-implemented");
		    	}
			break;
			}
    //	default:
    	//	g_assert_not_reached ();
    	//	break;
    	} 
    	
    
	if ( main_loop )
		{
		g_main_loop_quit ( main_loop );
		}	
	
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    }   
//-----------------------------------------------------------------------------
// Ctstlm::lm_simple_communications_blockingL
// Description  : Asynchronous call to send a message
//				  XEP-0191: Simple Communications Blocking
//				  Not implemented by Gizmo
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_simple_communications_blockingL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message;
	LmMessageNode * q_node = NULL;
	LmMessageHandler *handler = NULL;
	iLog->Log ( _L ( "In lm_simple_communications_blockingL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	
	main_loop = g_main_loop_new ( NULL, FALSE );	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }

	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );	    
	g_free ( username );
	
	// Construct a message
	/*
	<iq type='get' id='blocklist1'>
  		<blocklist xmlns='urn:xmpp:blocking'/>
	</iq>
	*/
	main_loop = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new_with_sub_type (NULL,
                                         LM_MESSAGE_TYPE_IQ,
                                         LM_MESSAGE_SUB_TYPE_GET );
    q_node = lm_message_node_add_child ( message->node, "blocklist", NULL );
    lm_message_node_set_attribute ( q_node, "xmlns", "urn:xmpp:blocking" );
    
	// Send message to the server
	handler = lm_message_handler_new ( handle_simple_communications_blocking, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_send_with_reply in lm_simple_communications_blockingL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	lm_message_unref ( message );    
	lm_message_handler_unref(handler);
	
	
	// Close the connection
	lm_connection_close ( connection, NULL );
	// Remove a reference on connection
	lm_connection_unref ( connection );	
	free_data ( connect_data );	
	// Message sending passed
	iLog->Log ( _L ( "lm_simple_communications_blockingL passed" ) );
	
	return KErrNone;
}
//--------------------------------------------------------------------------------
// function_name    : handle_send_im
// description      : callback function to handle messages
//					  XEP-0191: Simple Communications Blocking
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_send_im ( LmMessageHandler*/* handler*/,
                  LmConnection*     /*connection*/,
                  LmMessage*        reply,
                  gpointer          user_data )
    {
    GMainLoop *main_loop = ( GMainLoop * )user_data;	
	LmMessageSubType  type;
	LmMessageNode *item_node = NULL;
	type = lm_message_get_sub_type (reply); 
   
    switch (type) 
    	{
    	case LM_MESSAGE_SUB_TYPE_RESULT:        		        		
    		{
    		const char* reply_string;
    		reply_string = lm_message_node_get_attribute(reply->node,"id");  
			break;
    		}
    	case LM_MESSAGE_SUB_TYPE_ERROR:          	          		
    		{
    		/* //with gizmo
		    <message from='testlmnokia1111@chat.gizmoproject.com' 
		    	to='prima@chat.gizmoproject.com/mytest12345rt' 
				type='error' id='98332763125'>
				 <body>Hello</body>
				<error code='503' type='cancel'>
				<service-unavailable xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>
				</error>
			</message>
			*/ 	
    		if (reply->node) 
		    	{
		    	LmMessageNode* error_node = NULL;
		    	const char* msg_error_type;
	    		msg_error_type = lm_message_node_get_attribute(reply->node,"type");  
				error_node = lm_message_node_get_child(reply->node,"error");
	    		const char* error_code;
	    		error_code = lm_message_node_get_attribute(error_node,"code");  
				const char* error_type;
	    		error_type = lm_message_node_get_attribute(error_node,"type");  
				item_node = lm_message_node_get_child (error_node, "feature-not-implemented");
				if(item_node == NULL)
					{
					item_node = lm_message_node_get_child (error_node, "service-unavailable");
					}
		    	}
			break;
			}
    //	default:
    	//	g_assert_not_reached ();
    	//	break;
    	} 
    	
    
	if ( main_loop )
		{
		g_main_loop_quit ( main_loop );
		}	
	
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    } 
//-----------------------------------------------------------------------------
// Ctstlm::lm_send_im_with_replyL
// Description  : Asynchronous call to send a message. Needs a response
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_send_im_with_replyL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message;
	LmMessageHandler *handler = NULL;
	iLog->Log ( _L ( "In lm_send_im_with_replyL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );

	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    

	
	main_loop = g_main_loop_new ( NULL, FALSE );	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	g_free ( username );
	
	//------------------------------------------------------------------------
	//Send own presence
//	message = lm_message_new_with_sub_type ( NULL,
//	                                     LM_MESSAGE_TYPE_PRESENCE,
//	                                     LM_MESSAGE_SUB_TYPE_AVAILABLE );
	
   // gboolean result = lm_connection_send ( connection, message, NULL );	
   	//	lm_message_unref ( message );
	//------------------------------------------------------------------------
	//
	// Register a handler to recieve msgs
	//
	main_loop = g_main_loop_new ( NULL, FALSE );
	
	
	// Construct a message
	message = lm_message_new ( connect_data->msg_data->recipient,               
	                           LM_MESSAGE_TYPE_MESSAGE );
	lm_message_node_add_child ( message->node, "body", 
	                            connect_data->msg_data->message);	
	// Send message to the server
	if ( !lm_connection_send ( connection, message, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_send_im_with_replyL failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		lm_message_unref ( message );
		lm_message_handler_unref(handler);
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }	
	
	handler = lm_message_handler_new ( 
	                     (LmHandleMessageFunction)handle_send_im,
	                     main_loop,
	                     NULL );
	
	lm_connection_register_message_handler ( connection,
	                                         handler,
	                                         LM_MESSAGE_TYPE_IQ,
	                                         LM_HANDLER_PRIORITY_FIRST );
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	
	
	
	// Remove a reference on message
	lm_message_unref ( message );
	lm_message_handler_unref(handler);
	// Close the connection
	lm_connection_close ( connection, NULL );
	// Remove a reference on connection
	lm_connection_unref ( connection );
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_send_im_with_replyL passed" ) );
	
	return KErrNone;
}

//-----------------------------------------------------------------------------
// Ctstlm::lm_receive_any_messageL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send and receive
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_receive_any_messageL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	LmMessageHandler *handler = NULL;
	HandleData *handle_data = NULL;
	
	iLog->Log ( _L ( "In lm_receive_any_message" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	
	main_loop = g_main_loop_new ( NULL, FALSE );	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }

	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	g_free ( username );

	//------------------------------------------------------------------------
	//Send own presence
	message = lm_message_new_with_sub_type ( NULL,
	                                     LM_MESSAGE_TYPE_PRESENCE,
	                                     LM_MESSAGE_SUB_TYPE_AVAILABLE );
	
    gboolean result = lm_connection_send ( connection, message, NULL );	
   	
	//------------------------------------------------------------------------
	//
	// Register a handler to recieve msgs
	//
	main_loop = g_main_loop_new ( NULL, FALSE );
	handler = lm_message_handler_new ( 
	                     (LmHandleMessageFunction)handle_messages,
	                     main_loop,
	                     NULL );
	
	lm_connection_register_message_handler ( connection,
	                                         handler,
	                                         LM_MESSAGE_TYPE_MESSAGE,
	                                         LM_HANDLER_PRIORITY_FIRST );
	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	//------------------------------------------------------------------------
	lm_message_unref(message);
	lm_message_handler_unref(handler);
	// Close the connection
	lm_connection_close ( connection, NULL );	
	// Remove a reference on connection
	lm_connection_unref ( connection );		
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_send passed" ) );
	
	return KErrNone;
}

//--------------------------------------------------------------------------------
// function_name    : handle_multiple_session_messages
// description      : callback function to handle messages
// Returns          : LmHandlerResult
//--------------------------------------------------------------------------------
static LmHandlerResult
handle_multiple_session_messages ( LmMessageHandler* /*handler*/,
                  LmConnection*     /*connection*/,
                  LmMessage*        reply,
                  gpointer          user_data )
    {
    HandleData *handle_data = ( HandleData * )user_data;	
	
	if ( handle_data )
		{
		handle_data->Test_Success = RC_OK;
		LmMessageSubType  type;
		LmMessageNode *q_node = NULL;
		LmMessageNode *item_node = NULL;
		type = lm_message_get_sub_type (reply); 
	       	
		switch (type) 
	    	{
	    	case LM_MESSAGE_SUB_TYPE_RESULT:        		        		
	    		{
	    		const char* reply_string;
	    		reply_string = lm_message_node_get_attribute(reply->node,"id");  
				break;
	    		}
	    	case LM_MESSAGE_SUB_TYPE_ERROR:          	          		
	    		{
	    		if (q_node) 
			    	{
			    	LmMessageNode* error_node = NULL;
			    	error_node = lm_message_node_get_child(reply->node,"error");
		    		const char* error_code;
		    		error_code = lm_message_node_get_attribute(error_node,"code");  
					const char* error_type;
		    		error_type = lm_message_node_get_attribute(error_node,"type");  
					item_node = lm_message_node_get_child (q_node, "feature-not-implemented");
			    	}
				break;
				}
	    //	default:
	    	//	g_assert_not_reached ();
	    	//	break;
	    	} 
    	
	
	    g_main_loop_quit ( handle_data->main_loop );
		}	
	
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
    }

//-----------------------------------------------------------------------------
// Ctstlm::lm_login_multiple_sessionsL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send and receive
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_login_multiple_sessionsL ( CStifItemParser& aItem )
    {
	LmConnection *connection1  = NULL;
	LmConnection *connection2  = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	LmMessageHandler *handler1 = NULL;
	LmMessageHandler *handler2 = NULL;
	HandleData *handle_data = NULL;
	
	iLog->Log ( _L ( "In lm_receive_any_message" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection1 = lm_connection_new ( connect_data->server );
	connection2 = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection1 || !connection2 )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port and Set the JID
	lm_connection_set_port ( connection1, GTALK_SSL_PORT );
	lm_connection_set_jid ( connection1, connect_data->username );
	lm_connection_set_port ( connection2, GTALK_SSL_PORT );
	lm_connection_set_jid ( connection2, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection1, connect_data->proxy_data );
	SetProxy ( connection2, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection1 );
	SSLInit ( connection2 );
	/*********************************************************/
	//Login with connection1	
	GMainLoop	*main_loop = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );	
	if ( !lm_connection_open ( connection1, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection1 );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    	
	main_loop = g_main_loop_new ( NULL, FALSE );	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection1,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection1, NULL );
		lm_connection_unref ( connection1 );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	//Send own presence
	message = lm_message_new_with_sub_type ( NULL,
	                                     LM_MESSAGE_TYPE_PRESENCE,
	                                     LM_MESSAGE_SUB_TYPE_AVAILABLE );
    gboolean result = lm_connection_send ( connection1, message, NULL );	
	
	//Dont unref message here. It will be used to send agn .
	//lm_message_unref(message);


	// Register a handler to recieve msgs
	main_loop = g_main_loop_new ( NULL, FALSE );
	handler1 = lm_message_handler_new ( 
	                     (LmHandleMessageFunction)handle_multiple_session_messages,
	                     main_loop,
	                     NULL );
	lm_connection_register_message_handler ( connection1,
	                                         handler1,
	                                         LM_MESSAGE_TYPE_MESSAGE,
	                                         LM_HANDLER_PRIORITY_FIRST );	
	/*********************************************************/
	//Login with CONNECTION2
	GMainLoop* main_loop1 = g_main_loop_new ( NULL, FALSE );
	iLog->Log ( _L ( "before lm_connection_open" ) );	
	if ( !lm_connection_open ( connection2, ( LmResultFunction ) connection_open_cb,
	                           main_loop1, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection2 );
		g_main_loop_unref ( main_loop1 );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
	iLog->Log ( _L ( "after lm_connection_open" ) );	
	g_main_loop_run ( main_loop1 );
	g_main_loop_unref ( main_loop1 );    	
	main_loop1 = g_main_loop_new ( NULL, FALSE );	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection2,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop1,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection2, NULL );
		lm_connection_unref ( connection2 );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop1 );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop1 );
	g_main_loop_unref ( main_loop1 );
	g_free ( username );
	//Send own presence
	result = lm_connection_send ( connection2, message, NULL );	
	lm_message_unref(message);
	/*********************************************************/
	// Conn2 should have logged out. Send message should not work.	
	// Construct a message
//	main_loop1 = g_main_loop_new ( NULL, FALSE );
	message = lm_message_new ( connect_data->msg_data->recipient,               
	                           LM_MESSAGE_TYPE_MESSAGE );
	lm_message_node_add_child ( message->node, "body", 
	                            connect_data->msg_data->message);	
	// Send message to the server
	handler2 = lm_message_handler_new ( handle_send_im, main_loop, NULL );	
	if ( !lm_connection_send_with_reply ( connection1, message, handler2, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_send_im_with_replyL failed" ) );
		lm_connection_close ( connection1, NULL );
		lm_connection_unref ( connection1 );
		g_main_loop_unref ( main_loop );
		lm_message_unref ( message );
		free_data ( connect_data );		
		return KErrGeneral;
	    }	
	
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	lm_message_unref(message);
	lm_message_handler_unref(handler2);

	/***********************************************************/
	//Run the registered main loop for logout callbacks
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );
	

	// Close the connection
	lm_connection_close ( connection1, NULL );
	// Remove a reference on connection
	lm_connection_unref ( connection1 );
	
	// Close the connection
	lm_connection_close ( connection2, NULL );
	// Remove a reference on connection
	lm_connection_unref ( connection2 );
		
	free_data ( connect_data );
	
	// Message sending passed
	iLog->Log ( _L ( "lm_connection_send passed" ) );
	
	return KErrNone;
}

//-----------------------------------------------------------------------------
// Ctstlm::lm_change_own_presenceL
// Description  : Changing own presence information
// Arguements   :
//   connection	: 
//   message    : 
//   error      : 
// Returns      : 
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_change_own_presenceL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	LmMessage    *message = NULL;
	LmMessageNode *show_node = NULL;
    LmMessageNode *status_node = NULL;
    LmMessageHandler *handler = NULL;
    GMainLoop    *main_loop    = NULL;
	
	iLog->Log ( _L ( "In lm_change_own_presenceL" ) );
	
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	
	main_loop = g_main_loop_new ( NULL, FALSE );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	
	g_main_loop_run ( main_loop );

	g_main_loop_unref ( main_loop );    

	main_loop = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( connect_data );
		g_main_loop_unref ( main_loop );
		g_free ( username );
		return KErrGeneral;
	    }
	g_main_loop_run ( main_loop );
	g_main_loop_unref ( main_loop );    
	g_free ( username );
	
	/*
	<presence xml:lang='en'>
  		<show>dnd</show>
  		<status>Wooing Juliet</status>
  		<status xml:lang='cz'>Ja dvo&#x0159;&#x00ED;m Juliet</status>
	</presence>
	*/

	//publish own presence
	message = lm_message_new_with_sub_type ( NULL,
	                                     LM_MESSAGE_TYPE_PRESENCE,
	                                     LM_MESSAGE_SUB_TYPE_AVAILABLE );
	gboolean result = lm_connection_send ( connection, message, NULL );	
	//lm_connection_send_with_reply(
	// Remove a reference on message
	lm_message_unref ( message );	

	//------------------------------------------------------------------------
	//
	// Register a handler to recieve and update presence information
	//
	main_loop = g_main_loop_new ( NULL, FALSE );
		
	handler = lm_message_handler_new ( 
	                     (LmHandleMessageFunction)jabber_presence_handler_cb,
	                     main_loop,
	                     NULL );
	
	lm_connection_register_message_handler ( connection,
	                                         handler,
	                                         LM_MESSAGE_TYPE_PRESENCE,
	                                         LM_HANDLER_PRIORITY_NORMAL );
	
	g_main_loop_run ( main_loop );	
	g_main_loop_unref ( main_loop );
	lm_message_handler_unref(handler);
	//------------------------------------------------------------------------
	
	
 	
   	//Change presence again
   	message = lm_message_new_with_sub_type ( NULL,
	                                     LM_MESSAGE_TYPE_PRESENCE,
	                                     LM_MESSAGE_SUB_TYPE_AVAILABLE );
	lm_message_node_set_attributes ( message->node, "xml:lang", "en", NULL ); 
	show_node = lm_message_node_add_child ( message->node, "show", "busy" );
	status_node = lm_message_node_add_child ( message->node, "status", "goal goal goal" );
	result = lm_connection_send ( connection, message, NULL );	
	// Remove a reference on message
	lm_message_unref ( message );	
   	
   	//Change presence again
   	message = lm_message_new_with_sub_type ( NULL,
	                                     LM_MESSAGE_TYPE_PRESENCE,
	                                     LM_MESSAGE_SUB_TYPE_AVAILABLE );
	lm_message_node_set_attributes ( message->node, "xml:lang", "en", NULL ); 
	show_node = lm_message_node_add_child ( message->node, "show", "away" );
	status_node = lm_message_node_add_child ( message->node, "status", "imagine a world without nokia!" );
	result = lm_connection_send ( connection, message, NULL );	
	// Remove a reference on message
	lm_message_unref ( message );	
   	
   	//Change presence again
   	message = lm_message_new_with_sub_type ( NULL,
	                                     LM_MESSAGE_TYPE_PRESENCE,
	                                     LM_MESSAGE_SUB_TYPE_AVAILABLE );
	lm_message_node_set_attributes ( message->node, "xml:lang", "en", NULL ); 
	show_node = lm_message_node_add_child ( message->node, "show", "available" );
	status_node = lm_message_node_add_child ( message->node, "status", "there comes a time..." );
	result = lm_connection_send ( connection, message, NULL );	
	// Remove a reference on message
	lm_message_unref ( message );
	
	
	lm_connection_close ( connection, NULL );	
	// Remove a reference on connection
	lm_connection_unref ( connection );	
	free_data ( connect_data );
	// Message sending passed
	iLog->Log ( _L ( "lm_fetching_presenceL passed" ) );
	
	return KErrNone;
    }
    
//-----------------------------------------------------------------------------
// Ctstlm::lm_login_test_invalid_settingsL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_login_test_invalid_portL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	
	
	iLog->Log ( _L ( "In lm_login_test_invalid_portL" ) );
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, SSL_INVALID_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop* main_loop = g_main_loop_new ( NULL, FALSE );

	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );
	
	g_main_loop_run ( main_loop );

	g_main_loop_unref ( main_loop );    

	// Close the connection
	lm_connection_close ( connection, NULL );	
	// Remove a reference on connection
	lm_connection_unref ( connection );
	free_data ( connect_data );
	iLog->Log ( _L ( "lm_login_test_invalid_port: Invalid settings is handled correctly" ) );
	return KErrNone;
}   
  
//-----------------------------------------------------------------------------
// Ctstlm:: lm_login_test_invalid_serverL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm:: lm_login_test_invalid_serverL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	ConnectData  *connect_data = NULL;
	
	
	iLog->Log ( _L ( "In  lm_login_test_invalid_serverL" ) );
	// Read a data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, SSL_INVALID_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	GMainLoop* main_loop = g_main_loop_new ( NULL, FALSE );

	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( main_loop );
		free_data ( connect_data );		
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );
	
	g_main_loop_run ( main_loop );

	g_main_loop_unref ( main_loop );    

	// Close the connection
	lm_connection_close ( connection, NULL );	
	// Remove a reference on connection
	lm_connection_unref ( connection );
	free_data ( connect_data );
	iLog->Log ( _L ( " lm_login_test_invalid_server: Invalid settings is handled correctly" ) );
	return KErrNone;
}    
   
    
//-----------------------------------------------------------------------------
// Ctstlm::lm_memory_leak_testL
// Description  : Asynchronous call to send a message
// Arguements   :
//   connection	: an LmConnection used to send message
//   message    : LmMessage to send
//   error      : location to store error, or NULL
// Returns      : Returns TRUE if no errors were detected while sending,
//                FALSE otherwise
//-----------------------------------------------------------------------------
// 
TInt Ctstlm::lm_memory_leak_testL ( CStifItemParser& aItem )
    {
	LmConnection *connection   = NULL;
	//ConnectData  *connect_data = NULL;
	AuthData     *auth_data    = NULL;
	
	
	iLog->Log ( _L ( "In lm_memory_leak_testL" ) );
	
	auth_data = g_new0 ( AuthData, 1 );
	if ( !auth_data )
		{
		iLog->Log ( _L ( "memory allocation failed for auth_data" ) );
		return KErrGeneral;
		}	
	
	// Read a data from the CFG file
	if ( read_data ( auth_data->connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( auth_data->connect_data );
		return KErrGeneral;
		}
	iLog->Log ( _L ( "after read_data" ) );
	
	// Open a new closed connection
	connection = lm_connection_new ( auth_data->connect_data->server );
	iLog->Log ( _L ( "after lm_connection_new " ) );
	if ( !connection )
		{
		iLog->Log ( _L ( "lm_connection_new failed" ) );
		free_data ( auth_data->connect_data );
		return KErrGeneral;
		}
	
	// Set the gtalk's SSL port
	lm_connection_set_port ( connection, GTALK_SSL_PORT );
	
	// Set the JID
	lm_connection_set_jid ( connection, auth_data->connect_data->username );
	
	// Proxy settings for Emulator
#ifdef __WINSCW__
	SetProxy ( connection, auth_data->connect_data->proxy_data );
#endif
	
	// Set the connection to use SSL
	SSLInit ( connection );
	
	auth_data->main_loop = g_main_loop_new ( NULL, FALSE );

	iLog->Log ( _L ( "before lm_connection_open" ) );
	
	if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
	                           auth_data->main_loop, NULL, NULL ) ) 
	    {
		iLog->Log ( _L ( "lm_connection_open failed" ));
		lm_connection_unref ( connection );
		g_main_loop_unref ( auth_data->main_loop );
		free_data ( auth_data->connect_data );		
		g_free(auth_data);
		return KErrGeneral;
	    }
		
	iLog->Log ( _L ( "after lm_connection_open" ) );
	
	g_main_loop_run ( auth_data->main_loop );

	g_main_loop_unref ( auth_data->main_loop );    

	
	auth_data->main_loop = g_main_loop_new ( NULL, FALSE );
	
	// Get the username from the JID
	gchar *username = get_user_name ( auth_data->connect_data->username );
	
	// Authenticate with the server
	if ( !lm_connection_authenticate ( connection,
                                     username,
                                     auth_data->connect_data->password,
                                     auth_data->connect_data->resource,
                                      ( LmResultFunction ) lm_login_cb,
                                     auth_data,NULL,NULL ) )
	    {
		iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
		lm_connection_close ( connection, NULL );
		lm_connection_unref ( connection );
		free_data ( auth_data->connect_data );
		g_main_loop_unref ( auth_data->main_loop );
		g_free ( username );
		g_free ( auth_data );
		return KErrGeneral;
	    }

	g_main_loop_run ( auth_data->main_loop );
	g_main_loop_unref ( auth_data->main_loop ); 
	g_free ( username );
	
	// Close the connection
	lm_connection_close ( connection, NULL );	
	// Remove a reference on connection
	lm_connection_unref ( connection );
	free_data ( auth_data->connect_data );
	
	if ( auth_data->rCode == RC_ERROR )
	    {
	    iLog->Log ( _L ( "lm_memory_leak_testL failed" ) );
	    g_free ( auth_data );
	    return KErrGeneral;
	    }	
	g_free ( auth_data );
    

	
	return KErrNone;
}
// NFT test cases

TInt Ctstlm::lm_nft_updating_presenceL(CStifItemParser& aItem)	    
    {

    LmConnection *connection   = NULL;
    ConnectData  *connect_data = NULL;
    LmMessage    *message = NULL;
    LmMessageNode *show_node = NULL;
    LmMessageNode *status_node = NULL;
    LmMessageHandler *handler = NULL;
    GMainLoop    *main_loop    = NULL;
    
    iLog->Log ( _L ( "In lm_nft_updating_presenceL" ) );
    
    // Read a data from the CFG file
    if ( read_data ( connect_data, aItem ) != RC_OK )
        {
        iLog->Log ( _L ( "read_data failed" ) );
        free_data ( connect_data );
        return KErrGeneral;
        }
    
    // Open a new closed connection
    connection = lm_connection_new ( connect_data->server );
    if ( !connection )
        {
        iLog->Log ( _L ( "lm_connection_new failed" ) );
        free_data ( connect_data );
        return KErrGeneral;
        }
    
    // Set the gtalk's SSL port
    lm_connection_set_port ( connection, GTALK_SSL_PORT );
    
    // Set the JID
    lm_connection_set_jid ( connection, connect_data->username );
    
    // Proxy settings for Emulator
#ifdef __WINSCW__
    SetProxy ( connection, connect_data->proxy_data );
#endif
    
    // Set the connection to use SSL
    SSLInit ( connection );
    
    
    main_loop = g_main_loop_new ( NULL, FALSE );
    
    if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
                               main_loop, NULL, NULL ) ) 
        {
        iLog->Log ( _L ( "lm_connection_open failed" ));
        lm_connection_unref ( connection );
        g_main_loop_unref ( main_loop );
        free_data ( connect_data );     
        return KErrGeneral;
        }
        
    
    g_main_loop_run ( main_loop );

    g_main_loop_unref ( main_loop );    

    main_loop = g_main_loop_new ( NULL, FALSE );
    
    // Get the username from the JID
    gchar *username = get_user_name ( connect_data->username );
    
    // Authenticate with the server
    if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
        {
        iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
        lm_connection_close ( connection, NULL );
        lm_connection_unref ( connection );
        free_data ( connect_data );
        g_main_loop_unref ( main_loop );
        g_free ( username );
        return KErrGeneral;
        }
    g_main_loop_run ( main_loop );
    g_main_loop_unref ( main_loop );    
    g_free ( username );
    
    /*
    <presence xml:lang='en'>
        <show>dnd</show>
        <status>Wooing Juliet</status>
        <status xml:lang='cz'>Ja dvo&#x0159;&#x00ED;m Juliet</status>
    </presence>
    */

    //publish own presence
    //updating presence multiple time
    for (TInt i =1 ;i<100 ;i++)
        {
    message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_PRESENCE,
                                         LM_MESSAGE_SUB_TYPE_AVAILABLE );
    gboolean result = lm_connection_send ( connection, message, NULL ); 
    //lm_connection_send_with_reply(
    // Remove a reference on message
    lm_message_unref ( message );   
    
    //Change presence again
        message = lm_message_new_with_sub_type ( NULL,
                                             LM_MESSAGE_TYPE_PRESENCE,
                                             LM_MESSAGE_SUB_TYPE_AVAILABLE );
        lm_message_node_set_attributes ( message->node, "xml:lang", "en", NULL ); 
        show_node = lm_message_node_add_child ( message->node, "show", "busy" );
        status_node = lm_message_node_add_child ( message->node, "status", "goal goal goal" );
        result = lm_connection_send ( connection, message, NULL );  
        // Remove a reference on message
        lm_message_unref ( message );   
    
        }
    
    
    lm_connection_close ( connection, NULL );   
    // Remove a reference on connection
    lm_connection_unref ( connection ); 
    free_data ( connect_data );
    // Message sending passed
    iLog->Log ( _L ( "lm_nft_updating_presenceL" ) );
    
    return KErrNone;
    
    }

TInt Ctstlm ::lm_nft_presence_notificationL(CStifItemParser& aItem)
    {

    LmConnection *connection   = NULL;
    ConnectData  *connect_data = NULL;
    LmMessage    *message = NULL;
    LmMessageNode *show_node = NULL;
    LmMessageNode *status_node = NULL;
    LmMessageHandler *handler = NULL;
    GMainLoop    *main_loop    = NULL;
    HandleData *handle_data = NULL;
    
    iLog->Log ( _L ( "lm_nft_presence_notificationL" ) );
    
    // Read a data from the CFG file
    if ( read_data ( connect_data, aItem ) != RC_OK )
        {
        iLog->Log ( _L ( "read_data failed" ) );
        free_data ( connect_data );
        return KErrGeneral;
        }
    
    // Open a new closed connection
    connection = lm_connection_new ( connect_data->server );
    if ( !connection )
        {
        iLog->Log ( _L ( "lm_connection_new failed" ) );
        free_data ( connect_data );
        return KErrGeneral;
        }
    
    // Set the gtalk's SSL port
    lm_connection_set_port ( connection, GTALK_SSL_PORT );
    
    // Set the JID
    lm_connection_set_jid ( connection, connect_data->username );
    
    // Proxy settings for Emulator
#ifdef __WINSCW__
    SetProxy ( connection, connect_data->proxy_data );
#endif
    
    // Set the connection to use SSL
    SSLInit ( connection );
    
    
    main_loop = g_main_loop_new ( NULL, FALSE );
    
    if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
                               main_loop, NULL, NULL ) ) 
        {
        iLog->Log ( _L ( "lm_connection_open failed" ));
        lm_connection_unref ( connection );
        g_main_loop_unref ( main_loop );
        free_data ( connect_data );     
        return KErrGeneral;
        }
        
    
    g_main_loop_run ( main_loop );

    g_main_loop_unref ( main_loop );    

    main_loop = g_main_loop_new ( NULL, FALSE );
    
    // Get the username from the JID
    gchar *username = get_user_name ( connect_data->username );
    
    // Authenticate with the server
    if ( !lm_connection_authenticate ( connection,
                                     username,
                                     connect_data->password,
                                     connect_data->resource,
                                      ( LmResultFunction ) connection_auth_cb,
                                     main_loop,NULL,NULL ) )
        {
        iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
        lm_connection_close ( connection, NULL );
        lm_connection_unref ( connection );
        free_data ( connect_data );
        g_main_loop_unref ( main_loop );
        g_free ( username );
        return KErrGeneral;
        }
    g_main_loop_run ( main_loop );
    g_main_loop_unref ( main_loop );    
    g_free ( username );
    
    /*
    <presence xml:lang='en'>
        <show>dnd</show>
        <status>Wooing Juliet</status>
        <status xml:lang='cz'>Ja dvo&#x0159;&#x00ED;m Juliet</status>
    </presence>
    */

    //publish own presence
    message = lm_message_new_with_sub_type ( NULL,
                                         LM_MESSAGE_TYPE_PRESENCE,
                                         LM_MESSAGE_SUB_TYPE_AVAILABLE );
    gboolean result = lm_connection_send ( connection, message, NULL ); 
    //lm_connection_send_with_reply(
    // Remove a reference on message
    lm_message_unref ( message );   

    //------------------------------------------------------------------------
    //
    // Register a handler to recieve and update presence information
    //
   
        main_loop = g_main_loop_new ( NULL, FALSE );
        handle_data = g_new0 ( HandleData, 1 );
        handle_data->main_loop=main_loop;
        handle_data->count=0;
        handler = lm_message_handler_new ( 
                             (LmHandleMessageFunction)handle_100_presence_notification,
                             handle_data,
                             NULL );
        lm_connection_register_message_handler ( connection,
                                                 handler,
                                                 LM_MESSAGE_TYPE_PRESENCE,
                                                 LM_HANDLER_PRIORITY_NORMAL );
        g_main_loop_run ( main_loop );  
        g_main_loop_unref ( main_loop );
        lm_message_handler_unref(handler);
    
    
    
    lm_connection_close ( connection, NULL );   
    // Remove a reference on connection
    lm_connection_unref ( connection ); 
    free_data ( connect_data );
    // Message sending passed
    iLog->Log ( _L ( "lm_nft_presence_notificationL" ) );
    
    return KErrNone;
    
    
    }

 TInt Ctstlm::lm_nft_send_text_messageL(CStifItemParser& aItem)
     {

     LmConnection *connection   = NULL;
     ConnectData  *connect_data = NULL;
     LmMessage    *message;
     
     iLog->Log ( _L ( "In lm_nft_send_text_messageL" ) );
     
     // Read a data from the CFG file
     if ( read_data ( connect_data, aItem ) != RC_OK )
         {
         iLog->Log ( _L ( "read_data failed" ) );
         free_data ( connect_data );
         return KErrGeneral;
         }
     iLog->Log ( _L ( "after read_data" ) );
     
     // Open a new closed connection
     connection = lm_connection_new ( connect_data->server );
     iLog->Log ( _L ( "after lm_connection_new " ) );
     if ( !connection )
         {
         iLog->Log ( _L ( "lm_connection_new failed" ) );
         free_data ( connect_data );
         return KErrGeneral;
         }
     
     // Set the gtalk's SSL port
     lm_connection_set_port ( connection, GTALK_SSL_PORT );
     
     // Set the JID
     lm_connection_set_jid ( connection, connect_data->username );
     
     // Proxy settings for Emulator
 #ifdef __WINSCW__
     SetProxy ( connection, connect_data->proxy_data );
 #endif
     
     // Set the connection to use SSL
     SSLInit ( connection );
     
     GMainLoop   *main_loop = g_main_loop_new ( NULL, FALSE );

     iLog->Log ( _L ( "before lm_connection_open" ) );
     
     if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
                                main_loop, NULL, NULL ) ) 
         {
         iLog->Log ( _L ( "lm_connection_open failed" ));
         lm_connection_unref ( connection );
         g_main_loop_unref ( main_loop );
         free_data ( connect_data );     
         return KErrGeneral;
         }
         
     iLog->Log ( _L ( "after lm_connection_open" ) );
     g_main_loop_run ( main_loop );
     g_main_loop_unref ( main_loop );    
     
     main_loop = g_main_loop_new ( NULL, FALSE );
     
     // Get the username from the JID
     gchar *username = get_user_name ( connect_data->username );
     
     // Authenticate with the server
     if ( !lm_connection_authenticate ( connection,
                                      username,
                                      connect_data->password,
                                      connect_data->resource,
                                       ( LmResultFunction ) connection_auth_cb,
                                      main_loop,NULL,NULL ) )
         {
         iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
         lm_connection_close ( connection, NULL );
         lm_connection_unref ( connection );
         free_data ( connect_data );
         g_main_loop_unref ( main_loop );
         g_free ( username );
         return KErrGeneral;
         }
     g_main_loop_run ( main_loop );
     g_main_loop_unref ( main_loop );
     g_free ( username );
     
     // Construct a message
     message = lm_message_new ( connect_data->msg_data->recipient,               
                                LM_MESSAGE_TYPE_MESSAGE );
     lm_message_node_add_child ( message->node, "body", 
                                 connect_data->msg_data->message);
     for(TInt i =0 ; i<100 ;i++)
         {
     // Send message to the server
     if ( !lm_connection_send ( connection, message, NULL ) )
         {
         iLog->Log ( _L ( "lm_connection_send failed" ) );
         lm_connection_close ( connection, NULL );
         lm_connection_unref ( connection );
         lm_message_unref ( message );
         free_data ( connect_data );     
         return KErrGeneral;
         }  
         }
     // Close the connection
     lm_connection_close ( connection, NULL );
     
     // Remove a reference on connection
     lm_connection_unref ( connection );
     
     // Remove a reference on message
     lm_message_unref ( message );
     
     //free_data ( connect_data );
     
     // Message sending passed
     iLog->Log ( _L ( "lm_nft_send_text_messageL" ) );
     
     return KErrNone;

     }
 TInt Ctstlm::lm_nft_open_conversation_with_multipleL(CStifItemParser& aItem)
     {


     LmConnection *connection   = NULL;
     ConnectData  *connect_data = NULL;
     LmMessage    *message;
     
     iLog->Log ( _L ( "lm_nft_open_conversation_with_multipleL" ) );
     
     // Read a data from the CFG file
     if ( read_data ( connect_data, aItem ) != RC_OK )
         {
         iLog->Log ( _L ( "read_data failed" ) );
         free_data ( connect_data );
         return KErrGeneral;
         }
     iLog->Log ( _L ( "after read_data" ) );
     
     // Open a new closed connection
     connection = lm_connection_new ( connect_data->server );
     iLog->Log ( _L ( "after lm_connection_new " ) );
     if ( !connection )
         {
         iLog->Log ( _L ( "lm_connection_new failed" ) );
         free_data ( connect_data );
         return KErrGeneral;
         }
     
     // Set the gtalk's SSL port
     lm_connection_set_port ( connection, GTALK_SSL_PORT );
     
     // Set the JID
     lm_connection_set_jid ( connection, connect_data->username );
     
     // Proxy settings for Emulator
 #ifdef __WINSCW__
     SetProxy ( connection, connect_data->proxy_data );
 #endif
     
     // Set the connection to use SSL
     SSLInit ( connection );
     
     GMainLoop   *main_loop = g_main_loop_new ( NULL, FALSE );

     iLog->Log ( _L ( "before lm_connection_open" ) );
     
     if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
                                main_loop, NULL, NULL ) ) 
         {
         iLog->Log ( _L ( "lm_connection_open failed" ));
         lm_connection_unref ( connection );
         g_main_loop_unref ( main_loop );
         free_data ( connect_data );     
         return KErrGeneral;
         }
         
     iLog->Log ( _L ( "after lm_connection_open" ) );
     g_main_loop_run ( main_loop );
     g_main_loop_unref ( main_loop );    
     
     main_loop = g_main_loop_new ( NULL, FALSE );
     
     // Get the username from the JID
     gchar *username = get_user_name ( connect_data->username );
     
     // Authenticate with the server
     if ( !lm_connection_authenticate ( connection,
                                      username,
                                      connect_data->password,
                                      connect_data->resource,
                                       ( LmResultFunction ) connection_auth_cb,
                                      main_loop,NULL,NULL ) )
         {
         iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
         lm_connection_close ( connection, NULL );
         lm_connection_unref ( connection );
         free_data ( connect_data );
         g_main_loop_unref ( main_loop );
         g_free ( username );
         return KErrGeneral;
         }
     g_main_loop_run ( main_loop );
     g_main_loop_unref ( main_loop );
     g_free ( username );
     
     // Construct a message
     //receipient1
     const char *receipient = "rakesh.harsh@gmail.com";
     message = lm_message_new ( receipient,               
                                LM_MESSAGE_TYPE_MESSAGE );
     lm_message_node_add_child ( message->node, "body", 
                                 connect_data->msg_data->message);
     for(TInt i =0 ; i<20 ;i++)
         {
     // Send message to the server
     if ( !lm_connection_send ( connection, message, NULL ) )
         {
         iLog->Log ( _L ( "lm_connection_send failed" ) );
         lm_connection_close ( connection, NULL );
         lm_connection_unref ( connection );
         lm_message_unref ( message );
         free_data ( connect_data );     
         return KErrGeneral;
         }  
         }
             const char *receipient1 = "rakesh.harsha@gmail.com";
              message = lm_message_new ( receipient1,               
                                         LM_MESSAGE_TYPE_MESSAGE );
              lm_message_node_add_child ( message->node, "body", 
                                          connect_data->msg_data->message);
              for(TInt i =0 ; i<20 ;i++)
                  {
              // Send message to the server
              if ( !lm_connection_send ( connection, message, NULL ) )
                  {
                  iLog->Log ( _L ( "lm_connection_send failed" ) );
                  lm_connection_close ( connection, NULL );
                  lm_connection_unref ( connection );
                  lm_message_unref ( message );
                  free_data ( connect_data );     
                  return KErrGeneral;
                  }  
                  }
               const char *receipient2 = "testlm123@gmail.com";
               message = lm_message_new ( receipient2,               
                                          LM_MESSAGE_TYPE_MESSAGE );
               lm_message_node_add_child ( message->node, "body", 
                                           connect_data->msg_data->message);
               for(TInt i =0 ; i<20 ;i++)
                   {
               // Send message to the server
               if ( !lm_connection_send ( connection, message, NULL ) )
                   {
                   iLog->Log ( _L ( "lm_connection_send failed" ) );
                   lm_connection_close ( connection, NULL );
                   lm_connection_unref ( connection );
                   lm_message_unref ( message );
                   free_data ( connect_data );     
                   return KErrGeneral;
                   }  
                   }
               
                   const char *receipient3 = "nokiatestlm@gmail.com";
                    message = lm_message_new ( receipient3,               
                                               LM_MESSAGE_TYPE_MESSAGE );
                    lm_message_node_add_child ( message->node, "body", 
                                                connect_data->msg_data->message);
                    for(TInt i =0 ; i<20 ;i++)
                        {
                    // Send message to the server
                    if ( !lm_connection_send ( connection, message, NULL ) )
                        {
                        iLog->Log ( _L ( "lm_connection_send failed" ) );
                        lm_connection_close ( connection, NULL );
                        lm_connection_unref ( connection );
                        lm_message_unref ( message );
                        free_data ( connect_data );     
                        return KErrGeneral;
                        }  
                        } 
                                 const char *receipient4 = "testui123@gmail.com";
                                  message = lm_message_new ( receipient4,               
                                                             LM_MESSAGE_TYPE_MESSAGE );
                                  lm_message_node_add_child ( message->node, "body", 
                                                              connect_data->msg_data->message);
                                  for(TInt i =0 ; i<20 ;i++)
                                      {
                                  // Send message to the server
                                  if ( !lm_connection_send ( connection, message, NULL ) )
                                      {
                                      iLog->Log ( _L ( "lm_connection_send failed" ) );
                                      lm_connection_close ( connection, NULL );
                                      lm_connection_unref ( connection );
                                      lm_message_unref ( message );
                                      free_data ( connect_data );     
                                      return KErrGeneral;
                                      }  
                                      } 
                                  const char *receipient5 = "immeco10@gmail.com";
                                  message = lm_message_new ( receipient5,               
                                  LM_MESSAGE_TYPE_MESSAGE );
                                  lm_message_node_add_child ( message->node, "body", 
                                  connect_data->msg_data->message);
                                  for(TInt i =0 ; i<20 ;i++)
                                  {
                                  // Send message to the server
                                  if ( !lm_connection_send ( connection, message, NULL ) )
                                  {
                                  iLog->Log ( _L ( "lm_connection_send failed" ) );
                                  lm_connection_close ( connection, NULL );
                                  lm_connection_unref ( connection );
                                  lm_message_unref ( message );
                                  free_data ( connect_data );     
                                  return KErrGeneral;
                                  }  
                                  } 
                                  const char *receipient6 = "immeco11@gmail.com";
                                  message = lm_message_new ( receipient6,               
                                  LM_MESSAGE_TYPE_MESSAGE );
                                  lm_message_node_add_child ( message->node, "body", 
                                  connect_data->msg_data->message);
                                  for(TInt i =0 ; i<20 ;i++)
                                  {
                                  // Send message to the server
                                  if ( !lm_connection_send ( connection, message, NULL ) )
                                  {
                                  iLog->Log ( _L ( "lm_connection_send failed" ) );
                                  lm_connection_close ( connection, NULL );
                                  lm_connection_unref ( connection );
                                  lm_message_unref ( message );
                                  free_data ( connect_data );     
                                  return KErrGeneral;
                                  }  
                                  } 
                                  const char *receipient7 = "immeco11@gmail.com";
                                  message = lm_message_new ( receipient7,               
                                  LM_MESSAGE_TYPE_MESSAGE );
                                  lm_message_node_add_child ( message->node, "body", 
                                  connect_data->msg_data->message);
                                  for(TInt i =0 ; i<20 ;i++) 
                                  {
                                  // Send message to the server
                                  if ( !lm_connection_send ( connection, message, NULL ) )
                                  {
                                  iLog->Log ( _L ( "lm_connection_send failed" ) );
                                  lm_connection_close ( connection, NULL );
                                  lm_connection_unref ( connection );
                                  lm_message_unref ( message );
                                  free_data ( connect_data );     
                                  return KErrGeneral;
                                  }  
                                  } 
                                  const char *receipient8 = "test.isoserver@gmail.com";
                                  message = lm_message_new ( receipient8,               
                                  LM_MESSAGE_TYPE_MESSAGE );
                                  lm_message_node_add_child ( message->node, "body", 
                                  connect_data->msg_data->message);
                                  for(TInt i =0 ; i<20 ;i++)
                                  {
                                  // Send message to the server
                                  if ( !lm_connection_send ( connection, message, NULL ) )
                                  {
                                  iLog->Log ( _L ( "lm_connection_send failed" ) );
                                  lm_connection_close ( connection, NULL );
                                  lm_connection_unref ( connection );
                                  lm_message_unref ( message );
                                  free_data ( connect_data );     
                                  return KErrGeneral;
                                  }  
                                  }   
                                  const char *receipient9 = "tele1236@gmail.com";
                                  message = lm_message_new ( receipient9,               
                                  LM_MESSAGE_TYPE_MESSAGE );
                                  lm_message_node_add_child ( message->node, "body", 
                                  connect_data->msg_data->message);
                                  for(TInt i =0 ; i<20 ;i++)
                                  {
                                  // Send message to the server
                                  if ( !lm_connection_send ( connection, message, NULL ) )
                                  {
                                  iLog->Log ( _L ( "lm_connection_send failed" ) );
                                  lm_connection_close ( connection, NULL );
                                  lm_connection_unref ( connection );
                                  lm_message_unref ( message );
                                  free_data ( connect_data );     
                                  return KErrGeneral;
                                  }  
                                  }                                   
                                  const char *receipient10 = "meco5555@gmail.com";
                                  message = lm_message_new ( receipient10,               
                                  LM_MESSAGE_TYPE_MESSAGE );
                                  lm_message_node_add_child ( message->node, "body", 
                                  connect_data->msg_data->message);
                                  for(TInt i =0 ; i<20 ;i++)
                                  {
                                  // Send message to the server
                                  if ( !lm_connection_send ( connection, message, NULL ) )
                                  {
                                  iLog->Log ( _L ( "lm_connection_send failed" ) );
                                  lm_connection_close ( connection, NULL );
                                  lm_connection_unref ( connection );
                                  lm_message_unref ( message );
                                  free_data ( connect_data );     
                                  return KErrGeneral;
                                  }  
                                  }                               
     // Close the connection
     lm_connection_close ( connection, NULL );
     
     // Remove a reference on connection
     lm_connection_unref ( connection );
     
     // Remove a reference on message
     lm_message_unref ( message );
     
     free_data ( connect_data );
     
     // Message sending passed
     iLog->Log ( _L ( "lm_nft_open_conversation_with_multipleL" ) );
     
     return KErrNone;
     
     }
 TInt Ctstlm::lm_nft_fetch300_contactL(CStifItemParser& aItem)
      {

      LmConnection *connection   = NULL;
      ConnectData  *connect_data = NULL;
      LmMessage    *message;
      LmMessageNode *q_node;
      //LmMessageSubType  type;
      LmMessageHandler *handler = NULL;
      
      iLog->Log ( _L ( "lm_nft_fetch300_contactL" ) );
      
      // Read a data from the CFG file
      if ( read_data ( connect_data, aItem ) != RC_OK )
          {
          iLog->Log ( _L ( "read_data failed" ) );
          free_data ( connect_data );
          return KErrGeneral;
          }
      
      // Open a new closed connection
      connection = lm_connection_new ( connect_data->server );
      if ( !connection )
          {
          iLog->Log ( _L ( "lm_connection_new failed" ) );
          free_data ( connect_data );
          return KErrGeneral;
          }
      
      // Set the gtalk's SSL port
      lm_connection_set_port ( connection, GTALK_SSL_PORT );
      
      // Set the JID
      lm_connection_set_jid ( connection, connect_data->username );
      
      // Proxy settings for Emulator
  #ifdef __WINSCW__
      SetProxy ( connection, connect_data->proxy_data );
  #endif
      
      // Set the connection to use SSL
      SSLInit ( connection );

      //Open call    
      GMainLoop   *main_loop = g_main_loop_new ( NULL, FALSE );
      
      if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
                                 main_loop, NULL, NULL ) ) 
          {
          iLog->Log ( _L ( "lm_connection_open failed" ));
          lm_connection_unref ( connection );
          free_data ( connect_data ); 
          g_main_loop_unref ( main_loop );    
          return KErrGeneral;
          }
          
      
      g_main_loop_run ( main_loop );

      g_main_loop_unref ( main_loop );    
      

      main_loop = g_main_loop_new ( NULL, FALSE );
      
      // Get the username from the JID
      gchar *username = get_user_name ( connect_data->username );
      
      // Authenticate with the server
      if ( !lm_connection_authenticate ( connection,
                                       username,
                                       connect_data->password,
                                       connect_data->resource,
                                        ( LmResultFunction )connection_auth_cb,
                                       main_loop,NULL,NULL ) )
          {
          iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
          lm_connection_close ( connection, NULL );
          lm_connection_unref ( connection );
          free_data ( connect_data );
          g_main_loop_unref ( main_loop );
          g_free ( username );
          return KErrGeneral;
          }
      g_main_loop_run ( main_loop );
      g_main_loop_unref ( main_loop );    
      g_free ( username );
      

                  
      // Construct a message
      main_loop = g_main_loop_new ( NULL, FALSE );
      message = lm_message_new_with_sub_type (NULL, LM_MESSAGE_TYPE_IQ,
                            LM_MESSAGE_SUB_TYPE_GET);
      q_node = lm_message_node_add_child (message->node, "query", NULL);
      lm_message_node_set_attributes (q_node,
                          "xmlns", "jabber:iq:roster",
                          NULL); 
      // Send message to the server
      handler = lm_message_handler_new ( handle_fetch_contactlist_messages, main_loop, NULL );
      
      if ( !lm_connection_send_with_reply ( connection, message, handler, NULL ) ) 
          {
          iLog->Log ( _L ( "lm_connection_send_with_reply failed" ) );
          lm_connection_close ( connection, NULL );
          lm_connection_unref ( connection );
          lm_message_unref ( message );
          free_data ( connect_data );
          g_main_loop_unref ( main_loop );
          return KErrGeneral;
          }   
      g_main_loop_run ( main_loop );
      g_main_loop_unref ( main_loop );                                    
                          
                  
      
      // Close the connection
      lm_connection_close ( connection, NULL );
      
      // Remove a reference on connection
      lm_connection_unref ( connection );
      
      // Remove a reference on message
      lm_message_unref ( message );
      
      free_data ( connect_data );
      
      // Message sending passed
      iLog->Log ( _L ( "lm_nft_fetch300_contactL" ) );
      
      return KErrNone;
      
      }
     
 TInt Ctstlm::lm_nft_loginlogoutmultipletime(CStifItemParser& aItem)
     {
     LmConnection *connection   = NULL;
     ConnectData  *connect_data = NULL;
     LmMessage    *message;
     
     iLog->Log ( _L ( "lm_nft_loginlogoutmultipletime" ) );
     
     // Read a data from the CFG file
     if ( read_data ( connect_data, aItem ) != RC_OK )
         {
         iLog->Log ( _L ( "read_data failed" ) );
         free_data ( connect_data );
         return KErrGeneral;
         }
     iLog->Log ( _L ( "after read_data" ) );
     
     // Open a new closed connection
     connection = lm_connection_new ( connect_data->server );
     iLog->Log ( _L ( "after lm_connection_new " ) );
     if ( !connection )
         {
         iLog->Log ( _L ( "lm_connection_new failed" ) );
         free_data ( connect_data );
         return KErrGeneral;
         }
     
     // Set the gtalk's SSL port
     lm_connection_set_port ( connection, GTALK_SSL_PORT );
     
     // Set the JID
     lm_connection_set_jid ( connection, connect_data->username );
     
     // Proxy settings for Emulator
 #ifdef __WINSCW__
     SetProxy ( connection, connect_data->proxy_data );
 #endif
     
     // Set the connection to use SSL
     SSLInit ( connection );
     
     GMainLoop   *main_loop = g_main_loop_new ( NULL, FALSE );

     iLog->Log ( _L ( "before lm_connection_open" ) );
     
     if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
                                main_loop, NULL, NULL ) ) 
         {
         iLog->Log ( _L ( "lm_connection_open failed" ));
         lm_connection_unref ( connection );
         g_main_loop_unref ( main_loop );
         free_data ( connect_data );     
         return KErrGeneral;
         }
         
     iLog->Log ( _L ( "after lm_connection_open" ) );
     
     g_main_loop_run ( main_loop );

     g_main_loop_unref ( main_loop );    

     
     GMainLoop   *main_loop1 = g_main_loop_new ( NULL, FALSE );
     
     // Get the username from the JID
     gchar *username = get_user_name ( connect_data->username );
     
     // Authenticate with the server
     if ( !lm_connection_authenticate ( connection,
                                      username,
                                      connect_data->password,
                                      connect_data->resource,
                                       ( LmResultFunction ) connection_auth_cb,
                                      main_loop1,NULL,NULL ) )
         {
         iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
         lm_connection_close ( connection, NULL );
         lm_connection_unref ( connection );
         free_data ( connect_data );
         g_main_loop_unref ( main_loop1 );
         g_free ( username );
         return KErrGeneral;
         }

     g_main_loop_run ( main_loop1 );

     g_main_loop_unref ( main_loop1 );
         

     g_free ( username );
     
     // Construct a message
     message = lm_message_new ( connect_data->msg_data->recipient,               
                                LM_MESSAGE_TYPE_MESSAGE );
     lm_message_node_add_child ( message->node, "body", 
                                 connect_data->msg_data->message);
     
     // Send message to the server
     if ( !lm_connection_send ( connection, message, NULL ) )
         {
         iLog->Log ( _L ( "lm_connection_send failed" ) );
         lm_connection_close ( connection, NULL );
         lm_connection_unref ( connection );
         lm_message_unref ( message );
         free_data ( connect_data );     
         return KErrGeneral;
         }   
     
     // Close the connection
     lm_connection_close ( connection, NULL );
     
     // Remove a reference on connection
     lm_connection_unref ( connection );
     
     // Remove a reference on message
     lm_message_unref ( message );
     
     free_data ( connect_data );
     
     // Message sending passed
     iLog->Log ( _L ( "lm_connection_send passed" ) );
     //second time
     // Read a data from the CFG file
          if ( read_data ( connect_data, aItem ) != RC_OK )
              {
              iLog->Log ( _L ( "read_data failed" ) );
              free_data ( connect_data );
              return KErrGeneral;
              }
          iLog->Log ( _L ( "after read_data" ) );
          
          // Open a new closed connection
          connection = lm_connection_new ( connect_data->server );
          iLog->Log ( _L ( "after lm_connection_new " ) );
          if ( !connection )
              {
              iLog->Log ( _L ( "lm_connection_new failed" ) );
              free_data ( connect_data );
              return KErrGeneral;
              }
          
          // Set the gtalk's SSL port
          lm_connection_set_port ( connection, GTALK_SSL_PORT );
          
          // Set the JID
          lm_connection_set_jid ( connection, connect_data->username );
          
          // Proxy settings for Emulator
      #ifdef __WINSCW__
          SetProxy ( connection, connect_data->proxy_data );
      #endif
          
          // Set the connection to use SSL
          SSLInit ( connection );
          
          //*main_loop = g_main_loop_new ( NULL, FALSE );

          iLog->Log ( _L ( "before lm_connection_open" ) );
          
          if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
                                     main_loop, NULL, NULL ) ) 
              {
              iLog->Log ( _L ( "lm_connection_open failed" ));
              lm_connection_unref ( connection );
              g_main_loop_unref ( main_loop );
              free_data ( connect_data );     
              return KErrGeneral;
              }
              
          iLog->Log ( _L ( "after lm_connection_open" ) );
          
          g_main_loop_run ( main_loop );

          g_main_loop_unref ( main_loop );    

          
            // *main_loop1 = g_main_loop_new ( NULL, FALSE );
          
          // Get the username from the JID
          //*username = get_user_name ( connect_data->username );
          
          // Authenticate with the server
          if ( !lm_connection_authenticate ( connection,
                                           username,
                                           connect_data->password,
                                           connect_data->resource,
                                            ( LmResultFunction ) connection_auth_cb,
                                           main_loop1,NULL,NULL ) )
              {
              iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
              lm_connection_close ( connection, NULL );
              lm_connection_unref ( connection );
              free_data ( connect_data );
              g_main_loop_unref ( main_loop1 );
              g_free ( username );
              return KErrGeneral;
              }

          g_main_loop_run ( main_loop1 );

          g_main_loop_unref ( main_loop1 );
              

          g_free ( username );
          
          // Construct a message
          message = lm_message_new ( connect_data->msg_data->recipient,               
                                     LM_MESSAGE_TYPE_MESSAGE );
          lm_message_node_add_child ( message->node, "body", 
                                      connect_data->msg_data->message);
          
          // Send message to the server
          if ( !lm_connection_send ( connection, message, NULL ) )
              {
              iLog->Log ( _L ( "lm_connection_send failed" ) );
              lm_connection_close ( connection, NULL );
              lm_connection_unref ( connection );
              lm_message_unref ( message );
              free_data ( connect_data );     
              return KErrGeneral;
              }   
          
          // Close the connection
          lm_connection_close ( connection, NULL );
          
          // Remove a reference on connection
          lm_connection_unref ( connection );
          
          // Remove a reference on message
          lm_message_unref ( message );
          
          free_data ( connect_data );
          
          // Message sending passed
          iLog->Log ( _L ( "lm_connection_send passed" ) );
          
//third time
          // Read a data from the CFG file
               if ( read_data ( connect_data, aItem ) != RC_OK )
                   {
                   iLog->Log ( _L ( "read_data failed" ) );
                   free_data ( connect_data );
                   return KErrGeneral;
                   }
               iLog->Log ( _L ( "after read_data" ) );
               
               // Open a new closed connection
               connection = lm_connection_new ( connect_data->server );
               iLog->Log ( _L ( "after lm_connection_new " ) );
               if ( !connection )
                   {
                   iLog->Log ( _L ( "lm_connection_new failed" ) );
                   free_data ( connect_data );
                   return KErrGeneral;
                   }
               
               // Set the gtalk's SSL port
               lm_connection_set_port ( connection, GTALK_SSL_PORT );
               
               // Set the JID
               lm_connection_set_jid ( connection, connect_data->username );
               
               // Proxy settings for Emulator
           #ifdef __WINSCW__
               SetProxy ( connection, connect_data->proxy_data );
           #endif
               
               // Set the connection to use SSL
               SSLInit ( connection );
               
              // *main_loop = g_main_loop_new ( NULL, FALSE );

               iLog->Log ( _L ( "before lm_connection_open" ) );
               
               if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
                                          main_loop, NULL, NULL ) ) 
                   {
                   iLog->Log ( _L ( "lm_connection_open failed" ));
                   lm_connection_unref ( connection );
                   g_main_loop_unref ( main_loop );
                   free_data ( connect_data );     
                   return KErrGeneral;
                   }
                   
               iLog->Log ( _L ( "after lm_connection_open" ) );
               
               g_main_loop_run ( main_loop );

               g_main_loop_unref ( main_loop );    

               
             // *main_loop1 = g_main_loop_new ( NULL, FALSE );
               
               // Get the username from the JID
              // gchar *username = get_user_name ( connect_data->username );
               
               // Authenticate with the server
               if ( !lm_connection_authenticate ( connection,
                                                username,
                                                connect_data->password,
                                                connect_data->resource,
                                                 ( LmResultFunction ) connection_auth_cb,
                                                main_loop1,NULL,NULL ) )
                   {
                   iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
                   lm_connection_close ( connection, NULL );
                   lm_connection_unref ( connection );
                   free_data ( connect_data );
                   g_main_loop_unref ( main_loop1 );
                   g_free ( username );
                   return KErrGeneral;
                   }

               g_main_loop_run ( main_loop1 );

               g_main_loop_unref ( main_loop1 );
                   

               g_free ( username );
               
               // Construct a message
               message = lm_message_new ( connect_data->msg_data->recipient,               
                                          LM_MESSAGE_TYPE_MESSAGE );
               lm_message_node_add_child ( message->node, "body", 
                                           connect_data->msg_data->message);
               
               // Send message to the server
               if ( !lm_connection_send ( connection, message, NULL ) )
                   {
                   iLog->Log ( _L ( "lm_connection_send failed" ) );
                   lm_connection_close ( connection, NULL );
                   lm_connection_unref ( connection );
                   lm_message_unref ( message );
                   free_data ( connect_data );     
                   return KErrGeneral;
                   }   
               
               // Close the connection
               lm_connection_close ( connection, NULL );
               
               // Remove a reference on connection
               lm_connection_unref ( connection );
               
               // Remove a reference on message
               lm_message_unref ( message );
               
               free_data ( connect_data );
               
               // Message sending passed
               iLog->Log ( _L ( "lm_connection_send passed" ) );
               
//fourth time
               // Read a data from the CFG file
                    if ( read_data ( connect_data, aItem ) != RC_OK )
                        {
                        iLog->Log ( _L ( "read_data failed" ) );
                        free_data ( connect_data );
                        return KErrGeneral;
                        }
                    iLog->Log ( _L ( "after read_data" ) );
                    
                    // Open a new closed connection
                    connection = lm_connection_new ( connect_data->server );
                    iLog->Log ( _L ( "after lm_connection_new " ) );
                    if ( !connection )
                        {
                        iLog->Log ( _L ( "lm_connection_new failed" ) );
                        free_data ( connect_data );
                        return KErrGeneral;
                        }
                    
                    // Set the gtalk's SSL port
                    lm_connection_set_port ( connection, GTALK_SSL_PORT );
                    
                    // Set the JID
                    lm_connection_set_jid ( connection, connect_data->username );
                    
                    // Proxy settings for Emulator
                #ifdef __WINSCW__
                    SetProxy ( connection, connect_data->proxy_data );
                #endif
                    
                    // Set the connection to use SSL
                    SSLInit ( connection );
                    
                   // *main_loop = g_main_loop_new ( NULL, FALSE );

                    iLog->Log ( _L ( "before lm_connection_open" ) );
                    
                    if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
                                               main_loop, NULL, NULL ) ) 
                        {
                        iLog->Log ( _L ( "lm_connection_open failed" ));
                        lm_connection_unref ( connection );
                        g_main_loop_unref ( main_loop );
                        free_data ( connect_data );     
                        return KErrGeneral;
                        }
                        
                    iLog->Log ( _L ( "after lm_connection_open" ) );
                    
                    g_main_loop_run ( main_loop );

                    g_main_loop_unref ( main_loop );    

                    
                   // *main_loop1 = g_main_loop_new ( NULL, FALSE );
                    
                    // Get the username from the JID
                   // gchar *username = get_user_name ( connect_data->username );
                    
                    // Authenticate with the server
                    if ( !lm_connection_authenticate ( connection,
                                                     username,
                                                     connect_data->password,
                                                     connect_data->resource,
                                                      ( LmResultFunction ) connection_auth_cb,
                                                     main_loop1,NULL,NULL ) )
                        {
                        iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
                        lm_connection_close ( connection, NULL );
                        lm_connection_unref ( connection );
                        free_data ( connect_data );
                        g_main_loop_unref ( main_loop1 );
                        g_free ( username );
                        return KErrGeneral;
                        }

                    g_main_loop_run ( main_loop1 );

                    g_main_loop_unref ( main_loop1 );
                        

                    g_free ( username );
                    
                    // Construct a message
                    message = lm_message_new ( connect_data->msg_data->recipient,               
                                               LM_MESSAGE_TYPE_MESSAGE );
                    lm_message_node_add_child ( message->node, "body", 
                                                connect_data->msg_data->message);
                    
                    // Send message to the server
                    if ( !lm_connection_send ( connection, message, NULL ) )
                        {
                        iLog->Log ( _L ( "lm_connection_send failed" ) );
                        lm_connection_close ( connection, NULL );
                        lm_connection_unref ( connection );
                        lm_message_unref ( message );
                        free_data ( connect_data );     
                        return KErrGeneral;
                        }   
                    
                    // Close the connection
                    lm_connection_close ( connection, NULL );
                    
                    // Remove a reference on connection
                    lm_connection_unref ( connection );
                    
                    // Remove a reference on message
                    lm_message_unref ( message );
                    
                    free_data ( connect_data );
                    
                    // Message sending passed
                    iLog->Log ( _L ( "lm_connection_send passed" ) );
                    
//five time
                    // Read a data from the CFG file
                         if ( read_data ( connect_data, aItem ) != RC_OK )
                             {
                             iLog->Log ( _L ( "read_data failed" ) );
                             free_data ( connect_data );
                             return KErrGeneral;
                             }
                         iLog->Log ( _L ( "after read_data" ) );
                         
                         // Open a new closed connection
                         connection = lm_connection_new ( connect_data->server );
                         iLog->Log ( _L ( "after lm_connection_new " ) );
                         if ( !connection )
                             {
                             iLog->Log ( _L ( "lm_connection_new failed" ) );
                             free_data ( connect_data );
                             return KErrGeneral;
                             }
                         
                         // Set the gtalk's SSL port
                         lm_connection_set_port ( connection, GTALK_SSL_PORT );
                         
                         // Set the JID
                         lm_connection_set_jid ( connection, connect_data->username );
                         
                         // Proxy settings for Emulator
                     #ifdef __WINSCW__
                         SetProxy ( connection, connect_data->proxy_data );
                     #endif
                         
                         // Set the connection to use SSL
                         SSLInit ( connection );
                         
                     // *main_loop = g_main_loop_new ( NULL, FALSE );

                          iLog->Log ( _L ( "before lm_connection_open" ) );
                         
                         if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
                                                    main_loop, NULL, NULL ) ) 
                             {
                             iLog->Log ( _L ( "lm_connection_open failed" ));
                             lm_connection_unref ( connection );
                             g_main_loop_unref ( main_loop );
                             free_data ( connect_data );     
                             return KErrGeneral;
                             }
                             
                         iLog->Log ( _L ( "after lm_connection_open" ) );
                         
                         g_main_loop_run ( main_loop );

                         g_main_loop_unref ( main_loop );    

                         
                       //    *main_loop1 = g_main_loop_new ( NULL, FALSE );
                         
                         // Get the username from the JID
                         //gchar *username = get_user_name ( connect_data->username );
                         
                         // Authenticate with the server
                         if ( !lm_connection_authenticate ( connection,
                                                          username,
                                                          connect_data->password,
                                                          connect_data->resource,
                                                           ( LmResultFunction ) connection_auth_cb,
                                                          main_loop1,NULL,NULL ) )
                             {
                             iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
                             lm_connection_close ( connection, NULL );
                             lm_connection_unref ( connection );
                             free_data ( connect_data );
                             g_main_loop_unref ( main_loop1 );
                             g_free ( username );
                             return KErrGeneral;
                             }

                         g_main_loop_run ( main_loop1 );

                         g_main_loop_unref ( main_loop1 );
                             

                         g_free ( username );
                         
                         // Construct a message
                         message = lm_message_new ( connect_data->msg_data->recipient,               
                                                    LM_MESSAGE_TYPE_MESSAGE );
                         lm_message_node_add_child ( message->node, "body", 
                                                     connect_data->msg_data->message);
                         
                         // Send message to the server
                         if ( !lm_connection_send ( connection, message, NULL ) )
                             {
                             iLog->Log ( _L ( "lm_connection_send failed" ) );
                             lm_connection_close ( connection, NULL );
                             lm_connection_unref ( connection );
                             lm_message_unref ( message );
                             free_data ( connect_data );     
                             return KErrGeneral;
                             }   
                         
                         // Close the connection
                         lm_connection_close ( connection, NULL );
                         
                         // Remove a reference on connection
                         lm_connection_unref ( connection );
                         
                         // Remove a reference on message
                         lm_message_unref ( message );
                         
                         free_data ( connect_data );
                         
                         // Message sending passed
                         iLog->Log ( _L ( "lm_connection_send passed" ) );
                         
//fifth time
                         // Read a data from the CFG file
                              if ( read_data ( connect_data, aItem ) != RC_OK )
                                  {
                                  iLog->Log ( _L ( "read_data failed" ) );
                                  free_data ( connect_data );
                                  return KErrGeneral;
                                  }
                              iLog->Log ( _L ( "after read_data" ) );
                              
                              // Open a new closed connection
                              connection = lm_connection_new ( connect_data->server );
                              iLog->Log ( _L ( "after lm_connection_new " ) );
                              if ( !connection )
                                  {
                                  iLog->Log ( _L ( "lm_connection_new failed" ) );
                                  free_data ( connect_data );
                                  return KErrGeneral;
                                  }
                              
                              // Set the gtalk's SSL port
                              lm_connection_set_port ( connection, GTALK_SSL_PORT );
                              
                              // Set the JID
                              lm_connection_set_jid ( connection, connect_data->username );
                              
                              // Proxy settings for Emulator
                          #ifdef __WINSCW__
                              SetProxy ( connection, connect_data->proxy_data );
                          #endif
                              
                              // Set the connection to use SSL
                              SSLInit ( connection );
                              
                          //    *main_loop = g_main_loop_new ( NULL, FALSE );

                              iLog->Log ( _L ( "before lm_connection_open" ) );
                              
                              if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
                                                         main_loop, NULL, NULL ) ) 
                                  {
                                  iLog->Log ( _L ( "lm_connection_open failed" ));
                                  lm_connection_unref ( connection );
                                  g_main_loop_unref ( main_loop );
                                  free_data ( connect_data );     
                                  return KErrGeneral;
                                  }
                                  
                              iLog->Log ( _L ( "after lm_connection_open" ) );
                              
                              g_main_loop_run ( main_loop );

                              g_main_loop_unref ( main_loop );    

                              
                       //        *main_loop1 = g_main_loop_new ( NULL, FALSE );
                              
                              // Get the username from the JID
                              //gchar *username = get_user_name ( connect_data->username );
                              
                              // Authenticate with the server
                              if ( !lm_connection_authenticate ( connection,
                                                               username,
                                                               connect_data->password,
                                                               connect_data->resource,
                                                                ( LmResultFunction ) connection_auth_cb,
                                                               main_loop1,NULL,NULL ) )
                                  {
                                  iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
                                  lm_connection_close ( connection, NULL );
                                  lm_connection_unref ( connection );
                                  free_data ( connect_data );
                                  g_main_loop_unref ( main_loop1 );
                                  g_free ( username );
                                  return KErrGeneral;
                                  }

                              g_main_loop_run ( main_loop1 );

                              g_main_loop_unref ( main_loop1 );
                                  

                              g_free ( username );
                              
                              // Construct a message
                              message = lm_message_new ( connect_data->msg_data->recipient,               
                                                         LM_MESSAGE_TYPE_MESSAGE );
                              lm_message_node_add_child ( message->node, "body", 
                                                          connect_data->msg_data->message);
                              
                              // Send message to the server
                              if ( !lm_connection_send ( connection, message, NULL ) )
                                  {
                                  iLog->Log ( _L ( "lm_connection_send failed" ) );
                                  lm_connection_close ( connection, NULL );
                                  lm_connection_unref ( connection );
                                  lm_message_unref ( message );
                                  free_data ( connect_data );     
                                  return KErrGeneral;
                                  }   
                              
                              // Close the connection
                              lm_connection_close ( connection, NULL );
                              
                              // Remove a reference on connection
                              lm_connection_unref ( connection );
                              
                              // Remove a reference on message
                              lm_message_unref ( message );
                              
                              free_data ( connect_data );
                              
                              // Message sending passed
                              iLog->Log ( _L ( "lm_connection_send passed" ) );
                              
//sixth time
                              // Read a data from the CFG file
                                   if ( read_data ( connect_data, aItem ) != RC_OK )
                                       {
                                       iLog->Log ( _L ( "read_data failed" ) );
                                       free_data ( connect_data );
                                       return KErrGeneral;
                                       }
                                   iLog->Log ( _L ( "after read_data" ) );
                                   
                                   // Open a new closed connection
                                   connection = lm_connection_new ( connect_data->server );
                                   iLog->Log ( _L ( "after lm_connection_new " ) );
                                   if ( !connection )
                                       {
                                       iLog->Log ( _L ( "lm_connection_new failed" ) );
                                       free_data ( connect_data );
                                       return KErrGeneral;
                                       }
                                   
                                   // Set the gtalk's SSL port
                                   lm_connection_set_port ( connection, GTALK_SSL_PORT );
                                   
                                   // Set the JID
                                   lm_connection_set_jid ( connection, connect_data->username );
                                   
                                   // Proxy settings for Emulator
                               #ifdef __WINSCW__
                                   SetProxy ( connection, connect_data->proxy_data );
                               #endif
                                   
                                   // Set the connection to use SSL
                                   SSLInit ( connection );
                                   
                        //        *main_loop = g_main_loop_new ( NULL, FALSE );

                                   iLog->Log ( _L ( "before lm_connection_open" ) );
                                   
                                   if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
                                                              main_loop, NULL, NULL ) ) 
                                       {
                                       iLog->Log ( _L ( "lm_connection_open failed" ));
                                       lm_connection_unref ( connection );
                                       g_main_loop_unref ( main_loop );
                                       free_data ( connect_data );     
                                       return KErrGeneral;
                                       }
                                       
                                   iLog->Log ( _L ( "after lm_connection_open" ) );
                                   
                                   g_main_loop_run ( main_loop );

                                   g_main_loop_unref ( main_loop );    

                                   
                                  // GMainLoop   *main_loop1 = g_main_loop_new ( NULL, FALSE );
                                   
                                   // Get the username from the JID
                                   //gchar *username = get_user_name ( connect_data->username );
                                   
                                   // Authenticate with the server
                                   if ( !lm_connection_authenticate ( connection,
                                                                    username,
                                                                    connect_data->password,
                                                                    connect_data->resource,
                                                                     ( LmResultFunction ) connection_auth_cb,
                                                                    main_loop1,NULL,NULL ) )
                                       {
                                       iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
                                       lm_connection_close ( connection, NULL );
                                       lm_connection_unref ( connection );
                                       free_data ( connect_data );
                                       g_main_loop_unref ( main_loop1 );
                                       g_free ( username );
                                       return KErrGeneral;
                                       }

                                   g_main_loop_run ( main_loop1 );

                                   g_main_loop_unref ( main_loop1 );
                                       

                                   g_free ( username );
                                   
                                   // Construct a message
                                   message = lm_message_new ( connect_data->msg_data->recipient,               
                                                              LM_MESSAGE_TYPE_MESSAGE );
                                   lm_message_node_add_child ( message->node, "body", 
                                                               connect_data->msg_data->message);
                                   
                                   // Send message to the server
                                   if ( !lm_connection_send ( connection, message, NULL ) )
                                       {
                                       iLog->Log ( _L ( "lm_connection_send failed" ) );
                                       lm_connection_close ( connection, NULL );
                                       lm_connection_unref ( connection );
                                       lm_message_unref ( message );
                                       free_data ( connect_data );     
                                       return KErrGeneral;
                                       }   
                                   
                                   // Close the connection
                                   lm_connection_close ( connection, NULL );
                                   
                                   // Remove a reference on connection
                                   lm_connection_unref ( connection );
                                   
                                   // Remove a reference on message
                                   lm_message_unref ( message );
                                   
                                   free_data ( connect_data );
                                   
                                   // Message sending passed
                                   iLog->Log ( _L ( "lm_connection_send passed" ) );
                                   
//seventh time
                                   // Read a data from the CFG file
                                        if ( read_data ( connect_data, aItem ) != RC_OK )
                                            {
                                            iLog->Log ( _L ( "read_data failed" ) );
                                            free_data ( connect_data );
                                            return KErrGeneral;
                                            }
                                        iLog->Log ( _L ( "after read_data" ) );
                                        
                                        // Open a new closed connection
                                        connection = lm_connection_new ( connect_data->server );
                                        iLog->Log ( _L ( "after lm_connection_new " ) );
                                        if ( !connection )
                                            {
                                            iLog->Log ( _L ( "lm_connection_new failed" ) );
                                            free_data ( connect_data );
                                            return KErrGeneral;
                                            }
                                        
                                        // Set the gtalk's SSL port
                                        lm_connection_set_port ( connection, GTALK_SSL_PORT );
                                        
                                        // Set the JID
                                        lm_connection_set_jid ( connection, connect_data->username );
                                        
                                        // Proxy settings for Emulator
                                    #ifdef __WINSCW__
                                        SetProxy ( connection, connect_data->proxy_data );
                                    #endif
                                        
                                        // Set the connection to use SSL
                                        SSLInit ( connection );
                                        
                               //          *main_loop = g_main_loop_new ( NULL, FALSE );

                                        iLog->Log ( _L ( "before lm_connection_open" ) );
                                        
                                        if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
                                                                   main_loop, NULL, NULL ) ) 
                                            {
                                            iLog->Log ( _L ( "lm_connection_open failed" ));
                                            lm_connection_unref ( connection );
                                            g_main_loop_unref ( main_loop );
                                            free_data ( connect_data );     
                                            return KErrGeneral;
                                            }
                                            
                                        iLog->Log ( _L ( "after lm_connection_open" ) );
                                        
                                        g_main_loop_run ( main_loop );

                                        g_main_loop_unref ( main_loop );    

                                        
                                  //      GMainLoop   *main_loop1 = g_main_loop_new ( NULL, FALSE );
                                        
                                        // Get the username from the JID
                                        //gchar *username = get_user_name ( connect_data->username );
                                        
                                        // Authenticate with the server
                                        if ( !lm_connection_authenticate ( connection,
                                                                         username,
                                                                         connect_data->password,
                                                                         connect_data->resource,
                                                                          ( LmResultFunction ) connection_auth_cb,
                                                                         main_loop1,NULL,NULL ) )
                                            {
                                            iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
                                            lm_connection_close ( connection, NULL );
                                            lm_connection_unref ( connection );
                                            free_data ( connect_data );
                                            g_main_loop_unref ( main_loop1 );
                                            g_free ( username );
                                            return KErrGeneral;
                                            }

                                        g_main_loop_run ( main_loop1 );

                                        g_main_loop_unref ( main_loop1 );
                                            

                                        g_free ( username );
                                        
                                        // Construct a message
                                        message = lm_message_new ( connect_data->msg_data->recipient,               
                                                                   LM_MESSAGE_TYPE_MESSAGE );
                                        lm_message_node_add_child ( message->node, "body", 
                                                                    connect_data->msg_data->message);
                                        
                                        // Send message to the server
                                        if ( !lm_connection_send ( connection, message, NULL ) )
                                            {
                                            iLog->Log ( _L ( "lm_connection_send failed" ) );
                                            lm_connection_close ( connection, NULL );
                                            lm_connection_unref ( connection );
                                            lm_message_unref ( message );
                                            free_data ( connect_data );     
                                            return KErrGeneral;
                                            }   
                                        
                                        // Close the connection
                                        lm_connection_close ( connection, NULL );
                                        
                                        // Remove a reference on connection
                                        lm_connection_unref ( connection );
                                        
                                        // Remove a reference on message
                                        lm_message_unref ( message );
                                        
                                        free_data ( connect_data );
                                        
                                        // Message sending passed
                                        iLog->Log ( _L ( "lm_connection_send passed" ) );
                                        
//eight time
                                        // Read a data from the CFG file
                                             if ( read_data ( connect_data, aItem ) != RC_OK )
                                                 {
                                                 iLog->Log ( _L ( "read_data failed" ) );
                                                 free_data ( connect_data );
                                                 return KErrGeneral;
                                                 }
                                             iLog->Log ( _L ( "after read_data" ) );
                                             
                                             // Open a new closed connection
                                             connection = lm_connection_new ( connect_data->server );
                                             iLog->Log ( _L ( "after lm_connection_new " ) );
                                             if ( !connection )
                                                 {
                                                 iLog->Log ( _L ( "lm_connection_new failed" ) );
                                                 free_data ( connect_data );
                                                 return KErrGeneral;
                                                 }
                                             
                                             // Set the gtalk's SSL port
                                             lm_connection_set_port ( connection, GTALK_SSL_PORT );
                                             
                                             // Set the JID
                                             lm_connection_set_jid ( connection, connect_data->username );
                                             
                                             // Proxy settings for Emulator
                                         #ifdef __WINSCW__
                                             SetProxy ( connection, connect_data->proxy_data );
                                         #endif
                                             
                                             // Set the connection to use SSL
                                             SSLInit ( connection );
                                             
                              //                *main_loop = g_main_loop_new ( NULL, FALSE );

                                             iLog->Log ( _L ( "before lm_connection_open" ) );
                                             
                                             if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
                                                                        main_loop, NULL, NULL ) ) 
                                                 {
                                                 iLog->Log ( _L ( "lm_connection_open failed" ));
                                                 lm_connection_unref ( connection );
                                                 g_main_loop_unref ( main_loop );
                                                 free_data ( connect_data );     
                                                 return KErrGeneral;
                                                 }
                                                 
                                             iLog->Log ( _L ( "after lm_connection_open" ) );
                                             
                                             g_main_loop_run ( main_loop );

                                             g_main_loop_unref ( main_loop );    

                                             
                                    //         GMainLoop   *main_loop1 = g_main_loop_new ( NULL, FALSE );
                                             
                                             // Get the username from the JID
                                             //gchar *username = get_user_name ( connect_data->username );
                                             
                                             // Authenticate with the server
                                             if ( !lm_connection_authenticate ( connection,
                                                                              username,
                                                                              connect_data->password,
                                                                              connect_data->resource,
                                                                               ( LmResultFunction ) connection_auth_cb,
                                                                              main_loop1,NULL,NULL ) )
                                                 {
                                                 iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
                                                 lm_connection_close ( connection, NULL );
                                                 lm_connection_unref ( connection );
                                                 free_data ( connect_data );
                                                 g_main_loop_unref ( main_loop1 );
                                                 g_free ( username );
                                                 return KErrGeneral;
                                                 }

                                             g_main_loop_run ( main_loop1 );

                                             g_main_loop_unref ( main_loop1 );
                                                 

                                             g_free ( username );
                                             
                                             // Construct a message
                                             message = lm_message_new ( connect_data->msg_data->recipient,               
                                                                        LM_MESSAGE_TYPE_MESSAGE );
                                             lm_message_node_add_child ( message->node, "body", 
                                                                         connect_data->msg_data->message);
                                             
                                             // Send message to the server
                                             if ( !lm_connection_send ( connection, message, NULL ) )
                                                 {
                                                 iLog->Log ( _L ( "lm_connection_send failed" ) );
                                                 lm_connection_close ( connection, NULL );
                                                 lm_connection_unref ( connection );
                                                 lm_message_unref ( message );
                                                 free_data ( connect_data );     
                                                 return KErrGeneral;
                                                 }   
                                             
                                             // Close the connection
                                             lm_connection_close ( connection, NULL );
                                             
                                             // Remove a reference on connection
                                             lm_connection_unref ( connection );
                                             
                                             // Remove a reference on message
                                             lm_message_unref ( message );
                                             
                                             free_data ( connect_data );
                                             
                                             // Message sending passed
                                             iLog->Log ( _L ( "lm_connection_send passed" ) );
                                             
//ninth time
                                             // Read a data from the CFG file
                                                  if ( read_data ( connect_data, aItem ) != RC_OK )
                                                      {
                                                      iLog->Log ( _L ( "read_data failed" ) );
                                                      free_data ( connect_data );
                                                      return KErrGeneral;
                                                      }
                                                  iLog->Log ( _L ( "after read_data" ) );
                                                  
                                                  // Open a new closed connection
                                                  connection = lm_connection_new ( connect_data->server );
                                                  iLog->Log ( _L ( "after lm_connection_new " ) );
                                                  if ( !connection )
                                                      {
                                                      iLog->Log ( _L ( "lm_connection_new failed" ) );
                                                      free_data ( connect_data );
                                                      return KErrGeneral;
                                                      }
                                                  
                                                  // Set the gtalk's SSL port
                                                  lm_connection_set_port ( connection, GTALK_SSL_PORT );
                                                  
                                                  // Set the JID
                                                  lm_connection_set_jid ( connection, connect_data->username );
                                                  
                                                  // Proxy settings for Emulator
                                              #ifdef __WINSCW__
                                                  SetProxy ( connection, connect_data->proxy_data );
                                              #endif
                                                  
                                                  // Set the connection to use SSL
                                                  SSLInit ( connection );
                                                  
                                       //            *main_loop = g_main_loop_new ( NULL, FALSE );

                                                  iLog->Log ( _L ( "before lm_connection_open" ) );
                                                  
                                                  if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
                                                                             main_loop, NULL, NULL ) ) 
                                                      {
                                                      iLog->Log ( _L ( "lm_connection_open failed" ));
                                                      lm_connection_unref ( connection );
                                                      g_main_loop_unref ( main_loop );
                                                      free_data ( connect_data );     
                                                      return KErrGeneral;
                                                      }
                                                      
                                                  iLog->Log ( _L ( "after lm_connection_open" ) );
                                                  
                                                  g_main_loop_run ( main_loop );

                                                  g_main_loop_unref ( main_loop );    

                                                  
                                          //        GMainLoop   *main_loop1 = g_main_loop_new ( NULL, FALSE );
                                                  
                                                  // Get the username from the JID
                                                  //gchar *username = get_user_name ( connect_data->username );
                                                  
                                                  // Authenticate with the server
                                                  if ( !lm_connection_authenticate ( connection,
                                                                                   username,
                                                                                   connect_data->password,
                                                                                   connect_data->resource,
                                                                                    ( LmResultFunction ) connection_auth_cb,
                                                                                   main_loop1,NULL,NULL ) )
                                                      {
                                                      iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
                                                      lm_connection_close ( connection, NULL );
                                                      lm_connection_unref ( connection );
                                                      free_data ( connect_data );
                                                      g_main_loop_unref ( main_loop1 );
                                                      g_free ( username );
                                                      return KErrGeneral;
                                                      }

                                                  g_main_loop_run ( main_loop1 );

                                                  g_main_loop_unref ( main_loop1 );
                                                      

                                                  g_free ( username );
                                                  
                                                  // Construct a message
                                                  message = lm_message_new ( connect_data->msg_data->recipient,               
                                                                             LM_MESSAGE_TYPE_MESSAGE );
                                                  lm_message_node_add_child ( message->node, "body", 
                                                                              connect_data->msg_data->message);
                                                  
                                                  // Send message to the server
                                                  if ( !lm_connection_send ( connection, message, NULL ) )
                                                      {
                                                      iLog->Log ( _L ( "lm_connection_send failed" ) );
                                                      lm_connection_close ( connection, NULL );
                                                      lm_connection_unref ( connection );
                                                      lm_message_unref ( message );
                                                      free_data ( connect_data );     
                                                      return KErrGeneral;
                                                      }   
                                                  
                                                  // Close the connection
                                                  lm_connection_close ( connection, NULL );
                                                  
                                                  // Remove a reference on connection
                                                  lm_connection_unref ( connection );
                                                  
                                                  // Remove a reference on message
                                                  lm_message_unref ( message );
                                                  
                                                  free_data ( connect_data );
                                                  
                                                  // Message sending passed
                                                  iLog->Log ( _L ( "lm_connection_send passed" ) );
                                                  
//tenth time
                                                  // Read a data from the CFG file
                                                       if ( read_data ( connect_data, aItem ) != RC_OK )
                                                           {
                                                           iLog->Log ( _L ( "read_data failed" ) );
                                                           free_data ( connect_data );
                                                           return KErrGeneral;
                                                           }
                                                       iLog->Log ( _L ( "after read_data" ) );
                                                       
                                                       // Open a new closed connection
                                                       connection = lm_connection_new ( connect_data->server );
                                                       iLog->Log ( _L ( "after lm_connection_new " ) );
                                                       if ( !connection )
                                                           {
                                                           iLog->Log ( _L ( "lm_connection_new failed" ) );
                                                           free_data ( connect_data );
                                                           return KErrGeneral;
                                                           }
                                                       
                                                       // Set the gtalk's SSL port
                                                       lm_connection_set_port ( connection, GTALK_SSL_PORT );
                                                       
                                                       // Set the JID
                                                       lm_connection_set_jid ( connection, connect_data->username );
                                                       
                                                       // Proxy settings for Emulator
                                                   #ifdef __WINSCW__
                                                       SetProxy ( connection, connect_data->proxy_data );
                                                   #endif
                                                       
                                                       // Set the connection to use SSL
                                                       SSLInit ( connection );
                                                       
                                             //           *main_loop = g_main_loop_new ( NULL, FALSE );

                                                       iLog->Log ( _L ( "before lm_connection_open" ) );
                                                       
                                                       if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
                                                                                  main_loop, NULL, NULL ) ) 
                                                           {
                                                           iLog->Log ( _L ( "lm_connection_open failed" ));
                                                           lm_connection_unref ( connection );
                                                           g_main_loop_unref ( main_loop );
                                                           free_data ( connect_data );     
                                                           return KErrGeneral;
                                                           }
                                                           
                                                       iLog->Log ( _L ( "after lm_connection_open" ) );
                                                       
                                                       g_main_loop_run ( main_loop );

                                                       g_main_loop_unref ( main_loop );    

                                                       
                                            //           GMainLoop   *main_loop1 = g_main_loop_new ( NULL, FALSE );
                                                       
                                                       // Get the username from the JID
                                                       //gchar *username = get_user_name ( connect_data->username );
                                                       
                                                       // Authenticate with the server
                                                       if ( !lm_connection_authenticate ( connection,
                                                                                        username,
                                                                                        connect_data->password,
                                                                                        connect_data->resource,
                                                                                         ( LmResultFunction ) connection_auth_cb,
                                                                                        main_loop1,NULL,NULL ) )
                                                           {
                                                           iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
                                                           lm_connection_close ( connection, NULL );
                                                           lm_connection_unref ( connection );
                                                           free_data ( connect_data );
                                                           g_main_loop_unref ( main_loop1 );
                                                           g_free ( username );
                                                           return KErrGeneral;
                                                           }

                                                       g_main_loop_run ( main_loop1 );

                                                       g_main_loop_unref ( main_loop1 );
                                                           

                                                       g_free ( username );
                                                       
                                                       // Construct a message
                                                       message = lm_message_new ( connect_data->msg_data->recipient,               
                                                                                  LM_MESSAGE_TYPE_MESSAGE );
                                                       lm_message_node_add_child ( message->node, "body", 
                                                                                   connect_data->msg_data->message);
                                                       
                                                       // Send message to the server
                                                       if ( !lm_connection_send ( connection, message, NULL ) )
                                                           {
                                                           iLog->Log ( _L ( "lm_connection_send failed" ) );
                                                           lm_connection_close ( connection, NULL );
                                                           lm_connection_unref ( connection );
                                                           lm_message_unref ( message );
                                                           free_data ( connect_data );     
                                                           return KErrGeneral;
                                                           }   
                                                       
                                                       // Close the connection
                                                       lm_connection_close ( connection, NULL );
                                                       
                                                       // Remove a reference on connection
                                                       lm_connection_unref ( connection );
                                                       
                                                       // Remove a reference on message
                                                       lm_message_unref ( message );
                                                       
                                                       free_data ( connect_data );
                                                       
                                                       // Message sending passed
                                                       iLog->Log ( _L ( "lm_nft_loginlogoutmultipletime" ) );
                                                       
                                                  
     return KErrNone;
}
 
 TInt Ctstlm::lm_nft_receiving100_message_L(CStifItemParser& aItem)
     {
     LmConnection *connection   = NULL;
     ConnectData  *connect_data = NULL;
     LmMessage    *message = NULL;
     LmMessageHandler *handler = NULL;
     HandleData *handle_data = NULL;
     
     iLog->Log ( _L ( "lm_nft_receiving100_message_L" ) );
     
     // Read a data from the CFG file
     if ( read_data ( connect_data, aItem ) != RC_OK )
         {
         iLog->Log ( _L ( "read_data failed" ) );
         free_data ( connect_data );
         return KErrGeneral;
         }
     iLog->Log ( _L ( "after read_data" ) );
     
     // Open a new closed connection
     connection = lm_connection_new ( connect_data->server );
     iLog->Log ( _L ( "after lm_connection_new " ) );
     if ( !connection )
         {
         iLog->Log ( _L ( "lm_connection_new failed" ) );
         free_data ( connect_data );
         return KErrGeneral;
         }
     
     // Set the gtalk's SSL port
     lm_connection_set_port ( connection, GTALK_SSL_PORT );
     
     // Set the JID
     lm_connection_set_jid ( connection, connect_data->username );
     
     // Proxy settings for Emulator
 #ifdef __WINSCW__
     SetProxy ( connection, connect_data->proxy_data );
 #endif
     
     // Set the connection to use SSL
     SSLInit ( connection );
     
     GMainLoop   *main_loop = g_main_loop_new ( NULL, FALSE );
     iLog->Log ( _L ( "before lm_connection_open" ) );   
     if ( !lm_connection_open ( connection, ( LmResultFunction ) connection_open_cb,
                                main_loop, NULL, NULL ) ) 
         {
         iLog->Log ( _L ( "lm_connection_open failed" ));
         lm_connection_unref ( connection );
         g_main_loop_unref ( main_loop );
         free_data ( connect_data );     
         return KErrGeneral;
         }
         
     iLog->Log ( _L ( "after lm_connection_open" ) );    
     g_main_loop_run ( main_loop );
     g_main_loop_unref ( main_loop );    
     
     main_loop = g_main_loop_new ( NULL, FALSE );    
     // Get the username from the JID
     gchar *username = get_user_name ( connect_data->username );
     
     // Authenticate with the server
     if ( !lm_connection_authenticate ( connection,
                                      username,
                                      connect_data->password,
                                      connect_data->resource,
                                       ( LmResultFunction ) connection_auth_cb,
                                      main_loop,NULL,NULL ) )
         {
         iLog->Log ( _L ( "lm_connection_authenticate failed" ) );
         lm_connection_close ( connection, NULL );
         lm_connection_unref ( connection );
         free_data ( connect_data );
         g_main_loop_unref ( main_loop );
         g_free ( username );
         return KErrGeneral;
         }

     g_main_loop_run ( main_loop );
     g_main_loop_unref ( main_loop );
     g_free ( username );

     //------------------------------------------------------------------------
     //Send own presence
     message = lm_message_new_with_sub_type ( NULL,
                                          LM_MESSAGE_TYPE_PRESENCE,
                                          LM_MESSAGE_SUB_TYPE_AVAILABLE );
     
     gboolean result = lm_connection_send ( connection, message, NULL ); 
    // GMainLoop  *main_loop1;
        
     //------------------------------------------------------------------------
     //
     // Register a handler to recieve msgs
     //
     handle_data = g_new0 ( HandleData, 1 );
     GMainLoop *main_loop1 = g_main_loop_new ( NULL, FALSE );
     handle_data->main_loop=main_loop1;
     handle_data->count=0;
     handler = lm_message_handler_new ( 
                          (LmHandleMessageFunction)handle_100_messages,
                          handle_data,
                          NULL );
     
     lm_connection_register_message_handler ( connection,
                                              handler,
                                              LM_MESSAGE_TYPE_MESSAGE,
                                              LM_HANDLER_PRIORITY_FIRST );
     
     g_main_loop_run ( main_loop1 );
     g_main_loop_unref ( main_loop1 );
     //------------------------------------------------------------------------
        
     lm_message_unref(message);
     lm_message_handler_unref(handler);
     // Close the connection
     lm_connection_close ( connection, NULL );   
     // Remove a reference on connection
     lm_connection_unref ( connection );     
     free_data ( connect_data );
     
     // Message sending passed
     iLog->Log ( _L ( "lm_nft_receiving100_message_L" ) );
     
     return KErrNone;
}	    
//  End of File
