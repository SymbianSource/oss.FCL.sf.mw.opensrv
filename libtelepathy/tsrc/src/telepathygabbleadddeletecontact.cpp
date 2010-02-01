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
* Description:    Used for Contact management(add/delete etc) related Test Cases
*
*/




//Include files
#include <e32err.h>
#include <e32const.h>
#include <e32base.h>
#include <glib.h>
#include <StifLogger.h>

#include "telepathygabbleutils.h"
#include "telepathygabbleadddeletecontact.h"
#include "telepathygabblelogintest.h"



//-----------------------------------------------------------------------------
// function_name	: CTelepathyGabbleAddDeleteContact
// description     	: constructor
//----------------------------------------------------------------------------- 
CTelepathyGabbleAddDeleteContact::CTelepathyGabbleAddDeleteContact()
{
}

//-----------------------------------------------------------------------------
// function_name	: ConstructL
// description     	: constructor
//----------------------------------------------------------------------------- 
void CTelepathyGabbleAddDeleteContact::ConstructL()
{
	//do nothing
}

//-----------------------------------------------------------------------------
// function_name	: NewL
// description     	: constructor
//----------------------------------------------------------------------------- 
CTelepathyGabbleAddDeleteContact* CTelepathyGabbleAddDeleteContact::NewL()
{
	CTelepathyGabbleAddDeleteContact* self = new(ELeave) CTelepathyGabbleAddDeleteContact;
    CleanupStack::PushL( self );
    self->ConstructL();    
    CleanupStack::Pop();
    return self;
}

// Destructor
//-----------------------------------------------------------------------------
// function_name	: ~CTelepathyGabbleAddDeleteContact
// description     	: Destructor
//----------------------------------------------------------------------------- 
CTelepathyGabbleAddDeleteContact::~CTelepathyGabbleAddDeleteContact()
{
    	
}

//-----------------------------------------------------------------------------
// function_name	: add_contact
// description     	: adding a contact
//----------------------------------------------------------------------------- 
TInt CTelepathyGabbleAddDeleteContact::add_contact ( const gchar  *contact_id, 
													 const gchar *message,
													 CTestData* aTestData )
{
  	guint     handle;
	GArray    *handles = NULL;
	//gpointer userdata = (gpointer)contact_id;
	
	//first check the contact_id.
	if( contact_id == NULL )
	{
		return CTestData::EADD_FAIL;
	}
	
	aTestData->SetAddDeleteContactStatus(CTestData::EADD_FAIL);
	
	//acess the utils class function to get the contact handle.	
	handle = CTelepathyGabbleUtils::get_handle_from_contact ( contact_id, aTestData );

	if ( handle == KErrNotFound )
		{
		return CTestData::EADD_FAIL;	
		}

	handles = g_array_new (FALSE, FALSE, sizeof (guint));
	
	g_array_append_val (handles, handle);
	
	DBusGProxy *group_iface_subscribe = aTestData->GetGroupSubscribeInterface();
	DBusGProxy *group_iface_publish = aTestData->GetGroupPublishInterface();
	DBusGProxy *group_iface_known = aTestData->GetGroupKnownInterface();
	
	
	//register the callback, commented for now.
	dbus_g_proxy_connect_signal (group_iface_subscribe, "MembersChanged",
					     G_CALLBACK (roster_members_changed_cb ),
					     aTestData, NULL); 
	
	dbus_g_proxy_connect_signal (group_iface_publish, "MembersChanged",
					     G_CALLBACK (roster_members_changed_cb ),
					     aTestData, NULL); 
					     

	dbus_g_proxy_connect_signal (group_iface_known, "MembersChanged",
					     G_CALLBACK (roster_members_changed_cb ),
					     aTestData, NULL); 					     
	
	
	tp_chan_iface_group_add_members_async ( group_iface_subscribe, handles, message, add_contact_cb, aTestData );
	
	//Start the main loop
	g_main_loop_run (aTestData->GetMainLoop());

	g_array_free (handles, TRUE);
	
	//return the status was updated in the callback.
	return aTestData->GetAddDeleteContactStatus(); 
}


//-----------------------------------------------------------------------------
// function_name	: remove_contact
// description     	: remove the contact.
//----------------------------------------------------------------------------- 
TInt CTelepathyGabbleAddDeleteContact::remove_contact ( const gchar  *contact_id, 
														const gchar *message,
													    CTestData* aTestData
													  )
{

	guint     handle;
	GArray    *handles = NULL;

	//first check the contact_id.
	if( contact_id == NULL )
	{
		return CTestData::EDELETE_FAIL;
	}
	
	aTestData->SetAddDeleteContactStatus( CTestData::EDELETE_FAIL );
	
	handle = CTelepathyGabbleUtils::get_handle_from_contact ( contact_id, aTestData);
	
	if ( handle == KErrNotFound )
		{
		return CTestData::EDELETE_FAIL;	
		}


	DBusGProxy *group_iface_known = aTestData->GetGroupKnownInterface();
	
	handles = g_array_new (FALSE, FALSE, sizeof (guint));
	
	g_array_append_val (handles, handle);
	
	tp_chan_iface_group_remove_members_async ( group_iface_known, handles, message, remove_contact_cb, aTestData );

	//Start the main loop
	g_main_loop_run (aTestData->GetMainLoop());
	
	g_array_free (handles, TRUE);
	
	return (aTestData->GetAddDeleteContactStatus());
}


