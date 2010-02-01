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
* Description:    Used for Login-Logout related Test Cases
*
*/



 
//Include files
#include <e32err.h>
#include <e32const.h>
#include <e32base.h>

#include <glib.h>
#include <StifLogger.h>

#include "telepathygabbleutils.h"
#include "telepathygabblelogintest.h"
#include "telepathygabbletestdata.h"
#include "telepathygabbleadddeletecontact.h"

static void  test_log_handler1 (
		    const gchar    *message
		   )
{
   FILE* fp;
		
	fp = fopen("c:\\tplogs.txt","a");
	if(fp)
	{
		fprintf(fp,message);
		fprintf(fp,"\n");
		fclose(fp);
	}

	
}


//-----------------------------------------------------------------------------
// function_name	: CTelepathyGabbleLoginTest
// description     	: Constructor
// Returns          : None
//-----------------------------------------------------------------------------

CTelepathyGabbleLoginTest::CTelepathyGabbleLoginTest()
    {
    }
    
//-----------------------------------------------------------------------------
// function_name	: ConstructL
// description     	: Constructor
// Returns          : None
//-----------------------------------------------------------------------------

void CTelepathyGabbleLoginTest::ConstructL()
	{

	}

//-----------------------------------------------------------------------------
// function_name	: NewL
// description     	: Constructor
// Returns          : None
//-----------------------------------------------------------------------------
CTelepathyGabbleLoginTest* CTelepathyGabbleLoginTest::NewL()
    {
    
	CTelepathyGabbleLoginTest* self = new(ELeave) CTelepathyGabbleLoginTest;
    CleanupStack::PushL( self );
    self->ConstructL();    
    CleanupStack::Pop();
    return self;
    }

//-----------------------------------------------------------------------------
// function_name	: ~CTelepathyGabbleLoginTest
// description     	: destructor
// Returns          : None
//-----------------------------------------------------------------------------
CTelepathyGabbleLoginTest::~CTelepathyGabbleLoginTest()
    {
    
    //class variables
    //delete conn;
    //conn = NULL;

    }

