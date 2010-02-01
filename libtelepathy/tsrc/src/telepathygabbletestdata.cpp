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
* Description:    File containing Test Data at the global level (used by different test objects)
*
*/




// INCLUDE FILES
#include "telepathygabbletestdata.h"
#include <StifLogger.h>

//-----------------------------------------------------------------------------
// function_name	: CTestData
// description     	: Constructor
// Returns          : None
//-----------------------------------------------------------------------------

CTestData::CTestData(CStifLogger *aLogger)
	: iLogger(aLogger)
    {
    }
    
//-----------------------------------------------------------------------------
// function_name	: ConstructL
// description     	: Constructor
// Returns          : None
//-----------------------------------------------------------------------------

void CTestData::ConstructL()
	{
	iConnectionState = EDISCONNECTED; //default.
	iTestType = EOther;
	//Create the mainloop and also add the callback.
	iMainLoop = g_main_loop_new (NULL, FALSE);
	iSendRecvState = EMessageNotSent;
	iAddDeleteContactStatus = EADD_FAIL;
	iSearchState = ESearchStarted;
	iSearchKeys = g_new0( gchar*, 1);
	}

//-----------------------------------------------------------------------------
// function_name	: NewL
// description     	: Constructor
// Returns          : None
//-----------------------------------------------------------------------------
CTestData* CTestData::NewL(CStifLogger *aLogger)
    {
    
	CTestData* self = new(ELeave) CTestData(aLogger);
    CleanupStack::PushL( self );
    self->ConstructL();    
    CleanupStack::Pop();
    return self;
    }

//-----------------------------------------------------------------------------
// function_name	: ~CTestData
// description     	: destructor
// Returns          : None
//-----------------------------------------------------------------------------
CTestData::~CTestData()
    {
      
   	if( iLogger )
	    {
	     iLogger->Log( _L("~CTestData in\n") );
	    }
    //Destroy the hash table for text channels
    
    if( iTextChannelHashTable )
	    {
	    g_hash_table_destroy( iTextChannelHashTable );
	  	iTextChannelHashTable = NULL;
	    }
   
     //Dbus connection object
    if( iDBusConnection )
	   	{
	   	dbus_g_connection_unref (iDBusConnection); 
	   	iDBusConnection = NULL;	
	   	}
   
    if( iTpConnMgr )
	   	{
	   	g_object_unref (iTpConnMgr);
	   	iTpConnMgr = NULL;	
	   	}
	   	
   	//tp conn object
   	if( iTpConn )
	   	{
	   	g_object_unref (iTpConn);
	   	iTpConn = NULL;	
	   	}

	//main loop
	if( iMainLoop )
	   	{
	   	 g_main_loop_unref ( iMainLoop );  
	   	iMainLoop = NULL;	
	   	}
	   	
		 
    if( iSearchKeys )
	  {
	  	for( TInt i=0; iSearchKeys[i]; i++ )
			{
			g_free( iSearchKeys[i] );
			iSearchKeys[i] = NULL;
			}
	  	g_free( iSearchKeys );
	  	iSearchKeys = NULL;	
	  }
     
    if( iLogger )
	    {
	     iLogger->Log( _L("~CTestData out\n") );
	    }
    }

//-----------------------------------------------------------------------------
// function_name	: CTestData
// description     	: GetStifLogger
//-----------------------------------------------------------------------------

CStifLogger* CTestData::GetStifLogger()
	{
	return iLogger;	
	}


//-----------------------------------------------------------------------------
// function_name	: CTestData
// description     	: GetMainLoop
//-----------------------------------------------------------------------------

GMainLoop* CTestData::GetMainLoop()
	{
	return iMainLoop;	
	}

//-----------------------------------------------------------------------------
// function_name	: CTestData
// description     	: GetGroupSubscribeInterface
//-----------------------------------------------------------------------------

DBusGProxy* CTestData::GetGroupSubscribeInterface()
	{
	return iGroupSubscribeInterface;	
	}

