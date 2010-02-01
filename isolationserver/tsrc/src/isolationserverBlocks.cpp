/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   isolationserverBlocks.cpp
*
*/




// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>
#include "isolationserver.h"
#include  <e32const.h>

#include <glib.h>
#include <glib/gprintf.h>           // for g_stpcpy
#include <string.h>
#include <stdlib.h>

#include <glowmem.h>
#include "msgliterals.h"
#include "msg_enums.h"
#include <math.h>
#define ONE_K_OPENC_FIX 1020

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
	gchar      *proxy_port;	
}ProxyData;

typedef struct
{
	gchar       *server;
	gchar       *username;
	gchar       *password;		
	gchar       *resource;
	MessageData *msg_data;
	gchar       *port;	

    ProxyData   *proxy_data;
    gchar       *contact;
    gchar		*connmgr_bus;
    gchar		*connmgr_path;
    gchar		*protocol; 
    gchar		*key_given_name;
    gchar		*key_middle_name;
    gchar		*key_jid;
    gchar		*key_full_name;
    gchar       *mimetype;
    gchar       *image_file_name; 

} ConnectData;


// Function to read data from the cfg file
static RC_Code_t read_data ( ConnectData*& connect_data, 
                             CStifItemParser& aItem );
                             
// Function to free the allocated resources
static void free_data ( ConnectData*& connect_data );                             

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================



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
	

	ProxyData           *proxy_data = NULL;
//	TInt                proxy_port;


	
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
	

    proxy_data = g_new0 ( ProxyData, 1 );
    if ( !proxy_data ) 
    	{
    	free_data ( connect_data );
    	return RC_ERROR;
    	}
	connect_data->proxy_data = proxy_data;


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
    	
    	if ( aItem.GetNextString ( string ) == KErrNone )
    	{
    	temp.Copy ( string );
    	temp.Append ( '\0' );
    	proxy_data->proxy_port = g_new0 ( gchar, temp.Length() );
    	if ( !proxy_data->proxy_port )
    		{
    		free_data ( connect_data );
    		return RC_ERROR;
    		}
    	g_stpcpy ( proxy_data->proxy_port, ( gchar * ) temp.Ptr() );
    	}
    	
    	
    	
    /*	if ( aItem.GetNextString ( proxy_port ) == KErrNone )
    		{
    		proxy_data->proxy_port = proxy_port;
    		}  */  	
 

	// Read POrt address
	if ( aItem.GetNextString ( string ) == KErrNone )
	    {
	    temp.Copy ( string );
	    temp.Append ( '\0' );
	    connect_data->port = g_new0 ( gchar, temp.Length() );
	    if ( !connect_data->port )
	        {
	    	free_data ( connect_data );
	    	return RC_ERROR;
	        }
	    g_stpcpy ( connect_data->port, ( gchar * ) temp.Ptr() );		    
	    }
  
  // read Add member
       if ( aItem.GetNextString ( string ) == KErrNone )
	    {
		temp.Copy ( string );
		temp.Append ( '\0' );
		connect_data->contact = g_new0 ( gchar, temp.Length() );
		if ( !connect_data->contact )
		    {			
			free_data ( connect_data );
			return RC_ERROR;
		    }
		g_stpcpy ( connect_data->contact, ( gchar * ) temp.Ptr() ); 
	    }
  
   	// read CONNMGR_BUS
       if ( aItem.GetNextString ( string ) == KErrNone )
	    {
		temp.Copy ( string );
		temp.Append ( '\0' );
		connect_data->connmgr_bus = g_new0 ( gchar, temp.Length() );
		if ( !connect_data->connmgr_bus )
		    {			
			free_data ( connect_data );
			return RC_ERROR;
		    }
		g_stpcpy ( connect_data->connmgr_bus, ( gchar * ) temp.Ptr() ); 
	    }
	
	// read CONNMGR_PATH
       if ( aItem.GetNextString ( string ) == KErrNone )
	    {
		temp.Copy ( string );
		temp.Append ( '\0' );
		connect_data->connmgr_path = g_new0 ( gchar, temp.Length() );
		if ( !connect_data->connmgr_path )
		    {			
			free_data ( connect_data );
			return RC_ERROR;
		    }
		g_stpcpy ( connect_data->connmgr_path, ( gchar * ) temp.Ptr() ); 
	    }
	    
	// read PROTOCOL
       if ( aItem.GetNextString ( string ) == KErrNone )
	    {
		temp.Copy ( string );
		temp.Append ( '\0' );
		connect_data->protocol = g_new0 ( gchar, temp.Length() );
		if ( !connect_data->protocol )
		    {			
			free_data ( connect_data );
			return RC_ERROR;
		    }
		g_stpcpy ( connect_data->protocol, ( gchar * ) temp.Ptr() ); 
	    }	 
	        
    
	    // read given Name
       if ( aItem.GetNextString ( string ) == KErrNone )
	    {
		temp.Copy ( string );
		temp.Append ( '\0' );
		connect_data->key_given_name = g_new0 ( gchar, temp.Length() );
		if ( !connect_data->key_given_name )
		    {			
			free_data ( connect_data );
			return RC_ERROR;
		    }
		g_stpcpy ( connect_data->key_given_name, ( gchar * ) temp.Ptr() ); 
	    }	 
	    
	    // read middle Name
       if ( aItem.GetNextString ( string ) == KErrNone )
	    {
		temp.Copy ( string );
		temp.Append ( '\0' );
		connect_data->key_middle_name = g_new0 ( gchar, temp.Length() );
		if ( !connect_data->key_middle_name )
		    {			
			free_data ( connect_data );
			return RC_ERROR;
		    }
		g_stpcpy ( connect_data->key_middle_name, ( gchar * ) temp.Ptr() ); 
	    }	 
	    
	    // read email Name
       if ( aItem.GetNextString ( string ) == KErrNone )
	    {
		temp.Copy ( string );
		temp.Append ( '\0' );
		connect_data->key_jid = g_new0 ( gchar, temp.Length() );
		if ( !connect_data->key_jid )
		    {			
			free_data ( connect_data );
			return RC_ERROR;
		    }
		g_stpcpy ( connect_data->key_jid, ( gchar * ) temp.Ptr() ); 
	    }	 
	    
	    // read full Name
       if ( aItem.GetNextString ( string ) == KErrNone )
	    {
		temp.Copy ( string );
		temp.Append ( '\0' );
		connect_data->key_full_name = g_new0 ( gchar, temp.Length() );
		if ( !connect_data->key_full_name )
		    {			
			free_data ( connect_data );
			return RC_ERROR;
		    }
		g_stpcpy ( connect_data->key_full_name, ( gchar * ) temp.Ptr() ); 
	    }	 
       // read mimetype
       if ( aItem.GetNextString ( string ) == KErrNone )
           {
           temp.Copy ( string );
           temp.Append ( '\0' );
           connect_data->mimetype = g_new0 ( gchar, temp.Length() );
           if ( !connect_data->mimetype )
               {           
               free_data ( connect_data );
               return RC_ERROR;
               }
           g_stpcpy ( connect_data->mimetype, ( gchar * ) temp.Ptr() ); 
           }    
       // read image filename
       if ( aItem.GetNextString ( string ) == KErrNone )
           {
           temp.Copy ( string );
           temp.Append ( '\0' );
           connect_data->image_file_name = g_new0 ( gchar, temp.Length() );
           if ( !connect_data->image_file_name )
               {           
               free_data ( connect_data );
               return RC_ERROR;
               }
           g_stpcpy ( connect_data->image_file_name, ( gchar * ) temp.Ptr() ); 
           }          
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
	  
	    if ( connect_data->port )
		    {
			g_free ( connect_data->port );
			connect_data->port = NULL;
		    }  
		    
		    
		if ( connect_data->contact )
		    {
			g_free ( connect_data->contact );
			connect_data->contact = NULL;
		    }
		    
		if ( connect_data->key_given_name )
		    {
			g_free ( connect_data->key_given_name );
			connect_data->key_given_name = NULL;
		    }
		    
		if ( connect_data->key_middle_name )
		    {
			g_free ( connect_data->key_middle_name );
			connect_data->key_middle_name = NULL;
		    }
		    
		if ( connect_data->key_jid )
		    {
			g_free ( connect_data->key_jid );
			connect_data->key_jid = NULL;
		    }
		    
		if ( connect_data->key_full_name )
		    {
			g_free ( connect_data->key_full_name );
			connect_data->key_full_name = NULL;
		    }  
		          
		g_free ( connect_data );
		connect_data = NULL;    
	    }
    }
    
    //-----------------------------------------------------------------------------