//-----------------------------------------------------------------------------
// function_name	: action_login
// description     	: login
// Returns          : gboolean
//----------------------------------------------------------------------------- 
TInt CTelepathyGabbleLoginTest::action_login(char* username,char* password,
												char* server, char* resource, char* ssl_flag,
												char *port, char *proxy_server, char *proxy_port,
												CTestData* aTestData)
{
   
   
    TpConnMgr *connmgr = NULL;
    GError *error = NULL;
    
    test_log_handler1(username);
    test_log_handler1(password);
    test_log_handler1(server);
    test_log_handler1(resource);
    test_log_handler1(ssl_flag);
    test_log_handler1(port);
    
   	
	GHashTable *connection_parameters = g_hash_table_new(g_str_hash, g_str_equal);

	DBusGConnection *dbus_connection;
	TpConn *tp_conn;
	
	//Use the values passed from the cfg file.
	gchar* def1 = g_strdup(username);
	gchar* def2 = g_strdup(password);
	
	gchar* def3 = g_strdup(server);	
	gchar* def4 = g_strdup(resource);
	
	guint def5 = atoi(port);				//443;

	#ifdef __WINSCW__	
	gchar* def6 = g_strdup(proxy_server); 		//g_strdup("172.16.42.135"); //for emulater
	guint def7 = atoi(proxy_port);												//8080; //for emulater
	#endif

	GValue *value1 = g_new0(GValue, 1);
	GValue *value2 = g_new0(GValue, 1);
	GValue *value3 = g_new0(GValue, 1);
	GValue *value4 = g_new0(GValue, 1);
	GValue *value5 = g_new0(GValue, 1);
		
	#ifdef __WINSCW__	
	GValue *value6 = g_new0(GValue, 1);
	GValue *value7 = g_new0(GValue, 1);
	#endif

  GValue *value8 = g_new0(GValue, 1);	
	g_type_init();


  	// just use system bus and do not bother about others
  	 dbus_connection = dbus_g_bus_get( DBUS_BUS_SESSION , &error); 
  
  	aTestData->GetStifLogger()->Log( _L("after dbus_g_bus_get") );
    /* bsr 4 sep: this was null as host parsing failed -> transport was zero -> connectionw as 0 */
  	if (dbus_connection == NULL)
   	{
	    // if we get error as part of above step, connection variable is NULL, so we are here
	    //_dbus_verboseerr("Failed to open connection to bus: %s\n", error->message);
	    //test_log_handler("Found Error in dbus_g_bus_get exiting.\n");
	    aTestData->GetStifLogger()->Log( _L("dbus_connection == NULL") );
	    g_error_free(error);
	 	return (aTestData->GetConnectionState());	
   	}
   	else
   	{
   	aTestData->SetDBusConnection(dbus_connection);
   	}
   
  	aTestData->GetStifLogger()->Log( _L("b4 tp_connmgr_new") );
  	/* Create a connection manager object */
  	connmgr = tp_connmgr_new(dbus_connection, CONNMGR_BUS, CONNMGR_PATH,
			   /*CONNMGR_BUS*/ TP_IFACE_CONN_MGR_INTERFACE);
  	aTestData->GetStifLogger()->Log( _L("after tp_connmgr_new") );
	if (connmgr == NULL)
	{
	 aTestData->GetStifLogger()->Log( _L("connmgr == NULL") );
	 g_error("Failed to create a connection manager, skipping manager startup.");
	 return (aTestData->GetConnectionState());	
	}
	else
	{
		aTestData->SetTpConnMgr(connmgr);	
	}
		
	g_value_init(value1, G_TYPE_STRING);
	g_value_set_string (value1, def1);
	g_hash_table_insert(connection_parameters, (gpointer)"account", (gpointer)value1); 

	g_value_init(value2, G_TYPE_STRING); 
	g_value_set_string (value2, def2);
	g_hash_table_insert(connection_parameters,(gpointer) "password",(gpointer) value2); 

	g_value_init(value3, G_TYPE_STRING); 
	g_value_set_string (value3, def3);
	g_hash_table_insert(connection_parameters,(gpointer) "server",(gpointer)value3 ); 

	g_value_init(value4, G_TYPE_STRING); 
	g_value_set_string (value4, def4);
	g_hash_table_insert(connection_parameters, (gpointer)"resource", (gpointer)value4); 


	g_value_init(value5, G_TYPE_UINT); 
	g_value_set_uint (value5, def5);
	g_hash_table_insert(connection_parameters, (gpointer)"port", (gpointer)value5); 

	//convert the ssl_flag to ascii
	TInt ssl_bool = atoi(ssl_flag);
	
	//if (ssl_flag == "1") //def8 ==1 
	if (ssl_bool == 1) //i.e. only for gtalk, not for gizmo
	{
		gboolean def8 = TRUE; //for gtalk
		g_value_init(value8, G_TYPE_BOOLEAN);
		g_value_set_boolean (value8, def8);
  	g_hash_table_insert(connection_parameters, (gpointer)"old-ssl", (gpointer)value8); 
	}  
	
#ifdef __WINSCW__    
	g_value_init(value6, G_TYPE_STRING); 
	g_value_set_string (value6, def6);
	g_hash_table_insert(connection_parameters, (gpointer)"https-proxy-server", (gpointer)value6);


	g_value_init(value7, G_TYPE_UINT); 
	g_value_set_uint (value7, def7);
	g_hash_table_insert(connection_parameters, (gpointer)"https-proxy-port",(gpointer) value7); 
#endif
	
		
	// Create a new actual connection with the connection manager 
	//Ensure that conn is global. 
	// The same conn object is used for logout.
	aTestData->GetStifLogger()->Log( _L("b4 tp_connmgr_new_connection") );
	tp_conn = tp_connmgr_new_connection(connmgr,connection_parameters, PROTOCOL);
	aTestData->GetStifLogger()->Log( _L("after tp_connmgr_new_connection") );
	if( tp_conn == NULL )
  	{
  		aTestData->GetStifLogger()->Log( _L("tp_conn is null") );
  		return (aTestData->GetConnectionState());	
  	}
  	else
  	{
  	aTestData->SetTpConn(tp_conn);	
  	}
  
		
   	
	// add a callback for StatusChange signal 
	dbus_g_proxy_connect_signal(DBUS_G_PROXY(tp_conn), "StatusChanged",
	(G_CALLBACK(status_changed_cb)),
	aTestData,NULL );

	g_hash_table_destroy(connection_parameters);

	//Run the mainloop
	g_main_loop_run (aTestData->GetMainLoop());

	//Quit of main loop happened in thecallback.
	//unref this mainloop
	// same main loop can be used for all classes,
	// unref after logout in each test case when logintest object gets destroyed
	//g_main_loop_unref ( mainloop ); 
	
	//After the status_changed_cb , the control back here
	//Check the value of global variable that was updated
	//in the callback and return the same.

	
	//Get the connected state for login and return the same.
	return (aTestData->GetConnectionState());
}

//-----------------------------------------------------------------------------
// function_name	: action_logout
// description     	: Does logout
// Returns          : None
//-----------------------------------------------------------------------------

