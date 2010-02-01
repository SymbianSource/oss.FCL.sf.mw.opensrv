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
* Description:    Class declaration for Search
*
*/




 
#ifndef DBUS_API_SUBJECT_TO_CHANGE
#define DBUS_API_SUBJECT_TO_CHANGE
#endif


//Include files
#include <e32err.h>
#include <e32const.h>
#include <e32base.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <dbus/dbus-glib.h>
#include <glib.h>
	   
#include <unistd.h>

#include "tp-conn.h"
#include "tp-connmgr.h"
#include "tp-chan.h"
#include "tp-chan-gen.h"
#include "tp-ch-gen.h"
#include "tp-chan-iface-group-gen.h"
#include "tp-chan-type-text-gen.h"
#include "tp-chan-type-search-gen.h"
#include "tp-chan-type-streamed-media-gen.h"
#include "tp-props-iface.h"
#include "tp-constants.h"
#include "tp-interfaces.h"
#include <pthread.h>


// FORWARD DECLARATION
class CTestData;


#define VERBOSE


// CLASS DECLARATION
NONSHARABLE_CLASS(CTelepathyGabbleSearch) : public CBase
    {
    public:
    
   struct Search_UserData
    {
    GHashTable* data_to_search;
    CTestData* testData;
    };
    
    private:
    	/**
        * C++ default constructor.
        */
       
        CTelepathyGabbleSearch();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
   public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CTelepathyGabbleSearch* NewL();

        /**
        * Destructor.
        */
        ~CTelepathyGabbleSearch();
        
   
		
    public: // Member functions
    
		  	
	     TInt SearchL( GHashTable *aDataToSearch, CTestData* aTestData );
			
		 TInt GetSearchKeysL( CTestData* aTestData );
		 
		 void SearchAgainL( GHashTable *aDataToSearch, CTestData* aTestData );
		 
		 CTestData* GetTestData();
		 
		 void DoSearchL(GHashTable *aDataToSearch,CTestData *testData);
		 
		 void CloseChannel( CTestData* aTestdata );
			
	public : //Callback functions

	 	static void search_chan_closed_cb ( DBusGProxy	*proxy,
										  GError *error,
										gpointer	user_data
									   );
 		static void search_result_received_cb ( DBusGProxy	*proxy,
										guint       contact_handle,
										GHashTable	*values,
										gpointer	user_data
									   );
 		static void search_state_changed_cb ( DBusGProxy	*proxy,
										guint       search_state,
										gpointer	user_data
									   );						    
								    
								    
	    static void searchreply_cb( DBusGProxy *proxy, GError *error, gpointer user_data );
	    
	    //static void  setfield_foreach (gpointer key, gpointer value, gpointer user_data);

		static void do_search_reply( DBusGProxy *proxy, char *chan_object_path, 
 			GError *error, gpointer user_data );
		static void getsearchkeys_cb( DBusGProxy *proxy,  gchar * instr,
  							gchar ** keys, GError *error, gpointer user_data );
	private:
	
		CTestData *iTestData;  							
 };





				
