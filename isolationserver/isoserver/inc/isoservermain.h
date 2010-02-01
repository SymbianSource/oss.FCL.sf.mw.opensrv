/*
* ============================================================================
*  Name        : isoservermain.h
*  Part of     : isolation server.
*  Version     : %version: 18 %
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

/*!
* /def To avoid multiple inclusion of header
*/
#ifndef __ISO_SERVER_H_
#define __ISO_SERVER_H_

#include <glib.h>
#include <gtypes.h>
#include <sys/cdefs.h>
#include <sys/types.h> 


#include "tp-chan-type-text-gen.h"
#include "tp-conn.h"
#include "tp-connmgr.h"
#include "tp-chan.h"
#include "tp-conn-gen.h"
#include "tp-props-iface.h"
#include "tp-interfaces.h"
#include "tp-conn-iface-presence-gen.h"

#include "msg_enums.h"

/*! /file 
*	Header file for the isolation server main. This file has implimentations
*	to recieve messages from client and delegate the requests to other 
*	funtions based on the message type.
*	client anywhere in the documentation refers to anyone
*	using the isoserver
*/

enum connection_status
    {
    connecting = -1,
    connected,
    disconnected,
    disconnecting,
    not_connected
    };
/*! /typedef struct userConnection typedefed to userConnection
*/
typedef struct userConnection  userConnection;

/*! /struct userConnection isoservermain.h
*	/brief There is only one object of this struct. This struct represents
*	one login session. This strcut has all the necessary data members to
*	maintain a session.
*	
*	/var conn telepathy connection object
*	/var connmgr telepathy connection manager object
*	/var dbusCon D-Bus
*
*	/var isConnected Boolean : If loged in
*
*	/var current_members_names : 2D char array having the current members list
*	/var local_pending_names : 2D char array having the locally pending members list
*	/var remote_pending_names : 2D char array having the remotely pending members list
*
*	/var group_iface_subscribe : Interface to subscribe list
*	/var group_iface_publish : Interface to publish list
*	/var group_iface_known : Interface to known list
*
*	/var connmgr_bus : connection manager bus 
*
*	/var text_channels : open text(chat) channels
*
*	/var pres_iface : Interface for presence
*
*	/var search_chan : channel for search
*	/var search_hdr_req : request header for search
*
*/
struct userConnection 
	{
	TpConn* conn;
	DBusGConnection* dbusConn;
	TpConnMgr* connmgr;
	enum connection_status conn_stat;
	
	gchar** current_members_names;
	gchar** local_pending_names;
	gchar** remote_pending_names;
	
	
	DBusGProxy    *group_iface_subscribe;
	DBusGProxy    *group_iface_publish;
	DBusGProxy    *group_iface_known;
	
	gchar* connmgr_bus;
	
	GHashTable *text_channels;
	
	TpPropsIface *pres_iface;
	
	TpChan *search_chan;
	message_hdr_req* search_hdr_req;	
	
	gboolean logout_flag;
	};

/*! /typedef struct globalMainloop to globalMainloop
*/
typedef struct globalMainloop globalMainloop;

/*!	/struct globalMainloop isoservermain.h
*	/brief Has a pointer to the mainloop. This mainloop runs in a 
*	thread created by the main thread(isoserver.exe). This loop 
*	listens to the event from telepathygabble
*
*	/var mainloop pointer to the mainloop
*/
struct globalMainloop 
	{
	GMainLoop* mainloop;
	};

/*! /var mainloop_struct global variable for mainloop
*	/brief This variable can not be put into userConnection as the life 
*	time of a mainloop is not related to per login session
*/
extern globalMainloop mainloop_struct;

/*!	/var globalCon represents one login session 
*/
extern userConnection globalCon;

/*!	/brief This message reads message type from the message buffer
*	passed as argument
*
* 	/var pc message buffer
*	
*	/return pointer to message header response which has the message type
*/
message_hdr_resp* read_message_type( gchar* pc ) ;

/*! /brief This is an entry point to the thread. isoserver.exe
*	creates a thread which runs a mainloop. mainloop is passed as 
*	argument to the thread entry function. This mainloop which is 
*	run in the thread listens for the events from telepathygabble
*
*	/var data unused param
*/
gpointer thread_entry( gpointer data ) ;

/*! /brief If there are any parse error, this function is called to
*	dispatch the error to client.
*	
*	/param msg_struct The response header to be sent 
*	/param err Error
*/
gint send_error( message_hdr_resp* msg_struct, gint err );

/*! /brief This function waits for the requests from the client.
*	Requests are parsed, validated and appropriate actions taken.
*	A new thread is created when login happens. This function runs 
*	a while loop which is only quit when a kill request is recieved 
*	from the client.
*
*	/remark should this thread be created before login?
*	/remark when is the mainloop quit ? (Should that be after logout or 
*	after getting a kill request from the client)
*/
int message_send_recv() ;

#endif //__ISO_SERVER_H_