TInt CTelepathyGabbleLoginTest::action_logout(CTestData* aTestData)
{
	GError *error = NULL;
	
	//Do the disconnect
	tp_conn_disconnect (DBUS_G_PROXY( aTestData->GetTpConn()), &error);
	
	//g_hash_table_destroy(global_data_ptr->Getptrhashtable_textchan());
	
	//Start the main loop
	g_main_loop_run (aTestData->GetMainLoop());
	

	//After this see that the callback status_changed_cb
	//is hit with TP_CONN_STATUS_DISCONNECTED as the status.
	
	//After the status_changed_cb , the control back here
	//Check the value of global variable that was updated in the 
	//callback and return the same.
	
	if (error)
	{
		// There is some error
		g_error_free (error);	
		// Update the global to connected 
		//global_connected = TRUE;
	}
	//else global_connected is made FALSE in the status_changed_cb
      
	
	//return global_connected; 
	//Get the connected state for login and return the same.
	return (aTestData->GetConnectionState());
}

//-----------------------------------------------------------------------------
// function_name	: action_cancel
// description     	: cancel login
// Returns          : None
//-----------------------------------------------------------------------------

TInt CTelepathyGabbleLoginTest::action_cancel(char* username,
													char* password, char* server,char* resource, char* ssl_flag,
													char *port,char *proxy_server, char *proxy_port,
												    CTestData* aTestData)
{
	aTestData->SetTestType(CTestData::ECancel) ;
	aTestData->GetStifLogger()->Log( _L("inside action_cancel\n") );
	action_login(username,password,server,resource,ssl_flag,
								port, proxy_server, proxy_port, aTestData);
	
	action_logout(aTestData);//Do tp_disconnected.
	
	return (aTestData->GetConnectionState());
}

//-----------------------------------------------------------------------------
// function_name	: status_changed_cb
//-----------------------------------------------------------------------------
gboolean CTelepathyGabbleLoginTest::status_changed_cb(DBusGProxy* /*proxy*/,
				  guint status, guint reason,
				  gpointer user_data)
{
  GHashTable*  textChannelsHT = NULL;
  CTestData* testData = static_cast<CTestData*> (user_data);
  CStifLogger* logger = testData->GetStifLogger();
  logger->Log( _L("status_changed_cb\n") );

  if (status == TP_CONN_STATUS_CONNECTED ) 
  {
  	
 
    logger->Log( _L("status_changed_cb::connected\n") );
    
    
    if( testData->GetTestType() != CTestData::ELogin )
    {
    
    
 	textChannelsHT =
		g_hash_table_new_full(g_str_hash,
				      g_str_equal,
				      (GDestroyNotify) g_free,
				      (GDestroyNotify) g_object_unref);
	
	testData->SetTextChannelsHT(textChannelsHT);
	//Register the callback for new channel
	//Commented for now has been put in the SendMessage function,
	dbus_g_proxy_connect_signal(DBUS_G_PROXY(testData->GetTpConn()), "NewChannel",
			      (G_CALLBACK(new_channel_handler)),			      
			      user_data,NULL);
	}
	else
	{
		// if the test case is login related test case, new channel callback not required
		//so dont register for NewChannel
	g_main_loop_quit(testData->GetMainLoop());
	testData->ResetLoginCompleteFlag();		      
	}
    //update the state_of_conenction enum
  testData->SetConnectionState(CTestData::ECONNECTED );
   
  }
  else if (status == TP_CONN_STATUS_CONNECTING)
  {
    //if the test case is for cancel disconnect here, meaning cancel the login here.
  	//if cancel test case, do disconnect in connecting state
  	testData->SetConnectionState(CTestData::ECONNECTING);
  	if ( testData->GetTestType() == CTestData::ECancel)
  	{
  		//quit from this callback so that we return to point from where
  		//action_login was called.
  		g_main_loop_quit(testData->GetMainLoop());	
  	}
  	
  }
  else if ( status == TP_CONN_STATUS_DISCONNECTED )
  {
  	
  	
  	if ( testData->GetTestType() == CTestData::ECancel) 
  	{
  		//this disconnect was initiated from the cancel test case
  		logger->Log( _L("status_changed_cb :cancelled \n") );
  		testData->SetConnectionState(CTestData::EDISCONNECTED);
  		//quit the main loop.
  		g_main_loop_quit(testData->GetMainLoop());
  	}
  	else
  	{
  	
  		if(reason == TP_CONN_STATUS_REASON_AUTHENTICATION_FAILED)
  		{
  			// Here disconnect is received as the authentication has failed.
	  		//it could be because of invalid user_id or invalid passwd issued 
	  		//while passing the login parameters.
	  		//Here we are checking for TP_CONN_STATUS_REASON_AUTHENTICATION_FAILED since
	  		// the gabblelogs show the reason value as 3 if invalid user_id and passwd are given.
	  	
	  		testData->SetConnectionState(CTestData::EDISCONNECTED);
	  		
	  		//Quit the main loop started in action_login.
			g_main_loop_quit(testData->GetMainLoop());
			
  		}
  		else
  		{
  			//this was initiated in the logout testcase,
	  		//update the state_of_connection
	  		//LOGOUT AFTER SUCCESSFUL LOGIN
  			
  			if( testData->GetConnectionState() != CTestData::EDISCONNECTED )
  			{
  		
  			logger->Log( _L("status_changed_cb::not disconnected\n") );
  			
  			testData->IncLoginCompleteFlag();
  		
			//currently for all the reasons except TP_CONN_STATUS_REASON_AUTHENTICATION_FAILED
			//disconencted will come here.
			//This code can be updated for variuos "reason" values
			//based on the scenarios and the need for the test cases for those scenarios. 	
  			}
  			
  			if( testData->GetLoginCompleteFlag() && testData->GetConnectionState() == CTestData::EDISCONNECTED  )
  			{
  			logger->Log( _L("status_changed_cb::disconnected\n") );
  			testData->ResetLoginCompleteFlag();
  			//Quit the main loop started in action_login.
			g_main_loop_quit(testData->GetMainLoop());	
				
  			}
  			logger->Log( _L("status_changed_cb::before SetConnectionState\n") );
  			testData->SetConnectionState(CTestData::EDISCONNECTED);
	 	
  		}
  	}
  	
  }
  logger->Log( _L("status_changed_cb out\n") ); 
  return TRUE;
}

