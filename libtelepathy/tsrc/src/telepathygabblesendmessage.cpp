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
* Description:    Used for Send/Receive Test Cases
*
*/



 
//Include files
#include <e32err.h>
#include <e32const.h>
#include <e32base.h>
#include <glib.h>
#include <StifLogger.h>
#include "telepathygabbleutils.h"
#include "telepathygabblesendmessage.h"
#include "telepathygabbletestdata.h"




//-----------------------------------------------------------------------------
// function_name	: CTelepathyGabbleSendRecvMessage
// description     	: constructor
//----------------------------------------------------------------------------- 

CTelepathyGabbleSendRecvMessage::CTelepathyGabbleSendRecvMessage()
{
}

//-----------------------------------------------------------------------------
// function_name	: ConstructL
// description     	: constructor
//----------------------------------------------------------------------------- 
void CTelepathyGabbleSendRecvMessage::ConstructL()
{
	
}

//-----------------------------------------------------------------------------
// function_name	: NewL
// description     	: NewL
//----------------------------------------------------------------------------- 
CTelepathyGabbleSendRecvMessage* CTelepathyGabbleSendRecvMessage::NewL()
    {
	CTelepathyGabbleSendRecvMessage* self = new(ELeave) CTelepathyGabbleSendRecvMessage;
    CleanupStack::PushL( self );
    self->ConstructL();    
    CleanupStack::Pop();
    return self;
    }

//-----------------------------------------------------------------------------
// function_name	:  ~CTelepathyGabbleSendRecvMessage
// description     	: Destructor
//----------------------------------------------------------------------------- 
CTelepathyGabbleSendRecvMessage::~CTelepathyGabbleSendRecvMessage()
    {
    }
    	

//-----------------------------------------------------------------------------
// function_name	: SendMessage
// description     	: To send a message
//----------------------------------------------------------------------------- 
TInt CTelepathyGabbleSendRecvMessage::SendMessage( const char *contact_id, 
													const char *message,
													CTestData* aTestData )
{
	TpChan* text_chan = NULL; //for send msg

	TInt contact_handle;
	GArray *handles = NULL;
	GError* error = NULL;
	DBusGProxy  *text_iface = NULL;

	gpointer userdata = NULL; 
	
	
	//get the contact handle.
	contact_handle =  CTelepathyGabbleUtils::get_handle_from_contact(contact_id,aTestData);
	
	if ( contact_handle == KErrNotFound )
		{
		return CTestData::EMessageSendErr;	
		}

	text_chan = (TpChan*)g_hash_table_find(
								   aTestData->GetTextChannelsHT(),
							      (GHRFunc) text_channels_find_func,
							      &contact_handle );	 
	      
	//text_chan for the particular contact does not exist.
	//New text_chan must be created.						      
	if (text_chan == NULL)	
		{ 
		text_chan = tp_conn_new_channel(
							(aTestData->GetDBusConnection()),
							(aTestData->GetTpConn()),CONNMGR_BUS,TP_IFACE_CHANNEL_TYPE_TEXT,
							TP_CONN_HANDLE_TYPE_CONTACT,contact_handle, TRUE );	

		//Insert the new text_chan to the hash table.							

		g_hash_table_insert(
				(aTestData->GetTextChannelsHT()),
				g_strdup(dbus_g_proxy_get_path(DBUS_G_PROXY(text_chan))),text_chan);
					
		}

	text_iface = tp_chan_get_interface(text_chan,TELEPATHY_CHAN_IFACE_TEXT_QUARK);	
	
	
	if( aTestData->GetTestType() == CTestData::ESendErr ||
		aTestData->GetTestType() == CTestData::EReceive )
		{
		//Register the callback to get the error
		//when the recipeint_id is invalid.
		dbus_g_proxy_connect_signal(text_iface, "SendError",
					G_CALLBACK(SendError_cb),
						aTestData, NULL);	
		}

	if( aTestData->GetTestType() == CTestData::EReceive )
		{
		//Register for the receive message callback here.
	 	dbus_g_proxy_connect_signal(text_iface, "Received",
					(G_CALLBACK(receivedMessage_cb)),
					aTestData, NULL);	
		}

 	
	
	//This callback tells whether the message is sent to the server or not.
	
	tp_chan_type_text_send_async( text_iface, TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL, 
	message,sendmessage_cb, aTestData); 
	
	
	// user_data = this i.e. the class pointer  

	//Run the mainloop
	g_main_loop_run (aTestData->GetMainLoop());


	//return the Message_sent that was updated in the callback sendmessage_cb	
	return aTestData->GetSendRecvMsgState();
}

