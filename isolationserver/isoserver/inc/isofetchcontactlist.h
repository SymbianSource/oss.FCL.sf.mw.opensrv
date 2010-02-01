
/* ============================================================================
*  Name        : isofetchcontactlist.h
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
#ifndef __IFETCHCONTACTLIST_H__
#define __IFETCHCONTACTLIST_H__

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

#include "isoim.h"

/*!	\file
*	Interfaces for fetching of contacts.
*	client anywhere in the documentation refers to anyone
*	using the isoserver
*/

/*! \enum channel_type
 *	type of the channel
 */
enum channel_type
	{
	ESubscribe_Channel,
	EPublish_Channel	
	};

/*!	\brief This function is called as a callback to get the contact names from 
 * 	contact handles. Contact names are sent to client in multiple packets of
 * 	size MAX_MSG_SIZE
 *
 * 	\param proxy unused
 * 	\param handles_names 2D array of contact names
 * 	\param error error if any 
 * 	\param userdata msg_type
 */
static void inspect_handles_cb( DBusGProxy *proxy,char **handles_names, 
			GError *error, gpointer userdata ) ;


/*!	\brief This function is called if there are 0 contacts or there was an error
 *
 * 	\param contact_type : Type of the contacts
 * 	\param err : err if any, 0 otherwise
 */
static gint send_fetch_contacts_error( gint contact_type , gint err );

/*!	\brief This function is a callback for request for the contacts 
 *	in local pending contatct list from server. This is 
 *	only called for publish channel members. fetch_contacts is called 
 *	to get the contact names from the handles. If there are no conatcts in 
 *	the list it calls send_fetch_contacts_error else requests for the contact names
 *	from the contact handles
 *
 * 	\param proxy unused 
 * 	\param current_members array of handles of current contatcs
 * 	\param local_pending_members unused 
 * 	\param remote_pending_members array of handles of remote pending contatcs
 * 	\param error error if any
 * 	\param userdata unused 
 */
static void get_roster_local_member_cb( DBusGProxy *proxy,GArray* current_members, 
			GArray* local_pending_members, GArray* remote_pending_members, 
			GError *error, gpointer userdata );
			
/*!	\brief Requests the server to get contact names from the handles
 *	The contact names are returned in inspect_handles_cb. From there
 *	contacts are sent to client
 *
 * 	\param contact_mem : handles to contact members
 * 	\param msg_type : type of the message, which wil be passed to cb
 * 	\return  Errors if any, else 0
 */
			
static gint fetch_contacts( GArray* contact_mem, gint* msg_type ) ;			

/*!	\brief This function is a callback for request for the contacts 
 *	in msg_type(subscribe or publish) contatct list from server. This is 
 *	only called for subscribe channel members. fetch_contacts is called 
 *	to get the contact names from the handles. If there are no conatcts in 
 *	the list it calls send_fetch_contacts_error else requests for the contacts 
 *	from the handles
 *
 * 	\param proxy unused 
 * 	\param current_members array of handles of current contatcs
 * 	\param local_pending_members unused 
 * 	\param remote_pending_members array of handles of remote pending contatcs
 * 	\param error error if any
 * 	\param userdata unused 
 */
static void get_roster_member_cb( DBusGProxy *proxy,GArray* current_members,
		 GArray* local_pending_members, GArray* remote_pending_members, 
		 	GError *error, gpointer userdata ) ;
		 	

/*!	\brief this function requests for the contacts in its contatct list from server
 *	
 *	\param	type of the contacts channel( publish, subscribe )
 */
void request_roster( enum channel_type type );

/*! \brief Sends contacts to the client. Contact names are sent to client in multiple 
 * 	packets of size less than or equal to MAX_MSG_SIZE. Contact will be sent as a whole. 
 * 	A single contact will not be sent in two consecutive packets
 *
 * 	\param members : Contacts to be sent to client
 * 	\param mem_count : no. of contacts 
 * 	\param contact_type : type of the contact
 * 	\return error code..
 */
gint send_contacts_to_client( gchar** members, gint mem_count, gint contact_type ) ;


/*! \brief Fetches the cached contacts stored in telepathy gabble
 *   This is as good as fetching the contacts
 */ 
void fetch_cached_contacts();

#endif //__IFETCHCONTACTLIST_H__