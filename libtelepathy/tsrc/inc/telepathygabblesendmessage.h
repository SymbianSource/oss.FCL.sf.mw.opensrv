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
* Description:    Class declaration for SendMessage.
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


// FORWARD DECLARATION
class CTestData;


#define VERBOSE


// CLASS DECLARATION
NONSHARABLE_CLASS(CTelepathyGabbleSendRecvMessage) : public CBase
    {
    
    private:
    	/**
        * C++ default constructor.
        */
       
        CTelepathyGabbleSendRecvMessage();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
   public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CTelepathyGabbleSendRecvMessage* NewL();

        /**
        * Destructor.
        */
        ~CTelepathyGabbleSendRecvMessage();
        
   
		
    public: // Member functions
    
		  	// Member function to send the message.
	     TInt SendMessage( const char *contact_id, const char *message,
	     					CTestData* aTestData  );
			
		 TInt ReceiveMessage(const char *contact_id, const char *message,
		 						CTestData* aTestData );
		
			
	public : //Callback functions

		 //Callback function for sendmessage
		 static void sendmessage_cb( DBusGProxy *proxy, GError *error, gpointer userdata );
	
	
		//Callback for receive message
		static void receivedMessage_cb (  DBusGProxy *proxy,guint            message_id,
								   	guint            timestamp,guint            from_handle,
								   	guint            message_type,guint            message_flags,
								    gchar           *message_body,gpointer		user_data );
								    
								    
		static void SendError_cb (  DBusGProxy *proxy,
								guint            error,
							   	guint            timestamp,
							    guint            message_type,
							    gchar           *message_body,
							    gpointer		user_data )	;							    
								    
								    
		//Callback for the text_channel_find for a particular contact_id.			  
		static gboolean text_channels_find_func(gchar *key,TpChan *text_channel, guint *contact_handle);
	

 };





				