//-----------------------------------------------------------------------------
// function_name	: ReceiveMessage
// description     	: receive message
//----------------------------------------------------------------------------- 
TInt CTelepathyGabbleSendRecvMessage::ReceiveMessage(   const char *contact_id,
														const char *message,
														CTestData* aTestData )
{
	TpChan* text_chan = NULL; //for send msg
	DBusGProxy *text_iface;

	guint contact_handle;
	
	
	 
	//send msg 1
	//For Automation, Do the send message to yourself
	//own contact_id received from the cfg file.
	TInt msg_status = SendMessage(contact_id,message,aTestData);
	if( msg_status == CTestData::EMessageNotSent )
		{
			aTestData->GetStifLogger()->Log( _L("ReceiveMessage: Msg is not sent successfully \n") );
			
		    if( aTestData->GetTestType() == CTestData::ERecvBlank )
		    	g_main_loop_quit(aTestData->GetMainLoop()); 
		    
			return CTestData::EMessageNotSent;
		}
	 

 	/*contact_handle =  CTelepathyGabbleUtils::get_handle_from_contact(contact_id,aTestData);
 	
 	text_chan = (TpChan*)g_hash_table_find(
 					(aTestData->GetTextChannelsHT()),
				      (GHRFunc) text_channels_find_func,
				      &contact_handle );	
    text_iface = tp_chan_get_interface(text_chan ,
					   TELEPATHY_CHAN_IFACE_TEXT_QUARK);
					   
 	//Register for the receive message callback here.
 	dbus_g_proxy_connect_signal(text_iface, "Received",
				(G_CALLBACK(receivedMessage_cb)),
				aTestData, NULL);
 	
 	//send msg 2
 	//Do the send message to yourself
 	
	msg_status == CTestData::EMessageNotSent )
		{
			return CTestData::EMessageNotSent;
		}*/
	aTestData->GetStifLogger()->Log( _L("after : register receivedMessage_cb \n") );
	if (aTestData->GetTestType() == CTestData::EReceive && 
			aTestData->GetSendRecvMsgState() != CTestData::EMessageSendErr )
	{
		//there are two callbacks now.
		//receivedMessage_cb and sendmessage_cb.
		
		//Run the mainloop again as it was quit in the callback sendmessage_cb.
		//the message_state is not yet MESSAGE_RECEIVED.
		//we need to start the main loop again to get the receivedmessage_cb.
		aTestData->GetStifLogger()->Log( _L("start mainloop if state not as EMESSAGE_RCVD \n") );
 		g_main_loop_run (aTestData->GetMainLoop());
 		
 	}
  return aTestData->GetSendRecvMsgState();
}

//-----------------------------------------------------------------------------
// function_name	: sendmessage_cb
// description     	: Callback for Send Message
//----------------------------------------------------------------------------- 
void CTelepathyGabbleSendRecvMessage::sendmessage_cb( DBusGProxy* /*proxy*/, GError* /*error*/, gpointer userdata ) 
{
	CTestData* testdata = static_cast<CTestData*> (userdata);
	
	testdata->GetStifLogger()->Log( _L("inside sendmessage_cb \n") );
		
 	//This is the test case for sending message to invalid userid.
 	if ( testdata->GetTestType() != CTestData::ESendErr )
 		
 	{
 	    	    
		testdata->SetSendRecvMsgState(CTestData::EMessageSent);
		testdata->GetStifLogger()->Log( _L("sendmessage_cb::msg sent successfully \n") );
 
 		//Quit the main loop , just for now
 		g_main_loop_quit(testdata->GetMainLoop()); 
 	}
 	else if( testdata->GetTestType() == CTestData::ERecvBlank )
 	{
 	  	
 		//Quit the main loop , just for now
 		testdata->GetStifLogger()->Log( _L("sendmessage_cb::test case is for blank msg sending \n") );
 		g_main_loop_quit(testdata->GetMainLoop()); 	
 	}
 	
 
 	//otherwise send message error test case, dont quit.

}

//-----------------------------------------------------------------------------
// function_name	: receivedMessage_cb
// description     	: Callback for receive message
//----------------------------------------------------------------------------- 
void CTelepathyGabbleSendRecvMessage::receivedMessage_cb (  DBusGProxy* /*proxy*/,
								guint            /* message_id */,
							   	guint            /* timestamp */,
							    guint           /* from_handle */,
							    guint           /* message_type */,
							    guint           /* message_flags */,
							    gchar           * /*message_body */,
							    gpointer		user_data )
{
	//GArray  *message_ids;
	//GArray  *handles = NULL;
	CTestData* testdata = static_cast<CTestData*> (user_data);
	
	
	/*message_ids = g_array_new (FALSE, FALSE, sizeof (guint));

	g_array_append_val (message_ids, message_id);

	g_array_free (message_ids, TRUE);
	
	handles = g_array_new (FALSE, FALSE, sizeof (guint32));
	
	g_array_append_val (handles, from_handle);	
	
	g_array_free (handles, TRUE);*/
	testdata->GetStifLogger()->Log( _L("receivedMessage_cb::test case is for receive msg \n") );
	
	if( testdata->GetTestType() == CTestData::EReceive )
		{
		testdata->SetSendRecvMsgState(CTestData::EMessageRecvd);
		
		//Quit the main loop once the receive message callback is received. 	
	 	g_main_loop_quit(testdata->GetMainLoop());
			
		}
	

	
}

//-----------------------------------------------------------------------------
// function_name	: text_channels_find_func
// description     	: find whether the text channel 
//										is existing given the hash table pointer
//----------------------------------------------------------------------------- 
gboolean CTelepathyGabbleSendRecvMessage::text_channels_find_func(	gchar* /*key*/,
									TpChan *text_channel_ht_ptr,
									guint *contact_handle)
{
	if ((text_channel_ht_ptr->handle == *contact_handle))
	{
		return TRUE;
	}//else
	return FALSE;
} 

void CTelepathyGabbleSendRecvMessage::SendError_cb (  DBusGProxy* /*proxy*/,
								guint            error,
							   	guint            /*timestamp*/,
							    guint            /*message_type*/,
							    gchar*           /*message_body*/,
							    gpointer		user_data )
{
	
	//Get the testdata
	CTestData* testdata = static_cast<CTestData*> (user_data);
	
	FILE* fp;
		
	fp = fopen("c:\\testlogs.txt","a");
	if(fp)
	{
		fprintf(fp,"error  code is %u", error);
		fprintf(fp,"\n");
		fclose(fp);
	}

	testdata->GetStifLogger()->Log( _L("inside SendError_cb \n") );
	

	testdata->SetSendRecvMsgState(CTestData::EMessageSendErr);		
  	//quit the main loop
  	g_main_loop_quit(testdata->GetMainLoop());	
}