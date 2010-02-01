/*
* ============================================================================
*  Name        : isoutils.c
*  Part of     : isolation server.
*  Version     : %version: 15 %
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "isoutils.h"
#include "msgliterals.h"
#include "isoservermain.h"

/*!	\file 
*	Impliments the functions in isoutils.h
*/

/*! \def Thread stack size
*/
#define LOG_FILENAME "c:\\isoserver.txt"

/*! \brief Documented in the header file
*/
void  iso_logger ( gchar* fmt, ... )
	{
	#ifdef _DEBUG
	
	FILE* fp;
	//open file to read..
	fp = fopen( LOG_FILENAME,"a" ); 
	if ( fp ) 
	    {
	    int ival;
	    double dval;
	    char* sval;
	    char* p;
	    va_list list;
	    //initalize the variable arg to fmt..
		va_start( list, fmt );
		//Go char by char thru' format string
		//example format string ("Hi, by %s", str)
		for ( p = fmt; *p; p++ ) 
			{
			if ( *p != '%' )
				{
				//Not format specifiers just print them to file
				//In example above printe "Hi, by " to file
				putc( *p, fp  );
				continue;	
				}
			switch( *++p ) 
				{
				case 'd' : 
					//Next arg got is int
					//read integer and print to file
					ival = va_arg( list, int );
					fprintf( fp, "%d", ival );
					break;
				
				case 'f' : 
					//Next arg got is float
					//read float and print to file
					dval = va_arg( list, double );
					fprintf( fp, "%f", dval, fp );
					break;
				
				case 's' :
					//Next arg got is string
					//read string and go char by char and print to file
					for ( sval = va_arg( list, char * ); *sval; sval++ ) 
			   			{
			   			putc( *sval, fp );
			   			}
			   			break;
			   	case 'u' : 
					//Next arg got is int
					//read integer and print to file
					ival = va_arg( list, uint );
					fprintf( fp, "%u", ival );
					break;
				default:
					break;
				}
			}
		//print a new line
		fprintf(fp,"\n");
		//close file
		fclose(fp);
		//close the variable arg list
		va_end( list );
	    }
	#endif //_DEBUG
	}

/*! \brief Documented in the header file
*/
gint parse_a_string( gchar* msg_buf, gchar** str, gint* len, gint msg_len ) 
	{
	//parsing the buffer into an array of strings(contacts)
	gint userlen = 0;
	gchar* users = NULL;
	
	while( '\0' != *( msg_buf + *len ) && *len < msg_len ) 
		{
		//len is different from userlen
		//len is total no. of characters read in msg
		//where as userlen is no. of char read for this contact
		userlen++;
		(*len)++;
		}
		
	//the string is not present
	//or,Reached the last byte of parse string and it is not '\0'
	if ( 0 == userlen || 
		( '\0' != *( msg_buf + *len ) && *len == msg_len ) )
		{
		//Got parse error..
		//Invalid message format..
		return INVALID_PARAMETERES;	
		}
	//Allocate memory and check for error
	users = ( gchar* ) malloc ( userlen + 1 ); 
	if ( NULL == users ) 
		{
		return MEM_ALLOCATION_ERROR;
		}
	memset( users, '\0', userlen + 1 );
	//copy one contact...
	memcpy( users, ( msg_buf + *len - userlen ), userlen + 1 );	
	*str = users;
	(*len)++;
	//On success return 0
	return 0;
	}

