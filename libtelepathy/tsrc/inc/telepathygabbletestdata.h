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
* Description:    test utility for having the global data 
*
*/



#ifndef __TELEPATHYGABBLETESTDATA_H__
#define __TELEPATHYGABBLETESTDATA_H__

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

#define VERBOSE


#define CONNMGR_NAME "telepathy-gabble"
#define CONNMGR_BUS "org.freedesktop.Telepathy.ConnectionManager.gabble"
#define CONNMGR_PATH "/org/freedesktop/Telepathy/ConnectionManager/gabble"
#define CHANNEL_BUS "org.freedesktop.Telepathy.Channel.Type.Text"
#define CHANNEL_PATH "/org/freedesktop/Telepathy/Channel/Type/Text"
#define PROTOCOL "jabber"

class CStifLogger;

class CTestData : public CBase
{
    public:
    enum TConnectionState	
		{
			ECONNECTED =0,
			EDISCONNECTED,
			ECONNECTING,
		};
	 enum TTestType	
		{
			ELogin = 0,
			ECancel,
			ESend,
			ESendErr,
			EReceive,
			ERecvBlank,
			EGetSearchKeys,
			ESearch,
			EOther
		};
	enum TSendRecvState
	   {
	   	 EMessageNotSent =0,
	   	 EMessageSent,
	   	 EMessageRecvd,
	   	 EMessageRecvErr,
	   	 EMessageSendErr
	   };

	enum TAddDeleteContactStatus
		{
			EADD_SUCCESS =0,
			EADD_FAIL ,
			EDELETE_SUCCESS,
			EDELETE_FAIL
		};
		
		
	enum TSearchState
		{
			ESearchStarted =0,
			ESearchError ,
			ESearchCompleted,
			ESearchKeysFound			
		};		
		
    private:
    	/**
        * C++ default constructor.
        */
       
        CTestData( CStifLogger* aLogger );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
   public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CTestData* NewL( CStifLogger* aLogger );

        /**
        * Destructor.
        */
        ~CTestData();
	


public://Set funtions

	   DBusGProxy* GetGroupSubscribeInterface();
	   DBusGProxy* GetGroupPublishInterface();
	   DBusGProxy* GetGroupKnownInterface();
	   DBusGProxy* GetGroupDenyInterface();
	   GMainLoop*  GetMainLoop();
	   GHashTable* GetTextChannelsHT();
	   DBusGConnection* GetDBusConnection();
	   TpConn* GetTpConn();
	   TTestType GetTestType();	   
	   TInt GetLoginCompleteFlag();
	   TInt GetTotalFetchCount();
	   TInt GetLastIndexInFetch();
	   TConnectionState GetConnectionState();
	   TSendRecvState GetSendRecvMsgState();
	   TAddDeleteContactStatus GetAddDeleteContactStatus();
	   CStifLogger* GetStifLogger();
	   TSearchState GetSearchState();
	   
	   void SetGroupSubscribeInterface( DBusGProxy* aGroupSubscribeInterface );
	   void SetGroupPublishInterface( DBusGProxy* aGroupPublishInterface );
	   void SetGroupKnownInterface( DBusGProxy* aGroupKnownInterface );
	   void SetGroupDenyInterface( DBusGProxy* aGroupDenyInterface );
	   void SetTextChannelsHT( GHashTable* aTextChannelHashTable );
	   void IncLoginCompleteFlag();
	   void ResetLoginCompleteFlag();
	   void SetTotalFetchCount( TInt aValue );
	   void SetLastIndexInFetch( TInt aLastIndex );
	   void SetConnectionState( TConnectionState aloginState );
	   void SetTestType( TTestType aTestType );	   
	   void SetSendRecvMsgState( TSendRecvState aSendRecvState );
	   void SetAddDeleteContactStatus(TAddDeleteContactStatus aAddDeleteContactStatus);
	   void SetTpConn(TpConn* aTpConn);
	   void SetTpConnMgr(TpConnMgr* aTpConnMgr);
 	   void SetDBusConnection( DBusGConnection* aDBusConnection );  
 	   void SetSearchState(TSearchState aSearchState);
 	   void SetSearchKeys( gchar **aKeys );
 	   gchar** GetSearchKeys( );
 	   void SetSearchChan(TpChan* aSearchChan );
 	   TpChan* GetSearchChan( );
 	    
private://Data members
     
		GMainLoop *iMainLoop;

		//Hash Table for the text channels.
		GHashTable 	*iTextChannelHashTable;	

		TpConn 		*iTpConn;
		
		TpConnMgr *iTpConnMgr;

		//Global dbus-connection object
		DBusGConnection *iDBusConnection;


		DBusGProxy    *iGroupSubscribeInterface; //used for adding a contact
		DBusGProxy    *iGroupKnownInterface; //used for removing a contact
		DBusGProxy    *iGroupPublishInterface; //gives local pending members 
		DBusGProxy    *iGroupDenyInterface;    //used to deny the contacts.	

		TInt 		  iLoginComplete;
		TInt		  iTotalFetchCount;		
		TConnectionState   iConnectionState;
		TSendRecvState  iSendRecvState;
		TAddDeleteContactStatus iAddDeleteContactStatus;
		TSearchState	iSearchState;
		TTestType  iTestType;
		
		CStifLogger *iLogger;
		
		//Used in Fetch Contacts Test Case
		TInt iLastIndex; 
		gchar** iSearchKeys;
		
		TpChan* iSearchChan;
};//End of class

#endif //__TELEPATHYGABBLETESTDATA_H__