//-----------------------------------------------------------------------------
// function_name	: new_channel_handler
// description     	: callback function for NewChannel signal
//-----------------------------------------------------------------------------
void CTelepathyGabbleLoginTest::new_channel_handler(DBusGProxy* /*proxy*/, const char *object_path,
				const char *channel_type, guint handle_type,
				guint channel_handle, gboolean /*suppress_handler*/,
				gpointer user_data)
{
CTestData* testData = static_cast<CTestData*> (user_data);
CStifLogger* logger = testData->GetStifLogger();
logger->Log( _L("new_channel_handler\n") );

//Use the class data member new_text_channel.
TpChan *new_chan;

//Get the new_chan 
new_chan = tp_chan_new(testData->GetDBusConnection(),
	       CONNMGR_BUS, object_path,
		       channel_type, handle_type, channel_handle);


//channel_type == TP_IFACE_CHANNEL_TYPE_TEXT
if ((strcmp(channel_type, TP_IFACE_CHANNEL_TYPE_TEXT) == 0)  )
	{
	g_hash_table_insert(testData->GetTextChannelsHT(), g_strdup(object_path), new_chan);		       
	//test_log_handler( "b4 text_channel_init" );
	//check for  new_chan objects for text channel should be loaclly stored/freed
	text_channel_init( new_chan );
	}
	
	
//channel_type == TP_IFACE_CHANNEL_TYPE_CONTACT_LIST
if( strcmp( channel_type, TP_IFACE_CHANNEL_TYPE_CONTACT_LIST  ) == 0 
 		&& (g_strrstr(object_path, "RosterChannel/subscribe") ) )
 		  
	{
	DBusGProxy* groupInterface = NULL;
	//Get the group_iface
	groupInterface =
     	tp_chan_get_interface(new_chan,
				      TELEPATHY_CHAN_IFACE_GROUP_QUARK);

	
	testData->SetGroupSubscribeInterface(groupInterface);
	
	//register the callback, commented for now.
	dbus_g_proxy_connect_signal (groupInterface, "MembersChanged",
					     G_CALLBACK (CTelepathyGabbleAddDeleteContact::roster_members_changed_cb ),
					     NULL, NULL); 
					     
	//add_contact("testcm6666@gmail.com",NULL);		
    
    testData->IncLoginCompleteFlag();
				       
	}

if( strcmp( channel_type, TP_IFACE_CHANNEL_TYPE_CONTACT_LIST  ) == 0 
 		&& (g_strrstr(object_path, "RosterChannel/publish") ) )
 		  
	{
	
	DBusGProxy* groupInterface = NULL;
	//Get the group_iface
	groupInterface ==
     	tp_chan_get_interface(new_chan,
				      TELEPATHY_CHAN_IFACE_GROUP_QUARK);
				      
	testData->SetGroupPublishInterface(groupInterface);
		
	dbus_g_proxy_connect_signal (groupInterface, "MembersChanged",
					     G_CALLBACK (CTelepathyGabbleAddDeleteContact::roster_members_changed_cb ),
					     NULL, NULL); 
					     
					     
				     
	//Increment the flag.
	testData->IncLoginCompleteFlag();
					     
 	}      
	
if( strcmp( channel_type, TP_IFACE_CHANNEL_TYPE_CONTACT_LIST  ) == 0 
 		&& (g_strrstr(object_path, "RosterChannel/known") ) )
 		  
	{
 	DBusGProxy* groupInterface = NULL;
	//Get the group_iface
	groupInterface = 	tp_chan_get_interface(new_chan,
				      TELEPATHY_CHAN_IFACE_GROUP_QUARK);				      

	
	//remove_contact( "testcm6666@gmail.com", NULL  );	
	testData->SetGroupKnownInterface(groupInterface);
				      
	dbus_g_proxy_connect_signal (groupInterface, "MembersChanged",
					     G_CALLBACK (CTelepathyGabbleAddDeleteContact::roster_members_changed_cb ),
					     NULL, NULL);

	
	//increment the flag
	testData->IncLoginCompleteFlag();
	
	}
	
if( strcmp( channel_type, TP_IFACE_CHANNEL_TYPE_CONTACT_LIST  ) == 0 
 		&& (g_strrstr(object_path, "RosterChannel/deny") ) )
 		  
	{
 	DBusGProxy* groupInterface = NULL;
	//Get the group_iface
	groupInterface =   	tp_chan_get_interface(new_chan,
				      TELEPATHY_CHAN_IFACE_GROUP_QUARK);				      
				      
	testData->SetGroupDenyInterface(groupInterface);
				      
	dbus_g_proxy_connect_signal (groupInterface, "MembersChanged",
					     G_CALLBACK (CTelepathyGabbleAddDeleteContact::roster_members_changed_cb ),
					     NULL, NULL);

	//remove_contact( "testcm6666@gmail.com", NULL  );	

	//increment the flag
	testData->IncLoginCompleteFlag();
	
	}
	//Quit the mainloop started once all the channel-types are recieved.
	//check the flag.
	
	if (testData->GetLoginCompleteFlag() == 3) 
	//In case of gizmo, the number of channels is 3.gtalk for deny also we get the callback
	//we must also store the server name in the login class and check  the server name here
	{
		//Quit the main loop.
		testData->ResetLoginCompleteFlag();
		testData->GetStifLogger()->Log( _L("new_channel_handler::after ResetLoginCompleteFlag\n") );
		g_main_loop_quit(testData->GetMainLoop());
	
	}
		

}

