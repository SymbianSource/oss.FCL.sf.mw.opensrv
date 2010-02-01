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
* Description:    Class declaration for Presence Test Cases.
*
*/




//Include files
#include <e32err.h>
#include <e32const.h>
#include <e32base.h>

#include <glib.h>

#ifndef DBUS_API_SUBJECT_TO_CHANGE
#define DBUS_API_SUBJECT_TO_CHANGE
#endif

#include <dbus/dbus-glib.h>
	   
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

#include "tp-conn-iface-presence-gen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "telepathy-presence.h"

// FORWARD DECLARATION
class CTestData;


#define VERBOSE


// CLASS DECLARATION
NONSHARABLE_CLASS(CTelepathyGabblePresenceTest) : public CBase
    {
    
    private:
    	/**
        * C++ default constructor.
        */
       
        CTelepathyGabblePresenceTest();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
   public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CTelepathyGabblePresenceTest* NewL();

        /**
        * Destructor.
        */
        ~CTelepathyGabblePresenceTest();
    public: //members
    
    	TpConn *iTpConn;
			
	public : 
	
		//presence related functions

		 static TelepathyPresenceState telepathy_presence_state_from_str (const gchar *str);
		 void telepathy_contacts_send_presence (DBusGProxy *pres_iface,
					 TelepathyPresence *presence/*, CTestData *aTestData*/);
				
		 	 
		 static void telepathy_presences_foreach (gpointer *key,gpointer *value,
		      									TelepathyPresence **presence);
		      
		      
		 //Callback functions
		 static void telepathy_presence_handle_foreach (gpointer  *key,gpointer *value);
		      
		 void telepathy_presence_handler(DBusGProxy *proxy,GHashTable *handle_hash);
		 

 };






				