// function_name	: running isolation server exe
// description     	: Deallocates all the data
// Returns          : None
//-----------------------------------------------------------------------------
  int  run_isoserver()
  {
  	// running isoserver 
    RProcess isosrv_launcher;
    TInt lunch_err = 0;
    _LIT(KProcess, "isoserver.exe");
    lunch_err = isosrv_launcher.Create( KProcess, KNullDesC );
     if ( KErrNone != lunch_err ) 
	    {
		return KErrGeneral;
	    	
	    }
	isosrv_launcher.Resume();
	return KErrNone;
  }

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cisolationserver::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cisolationserver::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Cisolationserver::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        
        ENTRY( "SecondSearch", Cisolationserver::SecondSearch), 
        ENTRY( "SearchAndAddContact", Cisolationserver::SearchAndAddContact ),
        ENTRY( "LoginWithCorrectData", Cisolationserver::LoginWithCorrectdata ),
        ENTRY( "LoginWithWrongPassword", Cisolationserver::LoginWithWrongPassword ),
        ENTRY( "LoginWithUnexistUserId", Cisolationserver::LoginWithUnexistUserId),
        ENTRY( "LoginWithUnexistServer", Cisolationserver::LoginWithUnexistServer),
        ENTRY( "LoginWithMixedLetterUserId", Cisolationserver::LoginWithMixedletterUserId),
        ENTRY( "LoginWithMaximumLenghtUserId", Cisolationserver::LoginWithMaximumLengthUserId),
        ENTRY( "LoginWithMinimumLenghtUserId", Cisolationserver::LoginWithMinimumLengthUserId),
        ENTRY( "LogoutFromServer", Cisolationserver::LogoutFromServer),
        ENTRY( "LoginWithBlankUserId", Cisolationserver::LoginWithBlankUserId),
        ENTRY( "LoginLogOutWithReapitiveSession", Cisolationserver::LoginLogOutWithReapitiveSession),
        ENTRY( "LoginWithCoreectUserIDWithSpace", Cisolationserver::LoginWithCoreectUserIDWithSpace),
        ENTRY( "LoginWithInvalidPortAddress", Cisolationserver::LoginWithInvalidPortAddress),
        ENTRY( "LoginWithMissingSettings", Cisolationserver::LoginWithMissingSettings),
        ENTRY( "LoginWhenTGAndDDAreNotRunning", Cisolationserver::LoginWhenTGAndDDAreNotRunning),
        ENTRY( "SendMessageToCorrectUserID", Cisolationserver::SendMessageToCorrectUserID),
        ENTRY( "SendMaxLenMessageToCorrectUserID", Cisolationserver::SendMaxLenMessageToCorrectUserID),
        ENTRY( "SendTextMessageUnExistUserID", Cisolationserver::SendTextMessageUnExistUserID),
        ENTRY( "SendTextMessageWithSpecialCharacter", Cisolationserver::SendTextMessageWithSpecialCharacter),
        ENTRY( "SendMessageWithBlankUserID", Cisolationserver::SendMessageWithBlankUserID),
        ENTRY( "SendSmilyToCorrectUserID", Cisolationserver::SendSmilyToCorrectUserID),
        ENTRY( "SendBlankMessageToUserID", Cisolationserver::SendBlankMessageToUserID),
        ENTRY( "SendMessageToOfflineContact", Cisolationserver::SendMessageToOfflineContact),
        ENTRY( "SendContinuousltReapitiveMessage", Cisolationserver::SendContinuousltReapitiveMessage),
        ENTRY( "SendTextMessageToBlockedUser", Cisolationserver::SendTextMessageToBlockedUser),
        ENTRY( "SendTextMessageToUserIdWhoIsBlockedSender", Cisolationserver::SendTextMessageToUserIdWhoIsBlockedSender),
        ENTRY( "SendLongTextMessageToUserId", Cisolationserver::SendLongTextMessageToUserId),
        ENTRY( "SendTextMessageUserWhoIsNotAddedInContactList", Cisolationserver::SendTextMessageUserWhoIsNotAddedInContactList),
        ENTRY( "MessageReceiveFromUser", Cisolationserver::MessageReceiveFromUser),
        ENTRY( "AddExistingContact", Cisolationserver::AddExistingContact),
        ENTRY( "ClientSendBlankUserIdForADD", Cisolationserver::ClientSendBlankUserIdForADD),
        ENTRY( "AddContactWhichIsAlreadyAdded", Cisolationserver::AddContactWhichIsAlreadyAdded),
        ENTRY( "AddExistingContactWIthSpace", Cisolationserver::AddExistingContactWIthSpace),
        ENTRY( "AddUnexistingUserID", Cisolationserver::AddUnexistingUserID),
        ENTRY( "AddExistingContactWIthMixedLetter", Cisolationserver::AddExistingContactWIthMixedLetter),
        ENTRY( "AddInvalidContactWIthSpecialLetter", Cisolationserver::AddInvalidContactWIthSpecialLetter),
        ENTRY( "DeleteExistingContact", Cisolationserver::DeleteExistingContact),
        ENTRY( "ClientSendBlankUserIdForDELETE", Cisolationserver::ClientSendBlankUserIdForDELETE),
        ENTRY( "DeleteContactWhichIsAlreadyDeleted", Cisolationserver::DeleteContactWhichIsAlreadyDeleted),
        ENTRY( "DeleteExistingContactWIthSpace", Cisolationserver::DeleteExistingContactWIthSpace),
        ENTRY( "DeleteUnexistingUserID", Cisolationserver::DeleteUnexistingUserID),
        ENTRY( "DeleteExistingContactWIthMixedLetter", Cisolationserver::DeleteExistingContactWIthMixedLetter),
        ENTRY( "DeleteInvalidContactWIthSpecialLetter", Cisolationserver::DeleteInvalidContactWIthSpecialLetter),
        ENTRY( "FetchContatcs", Cisolationserver::FetchContatcs),
        ENTRY( "AddContatcs", Cisolationserver::AddContatcs),
        ENTRY( "UpdatePresenceWithAway", Cisolationserver::UpdatePresenceWithAway),
        ENTRY( "UpdatePresenceWithDnd", Cisolationserver::UpdatePresenceWithDnd),
        ENTRY( "UpdatePresenceWithAvailable", Cisolationserver::UpdatePresenceWithAvailable),
        ENTRY( "UpdatePresenceMultipleTimeInSameSession", Cisolationserver::UpdatePresenceMultipleTimeInSameSession),
        ENTRY( "UpdateOnlyStatusText", Cisolationserver::UpdateOnlyStatusText),
        ENTRY( "UpdateOnlyUserAvailability", Cisolationserver::UpdateOnlyUserAvailability),
        ENTRY( "UpdateOwnPresenceWithBlankData", Cisolationserver::UpdateOwnPresenceWithBlankData),
        ENTRY( "UpdateStatusTextWithMaxLen", Cisolationserver::UpdateStatusTextWithMaxLen),
        ENTRY( "UpdateStatusTextWithSpecialCharacter", Cisolationserver::UpdateStatusTextWithSpecialCharacter),
        ENTRY( "HandelingPresenceNotification", Cisolationserver::HandelingPresenceNotification),
        ENTRY( "SearchKeyValue", Cisolationserver::SearchKeyValue),
        ENTRY( "SearchKeyValueWithResult", Cisolationserver::SearchKeyValueWithResult ),
        ENTRY( "SearchKeyValueInvalidParam", Cisolationserver::SearchKeyValueInvalidParam ),
        ENTRY( "SearchForLargeNoOfResult", Cisolationserver::SearchForLargeNoOfResult ),
        ENTRY( "SearchKeyValueInvalidParam2", Cisolationserver::SearchKeyValueInvalidParam2 ),
        ENTRY( "SearchKeyValueInvalidHashValues", Cisolationserver::SearchKeyValueInvalidHashValues ),
        ENTRY( "SearchKeyValueBlank", Cisolationserver::SearchKeyValueBlank ),
        ENTRY( "SearchKeyValueSpecialChar", Cisolationserver::SearchKeyValueSpecialChar ),
        ENTRY( "SearchKeyValueLargeString", Cisolationserver::SearchKeyValueLargeString ),
        ENTRY( "AcceptAddRequestFromUser", Cisolationserver::AcceptAddRequestFromUser ),
        ENTRY( "RejectAddRequestFromUser", Cisolationserver::RejectAddRequestFromUser ),
        ENTRY( "SendAcceptRequestWithoutReceivingAddrequest", Cisolationserver::SendAcceptRequestWithoutReceivingAddrequest ),
        ENTRY( "SendRejectRequestWithoutReceivingAddrequest", Cisolationserver::SendRejectRequestWithoutReceivingAddrequest ),
        ENTRY( "SendAcceptRequestWithoutAnyUserID", Cisolationserver::SendAcceptRequestWithoutAnyUserID ),
        ENTRY( "SendRejectRequestWithoutAnyUserID", Cisolationserver::SendRejectRequestWithoutAnyUserID ),
        ENTRY( "LogoutWhileUserIsNotLoggedIn", Cisolationserver::LogoutWhileUserIsNotLoggedIn),
        ENTRY( "GetKeysAndSearch", Cisolationserver::GetKeysAndSearch),
        ENTRY( "SetOwnAvatar", Cisolationserver::SetOwnAvatar),
        ENTRY( "ClearOwnAvatar", Cisolationserver::ClearOwnAvatar),
        ENTRY( "FetchCachedContacts", Cisolationserver::FetchCachedContacts),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }
    
   // -----------------------------------------------------------------------------
