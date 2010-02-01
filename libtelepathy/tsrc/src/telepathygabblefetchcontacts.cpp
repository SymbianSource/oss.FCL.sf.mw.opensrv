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
* Description:    Used for Fetch contacts related Test Cases
*
*/




//Include files
#include <e32err.h>
#include <e32const.h>
#include <e32base.h>
#include <glib.h>

#include "telepathygabblefetchcontacts.h"
#include "telepathygabbletestdata.h"





//-----------------------------------------------------------------------------
// function_name	: CTelepathyGabbleFetchContacts
// description     	: constructor
//----------------------------------------------------------------------------- 
CTelepathyGabbleFetchContacts::CTelepathyGabbleFetchContacts()
{
}

//-----------------------------------------------------------------------------
// function_name	: ConstructL
// description     	: constructor
//----------------------------------------------------------------------------- 
void CTelepathyGabbleFetchContacts::ConstructL()
{
	//do nothing
}

//-----------------------------------------------------------------------------
// function_name	: NewL
// description     	: constructor
//----------------------------------------------------------------------------- 
CTelepathyGabbleFetchContacts* CTelepathyGabbleFetchContacts::NewL()
    {
    
	CTelepathyGabbleFetchContacts* self = new(ELeave) CTelepathyGabbleFetchContacts;
    CleanupStack::PushL( self );
    self->ConstructL();    
    CleanupStack::Pop();
    return self;
    }

//-----------------------------------------------------------------------------
// function_name	: ~CTelepathyGabbleFetchContacts
// description     	: Destructor
//----------------------------------------------------------------------------- 
CTelepathyGabbleFetchContacts::~CTelepathyGabbleFetchContacts()
    {
	
    }
    
//-----------------------------------------------------------------------------
// function_name	: test_request_roster
// description     	: Function for fetching the contacts.
//----------------------------------------------------------------------------- 
TInt CTelepathyGabbleFetchContacts::test_request_roster( CTestData* aTestData )
{
	
	DBusGProxy * group_iface_subscribe = aTestData->GetGroupSubscribeInterface();
	
	tp_chan_iface_group_get_all_members_async(group_iface_subscribe, get_roster_member_cb, aTestData);

  	//Run the mainloop
	g_main_loop_run (aTestData->GetMainLoop());

	//retrun some error code here	
	if ( aTestData->GetTotalFetchCount() != KErrGeneral )
	{
		return KErrNone;
	}
	else
	{
		return KErrGeneral;
	}
}

