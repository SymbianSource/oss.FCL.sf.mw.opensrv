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
* Description:    Class declaration for Login Test Cases.
*
*/






#ifndef DBUS_API_SUBJECT_TO_CHANGE
#define DBUS_API_SUBJECT_TO_CHANGE
#endif

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

#define VERBOSE

#include "telepathygabbletestdata.h"


#define CONNMGR_NAME "telepathy-gabble"
#define CONNMGR_BUS "org.freedesktop.Telepathy.ConnectionManager.gabble"
#define CONNMGR_PATH "/org/freedesktop/Telepathy/ConnectionManager/gabble"
#define CHANNEL_BUS "org.freedesktop.Telepathy.Channel.Type.Text"
#define CHANNEL_PATH "/org/freedesktop/Telepathy/Channel/Type/Text"
#define PROTOCOL "jabber"



//forward decalration
class CTestData;

//class declaration			
NONSHARABLE_CLASS(CTelepathyGabbleLoginTest) : public CBase
    {
   
    private:
    	/**
        * C++ default constructor.
        */
       
        CTelepathyGabbleLoginTest();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
   public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CTelepathyGabbleLoginTest* NewL();

        /**
        * Destructor.
        */
        ~CTelepathyGabbleLoginTest();
        
   
		
    public: // Member functions

	   //Login and Logout functions
       TInt action_login(char* username,char* password,
       							char* server,char* resource,char* ssl_flag,
       							char *port,char *proxy_server, char *proxy_port,
       							CTestData* aTestData);

	   TInt action_logout( CTestData* aTestData );
	   
	   TInt action_cancel(char* username,char* password,
	   							char* server, char* resource,char* ssl_flag,
	   							char *port,char *proxy_server, char *proxy_port,
	   							CTestData* aTestData );
	 
	   
	public : //Callback functions
	   //Callback function for the status change

		static gboolean status_changed_cb(DBusGProxy *proxy,
		  guint status, guint reason,
		  gpointer user_data);

		//Callback for the creation of a new channel on succeccful login.				  
		static void new_channel_handler(DBusGProxy *proxy, const char *object_path,
			const char *channel_type, guint handle_type,
			guint channel_handle, gboolean suppress_handler,
			gpointer user_data);
		
	
		//Init the text_channel	
		static void text_channel_init  ( TpChan   *text_chan  );
		
	
	 };
 