// Cisolationserver::Login with correct data
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::LoginWithCorrectdata( CStifItemParser& aItem )
    {
  	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;

	FILE *fp;	
	int timeout = 100;
	int nBytes;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ELogin_Request;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    //appending password
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    //appending server name
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    // appending IAP id
    smsg[index++]= '1';
    smsg[index++]= '\0';
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    #endif
     //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    /* Send Message to queueOne */
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
   	if( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
     // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	 result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
    if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    return_value = KErrNone;
	goto return_code;
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;

    }


  // -----------------------------------------------------------------------------
// Cisolationserver::Logout from server
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::LogoutFromServer( CStifItemParser& aItem )
    {
  	ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;

	FILE *fp;	
	int timeout = 100;
	int nBytes;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ELogin_Request;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    //appending password
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    //appending server name
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    // appending IAP id
    smsg[index++]= '1';
    smsg[index++]= '\0';
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    #endif
     //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    /* Send Message to queueOne */
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
   	if( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
     // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	 result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
    if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    return_value = KErrNone;
	goto return_code;
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;

    }

  // -----------------------------------------------------------------------------
// Cisolationserver::Login with mixed letter user
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::LoginWithMixedletterUserId( CStifItemParser& aItem )
    {
  	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;

	FILE *fp;	
	int timeout = 100;
	int nBytes;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ELogin_Request;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    //appending password
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    //appending server name
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    // appending IAP id
    smsg[index++]= '1';
    smsg[index++]= '\0';
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    #endif
     //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    /* Send Message to queueOne */
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
   	if( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
     // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	 result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
    if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    return_value = KErrNone;
	goto return_code;
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;

    }
    
    // -----------------------------------------------------------------------------
// Cisolationserver::Login with mixed letter user
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::LoginWithMinimumLengthUserId( CStifItemParser& aItem )
    {
  	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;

	FILE *fp;	
	int timeout = 100;
	int nBytes;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ELogin_Request;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    //appending password
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    //appending server name
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    // appending IAP id
    smsg[index++]= '1';
    smsg[index++]= '\0';
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    #endif
     //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    /* Send Message to queueOne */
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
   	if( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
     // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	 result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
    if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    return_value = KErrNone;
	goto return_code;
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;

    }
    
     // -----------------------------------------------------------------------------
// Cisolationserver::Login with mixed letter user
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::LoginWithMaximumLengthUserId( CStifItemParser& aItem )
    {
  	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;

	FILE *fp;	
	int timeout = 100;
	int nBytes;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ELogin_Request;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    //appending password
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    //appending server name
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    // appending IAP id
    smsg[index++]= '1';
    smsg[index++]= '\0';
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    #endif
     //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    /* Send Message to queueOne */
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
   	if( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
     // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	 result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
    if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    return_value = KErrNone;
	goto return_code;
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;

    } 
// -----------------------------------------------------------------------------
// Cisolationserver::Login with bad password
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::LoginWithWrongPassword( CStifItemParser& aItem )
    {
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    int timeout = 100;
	int nBytes;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
     //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
	/* Send Message to queueOne */
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	if( msg_struct->response != 0)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	if( msg_struct->response != 0)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    goto return_code;
     //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::Login with unexist user id
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::LoginWithUnexistUserId( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	if( msg_struct->response != 0 )
    {
      return_value = KErrGeneral;
	goto return_code;
    }
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 0)
    {
    	return_value = KErrGeneral;
	goto return_code;
    } 
    
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }
    
    
  // -----------------------------------------------------------------------------
// Cisolationserver::connect with unexist server
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::LoginWithUnexistServer( CStifItemParser& aItem )
    {

    /*	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver();     
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	//Send Message to queueOne 
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 0)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 0)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    // login end
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    */
    //LM is not getting call back from server so we are just saying KerrGeneral
    return KErrGeneral;
    }
    
    
    
    // -----------------------------------------------------------------------------
// Cisolationserver::Login with correct data
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::LoginWithBlankUserId( CStifItemParser& aItem )
    {
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    int timeout = 100;
	int nBytes;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}

	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    // blank user id
    smsg[index++]='\0';    
    smsg[index++]='\0';
    
    //appending password
    
   int len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->error_type > 0)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    goto return_code;
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

 // -----------------------------------------------------------------------------
// Cisolationserver::Logout from server
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::LoginLogOutWithReapitiveSession( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	 index=0;
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
	msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isolation server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    	//request formation	
	 index=0;
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
     len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    
    //running isoserver   
    run_isoserver(); 
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
	msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    
    
     //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }
    
    // -----------------------------------------------------------------------------
// Cisolationserver::connect with invalid port
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::LoginWithInvalidPortAddress( CStifItemParser& aItem )
    {

    /*	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    
    smsg[index++]='4';
    smsg[index++]='4';
    smsg[index++]='2';
    
    smsg[index++]='\0';
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	// Send Message to queueOne 
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response == 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    */
    //LM is not getting call back from server so we are just saying KerrGeneral
    return KErrGeneral;
    }

   
   // -----------------------------------------------------------------------------
// Cisolationserver::Login with correct data
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::LoginWithMissingSettings( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    FILE *fp;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
   
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    // ssl is missing here
        
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver();    
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
	
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
   
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 0)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

   // -----------------------------------------------------------------------------
// Cisolationserver::LoginWhenTGAndDDAreNotRunning
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::LoginWhenTGAndDDAreNotRunning( CStifItemParser& aItem )
    {
    /*
    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    FILE *fp;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
   
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver();     
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
	
	
	// Send Message to queueOne 
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
   
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 0)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value; */
    return KErrNone;
    }

    
    // -----------------------------------------------------------------------------
// Cisolationserver::Login with correct data
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::LoginWithCoreectUserIDWithSpace( CStifItemParser& aItem )
    {
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    int timeout = 100;
	int nBytes;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ELogin_Request;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    smsg[index++]='\0';
    //appending password
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
     
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
 
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->error_type >0)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    return_value = KErrNone;
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

 // -----------------------------------------------------------------------------
// Cisolationserver::Send message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::SendMessageToCorrectUserID( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //send messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESend_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;
    
     // adding recipient
    /*len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;*/
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strcpy( smsg + index, connect_data->msg_data->message );
    index += len + 1;
     
    
    
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ESend_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }    
    
    
    
    //send message end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::Send message with max len
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::SendMaxLenMessageToCorrectUserID( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    
    run_isoserver();
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
   //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //send messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESend_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;
    
     // adding recipient
    /*len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;*/
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strcpy( smsg + index, connect_data->msg_data->message );
    index += len + 1;
     
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ESend_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    } 
    
    
    //send message end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    
     //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

   // -----------------------------------------------------------------------------
// Cisolationserver::Send message to unexist user id
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::SendTextMessageUnExistUserID( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
   //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //send messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESend_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;
    
     // adding recipient
    /*len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;*/
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strcpy( smsg + index, connect_data->msg_data->message );
    index += len + 1;
     
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ESend_Request);
   
    /* if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }*/
    
    //send message end---------------------------------------
    
    
   // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

    // -----------------------------------------------------------------------------
// Cisolationserver::Send message to unexist user id
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::SendTextMessageWithSpecialCharacter( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
       
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end

    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end     
    //send messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESend_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;
    
     // adding recipient
    /*len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;*/
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strcpy( smsg + index, connect_data->msg_data->message );
    index += len + 1;
     
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ESend_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //send message end---------------------------------------
    
    
   // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

   // -----------------------------------------------------------------------------
// Cisolationserver::Send message to blank user id
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::SendMessageWithBlankUserID( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    
    //running isoserver   
    run_isoserver(); 
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //send messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESend_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding recipient
    smsg[index++] = '\0';
    
     // adding recipient
    /*len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;*/
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strcpy( smsg + index, connect_data->msg_data->message );
    index += len + 1;
     
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ESend_Request);
   
     if ( msg_struct->response != 0)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //send message end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
     //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

  // -----------------------------------------------------------------------------
// Cisolationserver::Send blank message to user id
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::SendBlankMessageToUserID( CStifItemParser& aItem )
    {
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end

     
    //send messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESend_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;
    
     // adding recipient
    /*len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;*/
    
    smsg[index++] = '\0';
    
    // adding message part
     
    smsg[index++]=' ';
    smsg[index++]='\0';
       
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ESend_Request);
   
     if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    
    
    //send message end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    return_value = KErrNone;
	goto return_code;
     //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }
    
     // -----------------------------------------------------------------------------
// Cisolationserver::Send blank message to user id
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::SendSmilyToCorrectUserID( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //send messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESend_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;
    
     // adding recipient
    /*len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;*/
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strncpy( smsg + index, connect_data->msg_data->message, strlen( connect_data->msg_data->message ) );
    index += len;
    smsg[index++] = ' ';
    smsg[index++]=':';
    smsg[index++]='-';
    smsg[index++]=')';
    smsg[index++] = '\0';
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ESend_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    //send message end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }
    
    // -----------------------------------------------------------------------------
// Cisolationserver::Send message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::SendMessageToOfflineContact( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }   
    
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
    //send messge--------------------------------------------
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESend_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;
    
     // adding recipient
    /*len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;*/
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strcpy( smsg + index, connect_data->msg_data->message );
    index += len + 1;
     
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ESend_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }    
    
    //send message end---------------------------------------
    
   // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
     //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }


// -----------------------------------------------------------------------------
// Cisolationserver::Send message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::SendContinuousltReapitiveMessage( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //send messge 1 --------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESend_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;
    
     // adding recipient
    /*len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;*/
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strcpy( smsg + index, connect_data->msg_data->message );
    index += len + 1;
     
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ESend_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //send message end---------------------------------------
    
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESend_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;
    
     // adding recipient
    /*len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;*/
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strcpy( smsg + index, connect_data->msg_data->message );
    index += len + 1;
     
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ESend_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    

    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
     //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }


// -----------------------------------------------------------------------------
// Cisolationserver::Send message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::SendTextMessageToBlockedUser( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end

     //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
    //send messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESend_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;
    
     // adding recipient
    /*len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;*/
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strcpy( smsg + index, connect_data->msg_data->message );
    index += len + 1;
     
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ESend_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    //send message end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
     //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::Send message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::SendTextMessageToUserIdWhoIsBlockedSender( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end

    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end 
    //send messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESend_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;
    
     // adding recipient
    /*len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;*/
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strcpy( smsg + index, connect_data->msg_data->message );
    index += len + 1;
     
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ESend_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    //send message end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
     //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::Send message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::SendLongTextMessageToUserId( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    
    run_isoserver();
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
   //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //send messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESend_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;
    
     // adding recipient
    /*len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;*/
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strcpy( smsg + index, connect_data->msg_data->message );
    index += len + 1;
     
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ESend_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    } 
    
    
    //send message end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
     //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::Send message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::SendTextMessageUserWhoIsNotAddedInContactList( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end

    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
    //send messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESend_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;
    
     // adding recipient
    /*len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;*/
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strcpy( smsg + index, connect_data->msg_data->message );
    index += len + 1;
     
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ESend_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    //send message end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
     //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

 

// -----------------------------------------------------------------------------
// Cisolationserver::Login with correct data
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::MessageReceiveFromUser( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    int i=0;
	int msglen=0;	
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    FILE *fp ;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    fp = fopen("c:\\receive_message_logs.txt","a");TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    // running isoserver
    run_isoserver();
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	
	
	//send messge to our self--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESend_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	TBool ESend = EFalse , EReceive = EFalse;
	
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;
    
     // adding recipient
    /*len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;*/
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strcpy( smsg + index, connect_data->msg_data->message );
    index += len + 1;
     
    
    
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
	    if(msg_struct->hdr_req.message_type==ESend_Request)
	    ESend=ETrue;
	    if(msg_struct->hdr_req.message_type==EText_Message_Receive)
	    ESend=ETrue;
		} while (ESend/* == ETrue */&& EReceive/* ==ETrue*/) ;//(msg_struct->hdr_req.message_type!=ESend_Request);
   
 /*    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }    
            
    memset( rmsg, '\0', MAX_MSG_SIZE );
        //wait for message receive 
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    fprintf( fp, "receive message %s\n", rmsg+24 ); 
    
    for ( i=24 ; i <rmsg[i]!='\0';i++)
    msglen++;
    fprintf( fp, "receive message %s\n", rmsg+24+msglen+1 ); 
    */
    
    // login end
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
     //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    
    }


// -----------------------------------------------------------------------------
// Cisolationserver::add member message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::AddExistingContact( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    
    //running isoserver   
    run_isoserver(); 
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end

     //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	
	
    //adding contact --------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EAdd_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	// adding add contact
    len = strlen(connect_data->contact );
    strcpy( smsg + index, connect_data->contact );
    index += len + 1;
    smsg[index++]='\0';
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EAdd_Contact_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //add contact end---------------------------------------
    
    
   // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::add member message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::AddUnexistingUserID( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    
    //running isoserver   
    run_isoserver(); 
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
   //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
    //add contact --------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EAdd_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	// adding add contact
    len = strlen(connect_data->contact );
    strcpy( smsg + index, connect_data->contact );
    index += len + 1;
    smsg[index++] = '\0';
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EAdd_Contact_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //add contact end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::add member message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::AddContactWhichIsAlreadyAdded( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    
    //running isoserver   
    run_isoserver(); 
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
    //add member  1 st time--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EAdd_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding add contact
    len = strlen(connect_data->contact );
    strcpy( smsg + index, connect_data->contact );
    index += len + 1;
    
     
    
     
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EAdd_Contact_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //add contact end---------------------------------------
    
     //add member  2 nd time--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EAdd_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding add contact
    len = strlen(connect_data->contact );
    strcpy( smsg + index, connect_data->contact );
    index += len + 1;
    
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EAdd_Contact_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //add contact end---------------------------------------
    

    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }


  // -----------------------------------------------------------------------------
// Cisolationserver::add member message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::ClientSendBlankUserIdForADD( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
     TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end

    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
    //add contact --------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EAdd_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    smsg[index++]='\0';
    //client is sending blank user id
    smsg[index++]='\0';
    
     
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EAdd_Contact_Request);
   
     if( msg_struct->error_type >0)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    
    //add contact end---------------------------------------
    
    
   // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
   if( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::add member message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::AddExistingContactWIthSpace( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    
    //running isoserver   
    run_isoserver(); 
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end

    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	
    //send messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EAdd_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding add contact
    len = strlen(connect_data->contact );
    strcpy( smsg + index, connect_data->contact );
    index += len + 1;
    smsg[index++]=' ';
    
    smsg[index++]= '\0'; 
    
     
    
     
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EAdd_Contact_Request);
   
     if( msg_struct->response != 0)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //add contact end---------------------------------------
    
    
   // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::add member message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::AddExistingContactWIthMixedLetter( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    
    //running isoserver   
    run_isoserver(); 
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //send messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EAdd_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding add contact
    len = strlen(connect_data->contact );
    strcpy( smsg + index, connect_data->contact );
    index += len + 1;
    smsg[index++]='\0';
     
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EAdd_Contact_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //add contact end---------------------------------------
    
    
   // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

  // -----------------------------------------------------------------------------
// Cisolationserver::add member message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::AddInvalidContactWIthSpecialLetter( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    
    //running isoserver   
    run_isoserver(); 
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end

   //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
    //send messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EAdd_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding add contact
    len = strlen(connect_data->contact );
    strcpy( smsg + index, connect_data->contact );
    index += len + 1;
     
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EAdd_Contact_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //add contact end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }


// -----------------------------------------------------------------------------
// Cisolationserver::add member message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::DeleteExistingContact( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //delete contact--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EDelete_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding add contact
    len = strlen(connect_data->contact );
    strcpy( smsg + index, connect_data->contact );
    index += len + 1;
    smsg[index++] = '\0';
     
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EDelete_Contact_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //add contact end---------------------------------------
    
    
   // logout test code from server
   // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::add member message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::DeleteUnexistingUserID( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
   //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end     
    //delete contact--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EDelete_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding add contact
    len = strlen(connect_data->contact );
    strcpy( smsg + index, connect_data->contact );
    index += len + 1;
   
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EDelete_Contact_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //add contact end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::add member message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::DeleteContactWhichIsAlreadyDeleted( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    
    //running isoserver   
    run_isoserver(); 
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
   //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
     
      //add member  1 st time--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EAdd_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
    // addiing in q for deleting contact
    len = strlen(connect_data->contact );
    strcpy( smsg + index, connect_data->contact );
    index += len + 1;
        
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EAdd_Contact_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //add contact end 1---------------------------------------
    
    //delete member  1 st time--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EDelete_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // addiing in q for deleting contact
    len = strlen(connect_data->contact );
    strcpy( smsg + index, connect_data->contact );
    index += len + 1;
        
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EDelete_Contact_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }    
    //delete contact end 1---------------------------------------
    
     //delete contact  2 nd time--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EDelete_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // delete  contact
    len = strlen(connect_data->contact );
    strcpy( smsg + index, connect_data->contact );
    index += len + 1;
    
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EDelete_Contact_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //delete contact end---------------------------------------
    

    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
   //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }


  // -----------------------------------------------------------------------------
// Cisolationserver::add member message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::ClientSendBlankUserIdForDELETE( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
   //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //delete contact --------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EDelete_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    smsg[index++]='\0';
    //client is sending blank user id
    smsg[index++]='\0';
    
     
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EDelete_Contact_Request);
   
     if( msg_struct->error_type >0)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //delete contact end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::add member message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::DeleteExistingContactWIthSpace( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end

     //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	
    //delete contact--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EDelete_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding delete contact in queue
    len = strlen(connect_data->contact );
    strcpy( smsg + index, connect_data->contact );
    index += len + 1;
    smsg[index++]='\0';
        
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EDelete_Contact_Request);
   
      if( msg_struct->response != 0)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //delete contact end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
   //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::add member message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::DeleteExistingContactWIthMixedLetter( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end

    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	
    //delete contact--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EDelete_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding delete contact id
    len = strlen(connect_data->contact );
    strcpy( smsg + index, connect_data->contact );
    index += len + 1;
     
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EDelete_Contact_Request);
   
    
    //delete contact end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

  // -----------------------------------------------------------------------------
