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
* Description:    Used for Search Related Test Cases
*
*/



 
//Include files
#include <e32err.h>
#include <e32const.h>
#include <e32base.h>
#include <glib.h>
#include <TestScripterInternal.h>
#include "telepathygabbleutils.h"
#include "telepathygabblesearch.h"
#include "telepathygabbletestdata.h"
#include "tp-chan.h"
#include "tp-conn-gen.h"




//-----------------------------------------------------------------------------
// function_name	: CTelepathyGabbleSearch
// description     	: constructor
//----------------------------------------------------------------------------- 

CTelepathyGabbleSearch::CTelepathyGabbleSearch()
{
}

//-----------------------------------------------------------------------------
// function_name	: ConstructL
// description     	: constructor
//----------------------------------------------------------------------------- 
void CTelepathyGabbleSearch::ConstructL()
{
	
}

//-----------------------------------------------------------------------------
// function_name	: NewL
// description     	: NewL
//----------------------------------------------------------------------------- 
CTelepathyGabbleSearch* CTelepathyGabbleSearch::NewL()
    {
	CTelepathyGabbleSearch* self = new(ELeave) CTelepathyGabbleSearch;
    CleanupStack::PushL( self );
    self->ConstructL();    
    CleanupStack::Pop();
    return self;
    }

//-----------------------------------------------------------------------------
// function_name	:  ~CTelepathyGabbleSearch
// description     	: Destructor
//----------------------------------------------------------------------------- 
CTelepathyGabbleSearch::~CTelepathyGabbleSearch()
    {
    }
    	

//-----------------------------------------------------------------------------
// function_name	: SearchL
// description     	: Used for searching contacts 
//----------------------------------------------------------------------------- 
TInt CTelepathyGabbleSearch::SearchL( GHashTable *aDataToSearch, CTestData* aTestData )
{
	Search_UserData	*userData = new(ELeave) Search_UserData;
	
	userData->data_to_search = aDataToSearch;
	userData->testData = aTestData;

	iTestData = aTestData;

	if( iTestData->GetSearchChan() )
		{
		SearchAgainL( aDataToSearch, iTestData );
		//Run the mainloop
		g_main_loop_run (iTestData->GetMainLoop());	
		}
	else
	{
		if (!tp_conn_request_channel_async( DBUS_G_PROXY( aTestData->GetTpConn() ),
                               TP_IFACE_CHANNEL_TYPE_CONTACT_SEARCH, 
                               TP_CONN_HANDLE_TYPE_NONE, 0, TRUE,
                               do_search_reply,(gpointer) userData ) )
                               
			{
			return KErrGeneral;	
			}	
		//Run the mainloop
		g_main_loop_run (iTestData->GetMainLoop());	
		DoSearchL( aDataToSearch, aTestData);
	}
	


	
	if(userData)
		{
			delete userData;
			userData = NULL;
		}

	//return the Message_sent that was updated in the callback sendmessage_cb	
	return iTestData->GetSearchState();
}

//-----------------------------------------------------------------------------
// function_name	: SearchAgainL
// description     	: Used for searching contacts second or more number of times
//----------------------------------------------------------------------------- 

 void CTelepathyGabbleSearch::SearchAgainL( GHashTable *aDataToSearch, CTestData* aTestData )
 {
 
	DBusGProxy *search_iface = NULL;
	TpChan *search_chan = NULL;
	
 	if(aTestData)
	 	{
	 	search_chan = aTestData->GetSearchChan();	
	 	}
  	if ( NULL == search_chan ) 
		{
		return ;	
		}
	//Get chan interface					
	search_iface = tp_chan_get_interface( search_chan	, 
		TELEPATHY_CHAN_IFACE_CONTACTSEARCH_QUARK );	
	
	if ( NULL == search_iface ) 
		{
		return ;	
		}					

	//Call the search on tp		      										      
	tp_chan_type_search_async( search_iface, aDataToSearch, searchreply_cb, 
			aTestData );
			

 }


