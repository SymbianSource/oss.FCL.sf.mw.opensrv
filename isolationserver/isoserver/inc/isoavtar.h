/*
* ============================================================================
*  Name        : isoconnectionmanager.h
*  Part of     : isolation server.
*  Version     : %version: 5 %
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

#ifndef __ISOAVTAR_H__
#define __ISOAVTAR_H__

#include <glib.h>
#include <gtypes.h>

#include "isoservermain.h"
#include "msg_enums.h"

#include "tp-conn.h"
#include "tp-connmgr.h"
#include "tp-props-iface.h"
#include "tp-conn-iface-avatars-gen.h"
#include "tp-interfaces.h"
G_BEGIN_DECLS

/*! \file
*	This file has interfaces for avtar implimentation. 
*	client anywhere in the documentation refers to anyone
*	using the isoserver
*/


/*!	\brief callback for updating own avtar. This function 
 *	sends response to client
 *
 * 	\param proxy : unused
 *  \param avatar_sha1: token string 
 * 	\param error : error if any
 * 	\param message : message header  
 * 	\return : void
 */
void  update_avtar_cb(DBusGProxy *proxy, char* avatar_sha1,
			GError* error, 
			gpointer message);


/*!	\brief sends avtar of the client to n/w server thru' gabble
 * 	calls tp_conn_iface_avatars_set_avatar_async to set avatar
 *  
 *  \param avtarcontent : avatar image content
 *  \param mimetype : mime type
 * 	\param msghdr request header that will be passed back to client
 *
 * 	\return : error code on failure, 0 on success
 */
gint send_avtar( GArray* avtarcontent , char * mimetype , message_hdr_req *msg_hdr );

/*!	\brief callback for clearing own avtar. This function 
 *	sends response to client
 *
 * 	\param proxy : unused
 * 	\param error : error if any
 * 	\param userdata : message header  
 * 	\return : void
 */
void clear_avtar_cb( DBusGProxy *proxy, GError* error, gpointer userdata );

/*! \brief clears the self avtar 
 *
 *  \param msghdr request header that will be passed back to client
 *
 *  \return : error code on failure, 0 on success
 */
gint clear_avatar( message_hdr_req *msg_hdr );

/*! \brief clears the self avtar 
 *  \param rmsg : message buffer to be parsed
 *  \param msg_struct : request header 
 *  \param result : message buffer length
 *  \param avtarcontent : pointer to the avatar image data
 * 
 *  \return : error code on failure, 0 on success
 */
gint update_own_avatar(char* rmsg,message_hdr_resp* msg_struct,gint result, GArray** avtarcontent);

G_END_DECLS


#endif //__ISOAVTAR_H__
