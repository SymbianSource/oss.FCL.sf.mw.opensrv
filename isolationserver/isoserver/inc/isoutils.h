
/* ============================================================================
*  Name        : isoutils.h
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
#ifndef __ISOUTILS_H__
#define __ISOUTILS_H__

/*
*! /file 
*	Header file for the isolation utilities. 
*	client anywhere in the documentation refers to anyone
*	using the isoserver
*/
#include "msg_enums.h"


/*
 * ! /brief Function for to log the messages. Ultimately uses fprintf and putc to 
 *  write to file. This function takes variable no of arguments.  
 *  
 *  /param fmt format specifier.
 *  /return void  
 */
void  iso_logger ( gchar* fmt, ... );

/*
*! /brief parses a buffer into an array of strings. Message buffer  
*   test.ximp@gmail.com/0ximp.telepathy@gmail.com/0/0
*                       ^                         ^ 
*   contact1------------|contact2-----------------|
*   str will have contact1 for 1st time, contact2 on 2nd call
* 
*   /param message buffer : Buffer to be parsed
*   /param str : str will have pointer to a string
*   /param len : Len returns the number of bytes read
*   /param msg_len : no. of bytes in msg_buf
*   /return gint : error if any on failure, else no. of strings read from buffer
*/
gint parse_a_string( gchar* msg_buf, gchar** str, gint* len, gint msg_len );

/*
*!
*   /brief parses a buffer into an array of strings
* 
*   /param message buffer : Buffer to be parsed
*   /param arr_str : Parsed strings are returned in this array of strings
*   /param len : Len returns the number of bytes read
*   /param len : msg_len no. of bytes in msg_buf
*   /param cntidscount : no. of contacts
*   /return gint : error if any on failure, else no. of strings read from buffer
*   example 
*	test.ximp@gmail.com/0ximp.telepathy@gmail.com/0/0
*						  ^ 						^ ^	            ^ 
*	contact1------------|contact2-----------------|-|
*	is parsed into
*	arr_str[0] = test.ximp@gmail.com/0
*	arr_str[1] = ximp.telepathy@gmail.com/0
*/
gint parse_into_array_of_strings( gchar* msg_buf, 
				gchar*** arr_str, gint* len, gint msg_len,
				gint* cntidscount ) ;

/*
 *! /brief Frees the memory allocated to the array of string and to the message
 * 
 *  /param contactid : array of string to be freed.
 *  /param contacts_count : no of contacts
 *  /param sendmsg : msg to be freed
 */	
void free_msg_args( gchar** contactid, gint contacts_count, gchar* sendmsg );


/*
 *! /brief Send the response for the porcessing of request to the client
 * 
 *  /param hdr_req : message request header
 *  /param error : error code
 *  /param response : whether the processing of request successful
 *
 */
gint send_response_to_client( message_hdr_req* hdr_req, gint error, 
		gboolean response );
 
#endif //__ISOUTILS_H__

