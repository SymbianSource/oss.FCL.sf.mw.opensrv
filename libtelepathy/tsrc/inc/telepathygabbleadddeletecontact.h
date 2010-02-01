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
* Description:    Class declaration for Contact Managemnet Test Cases.
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
#include "tp-chan-type-streamed-media-gen.h"
#include "tp-props-iface.h"
#include "tp-constants.h"
#include "tp-interfaces.h"

#include <pthread.h>
//for the global data
#include "telepathygabbletestdata.h"

#define VERBOSE


// FORWARD DECLARATION
class CTestData;


// CLASS DECLARATION
NONSHARABLE_CLASS(CTelepathyGabbleAddDeleteContact) : public CBase
    {
    
    private:
		/*
        * C++ default constructor.
        */
       
        CTelepathyGabbleAddDeleteContact();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
   public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CTelepathyGabbleAddDeleteContact* NewL();

        /**
        * Destructor.
        */
        ~CTelepathyGabbleAddDeleteContact();
     
		
    public: // Member functions

			TInt add_contact ( const gchar  *contact_id, const gchar *message, 
								CTestData* aTestData );	
			TInt remove_contact ( const gchar  *contact_id, const gchar *message,
									CTestData* aTestData );
				
	public : //Callback functions
 
			static void add_contact_cb( DBusGProxy *proxy, GError *error, gpointer userdata );	
			static void remove_contact_cb( DBusGProxy *proxy, GError *error, gpointer userdata );
			
			static void roster_members_changed_cb (DBusGProxy           *group_iface,
									    gchar                *message,
									    GArray               *added,
									    GArray               *removed,
									    GArray               *local_pending,
									    GArray               *remote_pending,
									    guint                 actor,
									    guint                 reason,
									    gpointer  			  userdata);
 };






				