/*! \brief Documented in the header file
*/
gint parse_into_array_of_strings( gchar* msg_buf, gchar*** contact_ids, 
		gint* len, gint msg_len, gint* cntidscount ) 
	{
	gchar** cntids = NULL;
	gchar* users = NULL;
	gint err = 0;
	
	//to calc MAX_MSG_RECEIPIENTS dynamically one more loop needs to be done 
	//for this
	//Allocate memory and check for error returns
	cntids = ( gchar** ) malloc( MAX_MSG_RECEIPIENTS * sizeof( gchar* ) );
	if ( NULL == cntids ) 
		{
		return MEM_ALLOCATION_ERROR;
		}
	do
		{
		//get one string at a time..	
		err = parse_a_string( msg_buf, &users, len, msg_len );
		
		if ( err < 0 ) 
			{
			return err;
			}
		//check if max no of contacts are read..
		//if not, copy contact into 2D array of strings...
		//else ignore the extra contacts
		//do not over run the cntids , incase of 
		//no. of contacts more than max msg recp. @ line 197
		//cntids[*cntidscount] = NULL;
		if ( *cntidscount < MAX_MSG_RECEIPIENTS - 1 ) 
			{
			cntids[*cntidscount] = users;	
			( *cntidscount )++;
			}
		else 
			{
			//users to be freed..ignoring extra no. of recipients
			free ( users );
			}
		
		//increment the no. of contacts..
		
		//all contacts read break the loop
		//
		if ( '\0' == *( msg_buf + *len ) )
			{
			break;
			}
		//If maximum no. of receipients is reached break 
		//so here of total no. of contacts, message is only sent to
		//first MAX_MSG_RECEIPIENTS - 1
		} while( *len < msg_len );
	cntids[*cntidscount] = NULL;
	
	/*
	//Did not find any use of this as of now..
	//But in case of any mem issues can be looked into
	if ( *cntidscount < MAX_MSG_RECEIPIENTS ) 
		{
		//realloc is debatable ...
		cntids = ( char** ) realloc( cntids, ( *cntidscount + 1 ) * sizeof( char* ) );
		}*/
	//Assign to contact ids
	*contact_ids = cntids;
	(*len)++; //skip the last '\0' ... 2D arrays end with '\0'
	return 0;	
	}

/*! \brief Documented in the header file
*/
void free_msg_args( gchar** contactid, gint contacts_count, gchar* sendmsg ) 
	{
	gint i = 0;
	//free all the contacts
	if ( NULL != contactid ) 
		{
		for ( i = 0; i < contacts_count; i++ ) 
			{
			if ( NULL != (contactid)[i] )	
				{
				free ( (contactid)[i] );
				}
			}
		}
	//free the message
	if ( NULL != sendmsg ) 
		{
		free ( sendmsg );
		}
	}

/*! \brief Documented in the header file
*/	
gint send_response_to_client( message_hdr_req* hdr_req, gint error, 
		gboolean response )
	{
	int pri = MSG_PRI_NORMAL;
	gint result = 0;
	gint err = 0;
	int timeout = NO_WAIT; 
	message_hdr_resp* msg_struct = NULL;
	
	result = MsgQCreate( RESPONSE_QUEUE, MAX_MSG_Q_SIZE, MSG_Q_FIFO, &err );
	if ( ERROR == result )
		{
		return ERROR;
		}
		
	msg_struct = ( message_hdr_resp* ) malloc ( sizeof( message_hdr_resp ) );
	if ( NULL == msg_struct ) 
		{
		return 	MEM_ALLOCATION_ERROR;
		}

	//intialize memory to 0
	memset( msg_struct, '\0', sizeof( message_hdr_resp ) );
	//Copy the header request into response header..
	//This req header in resp will be used to map the req to resp.
	msg_struct->hdr_req.message_type = hdr_req->message_type;
	msg_struct->hdr_req.protocol_id = hdr_req->protocol_id;
	msg_struct->hdr_req.session_id = hdr_req->session_id;
	msg_struct->hdr_req.request_id = hdr_req->request_id;
	
	//set the error and responses..
   	msg_struct->error_type = error;
	msg_struct->response = response;
	
	//send the message to the client			
	result = MsgQSend( RESPONSE_QUEUE, (void*)msg_struct, sizeof( message_hdr_resp ),
			 pri, timeout, &err );
			 	
	free ( msg_struct );
	
	if ( 0 > result ) 
		{
		return MSG_Q_SEND_FAILED;
		}
			
	return 0;
	
	}
