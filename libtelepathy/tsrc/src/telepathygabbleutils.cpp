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
* Description:    File containing Utility functions
*
*/





//Include files
#include <e32err.h>
#include <e32const.h>
#include <e32base.h>

#include <glib.h>

#include "telepathygabbletestdata.h"
#include "telepathygabbleutils.h"




//-----------------------------------------------------------------------------
// function_name	: CTelepathyGabbleUtils
// description     	: constructor
//----------------------------------------------------------------------------- 
guint CTelepathyGabbleUtils::get_handle_from_contact ( const gchar  *contact_id,
														CTestData* aTestData)
	 
{

	guint          handle;
	gboolean       success;
	GError        *error = NULL;
	
	
	//check for the return value if 0 is ok or should some negative value to be passed
	g_return_val_if_fail (contact_id != NULL, 0); 
	
	GArray      *handles = NULL;	
	const gchar *contact_ids[2] ; //contact_ids initialization here gives error ..may be some compiler issue as otherwise okay to initialze here
	
	
	contact_ids[0]= contact_id;
	contact_ids[1]= NULL;
	

	success = tp_conn_request_handles (DBUS_G_PROXY (aTestData->GetTpConn()),
					   TP_CONN_HANDLE_TYPE_CONTACT,
					   contact_ids, &handles, &error);
	if( !handles || error )	
	{
	 	//do some error handling here
	 	
	 	g_error_free(error);
	 	return KErrNotFound; 
	}				   
						   

	handle = g_array_index(handles, guint, 0);
	g_array_free (handles, TRUE);
	

	if (!success) 
	{
		//test_log_handler ( error->message );
		g_clear_error (&error);
		
		//check for the return value if 0 is ok or should some negative value to be passed
		return 0;
	}

	return handle;
}

//  End of File