//-----------------------------------------------------------------------------
// function_name	: GetSearchKeysL
// description     	: Used for getting search keys 
//----------------------------------------------------------------------------- 
TInt CTelepathyGabbleSearch::GetSearchKeysL( CTestData* aTestData )
{
	Search_UserData	*userData = new(ELeave) Search_UserData;
	
	userData->data_to_search = NULL;
	userData->testData = aTestData;

	iTestData = aTestData;
	
	if (!tp_conn_request_channel_async( DBUS_G_PROXY( aTestData->GetTpConn() ),
                               TP_IFACE_CHANNEL_TYPE_CONTACT_SEARCH, 
                               TP_CONN_HANDLE_TYPE_NONE, 0, TRUE,
                               do_search_reply,(gpointer) userData ) )
                               
		{
		return KErrGeneral;	
		}

	//Run the mainloop
	g_main_loop_run (aTestData->GetMainLoop());
	
	DoSearchL(NULL,aTestData);
		
	if(userData)
		{
			delete userData;
			userData = NULL;
		}

	//return the Message_sent that was updated in the callback sendmessage_cb	
	return iTestData->GetSearchState();
}


void CTelepathyGabbleSearch::do_search_reply( DBusGProxy* /*proxy*/, char *chan_object_path, 
 			GError *error, gpointer user_data )
 {
 	Search_UserData* userData = static_cast<Search_UserData*> (user_data);
 	TpChan *search_chan = NULL;
 	
 	TInt search_state = userData->testData->GetSearchState();
 	
	
	/* Create the object to represent the channel */
	if ( error ) 
		{
		userData->testData->SetSearchState(CTestData::ESearchError);
		return;
		}
	search_chan = tp_chan_new( userData->testData->GetDBusConnection(), CONNMGR_BUS, chan_object_path, 
					TP_IFACE_CHANNEL_TYPE_CONTACT_SEARCH, TP_CONN_HANDLE_TYPE_NONE, 0 );
	                         
	                    
	g_free(chan_object_path);
  
  	
  	if ( NULL == search_chan ) 
		{
		userData->testData->SetSearchState(CTestData::ESearchError);
		return ;	
		}
	
	userData->testData->SetSearchChan( search_chan );	
	
	g_main_loop_quit(userData->testData->GetMainLoop());

  
 }


void CTelepathyGabbleSearch::DoSearchL(GHashTable *aDataToSearch,CTestData *testData)
{
	DBusGProxy *search_iface = NULL;
	TInt testType = testData->GetTestType();
	
	//Get chan interface					
	search_iface = tp_chan_get_interface( testData->GetSearchChan(), 
		TELEPATHY_CHAN_IFACE_CONTACTSEARCH_QUARK );	
	
	if ( NULL == search_iface ) 
		{
		testData->SetSearchState(CTestData::ESearchError);
		return ;	
		}					
   	//Register for the SearchResultReceived signal
	//ownership of the srch_result->user_data is transfered
	dbus_g_proxy_connect_signal( search_iface, "SearchResultReceived",
							G_CALLBACK( search_result_received_cb ),
							(gpointer)this, NULL );
	

	//Register for the SearchStateChanged signal
	dbus_g_proxy_connect_signal( search_iface, "SearchStateChanged",
							G_CALLBACK( search_state_changed_cb ),
							(gpointer)this, NULL );
   	      
    if( testType == CTestData::EGetSearchKeys )
	   {
	   	//Call the getsearchkeys on tp		      										      
		tp_chan_type_search_get_search_keys_async(search_iface,getsearchkeys_cb, testData );

	   }
    else if( testType == CTestData::ESearch )
	   {
	    //Call the search on tp		      										      
		tp_chan_type_search_async( search_iface, aDataToSearch, 
								searchreply_cb, testData);
	   }
	   
	g_main_loop_run(testData->GetMainLoop());   
	   	
}
void CTelepathyGabbleSearch::getsearchkeys_cb( DBusGProxy* /*proxy*/,  gchar* /*instr*/,
  							gchar** keys, GError *error, gpointer user_data )
{
CTestData* testData = static_cast<CTestData*> (user_data);

if(!keys || error )
	{
    testData->SetSearchState( CTestData::ESearchError );
	}

if(keys)
	{
	testData->GetStifLogger()->Log( _L("getsearchkeys_cb:SearchKeysFound") );
	testData->SetSearchState( CTestData::ESearchKeysFound );
	testData->SetSearchKeys(keys);
	}
g_main_loop_quit(testData->GetMainLoop());	
	
}



