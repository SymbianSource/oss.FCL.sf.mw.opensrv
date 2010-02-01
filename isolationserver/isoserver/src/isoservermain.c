/*
* ============================================================================
*  Name        : isoservermain.c
*  Part of     : isolation server.
*  Version     : %version: 33 %
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


#include <glib.h>

#include <gtypes.h>
#include <sys/types.h>   
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <stdio.h>  
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include "msgqlib.h"
#include "msg_enums.h"
#include "msgliterals.h"
#include "isoservermain.h"
#include "isoim.h"
#include "isoconnectionmanager.h"
#include "isomodifycontactlist.h"
#include "isoutils.h"
#include "isopresence.h"
#include "isoutils.h"
#include "isoavtar.h"
#include "isosearch.h"
#include "isogetsearchkeys.h"
#include "isofetchcontactlist.h"

/*!	\file 
*	Impliments the functions in isoservermain.h
*/

/*!	\def Thread stack size
*/
#define THREAD_STACK_SIZE	26000

/*! \var mainloop_struct global variable having a pointer to mainloop
*/
globalMainloop mainloop_struct;

/*!	\var globalCon one object per login session
*/
userConnection globalCon;


/*! /brief This message reads message type from the message buffer
*   passed as argument
*
*   /var pc message buffer
*   
*   /return pointer to message header response which has the message type
*/
message_hdr_resp* read_message_type( gchar* buf ) 
	{
	
	message_hdr_resp *msg_hdr = NULL;
	iso_logger( "%s", "In - read_message_type\n" );
	//allocate memory and check for error
	msg_hdr = ( message_hdr_resp* ) malloc( sizeof( message_hdr_resp ) );
	if ( NULL == msg_hdr ) 
		{
		return NULL;	
		}
	//initialze memory to 0
    memset( msg_hdr, '\0', sizeof( message_hdr_resp ) );
    //read message request header from buffer
    memcpy( msg_hdr, buf, sizeof( message_hdr_req ) );
    iso_logger( "%s", "Out - read_message_type\n" );
    //return message header read from buffer
	return msg_hdr;
	}

/*! /brief This is an entry point to the thread. isoserver.exe
*   creates a thread which runs a mainloop. mainloop is passed as 
*   argument to the thread entry function. This mainloop which is 
*   run in the thread listens for the events from telepathygabble
*
*   /var data unused param
*/
gpointer thread_entry( gpointer data ) 
	{
	UNUSED_FORMAL_PARAM(data);
	//wait till status_changed_cb to happen
	//The only main loop is run in a thread..
	iso_logger( "%s", "In - thread_entry\n" );
	//run the mainloop in thread..
	g_main_loop_run( mainloop_struct.mainloop ); 
	iso_logger( "%s", "Out - thread_entry\n" );
	return NULL;
	}
	
/*! /brief If there are any parse error, this function is called to
*   dispatch the error to client.
*   
*   /param msg_struct The response header to be sent 
*   /param err Error
*/
gint send_error( message_hdr_resp* msg_struct, gint err )	
	{
	int result = 0;
	int pri = MSG_PRI_NORMAL;
	int timeout = NO_WAIT; 
	int error = 0;  
	
	//set the error and reason
	msg_struct->error_type = err;
	//Since this is method to send error
	//response is always 0
	msg_struct->response = 0;
	
	//request has fialed send appr. response
	result = MsgQCreate( RESPONSE_QUEUE, MAX_MSG_Q_SIZE, MSG_Q_FIFO, &error );
	if ( ERROR == result )
		{
		//graceful exit?
		return ERROR;
		}
		
	//send message created to client		
	result = MsgQSend( RESPONSE_QUEUE, (void*)msg_struct, sizeof( message_hdr_resp ),
			 pri, timeout, &error );
			
	if( result != 0 )
		{
		// msg deliver falied 
		return MSG_Q_SEND_FAILED;
		}
	return 0;	
	}
	
