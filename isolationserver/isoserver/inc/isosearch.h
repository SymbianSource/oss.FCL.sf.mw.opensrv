/*
* ============================================================================
*  Name        : isosearch.h
*  Part of     : isolation server.
*  Version     : %version: 8 %
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
#ifndef __ISOSEARCH_H__
#define __ISOSEARCH_H__

/*! /file
*	Header file for search feature implimentation. The interfaces mentioned are 
*	the only those which are used(extern) by functions in other files. Functions which 
*	are locally used in isosearch.c are not added here. And those functions are forward 
*	declared.
*	client anywhere in the documentation refers to anyone
*	using the isoserver
*/

/*
! /This header has definitions for glib types
*/
#include <gtypes.h>

/*
! /This header has definitions for DBusGProxy
*/
#include <dbus/dbus-glib.h>


/*
! /brief This function is called by message_send_recv function in isoservermain.c if message type
*	is ESearch. This function parses the rmsg, validates the parameter passed, if parameters are 
*	correct a search performed for them. else INVALID_PARAMETERES error is returned
*
* 	/param rmsg message buffer to be parsed
*	/param rmsg_len the length of the rmsg
*	/remark rmsg_len is not strlen(rmsg)
*	/return returns error code or 0 on success
*/
gint action_parse_search( gchar* rmsg, gint rmsg_len );



/*
!	/brief This function is called as a callback when a search channel is closed
*	/remark This function should have been a static function in isosearch.c
*	A work around has been done to make multiple search requests.
*
* 	/param proxy unused variable
*	/param error Error if any
*	/param user_data callback data
*	/return void
*/
void search_chan_closed_cb ( DBusGProxy	*proxy,
									GError *error,
									gpointer	user_data
								  );

#endif //__ISOSEARCH_H__