void CTelepathyGabbleSearch::searchreply_cb( DBusGProxy* /*proxy*/, GError *error, gpointer user_data ) 
{
 CTestData* testData = static_cast<CTestData*> (user_data);
 TInt testType = testData->GetTestType();
 
 testData->GetStifLogger()->Log( _L("searchreply_cb") );
 
 if ( testType == CTestData::ESearch )
 	{
		if(error)
			 {
			 	testData->GetStifLogger()->Log( _L("Error in search") );
			 	testData->SetSearchState(CTestData::ESearchError);
			 	tp_chan_close_async( DBUS_G_PROXY( testData->GetSearchChan() ), search_chan_closed_cb, testData );
			 }
	}
 
}


	

void CTelepathyGabbleSearch::search_result_received_cb ( DBusGProxy* /*proxy*/,
										guint       /*contact_handle*/,
										GHashTable	*values, 
										gpointer	user_data
									   )
{
CTelepathyGabbleSearch* search_userData = static_cast<CTelepathyGabbleSearch*> (user_data);
guint result_count = 0;
 
//iTestData->GetStifLogger()->Log( _L("search_result_received_cb") );
search_userData->GetTestData()->GetStifLogger()->Log( _L("search_result_received_cb") );

if(values )	
	result_count = g_hash_table_size(values);


//do this later
/*if(result_count)
	{
	g_hash_table_foreach(values,setfield_foreach,NULL);	
	}*/

	
}

void CTelepathyGabbleSearch::search_state_changed_cb ( DBusGProxy* /*proxy*/,
										guint       search_state,
										gpointer	user_data
									   )
{
CTelepathyGabbleSearch* search_userData = static_cast<CTelepathyGabbleSearch*> (user_data);
DBusGProxy *search_iface = NULL;
GError **err = NULL;
guint *state = NULL;
TpChan *search_chan = NULL;
CTestData* testData = search_userData->GetTestData();

if(search_userData && testData )
	{
	testData->GetStifLogger()->Log( _L("search_state_changed_cb") );	
	search_chan = testData->GetSearchChan();
	}


if( search_state == TP_CHANNEL_CONTACT_SEARCH_STATE_AFTER && search_userData  && search_chan )
	{
	testData->SetSearchState(CTestData::ESearchCompleted);	
	search_iface = tp_chan_get_interface( search_chan, 
		TELEPATHY_CHAN_IFACE_CONTACTSEARCH_QUARK );	
	tp_chan_type_search_get_search_state( search_iface,state,err);
	g_main_loop_quit(testData->GetMainLoop());	
	}
	
}

void CTelepathyGabbleSearch::search_chan_closed_cb ( DBusGProxy* /*proxy*/,
										  GError *error,
										gpointer	user_data
									   )
{
CTestData* testData = static_cast<CTestData*> (user_data);

testData->GetStifLogger()->Log( _L("search_chan_closed_cb") );
if(error)
	{
	testData->GetStifLogger()->Log( _L("search_chan_closed_cb : Error ") );
	testData->SetSearchState(CTestData::ESearchError);
	}
else
	{
	
	TInt testType = testData->GetTestType();
	if( testType == CTestData::EGetSearchKeys )
		{
		testData->SetSearchState(CTestData::ESearchKeysFound);
		testData->GetStifLogger()->Log( _L("search_chan_closed_cb : SearchKeysFound ") );
		}
	else if( testType == CTestData::ESearch )
		{
		testData->SetSearchState(CTestData::ESearchCompleted);
		testData->GetStifLogger()->Log( _L("search_chan_closed_cb : SearchCompleted ") );
		}
	}
g_main_loop_quit(testData->GetMainLoop());	
}

CTestData* CTelepathyGabbleSearch::GetTestData()
	 {
	 return iTestData;	
	 }

/*void  CTelepathyGabbleSearch::setfield_foreach (gpointer key, gpointer value, gpointer user_data)
{
  gchar *k = (gchar *) key;
  const char *v = g_value_get_string ( (const GValue*)value);
  CTestData* testdata = static_cast<CTestData*> (user_data);

  //testdata->GetStifLogger()->Log( _L("setfield_foreach") );

}*/

void CTelepathyGabbleSearch::CloseChannel( CTestData* aTestdata )
	{
		if( aTestdata->GetSearchChan() )
			{
			tp_chan_close_async( DBUS_G_PROXY( aTestdata->GetSearchChan() ), search_chan_closed_cb, aTestdata );	
			}
		//Run the mainloop
		g_main_loop_run (aTestdata->GetMainLoop());				
	}

//  End of File
