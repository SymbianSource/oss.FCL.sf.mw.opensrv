
/* ============================================================================
*  Name        : isomodifycontacts.h
*  Part of     : isolation server.
*  Version     : %version: 7 %
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
#ifndef __ISOADDCONTACTS_H__
#define __ISOADDCONTACTS_H__

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

/*! /file 
 *	Interfaces for the maintainence of the contact list. 
 *	client anywhere in the documentation refers to anyone
 *	using the isoserver
 */

/*!
 *  /brief This function is called by message_send_recv function in isoservermain.c if message type
 *  is EReject_Contact_Request, EAccept_Contact_Request, EAdd_Contact_Request and EDelete_Contact_Request. 
 *  Contacts added/removed to/from list. This function parses the message buffer, 
 *  validates for the arguments and calls modify_contact_list to add/remove contacts
 * 
 *  /param buf : buffer to be parsed
 *  /param buf_len : buffer length
 *  /return : error code if any, 0 on success 
 */
gint action_parse_modify_contact_list( gchar* buf, gint buf_len ) ;

/*!
 *  /brief gets the handles for contacts. Handles are returned in request_modify_contact_list_handles_cb 
 *  callback which is an arg for the async request to get handles.
 *  
 *  /param msg_hdr : The header and message passed to callbacks
 *  /param contact_id : contact ids whose handles to be returned
 *  /param no_cntcts : total no. of contacts to be added
 *  /return : error code if any, handle on success 
 */
guint get_handles_for_contacts ( send_msg_struct* msg_hdr, 
	const gchar **contact_id, gint no_cntcts );
	
/*!
 *  /brief Handles are returned in this callback. Which will then actually do modify 
 *	the contact list based on the request type(add/remove/accept/reject)
 *	modify_contact_list_cb is registered as the callback.
 *
 *  /param proxy : unused
 *  /param handles : handles of the contacts
 *  /param error : error if any
 *  /param message : userdata(send_msg_struct) passed back
 *  /return : error code if any, handle on success 
 */		
void request_modify_contact_list_handles_cb( DBusGProxy *proxy, GArray *handles, 
		GError* error, gpointer message );	

/*!
 *  /brief Calls get_handles_for_contacts to get the handles for contact. 
 * 
 *  /param msg_hdr : message request header passed to callbacks
 *  /param contact_id : contact ids to be added
 *  /param message : messgae if any
 *  /param no_cntcts : no of contacts to be added
 *  /return void 
 */
gint modify_contact_list ( send_msg_struct* msg_hdr, const gchar  **contact_ids, 
				gint no_cntcts );

/*!
 *  /brief called as callback for request for modify contact list 
 *	This function then sends the result to client
 * 
 *  /param proxy unused 
 *  /param error Error if any
 *  /param userdata request message header
 *  /return void 
 */
void modify_contact_list_cb( DBusGProxy *proxy, GError *error, gpointer userdata );

/*!
 *  /brief The contact names corresponding to the contact handles that are changed 
 *	are passed to the client
 *  /param proxy 
 * 	/param handles_name
 * 	/param error
 * 	/param userdata
 * 	/return void 
 */
static void rosters_changed_contacts_cb( DBusGProxy *proxy,
	gchar **handles_names, GError *error, gpointer userdata );
				 
				 
/*!	/brief Whenever contact list is changed by others( someone trying to add this user
 *	or someone rejecting the add request etc., ) this callback registered as a signal
 *	is called with the handles of the changed contacts. rosters_changed_contacts_cb is
 *	passed as callback to get the contact name of the changed handles
 * 
 * /param group_iface unused 
 * /param message unused
 * /param added List of contacts that are recently added
 * /param removed List of contacts that were recently removed
 * /param local_pending List of contacts that are locally pending for approval
 * /param remote_pending List of contacts that are remotely pending for approval
 * /param actor unused
 * /param reason unused
 * /param userdata unused
 * /return void
 */
void roster_members_changed_cb ( DBusGProxy           *group_iface,
									    gchar                *message,
									    GArray               *added,
									    GArray               *removed,
									    GArray               *local_pending,
									    GArray               *remote_pending,
									    guint                 actor,
									    guint                 reason,
									    gpointer  			  userdata );				


		
#endif //__ISOADDCONTACTS_H__	