// Cisolationserver::add member message
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::DeleteInvalidContactWIthSpecialLetter( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    
   //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //delete contact--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EDelete_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding delete contact
    len = strlen(connect_data->contact );
    strcpy( smsg + index, connect_data->contact );
    index += len + 1;
     
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EDelete_Contact_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //delete contact end---------------------------------------
    
    
   // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::Login with correct data
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::FetchContatcs( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
    // logout test code from server
   // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }


TInt Cisolationserver::AddContatcs( CStifItemParser& aItem )
   	{ 	
   	ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    
    
    //running isoserver   
    run_isoserver(); 
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	//send msg copy	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESend_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	//------------------------1
    // adding recipient
    len = strlen( "santhosh09431243.ic@gmail.com" );
    strcpy( smsg + index, "santhosh09431243.ic@gmail.com" );
    index += len + 1;
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strcpy( smsg + index, connect_data->msg_data->message );
    index += len + 1;
         
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ESend_Request);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //send msg copy	end
    
    
 	
   //EAdd_Contact_Request messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EAdd_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding recipient
    //len = strlen( "tele1236@gmail.com" );
    //strcpy( smsg + index, "tele1236@gmail.com" );
    //index += len + 1;
    
    strcpy( smsg + index, "testlm123@gmail.com" );
    index += strlen( "testlm123@gmail.com" ) + 1;	
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strcpy( smsg + index, connect_data->msg_data->message );
    index += len + 1;
        
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EAdd_Contact_Request);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
   // sleep( 50 );
    //EAdd_Contact_Request message end---------------------------------------
 
 /*	sleep( 100 );
 //--------------------------------Trying to add client notification----------------------
 
 	status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if ( status < 0 )
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
	msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    */
 //--------------------------------Trying to add client end----------------------
 
 
 
     
    //EAdd_Contact_Request messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EAdd_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;
    
    strcpy( smsg + index, "tele1236@gmail.com" );
    index += strlen( "tele1236@gmail.com" ) + 1;	
    
     // adding recipient
    //len = strlen(connect_data->msg_data->recipient );
    //strcpy( smsg + index, connect_data->msg_data->recipient );
    //index += len + 1;
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strcpy( smsg + index, connect_data->msg_data->message );
    index += len + 1;
        
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EAdd_Contact_Request);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //EAdd_Contact_Request message end---------------------------------------
 
 
    
    //EAdd_Contact_Request -ve messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EAdd_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	//-----------------1
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;
    
    strcpy( smsg + index, "tele1236@gmail.com" );
    index += strlen( "tele1236@gmail.com" ) + 1;	
    
    smsg[index++] = '\0';
    
    // adding message part
        
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EAdd_Contact_Request);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //-----------------2 -ve
    index = 0;
    index += sizeof( message_hdr_req );
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    memcpy( smsg + index, connect_data->msg_data->recipient, len );
    index += len;
    
    // adding message part
        
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EAdd_Contact_Request);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //-----------------
    
    
    //EAdd_Contact_Request -ve message end---------------------------------------
      
    //send messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESend_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	//------------------------1
    // adding recipient
    len = strlen( "santhosh.ic@gmail.com" );
    strcpy( smsg + index, "santhosh.ic@gmail.com" );
    index += len + 1;
    
     // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strcpy( smsg + index, connect_data->msg_data->message );
    index += len + 1;
     
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ESend_Request);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //-----------------------------------2 -ve
    index = 0;
    index += sizeof( message_hdr_req );
    // adding recipient
    for ( int i = 0; i <= 11; i++ ) 
	    {
	    len = strlen( "santhosh.ic@gmail.com" );
	    strcpy( smsg + index, "santhosh.ic@gmail.com" );
	    index += len + 1;
	    }
    smsg[index++] = '\0';
    // adding message part
        
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ESend_Request);
   
    if ( msg_struct->response != 0)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    //-----------------------------------3 -ve
    index = 0;
    index += sizeof( message_hdr_req );
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    memcpy( smsg + index, connect_data->msg_data->recipient, len );
    index += len;
    
    // adding message part
        
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ESend_Request);
   
    if ( msg_struct->response != 0)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //send message end---------------------------------------
    
   //EDelete_Contact_Request messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EDelete_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;
    
    strcpy( smsg + index, "tele1236@gmail.com" );
    index += strlen( "tele1236@gmail.com" ) + 1;	
    
     // adding recipient
    //len = strlen(connect_data->msg_data->recipient );
    //strcpy( smsg + index, connect_data->msg_data->recipient );
    //index += len + 1;
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strcpy( smsg + index, connect_data->msg_data->message );
    index += len + 1;
        
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EDelete_Contact_Request);
   
    if ( msg_struct->response != 0)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //--------------------delete----------------------
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EDelete_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding recipient
    len = strlen(connect_data->msg_data->recipient );
    strcpy( smsg + index, connect_data->msg_data->recipient );
    index += len + 1;
    
    strcpy( smsg + index, "rakesh.harsh@gmail.com" );
    index += strlen( "rakesh.harsh@gmail.com" ) + 1;	
    
     // adding recipient
    //len = strlen(connect_data->msg_data->recipient );
    //strcpy( smsg + index, connect_data->msg_data->recipient );
    //index += len + 1;
    
    smsg[index++] = '\0';
    
    // adding message part
     
    len = strlen(connect_data->msg_data->message );
    strcpy( smsg + index, connect_data->msg_data->message );
    index += len + 1;
        
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EDelete_Contact_Request);
   
    

    //EDelete_Contact_Request message end--------------------------------------- 
    
    //--------------Recieve message-----------------------------------
   /* do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EText_Message_Receive);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EText_Message_Receive);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    } */   //------------------------Recieve message end---------------------
  
   //-------------update presence with available---------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EUpdateOwnPresence;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // updating availability
    char *availability = "available";
    memcpy( smsg + index, availability, strlen( availability ) );
    index += strlen( availability ); 
    
    smsg[index++]='\0';
    
    //updating status text
     char *statustext = "i m busy";
     memcpy( smsg + index, statustext, strlen( statustext ) );
     index += strlen( statustext ); 
    
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EUpdateOwnPresence);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
//  ------------------------------------------------------------------

     //-------------update presence with available---------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EUpdateOwnPresence;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // updating availability
     char *availability1 = "dnd";
    memcpy( smsg + index, availability1, strlen( availability1 ) );
    index += strlen( availability1 ); 
    
    smsg[index++]='\0';
    
    //updating status text
      char *statustext1 = "i m busy";
     memcpy( smsg + index, statustext1, strlen( statustext1 ) );
     index += strlen( statustext1 ); 
    
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EUpdateOwnPresence);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
//  ------------------------------------------------------------------

 //-------------update presence with available---------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EUpdateOwnPresence;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // updating availability
    char *availability2 = "away";
    memcpy( smsg + index, availability2, strlen( availability2 ) );
    index += strlen( availability2 ); 
    
    smsg[index++]='\0';
    
    //updating status text
     char *statustext2 = "i m busy";
     memcpy( smsg + index, statustext2, strlen( statustext2 ) );
     index += strlen( statustext2 ); 
    
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EUpdateOwnPresence);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
//  ------------------------------------------------------------------

   //----------------------------presence negative case------------------
   
   memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EUpdateOwnPresence;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // updating availability with blank 
    smsg[index++]='\0';
    smsg[index++]='\0';
    //smsg[index++]='\0';
    
    //updating status text
    char *statustext4 = "i m not on desk";
    memcpy( smsg + index, statustext4, strlen( statustext4 ) );
    index += strlen( statustext4 );  
    
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EUpdateOwnPresence);
   
    if ( msg_struct->error_type > 0)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
   
  //-------------------------1 end 
  
  // update 2 negative
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EUpdateOwnPresence;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // updating availability
    char *availability4 = "available";
    memcpy( smsg + index, availability, strlen( availability4 ) );
    index += strlen( availability4 ); 
      
       
    
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EUpdateOwnPresence);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //----
   // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    	}

 // -----------------------------------------------------------------------------
// Cisolationserver::UpdatePresenceWithAway
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::UpdatePresenceWithAway( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    FILE *fp;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;

    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		

	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    
 
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	//fetch end
	
	 //-------------update presence---------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EUpdateOwnPresence;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // updating availability
    char *availability = "away";
    memcpy( smsg + index, availability, strlen( availability ) );
    index += strlen( availability ); 
    
    smsg[index++]='\0';
    
    //updating status text
     char *statustext = "i m busy";
     memcpy( smsg + index, statustext, strlen( statustext ) );
     index += strlen( statustext ); 
    
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EUpdateOwnPresence);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	 result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

  // -----------------------------------------------------------------------------
// Cisolationserver::UpdatePresenceWithAway
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::UpdatePresenceWithAvailable( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    FILE *fp;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;

    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		

	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    
 
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	//fetch end
	
	 //-------------update presence---------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EUpdateOwnPresence;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // updating availability
    char *availability = "available";
    memcpy( smsg + index, availability, strlen( availability ) );
    index += strlen( availability ); 
    smsg[index++]='\0';
    
    //updating status text
    char *statustext = "there is difference between thought and execution";
    memcpy( smsg + index, statustext, strlen( statustext ) );
    index += strlen( statustext ); 
    
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EUpdateOwnPresence);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::UpdatePresenceWithDnd
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::UpdatePresenceWithDnd( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    FILE *fp;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;

    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
     //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		

	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    
 
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	//fetch end
	
	 //-------------update presence---------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EUpdateOwnPresence;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // updating availability
    char *availability = "dnd";
    memcpy( smsg + index, availability, strlen( availability ) );
    index += strlen( availability ); 
    smsg[index++]='\0';
    
    //updating status text
    char *statustext = "i m busy";
    memcpy( smsg + index, statustext, strlen( statustext ) );
    index += strlen( statustext ); 
    
       
    
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EUpdateOwnPresence);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::UpdatePresenceWithDnd
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::UpdatePresenceMultipleTimeInSameSession( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    FILE *fp;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;

    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		

	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    
 
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	//fetch end
	
	 //-------------update presence---------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EUpdateOwnPresence;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // updating availability
    char *availability = "dnd";
    memcpy( smsg + index, availability, strlen( availability ) );
    index += strlen( availability ); 
    smsg[index++]='\0';
    
   //updating status text
    char *statustext = "i m busy";
    memcpy( smsg + index, statustext, strlen( statustext ) );
    index += strlen( statustext ); 
    
       
    
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EUpdateOwnPresence);
   
    if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    	 //-------------update presence 2nd time---------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EUpdateOwnPresence;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // updating availability
    char *availability1 = "away";
    memcpy( smsg + index, availability1, strlen( availability1 ) );
    index += strlen( availability1 ); 
    smsg[index++]='\0';
    
   //updating status text
    char *statustext1 = "i m not on desk";
    memcpy( smsg + index, statustext1, strlen( statustext1 ) );
    index += strlen( statustext1 ); 
    
       
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EUpdateOwnPresence);
   
    if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    

   // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ELogout_Request;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
    if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }
// -----------------------------------------------------------------------------
// Cisolationserver::UpdatePresenceWithDnd
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::UpdateOnlyStatusText( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    FILE *fp;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;

    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		

	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    
 
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	
	 //-------------update presence---------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EUpdateOwnPresence;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // updating availability with blank 
    smsg[index++]='\0';
    smsg[index++]='\0';
    //smsg[index++]='\0';
    
    //updating status text
    char *statustext = "i m not on desk";
    memcpy( smsg + index, statustext, strlen( statustext ) );
    index += strlen( statustext );  
    
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EUpdateOwnPresence);
   
    if ( msg_struct->error_type > 0)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

 // -----------------------------------------------------------------------------