//-----------------------------------------------------------------------------
// function_name	: text_channel_init
// description     	: To get pending messages after login
//-----------------------------------------------------------------------------

void CTelepathyGabbleLoginTest::text_channel_init  ( TpChan *text_chan )
	{
	GPtrArray  *messages_list ;
	guint       i;
	GArray     *message_ids;
	DBusGProxy *text_iface;

		
    text_iface = tp_chan_get_interface(text_chan ,
					   TELEPATHY_CHAN_IFACE_TEXT_QUARK);

		
	//check if need to fetch the pending message or simply register for receive only
	tp_chan_type_text_list_pending_messages(text_iface,
						FALSE,
						&messages_list,
						NULL);


	message_ids = g_array_new (FALSE, TRUE, sizeof (guint));

		
	for (i = 0; i < messages_list->len ; i++)
	 {
		guint          message_id;
		guint          timestamp;
		guint          from_handle;
		guint          message_type;
		guint          message_flags;
		const gchar   *message_body;
		GValueArray   *message_struct;
	
		
		
		message_struct = (GValueArray *) g_ptr_array_index (messages_list, i);
		
		message_id = g_value_get_uint(g_value_array_get_nth(message_struct, 0));

		timestamp = g_value_get_uint(g_value_array_get_nth(message_struct, 1));

		from_handle = g_value_get_uint(g_value_array_get_nth(message_struct, 2));

		message_type = g_value_get_uint(g_value_array_get_nth(message_struct, 3));

		message_flags = g_value_get_uint(g_value_array_get_nth(message_struct, 4));

		message_body = g_value_get_string(g_value_array_get_nth(message_struct, 5));

		g_array_append_val (message_ids, message_id);

	}


	tp_chan_type_text_acknowledge_pending_messages(text_iface, message_ids,
						       NULL);

					

}

				
						
//End of file