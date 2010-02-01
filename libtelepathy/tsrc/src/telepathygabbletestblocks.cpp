/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Stif Test Cases Test blocks file.
*		 RunMethodL mentions all the test cases that will be run.
*
*/





// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>
#include "telepathygabbletest.h"
#include "telepathygabbletestdata.h"
//glib related include files
#include <glib.h>
#include <glib/gprintf.h>           // for g_stpcpy
#include <string.h>
#include <stdlib.h>

#include <glowmem.h>

//Login related file
#include "telepathygabblelogintest.h"
//Send Message related file
#include "telepathygabblesendmessage.h"
//AddDeleteContact related file
#include "telepathygabbleadddeletecontact.h"
//FetchContacts related files
#include "telepathygabblefetchcontacts.h"
//Search related files
#include "telepathygabblesearch.h"


const TInt KRepeatedMessageCount=4;



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
	gchar      *key;
	gchar      *value;	
}SearchData;
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
    gchar       *port;
    gchar       *ssl_flag; //needed for gizmo and gtalk.
    SearchData  *search_data;

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
} HandleData;

// Function to read data from the cfg file
static RC_Code_t read_data ( ConnectData*& connect_data, 
                             CStifItemParser& aItem, CStifLogger* aLog );
                             
// Function to free the allocated resources
static void free_data ( ConnectData*& connect_data, CStifLogger* aLog);
                             

                             
// ============================= LOCAL FUNCTIONS ===============================

//-----------------------------------------------------------------------------
// function_name	: read_data
// description     	: Reads data from the CFG file
// Returns          : None
//-----------------------------------------------------------------------------
//
static RC_Code_t
read_data ( ConnectData*& connect_data, CStifItemParser& aItem, CStifLogger* aLog )
    {
	TPtrC               string;
	TBuf8<KMaxFileName> temp;
	
	//TBuf8<KMaxChar>		temp;
	MessageData         *msg_data   = NULL;
	
	SearchData         *search_data   = NULL;
#ifdef __WINSCW__
	ProxyData           *proxy_data = NULL;
#endif
	
	connect_data = g_new0 ( ConnectData, 1 );
	if ( !connect_data )
	    {
		return RC_ERROR;
	    }
	    
	msg_data = g_new0 ( MessageData, 1 );
	if ( !msg_data )
	    {
		free_data (connect_data, aLog );
		return RC_ERROR;
	    }
	
	connect_data->msg_data = msg_data;
	
	search_data = g_new0 ( SearchData, 1 );
	if ( !search_data )
	    {
		free_data (connect_data, aLog );
		return RC_ERROR;
	    }
	connect_data->search_data = search_data;	    
	
#ifdef __WINSCW__
    proxy_data = g_new0 ( ProxyData, 1 );
    if ( !proxy_data ) 
    	{
    	free_data (connect_data, aLog );
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
			free_data (connect_data, aLog );
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
	    	free_data (connect_data, aLog );
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
	    	free_data (connect_data, aLog );
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
	    	free_data (connect_data, aLog );
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
	    	free_data (connect_data, aLog );
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
	    	free_data (connect_data, aLog );
	    	return RC_ERROR;
	        }
	    g_stpcpy ( connect_data->resource, ( gchar * ) temp.Ptr() );		    
	    }
	 

	// Read proxy server and proxy port
    if ( aItem.GetNextString ( string ) == KErrNone )
    	{
    	temp.Copy ( string );
    	temp.Append ( '\0' );
    	#ifdef __WINSCW__
    	proxy_data->proxy_server = g_new0 ( gchar, temp.Length() );
    	if ( !proxy_data->proxy_server )
    		{
    		free_data (connect_data, aLog );
    		return RC_ERROR;
    		}
    	g_stpcpy ( proxy_data->proxy_server, ( gchar * ) temp.Ptr() );
    	#endif 
    	}
    	
    if ( aItem.GetNextString ( string ) == KErrNone )
    	{
    	temp.Copy ( string );
    	temp.Append ( '\0' );
    	#ifdef __WINSCW__
    	proxy_data->proxy_port = g_new0 ( gchar, temp.Length() );
    	if ( !proxy_data->proxy_port )
    		{
    		free_data (connect_data, aLog );
    		return RC_ERROR;
    		}
    	g_stpcpy ( proxy_data->proxy_port, ( gchar * ) temp.Ptr() );
    	#endif
    	}	
    	
    	/* if ( aItem.GetNextInt ( proxy_port ) == KErrNone )
    		{
    		proxy_data->proxy_port = proxy_port;
    		}    	 */
    		
    		

	//Read the port
	if ( aItem.GetNextString ( string ) == KErrNone )
	{
	temp.Copy ( string );
	temp.Append ( '\0' );
	connect_data->port = g_new0 ( gchar, temp.Length() );
	if ( !connect_data->port )
		{
		free_data (connect_data, aLog );
		return RC_ERROR;
		}
	g_stpcpy ( connect_data->port, ( gchar * ) temp.Ptr() ); 
	}

	//Read the ssl_flag value
  	if ( aItem.GetNextString ( string ) == KErrNone )
    {
	temp.Copy ( string );
	temp.Append ( '\0' );
	connect_data->ssl_flag = g_new0 ( gchar, temp.Length() );
	if ( !connect_data->ssl_flag )
	    {			
		free_data (connect_data, aLog );
		return RC_ERROR;
	    }
	g_stpcpy ( connect_data->ssl_flag, ( gchar * ) temp.Ptr() ); 
    }
    
    // Read key
	if ( aItem.GetNextString ( string ) == KErrNone )
	    {
	    temp.Copy ( string );
	    temp.Append ( '\0' );
	    search_data->key = g_new0 ( gchar, temp.Length() );
	    if ( !search_data->key )
	        {
	    	free_data (connect_data, aLog );
	    	return RC_ERROR;
	        }
	    g_stpcpy ( search_data->key, ( gchar * ) temp.Ptr() );
	    }
	
	// Read value
	if ( aItem.GetNextString ( string ) == KErrNone )
	    {
	    temp.Copy ( string );
	    temp.Append ( '\0' );	    
	    search_data->value = g_new0 ( gchar, temp.Length() );
	    if ( !search_data->value )
	        {
	    	free_data (connect_data, aLog );
	    	return RC_ERROR;
	        }
	    g_stpcpy ( search_data->value, ( gchar * ) temp.Ptr() );
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
free_data ( ConnectData*& connect_data, CStifLogger* aLog )
    {
    //iLog->Log( _L("free_data\n") );
	if ( connect_data )
	    {
		if ( connect_data->server )
		    {
		    //iLog->Log( _L("free server\n") );
			g_free ( connect_data->server );
			connect_data->server = NULL;
		    }
		
		if ( connect_data->username )
		    {
		    aLog->Log( _L(" free username\n") );
			g_free ( connect_data->username );
			connect_data->username = NULL;
		    }
		
		if ( connect_data->password )
		    {
		    aLog->Log( _L(" free password\n") );
			g_free ( connect_data->password );
			connect_data->password = NULL;
		    }
		
		if ( connect_data->msg_data )
		    {
			if ( connect_data->msg_data->recipient )
			    {
			    aLog->Log( _L(" free recipient\n") );
				g_free ( connect_data->msg_data->recipient );
				connect_data->msg_data->recipient = NULL;
			    }
			if ( connect_data->msg_data->message )
			    {
			    aLog->Log( _L(" free message\n") );
				g_free ( connect_data->msg_data->message );
				connect_data->msg_data->message = NULL;
			    }
			g_free ( connect_data->msg_data );
			connect_data->msg_data = NULL;
		    }		
		
		if ( connect_data->resource )
		    {
		    aLog->Log( _L(" free resource\n") );
			g_free ( connect_data->resource );
			connect_data->resource = NULL;
		    }
		    
	#ifdef __WINSCW__
	    if ( connect_data->proxy_data )
	    	{
	    	if ( connect_data->proxy_data->proxy_server )
	    		{
	    		aLog->Log( _L(" free proxy_server\n") );
	    		g_free ( connect_data->proxy_data->proxy_server );
	    		connect_data->proxy_data->proxy_server = NULL;
	    		}
    		if ( connect_data->proxy_data->proxy_port )
	    		{
	    		aLog->Log( _L(" free proxy_port\n") );
	    		g_free ( connect_data->proxy_data->proxy_port );
	    		connect_data->proxy_data->proxy_port = NULL;
	    		}
	    	g_free ( connect_data->proxy_data );
	    	connect_data->proxy_data = NULL;	
	    	}
	#endif	 
	
	    //port free
	    if ( connect_data->port )
		    {
		    aLog->Log( _L(" free port\n") );
			g_free ( connect_data->port );
			connect_data->port = NULL;
		    }
		    
		//ssl_flag free
		if ( connect_data->ssl_flag )
		    {
		        aLog->Log( _L(" free ssl_flag\n") );
				g_free ( connect_data->ssl_flag );
				connect_data->ssl_flag = NULL;
		    }
		    
		if ( connect_data->search_data )
		    {
			if ( connect_data->search_data->key )
			    {
			    aLog->Log( _L(" free key\n") );
				g_free ( connect_data->search_data->key );
				connect_data->search_data->key = NULL;
			    }
			if ( connect_data->search_data->value )
			    {
			    aLog->Log( _L(" free value\n") );
				g_free ( connect_data->search_data->value );
				connect_data->search_data->value = NULL;
			    }
			g_free ( connect_data->search_data );
			connect_data->search_data = NULL;
		    }
		aLog->Log( _L(" free connect_data\n") );		    		    
		g_free ( connect_data );
		connect_data = NULL;    
	    }
	    aLog->Log( _L(" free connect_data out\n") );
    }
    
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CTelepathyGabbleTest::Delete() 
    {
    	 iLog->Log( _L("CTelepathyGabbleTest::Delete in\n") );
	     if (iLoginObj)
	     {
	       	iLog->Log( _L("In Delete iLoginObj \n") );
	     	delete iLoginObj;
	     	iLoginObj = NULL;
	     }

		 if (iSendMsgObj)
		 {
		 	delete iSendMsgObj;
		 	iSendMsgObj = NULL;
		 }
		 
		 if (iAddDelContactObj)
		 {
		 	delete iAddDelContactObj;
		 	iAddDelContactObj = NULL;
		 }
		 
		 if (iFetchContactObj)
		 {
		 	delete iFetchContactObj;
		 	iFetchContactObj = NULL;
		 }
		 
		 if (iSearchObj)
		 {
		 	delete iSearchObj;
		 	iSearchObj = NULL;
		 }
		 
		 if (iTestData)
		 {
		    iLog->Log( _L("In Delete iTestData \n") );
		 	delete iTestData;
		 	iTestData = NULL;
		 	
		 }
		 iLog->Log( _L("CTelepathyGabbleTest::Delete out\n") );
    }

// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CTelepathyGabbleTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {
    
    iLog->Log( _L("CTelepathyGabbleTest::RunMethodL in") );
   
 
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        //ENTRY( "Example", CTelepathyGabbleTest::ExampleL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
        
        //Login - Logout related test cases
        ENTRY("LoginTest", CTelepathyGabbleTest::LoginTestL ),
        ENTRY("LogoutTest", CTelepathyGabbleTest::LogoutTestL ),
        ENTRY("LoginInvalidUseridTest", CTelepathyGabbleTest::LoginInvalidUseridTestL ),
        ENTRY("LoginInvalidPasswdTest", CTelepathyGabbleTest::LoginInvalidPasswdTestL ),
        ENTRY("LoginInvalidUseridPasswdTest", CTelepathyGabbleTest::LoginInvalidUseridPasswdTestL ),
        ENTRY("LoginblankUseridTest", CTelepathyGabbleTest::LoginblankUseridTestL ),
        ENTRY("LoginblankPasswdTest", CTelepathyGabbleTest::LoginblankPasswdTestL ),
        ENTRY("LoginblankUseridblankPasswdTest", CTelepathyGabbleTest::LoginblankUseridblankPasswdTestL ),
        ENTRY("LoginWithMaxLengthUserIDTest", CTelepathyGabbleTest::LoginTestL ),
        ENTRY("LoginWithMinLengthUserIDTest", CTelepathyGabbleTest::LoginTestL ),
        ENTRY("LoginUseridWithSplChrsTest", CTelepathyGabbleTest::LoginInvalidUseridTestL ),
        ENTRY("LoginCancelTest", CTelepathyGabbleTest::LoginCancelTestL ),
        
        
        //Send Message related Test Cases
        ENTRY("SendMessageTest", CTelepathyGabbleTest::SendMessageTestL ),
        ENTRY("SendMesasgeToMaxLengthUserIDTest", CTelepathyGabbleTest::SendMessageTestL ),        
		ENTRY("SendMesasgetoofflinecontactTest", CTelepathyGabbleTest::SendMessagetoinvalidUseridTestL ),
        ENTRY("SendMessageWith400Test", CTelepathyGabbleTest::SendMessageWith400TestL ),
        ENTRY("SendMesasgeBlankTest", CTelepathyGabbleTest::SendMesasgeBlankTestL ),
        ENTRY("SendMesasgeToBlankUserIdTest", CTelepathyGabbleTest::SendMesasgeToBlankUserIdTestL ),
        ENTRY("SendMesasgeSplCharsTest", CTelepathyGabbleTest::SendMessageTestL ),
        ENTRY("SendMesasgeEmoticonsTest", CTelepathyGabbleTest::SendMessageTestL ),        
        ENTRY("SendMessageRepeatedTest",CTelepathyGabbleTest:: SendMessageRepeatedTestL),        
        ENTRY("SendMessagetoUserNotInContactListTest",CTelepathyGabbleTest:: SendMessageTestL),
        ENTRY("SendMessagetoinvalidUseridTest",CTelepathyGabbleTest:: SendMessagetoinvalidUseridTestL),
        
        //Receive Message related Test Cases
        ENTRY("ReceiveMessageTest", CTelepathyGabbleTest::ReceiveMessageTestL ),
        ENTRY("ReceiveMessageMaxLengthTest", CTelepathyGabbleTest::ReceiveMessageMaxLengthTestL ),
        ENTRY("ReceiveMessageWithSplChrsTest", CTelepathyGabbleTest::ReceiveMessageTestL ),  
        ENTRY("ReceiveMessageWithEmoticonsTest", CTelepathyGabbleTest::ReceiveMessageTestL ),                
        ENTRY("ReceiveMessageBlankTest", CTelepathyGabbleTest::ReceiveMessageBlankTestL ),
        ENTRY("ReceiveRepeatedMessageTest", CTelepathyGabbleTest::ReceiveRepeatedMessageTestL ),
        
        //Add Contact related test cases
        ENTRY("AddContactTest", CTelepathyGabbleTest::AddContactTestL ),
        ENTRY("AddContactWithMaxLengthUserIdTest", CTelepathyGabbleTest::AddContactTestL ),
        ENTRY("AddContactWithBlankUserIdTest", CTelepathyGabbleTest::AddContactWithBlankUserIdTestL ),       
        ENTRY("AddContactinvalidTest", CTelepathyGabbleTest::AddContactTestL ),
        ENTRY("AddContactinvalidWithSplCharsTest", CTelepathyGabbleTest::AddContactTestL ),
        ENTRY("AddContactAlreadyInRosterTest", CTelepathyGabbleTest::AddContactAlreadyInRosterTestL ),
        
        
        //Remove Contact related test cases
        ENTRY("RemoveContactTest", CTelepathyGabbleTest::RemoveContactTestL ),
        ENTRY("RemoveInvalidContactTest", CTelepathyGabbleTest::RemoveInvalidContactTestL),
        ENTRY("RemoveContactAlreadyRemovedTest", CTelepathyGabbleTest::RemoveInvalidContactTestL ),
        ENTRY("RemoveContactWithMaxLengthUserIDTest", CTelepathyGabbleTest::RemoveContactTestL ),
        ENTRY("RemoveContactWithBlankUserIdTest", CTelepathyGabbleTest::RemoveContactWithBlankUserIdTestL ),       
        
        //Fetch Contact related test cases
        ENTRY("FetchContactsTest", CTelepathyGabbleTest::FetchContactsTestL ),
        ENTRY("FetchZeroContactTest", CTelepathyGabbleTest::FetchContactsTestL ),
       
        //Search related test cases
        ENTRY("SearchTest", CTelepathyGabbleTest::SearchTestL ),
        ENTRY("GetSearchKeysTest", CTelepathyGabbleTest::GetSearchKeysTestL ),
        ENTRY("SearchInvalidKeyTest", CTelepathyGabbleTest::SearchInvalidKeyTestL ),
        ENTRY("SearchNoKeyValueTest", CTelepathyGabbleTest::SearchNoKeyValueTestL ),
        ENTRY("SearchTwoInvalidKeysTest", CTelepathyGabbleTest::SearchTwoInvalidKeysTestL ),        
        ENTRY("SearchWithAllKeysTest", CTelepathyGabbleTest::SearchWithAllKeysTestL ),
        ENTRY("SearchBlankStringTest", CTelepathyGabbleTest::SearchBlankStringTestL ),
        ENTRY("SearchTwiceTest", CTelepathyGabbleTest::SearchTwiceTestL ),
        ENTRY("SearchTwiceWithDiffStringsTest", CTelepathyGabbleTest::SearchTwiceWithDiffStringsTestL ),
        ENTRY("SearchFiveTimesTest", CTelepathyGabbleTest::SearchFiveTimesTestL ),        
        ENTRY("SearchAfterGetSearchKeys", CTelepathyGabbleTest::SearchAfterGetSearchKeysL ),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::LoginTestL
// Test for login.
// -----------------------------------------------------------------------------
//

TInt CTelepathyGabbleTest::LoginTestL( 
    CStifItemParser& aItem  )
	{
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		iLog->Log( _L("LoginTestL starting") );
		
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog) != RC_OK )
		{
			//iLog->Log( _L("read_data failed") );
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  	proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
		iTestData = CTestData::NewL(iLog);
		
		iLog->Log( _L("iTestData created") );
	
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		iLog->Log( _L("iLoginObj created") );
			
		iTestData->SetTestType(CTestData::ELogin) ;
		
		//action_login will have parameters 
		//passed from cfg file
		TInt login_status = 
			iLoginObj->action_login(username,passwd,servername, resource_login,ssl_flag,
																port,proxy_server, proxy_port, iTestData);
			
		
		iLog->Log( _L("after action_login") );
		
		if (login_status == CTestData::ECONNECTED)
		{
			//Do the logout so that all the resources are deallocated properly
			login_status = iLoginObj->action_logout(iTestData);
			
			iLog->Log( _L("after action_logout") );
			
			free_data (connect_data, iLog);
			
			iLog->Log( _L("after free_data") );
			if (login_status != CTestData::EDISCONNECTED)
			{
			iLog->Log( _L("logout failed") );
			return KErrGeneral; 	//logout didnot happen successfully.
			}
			
			//we completed login and logout successfully.
			iLog->Log( _L("login-logout test case passed\n") );
			
			if (iTestData)
			 { 	iLog->Log( _L("delete iTestData \n") );
			 	delete iTestData;
			 	iTestData = NULL;
			 	
			 }
			 if (iLoginObj)
		     {
		       	iLog->Log( _L("delete iLoginObj \n") );
		     	delete iLoginObj;
		     	iLoginObj = NULL;
		     }
			return KErrNone;
		}
		else
		{
			//Login didnot happen successfully.
			//return from here itself, no need for a logout
			free_data (connect_data, iLog );
			iLog->Log( _L("login test case failed") );
			return KErrGeneral;
		
		}
	
	}


// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::LoginblankUseridblankPasswdTestL
// -----------------------------------------------------------------------------		
TInt CTelepathyGabbleTest::LoginblankUseridblankPasswdTestL( 
    CStifItemParser& aItem  )
	{
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data ( connect_data,iLog );
			return KErrGeneral;
		}
		

		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
		
		username = 	""; //make the username as blank
		passwd   =  ""; //make the username as blank
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  	proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
		
		iTestData = CTestData::NewL(iLog);
	
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		iTestData->SetTestType(CTestData::ELogin) ;
			
		//action_login will have parameters 
		//passed from cfg file
		TInt login_status = 
			iLoginObj->action_login(username,passwd,servername, resource_login, ssl_flag,
															port,proxy_server, proxy_port,iTestData);
		
		free_data (connect_data, iLog );
		
		if (login_status == CTestData::EDISCONNECTED)
		{
			//login_status is disconnected(default state) 
			//which is correct since the username was blank.
			
			return KErrNone;
		}
		else
		{
			//There was some error
			return KErrGeneral;
		}
			
	}

// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::LoginblankUseridTestL
// -----------------------------------------------------------------------------		
TInt CTelepathyGabbleTest::LoginblankUseridTestL( 
    CStifItemParser& aItem  )
	{
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data ( connect_data,iLog );
			return KErrGeneral;
		}
		

		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
		
		username = 	""; //make the username as blank
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  	proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
		
		iTestData = CTestData::NewL(iLog);
	
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		iTestData->SetTestType(CTestData::ELogin) ;
			
		//action_login will have parameters 
		//passed from cfg file
		TInt login_status = 
			iLoginObj->action_login(username,passwd,servername, resource_login, ssl_flag,
															port,proxy_server, proxy_port,iTestData);
		
		free_data (connect_data, iLog );
		
		if (login_status == CTestData::EDISCONNECTED)
		{
			//login_status is disconnected(default state) 
			//which is correct since the username was blank.
			
			return KErrNone;
		}
		else
		{
			//There was some error
			return KErrGeneral;
		}
			
	}

// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::LoginblankPasswdTestL
// -----------------------------------------------------------------------------	
TInt CTelepathyGabbleTest::LoginblankPasswdTestL( 
CStifItemParser& aItem  )
{
	//Pass the parameters to action_login() from the 
	//data mentioned in the cfg file.
	
	ConnectData  *connect_data = NULL;
	
	if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
	{
		free_data ( connect_data, iLog );
		return KErrGeneral;
	}

	char *username, *passwd , *servername , *resource_login,*ssl_flag, *port;
	char *proxy_server = NULL;
	char *proxy_port = NULL;
		
	
	username = 	connect_data->username;
	passwd   =  ""; //make the passwd as blank.
	servername = 	connect_data->server;
	resource_login = connect_data->resource;
	ssl_flag = connect_data->ssl_flag;
	port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
	
	iTestData = CTestData::NewL(iLog);

	iLoginObj = CTelepathyGabbleLoginTest::NewL();
	
	iTestData->SetTestType(CTestData::ELogin) ;
		
	//action_login will have parameters 
	//passed from cfg file
	TInt login_status = 
		iLoginObj->action_login(username,passwd,servername, resource_login,ssl_flag,
														port,proxy_server, proxy_port,iTestData);
	
	free_data (connect_data, iLog );
	
	if (login_status == CTestData::EDISCONNECTED)
	{
		//the status is disconnected which is correct since the 
		//login didnot happen with blank passwd.
		return KErrNone;
	}
	else
	{
		return KErrGeneral;
	
	}			
}



// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::LogoutTestL
// Test for logout
// -----------------------------------------------------------------------------
TInt CTelepathyGabbleTest::LogoutTestL( 
CStifItemParser& aItem  )
{
	//Pass the parameters to action_login() from the 
	//data mentioned in the cfg file.
	
	ConnectData  *connect_data = NULL;
	
	if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
	{
		free_data ( connect_data, iLog );
		return KErrGeneral;
	}
	

	char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
	char *proxy_server = NULL;
	char *proxy_port = NULL;
		
	
	username = 	connect_data->username;
	passwd   =  connect_data->password;
	servername = 	connect_data->server;
	resource_login = connect_data->resource;
	ssl_flag = connect_data->ssl_flag;
	port = connect_data->port;
	#ifdef __WINSCW__
  		proxy_server = connect_data->proxy_data->proxy_server;
		proxy_port = connect_data->proxy_data->proxy_port;
	#endif
		
	
	iTestData = CTestData::NewL(iLog);
	
	iLoginObj = CTelepathyGabbleLoginTest::NewL();
	
	iTestData->SetTestType(CTestData::ELogin);
	
	//action_login will have parameters 
	//passed from cfg file
	
	TInt login_status = iLoginObj->action_login(username,passwd,servername, resource_login,ssl_flag,
																							port,proxy_server, proxy_port,iTestData);
	
	
	if (login_status == CTestData::ECONNECTED)
	{
		//If the login variable is ECONNECTED
		//Do the Logout
		login_status = iLoginObj->action_logout(iTestData);
		
	
		if (login_status == CTestData::EDISCONNECTED)
		{
	
			//the logout happened succesfully
			free_data ( connect_data, iLog );
			return KErrNone;
		}
		else 
		{
			//login_status is still connected, there was a problem 
			//in logging out.
			free_data ( connect_data, iLog );
			return KErrGeneral;
		}
		
	}
	else
	{
		//Login itself didnot happen.
		free_data ( connect_data, iLog );
		return KErrGeneral;
	}
		
}



// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::SearchAfterGetSearchKeysL
// -----------------------------------------------------------------------------

TInt CTelepathyGabbleTest::SearchAfterGetSearchKeysL( CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data ( connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login, *ssl_flag, *value, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		value = connect_data->msg_data->message;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		

		if (iTestData == NULL)
		{
			//Create the iTestData if it is not existing.
			
			iTestData = CTestData::NewL(iLog);
		}
		
		iTestData->SetTestType(CTestData::EGetSearchKeys);
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = 
			iLoginObj->action_login(username,passwd,servername, 
									resource_login,ssl_flag,
									port,proxy_server, proxy_port,
									iTestData );
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login was successful
			//Create the Object of Send Message class
			iSearchObj = CTelepathyGabbleSearch::NewL();
				
			TInt search_state = iSearchObj->GetSearchKeysL( iTestData);
			
			if( search_state == CTestData::ESearchError )
				{
					return_value = KErrGeneral; //Error in finding search keys
				}
			else if( search_state == CTestData::ESearchKeysFound) 
				{
					return_value = KErrNone; 
					
					GHashTable *data_to_search = NULL;				
					data_to_search = g_hash_table_new(g_str_hash,g_str_equal);
					gchar **keys = iTestData->GetSearchKeys();
				
					iTestData->SetTestType(CTestData::ESearch);
					for(TInt i=0; i<8 ; i++ )
						{
							GValue *gvalue = NULL;
							gvalue = g_new0(GValue, 1);	
						
							g_value_init(gvalue, G_TYPE_STRING);
							g_value_set_string (gvalue, g_strdup(value) );
							g_hash_table_insert( data_to_search,keys[i], gvalue);	
							
						}
					
					search_state = iSearchObj->SearchL(data_to_search, iTestData);
			
					if( search_state == CTestData::ESearchError )
						{
							return_value = KErrGeneral; //Error in Searching
						}
					else if( search_state == CTestData::ESearchCompleted) 
						{
							return_value = KErrNone; 
						}
					//Free the hash table itself
					g_hash_table_destroy ( data_to_search );
					

				}
			iSearchObj->CloseChannel(iTestData);
			//Do the logout
			login_status = iLoginObj->action_logout(iTestData);

			if (login_status != CTestData::EDISCONNECTED)
				{
					//logout didnot happen successfully
					return_value = KErrGeneral;
				}
			
			//do this later	
		   	/*g_hash_table_foreach_remove ( data_to_search, 
					search_hash_remove, NULL );*/
		
		}
		else
		{
			//Login itself didnot happen.
			return_value =  KErrGeneral;
		}		
		
		free_data ( connect_data, iLog );	
		return return_value;
	}
	


// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::SearchWithAllKeysTestL
// -----------------------------------------------------------------------------

TInt CTelepathyGabbleTest::SearchWithAllKeysTestL( CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data ( connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login, *ssl_flag, *value, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		value = connect_data->msg_data->message;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		

		if (iTestData == NULL)
		{
			//Create the iTestData if it is not existing.
			
			iTestData = CTestData::NewL(iLog);
		}
		iTestData->SetTestType(CTestData::EGetSearchKeys);
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = 
			iLoginObj->action_login(username,passwd,servername, 
									resource_login,ssl_flag,
									port,proxy_server, proxy_port,
									iTestData );
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login was successful
			//Create the Object of Send Message class
			iSearchObj = CTelepathyGabbleSearch::NewL();
				
			TInt search_state = iSearchObj->GetSearchKeysL( iTestData);
			
			if( search_state == CTestData::ESearchError )
				{
					return_value = KErrGeneral; //Error in finding search keys
				}
			else if( search_state == CTestData::ESearchKeysFound) 
				{
					return_value = KErrNone; 
					
					iTestData->SetTestType(CTestData::ESearch);
					GHashTable *data_to_search = NULL;				
					TInt i = 0;			
					data_to_search = g_hash_table_new(g_str_hash,g_str_equal);
					gchar **keys = iTestData->GetSearchKeys();
					
					for( i=0; i<7 ; i++ )
						{
							GValue *gvalue = NULL;
							gvalue = g_new0(GValue, 1);	
						
							g_value_init(gvalue, G_TYPE_STRING);
							g_value_set_string (gvalue, g_strdup(value) );
							g_hash_table_insert( data_to_search, keys[i], gvalue);	
							
						}
						
					GValue *gvalue1 = NULL;
					gvalue1 = g_new0(GValue, 1);	
				
					g_value_init(gvalue1, G_TYPE_STRING);
					g_value_set_string (gvalue1, g_strdup("5") );
					g_hash_table_insert( data_to_search,keys[i], gvalue1);						
					
					search_state = iSearchObj->SearchL(data_to_search, iTestData);
			
					if( search_state == CTestData::ESearchError )
						{
							return_value = KErrGeneral; //Error in Searching
						}
					else if( search_state == CTestData::ESearchCompleted) 
						{
							return_value = KErrNone; 
						}
					//Free the hash table itself
					g_hash_table_destroy ( data_to_search );
					g_free(keys);

				}
			iSearchObj->CloseChannel(iTestData);
			//Do the logout
			login_status = iLoginObj->action_logout(iTestData);

			if (login_status != CTestData::EDISCONNECTED)
				{
					//logout didnot happen successfully
					return_value = KErrGeneral;
				}
			
			//do this later	
		   	/*g_hash_table_foreach_remove ( data_to_search, 
					search_hash_remove, NULL );*/
		
		}
		else
		{
			//Login itself didnot happen.
			return_value =  KErrGeneral;
		}		
		
		free_data ( connect_data, iLog );	
		return return_value;
	}
	


// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::SearchBlankStringTestL
// -----------------------------------------------------------------------------

TInt CTelepathyGabbleTest::SearchBlankStringTestL( 
    CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data ( connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		

		if (iTestData == NULL)
		{
			//Create the iTestData if it is not existing.
			
			iTestData = CTestData::NewL(iLog);
		}
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		iTestData->SetTestType(CTestData::ESearch);
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = 
			iLoginObj->action_login(username,passwd,servername, 
									resource_login,ssl_flag,
									port,proxy_server, proxy_port,
									iTestData );
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login was successful
			
			GHashTable *data_to_search = NULL;
			
			GValue *gvalue = NULL;
		
			char *key, *value;
			
			key = connect_data->msg_data->recipient;
			//value = connect_data->msg_data->message;
			//search for blank string
			value = "";
			
			data_to_search = g_hash_table_new(g_str_hash,g_str_equal);	
			
			gvalue = g_new0(GValue, 1);	
		
					 
			g_value_init(gvalue, G_TYPE_STRING);
			g_value_set_string (gvalue, value );
			g_hash_table_insert( data_to_search, key, gvalue);			      
			
		

			//Create the Object of Send Message class
			iSearchObj = CTelepathyGabbleSearch::NewL();	
			
			//call the SendMessage test case	
			TInt search_state = iSearchObj->SearchL(data_to_search, iTestData);
			
			if( search_state == CTestData::ESearchError )
				{
					return_value = KErrGeneral; //Error in Searching
				}
			else if( search_state == CTestData::ESearchCompleted) 
				{
					return_value = KErrNone; 
				}
			iSearchObj->CloseChannel(iTestData);
			//Do the logout
			login_status = iLoginObj->action_logout(iTestData);

			if (login_status != CTestData::EDISCONNECTED)
				{
					//logout didnot happen successfully
					return_value = KErrGeneral;
				}
			
			//do this later	
		   	/*g_hash_table_foreach_remove ( data_to_search, 
					search_hash_remove, NULL );*/
		
			//Free the hash table itself
			g_hash_table_destroy ( data_to_search );
		
		}
		else
		{
			//Login itself didnot happen.
			return_value =  KErrGeneral;
		}		
		
		free_data ( connect_data, iLog );	
		return return_value;
	}
	


// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::SearchTestL
// -----------------------------------------------------------------------------

TInt CTelepathyGabbleTest::SearchTestL( 
    CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data ( connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		

		if (iTestData == NULL)
		{
			//Create the iTestData if it is not existing.
			
			iTestData = CTestData::NewL(iLog);
		}
		iTestData->SetTestType(CTestData::ESearch);
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = 
			iLoginObj->action_login(username,passwd,servername, 
									resource_login,ssl_flag,
									port,proxy_server, proxy_port,
									iTestData );
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login was successful
			
			GHashTable *data_to_search = NULL;
			
			GValue *gvalue = NULL;
		
			char *key, *value;
			
			key = connect_data->msg_data->recipient;
			value = connect_data->msg_data->message;
			
			data_to_search = g_hash_table_new(g_str_hash,g_str_equal);	
			
			gvalue = g_new0(GValue, 1);	
		
					 
			g_value_init(gvalue, G_TYPE_STRING);
			g_value_set_string (gvalue, value );
			g_hash_table_insert( data_to_search, GetSearchLabelL(servername,key), gvalue);			      
			
		

			//Create the Object of Send Message class
			iSearchObj = CTelepathyGabbleSearch::NewL();	
			
			//call the SendMessage test case	
			TInt search_state = iSearchObj->SearchL(data_to_search, iTestData);
			
			if( search_state == CTestData::ESearchError )
				{
					return_value = KErrGeneral; //Error in Searching
				}
			else if( search_state == CTestData::ESearchCompleted) 
				{
					return_value = KErrNone; 
				}
			
			iSearchObj->CloseChannel(iTestData);
			//Do the logout
			login_status = iLoginObj->action_logout(iTestData);

			if (login_status != CTestData::EDISCONNECTED)
				{
					//logout didnot happen successfully
					return_value = KErrGeneral;
				}
			
			//do this later	
		   	/*g_hash_table_foreach_remove ( data_to_search, 
					search_hash_remove, NULL );*/
		
			//Free the hash table itself
			g_hash_table_destroy ( data_to_search );
		
		}
		else
		{
			//Login itself didnot happen.
			return_value =  KErrGeneral;
		}		
		
		free_data ( connect_data, iLog );	
		return return_value;
	}
	



// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::SearchTwoInvalidKeysTestL
// -----------------------------------------------------------------------------

TInt CTelepathyGabbleTest::SearchTwoInvalidKeysTestL( 
    CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data ( connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		

		if (iTestData == NULL)
		{
			//Create the iTestData if it is not existing.
			
			iTestData = CTestData::NewL(iLog);
		}
		iTestData->SetTestType(CTestData::ESearch);
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = 
			iLoginObj->action_login(username,passwd,servername, 
									resource_login,ssl_flag,
									port,proxy_server, proxy_port,
									iTestData );
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login was successful
			
			GHashTable *data_to_search = NULL;
			
			GValue *gvalue1, *gvalue2 ;
		
			char *key1,*key2, *value1, *value2;
			
			key1 = connect_data->msg_data->recipient;
			value1 = connect_data->msg_data->message;
			
			key2 = connect_data->search_data->key;
			value2 = connect_data->search_data->value;
			
			data_to_search = g_hash_table_new(g_str_hash,g_str_equal);	
			
			gvalue1 = g_new0(GValue, 1);	
			
					 
			g_value_init(gvalue1, G_TYPE_STRING);
			g_value_set_string (gvalue1, value1 );
			g_hash_table_insert( data_to_search, GetSearchLabelL(servername,key1), gvalue1);
			
			gvalue2 = g_new0(GValue, 1);	
			
					 
			g_value_init(gvalue2, G_TYPE_STRING);
			g_value_set_string (gvalue2, value2 );
			g_hash_table_insert( data_to_search, GetSearchLabelL(servername,key2), gvalue2);		
			
			
		

			//Create the Object of Send Message class
			iSearchObj = CTelepathyGabbleSearch::NewL();	
			
			//call the SendMessage test case	
			TInt search_state = iSearchObj->SearchL(data_to_search, iTestData);
			
			if( search_state == CTestData::ESearchError )
				{
				    //Error in Searching,
				    //as one of the search key passed is wrong
					return_value = KErrNone; 
				}
			else
				{
					return_value = KErrGeneral; 	
				}
			
			iSearchObj->CloseChannel(iTestData);		
			//Do the logout
			login_status = iLoginObj->action_logout(iTestData);

			if (login_status != CTestData::EDISCONNECTED)
				{
					//logout didnot happen successfully
					return_value = KErrGeneral;
				}
			
			//do this later	
		   	/*g_hash_table_foreach_remove ( data_to_search, 
					search_hash_remove, NULL );*/
		
			//Free the hash table itself
			g_hash_table_destroy ( data_to_search );
		
		}
		else
		{
			//Login itself didnot happen.
			return_value =  KErrGeneral;
		}		
		
		free_data ( connect_data, iLog );	
		return return_value;
	}
	


// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::SearchTwiceWithDiffStringsTestL
// -----------------------------------------------------------------------------

TInt CTelepathyGabbleTest::SearchTwiceWithDiffStringsTestL( 
    CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data ( connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		

		if (iTestData == NULL)
		{
			//Create the iTestData if it is not existing.
			
			iTestData = CTestData::NewL(iLog);
		}
		iTestData->SetTestType(CTestData::ESearch);
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = 
			iLoginObj->action_login(username,passwd,servername, 
									resource_login,ssl_flag,
									port,proxy_server, proxy_port,
									iTestData );
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login was successful
			
			GHashTable *data_to_search_first = NULL;
			GHashTable *data_to_search_second = NULL;	
			
			GValue *gvalue1, *gvalue2 ;
		
			char *key1,*value1, *key2, *value2;
		
			key1 = connect_data->msg_data->recipient;
			value1 = connect_data->msg_data->message;
			
			key2 = connect_data->search_data->key;
			value2 = connect_data->search_data->value;
			
			data_to_search_first = g_hash_table_new(g_str_hash,g_str_equal);	
			
			data_to_search_second = g_hash_table_new(g_str_hash,g_str_equal);
			
			gvalue1 = g_new0(GValue, 1);	
			g_value_init(gvalue1, G_TYPE_STRING);
			g_value_set_string (gvalue1, value1 );
			g_hash_table_insert( data_to_search_first, GetSearchLabelL(servername,key1), gvalue1);
			
			gvalue2 = g_new0(GValue, 1);
			g_value_init(gvalue2, G_TYPE_STRING);
			g_value_set_string (gvalue2, value2 );
			g_hash_table_insert( data_to_search_second, GetSearchLabelL(servername,key2), gvalue2);		
			
			
		

			//Create the Object of Send Message class
			iSearchObj = CTelepathyGabbleSearch::NewL();	
			
			//call the SendMessage test case	
			TInt search_state = iSearchObj->SearchL(data_to_search_first, iTestData);
			
			if( search_state == CTestData::ESearchError)
				{
				   return_value = KErrGeneral; 
				}
			else
				{
					search_state = iSearchObj->SearchL(data_to_search_second, iTestData);
					if( search_state == CTestData::ESearchError )
						{
						return_value = KErrGeneral; //Error in Searching	
						}
					
				}
			
			iSearchObj->CloseChannel(iTestData);		
			//Do the logout
			login_status = iLoginObj->action_logout(iTestData);

			if (login_status != CTestData::EDISCONNECTED)
				{
					//logout didnot happen successfully
					return_value = KErrGeneral;
				}
			
			//do this later	
		   	/*g_hash_table_foreach_remove ( data_to_search, 
					search_hash_remove, NULL );*/
		
			//Free the hash table itself
			g_hash_table_destroy ( data_to_search_first );
			g_hash_table_destroy ( data_to_search_second );	
		
		}
		else
		{
			//Login itself didnot happen.
			return_value =  KErrGeneral;
		}		
		
		free_data ( connect_data, iLog );	
		return return_value;
	}
	




// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::SearchNoKeyValueTestL
// -----------------------------------------------------------------------------

TInt CTelepathyGabbleTest::SearchNoKeyValueTestL( 
    CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data ( connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		

		if (iTestData == NULL)
		{
			//Create the iTestData if it is not existing.
			
			iTestData = CTestData::NewL(iLog);
		}
		iTestData->SetTestType(CTestData::ESearch);
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = 
			iLoginObj->action_login(username,passwd,servername, 
									resource_login,ssl_flag,
									port,proxy_server, proxy_port,
									iTestData );
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login was successful
			
			GHashTable *data_to_search = NULL;
			
			data_to_search = g_hash_table_new(g_str_hash,g_str_equal);	
		
			//Create the Object of Send Message class
			iSearchObj = CTelepathyGabbleSearch::NewL();	
			
			//call the SendMessage test case	
			TInt search_state = iSearchObj->SearchL(data_to_search, iTestData);
			
			if( search_state == CTestData::ESearchError )
				{
				    //Error in Searching,
				    //as one of the search key passed is wrong
					return_value = KErrNone; 
				}
			else
				{
					return_value = KErrGeneral; 	
				}
			iSearchObj->CloseChannel(iTestData);		
			//Do the logout
			login_status = iLoginObj->action_logout(iTestData);

			if (login_status != CTestData::EDISCONNECTED)
				{
					//logout didnot happen successfully
					return_value = KErrGeneral;
				}
			
			//do this later	
		   	/*g_hash_table_foreach_remove ( data_to_search, 
					search_hash_remove, NULL );*/
		
			//Free the hash table itself
			g_hash_table_destroy ( data_to_search );
		
		}
		else
		{
			//Login itself didnot happen.
			return_value =  KErrGeneral;
		}		
		
		free_data ( connect_data, iLog );	
		return return_value;
	}

// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::SearchInvalidKeyTest
// -----------------------------------------------------------------------------

TInt CTelepathyGabbleTest::SearchInvalidKeyTestL( 
    CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data ( connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		

		if (iTestData == NULL)
		{
			//Create the iTestData if it is not existing.
			
			iTestData = CTestData::NewL(iLog);
		}
		iTestData->SetTestType(CTestData::ESearch);
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = 
			iLoginObj->action_login(username,passwd,servername, 
									resource_login,ssl_flag,
									port,proxy_server, proxy_port,
									iTestData );
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login was successful
			
			GHashTable *data_to_search = NULL;
			
			GValue *gvalue = NULL;
		
			char *key, *value;
			
			key = connect_data->msg_data->recipient;
			value = connect_data->msg_data->message;
			
			data_to_search = g_hash_table_new(g_str_hash,g_str_equal);	
			
			gvalue = g_new0(GValue, 1);	
		
					 
			g_value_init(gvalue, G_TYPE_STRING);
			g_value_set_string (gvalue, value );
			//ivalid search key..label has to be passed so no need to pass label 
			g_hash_table_insert( data_to_search, key, gvalue);	
		

			//Create the Object of Send Message class
			iSearchObj = CTelepathyGabbleSearch::NewL();	
			
			//call the SendMessage test case	
			TInt search_state = iSearchObj->SearchL(data_to_search, iTestData);
			
			if( search_state == CTestData::ESearchError )
				{
				    //Error in Searching,
				    //as one of the search key passed is wrong
					return_value = KErrNone; 
				}
			else
				{
					return_value = KErrGeneral; 	
				}
			iSearchObj->CloseChannel(iTestData);		
			//Do the logout
			login_status = iLoginObj->action_logout(iTestData);

			if (login_status != CTestData::EDISCONNECTED)
				{
					//logout didnot happen successfully
					return_value = KErrGeneral;
				}
			
			//do this later	
		   	/*g_hash_table_foreach_remove ( data_to_search, 
					search_hash_remove, NULL );*/
		
			//Free the hash table itself
			g_hash_table_destroy ( data_to_search );
		
		}
		else
		{
			//Login itself didnot happen.
			return_value =  KErrGeneral;
		}		
		
		free_data (connect_data, iLog );	
		return return_value;
	}
	
// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::GetSearchKeysTestL
// -----------------------------------------------------------------------------

TInt CTelepathyGabbleTest::GetSearchKeysTestL( 
    CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		

		if (iTestData == NULL)
			{
			//Create the iTestData if it is not existing.
			
			iTestData = CTestData::NewL(iLog);
			}
		iTestData->SetTestType(CTestData::EGetSearchKeys);
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = 
			iLoginObj->action_login(username,passwd,servername, 
									resource_login,ssl_flag,
									port,proxy_server, proxy_port,
									iTestData );
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login was successful
			
			//Create the Object of Send Message class
			iSearchObj = CTelepathyGabbleSearch::NewL();	
			
			TInt search_state = iSearchObj->GetSearchKeysL( iTestData);
			
			if( search_state == CTestData::ESearchError )
				{
					return_value = KErrGeneral; //Error in Searching
				}
			else if( search_state == CTestData::ESearchKeysFound) 
				{
					return_value = KErrNone; 
				}
			iSearchObj->CloseChannel(iTestData);	
			//Do the logout
			login_status = iLoginObj->action_logout(iTestData);

			if (login_status != CTestData::EDISCONNECTED)
				{
					//logout didnot happen successfully
					return_value = KErrGeneral;
				}
		
		
		}
		else
		{
			//Login itself didnot happen.
			return_value =  KErrGeneral;
		}		
		
		free_data (connect_data, iLog );	
		return return_value;
	}
	

// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::SendMessageTestL
// -----------------------------------------------------------------------------

TInt CTelepathyGabbleTest::SendMessageTestL( 
    CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		

		if (iTestData == NULL)
		{
			//Create the iTestData if it is not existing.
			
			iTestData = CTestData::NewL(iLog);
		}
		
		iTestData->SetTestType(CTestData::ESend);
		
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = 
			iLoginObj->action_login(username,passwd,servername, 
									resource_login,ssl_flag,
									port,proxy_server, proxy_port,
									iTestData );
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login was successful
			//Send a message
			
			//for sending a message
			char *recipient_id, *msg;
			
			
			
			recipient_id = connect_data->msg_data->recipient;
			msg = connect_data->msg_data->message;

			//Create the Object of Send Message class
			iSendMsgObj = CTelepathyGabbleSendRecvMessage::NewL();
			
		
			
			//call the SendMessage test case	
			TInt sendmsg_state = iSendMsgObj->SendMessage(recipient_id,msg,iTestData);
			
			if( sendmsg_state == CTestData::EMessageNotSent )
				{
				return_value = KErrGeneral; //Error in Sending message
				}
			else if( sendmsg_state == CTestData::EMessageSendErr ) 
				{
				    // error was not expected
					return_value = KErrGeneral; 
				}
		    else if( sendmsg_state == CTestData::EMessageSent ) 
				{
					//If message sent to invalid userid or blank userid, state should EMessageSendErr
					// This is used in case of SendMessagetoinvalidUseridTestL
					return_value = KErrNone; 
				}
			
			//Do the logout
			login_status = iLoginObj->action_logout(iTestData);

			if (login_status != CTestData::EDISCONNECTED)
			{
				//logout didnot happen successfully
				return_value = KErrGeneral;
			}
		
		}
		else
		{
			//Login itself didnot happen.
			return_value =  KErrGeneral;
		}		
		
		free_data (connect_data, iLog );	
		return return_value;
	}
	



// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::SendMessageRepeatedTestL
// -----------------------------------------------------------------------------

TInt CTelepathyGabbleTest::SendMessageRepeatedTestL( 
    CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		

		if (iTestData == NULL)
		{
			//Create the iTestData if it is not existing.
			
			iTestData = CTestData::NewL(iLog);
		}
		
		iTestData->SetTestType(CTestData::ESend);
		
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = 
			iLoginObj->action_login(username,passwd,servername, 
									resource_login,ssl_flag,
									port,proxy_server, proxy_port,
									iTestData );
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login was successful
			//Send a message
			
			//for sending a message
			char *recipient_id, *msg;
			
			recipient_id = connect_data->msg_data->recipient;
			msg = connect_data->msg_data->message;

			//Create the Object of Send Message class
			iSendMsgObj = CTelepathyGabbleSendRecvMessage::NewL();
			
			//call the SendMessage test case	
			TInt sendmsg_state = CTestData::EMessageNotSent;
		
			for( TInt cnt =0 ; cnt < KRepeatedMessageCount ; cnt++ )
			{
				sendmsg_state = iSendMsgObj->SendMessage(recipient_id,msg,iTestData);	
				
				if( sendmsg_state != CTestData::EMessageSent )
					{
					return_value = KErrGeneral; //Error in Sending message	
					break;
					}
			}
		
				
			//Do the logout
			login_status = iLoginObj->action_logout(iTestData);

			if (login_status != CTestData::EDISCONNECTED)
			{
				//logout didnot happen successfully
				return_value = KErrGeneral;
			}
		
		}
		else
		{
			//Login itself didnot happen.
			return_value =  KErrGeneral;
		}		
		
		free_data (connect_data, iLog );	
		return return_value;
	}



// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::ReceiveRepeatedMessageTestL
// Test for login.
// -----------------------------------------------------------------------------
	
TInt CTelepathyGabbleTest::ReceiveRepeatedMessageTestL( 
    CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login,*ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
	
		iTestData = CTestData::NewL(iLog);
		
		iTestData->SetTestType(CTestData::EReceive);
	
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = 
			iLoginObj->action_login(username,passwd,
									servername, resource_login,ssl_flag,
									port,proxy_server, proxy_port,
									iTestData);
		
		if (login_status == CTestData::ECONNECTED)
		{
			//Get the contact_id and the message.
			char *recipient_id, *msg;
			
			recipient_id = connect_data->msg_data->recipient;
			msg = connect_data->msg_data->message;
		
		
		  //Do the receive message test case
		  //Create the Object of Send Message class
		  iSendMsgObj = CTelepathyGabbleSendRecvMessage::NewL();
		  
		  TInt recvmsg_state = CTestData::EMessageNotSent;
		
			for( TInt cnt =0 ; cnt < KRepeatedMessageCount ; cnt++ )
			{
				iTestData->SetSendRecvMsgState(CTestData::EMessageNotSent);
				recvmsg_state = iSendMsgObj->ReceiveMessage(recipient_id,msg,iTestData);	
				
				if( recvmsg_state != CTestData::EMessageRecvd )
					{
					return_value = KErrGeneral;	
					break;
					}
			}
		  		  
		  //do the logout and the cleanup
			login_status = iLoginObj->action_logout(iTestData);
			if (login_status != CTestData::EDISCONNECTED)
			{
				//logout didnot happen successfully
				return_value = KErrGeneral;
			}
		  
		}
		else
		{
			//login didnot happen successfully
			return_value = KErrGeneral;
		}
		free_data (connect_data, iLog );
		return return_value;
	}

// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::ReceiveMessageTestL
// Test for login.
// -----------------------------------------------------------------------------
	
TInt CTelepathyGabbleTest::ReceiveMessageTestL( 
    CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login,*ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
	
		iTestData = CTestData::NewL(iLog);
		
		iTestData->SetTestType(CTestData::EReceive);
	
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = 
			iLoginObj->action_login(username,passwd,
									servername, resource_login,ssl_flag,
									port,proxy_server, proxy_port,
									iTestData);
		
		if (login_status == CTestData::ECONNECTED)
		{
			//Get the contact_id and the message.
			char *recipient_id, *msg;
			
			recipient_id = connect_data->msg_data->recipient;
			msg = connect_data->msg_data->message;
		
		    iLog->Log( _L("ReceiveMessageTestL:: Logged In \n") );
		    
		  //Do the receive message test case
		  //Create the Object of Send Message class
		  iSendMsgObj = CTelepathyGabbleSendRecvMessage::NewL();
		  TInt recvmsg_state = iSendMsgObj->ReceiveMessage(recipient_id,msg,iTestData);	
		  
		  if( recvmsg_state != CTestData::EMessageRecvd )
				{
				iLog->Log( _L("ReceiveMessageTestL:: receive Error \n") );
				return_value = KErrGeneral;	
				}
		  //do the logout and the cleanup
		  login_status = iLoginObj->action_logout(iTestData);
		  if (login_status != CTestData::EDISCONNECTED)
				{
					iLog->Log( _L("ReceiveMessageTestL:: Logout Error \n") );
					//logout didnot happen successfully
					return_value = KErrGeneral;
				}
		  
		}
		else
		{
			//login didnot happen successfully
			return_value = KErrGeneral;
		}
		free_data (connect_data, iLog );
		return return_value;
	}


// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::ReceiveMessageBlankTestL
// Test for login.
// -----------------------------------------------------------------------------
	
TInt CTelepathyGabbleTest::ReceiveMessageBlankTestL( 
    CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login,*ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
	
		iTestData = CTestData::NewL(iLog);
	
		iTestData->SetTestType(CTestData::ERecvBlank);
		
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = 
			iLoginObj->action_login(username,passwd,
									servername, resource_login,ssl_flag,
									port,proxy_server, proxy_port,
									iTestData);
		
		if (login_status == CTestData::ECONNECTED)
		{
			iLog->Log( _L("ReceiveMessageBlankTestL:: Logged In \n") );
			//Get the contact_id and the message.
			char *recipient_id, *msg;
			
			recipient_id = connect_data->msg_data->recipient;
			msg = connect_data->msg_data->message;
			
			//Set the message to blamk string
			msg = "";
			
		  //Do the receive message test case
		  //Create the Object of Send Message class
		  iSendMsgObj = CTelepathyGabbleSendRecvMessage::NewL();
		  TInt recvmsg_state = iSendMsgObj->ReceiveMessage(recipient_id,msg,iTestData);	
		  
		  if( recvmsg_state == CTestData::EMessageNotSent  )
				{
				iLog->Log( _L("ReceiveMessageBlankTestL:: EMESSAGE_NOT_SENT \n") );
				return_value = KErrNone; //Error in Sending message
				}
				  
		    iLog->Log( _L("ReceiveMessageBlankTestL:: b4 action_logout \n") );
		   //do the logout and the cleanup
			login_status = iLoginObj->action_logout(iTestData);
			if (login_status != CTestData::EDISCONNECTED)
			{
				iLog->Log( _L("ReceiveMessageBlankTestL:: logout error \n") );
				//logout didnot happen successfully
				return_value = KErrGeneral;
			}
		  
		}
		else
		{
			//login didnot happen successfully
			return_value = KErrGeneral;
		}
		free_data (connect_data, iLog );
		return return_value;
		  
		}
		

// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::ReceiveMessageMaxLengthTestL
// Test for login.
// -----------------------------------------------------------------------------
	
TInt CTelepathyGabbleTest::ReceiveMessageMaxLengthTestL( 
    CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login,*ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
	
		iTestData = CTestData::NewL(iLog);
		
		iTestData->SetTestType(CTestData::EReceive);
	
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = 
			iLoginObj->action_login(username,passwd,
									servername, resource_login,ssl_flag,
									port,proxy_server, proxy_port,
									iTestData);
		
		if (login_status == CTestData::ECONNECTED)
		{
			//Get the contact_id and the message.
			char *recipient_id;
			//char *msg;
			
			recipient_id = connect_data->msg_data->recipient;
			// msg = connect_data->msg_data->message; //not required
		
		 //Set the message to maxlength string( with 400 chars)
			char *max_len_msg = {"abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghij"};
									
		  //Do the receive message test case
		  //Create the Object of Send Message class
		  iSendMsgObj = CTelepathyGabbleSendRecvMessage::NewL();
		  
		  TInt recvmsg_state = iSendMsgObj->ReceiveMessage(recipient_id,max_len_msg,iTestData);	
		  
		  if( recvmsg_state != CTestData::EMessageRecvd )
					{
					return_value = KErrGeneral;	
					}
		   //do the logout and the cleanup
			login_status = iLoginObj->action_logout(iTestData);
			if (login_status != CTestData::EDISCONNECTED)
			{
				//logout didnot happen successfully
				return_value = KErrGeneral;
			}
		  
		}
		else
		{
			//login didnot happen successfully
			return_value = KErrGeneral;
		}
		free_data (connect_data, iLog );
		return return_value;
		  
		}
		
// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::LoginCancelTestL
// Test for Cancelling the login.
// -----------------------------------------------------------------------------
	
TInt CTelepathyGabbleTest::LoginCancelTestL( 
    CStifItemParser& aItem  )
	{
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
		
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
		
		iTestData = CTestData::NewL(iLog);
		
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
			
		//action_login will have parameters 
		//passed from cfg file
		TInt login_status = 
					iLoginObj->action_cancel(username,passwd,
											servername, resource_login,ssl_flag,
											port,proxy_server, proxy_port,
											iTestData );
		
		free_data (connect_data, iLog );
		
		if (login_status == CTestData::EDISCONNECTED )
		{
			//i.e the login didnot happen successfully
			//which is correct, since this is a cancel test case.
			return KErrNone;	
		}
		return KErrGeneral;	 //login status must be disconnceted else an error
	}


// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::LoginInvalidUseridTestL
// Test for login.
// -----------------------------------------------------------------------------
	
TInt CTelepathyGabbleTest::LoginInvalidUseridTestL( CStifItemParser& aItem  )
{
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
		
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
		
		iTestData = CTestData::NewL(iLog);
		
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
			
		//action_login will have parameters 
		//passed from cfg file
		TInt login_status = 
					iLoginObj->action_login(username,passwd,
											servername, resource_login,ssl_flag,
											port,proxy_server, proxy_port,
											iTestData );
		
		free_data (connect_data, iLog );
		
		if (login_status == CTestData::EDISCONNECTED)
		{
			//i.e. login didnot happen with invalid user-id and passwd
			return KErrNone;
		}
		
		return KErrGeneral;	 //login status must be disconnceted else an error
}


// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::LoginInvalidPasswdTestL
// Test for login.
// -----------------------------------------------------------------------------
	
TInt CTelepathyGabbleTest::LoginInvalidPasswdTestL( CStifItemParser& aItem  )
{
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
		
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
		
		iTestData = CTestData::NewL(iLog);
		
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
			
		//action_login will have parameters 
		//passed from cfg file
		TInt login_status = 
					iLoginObj->action_login(username,passwd,
											servername, resource_login,ssl_flag,
											port,proxy_server, proxy_port,
											iTestData );
		
		free_data (connect_data, iLog );
		
		if (login_status == CTestData::EDISCONNECTED)
		{
			//i.e. login didnot happen with invalid user-id and passwd
			return (KErrNone);
		}
		
		return KErrGeneral;	 //login status must be disconnceted else an error
}

// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::LoginInvalidUseridPasswdTestL
// Test for login.
// -----------------------------------------------------------------------------
	
TInt CTelepathyGabbleTest::LoginInvalidUseridPasswdTestL( 
    CStifItemParser& aItem  )
{
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
		
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
		
		iTestData = CTestData::NewL(iLog);
		
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
			
		//action_login will have parameters 
		//passed from cfg file
		TInt login_status = 
					iLoginObj->action_login(username,passwd,
											servername, resource_login,ssl_flag,
											port,proxy_server, proxy_port,
											iTestData );
		
		free_data (connect_data, iLog );
		
		if (login_status == CTestData::EDISCONNECTED)
		{
			//i.e. login didnot happen with invalid user-id and passwd
			return KErrNone;
		}
		
		return KErrGeneral;	 //login status must be disconnceted else an error
}


// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::SendMessageWith400TestL
// Test for login.
// -----------------------------------------------------------------------------

TInt CTelepathyGabbleTest::SendMessageWith400TestL( 
    CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
	    ssl_flag = connect_data->ssl_flag;
	    port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
	    
	    iTestData = CTestData::NewL(iLog);
	    
	    iTestData->SetTestType(CTestData::ESend);
	     
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = 
			iLoginObj->action_login( username,passwd,
									servername, resource_login,ssl_flag,
									port,proxy_server, proxy_port,
									iTestData );
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login was successful
			//Send a message
			
			//for sending a message
			char *recipient_id;
			
			recipient_id = connect_data->msg_data->recipient;
			//msg = connect_data->msg_data->message;
			//hardcode the message of 400 characters.
			//Set the message to maxlength string( with 400 chars)
			char *msg = {"abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghijklmnopqrstuvwxyzabcd  \
							abcdefghij"};

			//Create the Object of Send Message class
			iSendMsgObj = CTelepathyGabbleSendRecvMessage::NewL();
			
			//call the SendMessage test case	
			TInt sendmsg_state = iSendMsgObj->SendMessage(recipient_id,msg,iTestData );
			
			if( sendmsg_state == CTestData::EMessageNotSent || 
				sendmsg_state == CTestData::EMessageSendErr  )
				{
				return_value = KErrGeneral; //Error in Sending message
				}
			else if( sendmsg_state == CTestData::EMessageSent ) 
				{
				    // EMessageSent was expected
					return_value = KErrNone; 
				}
		   	
		
			//Do the logout.
			login_status = iLoginObj->action_logout( iTestData );
			if (login_status != CTestData::EDISCONNECTED)
				{
					//logout didnot happen successfully
					return_value = KErrGeneral;
				}					
		}
		else
		{
			//Login itself didnot happen.
			return_value =  KErrGeneral;
		}
		
		
		free_data (connect_data, iLog );	
		return return_value;
	}



// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::SendMesasgeToBlankUserIdTestL
// Test for login.
// -----------------------------------------------------------------------------
TInt CTelepathyGabbleTest::SendMesasgeToBlankUserIdTestL( 
    CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
		
		
		iTestData = CTestData::NewL(iLog);
		
		iTestData->SetTestType(CTestData::ESendErr);
		
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = iLoginObj->action_login(username,passwd,
								servername, resource_login,ssl_flag,
								port,proxy_server, proxy_port,
								iTestData );
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login was successful
			//Send a message
			
			//for sending a message
			char *recipient_id, *msg;
			
			recipient_id = "";//hardcode the recipient for blank.
		
			msg = connect_data->msg_data->message;

			//Create the Object of Send Message class
			iSendMsgObj = CTelepathyGabbleSendRecvMessage::NewL();
			
			
			//call the SendMessage test case	
			TInt sendmsg_state = iSendMsgObj->SendMessage(recipient_id,msg,iTestData);
			
			if( sendmsg_state == CTestData::EMessageNotSent 
				|| sendmsg_state == CTestData::EMessageSent )
				{
				return_value = KErrGeneral; //Error in Sending message
				}
		    else if( sendmsg_state == CTestData::EMessageSendErr ) 
				{
					//If message sent to invalid userid or blank userid, state should EMessageSendErr
					// This is used in case of SendMessagetoinvalidUseridTestL
					return_value = KErrNone; 
				}
				
			//Do the logout.
			login_status = iLoginObj->action_logout(iTestData);
			if (login_status != CTestData::EDISCONNECTED)
				{
					//logout didnot happen successfully
					return_value = KErrGeneral;
				}
				
			
					
		}
		else
		{
			//Login itself didnot happen.
			return_value =  KErrGeneral;
		}
		
		free_data (connect_data, iLog );	
		return return_value;
	}



// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::SendMesasgeBlankTestL
// Test for login.
// -----------------------------------------------------------------------------
TInt CTelepathyGabbleTest::SendMesasgeBlankTestL( 
    CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
		
		
		iTestData = CTestData::NewL(iLog);
		
		iTestData->SetTestType(CTestData::ESend);
		
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = iLoginObj->action_login(username,passwd,
								servername, resource_login,ssl_flag,
								port,proxy_server, proxy_port,
								iTestData );
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login was successful
			//Send a message
			
			//for sending a message
			char *recipient_id, *msg;
			
			recipient_id = connect_data->msg_data->recipient;
			//msg = connect_data->msg_data->message;
			//hardcode the message for blank.
			msg = "";

			//Create the Object of Send Message class
			iSendMsgObj = CTelepathyGabbleSendRecvMessage::NewL();
			
			//call the SendMessage test case	
			TInt sendmsg_state = iSendMsgObj->SendMessage(recipient_id,msg,iTestData);
			if( sendmsg_state != CTestData::EMessageSent )
			{
			//There was some error while sending the message.
			return_value = KErrGeneral;
			}
			
				
			//Do the logout.
			login_status = iLoginObj->action_logout(iTestData);
			if (login_status != CTestData::EDISCONNECTED)
			{
				//logout didnot happen successfully
				return_value = KErrGeneral;
			}
			
			
					
		}
		else
		{
			//Login itself didnot happen.
			return_value =  KErrGeneral;
		}
		
		free_data (connect_data, iLog );	
		return return_value;
	}
	


// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::SendMessagetoinvalidUseridTestL
// -----------------------------------------------------------------------------

