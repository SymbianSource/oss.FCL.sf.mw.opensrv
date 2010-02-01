/*
* ============================================================================
*  Name        : isoconnectionmanager.h
*  Part of     : isolation server.
*  Version     : %version: 9 %
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
* \def To avoid multiple inclusion of header
*/

#ifndef __ICONNECTIONMANAGER_H__
#define __ICONNECTIONMANAGER_H__

#include <glib.h>
#include <gtypes.h>
#include "isoservermain.h"
# include "msg_enums.h"

#ifdef __WINSCW__ 
/*!	\remark	Should LOGIN_ARG_COUNT this be hardcoded ?
*/
#define LOGIN_ARG_COUNT 12
#else
#define LOGIN_ARG_COUNT 10
#endif

/*!	\file
*	Connection related interfaces.
*	client anywhere in the documentation refers to anyone
*	using the isoserver
*/

/*! \typedef struct login_struct typedefed to login_struct 
*/
typedef struct login_struct login_struct;

/*!	\struct login_struct isoconnectionmanager.h
 *	\brief Buffer recieved from client for login request is parsed and the
 *	arguments are put into this structure
 *	\param hdr_req header request
 *	\param strs array of strings( which are arguments for login ) 
 */
struct login_struct
	{
	message_hdr_req* hdr_req;
	char *strs[LOGIN_ARG_COUNT];
	};

/*!	\brief This does things after login is successful. Now it sets the isConnected
 * 	variable to ETrue
 */
void action_logedin( ) ;

/*!	\brief This function is registered in action_login for statuschanged signal.
 * 	This function is called by telepathygabble when the status of the user changes
 *	Once user is connected this function registers for NewChannel signal with telepathygabble 
 *	client is also informed of the status change..
 *
 *	\param proxy
 * 	\param status status of the user ( connected/connecting/disconnected )
 * 	\param reason reason for status change(server disconnected/ user requested etc.,)
 * 	\param user_data  message header request
 *
 * 	\return : Boolean True on success, false on failure
 */

static gboolean status_changed_cb(DBusGProxy *proxy,
				  guint status, guint reason,
				  gpointer user_data) ;
				  
/*!	\brief new_channel_handler is registered as a callback for any new 
 *	channels craetion request, in function status_changed_cb after status is changed 
 *	to log in. new_channel_handler creates channel and also registers for the signals
 *	MembersChanged for contact related channels and Recieved and SendError for IM
 *	related channels.
 *
 * 	\param proxy unused
 * 	\param object_path object path of the channel
 * 	\param channel_type used to get a new channel
 * 	\param handle_type used to get a new channel, channel handle type
 * 	\param handle used to get a new channel, channel handle 
 * 	\param suppress_handler unused
 * 	\param user_data unused
 *
 * 	\return void
 */

static void new_channel_handler( DBusGProxy *proxy, const char *object_path,
				const char *channel_type, guint handle_type,
				guint handle, gboolean suppress_handler,
				gpointer user_data ) ;
				
				
/*!	\brief Is called after logout.. Any cleanup operations to
 *	 be performed here. Setting conn_stat to disconnected, cleaning up 
 *	of resources is done
 */
void action_logedout() ;

/*! \brief Callback for the logout 
 * 
 * 	\param proxy : unused
 * 	\param error : unused
 * 	\param userdata : unused
 * 	\return : void
 */
void logout_cb( DBusGProxy *proxy, GError *error, gpointer userdata );

/*!	\brief requests for closing of session. Also closes the search channel.
 *	\remark should not close the search channel
 * 
 * 	Request for log - out
 * 	\param pc - message  which has the request header
 * 	\return : MEM_ALLOCATION_ERROR or 0
 */
int action_logout( char* pc ) ;

/*!	\brief validates params, gets a dbus, creates a connection manager,
 *	and connection. Registers for the statuschange
 * 
 * 	\param ls_hdr login struct
 * 	\return Errors if invalid param or getting dbus, conn mgr or conn fails else 0
 */
gint action_login( login_struct* ls_hdr ) ;

/*!	\brief parses the message buffer and validates the parameters. The values are written
 *	to ls_hdr
 *
 *	\param aMsgBuffer - buf to be parsed
 * 	\param ls_hdr - login_struct struct to which values are written 
 * 	\param msg_len - no. of bytes in the message buffer aMsgBuffer
 *
 */
gint parse_for_login( gchar* aMsgBuffer, login_struct** ls_hdr, int msg_len ) ;

/*!	\brief Calls parse_for_login to parse and validate the arguments
 *	And then calls action_login for to do actual login
 *
 * 	\param aMsgBuf message buffer
 * 	\param msg_len - len of the message
 */
gint action_parse_login( char* aMsgBuf, int msg_len ) ;



#endif //__ICONNECTIONMANAGER_H__	
					    