// Cisolationserver::UpdatePresenceWithAway
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::UpdateOnlyUserAvailability( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    FILE *fp;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;

    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		

	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    
 
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
   //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	
	 //-------------update presence---------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EUpdateOwnPresence;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // updating availability
    char *availability = "available";
    memcpy( smsg + index, availability, strlen( availability ) );
    index += strlen( availability ); 
      
       
    
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EUpdateOwnPresence);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
   // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::UpdatePresenceWithAway
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::UpdateOwnPresenceWithBlankData( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    FILE *fp;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;

    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		

	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    
 
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
   //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	
	 //-------------update presence---------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EUpdateOwnPresence;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	   
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EUpdateOwnPresence);
   
    if ( msg_struct->error_type > 0)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
   // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }
// -----------------------------------------------------------------------------
// Cisolationserver::UpdatePresenceWithDnd
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::UpdateStatusTextWithSpecialCharacter( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    FILE *fp;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;

    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		

	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    
 
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	
	 //-------------update presence---------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EUpdateOwnPresence;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // updating availability
    char *availability = "dnd";
    memcpy( smsg + index, availability, strlen( availability ) );
    index += strlen( availability ); 
    smsg[index++]='\0';
    
    //updating status text
    char *statustext = "hi i m_ fine here @@@@@ ********* &&&&&&&&&&&&";
    memcpy( smsg + index, statustext, strlen( statustext ) );
    index += strlen( statustext ); 
    
       
    
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EUpdateOwnPresence);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::UpdatePresenceWithDnd
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::UpdateStatusTextWithMaxLen( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    FILE *fp;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;

    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		

	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    
 
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	
	 //-------------update presence---------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EUpdateOwnPresence;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // updating availability
    char *availability = "dnd";
    memcpy( smsg + index, availability, strlen( availability ) );
    index += strlen( availability ); 
    smsg[index++]='\0';
    
    //updating status text
    char *statustext = "thesr is difference between thoughts and execution ";
    memcpy( smsg + index, statustext, strlen( statustext ) );
    index += strlen( statustext ); 
    
       
    
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EUpdateOwnPresence);
   
    if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::UpdatePresenceWithDnd
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::HandelingPresenceNotification( CStifItemParser& aItem )
    {

    /*	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    FILE *fp;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;

    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver();    
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		

	// Send Message to queueOne 
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    
 
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	
	 //-------------update presence---------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EUpdateOwnPresence;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // updating availability
    char *availability = "dnd";
    memcpy( smsg + index, availability, strlen( availability ) );
    index += strlen( availability ); 
    smsg[index++]='\0';
    
    //updating status text
    char *statustext = "i m busy";
    memcpy( smsg + index, statustext, strlen( statustext ) );
    index += strlen( statustext ); 
    
       
    
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EUpdateOwnPresence);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
   //  waiting for second change 
    memset( rmsg, '\0', MAX_MSG_SIZE );
        //wait for message receive 
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    //  waiting for first change 
    memset( rmsg, '\0', MAX_MSG_SIZE );
        //wait for message receive 
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    
     //  waiting for first change 
    memset( rmsg, '\0', MAX_MSG_SIZE );
        //wait for message receive 
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    
    
     //  waiting for first change 
    memset( rmsg, '\0', MAX_MSG_SIZE );
        //wait for message receive 
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    
    
     //  waiting for first change 
    memset( rmsg, '\0', MAX_MSG_SIZE );
        //wait for message receive 
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	    goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value; */
    return KErrNone;
    }
    
    
Cisolationserver::SearchKeyValue( CStifItemParser& aItem )
	{
	ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    TInt return_value = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
      
    	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return KErrGeneral;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '0';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    run_isoserver();
    
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;	
    goto return_code;
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //search messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESearch;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
	const gchar* search_label = GetSearchLabelL(connect_data->server,connect_data->key_given_name);
    // adding key
    len = strlen(search_label);
    strcpy( smsg + index, search_label );
    index += len + 1;
    

    
    // adding value
     
    len = strlen("santhosh" );
    strcpy( smsg + index, "santhosh" );
    index += len + 1;
    

    smsg[index++] = '\0';
     
    
    
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!= ESearch_State_Finished );
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }    
     ///-------------------------------------------------------------
 
    
    //search message end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }

    
   // result = MsgQDelete(REQUEST_QUEUE, &err);
   // result = MsgQDelete(RESPONSE_QUEUE, &err);
    return_value = KErrNone;	
    goto return_code;
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;	
	}

Cisolationserver::SearchKeyValueWithResult( CStifItemParser& aItem )
	{
	ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;
	TInt return_value = 0;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
      
    	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;	
    	goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '0';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    run_isoserver();
    
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //search messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESearch;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
	const gchar* search_label = GetSearchLabelL(connect_data->server,connect_data->key_given_name);
    // adding key
    len = strlen(search_label);
    strcpy( smsg + index, search_label );
    index += len + 1;	
    
    // adding value
     
    len = strlen("rakesh" );
    strcpy( smsg + index, "rakesh" );
    index += len + 1;
    
    smsg[index++] = '\0';
     
    
    
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!= ESearch_State_Finished );
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }    
    
    
    
    //search message end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }

    
   // result = MsgQDelete(REQUEST_QUEUE, &err);
   // result = MsgQDelete(RESPONSE_QUEUE, &err);
    return_value = KErrNone;	
    goto return_code;
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;	
	}



Cisolationserver::SearchKeyValueInvalidParam( CStifItemParser& aItem )
	{
	ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;
	TInt return_value = 0;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
      
    	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;	
    	goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '0';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    run_isoserver();
    
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;	
    	goto return_code;
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //search messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESearch;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	const gchar* search_label = GetSearchLabelL(connect_data->server,connect_data->key_given_name);
    // adding key
    len = strlen(search_label);
    strcpy( smsg + index, search_label );
    index += len + 1;
    
    smsg[index++] = '\0';
     
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!= ESearch );
   
     if ( msg_struct->response != 0)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }    
    
    
    
    //search message end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
     
   // result = MsgQDelete(REQUEST_QUEUE, &err);
   // result = MsgQDelete(RESPONSE_QUEUE, &err);
    return_value = KErrNone;	
    goto return_code;
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;

	}


Cisolationserver::SearchForLargeNoOfResult( CStifItemParser& aItem )
	{
	ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;
	TInt return_value = 0;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
      
    	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;	
    	goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '0';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    run_isoserver();
    
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;	
    	goto return_code;
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //search messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESearch;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
   	const gchar* search_label = GetSearchLabelL(connect_data->server,connect_data->key_given_name);
    // adding key
    len = strlen(search_label);
    strcpy( smsg + index, search_label );
    index += len + 1;
    

    
    // adding value
     
    len = strlen("a*" );
    strcpy( smsg + index, "a*" );
    index += len + 1;
    
    smsg[index++] = '\0';
     
    
    
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!= ESearch_State_Finished );
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }    
    
    
    
    //search message end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }

    
   // result = MsgQDelete(REQUEST_QUEUE, &err);
   // result = MsgQDelete(RESPONSE_QUEUE, &err);
    return_value = KErrNone;	
    goto return_code;
    
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;	
	}

Cisolationserver::SearchKeyValueInvalidParam2( CStifItemParser& aItem )
	{
	ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;
	TInt return_value = 0;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
      
    	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;	
    	goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '0';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    run_isoserver();
    
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;	
    goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //search messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESearch;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
     
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!= ESearch );
   
     if ( msg_struct->response != 0 )
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }    
    
    
    
    //search message end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
     
   // result = MsgQDelete(REQUEST_QUEUE, &err);
   // result = MsgQDelete(RESPONSE_QUEUE, &err);
    return_value = KErrNone;	
    goto return_code;
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;

	}
Cisolationserver::SearchKeyValueInvalidHashValues( CStifItemParser& aItem )
	{
	ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;
	TInt return_value = 0;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
      
    	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;	
    	goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '0';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    run_isoserver();
    
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;	
    	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //search messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESearch;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding key
    len = strlen( "firstGname" );
    strcpy( smsg + index, "firstGname" );
    index += len + 1;
        
    // adding value
    len = strlen("I" );
    strcpy( smsg + index, "I" );
    index += len + 1;
    
    smsg[index++] = '\0';
     
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!= ESearch );
   
     if ( msg_struct->response != 0)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }    
    
    
    
    //search message end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
     
   // result = MsgQDelete(REQUEST_QUEUE, &err);
   // result = MsgQDelete(RESPONSE_QUEUE, &err);
    return_value = KErrNone;	
    goto return_code;
    
//killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
	}

Cisolationserver::SearchKeyValueBlank( CStifItemParser& aItem )
	{
	ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;
	TInt return_value = 0;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
      
    	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;	
    	goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '0';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    run_isoserver();
    
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;	
    	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //search messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESearch;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding key
    len = strlen( connect_data->key_jid );
    strcpy( smsg + index, connect_data->key_jid );
    index += len + 1;
        
    // adding value
    len = strlen("" );
    strcpy( smsg + index, "" );
    index += len + 1;
    
    smsg[index++] = '\0';
     
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!= ESearch );
   
   
   if( msg_struct->error_type != INVALID_PARAMETERES )
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    
     
    
    
    //search message end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
     
   // result = MsgQDelete(REQUEST_QUEUE, &err);
   // result = MsgQDelete(RESPONSE_QUEUE, &err);
    return_value = KErrNone;	
    goto return_code;
    
//killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
	}

Cisolationserver::SearchKeyValueLargeString( CStifItemParser& aItem )
	{
	ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;
	TInt return_value = 0;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
      
    	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;	
    	goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '0';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    run_isoserver();
    
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;	
    	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //search messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESearch;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    const gchar* search_label = GetSearchLabelL(connect_data->server,connect_data->key_given_name);
    // adding key
    len = strlen(search_label);
    strcpy( smsg + index, search_label );
    index += len + 1;
        
    // adding value
    len = strlen( "Gmail is a new kind of webmail built on \
    the idea that email can be more intuitive efficient and useful" );
    
    strcpy( smsg + index, "Gmail is a new kind of webmail built on \
    the idea that email can be more intuitive efficient and useful" );
    index += len + 1;
    
    smsg[index++] = '\0';
     
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!= ESearch_State_Finished );
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }    
    
    
    
    //search message end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
     
   // result = MsgQDelete(REQUEST_QUEUE, &err);
   // result = MsgQDelete(RESPONSE_QUEUE, &err);
    return_value = KErrNone;	
    goto return_code;
    
//killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
	}

Cisolationserver::SearchKeyValueSpecialChar( CStifItemParser& aItem )
	{
	ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;
	TInt return_value = 0;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
      
    	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;	
    	goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '0';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    run_isoserver();
    
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;	
    	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //search messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESearch;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    const gchar* search_label = GetSearchLabelL(connect_data->server,connect_data->key_given_name);
    // adding key
    len = strlen(search_label);
    strcpy( smsg + index, search_label );
    index += len + 1;
        
    // adding value
    len = strlen("happy_123@chat.gizmoproject.com" );
    strcpy( smsg + index, "happy_123@chat.gizmoproject.com" );
    index += len + 1;
    
    smsg[index++] = '\0';
     
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!= ESearch_State_Finished );
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }    
    
    
    
    //search message end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
     
   // result = MsgQDelete(REQUEST_QUEUE, &err);
   // result = MsgQDelete(RESPONSE_QUEUE, &err);
    return_value = KErrNone;	
    goto return_code;
    
//killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
	}

    // -----------------------------------------------------------------------------