//-----------------------------------------------------------------------------
// function_name	: CTestData
// description     	: GetGroupKnownInterface
//-----------------------------------------------------------------------------

DBusGProxy* CTestData::GetGroupKnownInterface()
	{
	return iGroupKnownInterface;	
	}

//-----------------------------------------------------------------------------
// function_name	: CTestData
// description     	: GetGroupPublishInterface
//-----------------------------------------------------------------------------

DBusGProxy* CTestData::GetGroupPublishInterface()
	{
	return iGroupPublishInterface;	
	}
//-----------------------------------------------------------------------------
// function_name	: CTestData
// description     	: GetGroupDenyInterface
//-----------------------------------------------------------------------------

DBusGProxy* CTestData::GetGroupDenyInterface()
	{
	return iGroupDenyInterface;	
	}
//-----------------------------------------------------------------------------
// function_name	: CTestData
// description     	: GetTextChannelsHT
//-----------------------------------------------------------------------------

GHashTable* CTestData::GetTextChannelsHT()
	{
	return iTextChannelHashTable;	
	} 

//-----------------------------------------------------------------------------
// function_name	: CTestData
// description     	: SetGroupSubscribeInterface
//-----------------------------------------------------------------------------

void CTestData::SetGroupSubscribeInterface( DBusGProxy* aGroupSubscribeInterface )
	{
	iGroupSubscribeInterface = aGroupSubscribeInterface;	
	}

//-----------------------------------------------------------------------------
// function_name	: CTestData
// description     	: GetGroupKnownInterface
//-----------------------------------------------------------------------------

void CTestData::SetGroupKnownInterface( DBusGProxy* aGroupKnownInterface )
	{
	iGroupKnownInterface = aGroupKnownInterface;	
	}

//-----------------------------------------------------------------------------
// function_name	: CTestData
// description     	: GetGroupPublishInterface
//-----------------------------------------------------------------------------

void  CTestData::SetGroupPublishInterface( DBusGProxy* aGroupPublishInterface )
	{
	iGroupPublishInterface = aGroupPublishInterface;	
	}
//-----------------------------------------------------------------------------
// function_name	: CTestData
// description     	: GetGroupDenyInterface
//-----------------------------------------------------------------------------
void CTestData::SetGroupDenyInterface(DBusGProxy*  aGroupDenyInterface)
	{
	iGroupDenyInterface = aGroupDenyInterface;	
	}
//-----------------------------------------------------------------------------
// function_name	: CTestData
// description     	: GetTextChannelsHT
//-----------------------------------------------------------------------------

void CTestData::SetTextChannelsHT( GHashTable*  aTextChannelHashTable )
	{
	iTextChannelHashTable = aTextChannelHashTable;	
	} 


//-----------------------------------------------------------------------------
// function_name	: CTestData
// description     	: GetDBusConnection
//-----------------------------------------------------------------------------
DBusGConnection* CTestData::GetDBusConnection()
	{
	return iDBusConnection;
	}

//-----------------------------------------------------------------------------
// function_name	: CTestData
// description     	: SetDBusConnection
//-----------------------------------------------------------------------------
void CTestData::SetDBusConnection( DBusGConnection* aDBusConnection )
	{
	iDBusConnection = aDBusConnection;
	}

//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : GetTpConn
//-----------------------------------------------------------------------------

TpConn* CTestData::GetTpConn()
	{
	return iTpConn;
	}	 

//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : SetTpConn
//-----------------------------------------------------------------------------

void CTestData::SetTpConn(TpConn* aTpConn)
	{
	 iTpConn = aTpConn;
	}	 

//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : SetTpConnMgr
//-----------------------------------------------------------------------------

void CTestData::SetTpConnMgr(TpConnMgr* aTpConnMgr)
	{
	 iTpConnMgr = aTpConnMgr;
	}

	 
//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : GetLoginCompleteFlag
//-----------------------------------------------------------------------------

TInt CTestData::GetLoginCompleteFlag()
	{
	return iLoginComplete;
	}	 


//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : IncLoginCompleteFlag
//-----------------------------------------------------------------------------