/*! /brief This function waits for the requests from the client.
*   Requests are parsed, validated and appropriate actions taken.
*   A new thread is created when login happens. This function runs 
*   a while loop which is only quit when a kill request is recieved 
*   from the client.
*
*   /remark should this thread be created before login?
*   /remark when is the mainloop quit ? (Should that be after logout or 
*   after getting a kill request from the client)
*/
int message_send_recv() 
	{
    
	int err = 0;
	int result = 0;
	int timeout1 = TIME_OUT; // block for 100 ms
	char rmsg[MAX_MSG_SIZE];  // 1024 bytes
	GMainLoop* mainloop = NULL;
	GThread* join1 = NULL;
	message_hdr_resp* msg_struct = NULL;
	GArray* avtarcontent = NULL;
	
	globalCon.conn_stat = disconnected;
	globalCon.logout_flag = 0;

	/* Try to Create queueOne again, this will create the queue again,
	   this will just return as its already created by main thread */
	iso_logger( "%s", "In - message_send_recv\n" );
	
	result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE, MSG_Q_FIFO, &err);
	
	//logic here to accept multiple clients
	mainloop = g_main_loop_new ( NULL, FALSE ); //global mainloop
	
	
	if ( ERROR == result || NULL == mainloop )
		{
		return ERROR;
		}
	
	mainloop_struct.mainloop = mainloop;

	while( 1 ) 
		{
			
		//initalize memory to 0
		memset( rmsg, '\0', MAX_MSG_SIZE );
		// Receive Message from queueTwo 
		result = MsgQReceive(REQUEST_QUEUE, rmsg, MAX_MSG_SIZE, timeout1, &err); 
		
		//MRT bug - 4 more bytes recieved than sent..
		result -= 4; 


		//The message should be atleast of size msg_hdr_req
		//sizeof should be type casted to gint 
		//coz when comparing gint and guint(sizeof return value) 
		//gint is converted to guint
		if( result >= ( gint )sizeof( message_hdr_req ) )
			{
			gboolean stat_err = FALSE;
			// read message header and get type..
			msg_struct = read_message_type( rmsg );
			if ( NULL == msg_struct ) 
				{
				//needs to be discussed what can be done..
				//should exit from here?
				return MEM_ALLOCATION_ERROR;	
				}
			switch( globalCon.conn_stat )
			    {
			    case connected:
			        {
			            //switch to the message type, where action to be performed
			            //is decided dpending on the message type
			            switch ( msg_struct->hdr_req.message_type ) 
			            {
                            
                        case ESend_Request:
                            {
                            iso_logger( "%s", "In - ESend_Request\n" );
                            //parse the request and 
                            // send the request to telepathy
                            err = action_parse_send( rmsg, result );
                            if ( err < 0 ) 
                                {
                                //Send request has failed..
                                //send the error code to the client
                                send_error( msg_struct, err );
                                }
                        
                            break;
                            }
                        case EUpdateOwnPresence:
                            {
                            iso_logger( "%s", "In - EUpdateOwnPresence\n" );
                            //parse the request and 
                            // send the request to telepathy
                            err = action_parse_presence( rmsg, result );
                            if ( err < 0 ) 
                                {
                                //Send request has failed..
                                //send the error code to the client
                                send_error( msg_struct, err );
                                }
                        
                            break;
                            } 
                        //For accept, add and delete same function does the job
                        //depending on the message type that is passed with rmsg
                        case EReject_Contact_Request:
                        
                        case EAccept_Contact_Request:
                        
                        case EAdd_Contact_Request:
                        
                        case EDelete_Contact_Request:
                            {
                            iso_logger( "%s", "In - modify Contact \n" );
                            //Parse the request for to add contact and 
                            //request telepathy to add contact
                            err = action_parse_modify_contact_list( rmsg, result );
                            if ( err < 0 ) 
                                {
                                //addcontacts has fialed send appr. response
                                send_error( msg_struct, err );
                                }
                        
                            break;
                            }
                        
                        case ESearch:
                            {
                            iso_logger( "%s", "In - ESearch\n" );
                            //parse the request and 
                            // send the request to telepathy
                            err = action_parse_search( rmsg, result );
                            if ( err < 0 ) 
                                {
                                //Send request has failed..
                                //send the error code to the client
                                iso_logger( "%s %d", "error in - ESearch error code:", err );
                                send_error( msg_struct, err );
                                }
                            break;                      
                            }
                            
                        case ESearch_Get_Keys:
                            {
                            iso_logger( "%s", "In - ESearch_Get_Keys\n" );
                            err = get_search_keys( msg_struct );
                            
                            if ( err < 0 ) 
                                {
                                //Send request has failed..
                                //send the error code to the client
                                iso_logger( "%s %d", "error in - ESearch_Get_Keys error code:", err );
                                send_error( msg_struct, err );
                                }
                                
                            break;
                            }
                        case ELogout_Request:
                            {
                            //Message type is to logout
                            iso_logger( "%s", "In - logout\n" );
                            globalCon.conn_stat = disconnecting;
                            action_logout( rmsg );
                            //globalCon.logout_flag = 1;
                            break;  
                            }
                        case EUpdateOwnAvtar:
                           {
                           err = update_own_avatar( rmsg,msg_struct,result, &avtarcontent);
                           if ( err < 0 ) 
                               {
                               //Send request has failed..
                               //send the error code to the client
                               iso_logger( "%s %d", "error in - ESearch_Get_Keys error code:", err );
                               send_error( msg_struct, err );
                               }                     
                           break;
                           }
                        case EClearOwnAvatar:
                            {
                            err = clear_avatar( &(msg_struct->hdr_req) );
                            if ( err < 0 ) 
                               {
                               //Send request has failed..
                               //send the error code to the client
                               send_error( msg_struct, err );
                               }
                            break;
                            }
                        case EFetchCachedContacts:
                            fetch_cached_contacts();
                            break;
                            
                        default:
                            {
                            iso_logger( "%s", "In - default state is connected\n" );
                            stat_err = TRUE;
                            break;  
                            }
                        
                         }
			        }
			        break;
			    case disconnected:
			        switch( msg_struct->hdr_req.message_type )
                    {
                    case ELogin_Request:
                        {
                        //parse the message buffer and login
                        iso_logger( "%s", "In - login\n" );
                        //parses the request message and does a login 
                        //if all the parameters are correct
                        globalCon.conn_stat = connecting;
                        err = action_parse_login( rmsg, result );
                        //If there is no error in login create a new thread
                        //where mainloop will be run..
                        if ( !err )  
                            {
                            //Create a thread for the mainloop to run..
                            //Thread entry does a run on mainloop
                            join1 = g_thread_create_full( thread_entry, mainloop, 
                                    THREAD_STACK_SIZE, TRUE, FALSE,
                                    G_THREAD_PRIORITY_NORMAL , NULL );  
                        
                            
                            }
                        else 
                            {
                            //There was some error while loging in.. send the error 
                            //to client
                            globalCon.conn_stat = disconnected;
                            send_error( msg_struct, err );
                            }
                        break;
                        }
                        
                    default:
                        {
                        iso_logger( "%s", "In - default state is disconnected\n" );
                        stat_err = TRUE;
                        }
                    }
			        break;
			    case not_connected:
			        switch( msg_struct->hdr_req.message_type )
			            {
			            case  EKill_Process :
                            {
                            //no need to change the state as anyways it is going to be killed
                            //globalCon.conn_stat = not_connected;
                            globalCon.logout_flag = 1;
                            break;
                            }
			            default:
                            {
                            iso_logger( "%s", "In - default state is not_connected\n" );
                            stat_err = TRUE;
                            }
			            }
			        break;
			    default:
			        {
			        iso_logger( "%s", "In - default state connecting or disconnecting \n" );
                    stat_err = TRUE;
                    break;  
                    }
			    }
			if ( stat_err )
			    {
			    stat_err = FALSE;
			    iso_logger( "%s", "In - stat_err true \n" );
			    send_error( msg_struct, CONNECTION_STATUS_ERROR );
			    }
			if ( globalCon.logout_flag ) 
				{
				//quit mainloop before exiting the thread and then server
				//quiting mainloop will inturn make the thread to fall off
				g_main_loop_quit( mainloop_struct.mainloop );
				iso_logger( "%s", "logout flag break \n" );
				break;					
				}
			free ( msg_struct );

			}
		else 
			{
			break;			
			}
		}
	iso_logger( "%s", "In - g_thread_join\n" );
	
	

	//join the thread created for to run mainloop before exiting
	g_thread_join( join1 );	
	iso_logger( "%s", "After this should be after mainloop quit - g_thread_join\n" );
	
	/* delete message queueOne */
	result=MsgQDelete( REQUEST_QUEUE, &err );
	
	iso_logger( "%s", "Out - MsgQDelete\n" );
	return 0;
	}

/*! \brief server main process which listens to requests from client
*	and processes those requests. A new thread is created to listen to
*	the events from the telepathygabble. 
*/
int main()
	{
	int err = 0;
	//FILE *fp = NULL;
	    
	g_type_init();
	
	g_thread_init( NULL );/*
	_dbus_setenv("DBUS_VERBOSE","1");
	fp= freopen("c:\\iso-dbus.txt", "a",stderr);*/
	iso_logger( "%s", "In - main\n" );
	//wait in a loop to accept requests from the client
	//Can mainloop be used instead of waiting in a loop?
	err = message_send_recv();
	if ( err != 0 ) 
		{
		printf("exit err code is %d\n", err );	
		}
	iso_logger( "%s", "Out - main\n" );
	
	//fclose(fp);
	return 0;
	}