// Cisolationserver::RejectAddRequestFromUser
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//RejectAddRequestFromUser
TInt Cisolationserver::RejectAddRequestFromUser( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    FILE *fp;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;

    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		

	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    
 
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	//fetch end
	// getting add notification from user
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    
	do  {
    	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	  //  memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ETrying_To_Add_Contact_Publish);
   
    if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    char *userid = (char *)malloc(100);
    len=0;
    len+=sizeof(message_hdr_resp);
    strcpy(userid,rmsg+len);
    
    
    

    // rejecting add request from user 
    
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EReject_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	len=0;
    len+=strlen(userid);
	strcpy( smsg + index ,userid   );
    index += len + 1;
	
	
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EReject_Contact_Request);
   
    if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	 result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

     // -----------------------------------------------------------------------------
// Cisolationserver::AcceptAddRequestFromUser
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
Cisolationserver::SearchAndAddContact( CStifItemParser& aItem )
	{
	
	ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;
	TInt return_value = 0;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
      
    	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;	
    	goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '0';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    run_isoserver();
    
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;	
    	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //search messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESearch;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    const gchar* search_label = GetSearchLabelL(connect_data->server,connect_data->key_given_name);
    // adding key
    len = strlen(search_label);
    strcpy( smsg + index, search_label );
    index += len + 1;
        
    // adding value
    len = strlen("rakesh" );
    strcpy( smsg + index, "rakesh" );
    index += len + 1;
    
    smsg[index++] = '\0';
    
    gint temp = 0;
    gchar* add_contact = NULL;
     
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  
     	{
		memset( rmsg, '\0', MAX_MSG_SIZE );
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    //To add contact only once for the first time
	    if ( 0 == temp && ESearch == msg_struct->hdr_req.message_type )
	    	{
	        // length will point to the start of the contact to be added
		    int length = 0;
		    length += sizeof( message_hdr_resp );
		    
		    while ( 0 != strcmp( "jid", rmsg + length ) 
		    	&& '\0' != *( rmsg + length + 1) )
			    {
			    length += strlen( rmsg + length ) + 1;
			    gchar* add_contact = rmsg + length;
			    }
		    length += strlen( rmsg + length ) + 1;
		    
		    add_contact = strdup( rmsg + length );
		    //add contact end---------------------------------------   	
	   		temp++;
	   	 }
	    
	    
	 	} while (msg_struct->hdr_req.message_type!= ESearch_State_Finished );
   
     if ( msg_struct->response != 1 )
    	{
    	return_value = KErrGeneral;	
    	goto return_code;
   		}    
   		
    //search message end---------------------------------------
 
  //adding contact --------------------------------------------
		   
    memset( smsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EAdd_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	// adding add contact
    len = strlen( add_contact );
    strcpy( smsg + index, add_contact );
    index += len + 1;
    smsg[index++]='\0';
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		
		memset( rmsg, '\0', MAX_MSG_SIZE );
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    
		} while (msg_struct->hdr_req.message_type!=EAdd_Contact_Request);
   
     if ( msg_struct->response != 1 )
    {
    	return_value = KErrGeneral;
		goto return_code;
    }
 
 
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
     
   // result = MsgQDelete(REQUEST_QUEUE, &err);
   // result = MsgQDelete(RESPONSE_QUEUE, &err);
    return_value = KErrNone;	
    goto return_code;
    
//killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;		
	}

// -----------------------------------------------------------------------------
// Cisolationserver::AcceptAddRequestFromUser
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
Cisolationserver::SecondSearch( CStifItemParser& aItem )
	{
	
	ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;
	TInt return_value = 0;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
      
    	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;	
    	goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '0';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    run_isoserver();
    
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;	
    	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
     
    //search messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESearch;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	const gchar* search_label = GetSearchLabelL(connect_data->server,connect_data->key_given_name);
    // adding key
    len = strlen(search_label);
    strcpy( smsg + index, search_label );
    index += len + 1;
        
    // adding value
    len = strlen("sharath" );
    strcpy( smsg + index, "sharath" );
    index += len + 1;
    
    smsg[index++] = '\0';
    
    gint temp = 0;
    gchar* add_contact = NULL;
     
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  
     	{
		memset( rmsg, '\0', MAX_MSG_SIZE );
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );		    
	    
	 	} while (msg_struct->hdr_req.message_type!= ESearch_State_Finished );
   
     if ( msg_struct->response != 1 )
    	{
    	return_value = KErrGeneral;	
    	goto return_code;
   		}    
   		
    //search message end---------------------------------------
 
 
     //second search messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESearch;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	

    
    const gchar* search_label1 = GetSearchLabelL(connect_data->server,connect_data->key_middle_name);
    // adding key
    len = strlen(search_label1);
    strcpy( smsg + index, search_label1 );
    index += len + 1;
        
    // adding value
    len = strlen("Jeppu" );
    strcpy( smsg + index, "Jeppu" );
    index += len + 1;
    
    smsg[index++] = '\0';
    

     
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  
     	{
		memset( rmsg, '\0', MAX_MSG_SIZE );
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );		    
	 	} while (msg_struct->hdr_req.message_type!= ESearch_State_Finished );
   
     if ( msg_struct->response != 1 )
    	{
    	return_value = KErrGeneral;	
    	goto return_code;
   		}    
   	
    //search message end---------------------------------------
 
 
 
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
     
   // result = MsgQDelete(REQUEST_QUEUE, &err);
   // result = MsgQDelete(RESPONSE_QUEUE, &err);
    return_value = KErrNone;	
    goto return_code;
    
//killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;		
	}


TInt Cisolationserver::AcceptAddRequestFromUser( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    FILE *fp;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;

    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		

	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    
 
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	//fetch end
	// getting add notification from user
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    
	do  {
    	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	  //  memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ETrying_To_Add_Contact_Publish);
   
    if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    char *userid = (char *)malloc(100);
    len=0;
    len+=sizeof(message_hdr_resp);
    strcpy(userid,rmsg+len);
     // accepting add request from user 
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EReject_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	len=0;
    len+=strlen(userid);
	strcpy( smsg + index ,userid   );
    index += len + 1;
	
	
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EAccept_Contact_Request);
   
    if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	 result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

 
	  // -----------------------------------------------------------------------------
// Cisolationserver::SendAcceptRequestWithoutReceivingAddrequest
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::SendAcceptRequestWithoutReceivingAddrequest( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    
    //running isoserver   
    run_isoserver(); 
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
   //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
    //Accepting Add request from  contact without ne add notification --------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EAccept_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	// adding add contact
    len = strlen(connect_data->contact );
    strcpy( smsg + index, connect_data->contact );
    index += len + 1;
    smsg[index++] = '\0';
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EAccept_Contact_Request);
   
     if ( msg_struct->response != 0)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //add contact end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

	  // -----------------------------------------------------------------------------
// Cisolationserver::SendRejectRequestWithoutReceivingAddrequest
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::SendRejectRequestWithoutReceivingAddrequest( CStifItemParser& aItem )
    {

    	
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    
    //running isoserver   
    run_isoserver(); 
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 0)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
   //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
    //Accepting Add request from  contact without ne add notification --------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EReject_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	// adding add contact
    len = strlen(connect_data->contact );
    strcpy( smsg + index, connect_data->contact );
    index += len + 1;
    smsg[index++] = '\0';
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EReject_Contact_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    //add contact end---------------------------------------
    
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }


	  // -----------------------------------------------------------------------------
// Cisolationserver::SendAcceptRequestWithoutAnyUserID
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::SendAcceptRequestWithoutAnyUserID( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    FILE *fp;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;

    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		

	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    
 
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	//fetch end
	// getting add notification from user
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    
	do  {
    	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	  //  memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ETrying_To_Add_Contact_Publish);
   
    if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    char *userid = (char *)malloc(100);
    len=0;
    len+=sizeof(message_hdr_resp);
    strcpy(userid,rmsg+len);
     // accepting add request from user 
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EReject_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	//no need to send user id with this message
	//len=0;
    //len+=strlen(userid);
	//strcpy( smsg + index ,userid   );
    //index += len + 1;
	
	
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EAccept_Contact_Request);
   // need to check response
    if ( msg_struct->response != 0)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	 result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

    // -----------------------------------------------------------------------------
// Cisolationserver::SendRejectRequestWithoutAnyUserID
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//RejectAddRequestFromUser
TInt Cisolationserver::SendRejectRequestWithoutAnyUserID( CStifItemParser& aItem )
    {

    	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    
		
	int timeout = 100;
	int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    FILE *fp;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;

    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		

	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
	goto return_code;	
    }
    
 
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
	//fetch end
	// getting add notification from user
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    
	do  {
    	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	  //  memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ETrying_To_Add_Contact_Publish);
   
    if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    char *userid = (char *)malloc(100);
    len=0;
    len+=sizeof(message_hdr_resp);
    strcpy(userid,rmsg+len);
    
    
    

    // rejecting add request from user 
    
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EReject_Contact_Request;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	//len=0;
    //len+=strlen(userid);
	//strcpy( smsg + index ,userid   );
    //index += len + 1;
	
	
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=EReject_Contact_Request);
   
    if ( msg_struct->response != 0)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	 result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
    if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;
	goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

    // -----------------------------------------------------------------------------
// Cisolationserver::Login with correct data
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::LogoutWhileUserIsNotLoggedIn( CStifItemParser& aItem )
    {
  	
    ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;

	FILE *fp;	
	int timeout = 100;
	int nBytes;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
        	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;
		goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ELogin_Request;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    //appending password
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    //appending server name
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    // appending IAP id
    smsg[index++]= '1';
    smsg[index++]= '\0';
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    #endif
     //running isoserver   
    run_isoserver(); 
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    
	//fetch end
    
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	 result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
    if ( msg_struct->response != 1)
    {
    return_value = KErrGeneral;
	goto return_code;
    }
    return_value = KErrNone;
	goto return_code;
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;

    }


// -----------------------------------------------------------------------------
// Cisolationserver::GetSearchLabelForYukonL
// -----------------------------------------------------------------------------
const gchar* Cisolationserver::GetSearchLabelForYukonL(gchar *search_key)
{
if(strcmp(search_key,"user") == 0)
	{
	gchar* label= "User";
	return label;	
	}
else if(strcmp(search_key,"fn") == 0)
	{
	gchar* label= "Full Name";
	return label;	
	}	
else if(strcmp(search_key,"first") == 0)
	{
	gchar* label= "Name";
	return label;	
	}	
else if(strcmp(search_key,"middle") == 0)
	{
	gchar* label= "Middle Name";
	return label;	
	}		
else if(strcmp(search_key,"last") == 0)
	{
	gchar* label= "Family Name";
	return label;	
	}
else if(strcmp(search_key,"nick") == 0)
	{
	gchar* label= "Nickname";
	return label;	
	}
else if(strcmp(search_key,"email") == 0)
	{
	gchar* label= "Email";
	return label;	
	}	
else if(strcmp(search_key,"bday") == 0)
	{
	gchar* label= "Birthday";
	return label;	
	}	
else if(strcmp(search_key,"ctry") == 0)
	{
	gchar* label= "Country";
	return label;	
	}		
else if(strcmp(search_key,"orgname") == 0)
	{
	gchar* label= "Organization Name";
	return label;	
	}
else if(strcmp(search_key,"locality") == 0)
	{
	gchar* label= "City";
	return label;	
	}
else if(strcmp(search_key,"orgunit") == 0)
	{
	gchar* label= "Organization Unit";
	return label;	
	}			
}


// -----------------------------------------------------------------------------
// Cisolationserver::GetSearchLabelForGizmoL
// -----------------------------------------------------------------------------
const gchar* Cisolationserver::GetSearchLabelForGizmoL(gchar *search_key)
{
if(strcmp(search_key,"xmpp_user") == 0)
	{
	gchar* label= "Account Name";
	return label;	
	}
else if(strcmp(search_key,"email_address") == 0)
	{
	gchar* label= "Email Address";
	return label;	
	}	
else if(strcmp(search_key,"first_name") == 0)
	{
	gchar* label= "First Name";
	return label;	
	}	
else if(strcmp(search_key,"last_name") == 0)
	{
	gchar* label= "Family Name";
	return label;	
	}
else if(strcmp(search_key,"country") == 0)
	{
	gchar* label= "Country";
	return label;	
	}		
else if(strcmp(search_key,"state") == 0)
	{
	gchar* label= "State/Province";
	return label;	
	}
else if(strcmp(search_key,"city") == 0)
	{
	gchar* label= "City";
	return label;	
	}
else if(strcmp(search_key,"max_results") == 0)
	{
	gchar* label= "Maximum # of Results";
	return label;	
	}			
}

// -----------------------------------------------------------------------------
// Cisolationserver::GetSearchLabelL
// -----------------------------------------------------------------------------
const gchar* Cisolationserver::GetSearchLabelL(gchar* aService,gchar* aSearchKey)
{
if(strcmp(aService,"chat.gizmoproject.com") == 0)
	{
	GetSearchLabelForGizmoL(aSearchKey);	
	}
else if(strcmp(aService,"chat.ovi.com") == 0 )
	{
	GetSearchLabelForYukonL(aSearchKey);	
	}
}
	
  
// -----------------------------------------------------------------------------
// Cisolationserver::?member_function
// ?implementation_description
// (other items were commented in a header).

// -----------------------------------------------------------------------------
// Cisolationserver::AcceptAddRequestFromUser
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
Cisolationserver::GetKeysAndSearch( CStifItemParser& aItem )
	{
	
	ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
		
	int timeout = 200;
	int nBytes;
	TInt return_value = 0;
    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
      
    	// Read data from the CFG file
	if ( read_data ( connect_data, aItem ) != RC_OK )
		{
		iLog->Log ( _L ( "read_data failed" ) );
		free_data ( connect_data );
		return_value = KErrGeneral;	
    	goto return_code;
		}
	//request formation	
	memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	//message header use
	msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '0';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    run_isoserver();
    
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
		
	
	/* Send Message to queueOne */
	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;	
    	goto return_code;	
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
	
	if( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
    
    
    // login end
    //fetch req..
    for (int i= 0 ;i<3 ;i++)
    {
        
    fp = fopen("c:\\fetch_contact.txt","a");
	do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
		fwrite(rmsg,1,status,fp);
		memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
		          ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
	fclose(fp);	
	
    }

	//fetch end
 
 
 
    // get search keys test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ESearch_Get_Keys;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		memset( rmsg, '\0', MAX_MSG_SIZE );
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    
		} while (msg_struct->hdr_req.message_type!= ESearch_Get_Keys );
   
     if ( msg_struct->response != 1)
    	{
    	return_value = KErrGeneral;	
    	goto return_code;
    	}
    	
    int length = sizeof( message_hdr_resp );
    char* search_value = NULL;
    do  {
		search_value = strdup ( rmsg + length );
	    	
		if ( !search_value )
			{
			return_value = KErrGeneral;	
	    	goto return_code;
			}  
		length += strlen( search_value ) + 1;
	    } while ( ( 0 != strcmp( "First Name", search_value ) )
		    && length < status &&( free ( search_value ), search_value = NULL, 1 ) );
    
 
 
     
    //search messge--------------------------------------------
   
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = ESearch;	
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
	index += sizeof( message_hdr_req );
	
	
    // adding key
    len = strlen( search_value );
    strcpy( smsg + index, search_value );
    index += len + 1;
        
    // adding value
    len = strlen( "santhosh" );
    strcpy( smsg + index, "santhosh" );
    index += len + 1;
    
    smsg[index++] = '\0';
    
    gint temp = 0;
    gchar* add_contact = NULL;
     
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  
     	{
		memset( rmsg, '\0', MAX_MSG_SIZE );
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );		    
	    
	 	} while (msg_struct->hdr_req.message_type!= ESearch_State_Finished );
   
     if ( msg_struct->response != 1 )
    	{
    	return_value = KErrGeneral;	
    	goto return_code;
   		}    
   		
    //search message end---------------------------------------
 
 
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

	
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
		status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
		memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );	
	    memset( rmsg, '\0', MAX_MSG_SIZE );
		} while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
    	return_value = KErrGeneral;	
    	goto return_code;
    }
     
   // result = MsgQDelete(REQUEST_QUEUE, &err);
   // result = MsgQDelete(RESPONSE_QUEUE, &err);
    return_value = KErrNone;	
    goto return_code;
    
//killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
	memset( rmsg, '\0', MAX_MSG_SIZE );
	memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
	result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;		
	}

// -----------------------------------------------------------------------------
// Cisolationserver::set avatar
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::SetOwnAvatar( CStifItemParser& aItem )
    {

        
   ConnectData  *connect_data = NULL;
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    FILE *fp;
        
    int timeout = 200;
    int nBytes;

    int result = 0;
    int err = 0;
    int status = 0;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
            // Read data from the CFG file
    if ( read_data ( connect_data, aItem ) != RC_OK )
        {
        iLog->Log ( _L ( "read_data failed" ) );
        free_data ( connect_data );
        return_value = KErrGeneral;
        goto return_code;
        }
    //request formation 
    memset( smsg, '\0', MAX_MSG_SIZE );
    memset( rmsg, '\0', MAX_MSG_SIZE );
    
    //message header use
    msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogin_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    
    int len = strlen(connect_data->username);
    strcpy( smsg + index, connect_data->username );
    index += len + 1;
    
    //appending password
    
    len = strlen(connect_data->password );
    strcpy( smsg + index, connect_data->password );
    index += len + 1;
    
    //appending server name
    
    len = strlen(connect_data->server );
    strcpy( smsg + index, connect_data->server );
    index += len + 1;
    
    //appending resource
    len = strlen(connect_data->resource );
    strcpy( smsg + index, connect_data->resource );
    index += len + 1;
    
    //appending ssl
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending server port
    len = strlen(connect_data->port );
    strcpy( smsg + index, connect_data->port );
    index += len + 1;
    
    // appending IAP id
    
    smsg[index++]= '1';
    smsg[index++]= '\0';
    
    //appending connmgr_bus
    len = strlen(connect_data->connmgr_bus );
    strcpy( smsg + index, connect_data->connmgr_bus );
    index += len + 1;
    
    //appending connmgr_path
    len = strlen(connect_data->connmgr_path );
    strcpy( smsg + index, connect_data->connmgr_path );
    index += len + 1;
    
    //appending protocol
    len = strlen(connect_data->protocol );
    strcpy( smsg + index, connect_data->protocol );
    index += len + 1;
    
    #ifdef __WINSCW__
    //proxy server
    len = strlen(connect_data->proxy_data->proxy_server );
    strcpy( smsg + index, connect_data->proxy_data->proxy_server );
    index += len + 1;
    
    //proxy port
    len = strlen(connect_data->proxy_data->proxy_port );
    strcpy( smsg + index, connect_data->proxy_data->proxy_port );
    index += len + 1;
    
    #endif
    
    //running isoserver   
    run_isoserver(); 
    
        
    //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    //create message queue for response request
    result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
        
    
    /* Send Message to queueOne */
    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
    
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
    
    if (status < 0)
    {
    return_value = KErrGeneral;
    goto return_code;   
    }
    msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
    memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
    memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
    
    if( msg_struct->response != 1)
    {
        return_value = KErrGeneral;
    goto return_code;
    }
    
    
    // login end

    //fetch req..
       for (int i= 0 ;i<3 ;i++)
       {
           
       fp = fopen("c:\\fetch_contact.txt","a");
       do  {
           status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
           memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) ); 
           fwrite(rmsg,1,status,fp);
           memset( rmsg, '\0', MAX_MSG_SIZE );
           } while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
                     ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
                     ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
       fclose(fp); 
       
       }

       //fetch end
    //set avatar
    TPtrC               filename;
    TPtrC               mimetype;
    gchar        *file_name;
    gchar        *mime_type;
    gchar        *filecontent; 
    int n;
    TBuf8<KMaxFileName> temp;
   
   index = 0;
    index += sizeof( message_hdr_req );
    // setting avatar
    FILE *file;
    char *buffer;
    unsigned long fileLen;

    //Open file
    file = fopen(connect_data->image_file_name, "rb");
    if (!file)
        {
        fprintf(stderr, "Unable to open file %s", file_name);
        return;
        }

    //Get file length
    fseek(file, 0, SEEK_END);
    fileLen=ftell(file);
    fseek(file, 0, SEEK_SET);

   
    //strcpy(mime_type,"image\jpeg");
    len = fileLen + strlen(connect_data->mimetype) + 1;
    buffer=(char *)malloc(len);
    if (!buffer)
        {
        fprintf(stderr, "Memory error!");
        fclose(file);
        return;
        }
    memset( buffer, '\0', len );
	memcpy( buffer, connect_data->mimetype, strlen(connect_data->mimetype) );
    //Read file contents into buffer
    
    fread(buffer + strlen(connect_data->mimetype) + 1, fileLen, 1, file);
    fclose(file);

    //Do what ever with buffer
    memset( smsg, '\0', MAX_MSG_SIZE );
    memset( rmsg, '\0', MAX_MSG_SIZE );
    index=0;
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EUpdateOwnAvtar;    
    // memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    /* Send Message to queueOne */
    TInt header_size = sizeof( message_hdr_req );
    //crashes for perticuler image.Need to fix
    char* one_pdu = ( char* ) malloc( ONE_K_OPENC_FIX );
    //run a loop for rest of the data and keep track of the case of nBytes being 
    //multiple of 1020 - sizeof(msghdr) and it being not
    //Amount of payload that can be sent
    TInt single_read = ONE_K_OPENC_FIX - header_size;
    //Data recieved from PublishOwnPresenceL has message header at begining
    TInt actual_data = fileLen;
    //No of packets needed to send the image data
    TInt total_packets = (TInt)ceil( ( double ) ( actual_data ) / single_read );
    //This offset moves thru the msg pointer
    TInt navigate_offset = 0;
    for ( TInt i = 0; i < total_packets; i++ )
        {
        //till the last packet size_tobe_sent will be ONE_K_OPENC_FIX
        //last packet data is actual_data % single_read if less than single_read else ONE_K_OPENC_FIX
        TInt size_tobe_sent = ( i < total_packets - 1 ) ? ONE_K_OPENC_FIX : 
        ( actual_data % single_read ? 
            actual_data % single_read  + header_size : ONE_K_OPENC_FIX );

        if ( i < total_packets - 1 )
            {
            msgHdr->continue_flag = 1;
            }
        else 
            {
            msgHdr->continue_flag = 0;
            }

        //memcpy ( msg + header_size , msg + navigate_offset, size_tobe_sent - header_size );
        memcpy( one_pdu , msgHdr, header_size );
        memcpy( one_pdu + header_size, buffer + navigate_offset, size_tobe_sent - header_size );
        result = MsgQSend ( REQUEST_QUEUE, one_pdu, size_tobe_sent, 
                MSG_PRI_NORMAL, timeout, &err );
        User::LeaveIfError ( result );
        navigate_offset += single_read;
        }
    free(one_pdu);
    free(buffer);
    do  {
    status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
    memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) ); 
    memset( rmsg, '\0', MAX_MSG_SIZE );
    } while (msg_struct->hdr_req.message_type!=EUpdateOwnAvtar);

    if ( msg_struct->response != 1)
        {
        return_value = KErrGeneral;
        goto return_code;
        }

    //set avatar end     
    // logout test code from server
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
    memset( rmsg, '\0', MAX_MSG_SIZE );
    
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    
    msgHdr->message_type = ELogout_Request;
 
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );

    
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     do  {
        status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
        memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) ); 
        memset( rmsg, '\0', MAX_MSG_SIZE );
        } while (msg_struct->hdr_req.message_type!=ELogout_Request);
   
     if ( msg_struct->response != 1)
    {
        return_value = KErrGeneral;
    goto return_code;
    }
    //killing isoserver
    return_code:
    index=0;
    memset( smsg, '\0', MAX_MSG_SIZE );
    memset( rmsg, '\0', MAX_MSG_SIZE );
    memset( msgHdr, '\0', sizeof( message_hdr_req ) );
    msgHdr->message_type = EKill_Process;
    memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
    index += sizeof( message_hdr_req );
    result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
    free( msg_struct );
    return return_value;
    }