TInt CTelepathyGabbleTest::SendMessagetoinvalidUseridTestL( 
    CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		

		if (iTestData == NULL)
		{
			//Create the iTestData if it is not existing.
			
			iTestData = CTestData::NewL(iLog);
		}
		
		iTestData->SetTestType(CTestData::ESendErr);
		
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = 
			iLoginObj->action_login(username,passwd,servername, 
									resource_login,ssl_flag,
									port,proxy_server, proxy_port,
									iTestData );
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login was successful
			//Send a message
			
			//for sending a message
			char *recipient_id, *msg;
			
			TInt testType = iTestData->GetTestType();
			
			recipient_id = connect_data->msg_data->recipient;
			msg = connect_data->msg_data->message;

			//Create the Object of Send Message class
			iSendMsgObj = CTelepathyGabbleSendRecvMessage::NewL();
			
		
			
			//call the SendMessage test case	
			TInt sendmsg_state = iSendMsgObj->SendMessage(recipient_id,msg,iTestData);
			
			if( sendmsg_state == CTestData::EMessageNotSent ||
					sendmsg_state == CTestData::EMessageSent )
				{
				// EMessageSendErr was expected
				return_value = KErrGeneral; //Error in Sending message
				}
		
		    else if( sendmsg_state == CTestData::EMessageSendErr ) 
				{
					//If message sent to invalid userid or blank userid, state should EMessageSendErr
					// This is used in case of SendMessagetoinvalidUseridTestL
					return_value = KErrNone; 
				}
			
			//Do the logout
			login_status = iLoginObj->action_logout(iTestData);

			if (login_status != CTestData::EDISCONNECTED)
			{
				//logout didnot happen successfully
				return_value = KErrGeneral;
			}
		
		}
		else
		{
			//Login itself didnot happen.
			return_value =  KErrGeneral;
		}		
		
		free_data (connect_data, iLog );	
		return return_value;
	}
	




// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::RemoveContactWithBlankUserIdTestL
// Test for removing blank userid
// -----------------------------------------------------------------------------
	
TInt CTelepathyGabbleTest::RemoveContactWithBlankUserIdTestL( 
    CStifItemParser& aItem  )
{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
		
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
		
		iTestData = CTestData::NewL(iLog);
	
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
			
		//action_login will have parameters 
		//passed from cfg file
		TInt login_status = iLoginObj->action_login(username,passwd,
														servername, resource_login,ssl_flag,
														port,proxy_server, proxy_port,
														iTestData);
		
		
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login happened successfully.
			//Create the AddDelete Contact class object.
			
			//iAddDelContactObj = CTelepathyGabbleAddDeleteContact::NewL();
			iAddDelContactObj = CTelepathyGabbleAddDeleteContact::NewL();
			
			
			//Get the contact_id and the message.
			char *contact_id, *msg;
			
			contact_id = ""; //make the contactid as blank
			msg = connect_data->msg_data->message;
			
			TInt contact_status = iAddDelContactObj->remove_contact( contact_id, msg, iTestData );
			
			if( contact_status == CTestData::EDELETE_FAIL )
			{
				return_value = KErrNone;
			}
			
			//Do the logout and then return KErrNone;
			login_status =	iLoginObj->action_logout(iTestData);
			
			if (login_status != CTestData::EDISCONNECTED)
			{
				return_value = KErrGeneral; //logout was not successful
			}
			
			free_data (connect_data, iLog );
			return return_value;
		}
		
		else
		  {
		  	free_data (connect_data, iLog );
		  	return KErrGeneral;//login was not successful
		  }
}

// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::AddContactWithBlankUserIdTestL
// Test for Adding blank userid
// -----------------------------------------------------------------------------
	
TInt CTelepathyGabbleTest::AddContactWithBlankUserIdTestL( 
    CStifItemParser& aItem  )
{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
		
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
		
		iTestData = CTestData::NewL(iLog);
	
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
			
		//action_login will have parameters 
		//passed from cfg file
		TInt login_status = iLoginObj->action_login(username,passwd,
														servername, resource_login,ssl_flag,
														port,proxy_server, proxy_port,
														iTestData);
		
		
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login happened successfully.
			//Create the AddDelete Contact class object.
			
			//iAddDelContactObj = CTelepathyGabbleAddDeleteContact::NewL();
			iAddDelContactObj = CTelepathyGabbleAddDeleteContact::NewL();
			
			
			//Get the contact_id and the message.
			char *addcontact_id, *msg;
			
			addcontact_id = ""; //make the contactid as blank
			msg = connect_data->msg_data->message;
			
			TInt addcontact_status = iAddDelContactObj->add_contact(addcontact_id,msg, iTestData );
			
			if( addcontact_status == CTestData::EADD_FAIL )
			{
				return_value = KErrNone;
			}
			
			//Do the logout and then return KErrNone;
			login_status =	iLoginObj->action_logout(iTestData);
			
			if (login_status != CTestData::EDISCONNECTED)
			{
				return_value = KErrGeneral; //logout was not successful
			}
			
			free_data (connect_data, iLog );
			return return_value;
		}
		
		else
		  {
		  	free_data (connect_data, iLog );
		  	return KErrGeneral;//login was not successful
		  }
}


// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::AddContactTestL
// Test for Adding Contact
// -----------------------------------------------------------------------------
	
TInt CTelepathyGabbleTest::AddContactTestL( 
    CStifItemParser& aItem  )
{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
		
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
		
		iTestData = CTestData::NewL(iLog);
	
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
			
		//action_login will have parameters 
		//passed from cfg file
		TInt login_status = iLoginObj->action_login(username,passwd,
														servername, resource_login,ssl_flag,
														port,proxy_server, proxy_port,
														iTestData);
		
		
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login happened successfully.
			//Create the AddDelete Contact class object.
			
			//iAddDelContactObj = CTelepathyGabbleAddDeleteContact::NewL();
			iAddDelContactObj = CTelepathyGabbleAddDeleteContact::NewL();
			
			
			//Get the contact_id and the message.
			char *addcontact_id, *msg;
			
			addcontact_id = connect_data->msg_data->recipient;
			msg = connect_data->msg_data->message;
			
			TInt addcontact_status = iAddDelContactObj->add_contact(addcontact_id,msg, iTestData );
			
			if( addcontact_status != CTestData::EADD_SUCCESS )
			{
				return_value = KErrGeneral;
			}
			
			//Do the logout and then return KErrNone;
			login_status =	iLoginObj->action_logout(iTestData);
			
			if (login_status != CTestData::EDISCONNECTED)
			{
				return_value = KErrGeneral; //logout was not successful
			}
			
			free_data (connect_data, iLog );
			return return_value;
		}
		
		else
		  {
		  	free_data (connect_data, iLog );
		  	return KErrGeneral;//login was not successful
		  }
}

// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::AddContactAlreadyInRosterTestL
// Test for Adding Contact which is already added
// -----------------------------------------------------------------------------
	
TInt CTelepathyGabbleTest::AddContactAlreadyInRosterTestL( 
    CStifItemParser& aItem  )
{
		TInt return_value = KErrNone;
		
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
		
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
		
		iTestData = CTestData::NewL(iLog);
	
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
			
		//action_login will have parameters 
		//passed from cfg file
		TInt login_status = iLoginObj->action_login(username,passwd,
														servername, resource_login,ssl_flag,
														port,proxy_server, proxy_port,
														iTestData);
		
		
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login happened successfully.
			//Create the AddDelete Contact class object.
						
			iAddDelContactObj = CTelepathyGabbleAddDeleteContact::NewL();
						
			//Get the contact_id and the message.
			char *addcontact_id, *msg;
			
			addcontact_id = connect_data->msg_data->recipient;
			msg = connect_data->msg_data->message;
			
			TInt addcontact_status = iAddDelContactObj->add_contact(addcontact_id,msg, iTestData );
			
			if( addcontact_status != CTestData::EADD_SUCCESS )
			{
				return_value = KErrGeneral;
				login_status = iLoginObj->action_logout(iTestData);
				if (login_status != CTestData::EDISCONNECTED)
					{
					return_value = KErrGeneral;
			  	}
				
			}
			else
			{
				//The function add_contact resets iAddDeleteContactStatus (in CTestData) to EADD_FAIL
				//to check again that adding a contact already added was successful(EADD_SUCCESS)
				//so no need to set here explicitly

				
				addcontact_status = iAddDelContactObj->add_contact(addcontact_id,msg, iTestData );
				if( addcontact_status != CTestData::EADD_SUCCESS )
					{
					return_value = KErrGeneral;
				  	}
				login_status = iLoginObj->action_logout(iTestData);
				if (login_status != CTestData::EDISCONNECTED)
					{
					return_value = KErrGeneral;
			   		}
			}
			
		free_data (connect_data, iLog );
		return return_value;	
	 }		
	else
		{ 
			free_data (connect_data, iLog ); 
		  return KErrGeneral;	 //login was not successful
		}
	
}



// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::RemoveContactAlreadyRemovedTestL
// Test for removing Contact which is already removed
// -----------------------------------------------------------------------------
	
TInt CTelepathyGabbleTest::RemoveContactAlreadyRemovedTestL( 
    CStifItemParser& aItem  )
{
		TInt return_value = KErrNone; 
		
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
		
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
		
		iTestData = CTestData::NewL(iLog);
	
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
			
		//action_login will have parameters 
		//passed from cfg file
		TInt login_status = iLoginObj->action_login(username,passwd,
														servername, resource_login,ssl_flag,
														port,proxy_server, proxy_port,
														iTestData);
		
		
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login happened successfully.
			//Create the AddDelete Contact class object.
						
			iAddDelContactObj = CTelepathyGabbleAddDeleteContact::NewL();
						
			//Get the contact_id and the message.
			char *contact_id, *msg;
			
			contact_id = connect_data->msg_data->recipient;
			msg = connect_data->msg_data->message;
			
			TInt contact_status = iAddDelContactObj->add_contact( contact_id, msg, iTestData );
			
			if( contact_status != CTestData::EADD_SUCCESS )
			{
				iLog->Log( _L("CTelepathyGabbleTest:: contact not added successfully \n") );
				return_value = KErrGeneral;
				login_status = iLoginObj->action_logout(iTestData);
				if (login_status != CTestData::EDISCONNECTED)
					{
					return_value = KErrGeneral;
			  	}
			  free_data (connect_data, iLog ); 
		    return return_value;	
			}
			
			contact_status = iAddDelContactObj->remove_contact( contact_id, msg, iTestData );
			
			if( contact_status != CTestData::EDELETE_SUCCESS )
			{
				//contact removed first time, so should be removed successfully, otherwise error
				iLog->Log( _L("CTelepathyGabbleTest::First Time  contact not deleted successfully \n") );
				return_value = KErrGeneral;
				login_status = iLoginObj->action_logout(iTestData);
				if (login_status != CTestData::EDISCONNECTED)
					{
					return_value = KErrGeneral;
			  	}
			 }
			else
			{
				//The function remove_contact resets iAddDeleteContactStatus (in CTestData) to EDELETE_FAIL
				//to check again that removing a contact already removed was not successful,
				// so no need to set  here explicitly
				iLog->Log( _L("CTelepathyGabbleTest::First Time  contact deleted successfully \n") );				
				contact_status = iAddDelContactObj->remove_contact(contact_id,msg, iTestData );
				
			
				// because contact to be removed was already deleted, so status should be EDELETE_FAIL
				if( contact_status != CTestData::EDELETE_FAIL )
					{
					iLog->Log( _L("CTelepathyGabbleTest::Second Time deleted successfully, it should give some error \n") );
					return_value = KErrGeneral; 
				  	}
				login_status = iLoginObj->action_logout(iTestData);
				if (login_status != CTestData::EDISCONNECTED)
					{
						iLog->Log( _L("CTelepathyGabbleTest::Error in logout \n") );
					return_value = KErrGeneral;
			  		}
			}
			
		free_data (connect_data, iLog );
		return return_value;	
	 }		
	else
		{ 
			free_data (connect_data, iLog ); 
		  return KErrGeneral;	 //login was not successful
		}
	
}

// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::RemoveContactTestL
// Test for removing a contact
// -----------------------------------------------------------------------------

TInt CTelepathyGabbleTest::RemoveContactTestL( 
    CStifItemParser& aItem  )
{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		char *username, *passwd , *servername , *resource_login,*ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
		
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
		
		iTestData = CTestData::NewL(iLog);
	
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
			
		//action_login will have parameters 
		//passed from cfg file
		TInt login_status = iLoginObj->action_login(username,passwd,
											servername, resource_login,ssl_flag,
											port,proxy_server, proxy_port,
											iTestData );
		
		
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login happened successfully.
			//Create the AddDelete Contact class object.
			
			
			iAddDelContactObj = CTelepathyGabbleAddDeleteContact::NewL();
			
			
			//Get the contact_id and the message.
			char *contact_id, *msg;
			
			contact_id = connect_data->msg_data->recipient;
			msg = connect_data->msg_data->message;
			
			TInt removecontact_status = iAddDelContactObj->remove_contact( contact_id, msg,	iTestData);
			
			if( removecontact_status != CTestData::EDELETE_SUCCESS )
			{
				return_value = KErrGeneral;
			}
			
			//Do the logout and then return KErrNone;
			login_status =	iLoginObj->action_logout(iTestData);
			
			if (login_status != CTestData::EDISCONNECTED)
			{
				return_value = KErrGeneral; //logout was not successful
			}
			
			free_data (connect_data, iLog );
			return return_value;
		}
		
		else
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
}

// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::RemoveInvalidContactTestL
// Test for removing invalid userid
// -----------------------------------------------------------------------------

TInt CTelepathyGabbleTest::RemoveInvalidContactTestL( 
    CStifItemParser& aItem  )
    
{	
	TInt err = RemoveContactTestL(aItem);
	if ( err == KErrGeneral
	 				&&	 iTestData->GetAddDeleteContactStatus() == CTestData::EDELETE_FAIL )
	{
		// This is correct, since the contact_id was invalid
		return KErrNone;
	}
	
	return KErrGeneral;
	
}
  
// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::FetchContactsTestL
// Test for fetching contacts
// -----------------------------------------------------------------------------

TInt CTelepathyGabbleTest::FetchContactsTestL( 
    CStifItemParser& aItem  )
{
	  TInt return_value = KErrNone;
	  //Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data (connect_data, iLog );
			return KErrGeneral;
		}
		

		char *username, *passwd , *servername , *resource_login,*ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
		
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		
		
		iTestData = CTestData::NewL(iLog);
	
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
			
		//action_login will have parameters 
		//passed from cfg file
		TInt login_status = 
					iLoginObj->action_login(username,passwd,
											servername, resource_login,ssl_flag,
											port,proxy_server, proxy_port,
											iTestData);
		
		if (login_status == CTestData::ECONNECTED)
		{
		
		  	//if successfully logged in
		  	//fetch the contacts
		  
		  	iFetchContactObj = CTelepathyGabbleFetchContacts::NewL();
		  						
			return_value = iFetchContactObj->test_request_roster( iTestData );
			
			//Do the logout and the cleanup.
			login_status = iLoginObj->action_logout(iTestData);
			if (login_status != CTestData::EDISCONNECTED)
				{
				 	return_value = KErrGeneral; //there was error in logout
				}
		 
		}
		else
		{
			return_value = KErrGeneral; //login didn't happen successfully
		}
		
		free_data (connect_data, iLog );
		return return_value;
			
}


// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::SearchTwiceTestL
// -----------------------------------------------------------------------------

TInt CTelepathyGabbleTest::SearchTwiceTestL( CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data ( connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		

		if (iTestData == NULL)
		{
			//Create the iTestData if it is not existing.
			
			iTestData = CTestData::NewL(iLog);
		}
		iTestData->SetTestType(CTestData::ESearch);
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = 
			iLoginObj->action_login(username,passwd,servername, 
									resource_login,ssl_flag,
									port,proxy_server, proxy_port,
									iTestData );
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login was successful
			
			GHashTable *data_to_search = NULL;
			
			GValue *gvalue = NULL;
		
			char *key, *value;
			
			key = connect_data->msg_data->recipient;
			value = connect_data->msg_data->message;
			
			data_to_search = g_hash_table_new(g_str_hash,g_str_equal);	
			
			gvalue = g_new0(GValue, 1);	
		
					 
			g_value_init(gvalue, G_TYPE_STRING);
			g_value_set_string (gvalue, value );
			g_hash_table_insert( data_to_search, GetSearchLabelL(servername,key), gvalue);			      
			
		

			//Create the Object of Send Message class
			iSearchObj = CTelepathyGabbleSearch::NewL();	
			
					
			//call the SendMessage test case	
			TInt search_state = iSearchObj->SearchL(data_to_search, iTestData);
			
			if( search_state == CTestData::ESearchError )
				{
					return_value = KErrGeneral; //Error in Searching
				}
			else if( search_state == CTestData::ESearchCompleted) 
				{
					search_state = iSearchObj->SearchL(data_to_search, iTestData);
					if( search_state == CTestData::ESearchError )
						{
						return_value = KErrGeneral; //Error in Searching	
						}
					//return_value = KErrNone; 
				}
			
			iSearchObj->CloseChannel(iTestData);
			//Do the logout
			login_status = iLoginObj->action_logout(iTestData);

			if (login_status != CTestData::EDISCONNECTED)
				{
					//logout didnot happen successfully
					return_value = KErrGeneral;
				}
			
			//do this later	
		   	/*g_hash_table_foreach_remove ( data_to_search, 
					search_hash_remove, NULL );*/
		
			//Free the hash table itself
			g_hash_table_destroy ( data_to_search );
		
		}
		else
		{
			//Login itself didnot happen.
			return_value =  KErrGeneral;
		}		
		
		free_data ( connect_data, iLog );	
		return return_value;
	}
	


// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::SearchFiveTimesTestL
// -----------------------------------------------------------------------------

TInt CTelepathyGabbleTest::SearchFiveTimesTestL( CStifItemParser& aItem  )
	{
		TInt return_value = KErrNone;
		//Pass the parameters to action_login() from the 
		//data mentioned in the cfg file.
		
		//step 1: Do the login.
		ConnectData  *connect_data = NULL;
		
		if ( read_data ( connect_data, aItem, iLog ) != RC_OK )
		{
			free_data ( connect_data, iLog );
			return KErrGeneral;
		}
		

		//for login
		char *username, *passwd , *servername , *resource_login, *ssl_flag, *port;
		char *proxy_server = NULL;
		char *proxy_port = NULL;
		
	
		username = 	connect_data->username;
		passwd   =  connect_data->password;
		servername = 	connect_data->server;
		resource_login = connect_data->resource;
		ssl_flag = connect_data->ssl_flag;
		port = connect_data->port;
		#ifdef __WINSCW__
	  		proxy_server = connect_data->proxy_data->proxy_server;
			proxy_port = connect_data->proxy_data->proxy_port;
		#endif
		

		if (iTestData == NULL)
		{
			//Create the iTestData if it is not existing.
			
			iTestData = CTestData::NewL(iLog);
		}
		iTestData->SetTestType(CTestData::ESearch);
		//Create the login object
		iLoginObj = CTelepathyGabbleLoginTest::NewL();
		
		//action_login will have parameters 
		//passed from cfg file
		
		TInt login_status = 
			iLoginObj->action_login(username,passwd,servername, 
									resource_login,ssl_flag,
									port,proxy_server, proxy_port,
									iTestData );
		
		if (login_status == CTestData::ECONNECTED)
		{
			//login was successful
			
			GHashTable *data_to_search = NULL;
			
			GValue *gvalue = NULL;
		
			char *key, *value;
			
			key = connect_data->msg_data->recipient;
			value = connect_data->msg_data->message;
			
			data_to_search = g_hash_table_new(g_str_hash,g_str_equal);	
			
			gvalue = g_new0(GValue, 1);	
		
					 
			g_value_init(gvalue, G_TYPE_STRING);
			g_value_set_string (gvalue, value );
			g_hash_table_insert( data_to_search, GetSearchLabelL(servername,key), gvalue);			      
			
		

			//Create the Object of Send Message class
			iSearchObj = CTelepathyGabbleSearch::NewL();	
			
			TInt search_state;	
				
			for(TInt i = 0; i<5 ;i++ )
				{
					search_state = iSearchObj->SearchL(data_to_search, iTestData);
					if( search_state == CTestData::ESearchError )
						{
						return_value = KErrGeneral; //Error in Searching
						break;	
						}
				}
			iSearchObj->CloseChannel(iTestData);
			//Do the logout
			login_status = iLoginObj->action_logout(iTestData);

			if (login_status != CTestData::EDISCONNECTED)
				{
					//logout didnot happen successfully
					return_value = KErrGeneral;
				}
			
			//do this later	
		   	/*g_hash_table_foreach_remove ( data_to_search, 
					search_hash_remove, NULL );*/
		
			//Free the hash table itself
			g_hash_table_destroy ( data_to_search );
		
		}
		else
		{
			//Login itself didnot happen.
			return_value =  KErrGeneral;
		}		
		
		free_data ( connect_data, iLog );	
		return return_value;
	}
	


// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::GetSearchLabelForYukonL
// -----------------------------------------------------------------------------
gchar* CTelepathyGabbleTest::GetSearchLabelForYukonL(gchar *search_key)
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
// CTelepathyGabbleTest::GetSearchLabelForGizmoL
// -----------------------------------------------------------------------------
gchar* CTelepathyGabbleTest::GetSearchLabelForGizmoL(gchar *search_key)
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
// CTelepathyGabbleTest::GetSearchLabelL
// -----------------------------------------------------------------------------
gchar* CTelepathyGabbleTest::GetSearchLabelL(gchar* aService,gchar* aSearchKey)
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
	
  	

/** 
* Free function for the  hash key/value pairs
*/
/*gboolean search_hash_remove( gpointer key, gpointer value,
                      gpointer user_data ) 
	{

	//Free function for the  hash values
	if ( key ) 
		{
		free( key );
		}
	if ( value ) 
		{
		free ( value );
		}
	
	return TRUE;
	}
*/
//End of File