//-----------------------------------------------------------------------------
// function_name	: get_roster_member_cb
// description     	: callback for getting the roster member.
//----------------------------------------------------------------------------- 
void  CTelepathyGabbleFetchContacts::get_roster_member_cb( DBusGProxy* /*proxy*/,GArray* current_members, GArray* local_pending_members, GArray* remote_pending_members, GError* /*error*/, gpointer userdata ) 
{
	guint fetch_count=0;
	guint fetch_loop_count=0;	
	GArray* fetch_members = NULL;
	guint i;
	guint total_len =0;
	CTestData* testData = static_cast<CTestData*> (userdata);

	
	total_len = current_members->len + local_pending_members->len + remote_pending_members->len;

	testData->SetTotalFetchCount(total_len);
	
	if( !total_len )
		{
			g_main_loop_quit(testData->GetMainLoop());
			return;		
			
		}
	
	fetch_members = g_array_new (FALSE, FALSE, sizeof (guint32));

	
	if( current_members->len > 0)
	{
		fetch_count = 0;
		fetch_loop_count = (current_members->len) / KMaxContactFetchCount + ( (current_members->len % KMaxContactFetchCount)? 1 : 0);
		
		for( i=0; i<fetch_loop_count; i++ )
		{
				
			g_array_remove_range(fetch_members,0,fetch_count);
			
			fetch_count = (current_members->len <= KMaxContactFetchCount)? current_members->len : KMaxContactFetchCount;
			
			g_array_append_vals (fetch_members,current_members->data,fetch_count);
			
			g_array_remove_range(current_members,0,fetch_count);
			
			//we will quit the main loop in inspect_handles_cb
			tp_conn_inspect_handles_async( DBUS_G_PROXY(testData->GetTpConn()),
					       TP_CONN_HANDLE_TYPE_CONTACT ,fetch_members,
					       inspect_handles_cb,userdata);
		}
	}
	
	if( local_pending_members->len > 0)
	{
		fetch_count = 0;
		
		g_array_remove_range(fetch_members,0,fetch_members->len);
		
		fetch_loop_count = (local_pending_members->len) / KMaxContactFetchCount + ( (local_pending_members->len % KMaxContactFetchCount)? 1 : 0);
		
		for( i=0; i<fetch_loop_count; i++ )
		{
				
			g_array_remove_range(fetch_members,0,fetch_count);
			
			fetch_count = (local_pending_members->len <= KMaxContactFetchCount)? local_pending_members->len : KMaxContactFetchCount;
			
			
			g_array_append_vals (fetch_members,local_pending_members->data,fetch_count);
			
			g_array_remove_range(local_pending_members,0,fetch_count);
			
			tp_conn_inspect_handles_async( DBUS_G_PROXY(testData->GetTpConn()),
					       TP_CONN_HANDLE_TYPE_CONTACT ,fetch_members,
					       inspect_handles_cb,userdata);
		}

	}

	if( remote_pending_members->len > 0)
	{
		fetch_count = 0;
		g_array_remove_range(fetch_members,0,fetch_members->len);
		fetch_loop_count = (remote_pending_members->len) / KMaxContactFetchCount + ( (remote_pending_members->len % KMaxContactFetchCount)? 1 : 0);
		
		for( i=0; i<fetch_loop_count; i++ )
		{
				
			g_array_remove_range(fetch_members,0,fetch_count);
			
			fetch_count = (remote_pending_members->len <= KMaxContactFetchCount)? remote_pending_members->len : KMaxContactFetchCount;
			
			g_array_append_vals (fetch_members,remote_pending_members->data,fetch_count);
			g_array_remove_range(remote_pending_members,0,fetch_count);
			tp_conn_inspect_handles_async( DBUS_G_PROXY(testData->GetTpConn()),
					       TP_CONN_HANDLE_TYPE_CONTACT ,fetch_members,
			  	       inspect_handles_cb,userdata);	
		}	
	
	}
	return;
	
}

//-----------------------------------------------------------------------------
// function_name	: inspect_handles_cb
// description     	: inspect the handle for the contact, return the contact handle 
//----------------------------------------------------------------------------- 
void CTelepathyGabbleFetchContacts::inspect_handles_cb( DBusGProxy* /*proxy*/,char **handles_names,
														GError *error, gpointer userdata ) 
{
    CTestData* testData = static_cast<CTestData*> (userdata);
    TInt last_index = testData->GetLastIndexInFetch(); 
    guint total_len = testData->GetTotalFetchCount();
    
	if(!last_index)
	{
		//allmembers_names = g_new0 ( char *, total_len + 1);		
	}
	
	if(!handles_names || error )
		{
		//error condition in inspect_handles_cb
		testData->SetTotalFetchCount(KErrGeneral);
		g_main_loop_quit(testData->GetMainLoop());
		return;
		}
		
	for( TInt i=0; handles_names[i]; i++ )
		{
		
		//handle_names can be logged to the log file
		last_index++;
		}
		
	testData->SetLastIndexInFetch(last_index);
	
	if (last_index == total_len )
		{
			//inspect_handle_cb for all the contacts has been receieved.
			//Quit the main loop.
			
			//Set the flag that inspect_handles_cb has been completed successfully.
			g_main_loop_quit(testData->GetMainLoop());	
		}
	
	return ;
}
//  End of File