// -----------------------------------------------------------------------------
// Cisolationserver::clear avatar
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::ClearOwnAvatar( CStifItemParser& aItem )
    {

    
    ConnectData  *connect_data = NULL;
     char smsg[MAX_MSG_SIZE];
     char rmsg[MAX_MSG_SIZE];
     int index=0;
     FILE *fp;
         
     int timeout = 200;
     int nBytes;

     int result = 0;
     int err = 0;
     int status = 0;
     message_hdr_req *msgHdr = NULL;  
     message_hdr_resp* msg_struct = NULL;
     TInt return_value = 0;
             // Read data from the CFG file
     if ( read_data ( connect_data, aItem ) != RC_OK )
         {
         iLog->Log ( _L ( "read_data failed" ) );
         free_data ( connect_data );
         return_value = KErrGeneral;
         goto return_code;
         }
     //request formation 
     memset( smsg, '\0', MAX_MSG_SIZE );
     memset( rmsg, '\0', MAX_MSG_SIZE );
     
     //message header use
     msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
     memset( msgHdr, '\0', sizeof( message_hdr_req ) );
     
     msgHdr->message_type = ELogin_Request;
  
     memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
     index += sizeof( message_hdr_req );
     
     int len = strlen(connect_data->username);
     strcpy( smsg + index, connect_data->username );
     index += len + 1;
     
     //appending password
     
     len = strlen(connect_data->password );
     strcpy( smsg + index, connect_data->password );
     index += len + 1;
     
     //appending server name
     
     len = strlen(connect_data->server );
     strcpy( smsg + index, connect_data->server );
     index += len + 1;
     
     //appending resource
     len = strlen(connect_data->resource );
     strcpy( smsg + index, connect_data->resource );
     index += len + 1;
     
     //appending ssl
     smsg[index++]= '1';
     smsg[index++]= '\0';
     
     //appending server port
     len = strlen(connect_data->port );
     strcpy( smsg + index, connect_data->port );
     index += len + 1;
     
     // appending IAP id
     
     smsg[index++]= '1';
     smsg[index++]= '\0';
     
     //appending connmgr_bus
     len = strlen(connect_data->connmgr_bus );
     strcpy( smsg + index, connect_data->connmgr_bus );
     index += len + 1;
     
     //appending connmgr_path
     len = strlen(connect_data->connmgr_path );
     strcpy( smsg + index, connect_data->connmgr_path );
     index += len + 1;
     
     //appending protocol
     len = strlen(connect_data->protocol );
     strcpy( smsg + index, connect_data->protocol );
     index += len + 1;
     
     #ifdef __WINSCW__
     //proxy server
     len = strlen(connect_data->proxy_data->proxy_server );
     strcpy( smsg + index, connect_data->proxy_data->proxy_server );
     index += len + 1;
     
     //proxy port
     len = strlen(connect_data->proxy_data->proxy_port );
     strcpy( smsg + index, connect_data->proxy_data->proxy_port );
     index += len + 1;
     
     #endif
     
     //running isoserver   
     run_isoserver(); 
     
         
     //create message queue for send request
     result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
     //create message queue for response request
     result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
         
     
     /* Send Message to queueOne */
     
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     
     status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
     
     if (status < 0)
     {
     return_value = KErrGeneral;
     goto return_code;   
     }
     msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
     memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
     memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
     
     if( msg_struct->response != 1)
     {
         return_value = KErrGeneral;
     goto return_code;
     }
     
     
     // login end

     //fetch req..
        for (int i= 0 ;i<3 ;i++)
        {
            
        fp = fopen("c:\\fetch_contact.txt","a");
        do  {
            status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
            memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) ); 
            fwrite(rmsg,1,status,fp);
            memset( rmsg, '\0', MAX_MSG_SIZE );
            } while (!((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
                      ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
                      ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts))&& 0 == msg_struct->continue_flag);
        fclose(fp); 
        
        }

        //fetch end
        
        //clear avatar
        memset( smsg, '\0', MAX_MSG_SIZE );
        memset( rmsg, '\0', MAX_MSG_SIZE );
        index=0;
        memset( msgHdr, '\0', sizeof( message_hdr_req ) );
        msgHdr->message_type = EClearOwnAvatar;    
        memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
        index += sizeof( message_hdr_req );
        smsg[index++]='\0';
        result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
         do  {
            status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
            memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) ); 
            memset( rmsg, '\0', MAX_MSG_SIZE );
            } while (msg_struct->hdr_req.message_type!=EClearOwnAvatar);
       
         if ( msg_struct->response != 1)
        {
            return_value = KErrGeneral;
        goto return_code;
        }
             //clear avatar end     
     // logout test code from server
     index=0;
     memset( smsg, '\0', MAX_MSG_SIZE );
     memset( rmsg, '\0', MAX_MSG_SIZE );
     
     memset( msgHdr, '\0', sizeof( message_hdr_req ) );
     
     msgHdr->message_type = ELogout_Request;
  
     memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
     index += sizeof( message_hdr_req );

     
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
      do  {
         status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
         memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) ); 
         memset( rmsg, '\0', MAX_MSG_SIZE );
         } while (msg_struct->hdr_req.message_type!=ELogout_Request);
    
      if ( msg_struct->response != 1)
     {
         return_value = KErrGeneral;
     goto return_code;
     }
     //killing isoserver
     return_code:
     index=0;
     memset( smsg, '\0', MAX_MSG_SIZE );
     memset( rmsg, '\0', MAX_MSG_SIZE );
     memset( msgHdr, '\0', sizeof( message_hdr_req ) );
     msgHdr->message_type = EKill_Process;
     memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
     index += sizeof( message_hdr_req );
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
     free( msg_struct );
     return return_value;
     }  


// -----------------------------------------------------------------------------
// Cisolationserver::clear avatar
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cisolationserver::FetchCachedContacts( CStifItemParser& aItem )
    {

    
    ConnectData  *connect_data = NULL;
     char smsg[MAX_MSG_SIZE];
     char rmsg[MAX_MSG_SIZE];
     int index=0;
     FILE *fp;
         
     int timeout = 200;
     int nBytes;

     int result = 0;
     int err = 0;
     int status = 0;
     message_hdr_req *msgHdr = NULL;  
     message_hdr_resp* msg_struct = NULL;
     TInt return_value = 0;
             // Read data from the CFG file
     if ( read_data ( connect_data, aItem ) != RC_OK )
         {
         iLog->Log ( _L ( "read_data failed" ) );
         free_data ( connect_data );
         return_value = KErrGeneral;
         goto return_code;
         }
     //request formation 
     memset( smsg, '\0', MAX_MSG_SIZE );
     memset( rmsg, '\0', MAX_MSG_SIZE );
     
     //message header use
     msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
     memset( msgHdr, '\0', sizeof( message_hdr_req ) );
     
     msgHdr->message_type = ELogin_Request;
  
     memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
     index += sizeof( message_hdr_req );
     
     int len = strlen(connect_data->username);
     strcpy( smsg + index, connect_data->username );
     index += len + 1;
     
     //appending password
     
     len = strlen(connect_data->password );
     strcpy( smsg + index, connect_data->password );
     index += len + 1;
     
     //appending server name
     
     len = strlen(connect_data->server );
     strcpy( smsg + index, connect_data->server );
     index += len + 1;
     
     //appending resource
     len = strlen(connect_data->resource );
     strcpy( smsg + index, connect_data->resource );
     index += len + 1;
     
     //appending ssl
     smsg[index++]= '1';
     smsg[index++]= '\0';
     
     //appending server port
     len = strlen(connect_data->port );
     strcpy( smsg + index, connect_data->port );
     index += len + 1;
     
     // appending IAP id
     
     smsg[index++]= '1';
     smsg[index++]= '\0';
     
     //appending connmgr_bus
     len = strlen(connect_data->connmgr_bus );
     strcpy( smsg + index, connect_data->connmgr_bus );
     index += len + 1;
     
     //appending connmgr_path
     len = strlen(connect_data->connmgr_path );
     strcpy( smsg + index, connect_data->connmgr_path );
     index += len + 1;
     
     //appending protocol
     len = strlen(connect_data->protocol );
     strcpy( smsg + index, connect_data->protocol );
     index += len + 1;
     
     #ifdef __WINSCW__
     //proxy server
     len = strlen(connect_data->proxy_data->proxy_server );
     strcpy( smsg + index, connect_data->proxy_data->proxy_server );
     index += len + 1;
     
     //proxy port
     len = strlen(connect_data->proxy_data->proxy_port );
     strcpy( smsg + index, connect_data->proxy_data->proxy_port );
     index += len + 1;
     
     #endif
     
     //running isoserver   
     run_isoserver(); 
     
         
     //create message queue for send request
     result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
     //create message queue for response request
     result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
         
     
     /* Send Message to queueOne */
     
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
     
     status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
     
     if (status < 0)
     {
     return_value = KErrGeneral;
     goto return_code;   
     }
     msg_struct = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
     memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
     memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) );
     
     if( msg_struct->response != 1)
     {
         return_value = KErrGeneral;
     goto return_code;
     }
     
     
     // login end

     //fetch req..
     TInt count = 0;
            
        fp = fopen("c:\\fetch_contact.txt","a");
        do  {
            status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
            memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) ); 
            fwrite(rmsg,1,status,fp);
            memset( rmsg, '\0', MAX_MSG_SIZE );
            
            if ( (msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
                                  ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
                                  ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts) && 0 == msg_struct->continue_flag )
                        {
                        count++;
                        }
            
            
            } while (count < 3 );
        fclose(fp); 

        
        

        //fetch end
        FILE* fop = NULL;
        //clear avatar
        memset( smsg, '\0', MAX_MSG_SIZE );
        memset( rmsg, '\0', MAX_MSG_SIZE );
        index=0;
        memset( msgHdr, '\0', sizeof( message_hdr_req ) );
        msgHdr->message_type = EFetchCachedContacts;    
        memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
        index += sizeof( message_hdr_req );
        smsg[index++]='\0';
        result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
        fop = fopen("c:\\santhosh.txt","a");
        count = 0;
        do  {
            status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err); 
            memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) ); 
            fwrite(rmsg,1,status,fop);
            memset( rmsg, '\0', MAX_MSG_SIZE );
            
            if ( ((msg_struct->hdr_req.message_type==ECurrent_Member_Contacts) 
                                  ||(msg_struct->hdr_req.message_type==ELocal_Pending_Contacts) 
                                  ||(msg_struct->hdr_req.message_type==ERemote_Pending_Contacts)) && 0 == msg_struct->continue_flag )
                        {
                        count++;
                        }
            
            
            } while (count < 3 );
        fclose(fop); 

     //clear avatar end     
     // logout test code from server
     index=0;
     memset( smsg, '\0', MAX_MSG_SIZE );
     memset( rmsg, '\0', MAX_MSG_SIZE );
     
     memset( msgHdr, '\0', sizeof( message_hdr_req ) );
     
     msgHdr->message_type = ELogout_Request;
  
     memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
     index += sizeof( message_hdr_req );

     
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
      do  {
         status = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, 2000, &err);
         memcpy( msg_struct, rmsg, sizeof( message_hdr_resp ) ); 
         memset( rmsg, '\0', MAX_MSG_SIZE );
         } while (msg_struct->hdr_req.message_type!=ELogout_Request);
    
      if ( msg_struct->response != 1)
     {
         return_value = KErrGeneral;
     goto return_code;
     }
     //killing isoserver
     return_code:
     index=0;
     memset( smsg, '\0', MAX_MSG_SIZE );
     memset( rmsg, '\0', MAX_MSG_SIZE );
     memset( msgHdr, '\0', sizeof( message_hdr_req ) );
     msgHdr->message_type = EKill_Process;
     memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
     index += sizeof( message_hdr_req );
     result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
     free( msg_struct );
     return return_value;
     }  

// -----------------------------------------------------------------------------
// Cisolationserver::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt Cisolationserver::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