//-----------------------------------------------------------------------------
// function_name	: add_contact_cb
// description     	:Callback for add contact
//----------------------------------------------------------------------------- 
void CTelepathyGabbleAddDeleteContact::add_contact_cb( DBusGProxy* /*proxy*/, GError* error, gpointer userdata )
{
		CTestData* testdata = static_cast<CTestData*> (userdata);
		if ( !error )
		{
			testdata->SetAddDeleteContactStatus(CTestData::EADD_SUCCESS);
		}
		else
		{
			testdata->SetAddDeleteContactStatus(CTestData::EADD_FAIL);
				
		}
		
		//Quit the main loop
		g_main_loop_quit(testdata->GetMainLoop());
			     	
}

//-----------------------------------------------------------------------------
// function_name	: remove_contact_cb
// description     	: Callback for remove contact.
//----------------------------------------------------------------------------- 
	
void CTelepathyGabbleAddDeleteContact::remove_contact_cb( DBusGProxy* proxy, GError *error, gpointer userdata )
{
		
		CTestData* testdata = static_cast<CTestData*> (userdata);
		if ( error )
		{
		testdata->GetStifLogger()->Log( _L("CTelepathyGabbleTest::deleting already removed contact,it should give some error \n") );
		testdata->SetAddDeleteContactStatus( CTestData::EDELETE_FAIL );
		}
		else
		{
		testdata->GetStifLogger()->Log( _L("CTelepathyGabbleTest::deleted successfully \n") );
		testdata->SetAddDeleteContactStatus( CTestData::EDELETE_SUCCESS );
		}
	  //Quit the main loop started in Delete contact.
	  g_main_loop_quit(testdata->GetMainLoop());
}

//-----------------------------------------------------------------------------
// function_name	: roster_members_changed_cb
// description     	: Callback for change in the contacts.
//----------------------------------------------------------------------------- 

void CTelepathyGabbleAddDeleteContact::roster_members_changed_cb (DBusGProxy*   /*group_iface*/,
									    gchar*                /*message */,
									    GArray               *added,
									    GArray               *removed,
									    GArray               *local_pending,
									    GArray               *remote_pending,
									    guint                 /*actor*/,
									    guint                 /*reason*/,
									    gpointer  			  userdata)
{

	gchar **contact_name = NULL;



	CTestData* atestData = static_cast<CTestData*> (userdata);
	
	//TInt return_value = KErrNone;
	

	
	if (added->len > 0)
	{
		
		//this is not working, use async api later
	    /*tp_conn_inspect_handles( DBUS_G_PROXY(atestData->GetTpConn()),
						       TP_CONN_HANDLE_TYPE_CONTACT ,added,
						       &contact_name,NULL);*/
		if(contact_name)				       
		{
			//The contact has been added.
			// Must verify this against the contact_name for which the request was sent?
			// This will depend on whether we will hit callback for one contact at a time
			// or multiple callbacks for all contacts at a single time. ?
			//return_value = KErrNone;
			//atestData->SetAddDeleteContactStatus(CTestData::EADD_SUCCESS);
			
		}
	}
	if (removed->len > 0)
	{
		/*tp_conn_inspect_handles( DBUS_G_PROXY(atestData->GetTpConn()),
						       TP_CONN_HANDLE_TYPE_CONTACT ,removed,
						       &contact_name,NULL);*/
		if(contact_name)					       
		{
			//The contact_name was deleted
			//Must verify this against the contact_name for which the request was sent?
			//return_value == KErrNone;
		}
	}
	if (local_pending->len > 0) 
	{
		/*tp_conn_inspect_handles( DBUS_G_PROXY(atestData->GetTpConn()),
						       TP_CONN_HANDLE_TYPE_CONTACT ,local_pending,
						       &contact_name,NULL);*/
		if(contact_name)				       
		{
			//what should be tested here ? 
			//like what is next action expected ?
			//since the contact is still local_pending, is the test case failed ?
			//return_value = KErrNone;	
			//atestData->GetAddDeleteContactStatus();
		}
		
	}
	if (remote_pending->len > 0) 
	{
		/*tp_conn_inspect_handles( DBUS_G_PROXY(atestData->GetTpConn()),
						       TP_CONN_HANDLE_TYPE_CONTACT ,remote_pending,
						       &contact_name,NULL);*/
		if(contact_name)					       
		{
			//return_value = KErrNone;		
			//atestData->GetAddDeleteContactStatus();
		}
		
	}
	
	//dont quit from here right now.
}

//  End of File
