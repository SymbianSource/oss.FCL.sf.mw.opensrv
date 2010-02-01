/*
* ============================================================================
*  Name        : msg_enums.h
*  Part of     : message queue library
*  Version     : %version: 3 %
*
*  Copyright © 2007-2008 Nokia.  All rights reserved.
*  All rights reserved.
*  Redistribution and use in source and binary forms, with or without modification, 
*  are permitted provided that the following conditions are met:
*  Redistributions of source code must retain the above copyright notice, this list 
*  of conditions and the following disclaimer.Redistributions in binary form must 
*  reproduce the above copyright notice, this list of conditions and the following 
*  disclaimer in the documentation and/or other materials provided with the distribution.
*  Neither the name of the Nokia Corporation nor the names of its contributors may be used 
*  to endorse or promote products derived from this software without specific prior written 
*  permission.
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
*  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
*  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
*  SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
*  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
*  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
*  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* ============================================================================
* Template version: 1.0
*/


#ifndef __MSG_ENUMS_H__
#define __MSG_ENUMS_H__

#include <stdio.h>
#include <gtypes.h>
#define MAX_PARAM_LEN 1024


/**
* set of resuest ids from the clients
*/
//enum Request_Types Request_Types;
enum Request_Types
	{
	// login  
	ELogin_Request = 1,	
	// logout 
	ELogout_Request,
	EServer_DisConnect_Request,
	//Fetch Contact : only used when there are no contacts
	EFetch_Contacts_Error,
	//to send a message
	ESend_Request,
	//For send error
	ESend_Error,
	//to fetch the presence
	EPresence_Request,
	// to add a list
	EAdd_List_Request,
	// to delete a list
	EDelete_List_Request,
	// to rename a list
	ERename_List_Request,
	// to add a contact
	EAdd_Contact_Request,
	// to delete a contact
	EDelete_Contact_Request,
	//accept a local pending request
	EAccept_Contact_Request,
	//Reject local pending request
	EReject_Contact_Request,
	// to rename a contact
	ERename_Conatct_Request,
	//to receive message
	EText_Message_Receive,
	//The order here should not be changed
	//The logic in code is heavily dependant
	//on the order of these three enums
	// to fetch the contacts from server
	ECurrent_Member_Contacts,
	//local pending
	ELocal_Pending_Contacts,
	//remote pending
	ERemote_Pending_Contacts,
	
	//user is our client and contact is othr end 
	//user -> contact
	//contact -> user
	
	//Some other contact trying to add client
	ETrying_To_Add_Contact,
	//User added a contact
	EKnown_Contact_Added,
	//User added contact and it is remote pending
	ESubscribe_Remote_Pending,
	//Remote pending request rejected or user removed the contact from list
	ESubscribe_Removed_Or_Rejected,
	//Remote pending request accepted for subscribe of contact presence
	//So now the user can know the presence of contact
	ESubscribe_RP_Accepted,
	//Remote pending request accepted for publish of users presence
	//So now the contact can know the presence of user
	EPublish_RP_Accepted,
	//Removed from publish list
	EPublish_Removed_Or_Rejected,
	//
	ETrying_To_Add_Contact_Known,
	//
	ETrying_To_Add_Contact_Publish,
	
	//to update own presence
	EUpdateOwnPresence,
	//to update own avtar
	EUpdateOwnAvtar,
    //clear avatar
	EClearOwnAvatar,
	//presence notification
	EPresenceNotification,	
	
	//Kill the process
	EKill_Process,
	//Search for contacts
	ESearch,
	
	//get the search keys
	ESearch_Get_Keys,
	
	//Search state changed
	ESearch_State_During,
	
	//Search state changed : Done
	ESearch_State_Finished,	
	
	EUserEndGprs,
	
	//Fetch the contacts
	EFetchCachedContacts,

	// add if  required
	} ;
	
/**
* message types send and recieve via queue
*/	
enum Message_Types
	{
	EInteger,
	EString,
	EChar,
	EEnums
	// add here if need
	};
/**
* Message header for request
*/
typedef struct message_hdr_req message_hdr_req;

struct message_hdr_req
	{
	gint message_type; //Type of the message
	gchar continue_flag;
	gint protocol_id;//from TRequestId	
	gint session_id;//from TRequestId
	gint request_id;//from TRequestId
	};
	
/**
* Message header for response 
*/
typedef struct message_hdr_resp message_hdr_resp;

struct message_hdr_resp
	{
	//Warning : !!!!!!!!!!!!!! The order should not be changed 
	message_hdr_req hdr_req;
	gchar continue_flag;
	gint response; //success or not 
	gint error_type; //if not, error type
	};
	

/**
* operations state enums variables 
*/

enum Login_State
	{
	// default request  error
	EGeneral_Err = -1,
	// connected to server successfully  
	EConnected,	
	// connecting to server in progress 
	EConnecting,
	// not connected to server or logged off
	ENotConnected

	// add if  required
	};
	
/**
* error codes..
*/
enum error_codes 
	{
	INVALID_PARAMETERES = -7000,
	DBUS_CONNECTION_ERROR,
	TP_CONNMGR_ERROR,
	TP_CONNECTION_ERROR,
	TP_HASHTABLE_ERROR,
	MEM_ALLOCATION_ERROR,
	NOT_CONNECTED,
	CONNECTION_STATUS_ERROR,
	MSG_Q_SEND_FAILED,
	TP_SEARCH_CHAN_ERROR,
	TP_SEARCH_IFACE_ERROR,
	TP_AVATAR_IFACE_ERROR,
	
	ELOGIN_AUTHENTICATION, //CASE 3 FRM SPEC 
	ELOGIN_NONE_SPECIFIED,//CAN BE USER CANCELLING THE A/P ETC.,  CASE 0 FRM SPEC
	ELOGIN_NETWORK_ERROR, //For the network errors
	ELOGIN_AUTHORIZATION, //ALL SSL RELATED ERRORS OTHR CASES
	};
	
#endif //__MSG_ENUMS_H__