void CTestData::IncLoginCompleteFlag()
	{
	iLoginComplete++;
	}	 

//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : ResetLoginCompleteFlag
//-----------------------------------------------------------------------------

void CTestData::ResetLoginCompleteFlag()
	{
	iLoginComplete = 0;
	}
 
 
//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : GetTotalFetchCount
//-----------------------------------------------------------------------------

TInt CTestData::GetTotalFetchCount()
	{
	return iTotalFetchCount;
	}

//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : GetLastIndexInFetch
//-----------------------------------------------------------------------------

TInt CTestData::GetLastIndexInFetch()
	{
	return iLastIndex;
	}
	
//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : SetLastIndexInFetch
//-----------------------------------------------------------------------------

void CTestData::SetLastIndexInFetch( TInt aLastIndex )
	{
	iLastIndex = aLastIndex;
	}
		
//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : SetTotalFetchCount
//-----------------------------------------------------------------------------

void CTestData::SetTotalFetchCount(TInt aValue)
	{
	iTotalFetchCount = aValue;
	}
	
//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : GetTotalFetchCount
//-----------------------------------------------------------------------------

CTestData::TConnectionState CTestData::GetConnectionState()
	{
	return iConnectionState;
	}
//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : SetTotalFetchCount
//-----------------------------------------------------------------------------

void CTestData::SetConnectionState(TConnectionState aConnectionState)
	{
	iConnectionState = aConnectionState;
	}
	
//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : GetTotalFetchCount
//-----------------------------------------------------------------------------

CTestData::TSendRecvState CTestData::GetSendRecvMsgState()
	{
	return iSendRecvState;
	}
//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : SetTotalFetchCount
//-----------------------------------------------------------------------------

void CTestData::SetSendRecvMsgState(TSendRecvState aSendRecvState)
	{
	iSendRecvState = aSendRecvState;
	}

//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : GetTotalFetchCount
//-----------------------------------------------------------------------------

CTestData::TAddDeleteContactStatus CTestData::GetAddDeleteContactStatus()
	{
	return iAddDeleteContactStatus;
	}
//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : SetTotalFetchCount
//-----------------------------------------------------------------------------

void CTestData::SetAddDeleteContactStatus(TAddDeleteContactStatus aAddDeleteContactStatus)
	{
	iAddDeleteContactStatus = aAddDeleteContactStatus;
	}

//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : GetSearchState
//-----------------------------------------------------------------------------

CTestData::TSearchState CTestData::GetSearchState()
	{
	return iSearchState;
	}
//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : SetSearchState
//-----------------------------------------------------------------------------

void CTestData::SetSearchState(TSearchState aSearchState)
	{
	iSearchState = aSearchState;
	}	

//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : GetTestType
//-----------------------------------------------------------------------------

CTestData::TTestType CTestData::GetTestType( )
	{
	return iTestType;
	}	

//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : SetTestType
//-----------------------------------------------------------------------------

void CTestData::SetTestType(TTestType aTestType)
	{
	iTestType = aTestType;
	}	
//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : SetSearchKeys
//-----------------------------------------------------------------------------

void CTestData::SetSearchKeys( gchar **aKeys )
	{
	TInt i = 0;
	for( i=0; aKeys[i]; i++ )
		{
		iSearchKeys[i] = g_strdup(aKeys[i]);	
		}
	iSearchKeys[i] = aKeys[i];	
	}
	
//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : GetSearchKeys
//-----------------------------------------------------------------------------

gchar** CTestData::GetSearchKeys( )
	{
	return iSearchKeys;	
	}	


//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : SetSearchChan
//-----------------------------------------------------------------------------

void CTestData::SetSearchChan(TpChan *aSearchChan )
	{
	iSearchChan = aSearchChan;
	}	
	
//-----------------------------------------------------------------------------
// function_name	: CTestData
/// description     : GetSearchChan
//-----------------------------------------------------------------------------

TpChan* CTestData::GetSearchChan( )
	{
	return iSearchChan;
	}	
		
	
//End of File						